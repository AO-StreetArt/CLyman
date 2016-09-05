# Configuration
## Properties File

CLyman can be configured via a properties file, which has a few command line options:

* `./lyman -dev` - This will start CLyman in dev mode
* `./lyman` - This will start CLyman with the default properties file, lyman.properties
* `./lyman -config-file=file.properties` - This will start CLyman with the properties file, file.properties.  Can be combined with -log-conf.
* `./lyman -log-conf=logging.properties` - This will start CLyman with the logging properties file, logging.properties.  Can be combined with -config-file.

The properties file can be edited in any text editor.

## Consul

Clyman can also be configured via a Consul Connection, in which we must specify the address of the consul agent, and the ip & port of the Inbound ZeroMQ Connection.

* `./lyman -consul-addr=localhost:8500 -ip=localhost -port=5555` - Start Clyman, register as a service with consul, and configure based on configuration values in Consul, and bind to an internal 0MQ port on localhost
* `./lyman -consul-addr=localhost:8500 -ip=tcp://my.ip -port=5555 -log-conf=logging.properties` - Start Clyman, register as a service with consul, and configure based on configuration values in Consul.  Bind to an external 0MQ port on tcp://my.ip, and configure from the logging configuration file, logging.properties.

When configuring from Consul the keys of the properties file are equal to the expected keys in Consul.

## Logging

The Logging Configuration File can also be edited with a text file, and the documentation for this can be found [here] (http://log4cpp.sourceforge.net/api/classlog4cpp_1_1PropertyConfigurator.html).  Note that logging configuration is not yet in Consul, and always exists in a properties file.

Two logging configuration files are provided, one for logging to the console and to a file (log4cpp.properties), and another to log to syslog and to a file (log4cpp_syslog.properties).  Both show all of the logging modules utilized by CLyman during all phases of execution, and all of these should be configured with the same names (for example, log4cpp.category.main).

Clyman is built with many different logging modules, so that configuration values can change the log level for any given module, the log file of any given module, or shift any given module to a different appender or pattern entirely.  These modules should always be present within configuration files, but can be configured to suit the particular deployment needs.

### Greylog
Greylog is a distributed logging solution, which can be used to centralize logging across many instances of CLyman.  In order to send messages to Greylog, we recommend configuring a SyslogAppender in the configuration files, and then following the instructions [here] (https://marketplace.graylog.org/addons/a47beb3b-0bd9-4792-a56a-33b27b567856) to push the messages logged to syslog out to Greylog.

## Configuration Key-Value Variables

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
* Smart_Update_Buffer_Duration - An Integer value denoting the seconds before an object is expired in the Redis buffer
* StampTransactionId - True to stamp Transaction ID's on both inbound responses and outbound messages, and to enforce atomic transactions for get/update messages.  False to turn off this functionality.  This is recommended, to guarantee that updates on the same object across different instances of Clyman are applied in order.
* SendOutboundFailureMsg - True to send outbound messages on errors.  False to suppress outbound messages in the case of an error.

# Next
Now that you have configured CLyman, you can move on to the [Architecture] (https://github.com/AO-StreetArt/CLyman/tree/master/docs/arch) section.
