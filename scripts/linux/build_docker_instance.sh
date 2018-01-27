#!/bin/bash

BRANCH_NAME=$1
DIR=$2

TAG_NAME="latest"

echo $BRANCH_NAME
echo $DIR

if [ $BRANCH_NAME != "master" ]; then
  cd $DIR && sudo docker build -t "aostreetart/clyman:$BRANCH_NAME" .
  TAG_NAME=$BRANCH_NAME
else
  cd $DIR && sudo docker build -t "aostreetart/clyman:$TAG_NAME" .
fi
