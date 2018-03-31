/*
Apache2 License Notice
Copyright 2017 Alex Barry

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <string>
#include <vector>
#include <exception>
#include "object_related.h"
#include "app_log.h"
#include "aossl/mongo/include/mongo_buffer_interface.h"

#ifndef SRC_MODEL_INCLUDE_OBJECT_INTERFACE_H_
#define SRC_MODEL_INCLUDE_OBJECT_INTERFACE_H_

// An Object Interface defines the functions for the Object-3D Data Model
// Represents a single document in Mongo
class ObjectInterface {
 public:
  virtual ~ObjectInterface() {}
  // Object Name
  // Non-Unique Object identifier
  virtual std::string get_name() const = 0;
  virtual void set_name(std::string new_name) = 0;
  // Object Type
  // The type of object, used in the specification of primitives
  // Examples: Mesh, Curve, etc
  virtual std::string get_type() const = 0;
  virtual void set_type(std::string new_type) = 0;
  // Object Subtype
  // The subtype of the object, used in the specification of primitives
  // Examples: Cube, Icosphere, Bezier Curve, etc
  virtual std::string get_subtype() const = 0;
  virtual void set_subtype(std::string new_subtype) = 0;
  // Object Owner
  // The device which owns the object, informational
  virtual std::string get_owner() const = 0;
  virtual void set_owner(std::string new_owner) = 0;
  // Object Key
  // The OID of the object in Mongo
  virtual std::string get_key() const = 0;
  virtual void set_key(std::string new_key) = 0;
  // Scene ID
  // The Unique Identifier of the scene to which the object is associated
  virtual std::string get_scene() const = 0;
  virtual void set_scene(std::string new_scene) = 0;
  // Object Assets
  // A Unique ID corresponding to a record in the asset module
  // Represents mesh files, texture files, shader scripts, etc
  virtual int num_assets() const = 0;
  virtual void add_asset(std::string id) = 0;
  virtual std::string get_asset(int index) const = 0;
  virtual void remove_asset(int index) = 0;
  virtual void clear_assets() = 0;
  // Object Transform
  // The transform holds the objects position, rotation, and scaling in 3-space
  virtual void transform(Transformation *t) = 0;
  virtual bool has_transform() const = 0;
  virtual Transformation* get_transform() const = 0;
  // Frame/Timestamp
  virtual int get_frame() const = 0;
  virtual int get_timestamp() const = 0;
  virtual void set_frame(int new_frame) = 0;
  virtual void set_timestamp(int new_timestamp) = 0;
  // Take a target object and apply it's fields as changes to this Object
  virtual void merge(ObjectInterface *target) = 0;
  // Take a target object and overwrite this object's fields with it
  virtual void overwrite(ObjectInterface *target) = 0;
  // to_bson method to build an object to save to Mongo
  virtual void to_bson(bool is_query, AOSSL::MongoBufferInterface *bson) = 0;
  virtual void to_bson(AOSSL::MongoBufferInterface *bson) = 0;
  // to_bson_update which outputs fields as update operators
  // this method should get used for OVERWRITE type messages
  virtual void to_bson_update(bool is_append_operation, bool is_query, AOSSL::MongoBufferInterface *bson) = 0;
  virtual void to_bson_update(bool is_query, AOSSL::MongoBufferInterface *bson) = 0;
  virtual void to_bson_update(AOSSL::MongoBufferInterface *bson) = 0;
  // to_json method to build an object to save to Mongo
  virtual std::string to_json() = 0;
  virtual std::string to_json(bool is_query) = 0;
  virtual std::string to_transform_json() = 0;
};

#endif  // SRC_MODEL_INCLUDE_OBJECT_INTERFACE_H_
