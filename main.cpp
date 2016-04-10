//This is the  main structural code of the module
//It is built to allow for real-time messaging without the need
//For polling, using a Dispatcher Pattern

#include <zmq.hpp>
#include <string>
#include <iostream>

#include "event_dispatcher.h"
#include "FastDelegate.h"
#include "lyman_utils.h"

#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

//Events

void create_object(const char *str) {
	std::cout << "Create object called with string: " << str << std::endl;
}

void update_object(const char *str) {
	std::cout << "Update object called with string: " << str << std::endl;
}

void get_object(const char *str) {
	std::cout << "Get object called with string: " << str << std::endl;
}

void delete_object(const char *str) {
	std::cout << "Delete object called with string: " << str << std::endl;
}

//Main Method

int main()
{

//Set up logging
log4cpp::Appender *appender = new log4cpp::FileAppender("default", "program.log");
appender->setLayout(new log4cpp::BasicLayout());

log4cpp::Category& log = log4cpp::Category::getRoot();
log.setPriority(log4cpp::Priority::DEBUG);
log.addAppender(appender);

//Set up internal variables
int current_event_type;
int msg_type;
rapidjson::Document d;
rapidjson::Value& s;
char resp[8]={'n','i','l','r','e','s','p','\0'};
log.info("Internal Variables Intialized");

//Set up the Event Dispatcher
ObjectDelegate dispatch[12];
log.info("Event Dispatcher Intialized");

//Bind the local functions
dispatch[OBJ_CRT].bind(&create_object);
dispatch[OBJ_UPD].bind(&update_object);
dispatch[OBJ_GET].bind(&get_object);
dispatch[OBJ_DEL].bind(&delete_object);
log.info("Local Event Functions bound");

//Connect to the ZMQ Socket
zmq::context_t context(1);
zmq::socket_t socket(context, ZMQ_REP);
socket.bind("tcp://*:5555");
log.info("ZMQ Socket Open, opening request loop");

while (true) {
        zmq::message_t request;

        //  Wait for next request from client
        socket.recv (&request);
	log.info("Request Recieved");

        //Convert the 0MQ message into a string to be passed on the event
        std::string req_string;
        req_string = hexDump (request);
	req_string.erase(0,58);
        const char * req_ptr = req_string.c_str();
	log.debug("Conversion to C String performed with result: %s", req_ptr);
        
	//Process the message header and set current_event_type

	d.parse(req_ptr);
	s = d["message_type"];
	msg_type = s.GetInt();

        if (msg_type == OBJ_UPD) {
                current_event_type=OBJ_UPD;
		log.debug("Current Event Type set to Object Update");
        }
        else if (msg_type == OBJ_CRT) {
                current_event_type=OBJ_CRT;
		log.debug("Current Event Type set to Object Create");
        }
        else if (msg_type == OBJ_GET) {
                current_event_type=OBJ_GET;
		log.debug("Current Event Type set to Object Get");
        }
	else if (msg_type == OBJ_DEL) {
                current_event_type=OBJ_DEL;
		log.debug("Current Event Type set to Object Delete");
        }
        else {
                current_event_type=-1;
		log.error("Current Event Type not found");
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
		log.debug("Object Update Event Emitted, response: %s", resp);
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
		log.debug("Object Create Event Emitted, response: %s", resp);
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
		log.debug("Object Get Event Emitted, response: %s", resp);
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
		log.debug("Object Delete Event Emitted, response: %s", resp);
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
		log.error("Object Event not Emitted, response: %s", resp);
        }

        //  Send reply back to client
        zmq::message_t reply (8);

        //Prepare return data
        memcpy (reply.data (), resp, 8);

	//Send the response
        socket.send (reply);
	log.debug("Response Sent");
        }
return 0;
}

