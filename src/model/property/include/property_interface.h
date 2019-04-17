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
#include <iterator>
#include "property_frame.h"
#include "model/core/include/animation_action.h"

#ifndef SRC_MODEL_INCLUDE_PROPERTY_INTERFACE_H_
#define SRC_MODEL_INCLUDE_PROPERTY_INTERFACE_H_

// A Property Interface defines the data model for object
// and scene-level values that are frameable but not associated
// to the transform of an object
// Represents a single document in Mongo
class PropertyInterface {
 public:
  virtual ~PropertyInterface() {}
  // Property Key
  // The OID of the property in Mongo
  virtual std::string get_key() const = 0;
  virtual void set_key(std::string new_key) = 0;
  // Parent Key
  // The OID of the original object in Mongo
  virtual std::string get_parent() const = 0;
  virtual void set_parent(std::string new_key) = 0;
  // Property Name
  // The name of the property
  virtual std::string get_name() const = 0;
  virtual void set_name(std::string new_name) = 0;
  // Property Value
  // The values of the property
  virtual double get_value(int index) const = 0;
  virtual void set_value(int index, double new_value) = 0;
  virtual void add_value(double new_value) = 0;
  virtual unsigned int num_values() const = 0;
  // Scene ID
  // The Unique Identifier of the scene to which the object is associated
  virtual std::string get_scene() const = 0;
  virtual void set_scene(std::string new_scene) = 0;
  // Identifier for the piece of an asset corresponding to this object.
  // This identifier lets us associate an object to a piece of an asset
  // from a parent object.
  virtual std::string get_asset_sub_id() const = 0;
  virtual void set_asset_sub_id(std::string new_asset_sub_id) = 0;
  // Convert to an Event JSON
  virtual void to_json(std::string& json_str) = 0;
  virtual void to_json(std::string& json_str, int mtype) = 0;
  virtual void to_json_writer(rapidjson::Writer<rapidjson::StringBuffer>& writer, int mtype) = 0;
  // Access actions
  virtual void add_action(std::string name, AnimationAction<PropertyFrame> *new_action) = 0;
  virtual AnimationAction<PropertyFrame>* get_action(std::string name) = 0;
  virtual std::map<std::string, AnimationAction<PropertyFrame>*>* get_actions() = 0;
};

#endif  // SRC_MODEL_INCLUDE_PROPERTY_INTERFACE_H_
