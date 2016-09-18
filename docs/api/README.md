Object Transformation API Design Document

Author – Alex Barry

<span id="anchor"></span>Section 1: Abstract
============================================

This document outlines the design for the Object Transformation Service
API. This service tracks objects location, rotation, and scaling in 3
space via a 4x4 transformation matrix. The design proposed in this
document follows a series of principles:

-   Messages should require only the fields necessary.
-   Where intelligent updates incur performance penalties, they should
    be configurable and scalable. Therefor, message behavior may depend
    on server side configurations. The accepted/expected fields in
    messages, however, will not change.

<span id="anchor-1"></span>Section 2: Objectives
================================================

-   The primary purpose of this document is to detail the API for the
    Object Transformation Service, Clyman.
-   This document will provide specifications for inbound and outbound
    messages from the service
-   Messaging will be via either JSON or Protocol Buffer format
    (depending on configuration) with 0MQ as the message
    transport layer.

<span id="anchor-2"></span>Section 3: Inbound Messages
======================================================
| Field Name | Description | Data Type | Required? | | | |
| ---------- | ----------- | --------- | --------- | --- | --- | --- |
| | | | Create | Retrieve | Update | Destroy |
|  |  |  |  |  |  |  |
| message_type | <ul><li>OBJ_UPD=0</li><li>OBJ_CRT=1</li><li>OBJ_GET=2</li><li>OBJ_DEL=3</li><li>OBJ_LOCK=5</li><li>OBJ_UNLOCK=6</li><li>KILL=999</li><li>PING=555</li></ul> | Integer | X | X | X | X |
| key | The UUID of the object | String | | X | X | X |
| name | The name of the object | String | X | | * | |
| type | The type of the object, ie. "Mesh" | String | X | | * | |
| subtype | The subtype of the object, ie. "Cube" | String | X | | * | |
| location | The Location of the object | Array - Double | X | | * | |
| rotation_euler | The Euler Rotation of the object | Array - Double | X | | * | |
| rotation_quaternion | The Quaternion Rotation of the object | Array - Double | X | | * | |
| scale | The Scaling of the object | Array - Double | X | | * | |
| transform | The Transformation Matrix of the object | Array - Double | X | | * | |
| bounding_box | The Bounding Box of the object | Array - Double | X | | * | |
| scenes | The list of string scene UUID's | Array - String | X | | * | * |
| owner_device_id | The UUID of the owner User Device. | String | X | | * | |
| lock_device_id | The UUID of the locking User Device. | String | X | | * | |
| mesh_id | The UUID of the mesh that corresponds to this object. | String | X | | | |
| transform_type | Global vs Local Transformations.  By default, local transformations are used.  However, a value of 'global' in this field will allow for updates according to global coordinate systems. | String | | | | |

X – Required

\* - Potentially Required for message to cause any action, depending on
configuration

<span id="anchor-3"></span>Object Create
----------------------------------------

The inbound object create message requires all fields to construct the
initial object, except for the lock\_device\_id and owner\_device\_id.
These are only required if the respective functionality is configured.

<span id="anchor-4"></span>Object Retrieve
------------------------------------------

The object retrieve message only requires a device UUID and message
type.

<span id="anchor-5"></span>Object Update
----------------------------------------

The behavior of the object update message depends upon the configuration
of Smart Updates.

If Smart Updates are disabled, then all fields in the message are
required and the update is a full-replace message.

If Smart Updates are enabled, then only the message\_type and key fields
are required. Any location, rotation, scaling, or transform matrix sent
will be interpreted as a transformation, and applied to the existing
object in the DB.

<span id="anchor-6"></span>Object Destroy
-----------------------------------------

For the object destroy message, the message\_type and key fields are
required.

<span id="anchor-6a"></span>Object Lock
-----------------------------------------

If configured, locking on an object requires a key, message type, and lock_device_id.  This sends a lock request, which may be accepted or denied.

<span id="anchor-6b"></span>Object Unlock
-----------------------------------------

If configured, unlocking an object requires a key, message type, and lock_device_id.  This sends an unlock request, which may be accepted or denied.

<span id="anchor-7"></span>Section 4: Inbound Responses
=======================================================
| Field Name | Description | Data Type | Included? | | | |
| ---------- | ----------- | --------- | --------- | --- | --- | --- |
| | | | Create | Retrieve | Update | Destroy |
|  |  |  |  |  |  |  |
| error_code | <ul><li>NO_ERROR=0</li><li>ERROR=100</li><li>DB_ERROR=110</li><li>REDIS_ERROR=120</li><li>TRANSLATION_ERROR=130</li><li>BAD_SERVER_ERROR=140</li><li>BAD_REQUEST_ERROR=150</li></ul> | Integer | * | * | * | * |
| error_message | A description of any error that occurred | String | * | * | * | * |
| object_id | The key of the object | String | X | X | X | X |
| transaction_id | The Transaction ID.  This is the same as the transaction ID given on the inbound response and can be used to link the two together, if this functionality is configured. | String | * | * | * | * |

X – Guaranteed

\* - Potentially Included, depending on whether we have a success or
failure response and/or configuration

Note: While Inbound and Outbound Messages work based off of the
Obj3.proto file for protocol-buffer messaging, Inbound Responses work
based off of the Response.proto file.

<span id="anchor-8"></span>Section 5: Outbound Messages
=======================================================
| Field Name | Description | Data Type | Included? | | | |
| ---------- | ----------- | --------- | --------- | --- | --- | --- |
| | | | Create | Retrieve | Update | Destroy |
|  |  |  |  |  |  |  |
| message_type | <ul><li>OBJ_UPD=0</li><li>OBJ_CRT=1</li><li>OBJ_GET=2</li><li>OBJ_DEL=3</li><li>OBJ_LOCK=5</li><li>OBJ_UNLOCK=6</li><li>KILL=999</li><li>PING=555</li></ul> | Integer | X | X | X | X |
| key | The UUID of the object | String | X | X | X | X |
| name | The name of the object | String | X | X | X | |
| type | The type of the object, ie. "Mesh" | String | X | X | X | |
| subtype | The subtype of the object, ie. "Cube" | String | X | X | X | |
| location | The Location of the object | Array - Double | X | X | X | |
| rotation_euler | The Euler Rotation of the object | Array - Double | X | X | X | |
| rotation_quaternion | The Quaternion Rotation of the object | Array - Double | X | X | X | |
| scale | The Scaling of the object | Array - Double | X | X | X | |
| transform | The Transformation Matrix of the object | Array - Double | X | X | X | |
| bounding_box | The Bounding Box of the object | Array - Double | X | X | X | |
| scenes | The list of string scene UUID's | Array - String | X | X | X | |
| is_locked | 0 if unlocked, 1 if locked | Integer | * | * | * | |
| mesh_id | The UUID of the mesh that corresponds to this object. | String | X | X | X | |
| error_message | If outbound error responses are enabled, than an error message may be included here. | String | * | * | * | |
| transaction_id | The Transaction ID.  This is the same as the transaction ID given on the inbound response and can be used to link the two together, if this functionality is configured. | String | * | * | * | * |

X – Guaranteed

\* - Potentially Included, depending on configuration

<span id="anchor-9"></span>Object Create
----------------------------------------

An outbound Object Create Message notifies listeners that an object has
been created.

<span id="anchor-10"></span>Object Retrieve
-------------------------------------------

An outbound Object Retrieve Message notifies listeners that a response
has been lodged to a previous retrieval request.

<span id="anchor-11"></span>Object Update
-----------------------------------------

An outbound Object Update Message notifies listeners that an object has
been updated.

<span id="anchor-12"></span>Object Destroy
------------------------------------------

An outbound Object Create Message notifies listeners that an object has
been destroyed.

<span id="anchor-13"></span>Appendix: JSON Message Samples
==========================================================

<span id="anchor-14"></span>Inbound
-----------------------------------

### <span id="anchor-15"></span>Object Create

{

"message\_type": 0,

"key": “24575768452345”,

"name": "Object1",

"type": "Mesh",

"subtype": "Mesh",

"location": \[0.0, 0.0, 0.0\],

"rotation\_euler": \[0.0, 0.0, 0.0\],

"rotation\_quaternion": \[0.0, 0.0, 0.0, 0.0\],

"scale": \[0.0, 0.0, 0.0\],

"transform": \[

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0

\],

"bounding\_box": \[

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0

\],

"scenes": \["12346453456", "7685632194"\],

“owner\_device\_id”: “235jher1234ms”,

"lock\_device\_id": "-1",

“mesh\_id”: “234xfd432xf”

}

### <span id="anchor-16"></span>Object Retrieve

{

"message\_type": 1,

"key": “24575768452345”,

}

### <span id="anchor-17"></span>Object Update

{

"message\_type": 2,

"key": “24575768452345”,

"name": "Object1",

"type": "Mesh",

"subtype": "Mesh",

"location": \[0.0, 0.0, 0.0\],

"rotation\_euler": \[0.0, 0.0, 0.0\],

"rotation\_quaternion": \[0.0, 0.0, 0.0, 0.0\],

"scale": \[0.0, 0.0, 0.0\],

"transform": \[

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0

\],

"bounding\_box": \[

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0

\],

"scenes": \["12346453456", "7685632194"\],

"lock\_device\_id": "-1"

}

### <span id="anchor-18"></span>Object Destroy

{

"message\_type": 3,

"key": “24575768452345”,

"lock\_device\_id": "-1"

}

<span id="anchor-19"></span>Inbound Responses
---------------------------------------------

{

"error\_code": 0,

"error\_message": “”,

"transaction\_id": "abdsd545cxdf45",

"object\_id": "dfg546dcn453d"

}

<span id="anchor-20"></span>Outbound
------------------------------------

### <span id="anchor-21"></span>Object Create

{

"message\_type": 0,

"key": “24575768452345”,

"name": "Object1",

"type": "Mesh",

"subtype": "Mesh",

"location": \[0.0, 0.0, 0.0\],

"rotation\_euler": \[0.0, 0.0, 0.0\],

"rotation\_quaternion": \[0.0, 0.0, 0.0, 0.0\],

"scale": \[0.0, 0.0, 0.0\],

"transform": \[

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0

\],

"bounding\_box": \[

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0

\],

"scenes": \["12346453456", "7685632194"\],

"is\_locked": true,

“mesh\_id”: “234xfd432xf”

}

### <span id="anchor-22"></span>Object Retrieve

{

"message\_type": 1,

"key": “24575768452345”,

"name": "Object1",

"type": "Mesh",

"subtype": "Mesh",

"location": \[0.0, 0.0, 0.0\],

"rotation\_euler": \[0.0, 0.0, 0.0\],

"rotation\_quaternion": \[0.0, 0.0, 0.0, 0.0\],

"scale": \[0.0, 0.0, 0.0\],

"transform": \[

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0

\],

"bounding\_box": \[

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0

\],

"scenes": \["12346453456", "7685632194"\],

"is\_locked": true,

“mesh\_id”: “234xfd432xf”

}

### <span id="anchor-23"></span>Object Update

{

"message\_type": 2,

"key": “24575768452345”,

"name": "Object1",

"type": "Mesh",

"subtype": "Mesh",

"location": \[0.0, 0.0, 0.0\],

"rotation\_euler": \[0.0, 0.0, 0.0\],

"rotation\_quaternion": \[0.0, 0.0, 0.0, 0.0\],

"scale": \[0.0, 0.0, 0.0\],

"transform": \[

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0

\],

"bounding\_box": \[

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0

\],

"scenes": \["12346453456", "7685632194"\],

"is\_locked": true,

“mesh\_id”: “234xfd432xf”

}

### <span id="anchor-24"></span>Object Destroy

{

"message\_type": 3,

"key": “24575768452345”,

“mesh\_id”: “234xfd432xf”

}
