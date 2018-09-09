#!/bin/bash
set -e

#This script will attempt to build CLyman dependencies

#Based on CentOS 7
#Not intended for use with other OS (should function correctly with Red Hat Enterprise Linux 7, untested)
COMPILER="g++"
if [ "$#" -gt 0 ]; then
  COMPILER=$1
fi
INSTALL_OPT="-none"
if [ "$#" -gt 1 ]; then
  INSTALL_OPT=$2
fi

PRE=./downloads
RETURN=..
mkdir $PRE

# Set up a reasonably new version of gcc
yum -y install openssl-devel boost-devel centos-release-scl wget git gcc gcc-c++ cmake3 zlib-devel
yum -y update
yum -y install devtoolset-6
scl enable devtoolset-6 bash

#Build & Install the Shared Service Library
if [ ! -d /usr/local/include/aossl ]; then

  wget https://github.com/AO-StreetArt/AOSharedServiceLibrary/releases/download/v2.4.1/aossl-rhel-2.4.1.tar.gz
  tar -xvzf aossl-rhel-2.4.1.tar.gz

  #Build the dependencies for the shared service library
  mkdir $PRE/aossl_deps
  cp aossl-rhel/deps/build_deps.sh $PRE/aossl_deps/
  cd $PRE/aossl_deps && ./build_deps.sh $INSTALL_OPT
  cd ../$RETURN

  #Build the shared service library
  cd aossl-rhel && make clean && make CC=$COMPILER && make install
  cd ../

fi

# Build and install Mongocxx
if [ ! -d /usr/local/include/neocpp ]; then

  wget https://github.com/mongodb/mongo-c-driver/releases/download/1.12.0/mongo-c-driver-1.12.0.tar.gz
  tar xzf mongo-c-driver-1.12.0.tar.gz
  mkdir mongo-c-driver-1.12.0/cmake-build
  cd mongo-c-driver-1.12.0/cmake-build && cmake3 -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF .. && make && make install
  cd ../../

  wget https://github.com/mongodb/mongo-cxx-driver/archive/r3.3.1.tar.gz
  tar -xzf r3.3.1.tar.gz
  cd mongo-cxx-driver-r3.3.1/build && cmake3 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBSONCXX_POLY_USE_MNMLSTC=1 .. && make EP_mnmlstc_core && make && make install
  cd ../../

fi

# Install GLM
if [ ! -d /usr/local/include/glm ]; then

  wget https://github.com/g-truc/glm/releases/download/0.9.9.0/glm-0.9.9.0.zip
  unzip glm-0.9.9.0.zip
  mkdir /usr/local/include/glm/
  cp -r glm/glm/* /usr/local/include/glm/

fi

printf "Finished installing dependencies"
