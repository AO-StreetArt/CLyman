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
#include "object_3d.h"
#include "object_interface.h"
#include "data_related.h"
#include "data_frameable.h"
#include "animation_property.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#ifndef SRC_MODEL_INCLUDE_OBJECT_DOCUMENT_H_
#define SRC_MODEL_INCLUDE_OBJECT_DOCUMENT_H_

// An Object Document extends a related object by:
//  - Storing Database metadata
//  - Offer parsing/writing methods for translation
//     to and from JSON.  These methods are meant for
//     use in communicating with the database ONLY,
//     external clients should communicate using the object_list API
class ObjectDocument : public FrameableData, public RelatedData, public Object3d, public ObjectInterface {
  // String attributes
  std::string name;
  std::string type;
  std::string subtype;
  std::string owner;
  // String Return Value
  const char* json_cstr_val = NULL;
  std::string json_str_val;
  // String Return Value
  const char* transform_cstr_val = NULL;
  std::string transform_str_val;
  std::vector<AnimationProperty*> properties;
 public:
  // Constructors
  ObjectDocument() : RelatedData(), FrameableData(), Object3d() {}
  // Parse a JSON document
  ObjectDocument(const rapidjson::Document &d);
  // Copy Constructor
  ObjectDocument(const ObjectDocument &o);
  ObjectDocument(const ObjectInterface &o);
  // Destructor
  ~ObjectDocument() {clear_props();}
  // String Getters
  std::string get_name() const override {return name;}
  std::string get_type() const override {return type;}
  std::string get_subtype() const override {return subtype;}
  std::string get_owner() const override {return owner;}
  // String Setters
  void set_name(std::string new_name) override {name.assign(new_name);}
  void set_type(std::string new_type) override {type.assign(new_type);}
  void set_subtype(std::string new_subtype) override {subtype.assign(new_subtype);}
  void set_owner(std::string new_owner) override {owner.assign(new_owner);}
  void write_string_attributes(ObjectInterface *target);
  // Take a target object and apply it's fields as changes to this Object
  void merge(ObjectInterface *target) override;
  // Take a target object and overwrite this object's fields with it
  void overwrite(ObjectInterface *target) override;
  // to_transform_message to build a JSON to send via UDP
  std::string to_transform_json() override;
  // Inherited Methods
  // Transform methods
  void transform(Transformation *t) override {Object3d::transform(t);}
  bool has_transform() const override {return Object3d::has_transform();}
  Transformation* get_transform() const override {return Object3d::get_transform();}
  // Frame/timestamp
  int get_frame() const override {return FrameableData::get_frame();}
  int get_timestamp() const override {return FrameableData::get_timestamp();}
  void set_frame(int new_frame) override {FrameableData::set_frame(new_frame);}
  void set_timestamp(int new_timestamp) override {FrameableData::set_timestamp(new_timestamp);}
  // String Getters
  std::string get_key() const override {return RelatedData::get_key();}
  std::string get_asset_sub_id() const override {return RelatedData::get_asset_sub_id();}
  std::string get_scene() const override {return RelatedData::get_scene();}
  std::string get_parent() const override {return RelatedData::get_parent();}
  // String Setters
  void set_key(std::string new_key) override {RelatedData::set_key(new_key);}
  void set_asset_sub_id(std::string new_key) override {RelatedData::set_asset_sub_id(new_key);}
  void set_scene(std::string new_scene) override {RelatedData::set_scene(new_scene);}
  void set_parent(std::string new_parent) override {RelatedData::set_parent(new_parent);}
  // Asset methods
  int num_assets() const override {return RelatedData::num_assets();}
  void add_asset(std::string id) override {RelatedData::add_asset(id);}
  std::string get_asset(int index) const override {return RelatedData::get_asset(index);}
  void remove_asset(int index) override {RelatedData::remove_asset(index);}
  void clear_assets() override {RelatedData::clear_assets();}
  // Object Properties
  int num_props() const {return properties.size();}
  void add_prop(AnimationProperty *new_prop) {properties.push_back(new_prop);}
  AnimationProperty* get_prop(int index) const {return properties[index];}
  void remove_prop(int index) {properties.erase(properties.begin()+index);}
  void clear_props() {
    for (int i = 0; i < properties.size(); i++) {
      delete properties[i];
    }
    properties.clear();
  }
  // Animation Frame
  AnimationFrameInterface* get_animation_frame() {return FrameableData::get_animation_frame();}
  void set_animation_frame(AnimationFrameInterface *new_aframe) {FrameableData::set_animation_frame(new_aframe);}
};

#endif  // SRC_MODEL_INCLUDE_OBJECT_DOCUMENT_H_
