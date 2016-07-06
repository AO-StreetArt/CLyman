//This defines some of the global variables that need to be accessed
//Across different files.  These are either numeric constants or
//singletons which form the backbone of the service.

#include "couchbase_admin.h"
#include "xredis_admin.h"
#include "document_manager.h"
#include "configuration_manager.h"

extern ConfigurationManager *cm;
extern CouchbaseAdmin *cb;
extern xRedisAdmin *xRedis;
extern DocumentManager *dm;
