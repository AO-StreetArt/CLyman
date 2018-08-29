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

#ifndef SRC_MODEL_INCLUDE_OBJECT_FRAME_INTERFACE_H_
#define SRC_MODEL_INCLUDE_OBJECT_FRAME_INTERFACE_H_

// An Object Frame Interface defines the functions for the Object-Frame Data Model
// Represents a single frame document in Mongo
class ObjectFrameInterface {
 public:
  virtual ~ObjectFrameInterface() {}
  // Object Key
  // The OID of the object in Mongo
  virtual std::string get_key() const = 0;
  virtual void set_key(std::string new_key) = 0;
  // Parent Key
  // The OID of the original object in Mongo
  virtual std::string get_parent() const = 0;
  virtual void set_parent(std::string new_key) = 0;
  // Object Transform
  // The transform holds the objects position, rotation, and scaling in 3-space
  virtual void transform(Transformation *t) = 0;
  virtual bool has_transform() const = 0;
  virtual Transformation* get_transform() const = 0;
  // Frame/Timestamp
  virtual int get_frame() const = 0;
  virtual int get_timestamp() const = 0;
  virtual void set_frame(int new_frame) = 0;
  virtual void set_timestamp(int new_timestamp) = 0;
  // Animation Frame
  virtual AnimationFrameInterface* get_animation_frame() = 0;
  virtual void set_animation_frame(AnimationFrameInterface *new_aframe) = 0;
};

#endif  // SRC_MODEL_INCLUDE_OBJECT_FRAME_INTERFACE_H_
