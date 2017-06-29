.. dependencies:

Dependency Resolution
=====================

:ref:`Go Home <index>`

For Ubuntu 14.04 & Debian 7, the build\_deps.sh script should allow for
automatic resolution of dependencies. However, links are still included
below for those who wish to build on other Operating Systems.

This library is built on top of the AO Shared Service Library
(https://github.com/AO-StreetArt/AOSharedServiceLibrary). This must be
built and linked/included.

You will need the Google Protocol Buffer C++ API, which can be found
`here <https://developers.google.com/protocol-buffers>`__. These can be
installed on many linux systems with a variant of the below command:

``sudo apt-get install libprotobuf-dev protobuf-compiler``

This service also depends on Eigen, a C++ Linear Algebra library. This
can be downloaded from the Eigen
(http://eigen.tuxfamily.org/index.php?title=Main\_Page) site.

You will need RapidJSON which can be found at
https://github.com/miloyip/rapidjson.

Finally, we also depend on the DVS Interface Library
(https://github.com/AO-StreetArt/DvsInterface), which houses a
collection of proto files for this project. If you are building
dependencies manually, you'll need to clone the repository and copy the
Obj3.proto file into the src/ folder of this project.

Acknowledgements
----------------

Much of the codebase for CLyman is built upon the works of others. In
this section I will try to give credit where credit is due:

::

    René Nyffenegger - Base64 Decoding Methods