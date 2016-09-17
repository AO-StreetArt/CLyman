# Getting Started with CLyman

This guide will cover a few of the fastest means of getting started with a CLyman development environment.

## Docker

This will require Docker installed on the development computer.

The Development Docker image for CLyman-Dev is ready for active use, and can be run with the command:

`docker run --name clyman -d aostreetart/clyman-automated-build`

Congratulations, you've got a fully functional build & test environment for CLyman!  You can connect to it with:

`docker exec -i -t clyman /bin/bash`

For a more detailed discussion on the deployment of CLyman, please see the [Deployment] (https://github.com/AO-StreetArt/CLyman/tree/master/docs/deploy) section of the documentation.  For now, once you are into the Docker instance, you can run the below command to start CLyman.  You can also execute 'make', 'make test', 'make benchmark', etc.

`./lyman`

## Building from Source

While using Docker is a much faster solution, it is sometimes necessary to build from source.

The recommended deployment for development of CLyman is a VM with either Ubuntu 14.04 or Debian 7 installed.  Running the below commands in a terminal on a fresh Virtual Machine will result in a fully functional build environment that will produce the lyman executable.

`git clone https://github.com/AO-StreetArt/CLyman.git`

`mkdir clyman_deps`

`cp CLyman/scripts/linux/deb/build_deps.sh clyman_deps/build_deps.sh`

`cd clyman_deps`

`./build_deps.sh`

You will be asked once for your sudo password.

`cd ../CLyman`

`make`

This will result in creation of the lyman executable, which we can run with the below command:

`./lyman -dev`

You may also build the test modules with:

`make tests`

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


#Next
Now that you have a working instance of CLyman, you can move on to the [Configuration] (https://github.com/AO-StreetArt/CLyman/tree/master/docs/config) Section.
