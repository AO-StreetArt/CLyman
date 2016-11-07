//This is the  main structural code of CLyman

//It sets up all of the Objects which help handle messages, then accepts messages,
//parses them, and passes them to the document manager which is responsible for them
//until they hit Couchbase.  Couchbase then gives us a set of callbacks which drive
//the flow from the time of response by couchbase to the sending of outbound messages.

//The smart update flow is a bit more complex, but mainly lies in the callbacks.
//The document manager starts the chain with a load command, and the callback for this
//calls a save command, which has a callback of it's own (albeit a simple one).

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

#include <zmq.hpp>
#include <Eigen/Dense>

#include "src/lyman_log.h"
#include "src/obj3.h"
#include "src/lyman_utils.h"
#include "src/configuration_manager.h"
#include "src/message_processor.h"
#include "src/globals.h"
#include "src/clyman_response.h"
#include "src/uuid.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "aossl/factory_cli.h"
#include "aossl/factory_mongo.h"
#include "aossl/factory_logging.h"
#include "aossl/factory_redis.h"
#include "aossl/factory_uuid.h"
#include "aossl/factory_zmq.h"
#include "aossl/factory_response.h"

#include "aossl/factory/mongo_interface.h"
#include "aossl/factory/redis_interface.h"
#include "aossl/factory/logging_interface.h"
#include "aossl/factory/uuid_interface.h"
#include "aossl/factory/commandline_interface.h"
#include "aossl/factory/response_interface.h"

enum {
  CACHE_TYPE_1,
  CACHE_TYPE_2,
  CACHE_TYPE_MAX,
};

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

      CommandLineInterpreterFactory *cli_factory = new CommandLineInterpreterFactory;
      MongoComponentFactory *mongo_factory = new MongoComponentFactory;
      RedisComponentFactory *redis_factory = new RedisComponentFactory;
      uuidComponentFactory *uuid_factory = new uuidComponentFactory;
      ZmqComponentFactory *zmq_factory = new ZmqComponentFactory;
      LoggingComponentFactory *logging_factory = new LoggingComponentFactory;

      //Set up our command line interpreter
      cli = cli_factory->get_command_line_interface( argc, argv );

      //Set up logging
	    std::string initFileName;

      //See if we have a command line setting for the log file
      if ( cli->opt_exist("-log-conf") ) {
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
      ua = uuid_factory->get_uuid_interface();

      std::string service_instance_id = "CLyman-";
      try {
        service_instance_id = service_instance_id + generate_uuid();
      }
      catch (std::exception& e) {
        main_logging->error("Exception encountered during UUID Generation");
        shutdown();
        exit(1);
      }

      //Set up our configuration manager with the CLI
      cm = new ConfigurationManager(cli, service_instance_id);

      //The configuration manager will  look at any command line arguments,
      //configuration files, and Consul connections to try and determine the correct
      //configuration for the service

      bool config_success;
      try {
        config_success = cm->configure();
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

      //Set up internal variables
      int msg_type = -1;
      rapidjson::Document d;
      rapidjson::Value *val;
      main_logging->info("Internal Variables Intialized");
      protoObj3::Obj3 new_proto;

      //Set up our Redis Connection List, which is passed to the Redis Admin to connect
      //The additional logic is needed to allow for connecting to clusters or single instance
      std::vector<RedisConnChain> RedisConnectionList = cm->get_redisconnlist();
      //Set up Redis Connection
      if (RedisConnectionList.size() > 0) {
        try {
          xRedis = redis_factory->get_redis_interface(RedisConnectionList[0].ip, RedisConnectionList[0].port);
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
      std::string DBConnStr = cm->get_dbconnstr();
      DBConnStr = trim(DBConnStr);
      std::string DBName = cm->get_dbname();
      DBName = trim(DBName);
      std::string DBCollection = cm->get_dbcollection();
      DBCollection = trim(DBCollection);
      if ( !(DBConnStr.empty() || DBName.empty() || DBCollection.empty()) ) {
        try {
          mongo = mongo_factory->get_mongo_interface( DBConnStr, DBName, DBCollection );
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

      //Set up the outbound ZMQ Admin
      std::string ob_zmq_connstr = cm->get_obconnstr();
      if (! (ob_zmq_connstr.empty()) ) {
        zmqo = zmq_factory->get_zmq_outbound_interface(ob_zmq_connstr, PUB_SUB);
        main_logging->info("Connected to Outbound OMQ Socket");
      }
      else {
        main_logging->error("No OB ZMQ Connection String Supplied");
        shutdown();
        exit(1);
      }

      //Connect to the inbound ZMQ Admin
      std::string ib_zmq_connstr = cm->get_ibconnstr();
      if ( !(ib_zmq_connstr.empty()) ) {
        zmqi = zmq_factory->get_zmq_inbound_interface(ib_zmq_connstr, REQ_RESP);
        main_logging->info("ZMQ Socket Open, opening request loop");
      }
      else {
        main_logging->error("No IB ZMQ Connection String Supplied");
        shutdown();
        exit(1);
      }

      //Set up the Message Processor
      processor = new MessageProcessor (mongo, zmqo, xRedis, cm);

      //Set up a response object to be sent back to the client
      resp = response_factory->get_application_response_interface();

      //Main Request Loop

      while (true) {

        msg_type = -1;

        //Convert the OMQ message into a string to be passed on the event
        std::string req_string = zmqi->recv();
        req_string = ltrim(req_string);
        const char * req_ptr = req_string.c_str();
        main_logging->debug("Conversion to C String performed with result: ");
        main_logging->debug(req_ptr);
        bool go_ahead=false;
        resp->set_error(NO_ERROR);

        //If we are expecting JSON Messages, then parse in this fashion
        if (cm->get_mfjson()) {
          //Process the message header and set current_event_type
          try {
            d.Parse(req_ptr);
            translated_object = new Obj3 (d, cm->get_objectlockingenabled());
            go_ahead=true;
          }
          //Catch a possible error and write to logs
          catch (std::exception& e) {
            main_logging->error("Exception occurred while parsing inbound document:");
            main_logging->error(e.what());
            resp->set_error(TRANSLATION_ERROR, e.what());
          }
          //Find the message type
          if (go_ahead) {
            val = &d["message_type"];
            msg_type = val->GetInt();
          }
        }

        //If we are expecting Protobuffer Messages, then parse in this fashion
        else if (cm->get_mfprotobuf()) {
          try {
            new_proto.Clear();
            new_proto.ParseFromString(req_string);
            translated_object = new Obj3 (new_proto, cm->get_objectlockingenabled());
            go_ahead=true;
          }
          //Catch a possible error and write to logs
          catch (std::exception& e) {
            main_logging->error("Exception occurred while parsing inbound document:");
            main_logging->error(e.what());
            resp->set_error(TRANSLATION_ERROR, e.what());
          }
          //Find the message type
          if (go_ahead) {
            msg_type = new_proto.message_type();
          }
        }

        //Generate a Transaction ID
        std::string tran_id_str;
        try {
          tran_id_str = generate_uuid();
          main_logging->debug("Generated Transaction ID: " + tran_id_str);
        }
        catch (std::exception& e) {
          main_logging->error("Exception encountered during UUID Generation");
          shutdown();
          exit(1);
        }
        main_logging->debug(tran_id_str);
        resp->set_transaction_id(tran_id_str);
        if (!translated_object)
        {
          main_logging->debug("No translated object to assign Transaction ID to");
        }
        else {
          translated_object->set_transaction_id(tran_id_str);
        }

        //Process the translated object
        std::string process_result = processor->process_message(translated_object);

        //TO-DO: Convert reply from message processor to reply for client

        //  Send reply back to client
        if (cm->get_mfjson()) {
          zmqi->send(resp->to_json());
        }
        else if (cm->get_mfprotobuf()) {
          zmqi->send(response_to_protobuffer(resp));
        }
        main_logging->debug("Response Sent");

        //Clear the response
        resp->clear();

        //Clear the translated object
        if (!translated_object) {
          main_logging->debug("Translated Object not found for deletion");
        }
        else {
          delete translated_object;
          translated_object = NULL;
        }
      }

      return 0;
    }
