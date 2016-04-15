# Project CLyman

This is a C++ microservice which synchronizes high-level object attributes across many user devices.

This project is currently under heavy development.  Functionality & API are subject to change.



## Dependency Resolution

This service depends on Eigen, a C++ Linear Algebra library.  This can be downloaded from the [Eigen] (http://eigen.tuxfamily.org/index.php?title=Main_Page) site.  Getting it into your include path can be accomplished on Linux/OSX by moving the Eigen folder into the /usr/local/include directory, or by manually linking with -I.

You will also need the FastDelegates code from [here] (http://www.codeproject.com/KB/cpp/FastDelegate/FastDelegate_src.zip).  You can include the two files in the /usr/local/include directory on Linux, or manually link with -I.

Next, you will need Zero MQ which can be found [here] (http://zeromq.org/intro:get-the-software). Be sure to get the C++ Drivers in addition to the software.

You will also need RapidJSON which can be found [here] (https://github.com/miloyip/rapidjson)

For logging, we use log4cpp, which can be found [here] (http://log4cpp.sourceforge.net/)

You will need both Redis and Couchbase drivers installed, (once initial development is done, you will be able to choose whether to deploy to one or the other)

Redis drivers can be found [here] (https://github.com/hmartiro/redox)

Couchbase drivers can be found [here] (http://developer.couchbase.com/documentation/server/4.1/sdks/c-2.4/overview.html)

## Compilation Steps

Note: These steps assume that all header dependencies have been moved to the user's include path manually.  Otherwise, please be sure to include the correct libraries with -I.

First, compile the object class:
g++ -c -o obj3.o obj3.cpp 

We can compile the Object tests here (note that this step is optional):
g++ -c -o obj3_test.o obj3_test.cpp

Here we can build the test app (again, optional):
g++ -o obj_test obj3.o obj3_test.o

Now, we build the event_dispatcher:
g++ -c -o event_dispatcher.o event_dispatcher.cpp

(Again, optional) Build the event dispatcher tests:
g++ -c -o event_dispatcher_test.o event_dispatcher_test.cpp

(Also optional) build the event test app
g++ -o event_test event_dispatcher.o event_dispatcher_test.o


Now, we get to build the main test app:

g++ -c -llog4cpp -lpthread -lzmq -o main_class_test.o main_class_test.cpp

g++ -o main_test event_dispatcher.o main_class_test.o -llog4cpp -lpthread -lzmq

And the test client:

g++ test_client.cpp -o test_client -lzmq

We then build the couchbase admin:

g++ -c -lcouchbase -o couchbase_admin.o couchbase_admin.cpp -std=c++11


Next step is to build ZMQ Client:

g++ -I /usr/local/lib -c -lzmq -o zmq_client.o zmq_client.cpp


We can build the couchbase tests:

g++ -c -o couchbase_test.o -lcouchbase couchbase_test.cpp -std=c++11
