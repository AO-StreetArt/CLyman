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

#ifndef SRC_MODEL_INCLUDE_ANIMATION_FRAME_H_
#define SRC_MODEL_INCLUDE_ANIMATION_FRAME_H_

// An Animation Frame Interface defines the functions for the Animation-Frame Data Model
// Represents a single animation frame document in Mongo.  Stores information
// from Animation Curves, namely graph handles associated to keyframes.  These
// are used by animators to tweak and refine animations.
class AnimationFrame {
  int frame = -9999;
  std::string owner;
 public:
   // Constructor
   AnimationFrame() {}

   // Explicitly disable copy constructor
   AnimationFrame(const AnimationFrame& obj) = delete;

   // Destructor
  ~AnimationFrame() {}

  // Owner
  void set_owner(std::string new_owner) {
    owner.assign(new_owner);
  }

  std::string get_owner() {
    return owner;
  }

  // Frame/Timestamp
  int get_frame() const {
    return frame;
  }

  void set_frame(int new_frame) {
    frame = new_frame;
  }
};

#endif  // SRC_MODEL_INCLUDE_ANIMATION_FRAME_H_
