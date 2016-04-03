#include "zmq_client.h"

void ZMQClient::send_msg (StringBuffer& buf)
{
	//Get the size of the buffer being passed in
	int buffer_size;
	buffer_size = buf.GetSize();

	//Set up the message to go out on 0MQ
	zmq::message_t request (buffer_size);
	memcpy (request.data (), buf.GetString(), buffer_size);

	//Send the message
	socket.send (request);

	//  Get the reply.
	zmq::message_t reply;
	socket.recv (&reply);

	//TO-DO: Process the reply
}
