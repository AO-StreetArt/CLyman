#!/bin/bash

g++ -c -o obj3_test.o obj3_test.cpp

#Here we can build the test app:

g++ -lpthread -llog4cpp -o obj_test ../logging.o ../obj3.o obj3_test.o

#Now, we get to build the main test app:

g++ -c -llog4cpp -lpthread -lzmq -I /usr/local/lib -o main_class_test.o main_class_test.cpp

g++ -o main_test ../event_dispatcher.o main_class_test.o -llog4cpp -lpthread -lzmq

#And the test client:

g++ -I /usr/local/lib test_client.cpp -o test_client -lzmq

#We can build the couchbase tests:

g++ -c -o couchbase_test.o -lcouchbase couchbase_test.cpp -std=c++11

#And the couchbase test app:

g++ -o cb_test event_dispatcher.o ../couchbase_admin.o couchbase_test.o ../obj3.o -lcouchbase -std=c++11
