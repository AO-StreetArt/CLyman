#include <hayai/hayai.hpp>
#include <uuid/uuid.h>
#include "aossl/factory/couchbase_interface.h"
#include "aossl/factory.h"
#include "aossl/factory/logging_interface.h"
#include "../obj3.h"
#include <libcouchbase/couchbase.h>

Obj3 *obj;
CouchbaseInterface *cb;
std::vector<std::string> uuid_list;
int savecounter = 0;
int getcounter = 0;
int delcounter = 0;
int loadcounter = 0;

static void storage_callback(lcb_t instance, const void *cookie, lcb_storage_t op,
   lcb_error_t err, const lcb_store_resp_t *resp)
{
  if (err != LCB_SUCCESS) {
    fprintf(stderr, "Couldn't retrieve item: %s\n", lcb_strerror(instance, err));
  }
}

static void get_callback(lcb_t instance, const void *cookie, lcb_error_t err,
   const lcb_get_resp_t *resp)
{
  if (err != LCB_SUCCESS) {
    fprintf(stderr, "Couldn't retrieve item: %s\n", lcb_strerror(instance, err));
  }
}

static void del_callback(lcb_t instance, const void *cookie, lcb_error_t err, const lcb_remove_resp_t *resp)
{
  if (err != LCB_SUCCESS) {
    fprintf(stderr, "Couldn't retrieve item: %s\n", lcb_strerror(instance, err));
  }
}


//----------------------------------------------------------------------------//
//------------------------------Benchmarks------------------------------------//
//----------------------------------------------------------------------------//

BENCHMARK(Couchbase, Save, 10, 100)
{
  std::string uuid_str = uuid_list[savecounter];
  obj->set_key( uuid_str );

  cb->create_object ( obj );
  cb->wait();

  savecounter=savecounter+1;
}

BENCHMARK(Couchbase, Get, 10, 100)
{
  std::string uuid_str = uuid_list[getcounter];
  cb->load_object ( uuid_str.c_str() );
  cb->wait();

  getcounter=getcounter+1;
}

BENCHMARK(Couchbase, Update, 10, 100)
{
  std::string uuid_str = uuid_list[loadcounter];
  obj->set_key( uuid_str );

  cb->save_object ( obj );
  cb->wait();

  loadcounter=loadcounter+1;
}

BENCHMARK(Couchbase, Delete, 10, 100)
{
  std::string uuid_str = uuid_list[delcounter];
  cb->delete_object ( uuid_str.c_str() );
  cb->wait();

  delcounter=delcounter+1;
}

//----------------------------------------------------------------------------//
//------------------------------Main Method-----------------------------------//
//----------------------------------------------------------------------------//

int main()
{

//Application Setup

ServiceComponentFactory *factory = new ServiceComponentFactory;

//-------------------------------Logging--------------------------------------//
//----------------------------------------------------------------------------//

std::string initFileName = "src/test/log4cpp_test.properties";
logging = factory->get_logging_interface(initFileName);

//Generate the UUID's for the benchmarks
int i=0;
int uuid_gen_result;
uuid_t uuid;
for (i=0; i< 1001; i++) {
  //Generate a new key for the object
  std::string uuid_str = std::to_string(i);
  uuid_list.push_back(uuid_str);
}

//Generate an object to use for the benchmarks

std::string name;
std::string key;
std::string type;
std::string subtype;
std::string owner;

name = "Test Object";
key = "abcdef-9876543";
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

//Set up the Couchbase Admin
//Build the Couchbase Admin (which will automatically connect to the DB)
cb = factory->get_couchbase_interface( "couchbase://localhost/default" );

//Supports both password authentication and clustering
printf("Connected to Couchbase");
//Bind callbacks
cb->bind_storage_callback(storage_callback);
cb->bind_get_callback(get_callback);
cb->bind_delete_callback(del_callback);

//------------------------------Run Tests-------------------------------------//
//----------------------------------------------------------------------------//

hayai::ConsoleOutputter consoleOutputter;

hayai::Benchmarker::AddOutputter(consoleOutputter);
hayai::Benchmarker::RunAllTests();

//-------------------------Post-Test Teardown---------------------------------//
//----------------------------------------------------------------------------//

delete obj;
delete cb;
delete logging;
delete factory;

return 0;

}
