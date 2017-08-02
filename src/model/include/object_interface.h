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
#include "object_related.h"
#include "app_log.h"

#ifndef SRC_MODEL_INCLUDE_OBJECT_INTERFACE_H_
#define SRC_MODEL_INCLUDE_OBJECT_INTERFACE_H_

// An Object Interface defines the functions for the Object-3D Data Model
class ObjectInterface {
 public:
  virtual ~ObjectInterface() {}
  // String Getters
  virtual std::string get_name() const = 0;
  virtual std::string get_type() const = 0;
  virtual std::string get_subtype() const = 0;
  virtual std::string get_owner() const = 0;
  virtual std::string get_key() const = 0;
  virtual std::string get_scene() const = 0;
  // String Setters
  virtual void set_name(std::string new_name) = 0;
  virtual void set_type(std::string new_type) = 0;
  virtual void set_subtype(std::string new_subtype) = 0;
  virtual void set_owner(std::string new_owner) = 0;
  virtual void set_key(std::string new_key) = 0;
  virtual void set_scene(std::string new_scene) = 0;
  // Asset methods
  virtual int num_assets() const = 0;
  virtual void add_asset(std::string id) = 0;
  virtual std::string get_asset(int index) const = 0;
  virtual void remove_asset(int index) = 0;
  virtual void clear_assets() = 0;
  // Transform methods
  virtual void transform(Transformation *t) = 0;
  virtual bool has_transform() const = 0;
  virtual Transformation* get_transform() const = 0;
  // Take a target object and apply it's fields as changes to this Object
  virtual void merge(ObjectInterface *target) = 0;
  // to_json method to build an object to save to Mongo
  virtual std::string to_json() = 0;
};

#endif  // SRC_MODEL_INCLUDE_OBJECT_INTERFACE_H_
