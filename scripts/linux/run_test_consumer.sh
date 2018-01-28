#!/bin/bash

docker run -i -t -d --network=dvs --name=test_consumer aostreetart/kafkatestconsumer _dvs queue:9092
