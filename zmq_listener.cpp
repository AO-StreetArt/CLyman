#include "zmq_listener.h"

void ZMQListener::listen(ObjectDelegate (&dispatch)[12], std::string conn)
{
int current_event_type;

int response_size;
//TO-DO: Determine what this number should be
response_size=7;
std::string resp;

//Connect to the ZMQ Socket
zmq::context_t context(1);
zmq::socket_t socket(context, ZMQ_REP);
socket.bind(conn);

while (true) {
	zmq::message_t request;

	//  Wait for next request from client
	socket.recv (&request);

	//Convert the 0MQ message into a string to be passed on the event
	std::string req_string;
	req_string = hexDump (request);
	const char * req_ptr = req_string.c_str();

	//TO-DO: Process the message header and set current_event_type
	if (req_string=="OBJ_UPD") {
		current_event_type=OBJ_UPD;
	}
	else if (req_string=="OBJ_CRT") {
		current_event_type=OBJ_CRT;
	}
	else if (req_string=="OBJ_GET") {
		current_event_type=OBJ_GET;
	}
	else if (req_string=="OBJ_DEL") {
		current_event_type=OBJ_DEL;
	}
	else {
		current_event_type=-1;
	}


	//Emit an event based on the event type
	if (current_event_type==OBJ_UPD) {
		dispatch[OBJ_UPD]( req_ptr );
		resp = "Success";
	}
	else if (current_event_type==OBJ_CRT) {
		dispatch[OBJ_CRT]( req_ptr );
		resp = "Success";
	}
	else if (current_event_type==OBJ_GET) {
		dispatch[OBJ_GET]( req_ptr );
		resp = "Success";
        }
	else if (current_event_type==OBJ_DEL) {
		dispatch[OBJ_DEL]( req_ptr );
		resp = "Success";
        }
	else
	{
		resp = "Failure";
	}

	//  Send reply back to client
	zmq::message_t reply (response_size);

	//Prepare return data
	memcpy (reply.data (), resp, response_size);

	//Send the response
	socket.send (reply);
	}
}
