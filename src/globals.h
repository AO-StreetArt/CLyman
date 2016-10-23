//This defines some of the global variables that need to be accessed
//Across different files.  These are either numeric constants or
//singletons which form the backbone of the service.

#include "aossl/factory/couchbase_interface.h"
#include "aossl/factory/redis_interface.h"
#include "aossl/factory/uuid_interface.h"
#include "aossl/factory/commandline_interface.h"
#include "aossl/factory/zmq_interface.h"
#include "aossl/factory/response_interface.h"
#include "aossl/factory/logging_interface.h"

#include "aossl/factory_response.h"
#include "aossl/factory_cli.h"
#include "aossl/factory_couchbase.h"
#include "aossl/factory_logging.h"
#include "aossl/factory_redis.h"
#include "aossl/factory_uuid.h"
#include "aossl/factory_zmq.h"

#include "document_manager.h"
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
extern DocumentManager *dm;

//Globals from the AO Shared Service Library
extern CouchbaseInterface *cb;
extern RedisInterface *xRedis;
extern uuidInterface *ua;
extern Zmqio *zmqo;
extern Zmqio *zmqi;
extern CommandLineInterface *cli;
extern ApplicationResponseInterface *resp;

//Global Factory Objects
extern CommandLineInterpreterFactory *cli_factory;
extern CouchbaseComponentFactory *couchbase_factory;
extern RedisComponentFactory *redis_factory;
extern uuidComponentFactory *uuid_factory;
extern ZmqComponentFactory *zmq_factory;
extern LoggingComponentFactory *logging_factory;
extern ResponseFactory *response_factory;

//Shutdown the application
inline void shutdown()
{
  //Delete objects off the heap
  delete dm;
  delete xRedis;
  cb->shutdown_session();
  delete cb;
  delete zmqo;
  delete zmqi;
  delete cm;
  delete ua;
  delete cli;

  if(!resp) {main_logging->debug("No response object active at the time of shutdown");}
  else
  {
    delete resp;
  }

  if (!translated_object) {main_logging->debug("No translated object active at time of shutdown");}
  else {delete translated_object;}

  shutdown_logging_submodules();
  delete logging;

  //Shut down protocol buffer library
  google::protobuf::ShutdownProtobufLibrary();

  delete cli_factory;
  delete couchbase_factory;
  delete redis_factory;
  delete uuid_factory;
  delete zmq_factory;
  delete logging_factory;
  delete response_factory;
}

#endif
