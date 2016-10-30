#!/bin/bash

#Populate default configuration values into Redis

curl -X PUT -d 'couchbase://localhost/default' http://localhost:8500/v1/kv/clyman/DB_ConnectionString

curl -X PUT -d 'False' http://localhost:8500/v1/kv/clyman/DB_AuthenticationActive

curl -X PUT -d '' http://localhost:8500/v1/kv/clyman/DB_Password

curl -X PUT -d 'tcp://localhost:5556' http://localhost:8500/v1/kv/clyman/0MQ_OutboundConnectionString

curl -X PUT -d '127.0.0.1--6379----2--5--0' http://localhost:8500/v1/kv/clyman/RedisConnectionString

curl -X PUT -d 'True' http://localhost:8500/v1/kv/clyman/SmartUpdatesActive

curl -X PUT -d 'protocol-buffer' http://localhost:8500/v1/kv/clyman/MessageFormat

curl -X PUT -d 'protocol-buffer' http://localhost:8500/v1/kv/clyman/RedisBufferFormat

curl -X PUT -d '240' http://localhost:8500/v1/kv/clyman/Smart_Update_Buffer_Duration

curl -X PUT -d 'python scripts/CLyman_Heartbeat_Protobuf' http://localhost:8500/v1/kv/clyman/HealthCheckScript

curl -X PUT -d '20' http://localhost:8500/v1/kv/clyman/HealthCheckInterval

curl -X PUT -d 'True' http://localhost:8500/v1/kv/clyman/StampTransactionId

curl -X PUT -d 'True' http://localhost:8500/v1/kv/clyman/SendOutboundFailureMsg

curl -X PUT -d 'True' http://localhost:8500/v1/kv/clyman/EnableObjectLocking
