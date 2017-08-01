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
#include "object_document.h"

#ifndef SRC_MODEL_INCLUDE_OBJECT_FACTORY_H_
#define SRC_MODEL_INCLUDE_OBJECT_FACTORY_H_

// An Object represents a single object in 3D space, and the factory
// Generates objects from parsed rapidjson documents
class ObjectFactory {
 public:
  ObjectInterface* build_object(const rapidjson::Document& d) \
    {return new ObjectDocument(d);}
  ObjectInterface* build_object() {return new ObjectDocument;}
};

#endif  // SRC_MODEL_INCLUDE_OBJECT_FACTORY_H_