#include "db_admin.h"

class CouchbaseAdmin: public DBAdmin
{
public:
	Obj3 load_object ( const char * key );
	void save_object ( Obj3& obj );
}
