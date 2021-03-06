.. dependencies:

Dependencies
============

:ref:`Go Home <index>`

CLyman is built on top of the work of many others, and here you will find information
on all of the libraries and components that CLyman uses to be successful.

Licenses for all dependencies can be found in the licenses folder within the repository.


RapidJson
---------
`RapidJson <https://github.com/miloyip/rapidjson>`__ is a very fast JSON parsing/writing library.

`RapidJson <https://github.com/miloyip/rapidjson>`__ is released under an MIT License.


AO Shared Service Library
-------------------------
`AOSSL <https://github.com/AO-StreetArt/AOSharedServiceLibrary>`__ is a collection
of C++ wrappers on many of the C libraries listed here.

`AOSSL <https://github.com/AO-StreetArt/AOSharedServiceLibrary>`__ is released under an MIT License.


MongoCxx
--------
`MongoCxx <https://github.com/mongodb/mongo-cxx-driver>`__ is the official C++
client for Mongodb, the database behind CLyman.

`MongoCxx <https://github.com/mongodb/mongo-cxx-driver>`__ is released under an Apache 2 License.


LibUUID
-------
`LibUUID <https://sourceforge.net/projects/libuuid/>`__ is a linux utility for generating Universally Unique ID's.

`LibUUID <https://sourceforge.net/projects/libuuid/>`__ is released under a BSD License.


POCO
----
`The POCO Project <https://pocoproject.org/>`__ is a set of libraries for building networked C++ applications.

It is released under the Boost Software License.


Boost
-----
`The Boost Project <https://www.boost.org/>`__ is a set of C++ libraries, that are primarily used for UDP Processing.

It is released under the Boost Software License.

Automatic Dependency Resolution
-------------------------------

For Ubuntu 16.04 or Centos7, the build\_deps.sh scripts should allow for
automatic resolution of dependencies.

Developers can utilize the Vagrant image, which will install dependencies in the VM.

End-Users can run the Docker image, which will install dependencies in the container.

Other Acknowledgements
----------------------

Here we will try to list authors of other public domain code that has been used:

::

    René Nyffenegger - Base64 Decoding Methods
