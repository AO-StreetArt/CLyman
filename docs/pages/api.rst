.. _api:

API Documentation
=================

Object Transformation API Design Document

Author – Alex Barry

:ref:`Go Home <index>`

Section 1: Abstract
-------------------

This document outlines the design for the Object Transformation Service
API. This service tracks objects location, rotation, and scaling in 3
space via a 4x4 transformation matrix. The design proposed in this
document follows a series of principles:

-  Messages should require only the fields necessary.
-  Where intelligent updates incur performance penalties, they should be
   configurable and scalable. Therefor, message behavior may depend on
   server side configurations. The accepted/expected fields in messages,
   however, will not change.

Section 2: Objectives
---------------------

-  The primary purpose of this document is to detail the API for the
   Object Transformation Service, Clyman.
-  This document will provide specifications for inbound and outbound
   messages from the service
-  Messaging will be via either JSON or Protocol Buffer format
   (depending on configuration) with 0MQ as the message transport layer.

Section 3: Inbound Messages
---------------------------

\| Field Name \| Description \| Data Type \| Required? \| \| \| \| \|
---------- \| ----------- \| --------- \| --------- \| --- \| --- \| ---
\| \| \| \| \| Create \| Retrieve \| Update \| Destroy \| \| \| \| \| \|
\| \| \| \| message\_type \|

.. raw:: html

   <ul>

.. raw:: html

   <li>

OBJ\_UPD=0

.. raw:: html

   </li>

.. raw:: html

   <li>

OBJ\_CRT=1

.. raw:: html

   </li>

.. raw:: html

   <li>

OBJ\_GET=2

.. raw:: html

   </li>

.. raw:: html

   <li>

OBJ\_DEL=3

.. raw:: html

   </li>

.. raw:: html

   <li>

OBJ\_LOCK=5

.. raw:: html

   </li>

.. raw:: html

   <li>

OBJ\_UNLOCK=6

.. raw:: html

   </li>

.. raw:: html

   <li>

KILL=999

.. raw:: html

   </li>

.. raw:: html

   <li>

PING=555

.. raw:: html

   </li>

.. raw:: html

   </ul>

\| Integer \| X \| X \| X \| X \| \| key \| The UUID of the object \|
String \| \| X \| X \| X \| \| name \| The name of the object \| String
\| X \| \| \* \| \| \| type \| The type of the object, ie. "Mesh" \|
String \| X \| \| \* \| \| \| subtype \| The subtype of the object, ie.
"Cube" \| String \| X \| \| \* \| \| \| location \| The Location of the
object \| Array - Double \| X \| \| \* \| \| \| rotation\_euler \| The
Euler Rotation of the object \| Array - Double \| X \| \| \* \| \| \|
rotation\_quaternion \| The Quaternion Rotation of the object \| Array -
Double \| X \| \| \* \| \| \| scale \| The Scaling of the object \|
Array - Double \| X \| \| \* \| \| \| transform \| The Transformation
Matrix of the object \| Array - Double \| X \| \| \* \| \| \|
bounding\_box \| The Bounding Box of the object \| Array - Double \| X
\| \| \* \| \| \| scenes \| The list of string scene UUID's \| Array -
String \| X \| \| \* \| \* \| \| owner\_device\_id \| The UUID of the
owner User Device. \| String \| X \| \| \* \| \| \| lock\_device\_id \|
The UUID of the locking User Device. \| String \| X \| \| \* \| \| \|
mesh\_id \| The UUID of the mesh that corresponds to this object. \|
String \| X \| \| \| \| \| transform\_type \| Global vs Local
Transformations. By default, local transformations are used. However, a
value of 'global' in this field will allow for updates according to
global coordinate systems. \| String \| \| \| \| \|

X – Required

\* - Potentially Required for message to cause any action, depending on
configuration

Object Create
-------------

The inbound object create message requires all fields to construct the
initial object, except for the lock\_device\_id and owner\_device\_id.
These are only required if the respective functionality is configured.

Object Retrieve
---------------

The object retrieve message only requires a device UUID and message
type.

Object Update
-------------

The behavior of the object update message depends upon the configuration
of Smart Updates.

If Smart Updates are disabled, then all fields in the message are
required and the update is a full-replace message.

If Smart Updates are enabled, then only the message\_type and key fields
are required. Any location, rotation, scaling, or transform matrix sent
will be interpreted as a transformation, and applied to the existing
object in the DB.

Object Destroy
--------------

For the object destroy message, the message\_type and key fields are
required.

Object Lock
-----------

If configured, locking on an object requires a key, message type, and
lock\_device\_id. This sends a lock request, which may be accepted or
denied.

Object Unlock
-------------

If configured, unlocking an object requires a key, message type, and
lock\_device\_id. This sends an unlock request, which may be accepted or
denied.

Section 4: Inbound Responses
----------------------------

\| Field Name \| Description \| Data Type \| Included? \| \| \| \| \|
---------- \| ----------- \| --------- \| --------- \| --- \| --- \| ---
\| \| \| \| \| Create \| Retrieve \| Update \| Destroy \| \| \| \| \| \|
\| \| \| \| error\_code \|

.. raw:: html

   <ul>

.. raw:: html

   <li>

NO\_ERROR=0

.. raw:: html

   </li>

.. raw:: html

   <li>

ERROR=100

.. raw:: html

   </li>

.. raw:: html

   <li>

DB\_ERROR=110

.. raw:: html

   </li>

.. raw:: html

   <li>

REDIS\_ERROR=120

.. raw:: html

   </li>

.. raw:: html

   <li>

TRANSLATION\_ERROR=130

.. raw:: html

   </li>

.. raw:: html

   <li>

BAD\_SERVER\_ERROR=140

.. raw:: html

   </li>

.. raw:: html

   <li>

BAD\_REQUEST\_ERROR=150

.. raw:: html

   </li>

.. raw:: html

   </ul>

\| Integer \| \* \| \* \| \* \| \* \| \| error\_message \| A description
of any error that occurred \| String \| \* \| \* \| \* \| \* \| \|
object\_id \| The key of the object \| String \| X \| X \| X \| X \| \|
transaction\_id \| The Transaction ID. This is the same as the
transaction ID given on the inbound response and can be used to link the
two together, if this functionality is configured. \| String \| \* \| \*
\| \* \| \* \|

X – Guaranteed

\* - Potentially Included, depending on whether we have a success or
failure response and/or configuration

Note: While Inbound and Outbound Messages work based off of the
Obj3.proto file for protocol-buffer messaging, Inbound Responses work
based off of the Response.proto file.

Section 5: Outbound Messages
----------------------------

\| Field Name \| Description \| Data Type \| Included? \| \| \| \| \|
---------- \| ----------- \| --------- \| --------- \| --- \| --- \| ---
\| \| \| \| \| Create \| Retrieve \| Update \| Destroy \| \| \| \| \| \|
\| \| \| \| message\_type \|

.. raw:: html

   <ul>

.. raw:: html

   <li>

OBJ\_UPD=0

.. raw:: html

   </li>

.. raw:: html

   <li>

OBJ\_CRT=1

.. raw:: html

   </li>

.. raw:: html

   <li>

OBJ\_GET=2

.. raw:: html

   </li>

.. raw:: html

   <li>

OBJ\_DEL=3

.. raw:: html

   </li>

.. raw:: html

   <li>

OBJ\_LOCK=5

.. raw:: html

   </li>

.. raw:: html

   <li>

OBJ\_UNLOCK=6

.. raw:: html

   </li>

.. raw:: html

   <li>

KILL=999

.. raw:: html

   </li>

.. raw:: html

   <li>

PING=555

.. raw:: html

   </li>

.. raw:: html

   </ul>

\| Integer \| X \| X \| X \| X \| \| key \| The UUID of the object \|
String \| X \| X \| X \| X \| \| name \| The name of the object \|
String \| X \| X \| X \| \| \| type \| The type of the object, ie.
"Mesh" \| String \| X \| X \| X \| \| \| subtype \| The subtype of the
object, ie. "Cube" \| String \| X \| X \| X \| \| \| location \| The
Location of the object \| Array - Double \| X \| X \| X \| \| \|
rotation\_euler \| The Euler Rotation of the object \| Array - Double \|
X \| X \| X \| \| \| rotation\_quaternion \| The Quaternion Rotation of
the object \| Array - Double \| X \| X \| X \| \| \| scale \| The
Scaling of the object \| Array - Double \| X \| X \| X \| \| \|
transform \| The Transformation Matrix of the object \| Array - Double
\| X \| X \| X \| \| \| bounding\_box \| The Bounding Box of the object
\| Array - Double \| X \| X \| X \| \| \| scenes \| The list of string
scene UUID's \| Array - String \| X \| X \| X \| \| \| is\_locked \| 0
if unlocked, 1 if locked \| Integer \| \* \| \* \| \* \| \| \| mesh\_id
\| The UUID of the mesh that corresponds to this object. \| String \| X
\| X \| X \| \| \| error\_message \| If outbound error responses are
enabled, than an error message may be included here. \| String \| \* \|
\* \| \* \| \| \| transaction\_id \| The Transaction ID. This is the
same as the transaction ID given on the inbound response and can be used
to link the two together, if this functionality is configured. \| String
\| \* \| \* \| \* \| \* \|

X – Guaranteed

\* - Potentially Included, depending on configuration

Object Create
-------------

An outbound Object Create Message notifies listeners that an object has
been created.

Object Retrieve
---------------

An outbound Object Retrieve Message notifies listeners that a response
has been lodged to a previous retrieval request.

Object Update
-------------

An outbound Object Update Message notifies listeners that an object has
been updated.

Object Destroy
--------------

An outbound Object Create Message notifies listeners that an object has
been destroyed.

Appendix: JSON Message Samples
------------------------------

Inbound
-------

Object Create
~~~~~~~~~~~~~

{

"message\_type": 0,

"key": “24575768452345”,

"name": "Object1",

"type": "Mesh",

"subtype": "Mesh",

"location": [0.0, 0.0, 0.0],

"rotation\_euler": [0.0, 0.0, 0.0],

"rotation\_quaternion": [0.0, 0.0, 0.0, 0.0],

"scale": [0.0, 0.0, 0.0],

"transform": [

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0

],

"bounding\_box": [

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0

],

"scenes": ["12346453456", "7685632194"],

“owner\_device\_id”: “235jher1234ms”,

"lock\_device\_id": "-1",

“mesh\_id”: “234xfd432xf”

}

Object Retrieve
~~~~~~~~~~~~~~~

{

"message\_type": 1,

"key": “24575768452345”,

}

Object Update
~~~~~~~~~~~~~

{

"message\_type": 2,

"key": “24575768452345”,

"name": "Object1",

"type": "Mesh",

"subtype": "Mesh",

"location": [0.0, 0.0, 0.0],

"rotation\_euler": [0.0, 0.0, 0.0],

"rotation\_quaternion": [0.0, 0.0, 0.0, 0.0],

"scale": [0.0, 0.0, 0.0],

"transform": [

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0

],

"bounding\_box": [

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0

],

"scenes": ["12346453456", "7685632194"],

"lock\_device\_id": "-1"

}

Object Destroy
~~~~~~~~~~~~~~

{

"message\_type": 3,

"key": “24575768452345”,

"lock\_device\_id": "-1"

}

Inbound Responses
-----------------

{

"error\_code": 0,

"error\_message": “”,

"transaction\_id": "abdsd545cxdf45",

"object\_id": "dfg546dcn453d"

}

Outbound
--------

Object Create
~~~~~~~~~~~~~

{

"message\_type": 0,

"key": “24575768452345”,

"name": "Object1",

"type": "Mesh",

"subtype": "Mesh",

"location": [0.0, 0.0, 0.0],

"rotation\_euler": [0.0, 0.0, 0.0],

"rotation\_quaternion": [0.0, 0.0, 0.0, 0.0],

"scale": [0.0, 0.0, 0.0],

"transform": [

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0

],

"bounding\_box": [

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0

],

"scenes": ["12346453456", "7685632194"],

"is\_locked": true,

“mesh\_id”: “234xfd432xf”

}

Object Retrieve
~~~~~~~~~~~~~~~

{

"message\_type": 1,

"key": “24575768452345”,

"name": "Object1",

"type": "Mesh",

"subtype": "Mesh",

"location": [0.0, 0.0, 0.0],

"rotation\_euler": [0.0, 0.0, 0.0],

"rotation\_quaternion": [0.0, 0.0, 0.0, 0.0],

"scale": [0.0, 0.0, 0.0],

"transform": [

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0

],

"bounding\_box": [

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0

],

"scenes": ["12346453456", "7685632194"],

"is\_locked": true,

“mesh\_id”: “234xfd432xf”

}

Object Update
~~~~~~~~~~~~~

{

"message\_type": 2,

"key": “24575768452345”,

"name": "Object1",

"type": "Mesh",

"subtype": "Mesh",

"location": [0.0, 0.0, 0.0],

"rotation\_euler": [0.0, 0.0, 0.0],

"rotation\_quaternion": [0.0, 0.0, 0.0, 0.0],

"scale": [0.0, 0.0, 0.0],

"transform": [

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0

],

"bounding\_box": [

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0

],

"scenes": ["12346453456", "7685632194"],

"is\_locked": true,

“mesh\_id”: “234xfd432xf”

}

Object Destroy
~~~~~~~~~~~~~~

{

"message\_type": 3,

"key": “24575768452345”,

“mesh\_id”: “234xfd432xf”

}
