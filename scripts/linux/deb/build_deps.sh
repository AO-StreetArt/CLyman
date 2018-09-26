#!/bin/bash
set -e
#This script will attempt to build CLyman dependencies

#Based on Ubuntu 18.04 LTS
#Not intended for use with other OS (should function correctly with Debian 7, untested)

COMPILER="g++"
if [ "$#" -gt 0 ]; then
  COMPILER=$1
fi
INSTALL_OPT="-none"
if [ "$#" -gt 1 ]; then
  INSTALL_OPT=$2
fi

printf "Creating Dependency Folder"
PRE=./downloads
RETURN=..
mkdir $PRE

printf "Calling apt-get update"

#Update the Ubuntu Server
apt-get -y update
apt-get install -y git libboost-all-dev openssl libssl-dev wget libz-dev cmake

# Build and install Mongocxx
if [ ! -d /usr/local/include/mongocxx ]; then

  wget https://github.com/mongodb/mongo-c-driver/releases/download/1.12.0/mongo-c-driver-1.12.0.tar.gz
  tar xzf mongo-c-driver-1.12.0.tar.gz
  mkdir mongo-c-driver-1.12.0/cmake-build
  cd mongo-c-driver-1.12.0/cmake-build && cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF .. && make && make install
  cd ../../

  wget https://github.com/mongodb/mongo-cxx-driver/archive/r3.3.1.tar.gz
  tar -xzf r3.3.1.tar.gz
  cd mongo-cxx-driver-r3.3.1/build && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBSONCXX_POLY_USE_MNMLSTC=1 .. && make EP_mnmlstc_core && make && make install
  cd ../../

fi

#Build & Install the Shared Service Library
if [ ! -d /usr/local/include/aossl ]; then

  wget https://github.com/AO-StreetArt/AOSharedServiceLibrary/releases/download/v2.4.3/aossl-deb-2.4.3.tar.gz
  tar -xvzf aossl-deb-2.4.3.tar.gz

  #Build the dependencies for the shared service library
  mkdir $PRE/aossl_deps
  cp aossl-deb/deps/build_deps.sh $PRE/aossl_deps/
  cd $PRE/aossl_deps && ./build_deps.sh $INSTALL_OPT
  cd ../$RETURN

  #Build the shared service library
  cd aossl-deb && make clean && make CC=$COMPILER && make install
  cd ../

fi

# Install GLM
if [ ! -d /usr/local/include/glm ]; then

  wget https://github.com/g-truc/glm/releases/download/0.9.9.0/glm-0.9.9.0.zip
  unzip glm-0.9.9.0.zip
  mkdir /usr/local/include/glm/
  cp -r glm/glm/* /usr/local/include/glm/

fi

printf "Finished installing dependencies"
