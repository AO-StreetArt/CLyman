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

#include "Obj3.pb.h"

#include "object_list.h"
#include "object_factory.h"

#ifndef SRC_API_INCLUDE_PROTOBUF_OBJECT_LIST_H_
#define SRC_API_INCLUDE_PROTOBUF_OBJECT_LIST_H_

// An ObjectList stores a vector of pointers to ObjectDocuments
// It is responsible for parsing requests from external clients,
// and writing the responses to go back to those external clients.
class PbObjectList : public ObjectList {
  // String Return Value
  std::string protobuf_string;
  //Object Factory
  ObjectFactory ofactory;

 public:
  // Constructor
  PbObjectList() : ObjectList() {}
  ~PbObjectList() {}
  // Inbound Message Translation methods
  PbObjectList(protoObj3::Obj3List proto_list);
  // Copy Constructor
  PbObjectList(const PbObjectList &olist) : ObjectList(olist) {}
  // Message generation methods
  void to_msg_string(std::string &out_string);
};

#endif  // SRC_API_INCLUDE_PROTOBUF_OBJECT_LIST_H_
