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
#include "animation_graph_handle.h"
#include "property_interface.h"
#include "data_frameable.h"
#include "data_related.h"
#include "app/include/clyman_utils.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#ifndef SRC_MODEL_INCLUDE_ANIMATION_PROPERTY_H_
#define SRC_MODEL_INCLUDE_ANIMATION_PROPERTY_H_

// A Property defines the data model for object
// and scene-level values that are frameable but not associated
// to the transform of an object
// Represents a single document in Mongo
class AnimationProperty : public FrameableData, public RelatedData, public PropertyInterface {
  std::vector<double> values;
  std::vector<AnimationGraphHandle*> handle;
  std::string name;
 public:
  AnimationProperty() {}
  // Parse a JSON document
  AnimationProperty(const rapidjson::Document &d);
  // Copy Constructor
  AnimationProperty(const AnimationProperty &o) = delete;
  AnimationProperty(const PropertyInterface &o) = delete;
  ~AnimationProperty() {for (AnimationGraphHandle* h : handle) delete h;}
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
  std::string get_name() const override {return name;}
  void set_name(std::string new_name) override {name.assign(new_name);}
  // Property Value
  // The values of the property
  double get_value(int index) const override {return values[index];}
  void set_value(int index, double new_value) override {values[index] = new_value;}
  void add_value(double new_value) override {
    values.push_back(new_value);
    handle.push_back(new AnimationGraphHandle);
  }
  int num_values() const override {return values.size();}
  // Scene ID
  // The Unique Identifier of the scene to which the object is associated
  std::string get_scene() const override {return RelatedData::get_scene();}
  void set_scene(std::string new_scene) override {RelatedData::set_scene(new_scene);}
  // Identifier for the piece of an asset corresponding to this object.
  // This identifier lets us associate an object to a piece of an asset
  // from a parent object.
  std::string get_asset_sub_id() const override {return RelatedData::get_asset_sub_id();}
  void set_asset_sub_id(std::string new_asset_sub_id) override {RelatedData::set_asset_sub_id(new_asset_sub_id);}
  // Convert to an Event JSON
  void to_json(std::string& json_str) const override;
  // Frame/Timestamp
  int get_frame() const override {return FrameableData::get_frame();}
  int get_timestamp() const override {return FrameableData::get_timestamp();}
  void set_frame(int new_frame) override {FrameableData::set_frame(new_frame);}
  void set_timestamp(int new_timestamp) override {FrameableData::set_timestamp(new_timestamp);}
  AnimationGraphHandle* get_handle(int index) override {return handle[index];}
  void set_handle(AnimationGraphHandle *new_handle, int index) override {handle[index] = new_handle;}
};

#endif  // SRC_MODEL_INCLUDE_ANIMATION_PROPERTY_H_
