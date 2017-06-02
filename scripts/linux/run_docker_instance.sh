#!/bin/bash

BRANCH_NAME=${1:-master}
CONSUL_ADDR=${2:-"localhost:8500"}
IP=${3:-localhost}
PORT=${4:-5556}

TAG_NAME="latest"

if [ $BRANCH_NAME != "master" ]; then
  TAG_NAME=$BRANCH_NAME
fi

docker run --name clyman --network=dvs -p $PORT:$PORT -d aostreetart/clyman:$TAG_NAME -consul-addr=$CONSUL_ADDR -ip=$IP -port=$PORT -log-conf=CLyman/log4cpp.properties
