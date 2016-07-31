## How to Use CLyman
CLyman has three recommended deployment types:

### Development

Those wishing to develop on CLyman have two options.  Preferably, they can use Docker to set up a build & test environment with a few simple steps.  Otherwise, the module will need to be built from source.

#### Docker

You will need to have an instance of Couchbase running to use CLyman, and this can be done via the below command (full instructions can be found [here] (https://docs.docker.com/engine/examples/couchbase/)):

`docker run -d --name db -p 8091-8093:8091-8093 -p 11210:11210 couchbase`

You will also need an instance of Redis Running to use CLyman.  This can be done via the below command (full instructions can be found [here] (https://hub.docker.com/_/redis/)):

`docker run --name some-redis -d redis`

The Development Docker image for CLyman-Dev is ready for active use, and can be run with the command:

`docker run --name clyman -d awbarry00/clyman-dev`

Congratulations, you've got a fully functional build & test environment for CLyman!

SSH keys are provided with the repository.  In order to run the development image for CLyman, you will need to access the Docker Process via SSH.  Access is granted with below set of commands:

    ssh-agent -s
    ssh-add ssh/id_rsa
    ssh root@localhost -p `sudo docker port ssh 22 | cut -d":" -f2`


## Configuration

CLyman can be configured via a properties file, which has a few command line options:

* `./lyman -dev` - This will start CLyman in dev mode
* `./lyman` - This will start CLyman with the default properties file, lyman.properties
* `./lyman -config-file=file.properties` - This will start CLyman with the properties file, file.properties.  Can be combined with -log-conf.
* `./lyman -log-conf=logging.properties` - This will start CLyman with the logging properties file, logging.properties.  Can be combined with -config-file.

Clyman can also be configured via a Consul Connection, in which we must specify the address of the consul agent, and the ip & port of the Inbound ZeroMQ Connection.

* `./lyman -consul-addr=localhost:8500 -ip=localhost -port=5555` - Start Clyman, register as a service with consul, and configure based on configuration values in Consul, and bind to an internal 0MQ port on localhost
* `./lyman -consul-addr=localhost:8500 -ip=tcp://my.ip -port=5555` - Start Clyman, register as a service with consul, and configure based on configuration values in Consul, and bind to an external 0MQ port on tcp://my.ip

The properties file, lyman.properties can be edited in any text editor, and when configuring from Consul the keys of the properties file are equal to the expected keys in Consul.

The Logging Configuration File can also be edited with a text file, and the documentation for this can be found [here] (http://log4cpp.sourceforge.net/api/classlog4cpp_1_1PropertyConfigurator.html).

Below you can find a summary of the options in the Properties File or Consul Key-Value Store:

### DB
* DB_ConnectionString - The string used to connect to the couchbase instance (example: couchbase://localhost/default)
* DB_AuthenticationActive - True or False, whether or not Database Authentication is active on the couchbase server
* DB_Password - If DB Authentication is active, then this is used as the password to connect to the couchbase instance

### 0MQ
* 0MQ_OutboundConnectionString - The connectivity string for the outbound 0MQ Port
* 0MQ_InboundConnectionString - The connectivity string for the inbound 0MQ Port

### Redis Connection
* RedisConnectionString - We have a list of Redis Connections in our cluster in the format "ip--port--password--poolsize--timeout--role"

### Behavior
* SmartUpdatesActive - True if we want to allow partial messages, and only update fields included in the partial messages.  False if we want to enforce full replace update messages.
* MessageFormat - json to take in and read out JSON Format, protocol-buffer to take in and read out Google Protocol Buffer Format
* RedisBufferFormat - json to read and write JSON format to the Redis Buffer, protocol-buffer to read and write Protocol Buffers to the Redis Buffer
