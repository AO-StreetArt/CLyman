#include <zmq.hpp>
#include "logging.h"
#include <string.h>

class ZMQClient
{
zmq::socket_t *internal_socket;
public:
	ZMQClient (std::string conn);
	~ZMQClient () {delete internal_socket;}
	void send_msg (const char * msg);
};
