#!/usr/bin/env python

import zmq
import sys

if __name__ == "__main__":

    context = zmq.Context()

    # Socket to talk to server
    print("Connecting to clyman")
    socket = context.socket(zmq.REQ)
    conn_str = "tcp://%s:%s" % (sys.argv[1], sys.argv[2])
    socket.connect(conn_str)

    # Send the request
    request = "{\"msg_type\": 999}"
    print("Sending request %s" % request)
    socket.send_string(request)

    # Get the reply.
    message = socket.recv()
    print("Received reply %s [ %s ]" % (request, message))
