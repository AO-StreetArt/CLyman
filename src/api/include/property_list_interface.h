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

#include "model/include/property_interface.h"
#include "data_list_interface.h"

#ifndef SRC_API_INCLUDE_PROPERTY_LIST_INTERFACE_H_
#define SRC_API_INCLUDE_PROPERTY_LIST_INTERFACE_H_

// An ObjectList stores a vector of pointers to ObjectDocuments
// It is responsible for parsing requests from external clients,
// and writing the responses to go back to those external clients.
class PropertyListInterface : public DataListInterface {
 public:
  // Destructor
  virtual ~PropertyListInterface() {}
  // Object List
  // An array of Object Interfaces
  virtual int num_props() const = 0;
  virtual void add_prop(PropertyInterface *o) = 0;
  virtual PropertyInterface* get_prop(int index) const = 0;
  virtual void remove_prop(int index) = 0;
  virtual void clear_props() = 0;
};

#endif  // SRC_API_INCLUDE_PROPERTY_LIST_INTERFACE_H_
