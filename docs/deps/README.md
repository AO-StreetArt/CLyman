# Dependency Resolution

For Ubuntu 14.04 & Debian 7, the build_deps.sh script should allow for automatic resolution of dependencies.  However, links are still included below for those who wish to build on other Operating Systems.

This library is built on top of the [AO Shared Service Library] (https://github.com/AO-StreetArt/AOSharedServiceLibrary).  This must be built and linked/included.

You will need the Google Protocol Buffer C++ API, which can be found [here](https://developers.google.com/protocol-buffers).  These can be installed on many linux systems with a variant of the below command:

`sudo apt-get install libprotobuf-dev protobuf-compiler`

This service also depends on Eigen, a C++ Linear Algebra library.  This can be downloaded from the [Eigen] (http://eigen.tuxfamily.org/index.php?title=Main_Page) site.

You will need RapidJSON which can be found [here] (https://github.com/miloyip/rapidjson).