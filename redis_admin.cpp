#include "redis_admin.h"

//Initialization
RedisAdmin::RedisAdmin ( const * char conn, int port )
{
//Set the connection & port info
connect=conn;
port_number=port;

//Return 1 if the connection is not made, 0 if it is
if(!rdx.connect(connect, port_number))
{
  return 1;
}
else
{
  return 0;
}
}

Obj3 RedisAdmin::load_object ( const char * key )
{

rapidjson::Document d;
rapidjson::Value& s;

//Load a json string
const char * string_ptr = rdx.get(key);

//Parse the json string
d.parse ( string_ptr );
s = d["name"];


//Build the object
Obj3 obj();
}

void RedisAdmin::save_object ( Obj3& obj )
{

}
