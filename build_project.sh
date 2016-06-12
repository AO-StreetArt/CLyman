#!/bin/bash
exec 3>&1 4>&2
trap 'exec 2>&4 1>&3' 0 1 2 3
exec 1>build_project.log 2>&1

opt=$1

if [-n "$opt"]
then

  if [ $opt -eq "g"]
  then

    #We start by compiling the proto buffer class
    protoc -I=src --cpp_out=src src/Obj3.proto

    #We start by compiling the logging module:

    g++ -g -c -llog4cpp -o src/logging.o src/logging.cpp -std=c++11

    #Then, compile the object class:

    g++ -g -c -o src/obj3.o src/obj3.cpp -std=c++11

    #Now, we build the event_dispatcher:

    g++ -g -c -o src/event_dispatcher.o src/event_dispatcher.cpp -std=c++11

    #We then build the couchbase admin:

    g++ -g -c -lcouchbase -o src/couchbase_admin.o src/couchbase_admin.cpp -std=c++11

    #Now, we build the Redis Admin
    g++ -g -c -o src/xredis_admin.o -lpthread -lxredis -llog4cpp src/xredis_admin.cpp `pkg-config --cflags --libs hiredis` -std=c++11

    #We compile the main object with:

    g++ -g -c -o main.o -lzmq -lcouchbase -lpthread -llog4cpp -lhiredis -lxredis main.cpp -std=c++11

    #Finally, we compile the main app with:

    g++ -g -o lyman src/Obj3.pb.cc src/logging.o src/event_dispatcher.o src/obj3.o src/couchbase_admin.o src/xredis_admin.o main.o -lpthread -lxredis -lzmq -lcouchbase -llog4cpp `pkg-config --cflags --libs protobuf hiredis` -std=c++11

  fi

else

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

  #Now, we build the Redis Admin
  g++ -c -o src/xredis_admin.o -lpthread -lxredis -llog4cpp src/xredis_admin.cpp `pkg-config --cflags --libs hiredis` -std=c++11

  #We compile the main object with:

  g++ -c -o main.o -lzmq -lcouchbase -lpthread -llog4cpp -lhiredis -lxredis main.cpp -std=c++11

  #Finally, we compile the main app with:

  g++ -o lyman src/Obj3.pb.cc src/logging.o src/event_dispatcher.o src/obj3.o src/couchbase_admin.o src/xredis_admin.o main.o -lpthread -lxredis -lzmq -lcouchbase -llog4cpp `pkg-config --cflags --libs protobuf hiredis` -std=c++11

fi
