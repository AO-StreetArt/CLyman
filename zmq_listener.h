#ifndef ZMQ_LISTENER_H
#define ZMQ_LISTENER_H

#include <zmq.hpp>
#include <string>

#include "event_dispatcher.h"
#include "FastDelegate.h"
#include "lyman_utils.h"

class ZMQListener
{
public:
	void listen(ObjectDelegate (&dispatch)[12], std::string conn);
};

#endif
