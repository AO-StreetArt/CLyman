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
#include "animation_graph_handle.h"

#ifndef SRC_MODEL_INCLUDE_ANIMATION_FRAME_INTERFACE_H_
#define SRC_MODEL_INCLUDE_ANIMATION_FRAME_INTERFACE_H_

// An Animation Frame Interface defines the functions for the Animation-Frame Data Model
// Represents a single animation frame document in Mongo.  Stores information
// from Animation Curves, namely handles for particular frames
class AnimationFrameInterface {
 public:
  virtual ~AnimationFrameInterface() {}
  // Animation Graph Handle - Left

  // Access and set the Translation Graph Handles
  virtual AnimationGraphHandle* get_translation(int index) = 0;
  virtual void set_translation(int index, AnimationGraphHandle* new_handle) = 0;

  // Access and set the Rotation Graph Handles
  virtual AnimationGraphHandle* get_rotation(int index) = 0;
  virtual void set_rotation(int index, AnimationGraphHandle* new_handle) = 0;

  // Access and set the Scale Graph Handles
  virtual AnimationGraphHandle* get_scale(int index) = 0;
  virtual void set_scale(int index, AnimationGraphHandle* new_handle) = 0;
};

#endif  // SRC_MODEL_INCLUDE_ANIMATION_FRAME_INTERFACE_H_
