#!/bin/bash

#We start with the Object Tests

g++ -c -o src/obj3_test.o -lpthread -llog4cpp src/obj3_test.cpp -std=c++11 `pkg-config --cflags --libs protobuf`

#Here we can build the test app:

g++ -o obj_test src/Obj3.pb.cc src/logging.o src/obj3.o src/obj3_test.o -lpthread -llog4cpp -std=c++11 `pkg-config --cflags --libs protobuf`

#We can build the couchbase tests:

g++ -c -o src/couchbase_test.o -lcouchbase src/couchbase_test.cpp -I src -std=c++11

#And the couchbase test app:

g++ -o cb_test src/Obj3.pb.cc src/logging.o src/event_dispatcher.o src/couchbase_admin.o src/couchbase_test.o src/obj3.o -lcouchbase -I src -std=c++11 `pkg-config --cflags --libs protobuf`

#Finally, the Redis tests:
g++ -c -o src/redis_test.o -lpthread -llog4cpp -lxredis src/redis_test.cpp `pkg-config --cflags --libs hiredis` -I src -std=c++11

g++ -o redis_test src/logging.o src/xredis_admin.o src/redis_test.o -lpthread -llog4cpp -lxredis `pkg-config --cflags --libs hiredis` -I src -std=c++11
