#!/bin/bash
set -e
#This script will attempt to build CLyman dependencies

#Based on Ubuntu 14.04 LTS
#Not intended for use with other OS (should function correctly with Debian 7, untested)

PRE=./downloads
mkdir $PRE

#Update the Ubuntu Server
apt-get -y update
apt-get install -y git

#Build & Install the Shared Service Library

if [ ! -d /usr/local/include/aossl ]; then

  #Create the folder to clone into
  mkdir $PRE/aossl

  #Pull the code down
  git clone https://github.com/AO-StreetArt/AOSharedServiceLibrary.git $PRE/aossl

  #Build the dependencies for the shared service library
  mkdir $PRE/aossl_deps
  cp $PRE/aossl/scripts/deb/build_deps.sh $PRE/aossl_deps/
  cd $PRE/aossl_deps
  ./build_deps.sh
  cd ../..

  #Build the shared service library
  cd $PRE/aossl
  make
  make install
  ldconfig
  cd ../..

fi

#Install glm, protocol buffers, boost
apt-get install -y libglm-dev libprotobuf-dev protobuf-compiler libboost-all-dev

# Here we look to install RapidJSON
if [ ! -d /usr/local/include/rapidjson ]; then
  printf "Cloning RapidJSON\n"

  mkdir $PRE/rapidjson

  #Get the RapidJSON Dependency
  git clone https://github.com/miloyip/rapidjson.git $PRE/rapidjson

  #Move the RapidJSON header files to the include path
  cp -r $PRE/rapidjson/include/rapidjson/ /usr/local/include

fi

# Install librdkafka
if [ ! -d /usr/local/include/librdkafka ]; then
  wget https://github.com/edenhill/librdkafka/archive/v0.11.3.tar.gz
  tar -xvzf v0.11.3.tar.gz
  cd librdkafka-0.11.3 && ./configure && make && make install
  cd ..
fi

# Here we look to install cppkafka
if [ ! -d /usr/local/include/cppkafka ]; then
  printf "Cloning CppKafka\n"

  mkdir $PRE/cppkafka

  #Get the RapidJSON Dependency
  git clone https://github.com/mfontanini/cppkafka.git $PRE/cppkafka

  # Build and install
  mkdir $PRE/cppkafka/build && cd $PRE/cppkafka/build && cmake .. && make && make install

fi

#Get the DVS Interface Protocol Buffer Library
git clone https://github.com/AO-StreetArt/DvsInterface.git
cd DvsInterface && make install

printf "Finished installing dependencies\n"
