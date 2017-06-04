#!/bin/bash

HOME_DIR=$1

cd $HOME_DIR && python 0-Meter/0-meter.py ci/obj_crt.xml
tail $HOME_DIR/obj_crt.log
sudo docker logs --tail 25 clyman
