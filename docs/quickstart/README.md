# Getting Started with CLyman

This guide will cover a few of the fastest means of getting started with a CLyman development environment.

## Docker

This will require Docker installed on the development computer.

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

For a more detailed discussion on the deployment of CLyman, please see the [Deployment] (https://github.com/AO-StreetArt/CLyman/tree/master/docs/deploy) section of the documentation.  For now, once you are into the Docker instance, you can run the below command to start CLyman:

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
