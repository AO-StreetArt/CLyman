################################################################

# Dockerfile to build CLyman Container Images
# Based on Ubuntu-ssh

################################################################

#Based on Ubuntu 14.04
FROM ubuntu:14.04

#Set the Maintainer
MAINTAINER Alex Barry

#Set up front end
ENV DEBIAN_FRONTEND noninteractive

#Ensure that base level build requirements are satisfied
RUN apt-get update
RUN	apt-get install -y apt-utils debconf-utils iputils-ping wget curl mc htop ssh
RUN	apt-get clean

#Build the dependencies and place them in the correct places
RUN apt-get update

#Ensure that specific build requirements are satisfied
RUN apt-get install -y build-essential libtool pkg-config autoconf automake uuid-dev libhiredis-dev libcurl4-openssl-dev libevent-dev git software-properties-common

#Get the RapidJSON Dependency
RUN git clone https://github.com/miloyip/rapidjson.git

#Move the RapidJSON header files to the include path
RUN cp -r rapidjson/include/rapidjson/ /usr/local/include

#Get the Mongo Dependencies
RUN git clone https://github.com/mongodb/mongo-c-driver.git
RUN cd mongo-c-driver && ./autogen.sh --with-libbson=bundled && make && sudo make install

#Get the Redis Dependencies
RUN git clone https://github.com/redis/hiredis.git ./hiredis

RUN cd ./hiredis && make && make install

#Get the ZMQ Dependencies
RUN wget https://github.com/zeromq/zeromq4-1/releases/download/v4.1.4/zeromq-4.1.4.tar.gz

#Build & Install ZMQ

#Unzip the ZMQ Directories
RUN tar -xvzf zeromq-4.1.4.tar.gz

#Configure, make, & install
RUN cd ./zeromq-4.1.4 && ./configure --without-libsodium && make && make install

#Run ldconfig to ensure that ZMQ is on the linker path
RUN ldconfig

#Get the ZMQ C++ Bindings
RUN git clone https://github.com/zeromq/cppzmq.git

#Get ZMQ C++ Header files into include path
RUN cp cppzmq/zmq.hpp /usr/local/include
RUN cp cppzmq/zmq_addon.hpp /usr/local/include

#hayai, for compiling benchmarks
RUN apt-add-repository -y ppa:bruun/hayai

#Update the apt-get cache
RUN apt-get update

#Install the dependencies
RUN apt-get install -y build-essential libprotobuf-dev protobuf-compiler liblog4cpp5-dev libhayai-dev

#Ensure we have access to the Protocol Buffer Interfaces
RUN mkdir $PRE/interfaces/
RUN git clone https://github.com/AO-StreetArt/DvsInterface.git $PRE/interfaces
RUN cd $PRE/interfaces && sudo make install

#Pull the project source from github
RUN git clone https://github.com/AO-StreetArt/AOSharedServiceLibrary.git

#Install the shared service library
RUN cd AOSharedServiceLibrary && make && make install

#Get the Eigen Dependencies
RUN wget http://bitbucket.org/eigen/eigen/get/3.2.8.tar.bz2

#Move the Eigen Header files to the include path

#Unzip the Eigen directories
RUN tar -vxjf 3.2.8.tar.bz2
RUN mv eigen-eigen* eigen

#Move the files
RUN cp -r eigen/Eigen /usr/local/include

#Pull the project source from github
RUN git clone https://github.com/AO-StreetArt/CLyman.git

RUN cd CLyman && make

#Expose some of the default ports
EXPOSE 22
EXPOSE 5555
EXPOSE 5556
EXPOSE 8091
EXPOSE 8092
EXPOSE 8093
EXPOSE 11210
EXPOSE 12345

#Start up the SSH terminal so that we can connect & start the app
CMD tail -f /dev/null
