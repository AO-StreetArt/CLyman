#!/bin/bash
exec 3>&1 4>&2
trap 'exec 2>&4 1>&3' 0 1 2 3
exec 1>build_project.log 2>&1

opt=$1

if [ -n "$opt" ]
then

  if [ $opt == "g" ]
  then

    #We start by compiling the proto buffer class
    protoc -I=src --cpp_out=src src/Obj3.proto
    #We then compile the configuration manager

    g++ -g -c -o src/configuration_manager.o src/configuration_manager.cpp -std=c++11

    #Then, compile the object class:

    g++ -g -c -o src/obj3.o src/obj3.cpp -std=c++11

    #Now, we build the event_dispatcher:

    g++ -g -c -o src/event_dispatcher.o src/event_dispatcher.cpp -std=c++11

    #We compile the globals

    g++ -g -c -o src/globals.o src/globals.cpp -std=c++11

    #We compile the Document Manager
    g++ -g -c -o src/document_manager.o src/document_manager.cpp -std=c++11

    #We compile the main object with:

    g++ -g -c -o main.o main.cpp -std=c++11

    #Finally, we compile the main app with:

    g++ -g -o lyman src/http_admin.o src/Obj3.pb.cc src/logging.o src/configuration_manager.o src/event_dispatcher.o src/globals.o src/zmqo.o src/obj3.o src/couchbase_admin.o src/xredis_admin.o src/document_manager.o main.o -lcurl -lpthread -lxredis -lzmq -lcouchbase -llog4cpp -luuid -laossl `pkg-config --cflags --libs protobuf hiredis` -std=c++11

    echo "Debugger Lines Set"

  fi

else

  #We start by compiling the proto buffer class
  protoc -I=src --cpp_out=src src/Obj3.proto
  #We then compile the configuration manager

  g++ -c -o src/configuration_manager.o src/configuration_manager.cpp -std=c++11

  #Then, compile the object class:

  g++ -c -o src/obj3.o src/obj3.cpp -std=c++11

  #Now, we build the event_dispatcher:

  g++ -c -o src/event_dispatcher.o src/event_dispatcher.cpp -std=c++11

  #We compile the globals

  g++ -c -o src/globals.o src/globals.cpp -std=c++11

  #We compile the Document Manager
  g++ -c -o src/document_manager.o src/document_manager.cpp -std=c++11

  #We compile the main object with:

  g++ -c -o main.o main.cpp -std=c++11

  #Finally, we compile the main app with:

  g++ -o lyman src/http_admin.o src/Obj3.pb.cc src/logging.o src/configuration_manager.o src/event_dispatcher.o src/globals.o src/zmqo.o src/obj3.o src/couchbase_admin.o src/xredis_admin.o src/document_manager.o main.o -lcurl -lpthread -lxredis -lzmq -lcouchbase -llog4cpp -luuid -laossl `pkg-config --cflags --libs protobuf hiredis` -std=c++11

  echo "Project Built"

fi
