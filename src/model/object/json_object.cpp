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

#include "include/json_object.h"

void JsonObject::to_json_writer(rapidjson::Writer<rapidjson::StringBuffer>& writer, int mtype) {
  // Start writing the object
  // Syntax taken directly from
  // simplewriter.cpp in rapidjson examples
  writer.StartObject();

  // Write string attributes
  write_json_int_elt(writer, "msg_type", mtype);
  write_json_string_elt(writer, "key", RelatedData::get_key());
  write_json_string_elt(writer, "name", ObjectDocument::get_name());
  write_json_string_elt(writer, "scene", RelatedData::get_scene());
  write_json_string_elt(writer, "type", ObjectDocument::get_type());
  write_json_string_elt(writer, "subtype", ObjectDocument::get_subtype());
  write_json_string_elt(writer, "owner", ObjectDocument::get_owner());

  // Write Transform
  if (Object3d::has_transform()) {
    writer.Key("transform");
    writer.StartArray();
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        writer.Double(Object3d::get_transform()->get_transform_element(i, j));
      }
    }
    writer.EndArray();
  }

  // Write Assets
  writer.Key("assets");
  writer.StartArray();
  for (int i = 0; i < RelatedData::num_assets(); i++) {
    writer.String(RelatedData::get_asset(i).c_str(), (rapidjson::SizeType)RelatedData::get_asset(i).length());
  }
  writer.EndArray();

  // Write actions
  writer.Key("actions");
  writer.StartArray();
  for (auto action_itr = ObjectDocument::get_actions()->begin(); action_itr != ObjectDocument::get_actions()->end(); ++action_itr) {
    writer.StartObject();
    write_json_string_elt(writer, "name", action_itr->first);
    write_json_string_elt(writer, "description", action_itr->second->get_description());
    write_json_string_elt(writer, "key", action_itr->second->get_key());

    // Write keyframes
    writer.Key("keyframes");
    writer.StartArray();
    for (auto keyframe_itr = action_itr->second->get_keyframes()->begin(); keyframe_itr != action_itr->second->get_keyframes()->end(); ++keyframe_itr) {
      writer.StartObject();

      // Write frame index
      write_json_int_elt(writer, "frame", keyframe_itr->first);

      // Write transform
      writer.Key("transform");
      writer.StartArray();
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
          writer.Double(keyframe_itr->second->get_transform()->get_transform_element(i, j));
        }
      }
      writer.EndArray();

      // Write translation graph handles
      writer.Key("translation_handle");
      writer.StartArray();
      for (int i = 0; i < 3; i++) {
        writer.StartObject();
        write_json_graph_handle(writer, keyframe_itr->second->get_translation(i));
        writer.EndObject();
      }
      writer.EndArray();

      // Write rotation graph handles
      writer.Key("rotation_handle");
      writer.StartArray();
      for (int i = 0; i < 4; i++) {
        writer.StartObject();
        write_json_graph_handle(writer, keyframe_itr->second->get_rotation(i));
        writer.EndObject();
      }
      writer.EndArray();

      // Write scale graph handles
      writer.Key("scale_handle");
      writer.StartArray();
      for (int i = 0; i < 3; i++) {
        writer.StartObject();
        write_json_graph_handle(writer, keyframe_itr->second->get_scale(i));
        writer.EndObject();
      }
      writer.EndArray();

      writer.EndObject();
    }
    writer.EndArray();

    writer.EndObject();
  }
  writer.EndArray();

  writer.EndObject();
}

std::string JsonObject::to_transform_json() {
  return to_transform_json(OBJ_UPD);
}

std::string JsonObject::to_transform_json(int mtype) {
  // Initialize the string buffer and writer
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  to_json_writer(writer, mtype);

  // The Stringbuffer now contains a json message
  // of the object
  transform_cstr_val = s.GetString();
  // Build the expected format for an Object Change Stream
  transform_str_val.assign(transform_cstr_val);
  return transform_str_val;
}
