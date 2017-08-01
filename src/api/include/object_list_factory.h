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

#include "Obj3.pb.h"

#include "object_list_interface.h"
#include "protobuf_object_list.h"
#include "json_object_list.h"

#ifndef SRC_API_INCLUDE_OBJECT_LIST_FACTORY_H_
#define SRC_API_INCLUDE_OBJECT_LIST_FACTORY_H_

// An ObjectList stores a vector of pointers to ObjectDocuments
// It is responsible for parsing requests from external clients,
// and writing the responses to go back to those external clients.
class ObjectListFactory {
 public:
  ObjectListInterface* build_object_list(const protoObj3::Obj3List proto_list) \
    {return new PbObjectList(proto_list);}
  ObjectListInterface* build_object_list(const rapidjson::Document& d) \
    {return new JsonObjectList(d);}
  ObjectListInterface* build_json_object_list() {return new JsonObjectList;}
  ObjectListInterface* build_proto_object_list() {return new PbObjectList;}
};

#endif  // SRC_API_INCLUDE_OBJECT_LIST_FACTORY_H_
