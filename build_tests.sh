#!/bin/bash

g++ -c -o src/obj3_test.o src/obj3_test.cpp -std=c++11

#Here we can build the test app:

g++ -lpthread -llog4cpp -o obj_test src/logging.o src/obj3.o src/obj3_test.o -std=c++11 `pkg-config --cflags --libs protobuf`

#Now, we get to build the main test app:

g++ -c -llog4cpp -lpthread -lzmq -I /usr/local/lib -o src/main_class_test.o src/main_class_test.cpp -std=c+11

g++ -o main_test src/event_dispatcher.o src/main_class_test.o -llog4cpp -lpthread -lzmq -std=c++11

#And the test client:

#g++ -I /usr/local/lib test_client.cpp -o src/tests/test_client -lzmq

#We can build the couchbase tests:

g++ -c -o src/couchbase_test.o -lcouchbase src/couchbase_test.cpp -I src -std=c++11

#And the couchbase test app:

g++ -o cb_test src/event_dispatcher.o src/couchbase_admin.o src/couchbase_test.o src/obj3.o -lcouchbase -I src -std=c++11

#Finally, the Redis test app:
g++ -c -o src/redis_test.o -lpthread -llog4cpp -lxredis src/redis_test.cpp `pkg-config --cflags --libs hiredis` -I src -std=c++11

g++ -o redis_test -lpthread -llog4cpp -lxredis src/logging.o src/xredis_admin.o src/redis_test.o `pkg-config --cflags --libs hiredis` -I src -std=c++11
