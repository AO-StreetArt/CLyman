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
#include "object_frame.h"
#include "property_interface.h"

#ifndef SRC_MODEL_INCLUDE_OBJECT_INTERFACE_H_
#define SRC_MODEL_INCLUDE_OBJECT_INTERFACE_H_

// An Object Interface defines the functions for the Object-3D Data Model
// Represents a single document in Mongo
class ObjectInterface {
 public:
  virtual ~ObjectInterface() {}
  // Object Key
  // The OID of the object in Mongo
  virtual std::string get_key() const = 0;
  virtual void set_key(std::string new_key) = 0;
  // Parent Key
  // The OID of the original object in Mongo
  virtual std::string get_parent() const = 0;
  virtual void set_parent(std::string new_key) = 0;
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
  // Scene ID
  // The Unique Identifier of the scene to which the object is associated
  virtual std::string get_scene() const = 0;
  virtual void set_scene(std::string new_scene) = 0;
  // Identifier for the piece of an asset corresponding to this object.
  // This identifier lets us associate an object to a piece of an asset
  // from a parent object.
  virtual std::string get_asset_sub_id() const = 0;
  virtual void set_asset_sub_id(std::string new_asset_sub_id) = 0;
  // Transform methods
  virtual void transform(Transformation *t) = 0;
  virtual bool has_transform() const = 0;
  virtual Transformation* get_transform() const = 0;
  // Object Assets
  // A Unique ID corresponding to a record in the asset module
  // Represents mesh files, texture files, shader scripts, etc
  virtual int num_assets() const = 0;
  virtual void add_asset(std::string id) = 0;
  virtual std::string get_asset(int index) const = 0;
  virtual void remove_asset(int index) = 0;
  virtual void clear_assets() = 0;
  // Object Properties
  // Named sets of values that can be keyframed seperately
  virtual int num_props() const = 0;
  virtual void add_prop(PropertyInterface *new_prop) = 0;
  virtual PropertyInterface* get_prop(int index) const = 0;
  virtual void remove_prop(int index) = 0;
  virtual void clear_props() = 0;
  // Convert to an Event JSON
  virtual void to_json_writer(rapidjson::Writer<rapidjson::StringBuffer>& writer, int mtype) = 0;
  virtual std::string to_transform_json(int mtype) = 0;
  virtual std::string to_transform_json() = 0;
  // Access actions
  virtual void add_action(std::string name, AnimationAction<ObjectFrame> *new_action) = 0;
  virtual AnimationAction<ObjectFrame>* get_action(std::string name) = 0;
};

#endif  // SRC_MODEL_INCLUDE_OBJECT_INTERFACE_H_
