//Document Driver
//Contains CRUD actions for Objects
//Highest level object, driven by input (zmq) and drives all other functionality.

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

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "aossl/factory/couchbase_interface.h"
#include "aossl/factory/redis_interface.h"
#include "aossl/factory/uuid_interface.h"
#include "aossl/factory/commandline_interface.h"
#include "aossl/factory/zmq_interface.h"

#include "obj3.h"
#include "configuration_manager.h"
#include "lyman_log.h"

#ifndef DOCUMENT_ADMIN
#define DOCUMENT_ADMIN

class DocumentManager
{
CouchbaseInterface *cb;
RedisInterface *xRedis;
ConfigurationManager *cm;
uuidInterface *ua;
Zmqio *zmqo;
public:
  //Initializer
  DocumentManager(CouchbaseInterface *cb_admin, RedisInterface *xr_admin, uuidInterface *uadmin, ConfigurationManager *cm_admin, Zmqio *zmq_out) {cb = cb_admin; xRedis = xr_admin; cm = cm_admin;ua = uadmin; zmqo = zmq_out;}
  DocumentManager(CouchbaseInterface *cb_admin, RedisInterface *xr_admin, uuidInterface *uadmin, Zmqio *zmq_out) {cb = cb_admin; xRedis = xr_admin;ua = uadmin;zmqo = zmq_out;}

  //configure
  void configure(ConfigurationManager *cm_admin) {cm = cm_admin;}

  //Put an obj to redis
  void put_to_redis(Obj3 *temp_obj, int msg_type, std::string transaction_id);

  //CRUD Exposures for Obj3
  std::string create_object(Obj3 *new_obj, std::string transaction_id);
  std::string update_object(Obj3 *temp_obj, std::string transaction_id);
  std::string get_object(Obj3 *temp_obj, std::string transaction_id);
  std::string delete_object(Obj3 *temp_obj, std::string transaction_id);

  //Wait for couchbase threads to finish
  void wait() {cb->wait();}
};

#endif
