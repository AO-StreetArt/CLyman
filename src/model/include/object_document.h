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
#include "object_interface.h"
#include "Obj3.pb.h"
#include "app_log.h"

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
class ObjectDocument : public RelatedObject, public ObjectInterface {
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

 public:
  // Constructors
  ObjectDocument() : RelatedObject() {}
  // Parse a JSON document from Mongo
  ObjectDocument(const rapidjson::Document &d);
  // Copy Constructor
  ObjectDocument(const ObjectDocument &o);
  ObjectDocument(const ObjectInterface &o);
  // Destructor
  ~ObjectDocument() {}
  // String Getters
  std::string get_name() const {return name;}
  std::string get_type() const {return type;}
  std::string get_subtype() const {return subtype;}
  std::string get_owner() const {return owner;}
  // String Setters
  void set_name(std::string new_name) {name.assign(new_name);}
  void set_type(std::string new_type) {type.assign(new_type);}
  void set_subtype(std::string new_subtype) {subtype.assign(new_subtype);}
  void set_owner(std::string new_owner) {owner.assign(new_owner);}
  void write_string_attributes(ObjectInterface *target);
  // Take a target object and apply it's fields as changes to this Object
  void merge(ObjectInterface *target);
  // Take a target object and overwrite this object's fields with it
  void overwrite(ObjectInterface *target);
  // to_json method to build an object to save to Mongo
  std::string to_json(bool is_query);
  std::string to_json();
  // to_transform_message to build a JSON to send via UDP
  std::string to_transform_json();
  // Inherited Methods
  // Transform methods
  void transform(Transformation *t) {Object3d::transform(t);}
  bool has_transform() const {return Object3d::has_transform();}
  Transformation* get_transform() const {return Object3d::get_transform();}
  // String Getters
  std::string get_key() const {return RelatedObject::get_key();}
  std::string get_scene() const {return RelatedObject::get_scene();}
  // String Setters
  void set_key(std::string new_key) {RelatedObject::set_key(new_key);}
  void set_scene(std::string new_scene) {RelatedObject::set_scene(new_scene);}
  // Asset methods
  int num_assets() const {return RelatedObject::num_assets();}
  void add_asset(std::string id) {RelatedObject::add_asset(id);}
  std::string get_asset(int index) const \
    {return RelatedObject::get_asset(index);}
  void remove_asset(int index) {RelatedObject::remove_asset(index);}
  void clear_assets() {RelatedObject::clear_assets();}
};

#endif  // SRC_MODEL_INCLUDE_OBJECT_DOCUMENT_H_
