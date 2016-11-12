#!/bin/bash

#Populate default configuration values into Redis

curl -X PUT -d 'mongodb://localhost:27017/' http://localhost:8500/v1/kv/clyman/DB_ConnectionString

curl -X PUT -d 'mycoll' http://localhost:8500/v1/kv/clyman/DB_CollectionName

curl -X PUT -d 'mydb' http://localhost:8500/v1/kv/clyman/DB_Name

curl -X PUT -d 'tcp://*:5556' http://localhost:8500/v1/kv/clyman/0MQ_OutboundConnectionString

curl -X PUT -d '127.0.0.1--6379----2--5--0' http://localhost:8500/v1/kv/clyman/RedisConnectionString

curl -X PUT -d 'protocol-buffer' http://localhost:8500/v1/kv/clyman/MessageFormat

curl -X PUT -d 'True' http://localhost:8500/v1/kv/clyman/StampTransactionId

curl -X PUT -d 'True' http://localhost:8500/v1/kv/clyman/AtomicTransactions

curl -X PUT -d 'True' http://localhost:8500/v1/kv/clyman/EnableObjectLocking

curl -X PUT -d 'True' http://localhost:8500/v1/kv/clyman/GlobalTransforms
