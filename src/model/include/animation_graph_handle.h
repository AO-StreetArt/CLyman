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

#ifndef SRC_MODEL_INCLUDE_ANIMATION_GRAPH_HANDLE_H_
#define SRC_MODEL_INCLUDE_ANIMATION_GRAPH_HANDLE_H_

// An Animation Frame Interface defines the functions for the Animation-Frame Data Model
// Represents a single animation frame document in Mongo.  Stores information
// from Animation Curves, namely handles for particular frames
class AnimationGraphHandle {
  float lh_x = 0.0;
  float lh_y = 0.0;
  float rh_x = 0.0;
  float rh_y = 0.0;
  std::string lh_type;
  std::string rh_type;
 public:
   AnimationGraphHandle() {}
  ~AnimationGraphHandle() {}
  // Animation Graph Handle - Left

  // Type of the handle
  std::string get_lh_type() const {return lh_type;}
  void set_lh_type(std::string new_type) {lh_type.assign(new_type);}

  // Location of the handle
  float get_lh_x() const {return lh_x;}
  float get_lh_y() const {return lh_y;}
  void set_lh_x(float x) {lh_x = x;}
  void set_lh_y(float y) {lh_y = y;}

  // Animation Graph Handle - Right

  // Type of the handle
  std::string get_rh_type() const {return rh_type;}
  void set_rh_type(std::string new_type) {rh_type.assign(new_type);}

  // Location of the handle
  float get_rh_x() const {return rh_x;}
  float get_rh_y() const {return rh_y;}
  void set_rh_x(float x) {rh_x = x;}
  void set_rh_y(float y) {rh_y = y;}
};

#endif  // SRC_MODEL_INCLUDE_ANIMATION_GRAPH_HANDLE_H_
