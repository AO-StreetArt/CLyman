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
#include "app/include/clyman_utils.h"
#include "model/core/include/animation_graph_handle.h"
#include "model/core/include/data_related.h"
#include "model/property/include/property_interface.h"
#include "object_3d.h"
#include "object_frame.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#ifndef SRC_MODEL_INCLUDE_OBJECT_DOCUMENT_H_
#define SRC_MODEL_INCLUDE_OBJECT_DOCUMENT_H_

// An Object Document extends a related object by:
//  - Storing Database metadata
//  - Offer parsing/writing methods for translation
//     to and from JSON.  These methods are meant for
//     use in communicating with the database ONLY,
//     external clients should communicate using the object_list API
class ObjectDocument : public RelatedData, public Object3d {
  // String attributes
  std::string name;
  std::string type;
  std::string subtype;
  std::string owner;
  // Map of actions containing keyframes
  std::map<std::string, AnimationAction<ObjectFrame>*> action_map;
 public:
  // Constructors
  ObjectDocument() : RelatedData(), Object3d() {}

  // Copy Constructor
  ObjectDocument(const ObjectDocument &o) = delete;
  ObjectDocument(const ObjectInterface &o) = delete;

  // Destructor
  virtual ~ObjectDocument() {for (auto map_itr = action_map.begin(); map_itr != action_map.end(); ++map_itr) \
      {delete map_itr->second;}}

  // String Getters
  std::string get_name() const {return name;}
  std::string get_type() const {return type;}
  std::string get_subtype() const {return subtype;}
  std::string get_owner() const {return owner;}

  // String Setters
  void set_name(std::string new_name) {name.assign(new_name);}
  void set_type(std::string new_type) {type.assign(new_type);}
  void set_subtype(std::string new_subtype) {subtype.assign(new_subtype);}
  void set_owner(std::string new_owner) {owner.assign(new_owner);}

  // Access actions
  void add_action(std::string name, AnimationAction<ObjectFrame> *new_action) \
      {action_map.emplace(name, new_action);}
  AnimationAction<ObjectFrame>* get_action(std::string name) \
      {return action_map[name];}
  std::map<std::string, AnimationAction<ObjectFrame>*>* get_actions() {return &action_map;}
};

#endif  // SRC_MODEL_INCLUDE_OBJECT_DOCUMENT_H_
