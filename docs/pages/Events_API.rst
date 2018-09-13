.. _event_stream_api:

Event API
=========

An event is a high-speed update with overwrites to the current state of an
object.  Upon receiving an event from a client via UDP, CLyman sends the event
via UDP to an event forwarder (typically Crazy Ivan), whose job it is to
pass along that event to any interested parties.

If configured, the event may be encrypted with an AES-256 symmetric key and
salt.  The event is a JSON message, which can take one of two formats but
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

Keep in mind that Events work on the premise of overwrites for any field in the
event.  You should, therefore, include any information (such as graph handles)
onto the objects that you are actually utilizing.  For a framed property using
graph handles, this might look a bit more like:

.. code-block:: json

   {
     "msg_type": 9,
     "key": "5b98880a270698496c36e392",
     "frame": 100,
     "values": [{"value": 100.0,
                "left_type": "vector",
                "left_x": 10.0,
                "left_y":10.0,
                "right_type": "vector",
                "right_x": 10.0,
                "right_y":10.0}]
   }
