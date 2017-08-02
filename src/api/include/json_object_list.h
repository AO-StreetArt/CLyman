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

#include <vector>
#include <string>

#include "object_document.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "object_list.h"
#include "object_factory.h"

#ifndef SRC_API_INCLUDE_JSON_OBJECT_LIST_H_
#define SRC_API_INCLUDE_JSON_OBJECT_LIST_H_

// An ObjectList stores a vector of pointers to ObjectDocuments
// It is responsible for parsing requests from external clients,
// and writing the responses to go back to those external clients.
class JsonObjectList : public ObjectList {
  // String Return Value
  const char* json_cstr_val;
  std::string json_str_val;
  //Object Factory
  ObjectFactory ofactory;

 public:
  // Constructor
  JsonObjectList() : ObjectList() {}
  ~JsonObjectList() {}
  // Inbound Message Translation methods
  JsonObjectList(const rapidjson::Document& d);
  // Copy Constructor
  JsonObjectList(const ObjectList &olist) : ObjectList(olist) {}
  // Message generation methods
  void to_msg_string(std::string &out_string);
};

#endif  // SRC_API_INCLUDE_JSON_OBJECT_LIST_H_
