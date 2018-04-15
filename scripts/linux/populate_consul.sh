#!/bin/bash

#Populate default configuration values into Consul

curl -X PUT -d 'mongodb://document-db:27017/' http://localhost:8500/v1/kv/clyman/DB_ConnectionString

curl -X PUT -d 'localhost:9092' http://localhost:8500/v1/kv/clyman/KafkaBrokerAddress

curl -X PUT -d 'True' http://localhost:8500/v1/kv/clyman/StampTransactionId

curl -X PUT -d 'False' http://localhost:8500/v1/kv/clyman/AtomicTransactions

curl -X PUT -d 'True' http://localhost:8500/v1/kv/clyman/ObjectLockingActive

curl -X PUT -d 'Json' http://localhost:8500/v1/kv/clyman/DataFormatType

curl -X PUT -d 'mydb' http://localhost:8500/v1/kv/clyman/Mongo_DbName

curl -X PUT -d 'test' http://localhost:8500/v1/kv/clyman/Mongo_DbCollection
