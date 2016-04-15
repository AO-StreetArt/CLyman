#include "zmq_client.h"

ZMQClient::ZMQClient (std::string conn)
{
	//Set the internal socket
	zmq::context_t context (1);
	zmq::socket_t socket (context, ZMQ_REQ);
	internal_socket=&socket;
}

void ZMQClient::send_msg (const char * msg)
{
	//Get the size of the buffer being passed in
	int buffer_size;
	buffer_size = strlen(msg);

	//Set up the message to go out on 0MQ
	zmq::message_t request (buffer_size);
	memcpy (request.data (), msg, buffer_size);

	//Send the message
	internal_socket->send (request);

	//  Get the reply.
	zmq::message_t reply;
	internal_socket->recv (&reply);

	//TO-DO: Process the reply
}
