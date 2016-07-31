# Project CLyman

## Build Status
* Project - Successful
* Tests - Successful

## Overview

This is a C++ microservice which synchronizes high-level 3-D object attributes across many user devices.  The goal is to synchronize the position, rotation, and scale of virtual objects projected into a real space.

This service is intended to fill a small role within a larger architecture designed to synchronize 3D objects across different client programs.  It is highly scalable, and many instances can run in parallel to support increasing load.

Features:
* Storage of 3-D Objects Location, Rotation, Scaling
* Enable real-time change feeds on the objects which are stored
* Connect to other services over Zero MQ using JSON or Google Protocol Buffers.
* Configurable Update logic & message formatting
* Scalable microservice design

## Table of Contents


## Acknowledgements
Much of the codebase for CLyman is built upon the works of others.  In this section I will try to give credit where credit is due:
* René Nyffenegger - Base64 Decoding Methods
