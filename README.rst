CLyman
======

.. figure:: https://travis-ci.org/AO-StreetArt/CLyman.svg?branch=master
   :alt:

Overview
--------

CLyman is a C++ microservice which synchronizes high-level 3-D object
attributes across many user devices. The goal is to synchronize the
position, rotation, and scale of virtual objects projected into a real
space.

This service is intended to fill a small role within a larger
architecture designed to synchronize 3D objects across different client
programs. It is highly scalable, and many instances can run in parallel
to support increasing load.

Features
--------

- Storage of 3-D Objects Location, Rotation, Scaling
- Enable real-time change feeds on the objects which are stored
- Connect to other services over Zero MQ using JSON or Google Protocol Buffers.
- Configurable Logic
- Scalable microservice design

CLyman is a part of the AO Aesel Project, along with
`Crazy Ivan <https://github.com/AO-StreetArt/CrazyIvan>`__. It therefore
utilizes the `DVS Interface
library <https://github.com/AO-StreetArt/DvsInterface>`__, also
available on github. It utilizes the Obj3.proto file for inbound
communications when configured to read protocol buffers.

Stuck and need help?  Have general questions about the application?  Reach out to the development team at clyman@emaillist.io
