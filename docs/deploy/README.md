# Docker

The Development Docker image for CLyman-Dev is ready for active use, and can be run with the command:

`docker run --name clyman -d aostreetart/clyman-dev`

Congratulations, you've got a fully functional build & test environment for CLyman!  You can connect to it with:

`sudo docker exec -i -t aossl-devel /bin/bash`

## Connecting Clyman to other Services with Docker



## Building the Docker Image

Should you so desire, the docker image can be rebuilt using the below commands

`docker pull awbarry00/clyman-dev`

`sudo docker build -t clyman_dev_2 .`

`docker run --name clyman -it -d -P clyman_dev_2`

### Production
The current recommended deployment for production is to build from source and scale manually.  However, rather than deploying a Couchbase Server & Redis Server locally beside CLyman, we should be connecting to a Couchbase Cluster & Redis Cluster on separate nodes.

Based on testing & evaluation, recommended deployment may change to go through Docker Images currently in development.

# Next
Now that you have deployed CLyman, and want to contribute back by developing, you can move on to the [Atchitecture] (https://github.com/AO-StreetArt/CLyman/tree/master/docs/arch) section.
