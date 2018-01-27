#!/bin/bash

#This script will attempt to build basic project dependencies

#Based on CentOS 7

PRE=./downloads
RETURN=..
mkdir $PRE

# Update the Server
sudo yum -y update

# Build & Install the Shared Service Library

if [ ! -d /usr/local/include/aossl ]; then

  # Create the folder to clone into
  mkdir $PRE/aossl

  # Get the latest code from the master branch on github
  # You may alternatively download a release from the github releases page and use that
  git clone https://github.com/AO-StreetArt/AOSharedServiceLibrary.git $PRE/aossl

  #Build the dependencies for the shared service library
  mkdir $PRE/aossl_deps
  cp $PRE/aossl/scripts/rhel/build_deps.sh $PRE/aossl_deps/
  cd $PRE/aossl_deps && sudo ./build_deps.sh
  cd ../$RETURN

  #Build the shared service library
  cd $PRE/aossl && make && sudo make install
  sudo ldconfig

fi

#Install glm, protocol buffers, boost
sudo yum install -y libglm-devel protobuf-devel protobuf-compiler boost-devel

# Here we look to install RapidJSON

# This is a recommended library for JSON Processing.
# Libprotobuf and protoc are also installed by default, for using Google Protocol Buffers.
# If you wish to use other parsing methods or message formats, simply remove these
if [ ! -d /usr/local/include/rapidjson ]; then

  printf "Cloning RapidJSON\n"

  mkdir $PRE/rapidjson

  #Get the RapidJSON Dependency
  git clone https://github.com/miloyip/rapidjson.git $PRE/rapidjson

  #Move the RapidJSON header files to the include path
  sudo cp -r $PRE/rapidjson/include/rapidjson/ /usr/local/include

fi

# Install librdkafka
if [ ! -d /usr/local/include/librdkafka ]; then
  wget https://github.com/edenhill/librdkafka/archive/v0.11.3.tar.gz
  tar -xvzf v0.11.3.tar.gz
  cd librdkafka-0.11.3 && ./configure && make && sudo make install
fi

# Here we look to install cppkafka
if [ ! -d /usr/local/include/cppkafka ]; then
  printf "Cloning CppKafka\n"

  mkdir $PRE/cppkafka

  #Get the RapidJSON Dependency
  git clone https://github.com/mfontanini/cppkafka.git $PRE/cppkafka

  # Build and install
  mkdir $PRE/cppkafka/build && cd $PRE/cppkafka/build && cmake .. && make && sudo make install

fi

#Get the DVS Interface Protocol Buffer Library
git clone https://github.com/AO-StreetArt/DvsInterface.git
cd DvsInterface && sudo make install

printf "Finished installing dependencies\n"
