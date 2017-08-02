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
#include <exception>
#include "transforms.h"

#ifndef SRC_MODEL_INCLUDE_OBJECT_3D_H_
#define SRC_MODEL_INCLUDE_OBJECT_3D_H_

// An Obj3 Exception

// A child class of std::exception
// which holds error information
struct Object3dException: public std::exception {
  // An error message passed on initialization
  std::string int_msg;
  const char * int_msg_cstr;

  // Create a Neo4j Exception, and store the given error message
  inline Object3dException(std::string msg) {
    int_msg = "Error in Object: " + msg;
    int_msg_cstr = int_msg.c_str();
  }

  Object3dException() {}
  ~Object3dException() throw() {}
  // Show the error message in readable format
  const char * what() const throw() {
    return int_msg_cstr;
  }
};

// The Object3d class stores data for a single object in 3-space.
// Basically, wraps a transform and provides some convinience methods
class Object3d {
  // Transformation matrix
  Transformation *trans = NULL;

 public:
  // Constructors
  Object3d() {trans = new Transformation;}
  // Copy Constructor
  inline Object3d(const Object3d &o) {
    trans = new Transformation;
    trans->multiply(*(o.get_transform()));
  }
  // Destructor
  virtual ~Object3d() {if (trans) delete trans;}
  // Transform methods
  void transform(Transformation *t) {trans->multiply(*t);}
  bool has_transform() const {if (trans) {return true;} else {return false;}}
  Transformation* get_transform() const {return trans;}
};

#endif  // SRC_MODEL_INCLUDE_OBJECT_3D_H_
