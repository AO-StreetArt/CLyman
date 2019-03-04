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
#include "animation_graph_handle.h"
#include "data_related.h"
#include "app/include/clyman_utils.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#ifndef SRC_MODEL_INCLUDE_PROPERTY_3D_H_
#define SRC_MODEL_INCLUDE_PROPERTY_3D_H_

// A Property  in 3d has an array of values, corresponding to each dimension.
class Property3d {
  std::vector<double> values;
 public:
  Property3d() {}
  // Explicitly disable copy constructor
  Property3d(const Property3d& obj) = delete;
  ~Property3d() {}
  // Property Value
  // The values of the property
  double get_value(int index) const {return values[index];}
  void set_value(int index, double new_value) {values[index] = new_value;}
  void add_value(double new_value) {
    values.push_back(new_value);
  }
  unsigned int num_values() const {return values.size();}
};

#endif  // SRC_MODEL_INCLUDE_PROPERTY_3D_H_
