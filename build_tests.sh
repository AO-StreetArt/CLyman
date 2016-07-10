#!/bin/bash
exec 3>&1 4>&2
trap 'exec 2>&4 1>&3' 0 1 2 3
exec 1>build_tests.log 2>&1

opt=$1

if [ -n "$opt" ]
then

  if [ $opt == "g" ]
  then

    #We start with the Configuration Manager Tests

    g++ -g -c -o src/configuration_test.o src/configuration_test.cpp -std=c++11
    g++ -g -o configuration_test src/configuration_manager.o src/configuration_test.o -lpthread -llog4cpp -laossl -std=c++11

    #Then we have the HTTP Tests
    g++ -g -c -o src/http_test.o src/http_test.cpp -std=c++11
    g++ -g -o http_test src/http_test.o -lpthread -llog4cpp -lcurl -laossl -std=c++11

    #Here we have the obj3 tests

    g++ -g -c -o src/obj3_test.o src/obj3_test.cpp -std=c++11
    g++ -g -o obj_test src/Obj3.pb.cc src/obj3.o src/obj3_test.o -lpthread -llog4cpp -laossl `pkg-config --cflags --libs protobuf` -std=c++11

    #And the object benchmark

    g++ -g -c -o src/obj3_benchmark.o src/obj3_benchmark.cpp -std=c++11
    g++ -g -o obj3_benchmark src/Obj3.pb.cc src/obj3.o src/obj3_benchmark.o -lpthread -llog4cpp -laossl `pkg-config --cflags --libs protobuf` -std=c++11

    #We can build the couchbase tests:

    g++ -g -c -o src/couchbase_test.o src/couchbase_test.cpp -std=c++11
    g++ -g -o cb_test src/Obj3.pb.cc src/event_dispatcher.o src/couchbase_test.o src/obj3.o -lpthread -llog4cpp -lcouchbase -laossl `pkg-config --cflags --libs protobuf` -std=c++11

    #And the couchbase benchmark
    g++ -g -c -o src/couchbase_benchmark.o src/couchbase_benchmark.cpp -std=c++11
    g++ -g -o couchbase_benchmark src/Obj3.pb.cc src/event_dispatcher.o src/couchbase_benchmark.o -lpthread -llog4cpp -lcouchbase -laossl `pkg-config --cflags --libs protobuf` -std=c++11

    #Finally, the Redis tests:
    g++ -g -c -o src/redis_test.o src/redis_test.cpp -std=c++11
    g++ -g -o redis_test src/redis_test.o -lpthread -llog4cpp -lxredis -laossl `pkg-config --cflags --libs hiredis` -std=c++11

    #And the Redis Benchmarks
    g++ -g -c -o src/redis_benchmark.o src/redis_benchmark.cpp -std=c++11
    g++ -g -o redis_benchmark src/redis_benchmark.o -lpthread -llog4cpp -lxredis -laossl `pkg-config --cflags --libs hiredis` -std=c++11

    #Finally, we look to build the main benchmark
    g++ -g -c -o main_benchmark.o main_benchmark.cpp -std=c++11
    g++ -g -o main_benchmark src/Obj3.pb.cc src/configuration_manager.o src/event_dispatcher.o src/globals.o src/obj3.o src/document_manager.o main_benchmark.o -lpthread -lxredis -lzmq -lcouchbase -llog4cpp -luuid -laossl `pkg-config --cflags --libs protobuf hiredis` -std=c++11

    echo "Debugger Lines Set"

  fi

else

  #We start with the Configuration Manager Tests

  g++ -c -o src/configuration_test.o src/configuration_test.cpp -std=c++11
  g++ -o configuration_test src/configuration_manager.o src/configuration_test.o -lpthread -llog4cpp -laossl -std=c++11

  #Then we have the HTTP Tests
  g++ -c -o src/http_test.o src/http_test.cpp -std=c++11
  g++ -o http_test src/http_test.o -lpthread -llog4cpp -lcurl -laossl -std=c++11

  #Here we have the obj3 tests

  g++ -c -o src/obj3_test.o src/obj3_test.cpp -std=c++11
  g++ -o obj_test src/Obj3.pb.cc src/obj3.o src/obj3_test.o -lpthread -llog4cpp -laossl `pkg-config --cflags --libs protobuf` -std=c++11

  #And the object benchmark

  g++ -c -o src/obj3_benchmark.o src/obj3_benchmark.cpp -std=c++11
  g++ -o obj3_benchmark src/Obj3.pb.cc src/obj3.o src/obj3_benchmark.o -lpthread -llog4cpp -laossl `pkg-config --cflags --libs protobuf` -std=c++11

  #We can build the couchbase tests:

  g++ -c -o src/couchbase_test.o src/couchbase_test.cpp -std=c++11
  g++ -o cb_test src/Obj3.pb.cc src/event_dispatcher.o src/couchbase_test.o src/obj3.o -lpthread -llog4cpp -lcouchbase -laossl `pkg-config --cflags --libs protobuf` -std=c++11

  #And the couchbase benchmark
  g++ -c -o src/couchbase_benchmark.o src/couchbase_benchmark.cpp -std=c++11
  g++ -o couchbase_benchmark src/Obj3.pb.cc src/event_dispatcher.o src/couchbase_benchmark.o -lpthread -llog4cpp -lcouchbase -laossl `pkg-config --cflags --libs protobuf` -std=c++11

  #Finally, the Redis tests:
  g++ -c -o src/redis_test.o src/redis_test.cpp -std=c++11
  g++ -o redis_test src/redis_test.o -lpthread -llog4cpp -lxredis -laossl `pkg-config --cflags --libs hiredis` -std=c++11

  #And the Redis Benchmarks
  g++ -c -o src/redis_benchmark.o src/redis_benchmark.cpp -std=c++11
  g++ -o redis_benchmark src/redis_benchmark.o -lpthread -llog4cpp -lxredis -laossl `pkg-config --cflags --libs hiredis` -std=c++11

  #Finally, we look to build the main benchmark
  g++ -c -o main_benchmark.o main_benchmark.cpp -std=c++11
  g++ -o main_benchmark src/Obj3.pb.cc src/configuration_manager.o src/event_dispatcher.o src/globals.o src/obj3.o src/document_manager.o main_benchmark.o -lpthread -lxredis -lzmq -lcouchbase -llog4cpp -luuid -laossl `pkg-config --cflags --libs protobuf hiredis` -std=c++11

fi
