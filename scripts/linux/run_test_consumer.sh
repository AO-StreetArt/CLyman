#!/bin/bash

docker run -i -t -d --network=dvs --name=test_consumer aostreetart/kafkadockerconsumer _dvs queue:9092
