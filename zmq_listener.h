#ifndef ZMQ_LISTENER_H
#define ZMQ_LISTENER_H

#include <zmq.hpp>
#include <string>

#include "event_dispatcher.h"
#include "FastDelegate.h"
#include "Obj3.h"
#include "lyman_utils.h"

class ZMQListener
{
//  Prepare our context and socket
zmq::context_t context (1);
zmq::socket_t socket (context, ZMQ_REP);

public:
	ZMQListener (std::string conn) {socket.bind (conn);}
	void listen(ObjectDelegate& dispatch);
};

#endif
