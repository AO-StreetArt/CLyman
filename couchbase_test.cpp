#include "couchbase_admin.h"
#include "obj3.h"
#include <libcouchbase/couchbase.h>

static void storage_callback(lcb_t instance, const void *cookie, lcb_storage_t op,
   lcb_error_t err, const lcb_store_resp_t *resp)
{
  printf("Stored %.*s\n", (int)resp->v.v0.nkey, (char*)resp->v.v0.key);
}

static void get_callback(lcb_t instance, const void *cookie, lcb_error_t err,
   const lcb_get_resp_t *resp)
{
  printf("Retrieved key %.*s\n", (int)resp->v.v0.nkey, (char*)resp->v.v0.key);
  printf("Value is %.*s\n", (int)resp->v.v0.nbytes, (char*)resp->v.v0.bytes);
}

int main ()
{
//Create an object
std::string name = "Test Object";
std::string key = "abcdef-9876543";
std::string type = "Mesh";
std::string subtype = "Cube";
std::string owner = "zxywvut-1234567";

Obj3 obj (name, key, type, subtype, owner);

//Build the Couchbase Admin (which will automatically connect to the DB)
CouchbaseAdmin cb ("couchbase://localhost/default");

//Supports both password authentication and clustering

//Bind callbacks
lcb_set_store_callback(cb.get_instance(), storage_callback);
lcb_set_get_callback(cb.get_instance(), get_callback);

//Write the object to the DB
cb.create_object ( obj );
cb.wait();

//Get the object from the DB
cb.load_object ( key.c_str() );
cb.wait();

//Update the object in the DB
obj.set_name ( "Weeee" );
cb.save_object ( obj );
cb.wait();

//Delete the object
cb.delete_object ( obj.get_key().c_str() );
cb.wait();

std::cout << "Object deleted" << std::endl;

return 0;
}
