# Architecture

This is designed to be used as a microservice within a larger architecture.  This will take in CRUD messages for objects in 3 space, and track location, rotation, and scaling.  It will also ensure that any updates are sent out on a separate port to allow streaming to all user devices.

A .proto file is included to allow generating the bindings for any language (the [protocol buffer compiler] (https://developers.google.com/protocol-buffers/) is installed by the build_deps script), which can be used to communicate via protocol buffers.  Alternatively, Clyman can be configured to accept JSON messages.  Full documentation on supported message formats will be forthcoming.

In addition, [0-Meter] (https://github.com/AO-StreetArt/0-Meter) has been developed to allow easy testing of the module with JSON message formats.

Please note that running CLyman requires an instance of both [Couchbase DB Server 4.1] (http://www.couchbase.com/) and [Redis 3.2.0] (http://redis.io/) to connect to in order to run.  Clustering of both Couchbase & Redis are supported.

CLyman can also be deployed with [Consul] (https://www.consul.io/) as a Service Discovery and Distributed Configuration architecture.  This requires the [Consul Agent] (https://www.consul.io/downloads.html) to be deployed that CLyman can connect to.

# Design

In order to allow for real-time, distributed visualization, one of the key problems that needs to be solved is allowing for real-time communication between devices regarding an object's position, orientation, and scaling.  This aims to solve that problem by providing a messaging API that is used to update objects, and a corresponding feed outbound which tells all devices about the update.

This means that, by design, a client application should not expect to update an object, than send an update message.  Rather, clients should send an update message to the server, and then update the object based on the messages received from the server.

# Next
Now you can move on to the [Deployment] (https://github.com/AO-StreetArt/CLyman/tree/master/docs/deploy) section.
