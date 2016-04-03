# Project CLyman

This is a C++ microservice which synchronizes high-level object attributes across many user devices.

## Dependency Resolution

This service depends on Eigen, a C++ Linear Algebra library.  This can be downloaded from the [Eigen] (http://eigen.tuxfamily.org/index.php?title=Main_Page) site.  Getting it into your include path can be accomplished on Linux/OSX by moving the Eigen folder into the /usr/local/include directory, or by manually linking with -I.

You will also need the FastDelegates code from [here] (http://www.codeproject.com/KB/cpp/FastDelegate/FastDelegate_src.zip).  You can include the two files in the /usr/local/include directory on Linux, or manually link with -I.

Next, you will need Zero MQ which can be found [here] (http://zeromq.org/intro:get-the-software).

You will also need RapidJSON which can be found [here] (https://github.com/miloyip/rapidjson)

## Compilation Steps

Note: These steps assume that all header dependencies have been moved to the user's include path manually.  Otherwise, please be sure to include the correct libraries with -I.

First, compile the list class
g++ -c -o list.o list.cpp

Then, compile the object class:
g++ -c -o obj3.o obj3.cpp 

We can compile the Object tests here (note that this step is optional):
g++ -c -o obj3_test.o obj3_test.cpp

Here we can build the test app (again, optional):
g++ -o obj_test list.o obj3.o obj3_test.o

Now, we build the event_dispatcher:
g++ -c -o event_dispatcher.o event_dispatcher.cpp

(Again, optional) Build the event dispatcher tests:
g++ -c -o event_dispatcher_test.o event_dispatcher_test.cpp

(Also optional) build the event test app
g++ -o event_test event_dispatcher.o event_dispatcher_test.o
