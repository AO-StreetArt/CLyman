#include "couchbase_admin.h"

CouchbaseAdmin::initialize (const char * conn)
{
	//Initializing
        printf("DB: Couchbase Admin Initializing");
        struct lcb_create_st cropts;
        cropts.version = 3; 
        cropts.v.v3.connstr = conn;

	//Add a password if authentication is active
	if (authentication_active == true) {
		cropts.v.v3.passwd = password;
	}

        lcb_error_t err;
        lcb_t instance;
        private_instance=instance;
        err = lcb_create(&private_instance, &cropts);
        if (err != LCB_SUCCESS) {
                printf("DB: Couldn't create instance!");
                exit(1);
        }

        //Connecting
        lcb_connect(private_instance);
        lcb_wait(private_instance);
        if ( (err = lcb_get_bootstrap_status(private_instance)) != LCB_SUCCESS ) {
                printf("DB: Couldn't bootstrap!");
                exit(1);
        }
}

CouchbaseAdmin::CouchbaseAdmin( const char * conn )
{
	authentication_active = false;
	initialize (conn);
}

CouchbaseAdmin::CouchbaseAdmin( const char * conn, const char * pswd )
{
        authentication_active = true;
	password = pswd;
	initialize (conn);
}

CouchbaseAdmin::~CouchbaseAdmin ()
{
	lcb_destroy(private_instance);
}

Obj3 CouchbaseAdmin::load_object ( const char * key )
{
	printf("DB: Object being loaded with key:");
	printf(key);
	//Initialize the variables
	lcb_get_cmd_t gcmd;
	const lcb_get_cmd_t *gcmdlist = &gcmd;
	gcmd.v.v0.key = key;
	gcmd.v.v0.nkey = strlen(key);

	//Schedule a Get operation
	err = lcb_get(private_instance, NULL, 1, &gcmdlist);
	if (err != LCB_SUCCESS) {
		printf("DB: Couldn't schedule get operation!");
		exit(1);
	}

	//This is a non-blocking call, wait can be called to wait until
	//the transaction stack is clear
}

void CouchbaseAdmin::save_object ( Obj3& obj )
{
	lcb_store_cmd_t scmd;
        const lcb_store_cmd_t *scmdlist = &scmd;
        std::string key = obj.get_key()
        scmd.v.v0.key = key.c_str();
        scmd.v.v0.nkey = key.length();
        const char * object_string = obj.to_json();
        scmd.v.v0.bytes = object_string;
        scmd.v.v0.nbytes = strlen(object_string);
        scmd.v.v0.operation = LCB_REPLACE;
        err = lcb_store(instance, NULL, 1, &scmdlist);
        if (err != LCB_SUCCESS) {
                printf("Couldn't schedule storage operation!\n");
                exit(1);
        }	
}

void CouchbaseAdmin::create_object ( Obj3& obj )
{
	lcb_store_cmd_t scmd;
	const lcb_store_cmd_t *scmdlist = &scmd;
	std::string key = obj.get_key()
	scmd.v.v0.key = key.c_str();
	scmd.v.v0.nkey = key.length();
	const char * object_string = obj.to_json();
	scmd.v.v0.bytes = object_string;
	scmd.v.v0.nbytes = strlen(object_string);
	scmd.v.v0.operation = LCB_SET;
	err = lcb_store(instance, NULL, 1, &scmdlist);
	if (err != LCB_SUCCESS) {
		printf("Couldn't schedule storage operation!\n");
		exit(1);
	}
}

void CouchbaseAdmin::delete_object ( const char * key )
	lcb_remove_cmd_t cmd;
	const lcb_remove_cmd_t *cmdlist = &cmd;
	cmd.v.v0.key = key;
	cmd.v.v0.nkey = strlen(key);
	err = lcb_remove(instance, NULL, 1, &cmdlist);
	if (err != LCB_SUCCESS) {
		printf("Couldn't schedule remove operation: %s\n", lcb_strerror(instance, err));
	} 
}

lcb_t CouchbaseAdmin::get_instance ()
{
	return private_instance;
}

void CouchbaseAdmin::wait ()
{
	printf("DB: Clear Function Stack Called");
	lcb_wait(private_instance);
}
