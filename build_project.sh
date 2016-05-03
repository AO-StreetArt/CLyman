#!/bin/bash

#Change to the src directory to compile the lower level objects

cd src

#We start by compiling the logging module:

g++ -c -llog4cpp -o logging.o logging.cpp -std=c++11

#Then, compile the object class:

g++ -c -o obj3.o obj3.cpp -std=c++11

#Now, we build the event_dispatcher:

g++ -c -o event_dispatcher.o event_dispatcher.cpp -std=c++11

#We then build the couchbase admin:

g++ -c -lcouchbase -o couchbase_admin.o couchbase_admin.cpp -std=c++11

#Change directories back to the main directory
cd ..

#We compile the main object with:

g++ -c -o main.o -lzmq -lcouchbase -lpthread -llog4cpp main.cpp -std=c++11

#Finally, we compile the main app with:

g++ -o lyman src/logging.o src/event_dispatcher.o src/obj3.o src/couchbase_admin.o main.o -std=c++11 -lzmq -lcouchbase -lpthread -llog4cpp
