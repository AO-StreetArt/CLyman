#include "globals.h"

//Declare our global variables

//UUID Generator
uuidInterface *ua;

//Configuration Manager
ConfigurationManager *cm;

//Global Couchbase Admin Object
CouchbaseInterface *cb;

//Smart Update Buffer
RedisInterface *xRedis;

//Document Manager, highest level object
DocumentManager *dm;

//Command Line interpreter
CommandLineInterface *cli;

//ZMQ Admins
Zmqio *zmqo;
Zmqio *zmqi;
