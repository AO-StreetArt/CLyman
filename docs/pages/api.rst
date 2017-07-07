.. _api:

API Overview
============

The Clyman API utilizes either JSON or Protocol Buffers, based on what
the server is configured to process. In either case, the field names and
message structure remains the same. This document will focus on the JSON
API, but with this knowledge and the DVS Interface Protocol Buffer
files, the use of the Protocol Buffer API should be equally clear.

Response Messages follow the same format as inbound messages, with a few
additional fields.

To start with, here is an example JSON message which will create a
single object:

{

 "msg\_type": 0,

 "transaction\_id": "12352",

 "num\_records": 1,

 "objects": [

 {

 "key": "ABCDEF131",

 "name": "Test Object8",

 "type": "Mesh",

 "subtype": "Cube",

 "owner": "123",

 "scene": "DEFGHI8",

 "translation": [0, 0, 0],

 "rotation\_euler": [0, 0, 0],

 "rotation\_quaternion": [0, 0, 0, 0],

 "scale": [1, 1, 1],

 "assets": ["Asset\_4"]

 }

 ]

}

Let’s take a look at the individual fields.

Object List
-----------

The Object List is the highest level wrapper in the API. It only
contains 6 keys, one of which is an array of objects.

-  msg\_type – 0 for create, 1 for update, 2 for retrieve, 3 for delete,
   4 for query. The message type applies to all objects in the objects
   array.
-  err\_code - An integer error code for the response, full list of codes
   can be found in the appendix.
-  err\_msg - A string error message for the response, will not be present when
   no error was encountered.
-  transaction\_id – An ID to distinguish a transaction within a larger
   network of applications
-  num\_records – This lets us give Clyman a maximum number of values to
   return from a query
-  objects – An array containing objects

Object
------

A single Object (or Obj3, from here on out), is represented by a single
element of the array from the “objects” key of the object list.

-  key – Obj3 Key value (UUID)
-  name – Name of the Obj3
-  type – A string type of the Obj3. Normal types are ‘Mesh’, ‘Curve’,
   etc. However, no limitations are placed on what types may be entered.
-  subtype – Similar to type, meant to differentiate between different
   types and allow for use of basic primitives which can be communicated
   from device to device very quickly. For example, ‘Cube’, ‘Sphere’,
   etc.
-  owner – Identifier for the owner of the Obj3
-  scene – ID For the Scene containing the object
-  translation – X, Y, and Z values for the translation of the object
   from it’s origin
-  rotation\_euler – X, Y, and Z values for the local rotation of the object
   about each axis
-  rotation\_quaternion – W, X, Y, and Z values for the local quaternion
   rotation of the object
-  scale – X, Y, and Z values for the scaling of the object
-  assets – An Array of identifiers for “assets”, which should be
   downloaded by users in order to view the object.

Field Mapping
=============

+------------------------+------------------+------------+----------+------------+------------+-----------+
| **Field**              | **Data Type**    | **Create** | **Get**  | **Update** | **Delete** | **Query** |
+------------------------+------------------+------------+----------+------------+------------+-----------+
| msg\_type              | Integer          | X          | X        | X          | X          | X         |
+------------------------+------------------+------------+----------+------------+------------+-----------+
| transaction\_id        | String           | \*         | \*       | \*         | \*         | \*        |
+------------------------+------------------+------------+----------+------------+------------+-----------+
| num\_records           | String           |            |          |            |            | \*        |
+------------------------+------------------+------------+----------+------------+------------+-----------+
| key                    | String           |            | X        | \*         | X          | \*        |
+------------------------+------------------+------------+----------+------------+------------+-----------+
| name                   | String           | X          |          | \*         |            | \*        |
+------------------------+------------------+------------+----------+------------+------------+-----------+
| type                   | String           | \*         |          | \*         |            | \*        |
+------------------------+------------------+------------+----------+------------+------------+-----------+
| subtype                | String           | \*         |          | \*         |            | \*        |
+------------------------+------------------+------------+----------+------------+------------+-----------+
| owner                  | String           | \*         |          | \*         |            | \*        |
+------------------------+------------------+------------+----------+------------+------------+-----------+
| scene                  | String           | X          |          | \*         |            | \*        |
+------------------------+------------------+------------+----------+------------+------------+-----------+
| translation            | Array - Double   | X          |          | \*         |            | \*        |
+------------------------+------------------+------------+----------+------------+------------+-----------+
| rotation\_euler        | Array - Double   | X          |          | \*         |            | \*        |
+------------------------+------------------+------------+----------+------------+------------+-----------+
| rotation\_quaternion   | Array - Double   | X          |          | \*         |            | \*        |
+------------------------+------------------+------------+----------+------------+------------+-----------+
| scale                  | Array - Double   | X          |          | \*         |            | \*        |
+------------------------+------------------+------------+----------+------------+------------+-----------+
| assets                 | Array - String   | X          |          | \*         |            |           |
+------------------------+------------------+------------+----------+------------+------------+-----------+

X – Required

\* - Optional

Message Types
=============

Object Create
-------------

Create a new Obj3. Returns a unique key for the object.

Object Retrieve
---------------

The object retrieve message will retrieve an object by key, and return
the full object

Object Update
-------------

Object updates can be used to either update basic object attributes
(name, type, etc), or to apply transformations to the object.
Transformations will be applied in the order that they are received, and
if Atomic Transactions are enabled, then they will be applied in the
order that they are received even if sent to different instances of
Clyman, as long as they are connected to the same Redis instance.

Object Destroy
--------------

Destroy an existing Obj3 by key. Basic success/failure response.

Appendix A: JSON Message Samples
================================

Inbound
-------

Object Create
~~~~~~~~~~~~~

{

 "msg\_type": 0,

 "transaction\_id": "12354",

 "num\_records": 1,

 "objects": [

 {

 "key": "ABCDEF133",

 "name": "Test Object10",

 "type": "Mesh",

 "subtype": "Cube",

 "owner": "123",

 "scene": "DEFGHI10",

 "translation": [0, 0, 0],

 "rotation\_euler": [0, 0, 0],

 "rotation\_quaternion": [0, 0, 0, 0],

 "scale": [1, 1, 1],

 "assets": ["Asset\_5"]

 }

 ]

}

Object Retrieve
~~~~~~~~~~~~~~~

{

 "msg\_type": 2,

 "transaction\_id": "123463",

 "num\_records": 128,

 "objects": [

 {

 "key": "5951dd759af59c00015b1409"

 }

 ]

}

Object Update
~~~~~~~~~~~~~

{

 "msg\_type": 1,

 "transaction\_id": "123464",

 "num\_records": 1,

 "objects": [

 {

 "key": "5951dd759af59c00015b140a",

 "name": "Test Object 123464",

 "type": "Curve",

 "subtype": "Sphere",

 "owner": "456",

 "scene": "DEFGHIJ123464",

 "translation": [0, 0, 1],

 "rotation\_euler": [0, 0, 0],

 "rotation\_quaternion": [0, 0.2, 0.3, 0.5],

 "scale": [1, 1, 2],

 "assets": ["Asset\_5"]

 }

 ]

}

Object Destroy
~~~~~~~~~~~~~~

{

 "msg\_type": 3,

 "transaction\_id": "123463",

 "num\_records": 1,

 "objects": [

 {

 "key": "5951dd759af59c00015b1409"

 }

 ]

}

Object Query
~~~~~~~~~~~~

{

 "msg\_type": 4,

 "transaction\_id": "123463",

 "num\_records": 1,

 "objects": [

 {

 "name": "Test Object 123463"

 },

 {

 "name": "Bad Object"

 }

 ]

}

Response
--------

Object Create
~~~~~~~~~~~~~

{
  "msg\_type":0,
  "err\_code":100,
  "num\_records":1,
  "objects":[
    {
      "key":"5951dd759af59c00015b140b"
    }
  ]
}

Object Update
~~~~~~~~~~~~~

{
  "msg\_type":1,
  "err\_code":100,
  "num\_records":1,
  "objects":[
    {
      "key":"5951dd759af59c00015b1409",
      "name":"Test Object 123463",
      "scene":"DEFGHIJ123463",
      "type":"Mesh",
      "subtype":"Cube",
      "owner":"456",
      "translation":[0.0,0.0,0.0],
      "scale":[1.0,2.0,1.0]
    }
  ]
}

Object Retrieve
~~~~~~~~~~~~~~~

{
  "msg\_type":2,
  "err\_code":100,
  "num\_records":1,
  "objects":[
    {
      "name":"Test Object10",
      "scene":"DEFGHI10",
      "type":"Mesh",
      "subtype":"Cube",
      "owner":"123",
      "translation":[0.0,0.0,0.0],
      "scale":[1.0,1.0,1.0]
    }
  ]
}

Object Destroy
~~~~~~~~~~~~~~

{
  "msg\_type":3,
  "err\_code":100,
  "num\_records":1,
  "objects":[
    {
      "key":"5951dd759af59c00015b1408"
    }
  ]
}

Object Query
~~~~~~~~~~~~

{
  "msg\_type":4,
  "err\_code":100,
  "num\_records":2,
  "objects":[
    {
      "name":"Test Object 123465",
      "scene":"DEFGHIJ123465",
      "type":"Mesh",
      "subtype":"Cube",
      "owner":"456",
      "translation":[0.0,0.0,0.0],
      "scale":[1.0,1.0,2.0]
    },
    {
      "name":"Test Object 123456",
      "scene":"DEFGHIJ123456",
      "type":"Curve",
      "subtype":"Sphere",
      "owner":"456",
      "translation":[0.0,0.0,0.0],
      "scale":[2.0,1.0,1.0]
    }
  ]
}

Appendix B: Error Codes
=======================

NO\_ERROR = 100

Operation was successful

ERROR = 101

An unknown error occurred

NOT\_FOUND = 102

Data was not found

TRANSLATION\_ERROR = 110

JSON/Protocol Buffer parsing error

PROCESSING\_ERROR = 120

Unknown error occurred during processing stage of execution

BAD\_MSG\_TYPE\_ERROR = 121

An invalid msg_type was recieved (valid values are integers from 0 to 4)

INSUFF\_DATA\_ERROR = 122

Insufficient data received on message to form a valid response


:ref:`Go Home <index>`
