#include <hayai/hayai.hpp>
#include <uuid/uuid.h>
#include "couchbase_admin.h"
#include "obj3.h"
#include <libcouchbase/couchbase.h>

Obj3 *obj;
CouchbaseAdmin *cb;
std::vector<std::string> uuid_list;
int savecounter = 0;
int getcounter = 0;
int delcounter = 0;
int loadcounter = 0;

static void storage_callback(lcb_t instance, const void *cookie, lcb_storage_t op,
   lcb_error_t err, const lcb_store_resp_t *resp)
{
  if (err == LCB_SUCCESS) {
    printf("Stored %.*s\n", (int)resp->v.v0.nkey, (char*)resp->v.v0.key);
  }
  else {
    fprintf(stderr, "Couldn't retrieve item: %s\n", lcb_strerror(instance, err));
  }
}

static void get_callback(lcb_t instance, const void *cookie, lcb_error_t err,
   const lcb_get_resp_t *resp)
{
  printf("Retrieved key %.*s\n", (int)resp->v.v0.nkey, (char*)resp->v.v0.key);
  printf("Value is %.*s\n", (int)resp->v.v0.nbytes, (char*)resp->v.v0.bytes);
}

static void del_callback(lcb_t instance, const void *cookie, lcb_error_t err, const lcb_remove_resp_t *resp)
{
  if (err == LCB_SUCCESS) {
    logging->info("Removed:");
    logging->info( (char*)resp->v.v0.key );
  }
  else {
    logging->error("Couldn't remove item:");
    logging->error(lcb_strerror(instance, err));
  }
}


//----------------------------------------------------------------------------//
//------------------------------Benchmarks------------------------------------//
//----------------------------------------------------------------------------//

BENCHMARK(Couchbase, Save, 10, 100)
{
  obj->set_key( uuid_list[savecounter] );

  cb->create_object ( obj );
  cb->wait();

  savecounter=savecounter+1;
}

BENCHMARK(Couchbase, Get, 10, 100)
{
  cb->load_object ( uuid_list[getcounter] );
  cb->wait();

  getcounter=getcounter+1;
}

BENCHMARK(Couchbase, Update, 10, 100)
{
  obj->set_key( uuid_list[loadcounter] );

  cb->save_object ( obj );
  cb->wait();

  loadcounter=loadcounter+1;
}

BENCHMARK(Couchbase, Delete, 10, 100)
{
  cb->delete_object ( uuid_list[delcounter] );
  cb->wait();

  delcounter=delcounter+1;
}

//----------------------------------------------------------------------------//
//------------------------------Main Method-----------------------------------//
//----------------------------------------------------------------------------//

int main()
{

//Application Setup

std::string initFileName = "log4cpp_test.properties";
try {
	log4cpp::PropertyConfigurator::configure(initFileName);
}
catch ( log4cpp::ConfigureFailure &e ) {
	printf("[log4cpp::ConfigureFailure] caught while reading Logging Configuration File");
	printf(e.what());
	exit(1);
}

log4cpp::Category& root = log4cpp::Category::getRoot();

log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("sub1"));

log4cpp::Category& log = log4cpp::Category::getInstance(std::string("sub1.log"));

logging = &log;

//Generate the UUID's for the benchmarks
int i=0;
int uuid_gen_result;
uuid_t uuid;
char uuid_str[37];
for (i=0; i< 1001; i++) {
  //Generate a new key for the object
  std::string uuid_str = std::to_string(i);
  uuid_list.push_back(uuid_str.c_str());
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
cb = new CouchbaseAdmin ("couchbase://localhost/default");

//Supports both password authentication and clustering
printf("Connected to Couchbase");
//Bind callbacks
lcb_set_store_callback(cb->get_instance(), storage_callback);
lcb_set_get_callback(cb->get_instance(), get_callback);
lcb_set_remove_callback(cb->get_instance(), del_callback);

//------------------------------Run Tests-------------------------------------//
//----------------------------------------------------------------------------//

hayai::ConsoleOutputter consoleOutputter;

hayai::Benchmarker::AddOutputter(consoleOutputter);
hayai::Benchmarker::RunAllTests();

//-------------------------Post-Test Teardown---------------------------------//
//----------------------------------------------------------------------------//

delete obj;
delete cb;

return 0;

}
