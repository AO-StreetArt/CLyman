//This is the  main structural code of the module
//It is built to allow for real-time messaging without the need
//For polling, using a Dispatcher Pattern

#include <zmq.hpp>
#include <sstream>
#include <string>
#include <string.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>
#include <exception>
#include <uuid/uuid.h>
#include <Eigen/Dense>

#include "src/event_dispatcher.h"
#include "src/obj3.h"
#include "src/couchbase_admin.h"
#include "src/xredis_admin.h"
#include "src/lyman_utils.h"
#include "src/document_manager.h"
#include "src/configuration_manager.h"
#include "src/couchbase_callbacks.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "src/logging.h"
#include "src/globals.h"

enum {
  CACHE_TYPE_1,
  CACHE_TYPE_2,
  CACHE_TYPE_MAX,
};

    //-----------------------
    //------Main Method------
    //-----------------------

    int main()
    {

      //Set up our configuration manager
      cm = new ConfigurationManager ();

      //Set up logging
      //This reads the logging configuration file
      std::string initFileName = "log4cpp.properties";
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

      //Read the application configuration file

      cm->configure("lyman.properties");

      //Set up internal variables
      int current_event_type;
      int msg_type;
      rapidjson::Document d;
      rapidjson::Value *s;
      char resp[8]={'n','i','l','r','e','s','p','\0'};
      logging->info("Internal Variables Intialized");
      protoObj3::Obj3 new_proto;

      //Set up our Redis Connection List
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

      zmq::context_t context(1, 2);

      //Set up the outbound ZMQ Client
      //zmq::socket_t zout(context, ZMQ_REQ);
      zmqo = new zmq::socket_t (context, ZMQ_REQ);
      logging->info("0MQ Constructor Called");
      zmqo->connect(cm->get_obconnstr());
      logging->info("Connected to Outbound OMQ Socket");

      //Connect to the inbound ZMQ Socket
      zmq::socket_t socket(context, ZMQ_REP);
      socket.bind(cm->get_ibconnstr());
      logging->info("ZMQ Socket Open, opening request loop");

      //Set up the Document Manager
      dm = new DocumentManager (cb, xRedis, cm);

      //Main Request Loop

      while (true) {
        zmq::message_t request;

        //  Wait for next request from client
        socket.recv (&request);
        logging->info("Request Recieved");

        //Convert the OMQ message into a string to be passed on the event
        std::string req_string;
        req_string = left_trim_string (hexDump (request));
        const char * req_ptr = req_string.c_str();
        logging->debug("Conversion to C String performed with result: ");
        logging->debug(req_ptr);
        bool go_ahead=false;
        if (cm->get_mfjson()) {
          //Process the message header and set current_event_type
          try {
            d.Parse(req_ptr);
            go_ahead=true;
          }
          catch (std::exception& e) {
            logging->error("Exception occurred while parsing inbound document:");
            logging->error(e.what());
          }
          //Catch a possible error and write to logs
          if (go_ahead) {
            s = &d["message_type"];
            msg_type = s->GetInt();
          }
        }
        else if (cm->get_mfprotobuf()) {
          try {
            new_proto.Clear();
            new_proto.ParseFromString(req_string);
            go_ahead=true;
          }
          catch (std::exception& e) {
            logging->error("Exception occurred while parsing inbound document:");
            logging->error(e.what());
          }
          //Catch a possible error and write to logs
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
        else if (msg_type == 999) {
          end_log();

          //Delete objects off the heap
          delete xRedis;
          delete cb;
          delete zmqo;
          delete cm;
          delete dm;

          resp[0]='s';
          resp[1]='u';
          resp[2]='c';
          resp[3]='c';
          resp[4]='e';
          resp[5]='s';
          resp[6]='s';

          //  Send reply back to client
          zmq::message_t reply (8);

          //Prepare return data
          memcpy (reply.data (), resp, 8);
          //Send the response
          socket.send (reply);
          // Optional:  Delete all global objects allocated by libprotobuf.
          google::protobuf::ShutdownProtobufLibrary();
          logging->debug("Response Sent, terminating");
          return 0;
        }
        else {
          current_event_type=-1;
          logging->error("Current Event Type not found");
        }


        //Emit an event based on the event type & build the response message
        if (current_event_type==OBJ_UPD) {
          resp[0]='s';
          resp[1]='u';
          resp[2]='c';
          resp[3]='c';
          resp[4]='e';
          resp[5]='s';
          resp[6]='s';
          logging->debug("Object Update Event Emitted, response:");
          logging->debug(resp);
          //  Send reply back to client
          zmq::message_t reply (8);

          //Prepare return data
          memcpy (reply.data (), resp, 8);

          //Send the response
          socket.send (reply);
          logging->debug("Response Sent");
          if (cm->get_mfjson()) {
            dm->update_objectd( d );
          }
          else if (cm->get_mfprotobuf()) {
            dm->update_objectpb(new_proto);
          }

        }
        else if (current_event_type==OBJ_CRT) {
          resp[0]='s';
          resp[1]='u';
          resp[2]='c';
          resp[3]='c';
          resp[4]='e';
          resp[5]='s';
          resp[6]='s';
          logging->debug("Object Create Event Emitted, response: ");
          logging->debug(resp);
          //  Send reply back to client
          zmq::message_t reply (8);

          //Prepare return data
          memcpy (reply.data (), resp, 8);

          //Send the response
          socket.send (reply);
          logging->debug("Response Sent");
          if (cm->get_mfjson()) {
            dm->create_objectd( d );
          }
          else if (cm->get_mfprotobuf()) {
            dm->create_objectpb(new_proto);
          }
        }
        else if (current_event_type==OBJ_GET) {
          resp[0]='s';
          resp[1]='u';
          resp[2]='c';
          resp[3]='c';
          resp[4]='e';
          resp[5]='s';
          resp[6]='s';
          logging->debug("Object Get Event Emitted, response: ");
          logging->debug(resp);
          //  Send reply back to client
          zmq::message_t reply (8);

          //Prepare return data
          memcpy (reply.data (), resp, 8);

          //Send the response
          socket.send (reply);
          logging->debug("Response Sent");
          if (cm->get_mfjson()) {
            dm->get_objectd( d );
          }
          else if (cm->get_mfprotobuf()) {
            dm->get_objectpb (new_proto);
          }
        }
        else if (current_event_type==OBJ_DEL) {
          resp[0]='s';
          resp[1]='u';
          resp[2]='c';
          resp[3]='c';
          resp[4]='e';
          resp[5]='s';
          resp[6]='s';
          logging->debug("Object Delete Event Emitted, response: ");
          logging->debug(resp);
          //  Send reply back to client
          zmq::message_t reply (8);

          //Prepare return data
          memcpy (reply.data (), resp, 8);

          //Send the response
          socket.send (reply);
          logging->debug("Response Sent");
          if (cm->get_mfjson()) {
            dm->delete_objectd( d );
          }
          else if (cm->get_mfprotobuf()) {
            dm->delete_objectpb(new_proto);
          }
        }
        else
        {
          resp[0]='f';
          resp[1]='a';
          resp[2]='i';
          resp[3]='l';
          resp[4]='u';
          resp[5]='r';
          resp[6]='e';
          logging->error("Object Event not Emitted, response: ");
          logging->error(resp);
          //  Send reply back to client
          zmq::message_t reply (8);

          //Prepare return data
          memcpy (reply.data (), resp, 8);

          //Send the response
          socket.send (reply);
          logging->debug("Response Sent");
        }
      }

      return 0;
    }
