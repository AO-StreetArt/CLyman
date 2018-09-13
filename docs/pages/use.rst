.. _use:

How to Use CLyman
=================

Overview
--------

CLyman's primary objective is to track specific attributes of 3-Dimensional
scenes which then need to be distributed to interested parties.  CLyman provides
both an HTTP and UDP API which can be used to create, manipulate, and destroy
these attributes.  Required changes are then pushed over UDP to an external
system for distribution to end-users.

CLyman is an extremely flexible system, but it's design is primarily based
on the concepts present in 3D Animation Software (such as Blender or Maya).

Data Model
----------

We start at the basic level with a 3D Object (via the :ref:`Object API <object_api>`),
which can move through space.  We track these objects via a 4x4 matrix, assuming
all users are utilizing the same coordinate system to view the objects.

Objects can be tracked in real-time, or they can be tied to specific frames.  A
frame is a single point in the animation, and typically animators work by assigning
'keyframes', which are then stored in by CLyman.

Animators frequently utilize a graph-based editor to then tweak the key frames,
utilizing 'handles' on either side of each key frame that can be used to adjust
individual parts of the movement.  These are optional, but can be provided
for any object.

Finally, we frequently want to track other attributes over time, which may not
be associated directly to the movement of the object.  Examples of this might
be the value of gravity changing over time, or a keyframed parameter in a shader.
Clyman can store these as 'Properties', in the :ref:`Property API <property_api>`,
which can have one, two, or three individual values that can each have graph
handles as well.

Using CLyman is done by creating Objects and Properties, then using updates
to push changes out to all interested parties.  Users are expected to create and
delete objects and properties throughout any real use case, while CLyman tracks
the required values and pumps out an event stream.

Event Streams
-------------

Event Streams are designed to be as fast as possible.  Communication of events
is limited to UDP and/or shared memory, and events can be restricted to
specific clusters of CLyman and other components to ensure minimal network
latency.

Optionally, Event Streams can also utilize AES symmetric-encryption, to make
sure that the live updates cannot be read by prying eyes.

:ref:`Go Home <index>`
