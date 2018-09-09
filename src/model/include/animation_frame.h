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

#include "animation_frame_interface.h"

#ifndef SRC_MODEL_INCLUDE_ANIMATION_FRAME_H_
#define SRC_MODEL_INCLUDE_ANIMATION_FRAME_H_

// An Animation Frame Interface defines the functions for the Animation-Frame Data Model
// Represents a single animation frame document in Mongo.  Stores information
// from Animation Curves, namely graph handles associated to keyframes.  These
// are used by animators to tweak and refine animations.
class AnimationFrame : public AnimationFrameInterface {
  std::vector<AnimationGraphHandle*> translation;
  std::vector<AnimationGraphHandle*> rotation;
  std::vector<AnimationGraphHandle*> scale;
 public:
   AnimationFrame() {
     for (int i=0; i<3; i++) {
       translation.push_back(new AnimationGraphHandle);
       rotation.push_back(new AnimationGraphHandle);
       scale.push_back(new AnimationGraphHandle);
     }
     rotation.push_back(new AnimationGraphHandle);
   }
   AnimationFrame(const AnimationFrame& obj) = delete;
  ~AnimationFrame() {
    for (int i=0; i<3; i++) {
      delete translation[i];
      delete rotation[i];
      delete scale[i];
    }
    delete rotation[3];
  }

  // Access and set the Translation Graph Handles
  AnimationGraphHandle* get_translation(int index) {return translation[index];}
  void set_translation(int index, AnimationGraphHandle* new_handle) {translation[index] = new_handle;}

  // Access and set the Rotation Graph Handles
  AnimationGraphHandle* get_rotation(int index) {return rotation[index];}
  void set_rotation(int index, AnimationGraphHandle* new_handle) {rotation[index] = new_handle;}

  // Access and set the Scale Graph Handles
  AnimationGraphHandle* get_scale(int index) {return scale[index];}
  void set_scale(int index, AnimationGraphHandle* new_handle) {scale[index] = new_handle;}
};

#endif  // SRC_MODEL_INCLUDE_ANIMATION_FRAME_H_
