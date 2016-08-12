# -*- coding: utf-8 -*-
"""
Created on Thu May 19 19:42:39 2016

This is a heartbeat script for CLyman running Protobuffers

@author: alex
"""

import zmq
import Obj3_pb2
import sys, traceback
import logging
from ConfigManager import ConfigurationManager

def get_exception():
    exc_type, exc_value, exc_traceback = sys.exc_info()
    return repr(traceback.format_exception(exc_type, exc_value, exc_traceback))

def execute_main():

    cm = ConfigurationManager()

    cm.configure("heartbeat_config.txt")

    logging.basicConfig(filename="heartbeat.log", level=logging.DEBUG)

    #Set up the base objects for the script
    try:
        #set up the ZMQ Connection
        context = zmq.Context()

        socket = context.socket(zmq.REQ)
        socket.connect(cm["Destination_Address"])

        # Build a proto buf object
        obj = Obj3_pb2.Obj3()
        obj.message_type = 555
    except Exception as e:
        logging.error("Error building base objects")
        logging.error(e)
        logging.error(get_exception())
        return 1

    try:
        #Serialize the proto buffer
        obj_str = obj.SerializeToString()
    except Exception as e:
        logging.error("Error serializing message")
        logging.error(e)
        logging.error(get_exception())
        return 1

    try:
        #Send the message
        socket.send(obj_str)
    except Exception as e:
        logging.error("Error sending message")
        logging.error(e)
        logging.error(get_exception())
        return 1

    try:
        #  Get the reply.
        message = socket.recv()
    except Exception as e:
        logging.error("Error getting response")
        logging.error(e)
        logging.error(get_exception())
        return 1

    if message == "'b'success" or message == "success":
        return 0
    else:
        logging.error("Non-Success Response Recieved, failing heartbeat")
        logging.error(message)
        return 1

if __name__ == "__main__":
    sys.exit(execute_main())
