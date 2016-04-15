//This is the DB Admin Interface
//This defines the methods that must
//Be implemented in order to add support
//for a particular DB

#include "obj3.h"

class DBAdmin
{
public:
	DBAdmin() {}
	~DBAdmin() {}
	//Basic CRUD Operations
	virtual Obj3 load_object ( const char * key ) = 0;
	virtual void save_object ( Obj3& obj ) = 0;
	virtual void create_object (Obj3& obj ) = 0;
	virtual void delete_object (const char * key) = 0;

	//If the engine is asynchronous, wait for the
	//active threads to complete.  Otherwise, do nothing
	virtual void wait () = 0;

	//Methods to prevent duplication of singleton
        //intentionally deleted
};
