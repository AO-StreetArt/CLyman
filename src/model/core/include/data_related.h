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

#ifndef SRC_MODEL_INCLUDE_DATA_RELATED_H_
#define SRC_MODEL_INCLUDE_DATA_RELATED_H_

// A RelatedData is anything which is related to other Data Components
class RelatedData {
  // Local key for CLyman DB
  std::string key;
  // Parent key for Clyman DB
  std::string parent;
  // Identifier for the piece of an asset corresponding to this object.
  // This identifier lets us associate an object to a piece of an asset
  // from a parent object.
  std::string asset_sub_id;
  // Remote key for a Scene ID
  std::string scene_id;
  // Remote keys for assets (meshes, textures, etc)
  std::vector<std::string> asset_ids;

 public:
  // Constructors
  RelatedData() {}
  // Copy Constructor
  RelatedData(const RelatedData &o) {}
  // Destructor
  virtual ~RelatedData() {}
  // String Getters
  std::string get_key() const {return key;}
  std::string get_scene() const {return scene_id;}
  std::string get_parent() const {return parent;}
  std::string get_asset_sub_id() const {return asset_sub_id;}
  // String Setters
  void set_key(std::string new_key) {key.assign(new_key);}
  void set_scene(std::string new_scene) {scene_id.assign(new_scene);}
  void set_parent(std::string new_parent) {parent.assign(new_parent);}
  void set_asset_sub_id(std::string new_asset_sub_id) {asset_sub_id.assign(new_asset_sub_id);}
  // Asset methods
  int num_assets() const {return asset_ids.size();}
  void add_asset(std::string id) {asset_ids.push_back(id);}
  std::string get_asset(int index) const {return asset_ids[index];}
  void remove_asset(int index) {asset_ids.erase(asset_ids.begin()+index);}
  void clear_assets() {asset_ids.clear();}
};

#endif  // SRC_MODEL_INCLUDE_DATA_RELATED_H_
