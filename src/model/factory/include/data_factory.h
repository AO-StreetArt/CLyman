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

#include "model/object/include/object_interface.h"
#include "model/object/include/json_object.h"
#include "model/property/include/property_interface.h"
#include "model/property/include/json_property.h"

#ifndef SRC_MODEL_INCLUDE_DATA_FACTORY_H_
#define SRC_MODEL_INCLUDE_DATA_FACTORY_H_

// The DataFactory allows for creation of new instances of the ObjectInterface
// and PropertyInterface
class DataFactory {
 public:
  // Create an ObjectInterface with the default settings
  ObjectInterface* build_object() {return new JsonObject;}
  // Create a PropertyInterface with the default settings
  PropertyInterface* build_property() {return new JsonProperty;}
};

#endif  // SRC_MODEL_INCLUDE_DATA_FACTORY_H_
