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

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "object_interface.h"
#include "json_object.h"
#include "object_list_interface.h"
#include "json_object_list.h"

#include "property_interface.h"
#include "json_property.h"
#include "property_list_interface.h"
#include "json_property_list.h"

#ifndef SRC_MODEL_INCLUDE_JSON_FACTORY_H_
#define SRC_MODEL_INCLUDE_JSON_FACTORY_H_

// The JsonFactory allows for creation of new instances data model interfaces
// from JSON strings
class JsonFactory {
  void parse_property_body(std::string& param_name, std::string& param_value, JsonProperty *new_prop);
  void parse_obj_body(std::string& param_name, std::string& param_value, JsonObject *new_obj);
  void parse_list_string_elements(std::string& param_name, std::string& param_value, DataListInterface *new_list);
  void parse_list_int_elements(std::string& param_name, int param_value, DataListInterface *new_list);
  void obj_from_iterator(JsonObject *new_obj, const rapidjson::GenericMember<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<> > &itr);
  void property_from_iterator(JsonProperty *new_prop, const rapidjson::GenericMember<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<> > &itr);
 public:
  // Create an ObjectInterface from a Parsed Rapidjson Document
  ObjectInterface* build_object(const rapidjson::Document& d);
  // Create an ObjectListInterface from a parsed Rapidjson Document
  ObjectListInterface* build_object_list(const rapidjson::Document& d);
  // Create a PropertyInterface from a Parsed Rapidjson Document
  PropertyInterface* build_property(const rapidjson::Document& d);
  // Create a PropertyListInterface from a parsed Rapidjson Document
  PropertyListInterface* build_property_list(const rapidjson::Document& d);
};

#endif  // SRC_MODEL_INCLUDE_JSON_FACTORY_H_
