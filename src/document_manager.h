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

#include "couchbase_admin.h"
#include "xredis_admin.h"
#include "obj3.h"
#include "configuration_manager.h"

#include "logging.h"
#include "zmqo.h"

#ifndef COUCHBASE_ADMIN
#define COUCHBASE_ADMIN

class DocumentManager
{
CouchbaseAdmin *cb;
xRedisAdmin *xRedis;
ConfigurationManager *cm;
void cr_obj_global(Obj3 *new_obj);
void upd_obj_global(Obj3 *temp_obj);
void get_obj_global(std::string rk_str);
void del_obj_global(std::string key);
public:
  //Initializer
  DocumentManager(CouchbaseAdmin *cb_admin, xRedisAdmin *xr_admin, ConfigurationManager *cm_admin) {cb = cb_admin; xRedis = xr_admin; cm = cm__admin;}
  DocumentManager(CouchbaseAdmin *cb_admin, xRedisAdmin *xr_admin) {cb = cb_admin; xRedis = xr_admin;}

  //configure
  void configure(ConfigurationManager *cm_admin) {cm = cm_admin;}

  //Create Object called with a Rapidjson Document
  void create_objectd(rapidjson::Document& d);
  //Create Object from a Protobuffer object
  void create_objectpb(protoObj3::Obj3 p_obj);

  //Update Object called with a Rapidjson Document
  void update_objectd(rapidjson::Document& d);
  //Update Object called with a Protobuffer object
  void update_objectpb(protoObj3::Obj3 p_obj);

  //Get object Protobuffer
  void get_objectpb(protoObj3::Obj3 p_obj);
  //Get Object Rapidson Document
  void get_objectd(rapidjson::Document& d);

  //Delete Object Protobuffer
  void delete_objectpb(protoObj3::Obj3 p_obj);
  //Delete Object Rapidjson Document
  void delete_objectd(rapidjson::Document& d);
}

#endif
