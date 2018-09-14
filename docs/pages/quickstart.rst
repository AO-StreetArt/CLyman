.. _quickstart:

Getting Started with Clyman
==============================

:ref:`Go Home <index>`

Docker
------

Using the Clyman Docker image is as simple as:

.. code-block:: bash

   docker run --publish=8768:8768 --publish=8762:8762/udp aostreetart/clyman:v2 clyman.prod.mongo=mongodb://mongo:27017

However, we also need a running instance of Mongo to do anything interesting.  The
above command assumes that you have an instance of Mongo running on the same
node with the container name 'mongo'.  To get you up and running quickly,
a Docker Compose file is provided.  To start up a Mongo instance and a Clyman
instance, simply run the following from the 'compose/min' folder:

.. code-block:: bash

   docker-compose up

Alternatively, you can deploy the stack with Docker Swarm using:

.. code-block:: bash

   docker stack deploy --compose-file compose/min/docker-compose.yml clyman-stack

Once the services have started, test them by hitting Clyman's healthcheck endpoint:

.. code-block:: bash

   curl http://localhost:8768/health

The Transaction (HTTP) API is available on port 8768, and the Event (UDP) API
is available on port 8762.  Keep in mind that this is not a secure deployment,
but is suitable for exploring the :ref:`Clyman API <api_index>`.

You may also continue on to the discussion of :ref:`How to Use Clyman <use>`.

Shutdown
--------
Shutdown of Clyman can be initiated with a kill or interrupt signal to the
container, or with 'docker stop'.  However, at least one udp message must be
received afterwards in order to successfully shut down the main event thread.
You can send one with:

.. code-block:: bash

   echo "kill" | nc -u $(ip addr show eth0 | grep -Po 'inet \K[\d.]+') 8762

Replacing 'eth0' with your network device, if necessary.

Latest Release
--------------

Download and unzip the latest release file from https://github.com/AO-StreetArt/CLyman/releases.

Once you have done this, you can run the easy_install script with the -d option
to install dependencies and the Clyman executable.  Alternatively, you can
simply run the install_deps.sh script from the scripts/ folder, and then run
the clyman executable from the main release folder.

.. code-block:: bash

   ./clyman

In order to run Clyman, you will need a Mongo Server installed locally.
Instructions can be found at https://docs.mongodb.com/manual/administration/install-on-linux/, or Mongo
can be started via a Docker image:

.. code-block:: bash

   docker run --name database -d -p 27017:27017 mongo:latest

Either way, the default connection for Clyman will connect without authentication.

You can move on to explore the :ref:`Clyman API <api_index>`, or
check out the :ref:`Configuration Section <configuration>` for more details
on the configuration options available when starting Clyman.

You may also continue on to the discussion of :ref:`How to Use Clyman <use>`.

Building from Source
--------------------

The recommended system for development of Clyman is either
Ubuntu 18.04 or CentOS7.  You will need gcc 6.0 or greater and gnu make
installed to successfully compile the program.

* Ubuntu

.. code-block:: bash

   sudo apt-get install gcc-6 g++-6
   export CC=gcc-6
   export CXX=g++-6

* Redhat

https://www.softwarecollections.org/en/scls/rhscl/devtoolset-6/

Next, you'll need to clone the repository and run the build_deps script.
This will install all of the required dependencies for Clyman, and may take
a while to run.

.. code-block:: bash

   git clone https://github.com/AO-StreetArt/CLyman.git
   mkdir clyman_deps
   cp CLyman/scripts/deb/build_deps.sh clyman_deps/build_deps.sh
   cd clyman_deps
   sudo ./build_deps.sh

You will also need to ensure that the POCO dependency is on the linker path,
which can be done with:

.. code-block:: bash

   export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"

Now, we can build Clyman:

.. code-block:: bash

   cd ../clyman
   make

This will result in creation of the clyman executable, which we can run
with the below command:

.. code-block:: bash

   ./clyman

When not supplied with any command line parameters, Clyman will look for an app.properties file to start from.

You may also build the test executable in the tests/ directory with:

.. code-block:: bash

   make tests

In order to run Clyman from a properties file, you will need:

-  A Mongo Server installed locally.  Instructions
   can be found at https://docs.mongodb.com/manual/administration/install-on-linux/

Neo4j can be started via a Docker image:

.. code-block:: bash

   docker run --name database -d -p 27017:27017 mongo:latest

Either way, the default connection for Clyman will connect without authentication.

You can move on to explore the :ref:`Clyman API <api_index>`, or
check out the :ref:`Configuration Section <configuration>` for more details
on the configuration options available when starting Clyman.

You may also continue on to the discussion of :ref:`How to Use Clyman <use>`.

Shutdown
--------
Shutdown of Clyman can be initiated with a kill or interrupt signal to the
main thread.  However, at least one udp message must be received afterwards
in order to successfully shut down the main event thread.  You can send one with:

.. code-block:: bash

   echo "kill" | nc -u $(ip addr show eth0 | grep -Po 'inet \K[\d.]+') 8764

Replacing 'eth0' with your network device, if necessary.
