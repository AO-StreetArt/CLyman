################################################################

# Dockerfile to build CLyman Container Images
# Based on Ubuntu-ssh

################################################################

#Based on Ubuntu 16.04
FROM ubuntu:16.04

#Set the Maintainer
MAINTAINER Alex Barry

#Set up front end
ENV DEBIAN_FRONTEND noninteractive

#Setup basic environment tools
RUN apt-get update
RUN apt-get install -y apt-utils debconf-utils iputils-ping wget curl mc htop ssh software-properties-common
RUN	apt-get clean

#Setup necessary components for building the library
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN add-apt-repository -y ppa:cleishm/neo4j
RUN apt-get update
RUN apt-get install -y build-essential g++-5 libtool pkg-config autoconf automake cmake uuid-dev libhiredis-dev libcurl4-openssl-dev libevent-dev git libsnappy-dev liblog4cpp5-dev libssl-dev openssl neo4j-client libneo4j-client-dev libprotobuf-dev protobuf-compiler libglm-dev
RUN	apt-get clean

#Setup g++5 as the Compiler
RUN unlink /usr/bin/gcc && ln -s /usr/bin/gcc-5 /usr/bin/gcc
RUN unlink /usr/bin/g++ && ln -s /usr/bin/g++-5 /usr/bin/g++

RUN gcc --version

#Get the Mongo Dependencies, we build from source as the version provided by apt-get uses deprecated functions
RUN wget https://github.com/mongodb/mongo-c-driver/releases/download/1.6.3/mongo-c-driver-1.6.3.tar.gz
RUN tar xzf mongo-c-driver-1.6.3.tar.gz
RUN cd mongo-c-driver-1.6.3 && ./configure --disable-automatic-init-and-cleanup --with-libbson=bundled && make && make install

#Get the ZMQ Dependencies
RUN wget https://github.com/zeromq/zeromq4-1/releases/download/v4.1.4/zeromq-4.1.4.tar.gz

#Build & Install ZMQ
RUN tar -xvzf zeromq-4.1.4.tar.gz
RUN cd ./zeromq-4.1.4 && ./configure --without-libsodium && make && make install

#Get the ZMQ C++ Bindings
RUN git clone https://github.com/zeromq/cppzmq.git

#Get ZMQ C++ Header files into include path
RUN cp cppzmq/zmq.hpp /usr/local/include
RUN cp cppzmq/zmq_addon.hpp /usr/local/include

#Get the RapidJSON Dependency
RUN git clone https://github.com/miloyip/rapidjson.git

#Move the RapidJSON header files to the include path
RUN cp -r rapidjson/include/rapidjson/ /usr/local/include

#Get Hayai, for benchmarks
RUN git clone https://github.com/nickbruun/hayai.git
RUN cd hayai && cmake . && make && make install

#Run ldconfig to ensure that dependencies are on the linker path
RUN ldconfig

#Ensure we have access to the Protocol Buffer Interfaces
RUN mkdir $PRE/interfaces/
RUN git clone https://github.com/AO-StreetArt/DvsInterface.git $PRE/interfaces
RUN cd $PRE/interfaces && make install

#Pull the AOSSL source from github
RUN git clone https://github.com/AO-StreetArt/AOSharedServiceLibrary.git

#Install the shared service library
RUN cd AOSharedServiceLibrary && make && make install

#Pull the project source
RUN git clone https://github.com/AO-StreetArt/CLyman.git

#Build the project and tests
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
