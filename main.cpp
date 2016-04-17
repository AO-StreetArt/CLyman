//This is the  main structural code of the module
//It is built to allow for real-time messaging without the need
//For polling, using a Dispatcher Pattern

#include <zmq.hpp>
#include <string>
#include <iostream>

#include "event_dispatcher.h"
#include "FastDelegate.h"
#include "lyman_utils.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "logging.h"

#include <fstream>

#include "couchbase_admin.h"
#include "zmq_client.h"

//TO-DO:Couchbase Callbacks
static void storage_callback(lcb_t instance, const void *cookie, lcb_storage_t op,
   lcb_error_t err, const lcb_store_resp_t *resp)
{
	if (err == LCB_SUCCESS) { 
		logging->info("Stored:");
		logging->info( (char*)resp->v.v0.key );
	}
	else {
		logging->error("Couldn't store item:"); 
		logging->error(lcb_strerror(instance, err));
	}
}

static void get_callback(lcb_t instance, const void *cookie, lcb_error_t err,
   const lcb_get_resp_t *resp)
{
	if (err == LCB_SUCCESS) {
		logging->info("Retrieved: ");
		logging->info( (char*)resp->v.v0.key );
		logging->info( (char*)resp->v.v0.bytes );
	}
	else {
		logging->error("Couldn't retrieve item:");
        	logging->error(lcb_strerror(instance, err));
	}
}

//TO-DO:Event Callbacks

void create_object(const char *str) {
	logging->info("Create object called with string: ");
}

void update_object(const char *str) {
	logging->info("Update object called with string: ");
}

void get_object(const char *str) {
	logging->info("Get object called with string: ");
}

void delete_object(const char *str) {
	logging->info("Delete object called with string: ");
}

//Main Method

int main()
{

//Set up logging
//This reads the logging configuration file
init_log();

//Read the application configuration file

//Declare Variables to store the values from the file
std::string DB_ConnStr;
bool DB_AuthActive;
std::string DB_Pswd;
std::string 0MQ_OBConnStr;
std::string 0MQ_IBConnStr;

//Open the file
log->info("Opening lyman.properties");
std::string line;
ifstream file ("lyman.properties");

if (file.is_open()) {
	while (getline (file, line) ) {
		//Read a line from the property file
		log->debug("Line read from configuration file:");
		log->debug(line);
		int eq_pos = line.find("=", 0);
		std::string var_name = line.substr(0, eq_pos);
		std::string var_value = line.substr(eq_pos, line.length() - eq_pos);
		log->debug(var_name);
		log->debug(var_value);
		if (var_name=="DB_ConnectionString") {
			DB_ConnStr=var_value;
		}
		else if (var_name=="DB_AuthenticationActive") {
			if (var_value=="True") {
				DB_AuthActive=true;
			}
			else {
				DB_AuthActive=false;
			}
		}
		else if (var_name=="DB_Password") {
			DB_Pswd=var_value;
		}
		else if (var_name=="0MQ_OutboundConnectionString") {
			0MQ_OBConnStr = var_value;
		}
		else if (var_name=="0MQ_InboundConnectionString") {
			0MQ_IBConnStr = var_value;
		}
	file.close();
}

//Set up internal variables
int current_event_type;
int msg_type;
rapidjson::Document d;
rapidjson::Value& s;
char resp[8]={'n','i','l','r','e','s','p','\0'};
logging->info("Internal Variables Intialized");

//Set up the Couchbase Connection
CouchbaseAdmin cb ( DB_ConnStr );
logging->info("Connected to Couchbase DB");

//Bind Couchbase Callbacks
lcb_set_store_callback(cb.get_instance(), storage_callback);
lcb_set_get_callback(cb.get_instance(), get_callback);

//Set up the Event Dispatcher
ObjectDelegate dispatch[12];
logging->info("Event Dispatcher Intialized");

//Bind the local functions
dispatch[OBJ_CRT].bind(&create_object);
dispatch[OBJ_UPD].bind(&update_object);
dispatch[OBJ_GET].bind(&get_object);
dispatch[OBJ_DEL].bind(&delete_object);
logging->info("Local Event Functions bound");

//Set up the outbound ZMQ Client
//By setting it up this way, we ensure that we can
//call it from within the callbacks
init_zmqo (0MQ_OBConnStr);
logging->info("Connected to Outbound 0MQ Socket");

//Connect to the inbound ZMQ Socket
zmq::context_t context(1);
zmq::socket_t socket(context, ZMQ_REP);
socket.bind(0MQ_IBConnStr);
logging->info("ZMQ Socket Open, opening request loop");

while (true) {
        zmq::message_t request;

        //  Wait for next request from client
        socket.recv (&request);
	logging->info("Request Recieved");

        //Convert the 0MQ message into a string to be passed on the event
        std::string req_string;
        req_string = hexDump (request);
	req_string.erase(0,58);
        const char * req_ptr = req_string.c_str();
	logging->debug("Conversion to C String performed with result: %s", req_ptr);
        
	//Process the message header and set current_event_type

	d.parse(req_ptr);
	s = d["message_type"];
	msg_type = s.GetInt();

        if (msg_type == OBJ_UPD) {
                current_event_type=OBJ_UPD;
		logging->debug("Current Event Type set to Object Update");
        }
        else if (msg_type == OBJ_CRT) {
                current_event_type=OBJ_CRT;
		logging->debug("Current Event Type set to Object Create");
        }
        else if (msg_type == OBJ_GET) {
                current_event_type=OBJ_GET;
		logging->debug("Current Event Type set to Object Get");
        }
	else if (msg_type == OBJ_DEL) {
                current_event_type=OBJ_DEL;
		logging->debug("Current Event Type set to Object Delete");
        }
        else {
                current_event_type=-1;
		logging->error("Current Event Type not found");
        }


        //Emit an event based on the event type & build the response message
        if (current_event_type==OBJ_UPD) {
                dispatch[OBJ_UPD]( req_ptr );
                resp[0]='s';
		resp[1]='u';
		resp[2]='c';
		resp[3]='c';
		resp[4]='e';
		resp[5]='s';
		resp[6]='s';
		logging->debug("Object Update Event Emitted, response: %s", resp);
        }
        else if (current_event_type==OBJ_CRT) {
                dispatch[OBJ_CRT]( req_ptr );
		resp[0]='s';
                resp[1]='u';
                resp[2]='c';
                resp[3]='c';
                resp[4]='e';
                resp[5]='s';
                resp[6]='s';
		logging->debug("Object Create Event Emitted, response: %s", resp);
        }
	else if (current_event_type==OBJ_GET) {
                dispatch[OBJ_GET]( req_ptr );
		resp[0]='s';
                resp[1]='u';
                resp[2]='c';
                resp[3]='c';
                resp[4]='e';
                resp[5]='s';
                resp[6]='s';
		logging->debug("Object Get Event Emitted, response: %s", resp);
        }
        else if (current_event_type==OBJ_DEL) {
                dispatch[OBJ_DEL]( req_ptr );
		resp[0]='s';
                resp[1]='u';
                resp[2]='c';
                resp[3]='c';
                resp[4]='e';
                resp[5]='s';
                resp[6]='s';
		logging->debug("Object Delete Event Emitted, response: %s", resp);
        }
        else
        {
		resp[0]='f';
                resp[1]='a';
                resp[2]='i';
                resp[3]='l';
                resp[4]='u';
                resp[5]='r';
                resp[6]='e';
		logging->error("Object Event not Emitted, response: %s", resp);
        }

        //  Send reply back to client
        zmq::message_t reply (8);

        //Prepare return data
        memcpy (reply.data (), resp, 8);

	//Send the response
        socket.send (reply);
	logging->debug("Response Sent");
        }
end_log();
return 0;
}

