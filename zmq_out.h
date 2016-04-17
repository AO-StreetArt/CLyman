#include "zmq_client.h"

#ifndef LYMAN_ZMQO
#define LYMAN_ZMQO

extern ZMQClient *zmqo;

void init_zmqo(std::string connstr);
#endif
