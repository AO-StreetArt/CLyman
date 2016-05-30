#!/bin/bash

#This script will attempt to build CLyman dependencies

#Based on Ubuntu 14.04 LTS
#Not intended for use with other OS (may function correctly with Debian 7, untested)

printf "Creating Dependency Folder"
PRE=../clyman_deps
mkdir $PRE

printf "Calling apt-get update"

#Update the Ubuntu Server
sudo apt-get -y update

#Build the dependencies and place them in the correct places

printf "Addressing pre-build requirements"

#Ensure that specific build requirements are satisfied
sudo apt-get install build-essential libtool pkg-config autoconf automake uuid-dev

printf "Cloning RapidJSON"

#Get the RapidJSON Dependency
git clone https://github.com/miloyip/rapidjson.git
mv rapidjson $PRE/rapidjson

#Move the RapidJSON header files to the include path
sudo cp -r $PRE/rapidjson/include/rapidjson/ /usr/local/include

printf "Pulling Down Repositories for Couchbase Client"

#Get the Couchbase dependecies
wget -P $PRE http://packages.couchbase.com/releases/couchbase-release/couchbase-release-1.0-2-amd64.deb
sudo dpkg -i $PRE/couchbase-release-1.0-2-amd64.deb

printf "Downloading Eigen"

#Get the Eigen Dependencies
wget -P $PRE http://bitbucket.org/eigen/eigen/get/3.2.8.tar.bz2

#Move the Eigen Header files to the include path

#Unzip the Eigen directories
tar -C $PRE -vxjf deps/3.2.8.tar.bz2

#Move the files
sudo cp -r $PRE/eigen-eigen*/Eigen /usr/local/include

printf "Getting ZMQ"

#Get the ZMQ Dependencies
wget -P $PRE https://github.com/zeromq/zeromq4-1/releases/download/v4.1.4/zeromq-4.1.4.tar.gz

#Build & Install ZMQ

#Unzip the ZMQ Directories
tar -C $PRE/zeromq-4.1.4 -xvzf zeromq-4.1.4.tar.gz

printf "Building ZMQ"

#Configure
.$PRE/zeromq-4.1.4/configure --without-libsodium

#Make
make

#Sudo Make Install
sudo make install

printf "Cloning ZMQ C++ Bindings"

#Get the ZMQ C++ Bindings
git clone https://github.com/zeromq/cppzmq.git
mv cppzmq $PRE/cppzmq

#Get ZMQ C++ Header files into include path
sudo cp $PRE/cppzmq/zmq.hpp /usr/local/include
sudo cp $PRE/cppzmq/zmq_addon.hpp /usr/local/include

printf "Update cache and install final dependencies through apt-get"

#Update the apt-get cache
sudo apt-get update

#Install the dependencies
sudo apt-get install libcouchbase-dev libcouchbase2-bin libprotobuf-dev protobuf-compiler liblog4cpp5-dev

printf "Finished installing dependencies"

