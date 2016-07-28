#include "globals.h"

//Declare our global variables

//UUID Generator
uuidAdmin *ua;

//Configuration Manager
ConfigurationManager *cm;

//Global Couchbase Admin Object
CouchbaseAdmin *cb;

//Smart Update Buffer
xRedisAdmin *xRedis;

//Document Manager, highest level object
DocumentManager *dm;

//Command Line interpreter
CommandLineInterpreter *cli;

//ZMQ Admins
Zmqo *zmqo;
Zmqi *zmqi;
