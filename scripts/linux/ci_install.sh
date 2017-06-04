#!/bin/bash

HOME_DIR=$1
BRANCH=$2

cd $HOME_DIR
# Setup Consul
- sudo docker pull consul
- sudo docker run -d --name=registry --network=dvs consul
# Set up Neo4j
- sudo docker pull mongo
- sudo docker run -d --network=dvs --name=document-db mongo
# Set up Redis
- sudo docker pull redis
- sudo docker run --network=dvs --name=cache -d redis
# Populate Consul Configuration variables
- sudo docker exec -t registry curl -X PUT -d 'cache--6379----2--5--0' http://localhost:8500/v1/kv/clyman/RedisConnectionString
- sudo docker exec -t registry curl -X PUT -d 'mongodb://document-db:27017/' http://localhost:8500/v1/kv/clyman/Mongo_ConnectionString
- sudo docker exec -t registry curl -X PUT -d 'mydb' http://localhost:8500/v1/kv/clyman/Mongo_DbName
- sudo docker exec -t registry curl -X PUT -d 'test' http://localhost:8500/v1/kv/clyman/Mongo_DbCollection
- sudo docker exec -t registry curl -X PUT -d 'True' http://localhost:8500/v1/kv/clyman/StampTransactionId
- sudo docker exec -t registry curl -X PUT -d 'True' http://localhost:8500/v1/kv/clyman/AtomicTransactions
- sudo docker exec -t registry curl -X PUT -d 'Json' http://localhost:8500/v1/kv/clyman/Data_Format_Type
# Start CLyman
- cd $HOME_DIR/scripts/linux && ./build_docker_instance.sh $BRANCH $HOME_DIR
- cd $HOME_DIR/scripts/linux && ./run_docker_instance.sh $BRANCH registry:8500 localhost 5556
- sleep 5
- sudo docker logs --tail 75 clyman
