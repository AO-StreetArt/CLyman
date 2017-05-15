# Overview

This is a C++ microservice which synchronizes high-level 3-D object attributes across many user devices. The goal is to synchronize the position, rotation, and scale of virtual objects projected into a real space.

This service is intended to fill a small role within a larger architecture designed to synchronize 3D objects across different client programs. It is highly scalable, and many instances can run in parallel to support increasing load.

## Features:

    Storage of 3-D Objects Location, Rotation, Scaling
    Enable real-time change feeds on the objects which are stored
    Connect to other services over Zero MQ using JSON or Google Protocol Buffers.
    Configurable Logic
    Scalable microservice design

CLyman is a part of the AO Distributed Visualization Server, along with [Crazy Ivan] (https://github.com/AO-StreetArt/CrazyIvan). It therefore utilizes the [DVS Interface library](https://github.com/AO-StreetArt/DvsInterface), also available on github. It utilizes the Obj3.proto file for inbound communications.

## Table of Contents

* [Quickstart](https://github.com/AO-StreetArt/CLyman/tree/master/docs/quickstart)
* [Configuration](https://github.com/AO-StreetArt/CLyman/tree/master/docs/config)
* [API](https://github.com/AO-StreetArt/CLyman/tree/master/docs/api)
* [Architecture](https://github.com/AO-StreetArt/CLyman/tree/master/docs/arch)
* [Deployment](https://github.com/AO-StreetArt/CLyman/tree/master/docs/deploy)
* [Tests](https://github.com/AO-StreetArt/CLyman/tree/master/docs/tests)
* [Dependencies](https://github.com/AO-StreetArt/CLyman/tree/master/docs/deps)

## Acknowledgements

Much of the codebase for CLyman is built upon the works of others. In this section I will try to give credit where credit is due:

    René Nyffenegger - Base64 Decoding Methods
