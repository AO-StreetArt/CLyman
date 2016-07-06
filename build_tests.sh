#!/bin/bash

#We start with the Configuration Manager Tests

g++ -c -o src/configuration_test.o src/configuration_test.cpp -std=c++11
g++ -o configuration_test src/logging.o src/configuration_manager.o src/configuration_test.o -lpthread -llog4cpp -std=c++11

#Here we have the obj3 tests

g++ -c -o src/obj3_test.o src/obj3_test.cpp -std=c++11
g++ -o obj_test src/Obj3.pb.cc src/logging.o src/obj3.o src/obj3_test.o -lpthread -llog4cpp `pkg-config --cflags --libs protobuf` -std=c++11

#And the object benchmark

g++ -c -o src/obj3_benchmark.o src/obj3_benchmark.cpp -std=c++11
g++ -o obj3_benchmark src/logging.o src/Obj3.pb.cc src/obj3.o src/obj3_benchmark.o -lpthread -llog4cpp `pkg-config --cflags --libs protobuf` -std=c++11

#We can build the couchbase tests:

g++ -c -o src/couchbase_test.o src/couchbase_test.cpp -std=c++11
g++ -o cb_test src/Obj3.pb.cc src/logging.o src/event_dispatcher.o src/couchbase_admin.o src/couchbase_test.o src/obj3.o -lpthread -llog4cpp -lcouchbase `pkg-config --cflags --libs protobuf` -std=c++11

#And the couchbase benchmark
g++ -c -o src/couchbase_benchmark.o src/couchbase_benchmark.cpp -std=c++11
g++ -o couchbase_benchmark src/Obj3.pb.cc src/logging.o src/event_dispatcher.o src/obj3.o src/couchbase_admin.o src/couchbase_benchmark.o -lpthread -llog4cpp -lcouchbase `pkg-config --cflags --libs protobuf` -std=c++11

#Finally, the Redis tests:
g++ -c -o src/redis_test.o src/redis_test.cpp -std=c++11
g++ -o redis_test src/logging.o src/xredis_admin.o src/redis_test.o -lpthread -llog4cpp -lxredis `pkg-config --cflags --libs hiredis` -std=c++11

#And the Redis Benchmarks
g++ -c -o src/redis_benchmark.o src/redis_benchmark.cpp -std=c++11
g++ -o redis_benchmark src/logging.o src/xredis_admin.o src/redis_benchmark.o -lpthread -llog4cpp -lxredis `pkg-config --cflags --libs hiredis` -std=c++11

#Finally, we look to build the main benchmark
g++ -c -o main_benchmark.o main_benchmark.cpp -std=c++11
g++ -o main_benchmark src/Obj3.pb.cc src/logging.o src/configuration_manager.o src/event_dispatcher.o src/globals.o src/zmqo.o src/obj3.o src/couchbase_admin.o src/xredis_admin.o src/document_manager.o main_benchmark.o -lpthread -lxredis -lzmq -lcouchbase -llog4cpp -luuid `pkg-config --cflags --libs protobuf hiredis` -std=c++11
