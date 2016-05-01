# Project CLyman

This is a C++ microservice which synchronizes high-level 3-D object attributes across many user devices.  The goal is to synchronize the position, rotation, and scale of virtual objects projected into a real space, across many different user devices.

This project is currently under heavy development.  Functionality & API are subject to change.



## Dependency Resolution

This service depends on Eigen, a C++ Linear Algebra library.  This can be downloaded from the [Eigen] (http://eigen.tuxfamily.org/index.php?title=Main_Page) site.  Getting it into your include path can be accomplished on Linux/OSX by moving the Eigen folder into the /usr/local/include directory, or by manually linking with -I.

Next, you will need Zero MQ which can be found [here] (http://zeromq.org/intro:get-the-software). Be sure to get the C++ Drivers in addition to the software.

You will also need RapidJSON which can be found [here] (https://github.com/miloyip/rapidjson)

For logging, we use log4cpp, which can be found [here] (http://log4cpp.sourceforge.net/)

You will need Couchbase drivers installed.  Couchbase drivers can be found [here] (http://developer.couchbase.com/documentation/server/4.1/sdks/c-2.4/overview.html)

While a running instance of Couchbase is not necessary to build the application, it is advised for testing purposes (if you have no instance to connect to, whether locally or otherwise, you can't run this service).

## Compilation Steps

Note: These steps assume that all header dependencies have been moved to the user's include path manually.  Otherwise, please be sure to include the correct libraries with -I.

### Main App

We start by compiling the logging module:

g++ -c -llog4cpp -o logging.o logging.cpp -std=c++11

Then, compile the object class:

g++ -c -o obj3.o obj3.cpp -std=c++11

Now, we build the event_dispatcher:

g++ -c -o event_dispatcher.o event_dispatcher.cpp -std=c++11

We then build the couchbase admin:

g++ -c -lcouchbase -o couchbase_admin.o couchbase_admin.cpp -std=c++11

We compile the main object with:

g++ -c -o main.o -lzmq -lcouchbase -lpthread -llog4cpp main.cpp -std=c++11

Finally, we compile the main app with:

g++ -o lyman src/logging.o src/event_dispatcher.o src/obj3.o src/couchbase_admin.o main.o -std=c++11 -lzmq -lcouchbase -lpthread -llog4cpp

### Tests

We can compile the Object tests here:
g++ -c -o obj3_test.o obj3_test.cpp

Here we can build the test app:

g++ -lpthread -llog4cpp -o obj_test ../logging.o ../obj3.o obj3_test.o

Now, we get to build the main test app:

g++ -c -llog4cpp -lpthread -lzmq -I /usr/local/lib -o main_class_test.o main_class_test.cpp

g++ -o main_test ../event_dispatcher.o main_class_test.o -llog4cpp -lpthread -lzmq

And the test client:

g++ -I /usr/local/lib test_client.cpp -o test_client -lzmq

We can build the couchbase tests:

g++ -c -o couchbase_test.o -lcouchbase couchbase_test.cpp -std=c++11

And the couchbase test app:

g++ -o cb_test event_dispatcher.o ../couchbase_admin.o couchbase_test.o ../obj3.o -lcouchbase -std=c++11
