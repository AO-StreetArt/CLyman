#include "globals.h"

//Declare our global variables

//UUID Generator
uuidInterface *ua = NULL;

//Configuration Manager
ConfigurationManager *cm = NULL;

//Global Couchbase Admin Object
CouchbaseInterface *cb = NULL;

//Smart Update Buffer
RedisInterface *xRedis = NULL;

//Document Manager, highest level object
DocumentManager *dm = NULL;

//Command Line interpreter
CommandLineInterface *cli = NULL;

//ZMQ Admins
Zmqio *zmqo = NULL;
Zmqio *zmqi = NULL;

//Response Object
ApplicationResponseInterface *resp = NULL;
