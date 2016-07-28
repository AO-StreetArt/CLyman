//This defines some of the global variables that need to be accessed
//Across different files.  These are either numeric constants or
//singletons which form the backbone of the service.

#include <aossl/couchbase_admin.h>
#include <aossl/xredis_admin.h>
#include <aossl/uuid_admin.h>
#include <aossl/zmqio.h>

#include "document_manager.h"
#include "configuration_manager.h"

#ifndef GLOBALS
#define GLOBALS

extern ConfigurationManager *cm;
extern CouchbaseAdmin *cb;
extern xRedisAdmin *xRedis;
extern DocumentManager *dm;
extern uuidAdmin *ua;
extern Zmqo *zmqo;
extern Zmqi *zmqi;
extern CommandLineInterpreter *cli;

#endif
