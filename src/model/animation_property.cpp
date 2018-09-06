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

#include "animation_property.h"

AnimationProperty::AnimationProperty(const rapidjson::Document &d) {
  if (d.IsObject()) {
    // Basic elements
    std::string key_value;
    find_json_string_elt_in_doc(d, "key", key_value);
    set_key(key_value);
    std::string name_value;
    find_json_string_elt_in_doc(d, "name", name_value);
    set_name(name_value);
    std::string parent_value;
    find_json_string_elt_in_doc(d, "parent", parent_value);
    set_parent(parent_value);
    std::string scene_value;
    find_json_string_elt_in_doc(d, "scene", scene_value);
    set_scene(scene_value);
    std::string asi_value;
    find_json_string_elt_in_doc(d, "asset_sub_id", asi_value);
    set_asset_sub_id(asi_value);
    set_frame(find_json_int_elt_in_doc(d, "frame"));
    set_timestamp(find_json_int_elt_in_doc(d, "timestamp"));
    // Array of Property Values
    auto props_itr = d.FindMember("values");
    if (props_itr != d.MemberEnd()) {
      if (props_itr->value.IsArray()) {
        for (auto& prop_elt_itr : props_itr->value.GetArray()) {
          std::string value;
          auto val_itr = prop_elt_itr.FindMember("value");
          if (val_itr != prop_elt_itr.MemberEnd()) {
            if (val_itr->value.IsDouble()) {
              add_value(val_itr->value.GetDouble());
            }
          }
          parse_json_graph_handle(prop_elt_itr, handle[handle.size() - 1]);
        }
      }
    }
  }
}

void AnimationProperty::to_json(std::string& json_str) const {
  // Initialize the string buffer and writer
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  // Start writing the object
  // Syntax taken directly from
  // simplewriter.cpp in rapidjson examples
  writer.StartObject();

  // Write basic attributes
  write_json_int_elt(writer, "msg_type", PROP_UPD);
  write_json_string_elt(writer, "key", get_key());
  write_json_string_elt(writer, "name", get_name());
  write_json_string_elt(writer, "parent", get_parent());
  write_json_string_elt(writer, "asset_sub_id", get_asset_sub_id());
  write_json_string_elt(writer, "scene", get_scene());
  write_json_int_elt(writer, "frame", get_frame());
  write_json_int_elt(writer, "timestamp", get_timestamp());

  // Write values array
  if (values.size() > 0) {
    writer.Key("values");
    writer.StartArray();
    for (unsigned int i = 0; i < values.size(); i++) {
      writer.StartObject();
      writer.Key("value");
      writer.Double(values[i]);
      write_json_graph_handle(writer, handle[i]);
      writer.EndObject();
    }
    writer.EndArray();
  }
  writer.EndObject();

  // The Stringbuffer now contains a json message
  // of the object
  json_str.assign(s.GetString());
}
