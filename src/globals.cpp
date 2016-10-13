#include "globals.h"

//Declare our global variables

//Set up an Obj3 pointer to hold the currently translated document information
Obj3 *translated_object = NULL;

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

//Factories
CommandLineInterpreterFactory *cli_factory = NULL;
CouchbaseComponentFactory *couchbase_factory = NULL;
RedisComponentFactory *redis_factory = NULL;
uuidComponentFactory *uuid_factory = NULL;
ZmqComponentFactory *zmq_factory = NULL;
LoggingComponentFactory *logging_factory = NULL;
ResponseFactory *response_factory = NULL;
