#!/bin/bash

HOME_DIR=$1

cd $HOME_DIR && python 0-Meter/0-meter.py ci/obj_crt_bad.xml
tail $HOME_DIR/obj_crt.log
sudo docker logs --tail 25 clyman
cd $HOME_DIR && python 0-Meter/0-meter.py ci/obj_get.xml
tail $HOME_DIR/obj_get.log
sudo docker logs --tail 25 clyman
cd $HOME_DIR && python 0-Meter/0-meter.py ci/obj_upd.xml
tail $HOME_DIR/obj_upd.log
sudo docker logs --tail 25 clyman
cd $HOME_DIR && python 0-Meter/0-meter.py ci/obj_name_query.xml
tail $HOME_DIR/obj_name_query.log
sudo docker logs --tail 25 clyman
cd $HOME_DIR && python 0-Meter/0-meter.py ci/obj_type_query.xml
tail $HOME_DIR/obj_type_query.log
sudo docker logs --tail 25 clyman
cd $HOME_DIR && python 0-Meter/0-meter.py ci/obj_del.xml
tail $HOME_DIR/obj_del.log
sudo docker logs --tail 25 clyman
