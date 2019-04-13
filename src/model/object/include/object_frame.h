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

#include "model/core/include/animation_frame.h"
#include "model/core/include/animation_graph_handle.h"
#include "object_3d.h"
#include "model/core/include/data_related.h"

#ifndef SRC_MODEL_INCLUDE_OBJECT_FRAME_H_
#define SRC_MODEL_INCLUDE_OBJECT_FRAME_H_

// An Object Frame is an animation frame with a transform for 3d objects
class ObjectFrame : public AnimationFrame, public Object3d, public RelatedData {
  std::vector<AnimationGraphHandle*> translation;
  std::vector<AnimationGraphHandle*> rotation;
  std::vector<AnimationGraphHandle*> scale;
 public:
  // Constructors
  ObjectFrame() : AnimationFrame(), Object3d() {
    for (int i=0; i<3; i++) {
      translation.push_back(new AnimationGraphHandle);
      rotation.push_back(new AnimationGraphHandle);
      scale.push_back(new AnimationGraphHandle);
    }
    rotation.push_back(new AnimationGraphHandle);
  }
  // Destructor
  ~ObjectFrame() {
    for (int i=0; i<3; i++) {
      delete translation[i];
      delete rotation[i];
      delete scale[i];
    }
    delete rotation[3];
  }
  // Explicitly disable copy constructor
  ObjectFrame(const ObjectFrame& obj) = delete;

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

#endif  // SRC_MODEL_INCLUDE_OBJECT_FRAME_H_
