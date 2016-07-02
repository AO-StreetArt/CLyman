//
//  Hello World client in C++
//  Connects REQ socket to tcp://localhost:5555
//  Sends "Hello" to server, expects "World" back
//
#include <zmq.hpp>
#include <string>
#include <iostream>

#include <string>
#include "lyman_utils.h"

int main ()
{
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REQ);

    std::cout << "Connecting to test server…" << std::endl;
    socket.connect ("tcp://localhost:5555");

    //  Do 10 requests, waiting each time for a response
    for (int request_nbr = 0; request_nbr != 10; request_nbr++) {
        zmq::message_t request (7);
        memcpy (request.data (), "OBJ_UPD", 7);
        std::cout << "Sending Update " << request_nbr << "…" << std::endl;
        socket.send (request);

        //  Get the reply.
        zmq::message_t reply;
        socket.recv (&reply);

	//translate reply to a string
	std::string resp_string;
	resp_string = hexDump (reply);

        std::cout << "Received Reply " << resp_string << std::endl;
    }
    return 0;
}
