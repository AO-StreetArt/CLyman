/*
Apache2 License Notice
Copyright 2017 Alex Barry

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// This sets up all of the components necessary for the service and runs the
// main loop for the application.

#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <exception>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#include "aossl/mongo/include/mongo_buffer_interface.h"

#include "aossl/commandline/include/commandline_interface.h"
#include "aossl/commandline/include/factory_cli.h"

#include "aossl/logging/include/logging_interface.h"
#include "aossl/logging/include/factory_logging.h"

#include "aossl/redis/include/redis_interface.h"
#include "aossl/redis/include/factory_redis.h"

#include "aossl/uuid/include/uuid_interface.h"
#include "aossl/uuid/include/factory_uuid.h"

#include "aossl/zmq/include/zmq_interface.h"
#include "aossl/zmq/include/factory_zmq.h"

#include "src/app/include/app_log.h"
#include "src/app/include/app_utils.h"
#include "src/app/include/configuration_manager.h"
#include "src/app/include/globals.h"
#include "src/app/include/redis_locking.h"
#include "src/app/include/query_helper.h"
#include "src/app/include/kafka_client.h"

#include "src/api/include/object_list_interface.h"
#include "src/api/include/object_list_factory.h"

// Catch a Signal (for example, keyboard interrupt)
void my_signal_handler(int s) {
  main_logging->error("Caught signal");
  std::string signal_type = std::to_string(s);
  main_logging->error(signal_type);
  shutdown();
  exit(1);
}

// Main Method
int main(int argc, char** argv) {
  // Set up a handler for any signals so that we always shutdown gracefully
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = my_signal_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

  // Setup the component factories
  cli_factory = new CommandLineInterpreterFactory;
  redis_factory = new RedisComponentFactory;
  uuid_factory = new uuidComponentFactory;
  zmq_factory = new ZmqComponentFactory;
  logging_factory = new LoggingComponentFactory;
  mongo_factory = new MongoComponentFactory;

  ObjectListFactory ofactory;
  ObjectFactory objfactory;

  // Set up our command line interpreter
  cli = cli_factory->get_command_line_interface(argc, argv);

  // Allow for wait on startup, if configured
  if (cli->opt_exist("-wait")) {
    std::string wait_time_string = cli->get_opt("-wait");
    int wait_time = std::stoi(wait_time_string, NULL);
    // Accept input on the command line in seconds, convert to microseconds
    usleep(wait_time * 1000000);
  }

  // Set up logging
  std::string initFileName;

  // See if we have a command line setting for the log file
  const char * env_logging_file = std::getenv("CRAZYIVAN_LOGGING_CONF");
  if (env_logging_file) {
    std::string tempFileName(env_logging_file);
    initFileName = tempFileName;
  } else if (cli->opt_exist("-log-conf")) {
    initFileName = cli->get_opt("-log-conf");
  } else {
    initFileName = "log4cpp.properties";
  }

  // This reads the logging configuration file
  logging = logging_factory->get_logging_interface(initFileName);

  // Set up the logging submodules for each category
  start_logging_submodules();

  // Set up the UUID Generator
  uid = uuid_factory->get_uuid_interface();

  std::string service_id = "Clyman";

  // Set up our configuration manager with the CLI
  config = new ConfigurationManager(cli, service_id);

  // Start up the Kafka Producer
  kafka = new KafkaClient();

  // The configuration manager will  look at any command line arguments,
  // configuration files, and Consul connections to try and determine the
  // correct configuration for the service

  // The configuration manager will  look at any command line arguments,
  // configuration files, and Consul connections to try and determine the
  // correct configuration for the service
  bool config_success = false;
  bool config_tried = false;
  int config_attempts = 0;
  // If we fail configuration, we should sleep for 5 seconds and try again
  while (!config_success) {
    if (config_attempts > 50) {
      main_logging->error("Max Config Attempts failed, exiting");
      shutdown();
      exit(1);
    }
    if (config_tried) {
      main_logging->error("Configuration Failed, trying again in 5 seconds");
      usleep(5000000);
    } else {
      config_tried = true;
    }
    try {
      config_success = config->configure();
    }
    catch (std::exception& e) {
      main_logging->error("Exception encountered during Configuration");
    }
  }

  // Set up our Redis Connection List, which is passed
  // to the Redis Admin to connect
  std::vector<RedisConnChain> RedisConnectionList = \
    config->get_redisconnlist();
  // Set up Redis Connection
  if (RedisConnectionList.size() > 0) {
    try {
      // Currently only support for single Redis instance
      red = redis_factory->get_redis_interface(RedisConnectionList[0].ip, \
        RedisConnectionList[0].port);
    }
    catch (std::exception& e) {
      main_logging->error("Exception encountered during Redis Startup");
      main_logging->error(e.what());
      shutdown();
      exit(1);
    }
    main_logging->info("Connected to Redis");
  } else {
    main_logging->error("No Redis Connections found in configuration");
  }

  // Set up the Mongo Connection
  std::string DBConnStr = config->get_mongoconnstr();
  std::string DBName = config->get_dbname();
  std::string DBHeaderCollection = config->get_dbheadercollection();
  if (!(DBConnStr.empty() || DBName.empty() \
    || DBHeaderCollection.empty())) {
    try {
      mongo = mongo_factory->get_mongo_interface(DBConnStr, \
        DBName, DBHeaderCollection);
      main_logging->debug("Connected to Mongo");
    }
    catch (std::exception& e) {
      main_logging->error("Exception encountered during Mongo Startup");
      main_logging->error(e.what());
      shutdown();
      exit(1);
    }
  } else {
    main_logging->error("Insufficient Mongo Connection Information");
    shutdown();
    exit(1);
  }

  // Connect to the inbound ZMQ Admin
  std::string ib_zmq_connstr = config->get_ibconnstr();
  if (!(ib_zmq_connstr.empty())) {
    zmqi = zmq_factory->get_zmq_inbound_interface(ib_zmq_connstr, REQ_RESP);
    main_logging->info("ZMQ Socket Open, opening request loop");
  } else {
    main_logging->error("No IB ZMQ Connection String Supplied");
    shutdown();
    exit(1);
  }

  // Set up the Redis Lock Helper
  RedisLocker lock(red);

  // Main Request Loop

  while (true) {
    std::string resp_str = "";
    rapidjson::Document d;
    protoObj3::Obj3List new_proto;

    // Convert the OMQ message into a string to be passed on the event
    char * req_ptr = zmqi->crecv();
    if (!req_ptr) continue;
    main_logging->debug("Conversion to C String performed with result: ");
    main_logging->debug(req_ptr);

    // Trim the string recieved
    std::string recvd_msg(req_ptr);
    std::string clean_string;

    std::string new_error_message = "";

    // Parsing logic - JSON
    if (config->get_formattype() == JSON_FORMAT) {
      response_message = ofactory.build_json_object_list();
      int final_closing_char = recvd_msg.find_last_of("}");
      int first_opening_char = recvd_msg.find_first_of("{");
      clean_string = \
        recvd_msg.substr(first_opening_char, final_closing_char+1);
      main_logging->debug("Input String Cleaned");
      main_logging->debug(clean_string);

      try {
        d.Parse<rapidjson::kParseStopWhenDoneFlag>(clean_string.c_str());
        if (d.HasParseError()) {
          main_logging->error("Parsing Error: ");
          main_logging->error(GetParseError_En(d.GetParseError()));
          response_message->set_error_code(TRANSLATION_ERROR);
          new_error_message.assign(GetParseError_En(d.GetParseError()));
        } else {inbound_message = ofactory.build_object_list(d);}
      }
      // Catch a possible error and write to logs
      catch (std::exception& e) {
        main_logging->error("Exception occurred while parsing document:");
        main_logging->error(e.what());
      }
    // Parsing logic - Protocol Buffers
    } else if (config->get_formattype() == PROTO_FORMAT) {
      response_message = ofactory.build_proto_object_list();
      clean_string = trim(recvd_msg);
      main_logging->debug("Input String Cleaned");
      main_logging->debug(clean_string);

      try {
        new_proto.ParseFromString(clean_string);
        inbound_message = ofactory.build_object_list(new_proto);
      } catch (std::exception& e) {
        main_logging->error("Exception occurred while parsing bytes:");
        main_logging->error(e.what());
        response_message->set_error_code(TRANSLATION_ERROR);
        new_error_message.assign(e.what());
      }
    }

      // Determine the Transaction ID
      UuidContainer id_container;
      id_container.id = "";
      if (config->get_transactionidsactive() && inbound_message) {
        // Get an existing transaction ID
        std::string existing_trans_id = \
          inbound_message->get_transaction_id();
        // If no transaction ID is sent in, generate a new one
        if (existing_trans_id.empty()) {
          try {
            id_container = uid->generate();
            if (!id_container.err.empty()) {
              main_logging->error(id_container.err);
            }
            inbound_message->set_transaction_id(id_container.id);
          }
          catch (std::exception& e) {
            main_logging->error("Exception encountered in UUID Generation");
            main_logging->error(e.what());
            shutdown();
            exit(1);
          }
        }
        main_logging->debug("Transaction ID: ");
        main_logging->debug(inbound_message->get_transaction_id());
      }

      bool shutdown_needed = false;

      // Core application logic
      if (inbound_message) {
        try {
          // Object Creation
          if (inbound_message->get_msg_type() == OBJ_CRT) {
            main_logging->info("Processing Object Creation Message");
            for (int i = 0; i < inbound_message->num_objects(); i++) {
              // Create a new Obj3 to add to the return list
              ObjectInterface *resp_element = objfactory.build_object();

              // Create the Obj3 document for Mongo
              AOSSL::MongoBufferInterface *bson = mongo_factory->get_mongo_buffer();
              inbound_message->get_object(i)->to_bson(bson);
              MongoResponseInterface *resp = mongo->create_document(bson);
              delete bson;

              // Add the key into the new obj3
              resp_element->set_key(resp->get_value());
              // Add the new obj3 to the response list
              response_message->add_object(resp_element);
              delete resp;
            }

          // Object Update
          } else if (inbound_message->get_msg_type() == OBJ_UPD || \
            inbound_message->get_msg_type() == OBJ_LOCK || \
            inbound_message->get_msg_type() == OBJ_UNLOCK ||
            inbound_message->get_msg_type() == OBJ_OVERWRITE) {
            main_logging->info("Processing Object Update Message");
            for (int i = 0; i < inbound_message->num_objects(); i++) {
              // Enforce atomic updates -- establish redis lock
              bool lock_obtained = true;
              if (config->get_atomictransactions()) {
                lock_obtained = \
                  lock.get_lock(inbound_message->get_object(i)->get_key());
              }
              // Enforce Object Locking -- establish redis lock
              if (lock_obtained && config->get_locking_active() && inbound_message->get_msg_type() == OBJ_LOCK) {
                std::string lock_key = "ObjectLock-";
                lock_key = lock_key + inbound_message->get_object(i)->get_key();
                lock_obtained = \
                  lock.get_lock(lock_key, \
                  inbound_message->get_object(i)->get_owner());
              }
              if (lock_obtained) {
                if (inbound_message->get_msg_type() == OBJ_OVERWRITE || \
                  inbound_message->get_msg_type() == OBJ_UPD) {
                    // Send an update on the Kafka 'dvs' topic
                    kafka->send(inbound_message->get_object(i)->to_transform_json(), config->get_kafkabroker());
                }
                if (inbound_message->get_msg_type() == OBJ_OVERWRITE) {
                  AOSSL::MongoBufferInterface *bson = mongo_factory->get_mongo_buffer();
                  inbound_message->get_object(i)->to_bson_update(bson);
                  const char *msg_key = inbound_message->get_object(i)->get_key().c_str();
                  mongo->save_document(bson, msg_key);
                  delete bson;
                } else {
                  // Load the current doc from the database
                  rapidjson::Document resp_doc;
                  MongoResponseInterface *resp = mongo->load_document(\
                    inbound_message->get_object(i)->get_key());
                  if (resp) {
                    std::string mongo_resp_str = resp->get_value();
                    main_logging->debug("Document loaded from Mongo");
                    main_logging->debug(mongo_resp_str);
                    resp_doc.Parse(mongo_resp_str.c_str());
                    ObjectInterface *resp_obj = objfactory.build_object(resp_doc);
                    // Apply the object message as changes to the DB Object
                    if (inbound_message->get_msg_type() == OBJ_OVERWRITE) {
                      resp_obj->overwrite(inbound_message->get_object(i));
                    } else {
                      // overwrite the changes on the DB object
                      resp_obj->merge(inbound_message->get_object(i));
                    }
                    // Save the resulting object
                    AOSSL::MongoBufferInterface *bson = mongo_factory->get_mongo_buffer();
                    resp_obj->to_bson_update(bson);
                    const char *msg_key = resp_obj->get_key().c_str();
                    mongo->save_document(bson, msg_key);
                    response_message->add_object(resp_obj);
                    delete bson;
                    delete resp;
                  } else {
                    main_logging->error("Document not found in Mongo");
                    response_message->set_error_code(NOT_FOUND);
                    new_error_message = "Object not Found";
                    response_message->set_error_message(new_error_message);
                  }
                }
                // Enforce atomic updates -- release redis lock
                if (config->get_atomictransactions()) {
                  if (!(lock.release_lock(\
                    inbound_message->get_object(i)->get_key()))) \
                    {main_logging->error("Failed to release Lock");}
                }
                // Enforce Object Locking -- release redis lock
                if (config->get_locking_active() && inbound_message->get_msg_type() == OBJ_UNLOCK) {
                  std::string lock_key = "ObjectLock-";
                  lock_key = lock_key + inbound_message->get_object(i)->get_key();
                  if (!(lock.release_lock(lock_key, \
                    inbound_message->get_object(i)->get_owner()))) \
                    {main_logging->error("Failed to release Lock");}
                }
              } else {
                main_logging->error("Object Lock Encountered");
                response_message->set_error_code(LOCK_EXISTS_ERROR);
                new_error_message = "Existing Lock Encountered";
                response_message->set_error_message(new_error_message);
              }
            }

          // Object Retrieve
          } else if (inbound_message->get_msg_type() == OBJ_GET) {
            main_logging->info("Processing Object Get Message");
            for (int i = 0; i < inbound_message->num_objects(); i++) {
              // Pull down and parse the value from Mongo
              rapidjson::Document resp_doc;
              MongoResponseInterface *resp = mongo->load_document(\
                inbound_message->get_object(i)->get_key());
              if (resp) {
                std::string mongo_resp_str = resp->get_value();
                main_logging->debug("Document loaded from Mongo");
                main_logging->debug(mongo_resp_str);
                resp_doc.Parse(mongo_resp_str.c_str());

                // Create a new Obj3 and add to the return list
                ObjectInterface *resp_obj = objfactory.build_object(resp_doc);
                response_message->add_object(resp_obj);
                delete resp;
              } else {
                main_logging->error("Document not found in Mongo");
                response_message->set_error_code(NOT_FOUND);
                new_error_message = "Object not Found";
                response_message->set_error_message(new_error_message);
              }
            }

          // Object Query
          } else if (inbound_message->get_msg_type() == OBJ_QUERY) {
            main_logging->info("Processing Object Batch Query Message");
            batch_query(inbound_message, response_message, mongo);

          // Object Delete
          } else if (inbound_message->get_msg_type() == OBJ_DEL) {
            main_logging->info("Processing Object Deletion Message");
            for (int i = 0; i < inbound_message->num_objects(); i++) {
              ObjectInterface *resp_element = objfactory.build_object();
              resp_element->set_key(\
                inbound_message->get_object(i)->get_key());
              mongo->delete_document(\
                inbound_message->get_object(i)->get_key());
              response_message->add_object(resp_element);
            }

          // Ping
          } else if (inbound_message->get_msg_type() == PING) {
            main_logging->info("Ping Message Recieved");

          // Kill
          } else if (inbound_message->get_msg_type() == KILL) {
            main_logging->info("Shutting Down");
            shutdown_needed = true;

          // Invalid Message Type
          } else {
            response_message->set_error_code(BAD_MSG_TYPE_ERROR);
            new_error_message = "Unknown Message Type";
            response_message->set_error_message(new_error_message);
          }
        // Exception during processing
        } catch (std::exception& e) {
          main_logging->error("Exception encountered during Processing");
          main_logging->error(e.what());
          response_message->set_error_code(PROCESSING_ERROR);
          new_error_message.assign(e.what());
          response_message->set_error_message(new_error_message);
        }
        response_message->set_msg_type(inbound_message->get_msg_type());
      }

      // Convert the response object to a message
      main_logging->debug("Building Response");
      std::string application_response;
      response_message->to_msg_string(application_response);

      // Send the response via ZMQ
      main_logging->info("Sending Response");
      main_logging->info(application_response);
      zmqi->send(application_response);

      // Cleanup
      if (response_message) {
        delete response_message;
        response_message = NULL;
      }
      if (inbound_message) {
        delete inbound_message;
        inbound_message = NULL;
      }

      // If we recieved a shutdown message, then we cleanup and exit
      if (shutdown_needed) {shutdown(); exit(1);}
  }
  return 0;
}
