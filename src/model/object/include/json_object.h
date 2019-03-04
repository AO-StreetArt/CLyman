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
#include "property_interface.h"
#include "object_frame.h"
#include "object_document.h"
#include "animation_graph_handle.h"
#include "app/include/clyman_utils.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#ifndef SRC_MODEL_INCLUDE_JSON_OBJECT_H_
#define SRC_MODEL_INCLUDE_JSON_OBJECT_H_

// An Object Document extends a related object by:
//  - Storing Database metadata
//  - Offer parsing/writing methods for translation
//     to and from JSON.  These methods are meant for
//     use in communicating with the database ONLY,
//     external clients should communicate using the object_list API
class JsonObject : public ObjectDocument, public ObjectInterface {
  // String Return Value
  const char* json_cstr_val = NULL;
  std::string json_str_val;
  // String Return Value
  const char* transform_cstr_val = NULL;
  std::string transform_str_val;

 public:
  // Constructors
  JsonObject() : ObjectDocument() {}
  // Copy Constructor
  JsonObject(const ObjectDocument &o) = delete;
  JsonObject(const ObjectInterface &o) = delete;
  // Destructor
  ~JsonObject() {}
  // String Getters
  std::string get_name() const override {return ObjectDocument::get_name();}
  std::string get_type() const override {return ObjectDocument::get_type();}
  std::string get_subtype() const override {return ObjectDocument::get_subtype();}
  std::string get_owner() const override {return ObjectDocument::get_owner();}
  // String Setters
  void set_name(std::string new_name) override {ObjectDocument::set_name(new_name);}
  void set_type(std::string new_type) override {ObjectDocument::set_type(new_type);}
  void set_subtype(std::string new_subtype) override {ObjectDocument::set_subtype(new_subtype);}
  void set_owner(std::string new_owner) override {ObjectDocument::set_owner(new_owner);}
  // to_transform_message to build a JSON to send via UDP
  std::string to_transform_json() override;
  std::string to_transform_json(int mtype) override;
  void to_json_writer(rapidjson::Writer<rapidjson::StringBuffer>& writer, int mtype) override;
  // Inherited Methods
  // Transform methods
  void transform(Transformation *t) override {Object3d::transform(t);}
  bool has_transform() const override {return Object3d::has_transform();}
  Transformation* get_transform() const override {return Object3d::get_transform();}
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
  int num_props() const override {return ObjectDocument::num_props();}
  void add_prop(PropertyInterface *new_prop) override {ObjectDocument::add_prop(new_prop);}
  PropertyInterface* get_prop(int index) const override {return ObjectDocument::get_prop(index);}
  void remove_prop(int index) override {ObjectDocument::remove_prop(index);}
  void clear_props() override {ObjectDocument::clear_props();}
  // Access actions
  void add_action(std::string name, AnimationAction<ObjectFrame> *new_action) override \
      {ObjectDocument::add_action(name, new_action);}
  AnimationAction<ObjectFrame>* get_action(std::string name) override \
      {return ObjectDocument::get_action(name);}
};

#endif  // SRC_MODEL_INCLUDE_JSON_OBJECT_H_
