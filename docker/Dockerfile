################################################################

# Dockerfile to build CLyman Container Images
# Based on Ubuntu-ssh

################################################################

#Based on Ubuntu 14.04
FROM ubuntu:14.04

#Set the Maintainer
MAINTAINER Alex Barry

#Set up SSH to allow configuration, starting, & stopping of service
ENV DEBIAN_FRONTEND noninteractive

ADD ssh/id_rsa.pub /root/.ssh/authorized_keys

RUN apt-get update; \
	apt-get install -y apt-utils debconf-utils iputils-ping wget curl mc htop ssh; \
	apt-get clean; \
	locale-gen en_US.UTF-8; update-locale LANG=en_US.UTF-8 LANGUAGE=en_US.UTF-8 LC_ALL=en_US.UTF-8; \
	sed -i 's/#PasswordAuthentication yes/PasswordAuthentication no/' /etc/ssh/sshd_config; \
	sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd; \
	chmod 700 /root/.ssh; chmod 600 /root/.ssh/authorized_keys; \
	sed -i 's/^exit 0/service ssh start\nexit 0/' /etc/rc.local

#Build the dependencies and place them in the correct places
RUN apt-get update

#Ensure that specific build requirements are satisfied
RUN apt-get install -y build-essential libtool pkg-config autoconf automake uuid-dev git libhiredis-dev

#Get the RapidJSON Dependency
RUN git clone https://github.com/miloyip/rapidjson.git

#Move the RapidJSON header files to the include path
RUN cp -r rapidjson/include/rapidjson/ /usr/local/include

#Get the Couchbase dependecies
RUN wget http://packages.couchbase.com/releases/couchbase-release/couchbase-release-1.0-2-amd64.deb
RUN dpkg -i couchbase-release-1.0-2-amd64.deb

#Get the Eigen Dependencies
RUN wget http://bitbucket.org/eigen/eigen/get/3.2.8.tar.bz2

#Move the Eigen Header files to the include path

#Unzip the Eigen directories
RUN tar -vxjf 3.2.8.tar.bz2
RUN mv eigen-eigen* eigen

#Move the files
RUN cp -r eigen/Eigen /usr/local/include

#Get the XRedis Dependencies
RUN git clone https://github.com/0xsky/xredis.git ./xredis

RUN cd ./xredis && make && make install

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

#Update the apt-get cache
RUN apt-get update

#Install the dependencies
RUN apt-get install -y libcouchbase-dev libcouchbase2-bin build-essential libprotobuf-dev protobuf-compiler liblog4cpp5-dev

#Expose the necessary ports for Lyman to function
EXPOSE 22
EXPOSE 5555
EXPOSE 5556
EXPOSE 8091
EXPOSE 8092
EXPOSE 8093
EXPOSE 11210

#Pull the project source from github
RUN git clone https://github.com/AO-StreetArt/CLyman.git

#Build the project itself
#RUN ./CLyman/build_project

#Start up the SSH terminal so that we can connect & start the app
CMD /etc/rc.local; bash
