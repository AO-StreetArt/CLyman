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

#Next
Now that you have a working instance of CLyman, you can move on to the [Configuration] (https://github.com/AO-StreetArt/CLyman/tree/master/docs/config) Section.
