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
#include "src/document_manager.h"
#include "src/configuration_manager.h"
#include "src/couchbase_callbacks.h"
#include "src/globals.h"
#include "src/clyman_response.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <aossl/couchbase_admin.h>
#include <aossl/xredis_admin.h>
#include <aossl/logging.h>
#include <aossl/uuid_admin.h>
#include <aossl/cli.h>

#include "aossl/factory.h"
#include "aossl/factory/couchbase_interface.h"
#include "aossl/factory/redis_interface.h"
#include "aossl/factory/logging_interface.h"
#include "aossl/factory/uuid_interface.h"
#include "aossl/factory/commandline_interface.h"
#include "aossl/factory/response_interface.h"

//Set up an Obj3 pointer to hold the currently translated document information
Obj3 *translated_object = NULL;

enum {
  CACHE_TYPE_1,
  CACHE_TYPE_2,
  CACHE_TYPE_MAX,
};

//Shutdown the application
void shutdown()
{
  //Delete objects off the heap
  delete dm;
  delete xRedis;
  delete cb;
  delete zmqo;
  delete zmqi;
  delete cm;
  delete ua;
  delete cli;

  if(!resp) {
    delete resp;
  }

  if (!translated_object) {main_logging->debug("No translated object active at time of shutdown");}
  else {delete translated_object;}

  shutdown_logging_submodules();
  delete logging;

  //Shut down protocol buffer library
  google::protobuf::ShutdownProtobufLibrary();
}

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

      ServiceComponentFactory *factory = new ServiceComponentFactory;

      //Set up our command line interpreter
      cli = factory->get_command_line_interface( argc, argv );

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
      logging = factory->get_logging_interface(initFileName);

      //Set up the logging submodules for each category
      start_logging_submodules();

      //Set up the UUID Generator
      ua = factory->get_uuid_interface();

      //Set up our configuration manager with the CLI and UUID Generator
      cm = new ConfigurationManager(cli, ua, factory);

      //The configuration manager will  look at any command line arguments,
      //configuration files, and Consul connections to try and determine the correct
      //configuration for the service

      bool config_success = cm->configure();
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
      xRedis = factory->get_redis_cluster_interface(RedisConnectionList);
      main_logging->info("Connected to Redis");

      //Set up the Couchbase Connection
      std::string DBConnStr = cm->get_dbconnstr();
      DBConnStr = trim(DBConnStr);
      bool DBAuthActive = cm->get_dbauthactive();
      if (DBAuthActive) {
        std::string DBPswd = cm->get_dbpswd();
        cb = factory->get_couchbase_interface( DBConnStr.c_str(), DBPswd.c_str() );
      }
      else {
        cb = factory->get_couchbase_interface( DBConnStr.c_str() );
      }

      //Bind Couchbase Callbacks
      cb->bind_storage_callback(my_storage_callback);
      cb->bind_get_callback(my_retrieval_callback);
      cb->bind_delete_callback(my_delete_callback);

      //Set up the outbound ZMQ Admin
      zmqo = factory->get_zmq_outbound_interface(cm->get_obconnstr());
      main_logging->info("Connected to Outbound OMQ Socket");

      //Connect to the inbound ZMQ Admin
      zmqi = factory->get_zmq_inbound_interface(cm->get_ibconnstr());
      main_logging->info("ZMQ Socket Open, opening request loop");

      //Set up the Document Manager
      //This relies on pointers to all the other objects we set up,
      //and drives the central functionality, along with the couchbase callbacks
      dm = new DocumentManager (cb, xRedis, ua, cm, zmqo);

      //Set up a response object to be sent back to the client
      resp = factory->get_application_response_interface();

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
            translated_object = new Obj3 (d);
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
            translated_object = new Obj3 (new_proto);
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
        std::string tran_id_str = ua->generate();
        resp->set_transaction_id(tran_id_str);

        //Set up the object key to be passed back on the response
        std::string object_key;

        if (msg_type == OBJ_UPD) {
          main_logging->debug("Current Event Type set to Object Update");

          //Call the appropriate method from the document manager to kick off the rest of the flow
          if (cm->get_mfjson()) {

            //Make the update
            object_key = dm->update_object( translated_object, tran_id_str );

            //Add the Object Key to the Response
            resp->set_object_id(object_key);

            //  Send reply back to client
            zmqi->send(resp->to_json());
            main_logging->debug("Response Sent");
          }
          else if (cm->get_mfprotobuf()) {

            //Make the update
            object_key = dm->update_object( translated_object, tran_id_str );

            //Add the Object Key to the Response
            resp->set_object_id(object_key);

            //  Send reply back to client
            zmqi->send(response_to_protobuffer(resp));
            main_logging->debug("Response Sent");
          }

          //Send the update to couchbase
          if (!translated_object) {
            main_logging->debug("Translated Object not found");
            if (resp->get_error_code == NOERROR) {
              resp->set_error(TRANSLATION_ERROR, "Translated Object not found";
            }
          }
          else
          {
            if (!object_key.empty()) {
              if (cm->get_smartupdatesactive()) {
                cb->load_object( object_key.c_str() );
                cb->wait();
              }
              else
              {
                cb->save_object (translated_object);
                cb->wait();
              }
            }
            else
            {
              main_logging->debug("Message recieved without key");
              resp->set_error(BAD_REQUEST_ERROR, "Message recieved without key");
            }
          }

        }
        else if (msg_type == OBJ_CRT) {
          main_logging->debug("Current Event Type set to Object Create");

          //Call the appropriate method from the document manager to kick off the rest of the flow
          if (cm->get_mfjson()) {

            //Create the object
            object_key = dm->create_object(translated_object, tran_id_str);

            //Add the Object Key to the Response
            resp->set_object_id(object_key);

            //Send the response
            zmqi->send(resp->to_json());
            main_logging->debug("Response Sent");
          }
          else if (cm->get_mfprotobuf()) {

            //Create the object
            object_key = dm->create_object(translated_object, tran_id_str);

            //Add the Object Key to the Response
            resp->set_object_id(object_key);

            //Send the response
            zmqi->send(response_to_protobuffer(resp));
            main_logging->debug("Response Sent");
          }
          if (!translated_object)
          {
            if (resp->get_error_code == NOERROR) {
              resp->set_error(TRANSLATION_ERROR, "Translated Object not found";
            }
            main_logging->debug("Translated Object not found");
          }
          else
          {
            if (!object_key.empty()) {
              //Save the object to the couchbase DB
              cb->create_object (translated_object);
              cb->wait();
            }
            else
            {
              main_logging->debug("Creation of new object key failed");
              resp->set_error(BAD_REQUEST_ERROR, "Creation of new object key failed");
            }
          }
        }

        else if (msg_type == OBJ_GET) {
          main_logging->debug("Current Event Type set to Object Get");

          //Call the appropriate method from the document manager to kick off the rest of the flow
          if (cm->get_mfjson()) {

            //Get the object
            object_key = dm->get_object( translated_object, tran_id_str );

            //Add the Object Key to the Response
            resp->set_object_id(object_key);

            //Send the response
            zmqi->send(resp->to_json());
            main_logging->debug("Response Sent");
          }
          else if (cm->get_mfprotobuf()) {

            //Get the object
            object_key = dm->get_object (translated_object, tran_id_str );

            //Add the Object Key to the Response
            resp->set_object_id(object_key);

            //Send the response
            zmqi->send(response_to_protobuffer(resp));
            main_logging->debug("Response Sent");
          }

          if (!translated_object)
          {
            if (resp->get_error_code == NOERROR) {
              resp->set_error(TRANSLATION_ERROR, "Translated Object not found";
            }
            main_logging->debug("Translated Object not found");
          }
          else
          {
            if (!object_key.empty()) {
              cb->load_object( object_key.c_str() );
              cb->wait();
            }
            else
            {
              main_logging->debug("Message recieved without key");
              resp->set_error(BAD_REQUEST_ERROR, "Message recieved without key");
            }
          }
        }

        else if (msg_type == OBJ_DEL) {
          main_logging->debug("Current Event Type set to Object Delete");

          //Call the appropriate method from the document manager to kick off the rest of the flow
          if (cm->get_mfjson()) {

            //Delete the object
            object_key = dm->delete_object( translated_object, tran_id_str );

            //Add the Object Key to the Response
            resp->set_object_id(object_key);

            //Send the response
            zmqi->send(resp->to_json());
            main_logging->debug("Response Sent");
          }
          else if (cm->get_mfprotobuf()) {

            //Delete the object
            object_key = dm->delete_object( translated_object, tran_id_str );

            //Add the Object Key to the Response
            resp->set_object_id(object_key);

            //Send the response
            zmqi->send(response_to_protobuffer(resp));
            main_logging->debug("Response Sent");
          }

          if (!translated_object)
          {
            if (resp->get_error_code == NOERROR) {
              resp->set_error(TRANSLATION_ERROR, "Translated Object not found";
            }
            main_logging->debug("Translated Object not found");
          }
          else
          {
            if (!object_key.empty()) {
              cb->delete_object( object_key.c_str() );
              cb->wait();
            }
            else
            {
              main_logging->debug("Message recieved without key");
              resp->set_error(BAD_REQUEST_ERROR, "Message recieved without key");
            }
          }
        }

        //Shutdown Message
        else if (msg_type == KILL) {

          //Send a success response
          if (cm->get_mfjson()) {
            zmqi->send(resp->to_json());
          }
          else if (cm->get_mfprotobuf()) {
            zmqi->send(response_to_protobuffer(resp));
          }

          shutdown();

          return 0;
        }

        //Healthcheck message
        else if (msg_type == PING) {
          main_logging->debug("Healthcheck Responded to");
          if (cm->get_mfjson()) {
            zmqi->send(resp->to_json());
          }
          else if (cm->get_mfprotobuf()) {
            zmqi->send(response_to_protobuffer(resp));
          }
        }
        
        //Message type failure
        else {
          main_logging->error("Current Event Type not found");

          resp->set_error(BAD_REQUEST_ERROR, "No Message type found");
          main_logging->error("Object Event not Emitted, response: ");

          //  Send reply back to client
          if (cm->get_mfjson()) {
            zmqi->send(resp->to_json());
          }
          else if (cm->get_mfprotobuf()) {
            zmqi->send(response_to_protobuffer(resp));
          }
          main_logging->debug("Response Sent");
        }

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
