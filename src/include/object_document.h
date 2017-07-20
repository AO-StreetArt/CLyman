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
#include "Obj3.pb.h"
#include "app_log.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#ifndef SRC_INCLUDE_OBJECT_DOCUMENT_H_
#define SRC_INCLUDE_OBJECT_DOCUMENT_H_

class ObjectDocument : public RelatedObject {
  // String attributes
  std::string name;
  std::string type;
  std::string subtype;
  std::string owner;
  // String Return Value
  const char* json_cstr_val = NULL;
  std::string json_str_val;

 public:
  // Constructors
  ObjectDocument() : RelatedObject() {}
  // Parse a JSON document from Mongo
  ObjectDocument(const rapidjson::Document &d);
  // Copy Constructor
  ObjectDocument(const ObjectDocument &o);
  // Destructor
  ~ObjectDocument() : ~RelatedObject() {if (json_cstr_val) delete json_cstr_val;}
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
  // Take a target object and apply it's fields as changes to this Object
  void merge(ObjectDocument *target);
  // to_json method to build an object to save to Mongo
  std::string to_json();
};

#endif  // SRC_INCLUDE_OBJECT_DOCUMENT_H_
