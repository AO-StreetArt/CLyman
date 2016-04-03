#include <zmq.hpp>
#include "rapidjson/stringbuffer.h"

class ZMQClient
{
zmq::context_t context (1);
zmq::socket_t socket (context, ZMQ_REQ);

public:
	ZMQClient (std::string conn) {socket.connect (conn);}
	void send_msg (StringBuffer buf);
};
