#include <zmq.hpp>
#include "logging.h"
#include "lyman_utils.h"
#include <string.h>

class ZMQClient
{
zmq::socket_t internal_socket;
zmq::context_t internal_context;
public:
	ZMQClient (std::string conn);
	void send_msg (const char * msg);
};
