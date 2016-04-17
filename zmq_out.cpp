#include "zmq_out.h"

ZMQClient *zmqo;

void init_zmqo(std::string connstr) {
ZMQClient zout (0MQ_OBConnStr);
zmqo = &zout;
}
