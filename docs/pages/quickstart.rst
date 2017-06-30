.. _quickstart:

Getting Started with CLyman
===========================

:ref:`Go Home <index>`

Docker
------

The easiest way to get started with CLyman is with `Docker <https://docs.docker.com/get-started/>`__

If you do not have Docker installed, please visit the link above to get setup before continuing.

The first thing we need to do is setup the Docker Network that will allow us to communicate between our containers:

``docker network create dvs``

Before we can start CLyman, we need to have a few other programs running first.
Luckily, these can all be setup with Docker as well:

``docker run -d --name=registry --network=dvs consul``

``docker run -d --network=dvs --name=document-db mongo``

``docker run --network=dvs --name=cache -d redis``

This will start up a single instance each of Mongo, Redis, and Consul.  Consul stores our configuration values, so we'll need to set those up.
You can either view the `Consul Documentation <https://www.consul.io/intro/getting-started/ui.html>`__ for information on starting the container with a Web UI, or you can use the commands below for a quick-and-dirty setup:

``docker exec -t registry curl -X PUT -d 'cache--6379----2--5--0' http://localhost:8500/v1/kv/clyman/RedisConnectionString``

``docker exec -t registry curl -X PUT -d 'mongodb://document-db:27017/' http://localhost:8500/v1/kv/clyman/Mongo_ConnectionString``

``docker exec -t registry curl -X PUT -d 'mydb' http://localhost:8500/v1/kv/clyman/Mongo_DbName``

``docker exec -t registry curl -X PUT -d 'test' http://localhost:8500/v1/kv/clyman/Mongo_DbCollection``

``docker exec -t registry curl -X PUT -d 'True' http://localhost:8500/v1/kv/clyman/StampTransactionId``

``docker exec -t registry curl -X PUT -d 'True' http://localhost:8500/v1/kv/clyman/AtomicTransactions``

``docker exec -t registry curl -X PUT -d 'Json' http://localhost:8500/v1/kv/clyman/Data_Format_Type``

Then, we can start up CLyman:

``docker run --name clyman --network=dvs -p 5555:5555 -d aostreetart/clyman -consul-addr=registry:8500 -ip=localhost -port=5555 -log-conf=CLyman/log4cpp.properties``

This will start an instance of CLyman with the following properties:

- Connected to network 'dvs', which lets us refer to the other containers in the network by name when connecting.
- Listening on localhost port 5555
- Connected to Consul Container

We can open up a terminal within the container by:

``docker exec -i -t clyman /bin/bash``

The 'stop_clyman.py' script is provided as an easy way to stop CLyman running as
a service.  This can be executed with:

``python stop_clyman.py hostname port``

For a more detailed discussion on the deployment of CLyman, please see
the :ref:`Deployment Section <deployment>`
of the documentation.

Using the Latest Release
------------------------

In order to use the latest release, you will still need to start up the
applications used by CLyman, namely Redis, Mongo, and Consul.  This can be done
using the docker instructions above, or by installing each to the system manually.
Instructions:
* `Redis <https://redis.io/topics/quickstart>`__
* `Mongo <https://docs.mongodb.com/getting-started/shell/>`__
* `Consul <https://www.consul.io/intro/getting-started/install.html>`__

Then, download the latest release from the `Releases Page <https://github.com/AO-StreetArt/CLyman/releases>`__

Currently, pre-built binaries are available for:

* Ubuntu 16.04
* CentOS7

Unzip/untar the release file and enter into the directory.  Then, we will use the
easy_install.sh script to install CLyman.  Running the below will attempt to install
the dependencies, and then install the CLyman executable:

``sudo ./easy_install.sh -d``

If you'd rather not automatically install dependencies, and only install the executable,
then you can simply leave off the '-d' flag.  Additionally, you may supply
a '-r' flag to uninstall CLyman:

``sudo ./easy_install -r``

Once the script is finished installing CLyman, you can start CLyman with:

``sudo systemctl start clyman.service``

The 'stop_clyman.py' script is provided as an easy way to stop CLyman running as
a service.  This can be executed with:

``python stop_clyman.py hostname port``

Building from Source
--------------------

The recommended system for development of CLyman is either
Ubuntu 16.04 or CentOS7.

``git clone https://github.com/AO-StreetArt/CLyman.git``

``mkdir clyman_deps``

``cp CLyman/scripts/linux/deb/build_deps.sh clyman_deps/build_deps.sh``

``cd clyman_deps``

``./build_deps.sh``

You will be asked once for your sudo password.

``cd ../CLyman``

``make``

This will result in creation of the clyman executable, which we can run
with the below command:

``./clyman``

When not supplied with any command line parameters, CLyman will look for an app.properties file and log4cpp.properties file to start from.

You may also build the test modules with:

``make tests``

In order to run CLyman from a properties file, you will need:

-  To install and configure a Redis Server locally, instructions can be
   found at http://redis.io/documentation

-  You will also need to have a Mongo Server installed locally.  Instructions
   can be found at https://docs.mongodb.com/getting-started/

Continue on to the :ref:`Configuration Section <configuration>` for more details
on the configuration options available when starting CLyman.
