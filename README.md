# Project CLyman

This is a C++ microservice which synchronizes high-level 3-D object attributes across many user devices.  The goal is to synchronize the position, rotation, and scale of virtual objects projected into a real space, across many different user devices.

Features:
* Storage of 3-D Objects Location, Rotation, Scaling
* Enable real-time change feeds on the objects which are stored
* Connect to other services over Zero MQ using JSON or Google Protocol Buffers.
* Configurable Update logic & message formatting


## Dependency Resolution

You will need RapidJSON which can be found [here] (https://github.com/miloyip/rapidjson), and the Google Protocol Buffer C++ API, which can be found [here](https://developers.google.com/protocol-buffers).  These can be installed on many linux systems with a variant of the below command:

`sudo apt-get install libprotobuf-dev protobuf-compiler`

This service also depends on Eigen, a C++ Linear Algebra library.  This can be downloaded from the [Eigen] (http://eigen.tuxfamily.org/index.php?title=Main_Page) site.

Next, you will need Zero MQ which can be found [here] (http://zeromq.org/intro:get-the-software). Be sure to get the [C++ Drivers] (https://github.com/zeromq/cppzmq) in addition to the software.

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

## Use & Configuration

### Use

This is designed to be used as a microservice within a larger architecture.  This will take in CRUD messages for objects in 3 space, and track location, rotation, and scaling.  It will also ensure that any updates are sent out on a separate port to allow streaming to all user devices.

The main service can be started with:

`./lyman`

The properties file, lyman.properties can be edited in any text editor.  Below you can find a summary of the options:

### Configuration

#### DB
* DB_ConnectionString - The string used to connect to the couchbase instance (example: couchbase://localhost/default)
* DB_AuthenticationActive - True or False, whether or not Database Authentication is active on the couchbase server
* DB_Password - If DB Authentication is active, then this is used as the password to connect to the couchbase instance

#### 0MQ
* 0MQ_OutboundConnectionString - The connectivity string for the outbound 0MQ Port
* 0MQ_InboundConnectionString - The connectivity string for the inbound 0MQ Port

#### Behavior
* SmartUpdatesActive - True if we want to allow partial messages, and only update fields included in the partial messages.  False if we want to enforce full replace update messages.
* MessageFormat - json to take in and read out JSON Format, protocol-buffer to take in and read out Google Protocol Buffer Format
