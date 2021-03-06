.. _configuration:

Configuration
=============

CLyman can be configured from one or more sources:

* Environment Variables
* Command Line Arguments
* Consul KV Store
* Vault KV Store
* Properties File

The application gives priority to the values retrieved in the above order.  This means
that an environment variable setting will override any other setting.

Command Line arguments and Properties File keys are lower case, and separated
by periods (ie. 'section.key=').  Environment Variables, Vault, and Consul keys
are all upper case, and are separated by underscores (ie. 'SECTION_KEY=').

All arguments are prefixed with the application name and profile name (ie.
'section.key' becomes 'clyman.prod.section.key').  The profile name can be changed
by providing the command line argument 'profile':

.. code-block:: bash

   ./clyman profile=dev

You can store multiple profiles in your configuration sources, and then specify
which one to use on startup of each instance.

Cluster Name
------------
The 'cluster' option on the command line or in a properties file, or the 'AOSSL_CLUSTER_NAME' environment variable,
will set the name of the cluster.  A cluster is a grouping of CLyman instances, which have been assigned particular
scenes to manage.  Each CLyman instance is designed to manage a set number of scenes, and this allows for highly
optimized streaming of object updates.

The cluster name will affect both how CLyman registers with Consul, if provided, as well as the names of
cluster-specific security properties.

Vault
-----
Vault Address - Starts CLyman against a Vault instance.  Specified by
a collection of arguments:

* vault (Environment Variable VAULT) - the address of the vault instance

.. code-block:: properties

   vault=http://localhost:8200

* vault.cert (Environment Variable VAULT_CERT) - the location of the SSL certificate
to use when communicating with Vault.  You may also leave this
blank to enable SSL encryption without providing a client certificate.

.. code-block:: properties

   vault.cert=

* vault.authtype (Environment Variable VAULT_AUTHTYPE) - the authentication type
used by Vault, currently supported options are 'APPROLE' and 'BASIC'

.. code-block:: properties

   vault.authtype=BASIC

* vault.un (Environment Variable VAULT_UN) - The Username/Role Id for
authenticating with Vault

.. code-block:: properties

   vault.un=test

* vault.pw (Environment Variable VAULT_PW) - The Password/Secret Id for
authenticating with Vault

.. code-block:: properties

   vault.pw=test

In addition, the Vault UN and PW can be loaded from files on disk, 'vault_un.txt' and 'vault_pw.txt'.  This is the recommended
method to set authentication info in CI/CD processes within an application container.

Secure Properties
-----------------
Secure Properties can be loaded from a properties file for development purposes, but in a
Production scenario should always be loaded from a Vault instance.  Once CLyman is connected
to a Vault instance, the following properties can be loaded:

* CONSUL_SSL_CERT - The SSL Certificate to use when communicating with Consul
* CONSUL_ACL_TOKEN - The ACL Token to use when communicating with Consul
* {cluster-name}_TRANSACTION_SECURITY_AUTH_USER - The username which will authenticate with CLyman over HTTP(s)
* {cluster-name}_TRANSACTION_SECURITY_AUTH_PASSWORD - The password which will authenticate with CLyman over HTTP(s)
* {cluster-name}_TRANSACTION_SECURITY_HASH_PASSWORD - The password for the hashing algorithm used to hash the password prior to storage.
* {cluster-name}_EVENT_SECURITY_OUT_AES_KEY - The key for the AES-256 encryption used for sending UDP messages.
* {cluster-name}_EVENT_SECURITY_OUT_AES_SALT - The salt used for the AES-256 encryption used for sending UDP messages.
* {cluster-name}_EVENT_SECURITY_IN_AES_KEY - The key for the AES-256 encryption used for receiving UDP messages.
* {cluster-name}_EVENT_SECURITY_IN_AES_SALT - The salt used for the AES-256 encryption used for receiving UDP messages.

Secure properties can be loaded from any configuration source, but when loaded
from Vault they should be present at the default path ('secret/') in the v2 KV Store.

Consul
------
Consul Address - Starts CLyman against a Consul instance.  Specified by
either the `consul` command line argument or the `AOSSL_CONSUL_ADDRESS`
environment variable.

.. code-block:: bash

   ./clyman consul=http://127.0.0.1:8500

We may also include the arguments:

* consul.cert (Environment Variable AOSSL_CONSUL_SSL_CERT) - The location of the
SSL Certificate to use when communicating with Consul.  You may also leave this
blank to enable SSL encryption without providing a client certificate.

.. code-block:: properties

   consul.cert=

* consul.token (Environment Variable AOSSL_CONSUL_ACL_TOKEN) - The ACL Token to use when communicating with Consul

This will enable property retrieval from Consul KV Store & registering with Consul on start up.

The Consul ACL Token can alternatively be generated from the Consul Secret Store in Vault.

* consul.token.role - The role configured in Vault to use to generate the Consul ACL Token.

.. code-block:: properties

   consul.token.role=consul-role

Properties File
---------------
Properties File - Starts CLyman against a Properties File.  Specified by either
the `props` command line argument or the `AOSSL_PROPS_FILE` environment variable.  For example:

.. code-block:: bash

   ./clyman props=app.properties

If no properties file is specified, CLyman will look for one named `app.properties` in both the
current working folder, and in /etc/clyman/.

The consul address can also be specified within the properties file, with the key `consul`.

HTTPS Setup
-----------
SSL Context Configuration is performed on startup, if enabled.  If the following properties
are set, then SSL Certs for CLyman can be generated dynamically from Vault:

* transaction.security.ssl.ca.vault.active - 'true' or 'false'

.. code-block:: properties

   transaction.security.ssl.ca.vault.active=true

* transaction.security.ssl.ca.vault.role_name - the name of the role to use to generate the SSL Cert

.. code-block:: properties

   transaction.security.ssl.ca.vault.role_name=test-role

* transaction.security.ssl.ca.vault.common_name - The Common-Name to use on the Certificate

.. code-block:: properties

   transaction.security.ssl.ca.vault.common_name=local

Otherwise, SSL Certificate Generation can be configured from a file in the current working directory called 'ssl.properties'.

HTTPS must be enabled with the following parameter:

* transaction.security.ssl.enabled - 'true' or 'false'

.. code-block:: properties

   transaction.security.ssl.enabled=true

Mongo Connection
----------------

* mongo - A full connection string may be supplied here.

.. code-block:: properties

   mongo=mongodb://localhost:27017

In Production Scenarios it is recommended to use Mongo Discovery.  If it is set
to true, then CLyman will use Consul to find a Mongo instance, and will
dynamically find new instances when it encounters many consecutive failures.
To enable this, just do not provide a pre-existing connection string.

* mongo.db - The name of the Database to store CLyman information in

.. code-block:: properties

   mongo.db=CLyman

* mongo.obj.collection - The name of the Collection in which CLyman will store Object Documents

.. code-block:: properties

   mongo.obj.collection=obj3

* mongo.prop.collection - The name of the Collection in which CLyman will store Property Documents

.. code-block:: properties

   mongo.prop.collection=prop

* mongo.ssl.active - Activate SSL Encryption on Mongo Connections, may be used with other SSL options in the config file.

.. code-block:: properties

   mongo.ssl.active=true

Other Values
------------

There are a number of other options that CLyman can be provided on startup.
Below is an overview of the remaining properties:

* Log File - Path on disk to write logs to

.. code-block:: properties

   log.file=clyman.log

* Log Level - Debug, Info, Warning, Error

.. code-block:: properties

   log.level=Debug

* HTTP host to register with Consul

.. code-block:: properties

   http.host=127.0.0.1

* HTTP Port

.. code-block:: properties

   http.port=8766

* UDP Port

.. code-block:: properties

   udp.port=8764

* Enable Event (UDP) Encryption

.. code-block:: properties

   event.security.aes.enabled=false

* Transaction ID's active or inactive.  If active, CLyman will ensure a Transaction Id is stamped on each message.

.. code-block:: properties

   transaction.id.stamp=True

* Format for transactions (HTTP traffic).  Currently only json is supported.

.. code-block:: properties

   transaction.format=json

* Method for streaming events.  Currently only udp is supported.

.. code-block:: properties

   event.stream.method=udp

* Format for streaming events.  Currently only json is supported

.. code-block:: properties

   event.format=json

:ref:`Go Home <index>`
