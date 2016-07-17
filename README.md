# Project CLyman

## Build Status
* Project - Successful
* Tests - Successful

## Overview

This is a C++ microservice which synchronizes high-level 3-D object attributes across many user devices.  The goal is to synchronize the position, rotation, and scale of virtual objects projected into a real space.

This service is intended to fill a small role within a larger architecture designed to synchronize 3D objects across different client programs.  It is highly scalable, and many instances can run in parallel to support increasing load.

Features:
* Storage of 3-D Objects Location, Rotation, Scaling
* Enable real-time change feeds on the objects which are stored
* Connect to other services over Zero MQ using JSON or Google Protocol Buffers.
* Configurable Update logic & message formatting
* Scalable microservice design

### Architecture

This is designed to be used as a microservice within a larger architecture.  This will take in CRUD messages for objects in 3 space, and track location, rotation, and scaling.  It will also ensure that any updates are sent out on a separate port to allow streaming to all user devices.

A .proto file is included to allow generating the bindings for any language (the [protocol buffer compiler] (https://developers.google.com/protocol-buffers/) is installed by the build_deps script), which can be used to communicate via protocol buffers.  Alternatively, Clyman can be configured to accept JSON messages.  Full documentation on supported message formats will be forthcoming.

In addition, [0-Meter] (https://github.com/AO-StreetArt/0-Meter) has been developed to allow easy testing of the module with JSON message formats.

Please note that running CLyman requires an instance of both [Couchbase DB Server 4.1] (http://www.couchbase.com/) and [Redis 3.2.0] (http://redis.io/) to connect to in order to run.  Clustering of both Couchbase & Redis are supported.

CLyman can also be deployed with [Consul] (https://www.consul.io/) as a Service Discovery and Distributed Configuration architecture.  This requires the [Consul Agent] (https://www.consul.io/downloads.html) to be deployed that CLyman can connect to.

See 'How to use CLyman' section for recommended deployment options.

## Configuration

CLyman can be configured via a properties file, which has a few command line options:

* `./lyman -dev` - This will start CLyman in dev mode
* `./lyman` - This will start CLyman with the default properties file, lyman.properties
* `./lyman -config-file=file.properties` - This will start CLyman with the properties file, file.properties.  Can be combined with -log-conf.
* `./lyman -log-conf=logging.properties` - This will start CLyman with the logging properties file, logging.properties.  Can be combined with -config-file.

Clyman can also be configured via a Consul Connection, in which we must specify the address of the consul agent, and the ip & port of the Inbound ZeroMQ Connection.

* `./lyman -consul-addr=localhost:8500 -ip=localhost -port=5555` - Start Clyman, register as a service with consul, and configure based on configuration values in Consul, and bind to an internal 0MQ port on localhost
* `./lyman -consul-addr=localhost:8500 -ip=tcp://my.ip -port=5555` - Start Clyman, register as a service with consul, and configure based on configuration values in Consul, and bind to an external 0MQ port on tcp://my.ip

The properties file, lyman.properties can be edited in any text editor, and when configuring from Consul the keys of the properties file are equal to the expected keys in Consul.

The Logging Configuration File can also be edited with a text file, and the documentation for this can be found [here] (http://log4cpp.sourceforge.net/api/classlog4cpp_1_1PropertyConfigurator.html).

Below you can find a summary of the options in the Properties File or Consul Key-Value Store:

### DB
* DB_ConnectionString - The string used to connect to the couchbase instance (example: couchbase://localhost/default)
* DB_AuthenticationActive - True or False, whether or not Database Authentication is active on the couchbase server
* DB_Password - If DB Authentication is active, then this is used as the password to connect to the couchbase instance

### 0MQ
* 0MQ_OutboundConnectionString - The connectivity string for the outbound 0MQ Port
* 0MQ_InboundConnectionString - The connectivity string for the inbound 0MQ Port

### Redis Connection
* RedisConnectionString - We have a list of Redis Connections in our cluster in the format "ip--port--password--poolsize--timeout--role"

### Behavior
* SmartUpdatesActive - True if we want to allow partial messages, and only update fields included in the partial messages.  False if we want to enforce full replace update messages.
* MessageFormat - json to take in and read out JSON Format, protocol-buffer to take in and read out Google Protocol Buffer Format
* RedisBufferFormat - json to read and write JSON format to the Redis Buffer, protocol-buffer to read and write Protocol Buffers to the Redis Buffer

## How to Use CLyman
CLyman has three recommended deployment types:

### Development

Those wishing to develop on CLyman have two options.  Preferably, they can use Docker to set up a build & test environment with a few simple steps.  Otherwise, the module will need to be built from source.

#### Docker

You will need to have an instance of Couchbase running to use CLyman, and this can be done via the below command (full instructions can be found [here] (https://docs.docker.com/engine/examples/couchbase/)):

`docker run -d --name db -p 8091-8093:8091-8093 -p 11210:11210 couchbase`

You will also need an instance of Redis Running to use CLyman.  This can be done via the below command (full instructions can be found [here] (https://hub.docker.com/_/redis/)):

`docker run --name some-redis -d redis`

The Development Docker image for CLyman-Dev is ready for active use, and can be run with the command:

`docker run --name clyman -d awbarry00/clyman-dev`

Congratulations, you've got a fully functional build & test environment for CLyman!

SSH keys are provided with the repository.  In order to run the development image for CLyman, you will need to access the Docker Process via SSH.  Access is granted with below set of commands:

    ssh-agent -s
    ssh-add ssh/id_rsa
    ssh root@localhost -p `sudo docker port ssh 22 | cut -d":" -f2`

##### Building the Docker Image

Should you so desire, the docker image can be rebuilt using the below commands

`docker pull awbarry00/clyman-dev`

`sudo docker build -t clyman_dev_2 .`

`docker run --name clyman -it -d -P clyman_dev_2`

#### Building from Source

While using Docker is a much faster solution, it is sometimes necessary to build from source.

The recommended deployment for development of CLyman is a VM with either Ubuntu 14.04 or Debian 7 installed.  Running the below commands in a terminal on a fresh Virtual Machine will result in a fully functional build environment that will produce the lyman executable.

`git clone https://github.com/AO-StreetArt/CLyman.git`

`mkdir clyman_deps`

`cp CLyman/build_deps.sh clyman_deps/build_deps.sh`

`cd clyman_deps`

`./build_deps.sh`

You will be asked once for your sudo password.

`cd ../CLyman`

`./build_project.sh`

This will result in creation of the lyman executable, which we can run with the below command:

`./lyman -dev`

You may also build the test modules with:

`./build_tests.sh`

You will need to have an instance of Couchbase Server & Redis Server up and running as well.  This can be done via Docker, or manually.

Please find instructions to install and configure a Couchbase Server locally [here] (http://www.couchbase.com/get-started-developing-nosql#)

Starting CLyman with the dev flag binds on the below connections:
* Couchbase - couchbase://localhost/default (no password)
* Outbund 0MQ Connection - tcp://localhost:5556
* Inbound 0MQ Connection - tcp://*:5555

In order to run CLyman from a properties file, you will need:

* To install and configure a Redis Server locally, instructions can be found [here] (http://redis.io/documentation)

* Finally, you should have a Consul Agent installed, please find instructions [here] (https://www.consul.io/docs/index.html)

`./lyman`

Will run the program and look for the default file, lyman.properties.  See the configuration section for more details.

##### Dependency Resolution

For Ubuntu 14.04 & Debian 7, the build_deps.sh script should allow for automatic resolution of dependencies.  However, links are still included below for those who wish to build on other Operating Systems.

This library is built on top of the [AO Shared Service Library] (https://github.com/AO-StreetArt/AOSharedServiceLibrary).  This must be built and linked/included.

You will need the Google Protocol Buffer C++ API, which can be found [here](https://developers.google.com/protocol-buffers).  These can be installed on many linux systems with a variant of the below command:

`sudo apt-get install libprotobuf-dev protobuf-compiler`

This service also depends on Eigen, a C++ Linear Algebra library.  This can be downloaded from the [Eigen] (http://eigen.tuxfamily.org/index.php?title=Main_Page) site.

You will need RapidJSON which can be found [here] (https://github.com/miloyip/rapidjson).

### Production
The current recommended deployment for production is to build from source and scale manually.  However, rather than deploying a Couchbase Server & Redis Server locally beside CLyman, we should be connecting to a Couchbase Cluster & Redis Cluster on separate nodes.

Based on testing & evaluation, recommended deployment may change to go through Docker Images currently in development.
