//This defines some of the global variables that need to be accessed
//Across different files.  These are either numeric constants or
//singletons which form the backbone of the service.

#include "aossl/factory/mongo_interface.h"
#include "aossl/factory/redis_interface.h"
#include "aossl/factory/uuid_interface.h"
#include "aossl/factory/commandline_interface.h"
#include "aossl/factory/zmq_interface.h"
#include "aossl/factory/response_interface.h"
#include "aossl/factory/logging_interface.h"

#include "aossl/factory_response.h"
#include "aossl/factory_cli.h"
#include "aossl/factory_mongo.h"
#include "aossl/factory_logging.h"
#include "aossl/factory_redis.h"
#include "aossl/factory_uuid.h"
#include "aossl/factory_zmq.h"

#include "message_processor.h"
#include "configuration_manager.h"
#include "Obj3.pb.h"
#include "lyman_log.h"

#include <unordered_map>

#ifndef GLOBALS
#define GLOBALS

//Set up an Obj3 pointer to hold the currently translated document information
extern Obj3 *translated_object;

//Globals defined within this service
extern ConfigurationManager *cm;
extern MessageProcessor *processor;

//Globals from the AO Shared Service Library
extern MongoInterface *mongo;
extern RedisInterface *xRedis;
extern uuidInterface *ua;
extern Zmqio *zmqo;
extern Zmqio *zmqi;
extern CommandLineInterface *cli;
extern ApplicationResponseInterface *resp;

//Global Factory Objects
extern CommandLineInterpreterFactory *cli_factory;
extern MongoComponentFactory *mongo_factory;
extern RedisComponentFactory *redis_factory;
extern uuidComponentFactory *uuid_factory;
extern ZmqComponentFactory *zmq_factory;
extern LoggingComponentFactory *logging_factory;
extern ResponseFactory *response_factory;

//Shutdown the application
inline void shutdown()
{
  //Delete objects off the heap
  if (processor) {
    delete processor;
  }
  if (xRedis) {
    delete xRedis;
  }
  if (mongo) {
    delete mongo;
  }
  if (zmqo) {
    delete zmqo;
  }
  if (zmqi) {
    delete zmqi;
  }
  if (cm) {
    delete cm;
  }
  if (ua) {
    delete ua;
  }
  if (cli) {
    delete cli;
  }

  if(!resp) {main_logging->debug("No response object active at the time of shutdown");}
  else
  {
    delete resp;
  }

  if (!translated_object) {main_logging->debug("No translated object active at time of shutdown");}
  else {delete translated_object;}

  shutdown_logging_submodules();
  if (logging) {
    delete logging;
  }

  //Shut down protocol buffer library
  google::protobuf::ShutdownProtobufLibrary();

  if (cli_factory) {
    delete cli_factory;
  }
  if (mongo_factory) {
    delete mongo_factory;
  }
  if (redis_factory) {
    delete redis_factory;
  }
  if (uuid_factory) {
    delete uuid_factory;
  }
  if (zmq_factory) {
    delete zmq_factory;
  }
  if (logging_factory) {
    delete logging_factory;
  }
  if (response_factory) {
    delete response_factory;
  }
}

#endif
