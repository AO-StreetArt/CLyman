Clyman
======

.. figure:: https://travis-ci.org/AO-StreetArt/CLyman.svg?branch=master
   :alt:

Overview
--------

CLyman is a service which maintains Objects and Properties over time, and
streams out updates to an external service which distributes them to anyone
viewing the same elements.

It's primary focus is on distributed animation (ie. using programs like Blender
or Maya, and having many artists collaborate on one Scene), as well as multi-user
Augmented Reality applications.  Clyman serves as the store for all information
which is distributed amongst devices, and as the starting point for that
distribution.

Clyman is built to be horizontally scalable, and performance is a top priority.
In addition, Clyman uses the latest security models for running in low-trust
environments, making sure your animations stay safe.

Detailed documentation can be found on `ReadTheDocs <http://clyman.readthedocs.io/en/v2/index.html>`__.

Features
--------

-  Storage of Renderable Objects and Properties
-  HTTP and UDP API's for both Objects and Properties
-  Stream updates via UDP to External Service (Crazy Ivan) for distribution
-  Multi-layered security
-  Scalable and Flexible Deployment Strategies

Clyman is a part of the AO Aesel Project, along
with `Crazy Ivan <https://github.com/AO-StreetArt/CrazyIvan>`__.

Stuck and need help?  Have general questions about the application?  Reach out to the development team at clyman@emaillist.io
