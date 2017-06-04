#!/bin/bash

HOME_DIR=$1

sudo unlink /usr/bin/gcc && sudo ln -s /usr/bin/gcc-5 /usr/bin/gcc
sudo unlink /usr/bin/g++ && sudo ln -s /usr/bin/g++-5 /usr/bin/g++
# Print the version of gcc
gcc --version
# Print the python & pip version
python -V
pip -V
# Install python protocol buffer & zmq modules
sudo pip install protobuf pyzmq
pip install protobuf pyzmq
sudo pip install --user protobuf pyzmq
# Get DVS Interfaces
git clone https://github.com/AO-StreetArt/DvsInterface.git
cd DvsInterface && sudo make install
sudo docker network create dvs
# Clone the repository for the python tests
cd $HOME_DIR
git clone https://github.com/AO-StreetArt/0-Meter.git
