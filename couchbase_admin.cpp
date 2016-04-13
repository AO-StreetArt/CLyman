#include "couchbase_admin.h"

CouchbaseAdmin::CouchbaseAdmin( const char * conn, int key_length, log4cpp::Category& log )
{
	private_key_length = key_length;
	private_log = log;
	
	//Initializing
	private_log.info("DB: Couchbase Admin Initializing");
	struct lcb_create_st cropts;
	cropts.version = 3;
	cropts.v.v3.connstr = conn;
	lcb_error_t err;
	lcb_t instance;
	private_instance=instance;
	err = lcb_create(&private_instance, &cropts);
	if (err != LCB_SUCCESS) {
		private_log.error("DB: Couldn't create instance!");
		exit(1);
  	}

	//Connecting
	lcb_connect(private_instance);
	lcb_wait(private_instance);
	if ( (err = lcb_get_bootstrap_status(private_instance)) != LCB_SUCCESS ) {
		private_log.error("DB: Couldn't bootstrap!");
		exit(1);
	}
}

CouchbaseAdmin::~CouchbaseAdmin ()
{
	lcb_destroy(private_instance);
}

Obj3 CouchbaseAdmin::load_object ( const char * key )
{
	private_log.info("DB: Object being loaded with key:");
	private_log.info(key);
	//Initialize the variables
	lcb_get_cmd_t gcmd;
	const lcb_get_cmd_t *gcmdlist = &gcmd;
	gcmd.v.v0.key = key;
	gcmd.v.v0.nkey = private_key_length;

	//Schedule a Get operation
	err = lcb_get(private_instance, NULL, 1, &gcmdlist);
	if (err != LCB_SUCCESS) {
		private_log.error("DB: Couldn't schedule get operation!");
		exit(1);
	}

	//This is a non-blocking call, wait can be called to wait until
	//the transaction stack is clear
}

void CouchbaseAdmin::save_object ( Obj3& obj )
{
	//TO-DO
}

lcb_t CouchbaseAdmin::get_instance ()
{
	return private_instance;
}

void CouchbaseAdmin::wait ()
{
	private_log.debug("DB: Clear Function Stack Called");
	lcb_wait(private_instance);
}
