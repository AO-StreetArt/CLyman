#include "db_admin.h"
#include <redox.hpp>
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

class RedisAdmin: public DBAdmin
{
std::string connect;
int port_number;
redox::Redox rdx;
public:
	RedisAdmin(std::string conn, int port);
	Obj3 load_object ( const char * key );
	void save_object ( Obj3& obj );
}
