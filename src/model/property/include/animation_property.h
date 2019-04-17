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
#include <iterator>
#include <map>
#include "model/core/include/animation_action.h"
#include "model/core/include/data_related.h"
#include "property_frame.h"
#include "property_interface.h"
#include "property_3d.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#ifndef SRC_MODEL_INCLUDE_ANIMATION_PROPERTY_H_
#define SRC_MODEL_INCLUDE_ANIMATION_PROPERTY_H_

// A Property defines the data model for object
// and scene-level values that are frameable but not associated
// to the transform of an object
// Represents a single document in Mongo
class AnimationProperty : public Property3d, public RelatedData {
  std::map<std::string, AnimationAction<PropertyFrame>*> action_map;
  std::string name;
 public:
  AnimationProperty() {}
  // Parse a JSON document
  AnimationProperty(const rapidjson::Document &d);
  // Copy Constructor
  AnimationProperty(const AnimationProperty &o) = delete;
  AnimationProperty(const PropertyInterface &o) = delete;
  virtual ~AnimationProperty() {for (auto itr = action_map.begin(); itr != action_map.end(); ++itr) {delete itr->second;}}
  // Property Name
  // The name of the property
  std::string get_name() const {return name;}
  void set_name(std::string new_name) {name.assign(new_name);}
  // Access actions
  void add_action(std::string name, AnimationAction<PropertyFrame> *new_action) \
      {action_map.emplace(name, new_action);}
  AnimationAction<PropertyFrame>* get_action(std::string name) {
    auto action_itr = action_map.find(name);
    if (action_itr != action_map.end()) {
      return action_itr->second;
    }
    return nullptr;
  }
  std::map<std::string, AnimationAction<PropertyFrame>*>* get_actions() {return &action_map;}
};

#endif  // SRC_MODEL_INCLUDE_ANIMATION_PROPERTY_H_
