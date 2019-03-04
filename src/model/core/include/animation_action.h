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
#include <iterator>
#include <map>

#ifndef SRC_MODEL_INCLUDE_ANIMATION_ACTION_H_
#define SRC_MODEL_INCLUDE_ANIMATION_ACTION_H_

// An Animation Action is a set of keyframes which can be applied to a given
// set of objects to produce an animation or a piece of an animation
template <class T>
class AnimationAction {
  std::string name;
  std::string description;
  std::string owner;
  std::map<int, T*> keyframes;
 public:
  // Constructor
  AnimationAction() {}

  // Destructor
  ~AnimationAction() {
    for (auto itr = keyframes.begin(); itr != keyframes.end(); ++itr) {
      delete itr->second;
    }
  }

  // Explicitly disable copy constructor
  AnimationAction(const AnimationAction& obj) = delete;

  // Name/Description
  std::string get_name() {return name;}
  void set_name(std::string &new_name) {name.assign(new_name);}
  std::string get_description() {return description;}
  void set_description(std::string &new_desc) {description.assign(new_desc);}

  // Owner
  void set_owner(std::string new_owner) {
    owner.assign(new_owner);
  }

  std::string get_owner() {
    return owner;
  }

  // Add a keyframe to the action
  void add_keyframe(int index, T *keyframe) {
    keyframes.emplace(index, keyframe);
  }

  // Get a keyframe from the action
  T* get_keyframe(int index) {
    auto itr = keyframes.find(index);
    if (itr != keyframes.end()) {
      return itr->second;
    } else {
      return nullptr;
    }
  }

  // Remove a keyframe from the action
  void remove_keyframe(int index) {
    keyframes.erase(index);
  }


  // std::iterator<int, T*> keyframe_begin() {return keyframes.begin();}
  // std::iterator<int, T*> keyframe_end() {return keyframes.end();}
  std::map<int, T*>* get_keyframes() {return &keyframes;}
};

#endif  // SRC_MODEL_INCLUDE_ANIMATION_ACTION_H_
