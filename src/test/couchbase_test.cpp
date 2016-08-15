#include "../obj3.h"
#include <libcouchbase/couchbase.h>

#include "aossl/factory/couchbase_interface.h"
#include "aossl/factory.h"
#include "aossl/factory/logging_interface.h"

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

int main ()
{

  ServiceComponentFactory *factory = new ServiceComponentFactory;

  //-------------------------------Logging--------------------------------------//
  //----------------------------------------------------------------------------//

  std::string initFileName = "src/test/log4cpp_test.properties";
  logging = factory->get_logging_interface(initFileName);

//Create an object
std::string name = "Test Object";
std::string key = "abcdef-9876542";
std::string type = "Mesh";
std::string subtype = "Cube";
std::string owner = "zxywvut-1234567";

Obj3 obj (name, key, type, subtype, owner);
obj.add_scene("1");

//Build the Couchbase Admin (which will automatically connect to the DB)
CouchbaseInterface *cb = factory.get_couchbase_interface("couchbase://localhost/default");

//Supports both password authentication and clustering
printf("Connected to Couchbase");
//Bind callbacks
cb->bind_storage_callback(storage_callback);
cb->bind_get_callback(get_callback);
printf("Callbacks bound");
//Write the object to the DB
Obj3 *obj_ptr = &obj;
cb->create_object ( obj_ptr );
cb->wait();
printf("Create Object Tested");
//Get the object from the DB
const char* obj_key = obj.get_key().c_str();
cb->load_object ( obj_key );
cb->wait();
printf("Load Object Tested");
//Update the object in the DB
obj.set_name ( "Weeee" );
cb->save_object ( &obj );
cb->wait();
printf("Save Object Tested");
//Delete the object
cb->delete_object ( obj.get_key().c_str() );
cb->wait();
printf("Delete Object Tested");

delete cb;
delete logging;
delete factory;

return 0;
}
