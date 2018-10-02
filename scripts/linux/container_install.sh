#!/bin/bash
#This script will install Crazy Ivan by cloning it from github and building from source

set -e

BRANCH="v2"

if [ "$#" -gt 0 ]; then
  BRANCH=$1
fi

apt-get update
apt-get -y install software-properties-common build-essential g++ make
add-apt-repository -y ppa:ubuntu-toolchain-r/test
apt-get -y update
apt-get -y install git g++-6
export CXX=g++-6
export CC=gcc-6
git clone --depth=50 --branch=$BRANCH https://github.com/AO-StreetArt/CLyman.git
mkdir clyman_deps
cp CLyman/scripts/linux/deb/build_deps.sh clyman_deps
cd clyman_deps && ./build_deps.sh g++-6 -no-poco
export LD_LIBRARY_PATH="/usr/local/lib:/usr/local/lib64:$LD_LIBRARY_PATH"
cd ../CLyman && make && make test
cp clyman /usr/bin
mkdir /etc/clyman
cp app.properties /etc/clyman/
