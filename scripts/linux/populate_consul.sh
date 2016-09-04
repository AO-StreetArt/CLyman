#!/bin/bash

#Populate default configuration values into Redis

curl -X PUT -d 'couchbase://localhost/default' http://localhost:8500/v1/kv/DB_ConnectionString

curl -X PUT -d 'False' http://localhost:8500/v1/kv/DB_AuthenticationActive

curl -X PUT -d '' http://localhost:8500/v1/kv/DB_Password

curl -X PUT -d 'tcp://localhost:5556' http://localhost:8500/v1/kv/0MQ_OutboundConnectionString

curl -X PUT -d '127.0.0.1--6379----2--5--0' http://localhost:8500/v1/kv/RedisConnectionString

curl -X PUT -d 'True' http://localhost:8500/v1/kv/SmartUpdatesActive

curl -X PUT -d 'protocol-buffer' http://localhost:8500/v1/kv/MessageFormat

curl -X PUT -d 'protocol-buffer' http://localhost:8500/v1/kv/RedisBufferFormat

curl -X PUT -d '240' http://localhost:8500/v1/kv/Smart_Update_Buffer_Duration

curl -X PUT -d 'python scripts/CLyman_Heartbeat_Protobuf' http://localhost:8500/v1/kv/HealthCheckScript

curl -X PUT -d '20' http://localhost:8500/v1/kv/HealthCheckInterval

curl -X PUT -d 'True' http://localhost:8500/v1/kv/StampTransactionId

curl -X PUT -d 'True' http://localhost:8500/v1/kv/SendOutboundFailureMsg
