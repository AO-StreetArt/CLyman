.. _event_stream_api:

Event API
=========

An event is a high-speed update with overwrites to the current state of an
object.  Upon receiving an event from a client via UDP, CLyman sends the event
via UDP to an event forwarder (typically Crazy Ivan), whose job it is to
pass along that event to any interested parties.

If configured, the event may be encrypted with an AES-256 symmetric key and
salt.

Event Input
-----------

The event is a JSON message, which can take one of several formats but
always includes the field "msg_type".  The first format is an Object Overwrite,
which follows the same JSON-schema as the :ref:`Object Update HTTP API <object_api>`.
The second format is a Property Overwrite, which follows the same JSON-schema as
the :ref:`Property Update HTTP API<property_api>`

Here is an example Object Update:

.. code-block:: json

   {
     "msg_type": 1,
     "key": "5b98880a270698496c36e392",
     "transform": [1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0]
   }

Sending this update would overwrite the transform matrix of the object to:

[[1.0, 1.0, 1.0, 0.0],
[1.0, 1.0, 0.0, 1.0],
[1.0, 0.0, 1.0, 1.0],
[0.0, 1.0, 1.0, 1.0]]

The key is the key of the object, and the "msg_type" = 1 let's CLyman know
that this particular update is for an Object and not a property.  We can,
of course, update a property with an event as well:

.. code-block:: json

   {
     "msg_type": 9,
     "key": "5b98880a270698496c36e392",
     "values": [{"value": 100.0}]
   }

This will overwrite the current value of this property to 100.0.

Updates can occur on keyframes as well in addition to objects, using the
same JSON format as the HTTP API.  See the listing of valid msg_type values below.

Event Output
------------

Event output follows the same basic format as the input, however there are
multiple additional message types that may be received.

While input events can only be Updates, output events can include the following
msg_type values:

* 0 - Object Create
* 1 - Object Update
* 3 - Object Delete
* 8 - Property Create
* 9 - Property Update
* 11 - Property Delete
* 15 - Object Frame Create
* 16 - Object Frame Update
* 18 - Object Frame Delete
* 19 - Object Action Create
* 20 - Object Action Update
* 22 - Object Action Delete
* 23 - Property Frame Create
* 24 - Property Frame Update
* 26 - Property Frame Delete
* 27 - Property Action Create
* 28 - Property Action Update
* 30 - Property Action Delete
