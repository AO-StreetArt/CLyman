.. _object_api:

Object API
==========

An Object is represented by a transformation matrix representing it’s
position in 3-space, as well as a collection of Assets (Mesh files,
Texture files, Shader scripts, etc.), and potentially a frame and/or timestamp.
This API exposes CRUD and Query operations for Objects.

Objects are meant to be interacted with by individual devices,
and these changes will be streamed to other devices via the Events API.
In addition, Create and Update messages sent to the HTTP API are
converted to events and streamed out to registered devices.

Object Creation
~~~~~~~~~~~~~~~

.. http:post:: /v1/object/

   Create a new object.

   :reqheader Content-Type: Application/json
   :statuscode 200: Success

.. include:: _examples/object/object_create.rst

Object Update
~~~~~~~~~~~~~

.. http:post:: /v1/object/{object_key}

   Update an existing object.

   :reqheader Content-Type: Application/json
   :statuscode 200: Success
   :statuscode 404: Object Not Found

.. include:: _examples/object/object_update.rst

Object Retrieval
~~~~~~~~~~~~~~~~

.. http:get:: /v1/object/(object_key)

   Get an object details in JSON Format.

   :statuscode 200: Success
   :statuscode 404: Object Not Found

.. include:: _examples/object/object_get.rst

Object Deletion
~~~~~~~~~~~~~~~

.. http:delete:: /v1/object/(object_key)

   Delete an object.

   :statuscode 200: Success
   :statuscode 404: Object Not Found

.. include:: _examples/object/object_delete.rst

Object Query
~~~~~~~~~~~~

.. http:get:: /v1/object/query

   Query for objects which match the input JSON.  This will only
   return as many records as specified in the field 'num_records'.

   :statuscode 200: Success

.. include:: _examples/object/object_query.rst

Object Lock
~~~~~~~~~~~

.. http:get:: /v1/object/lock

   A lock allows a single client to obtain 'ownership' of an object.  This is
   an atomic operation, and is guaranteed to return a lock to one and only one
   client who requests it.  Keep in mind, however, that no checks are performed
   within CLyman to verify that the object's owner is the only one updating it,
   it is the responsibility of the client to obtain a lock prior to making updates.

   :statuscode 200: Success
   :statuscode 423: Object Lock Failed
   :query string device: Required. The ID of the Device requesting the lock.

.. include:: _examples/object/object_lock.rst

Object Unlock
~~~~~~~~~~~~~

.. http:delete:: /v1/scene/(scene_name)/object/lock

   Unlocking allows a client to release 'ownership' of an object.  This is
   an atomic operation, and no additional locks will be granted on a locked
   object until this method is called by the owner.

   :statuscode 200: Success
   :query string device: Required. The ID of the Device requesting the lock.

.. include:: _examples/object/object_unlock.rst

Asset Addition
~~~~~~~~~~~~~~

.. http:put:: /v1/object/{object_key}/asset/{asset_key}

   We can add an asset to an object easily with this API, which requires
   no message body.

   :statuscode 200: Success

.. include:: _examples/object/asset_add.rst

Asset Removal
~~~~~~~~~~~~~

.. http:delete:: /v1/object/{object_key}/asset/{asset_key}

   We can remove an asset from an object easily with this API, which requires
   no message body.

   :statuscode 200: Success

.. include:: _examples/object/asset_remove.rst
