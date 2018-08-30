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
#include "transforms.h"
#include "animation_frame_interface.h"

#ifndef SRC_MODEL_INCLUDE_DATA_FRAMEABLE_H_
#define SRC_MODEL_INCLUDE_DATA_FRAMEABLE_H_

// Any entity which can be keyframed
class FrameableData {
  int frame = -9999;
  int timestamp = -9999;
  AnimationFrameInterface *aframe = nullptr;
 public:
  virtual ~FrameableData() {if (aframe) delete aframe;}
  // Frame/Timestamp
  int get_frame() const {return frame;}
  int get_timestamp() const {return timestamp;}
  void set_frame(int new_frame) {frame = new_frame;}
  void set_timestamp(int new_timestamp) {timestamp = new_timestamp;}
  // Animation Frame
  AnimationFrameInterface* get_animation_frame() {return aframe;}
  void set_animation_frame(AnimationFrameInterface *new_aframe) {aframe = new_aframe;}
};

#endif  // SRC_MODEL_INCLUDE_DATA_FRAMEABLE_H_
