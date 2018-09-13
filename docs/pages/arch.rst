.. _architecture:

Architecture
============

CLyman serves two primary purposes within the Aesel architecture:

1. Serves as the system of reference for animation/game data which is updated by clients.
2. Serves as the origin point for Events, high-speed updates streamed out to connected devices.

In other words, CLyman takes input from clients moving objects and/or adjusting
properties, streams that out to other devices, and maintains the current state
of all objects and properties throughout all changes.  It is the core service
which maintains the 3D objects that clients are expected to interact with.

Technical Overview
------------------

CLyman is designed to be used as a service within a larger
architecture. It will take in CRUD messages for 3D Objects and Properties
(both over HTTP).

Running CLyman requires an instance of `Mongo <https://www.mongodb.com/>`__
to connect to in order to perform most functions.  Mongo serves as the
back-end database for CLyman.

CLyman can also be deployed with `Consul <https://www.consul.io/>`__
as a Service Discovery and Distributed Configuration architecture. This
requires the `Consul Agent <https://www.consul.io/downloads.html>`__ to
be deployed that Crazy Ivan can connect to.

CLyman can be deployed securely using `Vault <https://www.vaultproject.io>`__
as a secret store and/or intermediate CA.

Object Change Streams (Events)
------------------------------

Object Change Streams ensure that all registered User Devices remain up to date about
objects within their scenes.  CLyman generates UDP messages to downstream services
whenever updates are received (either over HTTP or UDP).

The changes streams are designed to be high-speed and high-volume.  CLyman
can process many messages in parallel, and database persistence is performed
after streaming updates to downstream services.

Clustering
----------

Scene-specific clustering is a central idea in CLyman.  This is an idea
borrowed from large-scale MMORPG's, in which large maps are broken apart and
each piece is run by separate servers.  This allows for horizontal scaling of
the system to cover additional real-estate, physical or digital.

A cluster name can be provided by CLyman on startup, and other applications
should use this cluster name to identify the appropriate CLyman to send
messages to.

:ref:`Go Home <index>`
