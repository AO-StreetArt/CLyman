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
std::string cr_obj_global(Obj3 *new_obj, std::string transaction_id);
std::string upd_obj_global(Obj3 *temp_obj, std::string transaction_id);
void get_obj_global(std::string rk_str, std::string transaction_id, Obj3 *new_obj);
void del_obj_global(std::string key, std::string transaction_id, Obj3 *new_obj);
void put_to_redis(Obj3 *temp_obj, int msg_type, std::string transaction_id);
public:
  //Initializer
  DocumentManager(CouchbaseInterface *cb_admin, RedisInterface *xr_admin, uuidInterface *uadmin, ConfigurationManager *cm_admin, Zmqio *zmq_out) {cb = cb_admin; xRedis = xr_admin; cm = cm_admin;ua = uadmin; zmqo = zmq_out;}
  DocumentManager(CouchbaseInterface *cb_admin, RedisInterface *xr_admin, uuidInterface *uadmin, Zmqio *zmq_out) {cb = cb_admin; xRedis = xr_admin;ua = uadmin;zmqo = zmq_out;}

  //configure
  void configure(ConfigurationManager *cm_admin) {cm = cm_admin;}

  //Create Object called with a Rapidjson Document
  std::string create_objectd(rapidjson::Document& d, std::string transaction_id, Obj3 *new_obj);
  //Create Object from a Protobuffer object
  std::string create_objectpb(protoObj3::Obj3 p_obj, std::string transaction_id, Obj3 *new_obj);

  //Update Object called with a Rapidjson Document
  std::string update_objectd(rapidjson::Document& d, std::string transaction_id, Obj3 *new_obj);
  //Update Object called with a Protobuffer object
  std::string update_objectpb(protoObj3::Obj3 p_obj, std::string transaction_id, Obj3 *new_obj);

  //Get object Protobuffer
  std::string get_objectpb(protoObj3::Obj3 p_obj, std::string transaction_id, Obj3 *new_obj);
  //Get Object Rapidson Document
  std::string get_objectd(rapidjson::Document& d, std::string transaction_id, Obj3 *new_obj);

  //Delete Object Protobuffer
  std::string delete_objectpb(protoObj3::Obj3 p_obj, std::string transaction_id, Obj3 *new_obj);
  //Delete Object Rapidjson Document
  std::string delete_objectd(rapidjson::Document& d, std::string transaction_id, Obj3 *new_obj);

  //Wait for couchbase threads to finish
  void wait() {cb->wait();}
};

#endif
