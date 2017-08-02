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
#include "object_3d.h"
#include "app_log.h"

#ifndef SRC_MODEL_INCLUDE_OBJECT_RELATED_H_
#define SRC_MODEL_INCLUDE_OBJECT_RELATED_H_

// A RelatedObject is an object in 3-space which is related to the other
// attributes needed to actually create such an object (scene, assets, etc)
class RelatedObject : public Object3d {
  // Local key for CLyman DB
  std::string key;
  // Remote key for a Scene ID
  std::string scene_id;
  // Remote keys for assets (meshes, textures, etc)
  std::vector<std::string> asset_ids;

 public:
  // Constructors
  RelatedObject() : Object3d() {key.assign(""); scene_id.assign("");}
  // Copy Constructor
  RelatedObject(const RelatedObject &o) : Object3d(o) {}
  // Destructor
  virtual ~RelatedObject() {}
  // String Getters
  std::string get_key() const {return key;}
  std::string get_scene() const {return scene_id;}
  // String Setters
  void set_key(std::string new_key) {key.assign(new_key);}
  void set_scene(std::string new_scene) {scene_id.assign(new_scene);}
  // Asset methods
  int num_assets() const {return asset_ids.size();}
  void add_asset(std::string id) {asset_ids.push_back(id);}
  std::string get_asset(int index) const {return asset_ids[index];}
  void remove_asset(int index) {asset_ids.erase(asset_ids.begin()+index);}
  void clear_assets() {asset_ids.clear();}
};

#endif  // SRC_MODEL_INCLUDE_OBJECT_RELATED_H_
