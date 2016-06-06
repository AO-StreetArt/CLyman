#!/bin/bash

#We start by compiling the proto buffer class
protoc -I=src --cpp_out=src src/Obj3.proto

#We start by compiling the logging module:

g++ -c -llog4cpp -o src/logging.o src/logging.cpp -std=c++11

#Then, compile the object class:

g++ -c -o src/obj3.o src/obj3.cpp -std=c++11

#Now, we build the event_dispatcher:

g++ -c -o src/event_dispatcher.o src/event_dispatcher.cpp -std=c++11

#We then build the couchbase admin:

g++ -c -lcouchbase -o src/couchbase_admin.o src/couchbase_admin.cpp -std=c++11

#We compile the main object with:

g++ -c -o main.o -lzmq -lcouchbase -lpthread -llog4cpp -lhiredis -lxredis main.cpp -std=c++11

#Finally, we compile the main app with:

g++ -o lyman src/Obj3.pb.cc src/logging.o src/event_dispatcher.o src/obj3.o src/couchbase_admin.o main.o -lpthread -lxredis -lzmq -lcouchbase -llog4cpp `pkg-config --cflags --libs protobuf hiredis` -std=c++11
