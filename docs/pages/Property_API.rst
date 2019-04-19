.. _property_api:

Property API
============

A Property is a set of between 1 and 4 double values, which may
or not be associated to objects.  Properties can also support frames,
just like objects, but cannot be locked and have no transformations.

Properties are meant to be interacted with by individual devices,
and these changes will be streamed to other devices via the Events API.
In addition, Create and Update messages sent to the HTTP API are
converted to events and streamed out to registered devices.

Property Creation
~~~~~~~~~~~~~~~~~

.. http:post:: /v1/property/

   Create a new property.

   :reqheader Content-Type: Application/json
   :statuscode 200: Success

.. include:: _examples/property/property_create.rst

Property Update
~~~~~~~~~~~~~~~

.. http:post:: /v1/property/{property_key}

   Update an existing property.

   :reqheader Content-Type: Application/json
   :statuscode 200: Success
   :statuscode 404: Property Not Found

.. include:: _examples/property/property_update.rst

Property Retrieval
~~~~~~~~~~~~~~~~~~

.. http:get:: /v1/property/(property_key)

   Get property details in JSON Format.

   :statuscode 200: Success
   :statuscode 404: Property Not Found

.. include:: _examples/property/property_get.rst

Property Deletion
~~~~~~~~~~~~~~~~~

.. http:delete:: /v1/property/(property_key)

   Delete an property.

   :statuscode 200: Success
   :statuscode 404: Property Not Found

.. include:: _examples/property/property_delete.rst

Property Query
~~~~~~~~~~~~~~

.. http:get:: /v1/property/query

   Query for properties which match the input JSON.  This will only
   return as many records as specified in the field 'num_records'.

   :statuscode 200: Success

.. include:: _examples/property/property_query.rst

Property Action Create
~~~~~~~~~~~~~~~~~~~~~~

.. http:post:: /v1/property/{key}/action

   An action is a named set of keyframes, each holding the value of the property
   at that frame.  This endpoint allows creation of a new action against an
   existing property.

   :statuscode 200: Success

.. include:: _examples/property/property_action_create.rst

Property Action Update
~~~~~~~~~~~~~~~~~~~~~~

.. http:post:: /v1/property/{key}/action/{name}

   This endpoint allows for updating fields within an action.

   :statuscode 200: Success

.. include:: _examples/property/property_action_update.rst

Property Action Delete
~~~~~~~~~~~~~~~~~~~~~~

.. http:delete:: /v1/property/{key}/action/{name}

   This endpoint allows for removing actions from an property.

   :statuscode 200: Success

.. include:: _examples/property/property_action_delete.rst

Property Frame Creation
~~~~~~~~~~~~~~~~~~~~~~~

.. http:post:: /v1/property/{key}/action/{name}/keyframe

   This endpoint allows for adding keyframes to existing actions.

   :statuscode 200: Success

.. include:: _examples/property/property_frame_create.rst

Property Frame Update
~~~~~~~~~~~~~~~~~~~~~

.. http:post:: /v1/property/{key}/action/{name}/keyframe/{frame}

   This endpoint allows for updating elements within a keyframe.

   :statuscode 200: Success

.. include:: _examples/property/property_frame_update.rst

Property Frame Delete
~~~~~~~~~~~~~~~~~~~~~

.. http:delete:: /v1/property/{key}/action/{name}/keyframe/{frame}

   This endpoint removes a keyframe from an existing action.

   :statuscode 200: Success

.. include:: _examples/property/property_frame_delete.rst
