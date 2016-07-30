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

#include "src/obj3.h"
#include "src/lyman_utils.h"
#include "src/document_manager.h"
#include "src/configuration_manager.h"
#include "src/couchbase_callbacks.h"
#include "src/globals.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <aossl/couchbase_admin.h>
#include <aossl/xredis_admin.h>
#include <aossl/logging.h>
#include <aossl/uuid_admin.h>
#include <aossl/cli.h>

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
  delete cm;
  delete ua;
  delete cli;

  //Shut down libraries
  google::protobuf::ShutdownProtobufLibrary();
  end_log();
}

//Catch a Signal (for example, keyboard interrupt)
void my_signal_handler(int s){
   logging->error("Caught signal");
   std::string signal_type = std::to_string(s);
   logging->error(signal_type);
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

      //Set up the UUID Generator
      ua = new uuidAdmin;

      //Set up our configuration manager
      cm = new ConfigurationManager;

      //Set up our command line interpreter
      cli = new CommandLineInterpreter ( argc, argv );

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
      try {
        log4cpp::PropertyConfigurator::configure(initFileName);
      }
      catch ( log4cpp::ConfigureFailure &e ) {
        std::cout << "[log4cpp::ConfigureFailure] caught while reading" << initFileName << std::endl;
        std::cout << e.what();
        exit(1);
      }

      log4cpp::Category& root = log4cpp::Category::getRoot();

      log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("sub1"));

      log4cpp::Category& log = log4cpp::Category::getInstance(std::string("sub1.log"));

      logging = &log;

      //Here we pass the command line interpreter into the configuration manager
      //The configuration manager will then look at any command line arguments,
      //configuration files, and Consul connections to try and determine the correct
      //configuration for the service

      bool config_success = cm->configure( cli, ua );
      if (!config_success)
      {
        logging->error("Configuration Failed, defaults kept");
      }

      //Set up internal variables
      int current_event_type;
      int msg_type;
      rapidjson::Document d;
      rapidjson::Value *val;
      std::string resp = "nilresp";
      logging->info("Internal Variables Intialized");
      protoObj3::Obj3 new_proto;

      //Set up our Redis Connection List, which is passed to the Redis Admin to connect
      //The additional logic is needed to allow for connecting to clusters or single instance
      std::vector<RedisConnChain> RedisConnectionList = cm->get_redisconnlist();
      int conn_list_size = RedisConnectionList.size();
      RedisNode RedisList1[conn_list_size];
      int y = 0;
      for (int y = 0; y < conn_list_size; ++y)
      {
        //Pull the values from RedisConnectionList
        RedisNode redis_n;
        redis_n.dbindex = y;
        RedisConnChain redis_chain = RedisConnectionList[y];
        redis_n.host = redis_chain.ip.c_str();
        redis_n.port = redis_chain.port;
        redis_n.passwd = redis_chain.elt4.c_str();
        redis_n.poolsize = redis_chain.elt5;
        redis_n.timeout = redis_chain.elt6;
        redis_n.role = redis_chain.elt7;
        logging->debug("Line added to Redis Configuration List with IP:");
        logging->debug(redis_n.host);

        RedisList1[y] = redis_n;
      }
      logging->info("Redis Connection List Built");

      //Set up Redis Connection
      xRedis = new xRedisAdmin (RedisList1, conn_list_size);
      logging->info("Connected to Redis");

      //Set up the Couchbase Connection
      std::string DBConnStr = cm->get_dbconnstr();
      bool DBAuthActive = cm->get_dbauthactive();
      if (DBAuthActive) {
        std::string DBPswd = cm->get_dbpswd();
        cb = new CouchbaseAdmin ( DBConnStr.c_str(), DBPswd.c_str() );
      }
      else {
        cb = new CouchbaseAdmin ( DBConnStr.c_str() );
      }
      logging->info("Connected to Couchbase DB");

      //Bind Couchbase Callbacks
      lcb_set_store_callback(cb->get_instance(), storage_callback);
      lcb_set_get_callback(cb->get_instance(), get_callback);
      lcb_set_remove_callback(cb->get_instance(), del_callback);

      //We maintain the ZMQ Context and pass it to the ZMQ objects coming from aossl
      zmq::context_t context(1, 2);

      //Set up the outbound ZMQ Admin
      zmqo = new Zmqo (context);
      logging->info("0MQ Constructor Called");
      zmqo->connect(cm->get_obconnstr());
      logging->info("Connected to Outbound OMQ Socket");

      //Connect to the inbound ZMQ Admin
      zmqi = new Zmqi (context);
      logging->info("0MQ Constructor Called");
      zmqi->bind(cm->get_ibconnstr());
      logging->info("ZMQ Socket Open, opening request loop");

      //Set up the Document Manager
      //This relies on pointers to all the other objects we set up,
      //and drives the central functionality, along with the couchbase callbacks
      dm = new DocumentManager (cb, xRedis, ua, cm, zmqo);

      //Main Request Loop

      while (true) {

        //Convert the OMQ message into a string to be passed on the event
        std::string req_string = zmqi->recv();
        req_string = left_trim_string (req_string);
        const char * req_ptr = req_string.c_str();
        logging->debug("Conversion to C String performed with result: ");
        logging->debug(req_ptr);
        bool go_ahead=false;

        //If we are expecting JSON Messages, then parse in this fashion
        if (cm->get_mfjson()) {
          //Process the message header and set current_event_type
          try {
            d.Parse(req_ptr);
            go_ahead=true;
          }
          //Catch a possible error and write to logs
          catch (std::exception& e) {
            logging->error("Exception occurred while parsing inbound document:");
            logging->error(e.what());
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
            go_ahead=true;
          }
          //Catch a possible error and write to logs
          catch (std::exception& e) {
            logging->error("Exception occurred while parsing inbound document:");
            logging->error(e.what());
          }
          //Find the message type
          if (go_ahead) {
            msg_type = new_proto.message_type();
          }
        }

        if (msg_type == OBJ_UPD) {
          current_event_type=OBJ_UPD;
          logging->debug("Current Event Type set to Object Update");
        }
        else if (msg_type == OBJ_CRT) {
          current_event_type=OBJ_CRT;
          logging->debug("Current Event Type set to Object Create");
        }
        else if (msg_type == OBJ_GET) {
          current_event_type=OBJ_GET;
          logging->debug("Current Event Type set to Object Get");
        }
        else if (msg_type == OBJ_DEL) {
          current_event_type=OBJ_DEL;
          logging->debug("Current Event Type set to Object Delete");
        }
        //Shutdown Message
        else if (msg_type == KILL) {

          //Send a success response
          resp = "success";
          zmqi->send_str(resp);

          shutdown();

          return 0;
        }
        else if (msg_type == PING) {
          resp = "success";
          zmqi->send_str(resp);
        }
        else {
          current_event_type=-1;
          logging->error("Current Event Type not found");
        }


        //Emit an event based on the event type & build the response message
        if (current_event_type==OBJ_UPD) {
          resp = "success";
          logging->debug("Object Update Event Emitted, response:");
          logging->debug(resp);

          //  Send reply back to client
          zmqi->send_str(resp);
          logging->debug("Response Sent");

          //Call the appropriate method from the document manager to kick off the rest of the flow
          if (cm->get_mfjson()) {
            dm->update_objectd( d );
          }
          else if (cm->get_mfprotobuf()) {
            dm->update_objectpb(new_proto);
          }

        }
        else if (current_event_type==OBJ_CRT) {
          resp = "success";
          logging->debug("Object Create Event Emitted, response: ");
          logging->debug(resp);

          //Send the response
          zmqi->send_str(resp);
          logging->debug("Response Sent");

          //Call the appropriate method from the document manager to kick off the rest of the flow
          if (cm->get_mfjson()) {
            dm->create_objectd( d );
          }
          else if (cm->get_mfprotobuf()) {
            dm->create_objectpb(new_proto);
          }
        }
        else if (current_event_type==OBJ_GET) {
          resp = "success";
          logging->debug("Object Get Event Emitted, response: ");
          logging->debug(resp);

          //  Send reply back to client
          zmqi->send_str(resp);
          logging->debug("Response Sent");

          //Call the appropriate method from the document manager to kick off the rest of the flow
          if (cm->get_mfjson()) {
            dm->get_objectd( d );
          }
          else if (cm->get_mfprotobuf()) {
            dm->get_objectpb (new_proto);
          }
        }
        else if (current_event_type==OBJ_DEL) {
          resp = "success";
          logging->debug("Object Delete Event Emitted, response: ");
          logging->debug(resp);

          //  Send reply back to client
          zmqi->send_str(resp);
          logging->debug("Response Sent");

          //Call the appropriate method from the document manager to kick off the rest of the flow
          if (cm->get_mfjson()) {
            dm->delete_objectd( d );
          }
          else if (cm->get_mfprotobuf()) {
            dm->delete_objectpb(new_proto);
          }
        }
        else
        {
          resp = "failure";
          logging->error("Object Event not Emitted, response: ");
          logging->error(resp);

          //  Send reply back to client
          zmqi->send_str(resp);
          logging->debug("Response Sent");
        }
      }

      return 0;
    }
