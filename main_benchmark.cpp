#include <hayai/hayai.hpp>
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

#include "src/event_dispatcher.h"
#include "src/obj3.h"
#include "src/couchbase_admin.h"
#include "src/xredis_admin.h"
#include "src/lyman_utils.h"
#include "src/document_manager.h"
#include "src/configuration_manager.h"
#include "src/couchbase_callbacks.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "src/logging.h"
#include "src/globals.h"

enum {
  CACHE_TYPE_1,
  CACHE_TYPE_2,
  CACHE_TYPE_MAX,
};

Obj3 *obj;
std::string json_string;
std::string pb_string;

//----------------------------------------------------------------------------//
//------------------------------Benchmarks------------------------------------//
//----------------------------------------------------------------------------//

BENCHMARK(JSON, Save, 10, 100)
{
rapidjson::Document d;
d.Parse( json_string.c_str() );
dm->create_objectd( d );
}

BENCHMARK(JSON, Get, 10, 100)
{
rapidjson::Document d;
d.Parse( json_string.c_str() );
dm->get_objectd( json_string );
}

BENCHMARK(JSON, Update, 10, 100)
{
rapidjson::Document d;
d.Parse( json_string.c_str() );
dm->update_objectd(d);
}

BENCHMARK(Protobuffer, Save, 10, 100)
{
protoObj3::Obj3 new_proto;
new_proto.ParseFromString( pb_string.c_str() );
dm->create_objectpb( new_proto );
}

BENCHMARK(Protobuffer, Get, 10, 100)
{
protoObj3::Obj3 new_proto;
new_proto.ParseFromString( pb_string.c_str() );
dm->get_objectpb( new_proto );
}

BENCHMARK(Protobuffer, Update, 10, 100)
{
protoObj3::Obj3 new_proto;
new_proto.ParseFromString( pb_string.c_str() );
dm->update_objectpb(new_proto);
}

//-----------------------
//------Main Method------
//-----------------------

int main()
{

  //Set up our configuration manager
  cm = new ConfigurationManager ();

  //Set up logging
  //This reads the logging configuration file
  std::string initFileName = "log4cpp.properties";
  try {
    log4cpp::PropertyConfigurator::configure(initFileName);
  }
  catch ( log4cpp::ConfigureFailure &e ) {
    std::cout << "[log4cpp::ConfigureFailure] caught while reading" << initFileName << std::endl;
    std::cout << e.what();
    exit(1);
  }

  log4cpp::Category& root = log4cpp::Category::getRoot();

  log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("sub1"));

  log4cpp::Category& log = log4cpp::Category::getInstance(std::string("sub1.log"));

  logging = &log;

  //Read the application configuration file

  cm->configure("lyman.properties");

  //Set up internal variables
  int current_event_type;
  int msg_type;
  rapidjson::Document d;
  rapidjson::Value *s;
  char resp[8]={'n','i','l','r','e','s','p','\0'};
  logging->info("Internal Variables Intialized");
  protoObj3::Obj3 new_proto;

  //Set up our Redis Connection List
  std::vector<RedisConnChain> RedisConnectionList = cm->get_redisconnlist();
  int conn_list_size = RedisConnectionList.size();
  RedisNode RedisList1[conn_list_size];
  int y = 0;
  for (int y = 0; y < conn_list_size; ++y)
  {
    //Pull the values from RedisConnectionList
    RedisNode redis_n;
    redis_n.dbindex = y;
    RedisConnChain redis_chain = RedisConnectionList[y];
    redis_n.host = redis_chain.ip.c_str();
    redis_n.port = redis_chain.port;
    redis_n.passwd = redis_chain.elt4.c_str();
    redis_n.poolsize = redis_chain.elt5;
    redis_n.timeout = redis_chain.elt6;
    redis_n.role = redis_chain.elt7;
    logging->debug("Line added to Redis Configuration List with IP:");
    logging->debug(redis_n.host);

    RedisList1[y] = redis_n;
  }
  logging->info("Redis Connection List Built");

  //Set up Redis Connection
  if (cm->get_smartupdatesactive()) {
    xRedis = new xRedisAdmin (RedisList1, conn_list_size);
    logging->info("Connected to Redis");
  }

  //Set up the Couchbase Connection
  std::string DBConnStr = cm->get_dbconnstr();
  bool DBAuthActive = cm->get_dbauthactive();
  if (DBAuthActive) {
    std::string DBPswd = cm->get_dbpswd();
    cb = new CouchbaseAdmin ( DBConnStr.c_str(), DBPswd.c_str() );
  }
  else {
    cb = new CouchbaseAdmin ( DBConnStr.c_str() );
  }
  logging->info("Connected to Couchbase DB");

  //Bind Couchbase Callbacks
  lcb_set_store_callback(cb->get_instance(), storage_callback);
  lcb_set_get_callback(cb->get_instance(), get_callback);
  lcb_set_remove_callback(cb->get_instance(), del_callback);

  zmq::context_t context(1, 2);

  //Set up the outbound ZMQ Client
  //zmq::socket_t zout(context, ZMQ_REQ);
  zmqo = new zmq::socket_t (context, ZMQ_REQ);
  logging->info("0MQ Constructor Called");
  zmqo->connect(cm->get_obconnstr());
  logging->info("Connected to Outbound OMQ Socket");

  //Set up the Document Manager
  dm = new DocumentManager (cb, xRedis, cm);

  //Create an object
  std::string name;
  std::string key;
  std::string type;
  std::string subtype;
  std::string owner;

  name = "Test Object";
  key = "947fd3e2-3504-11e6-90f7-08002716b649";
  type = "Mesh";
  subtype = "Cube";
  owner = "zxywvut-1234567";

  //Set up an object with some base matrices
  Eigen::Vector3d new_location=Eigen::Vector3d::Zero(3);
  Eigen::Vector3d new_rotatione=Eigen::Vector3d::Zero(3);
  Eigen::Vector4d new_rotationq=Eigen::Vector4d::Zero(4);
  Eigen::Vector3d new_scale=Eigen::Vector3d::Zero(3);
  Eigen::Matrix4d new_transform=Eigen::Matrix4d::Zero(4, 4);
  Eigen::MatrixXd new_bounding_box=Eigen::MatrixXd::Zero(4, 8);

  //scale
  new_scale(0) = 1.0;
  new_scale(1) = 1.0;
  new_scale(2) = 1.0;

  //Transform and buffer
  new_transform(0, 0) = 1.0;
  new_transform(1, 1) = 1.0;
  new_transform(2, 2) = 1.0;
  new_transform(3, 3) = 1.0;

  //Bounding Box
  new_bounding_box(0, 1) = 1.0;
  new_bounding_box(1, 2) = 1.0;
  new_bounding_box(0, 3) = 1.0;
  new_bounding_box(1, 3) = 1.0;
  new_bounding_box(2, 4) = 1.0;
  new_bounding_box(0, 5) = 1.0;
  new_bounding_box(2, 5) = 1.0;
  new_bounding_box(1, 6) = 1.0;
  new_bounding_box(2, 6) = 1.0;
  new_bounding_box(0, 7) = 1.0;
  new_bounding_box(1, 7) = 1.0;
  new_bounding_box(2, 7) = 1.0;

  //Set up a scenes vector
  std::vector<std::string> scns;
  scns.push_back("12345");

  obj = new Obj3 (name, key, type, subtype, owner, scns, new_location, new_rotatione, new_rotationq, new_scale, new_transform, new_bounding_box);

  json_string = obj->to_json_msg(0);
  pb_string = obj->to_protobuf_msg(0);

  //------------------------------Run Tests-------------------------------------//
  //----------------------------------------------------------------------------//

  hayai::ConsoleOutputter consoleOutputter;

  hayai::Benchmarker::AddOutputter(consoleOutputter);
  hayai::Benchmarker::RunAllTests();

  //-------------------------Post-Test Teardown---------------------------------//
  //----------------------------------------------------------------------------//

  delete cm;
  delete cb;
  delete xRedis;
  delete dm;

  return 0;

}
