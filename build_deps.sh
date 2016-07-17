#!/bin/bash
exec 3>&1 4>&2
trap 'exec 2>&4 1>&3' 0 1 2 3
exec 1>build_deps.log 2>&1

#This script will attempt to build CLyman dependencies

#Based on Ubuntu 14.04 LTS
#Not intended for use with other OS (should function correctly with Debian 7, untested)

printf "Creating Dependency Folder"
PRE=./downloads
mkdir $PRE

printf "Calling apt-get update"

#Update the Ubuntu Server
sudo apt-get -y update

#Build the dependencies and place them in the correct places


#Determine if we Need Eigen
if [ ! -d /usr/local/include/Eigen ]; then

  printf "Downloading Eigen"

  mkdir $PRE/eigen

  #Get the Eigen Dependencies
  wget http://bitbucket.org/eigen/eigen/get/3.2.8.tar.bz2

  #Move the Eigen Header files to the include path

  #Unzip the Eigen directories
  tar -vxjf 3.2.8.tar.bz2
  mv ./eigen-eigen* $PRE/eigen

  #Move the files
  sudo cp -r $PRE/eigen/eigen*/Eigen /usr/local/include

fi

#Determine if we Need RapidJSON
if [ ! -d /usr/local/include/rapidjson ]; then

  printf "Cloning RapidJSON"

  mkdir $PRE/rapidjson

  #Get the RapidJSON Dependency
  git clone https://github.com/miloyip/rapidjson.git $PRE/rapidjson

  #Move the RapidJSON header files to the include path
  sudo cp -r $PRE/rapidjson/include/rapidjson/ /usr/local/include

fi

#Build & Install the Shared Service Library

#Create the folder to clone into
mkdir $PRE/aossl

#Pull the code down
git clone https://github.com/AO-StreetArt/AOSharedServiceLibrary.git $PRE/aossl

#Build the dependencies for the shared service library
mkdir $PRE/aossl_deps
cp $PRE/aossl/build_deps.sh $PRE/aossl_deps/
cd $PRE/aossl_deps && sudo ./build_deps.sh

#Build the shared service library
cd $PRE/aossl && ./build_project.sh

#Now we have a few things:
#1. A compiled shared library libaossl.a.x.y that needs to be put on the linker path
#2. A set of header files in the lib/include directory that need to be put onto the include path

#Shared Library
sudo cp $PRE/aossl/libaossl.a.* /usr/local/bin

#Header Files
sudo mkdir /usr/local/include/aossl
sudo cp $PRE/aossl/lib/include/* /usr/local/include/aossl

printf "Finished installing dependencies"
