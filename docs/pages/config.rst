.. _configuration:

Configuration
=============

Properties File
---------------

CLyman can be configured via a properties file, which has a few command
line options:

-  ``./clyman`` - This will start CLyman with the default properties
   file, app.properties
-  ``./clyman -config-file=file.properties`` - This will start CLyman
   with the properties file, file.properties. Can be combined with
   -log-conf.
-  ``./clyman -log-conf=logging.properties`` - This will start CLyman
   with the logging properties file, logging.properties. Can be combined
   with -config-file.

The properties file can be edited in any text editor.

Consul
------

Clyman can also be configured via a Consul Connection, in which we must
specify the address of the consul agent, and the ip & port of the
Inbound ZeroMQ Connection.

-  ``./clyman -consul-addr=localhost:8500 -ip=localhost -port=5555`` -
   Start Clyman, register as a service with consul, and configure based
   on configuration values in Consul, and bind to an internal 0MQ port
   on localhost
-  ``./clyman -consul-addr=localhost:8500 -ip=tcp://my.ip -port=5555 -log-conf=logging.properties``
   - Start Clyman, register as a service with consul, and configure
   based on configuration values in Consul. Bind to an external 0MQ port
   on tcp://my.ip, and configure from the logging configuration file,
   logging.properties.

We can also use both a properties file and a Consul connection, in which case
the properties file is used to define the ip and port of the inbound ZeroMQ connection,
while Consul is used for registration and all other configuration retrieval.

-  ``./clyman -consul-addr=localhost:8500 -config-file=file.properties``

When configuring from Consul the keys of the properties file are equal
to the expected keys in Consul.

Logging
-------

The Logging Configuration File can also be edited with a text file, and
the documentation for this can be found at
http://log4cpp.sourceforge.net/api/classlog4cpp\_1\_1PropertyConfigurator.html
Note that logging configuration is not yet in Consul, and always exists
in a properties file.

The logging configuration provided shows all of the logging
modules utilized by CLyman during all phases of execution, and all of
these should be configured with the same names (for example,
log4cpp.category.main).

Clyman is built with many different logging modules, so that
configuration values can change the log level for any given module, the
log file of any given module, or shift any given module to a different
appender or pattern entirely. These modules should always be present
within configuration files, but can be configured to suit the particular
deployment needs.

Startup
-------

CLyman can be started with an option to wait for a specified number of
seconds prior to looking for configuration values and opening up for requests.
This is particularly useful when used with orchestration providers, in order
to ensure that other components are properly started (in particular, in order
to allow time for Consul to be populated with default configuration values).

-  ``./clyman -wait=5`` - This will start CLyman with the default
   properties file, and wait 5 seconds before starting.

Configuration Key-Value Variables
---------------------------------

Below you can find a summary of the options in the Properties File or
Consul Key-Value Store:

DB
~~

-  Mongo\_ConnectionString - The string used to connect to the Mongo
   instance (example: mongodb://localhost:27017/)
-  Mongo\_DbName - The Mongo Database to connect to within the cluster
-  Mongo\_DbCollection - The Mongo Collection to utilize for storing documents

0MQ
~~~

-  0MQ\_InboundConnectionString - The connectivity string for the
   inbound 0MQ Port (example: tcp://\*:5555)

Kafka Connection
~~~~~~~~~~~~~~~~

- KafkaBrokerAddress - The address and port of the Kafka Broker to send Object Updates to

Behavior
~~~~~~~~~

-  DataFormatType - Are we communicating via JSON or Protocol Buffers
-  StampTransactionId - True or False, do we stamp Transaction IDs on messages that
   do not have them already
-  AtomicTransactions - True or False, do we enforce atomic transactions
   across all instances of CLyman for any given object.  This guarantees that
   updates will be processed in the order they are received across the entire CLyman network.

:ref:`Go Home <index>`
