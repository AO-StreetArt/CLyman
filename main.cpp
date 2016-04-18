//This is the  main structural code of the module
//It is built to allow for real-time messaging without the need
//For polling, using a Dispatcher Pattern

#include <zmq.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "src/event_dispatcher.h"
#include "src/FastDelegate.h"
#include "src/lyman_utils.h"
#include "src/obj3.h"
#include "src/couchbase_admin.h"
#include "src/list.h"
#include "src/zmq_client.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "src/logging.h"

//Declare our global config variables
std::string DB_ConnStr;
bool DB_AuthActive;
std::string DB_Pswd;
std::string 0MQ_OBConnStr;
std::string 0MQ_IBConnStr;
bool SmartUpdatesActive;
int AUB_StartSize;
int AUB_EndSize;

//Global Object List
//Necessary to implement smart updates
List<Obj3> *active_updates;

//Global Couchbase Admin Object
CouchbaseAdmin *cb;

//Global Outbound ZMQ Dispatcher
ZMQClient *zmqo;

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

//TO-DO: Document Event Callbacks
void create_objectd(rapdjson::Document d) {
        logging->info("Create object called with document: ");
}

void update_objectd(rapidjson::Document d) {
        logging->info("Update object called with document: ");
}

void get_objectd(rapidjson::Document d) {
        logging->info("Get object called with document: ");
}

void delete_objectd(rapidjson::Document d) {
        logging->info("Delete object called with document: ");
}

//Main Method

int main()
{

//Set up logging
//This reads the logging configuration file
std::string initFileName = "log4cpp.properties";
try {
        log4cpp::PropertyConfigurator::configure(initFileName);     
}
catch ( log4cpp::ConfigureFailure &e ) {
        std::cout << "[log4cpp::ConfigureFailure] caught while reading" << initFileName << std::endl;
        std::cout << e.what();
        exit(1);
}

log4cpp::Category& root = log4cpp::Category::getRoot();

log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("sub1"));

log4cpp::Category& log = log4cpp::Category::getInstance(std::string("sub1.log"));

logging = &log;

//Read the application configuration file

//Default Values for Configuration Variables
DB_ConnStr="";
DB_AuthActive=false;
DB_Pswd="";
0MQ_OBConnStr="";
0MQ_IBConnStr="";
SmartUpdatesActive=false;
AUB_StartSize=25;
AUB_StepSize=15;

//Open the file
log->info("Opening lyman.properties");
std::string line;
ifstream file ("lyman.properties");

if (file.is_open()) {
	while (getline (file, line) ) {
		//Read a line from the property file
		log->debug("Line read from configuration file:");
		log->debug(line);

		//Figure out if we have a blank or comment line
		bool keep_going = true;
		if (line.length() > 0) {
			if (line[0] == '/' && line[1] == '/') {
				keep_going=false;
			}
		}
		else {
			keep_going=false;
		}

		if (keep_going==true) {
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
			else if (var_name=="SmartUpdatesActive") {
				if (var_value=="True") {
                                	SmartUpdatesActive=true;
                        	}
                        	else {
                                	SmartUpdatesActive=false;
                        	}
			}
			else if (var_name=="ActiveUpdateBuffer_StartSize") {
				stringstream(var_value) >> AUB_StartSize;
			}
			else if (var_name=="ActiveUpdateBuffer_StepSize") {
				stringstream(var_value) >> AUB_StepSize;
			}
		}
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

//Active Update List
List<Obj3> up_list (AUB_StartSize, AUB_StepSize);
active_updates = &up_list;

//Set up the Couchbase Connection
CouchbaseAdmin c ( DB_ConnStr );
cb = &c;
logging->info("Connected to Couchbase DB");

//Bind Couchbase Callbacks
lcb_set_store_callback(cb->get_instance(), storage_callback);
lcb_set_get_callback(cb->get_instance(), get_callback);

//Bind the document event dispatcher
doc_dispatch[OBJ_CRT].bind(&create_objectd);
doc_dispatch[OBJ_UPD].bind(&update_objectd);
doc_dispatch[OBJ_GET].bind(&get_objectd);
doc_dispatch[OBJ_DEL].bind(&delete_objectd);
logging->info("Local Event Functions bound");

//Set up the outbound ZMQ Client
ZMQClient zout (0MQ_OBConnStr);
zmqo = &zout;
logging->info("Connected to Outbound 0MQ Socket");

//Connect to the inbound ZMQ Socket
zmq::context_t context(1);
zmq::socket_t socket(context, ZMQ_REP);
socket.bind(0MQ_IBConnStr);
logging->info("ZMQ Socket Open, opening request loop");


//Main Request Loop


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
	logging->debug("Conversion to C String performed with result: ");
	logging->debug(req_ptr);
        
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
		doc_dispatch[OBJ_UPD]( d );
                resp[0]='s';
		resp[1]='u';
		resp[2]='c';
		resp[3]='c';
		resp[4]='e';
		resp[5]='s';
		resp[6]='s';
		logging->debug("Object Update Event Emitted, response: "
		logging->debug(resp);
        }
        else if (current_event_type==OBJ_CRT) {
		doc_dispatch[OBJ_CRT]( d );
		resp[0]='s';
                resp[1]='u';
                resp[2]='c';
                resp[3]='c';
                resp[4]='e';
                resp[5]='s';
                resp[6]='s';
		logging->debug("Object Create Event Emitted, response: ");
		logging->debug(resp);
        }
	else if (current_event_type==OBJ_GET) {
		doc_dispatch[OBJ_GET]( d );
		resp[0]='s';
                resp[1]='u';
                resp[2]='c';
                resp[3]='c';
                resp[4]='e';
                resp[5]='s';
                resp[6]='s';
		logging->debug("Object Get Event Emitted, response: ");
		logging->debug(resp);
        }
        else if (current_event_type==OBJ_DEL) {
		doc_dispatch[OBJ_DEL]( d );
		resp[0]='s';
                resp[1]='u';
                resp[2]='c';
                resp[3]='c';
                resp[4]='e';
                resp[5]='s';
                resp[6]='s';
		logging->debug("Object Delete Event Emitted, response: ");
		logging->debug(resp);
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
		logging->error("Object Event not Emitted, response: ");
		logging->error(resp);
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

