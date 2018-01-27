################################################################

# Dockerfile to build CLyman Container Images

################################################################

#Based on Ubuntu 16.04
FROM ubuntu:16.04

#Set the Maintainer
MAINTAINER Alex Barry

#Set up front end
ENV DEBIAN_FRONTEND noninteractive

ADD ./scripts/linux/deb/build_deps.sh .

#Setup basic environment tools
RUN ./build_deps.sh

ENV LD_LIBRARY_PATH=/usr/local/lib

#Add the project source to the current directory in the container
ADD . CLyman/

#Build the project and tests
RUN ls CLyman
RUN cd CLyman && make && make test

#Expose some of the default ports
EXPOSE 22
EXPOSE 5555
EXPOSE 5556
EXPOSE 8091
EXPOSE 8092
EXPOSE 8093
EXPOSE 11210
EXPOSE 12345

#Expose the 5000 port range for DVS Services
EXPOSE 5000-5999

#Start up the SSH terminal so that we can connect & start the app
ENTRYPOINT ["CLyman/clyman"]
