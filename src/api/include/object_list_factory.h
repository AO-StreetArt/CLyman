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

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "object_list_interface.h"
#include "json_object_list.h"

#ifndef SRC_API_INCLUDE_OBJECT_LIST_FACTORY_H_
#define SRC_API_INCLUDE_OBJECT_LIST_FACTORY_H_

// The ObjectListFactory allows for creation of new instances of the
// ObjectListInterface
class ObjectListFactory {
 public:
  // Create an ObjectListInterface from a parsed Rapidjson Document
  ObjectListInterface* build_object_list(const rapidjson::Document& d) \
    {return new JsonObjectList(d);}
  // Create an empty ObjectListInterface which converts to JSON
  ObjectListInterface* build_json_object_list() {return new JsonObjectList;}
};

#endif  // SRC_API_INCLUDE_OBJECT_LIST_FACTORY_H_
