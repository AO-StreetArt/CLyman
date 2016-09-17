# Docker

The Production Docker Image is under construction, and can be run with the command:

`docker run --name clyman -d aostreetart/clyman`

`sudo docker exec -i -t clyman /bin/bash`

## Connecting Clyman to other Services with Docker



## Building the Docker Image

Should you so desire, the docker image can be rebuilt using the below commands

`docker pull awbarry00/clyman-dev`

`sudo docker build -t clyman_dev_2 .`

`docker run --name clyman -it -d -P clyman_dev_2`

# Next
Now that you have deployed CLyman, and want to contribute back by developing, you can move on to the [Atchitecture] (https://github.com/AO-StreetArt/CLyman/tree/master/docs/arch) section.
