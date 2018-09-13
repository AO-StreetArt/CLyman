.. _deployment:

Deployment
==========

This page includes an overview and notes on full production deployment of
Clyman.  A step-by-step walkthrough for setting up a secured, single-node
deployment is also available in the :ref:`Advanced Walkthrough <advanced_walkthrough>`.

A full deployment of Clyman involves several steps:

* Consul Setup
* Mongo Setup
* Vault Setup
* Clyman Setup

Each component has it's own encryption and authentication layers.

Clyman Setup
------------

Clyman instances can be deployed in clusters, along with Crazy Ivan.  Each cluster
can utilize the same or different databases.

Clyman can load configuration values from Consul and/or Vault, and uses SSL
encryption with HTTP Basic Authentication for transactions.  Events (sent via UDP)
utilize AES symmetric encryption.

Many configuration values are cluster-specific.  This allows us to set, for example,
separate encryption keys by cluster.

Consul Setup
------------

Deploying a Consul Cluster is covered in detail `on the Consul webpage <https://www.consul.io/>`__.

Clyman uses the Consul KV Store for unsecured configuration values, as well as
using Consul for Service Discovery.  It can utilize SSL encryption, as well as the ACL layer.

Mongo Setup
-----------

Deploying a Mongo Cluster is covered in detail `here <https://docs.mongodb.com/manual/administration/install-on-linux/>`__.

Mongo in containers is also supported.  Either way, once Mongo servers are active, they need to
be registered with Consul in order to be picked up by Service Discovery.  This can be done with curl, for example:

`curl -X PUT -d '{"ID": "mongo", "Name": "mongo", "Tags": ["Primary"], "Address": "localhost", "Port": 27017}' http://127.0.0.1:8500/v1/agent/service/register`

Vault Setup
-----------

Deploying a Vault Cluster is covered in detail `on the Vault webpage <https://www.vaultproject.io/>`__.

Clyman can utilize the following Secret Stores:

* Consul - Generate Consul ACL tokens
* PKI - Generate SSL Certs/Keys
* KV - Store secure configuration options

:ref:`Advanced Walkthrough <advanced_walkthrough>`

:ref:`Go Home <index>`
