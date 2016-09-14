//This defines some of the global variables that need to be accessed
//Across different files.  These are either numeric constants or
//singletons which form the backbone of the service.

#include "aossl/factory/couchbase_interface.h"
#include "aossl/factory/redis_interface.h"
#include "aossl/factory/uuid_interface.h"
#include "aossl/factory/commandline_interface.h"
#include "aossl/factory/zmq_interface.h"

#include "document_manager.h"
#include "configuration_manager.h"

#include <unordered_map>

#ifndef GLOBALS
#define GLOBALS

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

#endif
