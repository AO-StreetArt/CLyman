.. _quickstart:

Getting Started with CLyman
===========================

:ref:`Go Home <index>`

This guide will cover a few of the fastest means of getting started with
a CLyman development environment.

Docker
------

This will require Docker installed on the development computer.

The Development Docker image for CLyman-Dev is ready for active use, and
can be run with the command:

``docker run --name clyman -d aostreetart/clyman-automated-build``

Congratulations, you've got a fully functional build & test environment
for CLyman! You can connect to it with:

``docker exec -i -t clyman /bin/bash``

For a more detailed discussion on the deployment of CLyman, please see
the :ref:`Deployment Section <deployment>`
of the documentation. For now, once you are into the Docker instance,
you can run the below command to start CLyman. You can also execute
'make', 'make test', 'make benchmark', etc.

``./lyman``

Building from Source
--------------------

While using Docker is a much faster solution, it is sometimes necessary
to build from source.

The recommended deployment for development of CLyman is a VM with either
Ubuntu 14.04 or Debian 7 installed. Running the below commands in a
terminal on a fresh Virtual Machine will result in a fully functional
build environment that will produce the lyman executable.

``git clone https://github.com/AO-StreetArt/CLyman.git``

``mkdir clyman_deps``

``cp CLyman/scripts/linux/deb/build_deps.sh clyman_deps/build_deps.sh``

``cd clyman_deps``

``./build_deps.sh``

You will be asked once for your sudo password.

``cd ../CLyman``

``make``

This will result in creation of the lyman executable, which we can run
with the below command:

``./clyman -dev``

You may also build the test modules with:

``make tests``


Starting CLyman with the dev flag binds on the below connections: \*
Mongo - mongodb://localhost:27017/ \* Outbund 0MQ Connection -
tcp://localhost:5556 \* Inbound 0MQ Connection - tcp://\*:5555

In order to run CLyman from a properties file, you will need:

-  To install and configure a Redis Server locally, instructions can be
   found at http://redis.io/documentation

-  You will also need to have a Mongo Server installed locally.  Instructions
   can be found at https://docs.mongodb.com/getting-started/

-  Finally, you should have a Consul Agent installed, please find
   instructions at https://www.consul.io/docs/index.html

Continue on to the :ref:`Configuration Section <configuration>` for more details
on the configuration options available when starting CLyman.
