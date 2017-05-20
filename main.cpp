//This sets up all of the components necessary for the service and runs the main
//loop for the application.

#include <sstream>
#include <string>
#include <string.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>
#include <exception>
#include <signal.h>

#include "src/include/app_log.h"
#include "src/include/app_utils.h"
#include "src/include/configuration_manager.h"
#include "src/include/globals.h"
#include "src/include/redis_locking.h"
#include "src/include/query_helper.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

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

//Catch a Signal (for example, keyboard interrupt)
void my_signal_handler(int s){
   main_logging->error("Caught signal");
   std::string signal_type = std::to_string(s);
   main_logging->error(signal_type);
   shutdown();
   exit(1);
}

    //-----------------------
    //------Main Method------
    //-----------------------

    int main( int argc, char** argv )
    {

      //Set up a handler for any signal events so that we always shutdown gracefully
      struct sigaction sigIntHandler;
      sigIntHandler.sa_handler = my_signal_handler;
      sigemptyset(&sigIntHandler.sa_mask);
      sigIntHandler.sa_flags = 0;

      sigaction(SIGINT, &sigIntHandler, NULL);

      cli_factory = new CommandLineInterpreterFactory;
      redis_factory = new RedisComponentFactory;
      uuid_factory = new uuidComponentFactory;
      zmq_factory = new ZmqComponentFactory;
      logging_factory = new LoggingComponentFactory;
      mongo_factory = new MongoComponentFactory;

      //Set up our command line interpreter
      cli = cli_factory->get_command_line_interface( argc, argv );

      //Set up logging
	    std::string initFileName;

      //See if we have a command line setting for the log file
      const char * env_logging_file = std::getenv("CRAZYIVAN_LOGGING_CONF");
      if ( env_logging_file ) {
        std::string tempFileName (env_logging_file);
        initFileName = tempFileName;
      }
      else if ( cli->opt_exist("-log-conf") ) {
        initFileName = cli->get_opt("-log-conf");
      }
      else
      {
        initFileName = "log4cpp.properties";
      }

      //This reads the logging configuration file
      logging = logging_factory->get_logging_interface(initFileName);

      //Set up the logging submodules for each category
      start_logging_submodules();

      //Set up the UUID Generator
      uid = uuid_factory->get_uuid_interface();

      std::string service_instance_id = "Ivan-";
      UuidContainer sid_container;
      try {
        sid_container = uid->generate();
        if (!sid_container.err.empty()) {
          main_logging->error(sid_container.err);
        }
        service_instance_id = service_instance_id + sid_container.id;
      }
      catch (std::exception& e) {
        main_logging->error("Exception encountered during Service Instance ID Generation");
        shutdown();
        exit(1);
      }

      //Set up our configuration manager with the CLI
      config = new ConfigurationManager(cli, service_instance_id);

      //The configuration manager will  look at any command line arguments,
      //configuration files, and Consul connections to try and determine the correct
      //configuration for the service

      bool config_success = false;
      try {
        config_success = config->configure();
      }
      catch (std::exception& e) {
        main_logging->error("Exception encountered during Configuration");
        shutdown();
        exit(1);
      }
      if (!config_success)
      {
        main_logging->error("Configuration Failed, defaults kept");
      }

      //Set up our Redis Connection List, which is passed to the Redis Admin to connect
      std::vector<RedisConnChain> RedisConnectionList = config->get_redisconnlist();
      //Set up Redis Connection
      if (RedisConnectionList.size() > 0) {
        try {
          //Currently only support for single Redis instance
          red = redis_factory->get_redis_interface(RedisConnectionList[0].ip, RedisConnectionList[0].port);
        }
        catch (std::exception& e) {
          main_logging->error("Exception encountered during Redis Initialization");
          main_logging->error(e.what());
          shutdown();
          exit(1);
        }
        main_logging->info("Connected to Redis");
      }
      else {
        main_logging->error("No Redis Connections found in configuration");
      }

      //Set up the Mongo Connection
      std::string DBConnStr = config->get_mongoconnstr();
      std::string DBName = config->get_dbname();
      std::string DBHeaderCollection = config->get_dbheadercollection();
      if ( !(DBConnStr.empty() || DBName.empty() || DBHeaderCollection.empty()) ) {
        try {
          mongo = mongo_factory->get_mongo_interface( DBConnStr, DBName, DBHeaderCollection );
          main_logging->debug("Connected to Mongo");
        }
        catch (std::exception& e) {
          main_logging->error("Exception encountered during Mongo Initialization");
          main_logging->error(e.what());
          shutdown();
          exit(1);
        }
      }
      else {
        main_logging->error("Insufficient Mongo Connection Information Supplied");
        shutdown();
        exit(1);
      }

      //Connect to the inbound ZMQ Admin
      std::string ib_zmq_connstr = config->get_ibconnstr();
      if ( !(ib_zmq_connstr.empty()) ) {
        zmqi = zmq_factory->get_zmq_inbound_interface(ib_zmq_connstr, REQ_RESP);
        main_logging->info("ZMQ Socket Open, opening request loop");
      }
      else {
        main_logging->error("No IB ZMQ Connection String Supplied");
        shutdown();
        exit(1);
      }

      //Set up the Redis Lock Helper
      RedisLocker lock (red);

      //Main Request Loop

      while (true) {

        std::string resp_str = "";
        rapidjson::Document d;
        protoObj3::Obj3List new_proto;

        //Convert the OMQ message into a string to be passed on the event
        char * req_ptr = zmqi->crecv();
        main_logging->debug("Conversion to C String performed with result: ");
        main_logging->debug(req_ptr);

        //Trim the string recieved
        std::string recvd_msg (req_ptr);
        std::string clean_string = trim(recvd_msg);

        main_logging->debug("Input String Cleaned");
        main_logging->debug(clean_string);

        if (config->get_formattype() == JSON_FORMAT) {

          clean_string = clean_string.substr(0, clean_string.find_last_of("\n")+1);
          clean_string = clean_string.substr(0, clean_string.find_last_of("}")+1);

          try {
            d.Parse<rapidjson::kParseStopWhenDoneFlag>(clean_string.c_str());
            if (d.HasParseError()) {
              main_logging->error("Parsing Error: ");
              main_logging->error(GetParseError_En(d.GetParseError()));
            } else {inbound_message = new Obj3List (d);}
          }
          //Catch a possible error and write to logs
          catch (std::exception& e) {
            main_logging->error("Exception occurred while parsing inbound document:");
            main_logging->error(e.what());
          }

        } else if (config->get_formattype() == PROTO_FORMAT) {
          try {
            new_proto.ParseFromString(clean_string);
            inbound_message = new Obj3List (new_proto);
          } catch (std::exception& e) {
            main_logging->error("Exception occurred while parsing inbound bytes:");
            main_logging->error(e.what());
          }
        }

          //Determine the Transaction ID
          UuidContainer id_container;
          id_container.id = "";
          if ( config->get_transactionidsactive() && inbound_message ) {
            //Get an existing transaction ID, currently empty as we don't assume format
            std::string existing_trans_id = inbound_message->get_transaction_id();
            //If no transaction ID is sent in, generate a new one
            if ( existing_trans_id.empty() ) {
              try {
                id_container = uid->generate();
                if (!id_container.err.empty()) {
                  main_logging->error(id_container.err);
                }
                inbound_message->set_transaction_id(id_container.id);
              }
              catch (std::exception& e) {
                main_logging->error("Exception encountered during UUID Generation");
                main_logging->error(e.what());
                shutdown();
                exit(1);
              }
            }
            main_logging->debug("Transaction ID: ");
            main_logging->debug( inbound_message->get_transaction_id() );
          }

          std::string new_error_message = "";
          response_message = new Obj3List;

          bool shutdown_needed = false;

          //Core application logic
          if (inbound_message) {
            //Set the response message type
            try {
              //Object Creation
              if (inbound_message->get_msg_type() == OBJ_CRT) {
                for (int i = 0;i < inbound_message->num_objects(); i++) {
                  Obj3 *resp_element = new Obj3;
                  MongoResponseInterface *resp = mongo->create_document( inbound_message->get_object(i)->to_json() );
                  resp_element->set_key( resp->get_value() );
                  response_message->add_object( resp_element );
                  delete resp;
                }
              //Object Update
              } else if (inbound_message->get_msg_type() == OBJ_UPD) {
                for (int i = 0;i < inbound_message->num_objects(); i++) {
                  //Enforce atomic updates -- establish redis lock
                  if (config->get_atomictransactions()) lock.get_lock( inbound_message->get_object(i)->get_key() );
                  //Load the current doc from the database
                  rapidjson::Document resp_doc;
                  MongoResponseInterface *resp = mongo->load_document( inbound_message->get_object(i)->get_key() );
                  resp_doc.Parse(resp->get_value().c_str());
                  Obj3 *resp_obj = new Obj3(resp_doc);
                  //Apply the object message as changes to the DB Object
                  resp_obj->merge(inbound_message->get_object(i));
                  //Save the resulting object
                  mongo->save_document( resp_obj->to_json(), resp_obj->get_key() );
                  response_message->add_object( resp_obj );
                  delete resp;
                  //Enforce atomic updates -- release redis lock
                  if (config->get_atomictransactions()) lock.release_lock( inbound_message->get_object(i)->get_key() );
                }
              //Object Retrieve
              } else if (inbound_message->get_msg_type() == OBJ_GET) {
                for (int i = 0;i < inbound_message->num_objects(); i++) {
                  rapidjson::Document resp_doc;
                  MongoResponseInterface *resp = mongo->load_document( inbound_message->get_object(i)->get_key() );
                  resp_doc.Parse(resp->get_value().c_str());
                  Obj3 *resp_obj = new Obj3(resp_doc);
                  response_message->add_object( resp_obj );
                  delete resp;
                }
              //Object Query
              } else if (inbound_message->get_msg_type() == OBJ_QUERY) {
                response_message = batch_query(inbound_message, mongo);
              //Object Delete
              } else if (inbound_message->get_msg_type() == OBJ_DEL) {
                for (int i = 0;i < inbound_message->num_objects(); i++) {
                  Obj3 *resp_element = new Obj3;
                  resp_element->set_key( inbound_message->get_object(i)->get_key() );
                  mongo->delete_document( inbound_message->get_object(i)->get_key() );
                  response_message->add_object( resp_element );
                }
              //Ping
              } else if (inbound_message->get_msg_type() == PING) {
                main_logging->info("Ping Message Recieved");
              //Kill
              } else if (inbound_message->get_msg_type() == KILL) {
                main_logging->info("Shutting Down");
                shutdown_needed = true;
              //Invalid Message Type
              } else {
                response_message->set_error_code(BAD_MSG_TYPE_ERROR);
                new_error_message = "Unknown Message Type";
                response_message->set_error_message(new_error_message);
              }
            //Exception during processing
            } catch (std::exception& e) {
              main_logging->error("Exception encountered during Processing");
              main_logging->error(e.what());
              response_message->set_error_code(PROCESSING_ERROR);
              new_error_message.assign(e.what());
              response_message->set_error_message(new_error_message);
            }
            response_message->set_msg_type( inbound_message->get_msg_type() );
          }

          main_logging->debug("Building Response");
          std::string application_response = "";
          if (config->get_formattype() == JSON_FORMAT) application_response = response_message->to_json();
          if (config->get_formattype() == PROTO_FORMAT) application_response = response_message->to_protobuf();

          main_logging->info("Sending Response");
          main_logging->info( application_response );
          zmqi->send( application_response );

          if (shutdown_needed) {shutdown();exit(1);}

      }
      return 0;
    }
