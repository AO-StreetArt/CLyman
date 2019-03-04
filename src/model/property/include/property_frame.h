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

#include "property_3d.h"
#include "animation_frame.h"

#ifndef SRC_MODEL_INCLUDE_PROPERTY_FRAME_H_
#define SRC_MODEL_INCLUDE_PROPERTY_FRAME_H_

// A Property Frame is an animation frame with an array of property values
class PropertyFrame : public AnimationFrame, public Property3d {
  std::vector<AnimationGraphHandle*> handles;
 public:
  // Constructors
  PropertyFrame() : AnimationFrame(), Property3d() {}
  // Destructor
  ~PropertyFrame() {
    for (unsigned int i=0; i<handles.size(); i++) {
      delete handles[i];
    }
  }
  // Explicitly disable copy constructor
  PropertyFrame(const PropertyFrame& obj) = delete;

  // Access and set the Translation Graph Handles
  std::vector<AnimationGraphHandle*> get_handles() {return handles;}
  AnimationGraphHandle* get_handle(int index) {return handles[index];}
  unsigned int num_handles() {return handles.size();}
  void add_handle(AnimationGraphHandle *new_handle) {handles.push_back(new_handle);}
};

#endif  // SRC_MODEL_INCLUDE_PROPERTY_FRAME_H_
