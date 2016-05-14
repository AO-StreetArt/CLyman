# Project CLyman

This is a C++ microservice which synchronizes high-level 3-D object attributes across many user devices.  The goal is to synchronize the position, rotation, and scale of virtual objects projected into a real space, across many different user devices.

This project is currently under heavy development.  Functionality & API are subject to change.



## Dependency Resolution

This service depends on Eigen, a C++ Linear Algebra library.  This can be downloaded from the [Eigen] (http://eigen.tuxfamily.org/index.php?title=Main_Page) site.  Getting it into your include path can be accomplished on Linux/OSX by moving the Eigen folder into the /usr/local/include directory, or by manually linking with -I.

Next, you will need Zero MQ which can be found [here] (http://zeromq.org/intro:get-the-software). Be sure to get the [C++ Drivers] (https://github.com/zeromq/cppzmq) in addition to the software.

You will also need RapidJSON which can be found [here] (https://github.com/miloyip/rapidjson)

For logging, we use log4cpp, which can be found [here] (http://log4cpp.sourceforge.net/)

You will need Couchbase drivers installed.  Couchbase drivers can be found [here] (http://developer.couchbase.com/documentation/server/4.1/sdks/c-2.4/overview.html)

While a running instance of Couchbase is not necessary to build the application, it is advised for testing purposes (if you have no instance to connect to, whether locally or otherwise, you can't run this service).

## Compilation Steps

Note: These steps assume that all header dependencies have been moved to the user's include path manually.  The simplest way to accomplish this on a Linux system is by moving the required dependencies into /usr/local/include, but this may vary.  Please consult the documentation for your particular distribution.

### Main App

Unix users should be able to execute the bash script contained with the below commands:

`sudo chmod +755 build_project.sh`

`./build_project.sh`

Which will build the project modules.

### Tests

Unix users should be able to execute the bash script contained with the below commands:

`sudo chmod +755 build_tests.sh`

`./build_tests.sh`

Which will build the test modules.
