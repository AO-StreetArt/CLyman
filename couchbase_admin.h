#include "db_admin.h"
#include <iostream>

#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"

extern "C"
{
	#include <libcouchbase/couchbase.h>
}

class CouchbaseAdmin: public DBAdmin
{
int private_key_length;
lcb_t private_instance;
log4cpp::Category& private_log;
public:
	//Constructor & Destructor
	CouchbaseAdmin ( const char * conn, int key_length, log4cpp::Category& log );
	~CouchbaseAdmin ();

	//Load/Save an object
	Obj3 load_object ( const char * key );
	void save_object ( Obj3& obj );

	//Get the instance, needed for binding callbacks
	lcb_t get_instance ();

	//Blocking call until the transaction stack is empty
	void wait ();

	//Methods to prevent duplication of singleton
	//intentionally deleted
	CouchBaseAdmin ( CouchbaseAdmin const& ) = delete;
	void operator = ( CouchbaseAdmin const& ) = delete;
}
