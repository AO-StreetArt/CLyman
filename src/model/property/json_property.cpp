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

#include "json_property.h"

void JsonProperty::to_json_writer(rapidjson::Writer<rapidjson::StringBuffer>& writer, int mtype) {
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

  // Write values array
  if (Property3d::num_values() > 0) {
    writer.Key("values");
    writer.StartArray();
    for (unsigned int i = 0; i < Property3d::num_values(); i++) {
      writer.Double(Property3d::get_value(i));
    }
    writer.EndArray();
  }

  // Write animations array
  writer.Key("actions");
  writer.StartArray();
  for (auto action_itr = AnimationProperty::get_actions()->begin(); action_itr != AnimationProperty::get_actions()->end(); ++action_itr) {
    writer.StartObject();
    write_json_string_elt(writer, "name", action_itr->first);
    write_json_string_elt(writer, "description", action_itr->second->get_description());

    // Write keyframes
    writer.Key("keyframes");
    writer.StartArray();
    for (auto keyframe_itr = action_itr->second->get_keyframes()->begin(); keyframe_itr != action_itr->second->get_keyframes()->end(); ++keyframe_itr) {
      writer.StartObject();

      // Write frame index
      write_json_int_elt(writer, "frame", keyframe_itr->first);

      // Write values array
      if (keyframe_itr->second->num_values() > 0) {
        writer.Key("values");
        writer.StartArray();
        for (unsigned int i = 0; i < keyframe_itr->second->num_values(); i++) {
          writer.StartObject();
          writer.Key("value");
          writer.Double(keyframe_itr->second->get_value(i));
          if (i < keyframe_itr->second->num_handles()) {
            write_json_graph_handle(writer, keyframe_itr->second->get_handle(i));
          }
          writer.EndObject();
        }
        writer.EndArray();
      }

      writer.EndObject();
    }
    writer.EndArray();

    writer.EndObject();
  }
  writer.EndArray();

  writer.EndObject();
}

void JsonProperty::to_json(std::string& json_str) {
  to_json(json_str, PROP_UPD);
}

void JsonProperty::to_json(std::string& json_str, int mtype) {
  // Initialize the string buffer and writer
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  to_json_writer(writer, mtype);

  // The Stringbuffer now contains a json message
  // of the object
  json_str.assign(s.GetString());
}
