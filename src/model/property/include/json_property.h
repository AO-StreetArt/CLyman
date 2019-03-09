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
#include <iterator>
#include <map>
#include "animation_property.h"
#include "property_interface.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#ifndef SRC_MODEL_INCLUDE_JSON_PROPERTY_H_
#define SRC_MODEL_INCLUDE_JSON_PROPERTY_H_

// A Property defines the data model for object
// and scene-level values that are frameable but not associated
// to the transform of an object
// Represents a single document in Mongo
class JsonProperty : public AnimationProperty, public PropertyInterface {
 public:
  JsonProperty() : AnimationProperty() {}
  // Copy Constructor
  JsonProperty(const AnimationProperty &o) = delete;
  JsonProperty(const PropertyInterface &o) = delete;
  ~JsonProperty() {}
  // Property Key
  // The OID of the property in Mongo
  std::string get_key() const override {return RelatedData::get_key();}
  void set_key(std::string new_key) override {RelatedData::set_key(new_key);}
  // Parent Key
  // The OID of the original object in Mongo
  std::string get_parent() const override {return RelatedData::get_parent();}
  void set_parent(std::string new_key) override {RelatedData::set_parent(new_key);}
  // Property Name
  // The name of the property
  std::string get_name() const override {return AnimationProperty::get_name();}
  void set_name(std::string new_name) override {AnimationProperty::set_name(new_name);}
  // Scene ID
  // The Unique Identifier of the scene to which the object is associated
  std::string get_scene() const override {return RelatedData::get_scene();}
  void set_scene(std::string new_scene) override {RelatedData::set_scene(new_scene);}
  // Identifier for the piece of an asset corresponding to this object.
  // This identifier lets us associate an object to a piece of an asset
  // from a parent object.
  std::string get_asset_sub_id() const override {return RelatedData::get_asset_sub_id();}
  void set_asset_sub_id(std::string new_asset_sub_id) override {RelatedData::set_asset_sub_id(new_asset_sub_id);}
  // Property Value
  // The values of the property
  double get_value(int index) const override {return Property3d::get_value(index);}
  void set_value(int index, double new_value) override {Property3d::set_value(index, new_value);}
  void add_value(double new_value) override {Property3d::add_value(new_value);}
  unsigned int num_values() const override {return Property3d::num_values();}
  // Convert to an Event JSON
  void to_json(std::string& json_str) override;
  void to_json(std::string& json_str, int mtype) override;
  void to_json_writer(rapidjson::Writer<rapidjson::StringBuffer>& writer, int mtype) override;
  // Access actions
  void add_action(std::string name, AnimationAction<PropertyFrame> *new_action) override \
      {AnimationProperty::add_action(name, new_action);}
  AnimationAction<PropertyFrame>* get_action(std::string name) override \
      {return AnimationProperty::get_action(name);}
  std::map<std::string, AnimationAction<PropertyFrame>*>* get_actions() override \
      {return AnimationProperty::get_actions();}
};

#endif  // SRC_MODEL_INCLUDE_JSON_PROPERTY_H_
