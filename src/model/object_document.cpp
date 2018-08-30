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

#include "include/object_document.h"

// Constructor to parse a JSON from Mongo
ObjectDocument::ObjectDocument(const rapidjson::Document &d) {
  // Message Transforms
  // Start parsing the JSON Object
  if (d.IsObject()) {
    // Basic elements
    std::string key_value;
    find_json_string_elt_in_doc(d, "key", key_value);
    RelatedData::set_key(key_value);
    std::string parent_value;
    find_json_string_elt_in_doc(d, "parent", parent_value);
    RelatedData::set_parent(parent_value);
    std::string asi_value;
    find_json_string_elt_in_doc(d, "asset_sub_id", asi_value);
    RelatedData::set_asset_sub_id(asi_value);
    std::string scene_value;
    find_json_string_elt_in_doc(d, "scene", scene_value);
    RelatedData::set_scene(scene_value);
    std::string name_value;
    find_json_string_elt_in_doc(d, "name", name_value);
    name = name_value;
    std::string owner_value;
    find_json_string_elt_in_doc(d, "owner", owner_value);
    owner = owner_value;
    std::string type_value;
    find_json_string_elt_in_doc(d, "type", type_value);
    type = type_value;
    std::string subtype_value;
    find_json_string_elt_in_doc(d, "subtype", subtype_value);
    subtype = subtype_value;
    Object3d::set_frame(find_json_int_elt_in_doc(d, "frame"));
    Object3d::set_timestamp(find_json_int_elt_in_doc(d, "timestamp"));

    // Transformation Array
    auto transform_itr = d.FindMember("transform");
    if (transform_itr != d.MemberEnd()) {
      if (transform_itr->value.IsArray()) {
        int index = 0;
        for (auto& elt_itr : transform_itr->value.GetArray()) {
          int i = index / 4;
          int j = index % 4;
          if (elt_itr.IsNumber()) {
            Object3d::get_transform()->set_transform_element(i, j, elt_itr.GetDouble());
          }
          index++;
        }
      }
    }

    // Properties Array
    auto props_itr = d.FindMember("properties");
    if (props_itr != d.MemberEnd()) {
      if (props_itr->value.IsArray()) {
        for (auto& itr : props_itr->value.GetArray()) {
          AnimationProperty *new_prop = new AnimationProperty;

          // Basic Property Values
          std::string key_val;
          find_json_string_elt_in_array(itr, "key", key_val);
          new_prop->set_key(key_val);
          std::string parent_val;
          find_json_string_elt_in_array(itr, "parent", parent_val);
          new_prop->set_parent(parent_val);
          std::string name_val;
          find_json_string_elt_in_array(itr, "name", name_val);
          new_prop->set_name(name_val);
          std::string scene_val;
          find_json_string_elt_in_array(itr, "scene", scene_val);
          new_prop->set_scene(scene_val);
          std::string asi_val;
          find_json_string_elt_in_array(itr, "asset_sub_id", asi_val);
          new_prop->set_asset_sub_id(asi_val);
          new_prop->set_frame(find_json_int_elt_in_array(itr, "frame"));
          new_prop->set_timestamp(find_json_int_elt_in_array(itr, "timestamp"));

          // Values Array
          auto props_itr = itr.FindMember("values");
          if (props_itr != itr.MemberEnd()) {
            if (props_itr->value.IsArray()) {
              for (auto& prop_elt_itr : props_itr->value.GetArray()) {
                // Add a new property value
                auto val_itr = prop_elt_itr.FindMember("value");
                if (val_itr != prop_elt_itr.MemberEnd()) {
                  if (val_itr->value.IsString()) {
                    new_prop->add_value(val_itr->value.GetString());
                  }
                }
                parse_json_graph_handle(prop_elt_itr, new_prop->get_handle(new_prop->num_values() - 1));
              }
            }
          }

          add_prop(new_prop);
        }
      }
    }

    // Animation Graph Handles - Translation
    auto th_itr = d.FindMember("translation_handle");
    if (th_itr != d.MemberEnd()) {
      if (th_itr->value.IsArray()) {
        if (!FrameableData::get_animation_frame()) FrameableData::set_animation_frame(new AnimationFrame);
        int elt_indx = 0;
        for (auto& handle_elt_itr : th_itr->value.GetArray()) {
          // Here we iterate over each object in the translation handle
          parse_json_graph_handle(handle_elt_itr, FrameableData::get_animation_frame()->get_translation(elt_indx));
          elt_indx++;
        }
      }
    }

    auto rh_itr = d.FindMember("rotation_handle");
    if (rh_itr != d.MemberEnd()) {
      if (rh_itr->value.IsArray()) {
        if (!FrameableData::get_animation_frame()) FrameableData::set_animation_frame(new AnimationFrame);
        int elt_indx = 0;
        for (auto& handle_elt_itr : rh_itr->value.GetArray()) {
          // Here we iterate over each object in the translation handle
          // there should be three, 0 for w, 1 for x, and 2 for y, and 3 for z
          parse_json_graph_handle(handle_elt_itr, FrameableData::get_animation_frame()->get_rotation(elt_indx));
          elt_indx++;
        }
      }
    }

    // Animation Graph Handles - Scale
    auto sh_itr = d.FindMember("scale_handle");
    if (sh_itr != d.MemberEnd()) {
      if (sh_itr->value.IsArray()) {
        if (!FrameableData::get_animation_frame()) FrameableData::set_animation_frame(new AnimationFrame);
        int elt_indx = 0;
        for (auto& handle_elt_itr : sh_itr->value.GetArray()) {
          // Here we iterate over each object in the translation handle
          // there should be three, 0 for x, 1 for y, and 2 for z
          parse_json_graph_handle(handle_elt_itr, FrameableData::get_animation_frame()->get_scale(elt_indx));
          elt_indx++;
        }
      }
    }

    // Assets
    auto assets_itr = d.FindMember("assets");
    if (assets_itr != d.MemberEnd()) {
      if (assets_itr->value.IsArray()) {
        for (auto& asset_itr : assets_itr->value.GetArray()) {
          RelatedData::add_asset(asset_itr.GetString());
        }
      }
    }
  }
}

// Copy Constructor
ObjectDocument::ObjectDocument(const ObjectDocument &o) {
  // Copy String values
  RelatedData::set_key(o.RelatedData::get_key());
  name = o.get_name();
  RelatedData::set_scene(o.RelatedData::get_scene());
  type = o.get_type();
  subtype = o.get_subtype();
  owner = o.get_owner();
  Object3d::set_frame(o.get_frame());
  Object3d::set_timestamp(o.get_timestamp());
  // Apply transforms
  Object3d::transform(o.Object3d::get_transform());
  // Move over asset ids
  for (int i = 0; i < o.RelatedData::num_assets(); i++) {
    RelatedData::add_asset(o.RelatedData::get_asset(i));
  }
}

// Copy Constructor
ObjectDocument::ObjectDocument(const ObjectInterface &o) {
  // Copy String values
  RelatedData::set_key(o.get_key());
  name = o.get_name();
  RelatedData::set_scene(o.get_scene());
  type = o.get_type();
  subtype = o.get_subtype();
  owner = o.get_owner();
  Object3d::set_frame(o.get_frame());
  Object3d::set_timestamp(o.get_timestamp());
  // Apply transforms
  Object3d::transform(o.get_transform());
  // Move over asset ids
  for (int i = 0; i < o.num_assets(); i++) {
    RelatedData::add_asset(o.get_asset(i));
  }
}

void ObjectDocument::write_string_attributes(ObjectInterface *target) {
  // Copy String values
  if (!(target->get_key().empty())) RelatedData::set_key(target->get_key());
  if (!(target->get_name().empty())) name = target->get_name();
  if (!(target->get_scene().empty())) \
    {RelatedData::set_scene(target->get_scene());}
  if (!(target->get_type().empty())) type = target->get_type();
  if (!(target->get_subtype().empty())) subtype = target->get_subtype();
  owner = target->get_owner();
  if (target->get_frame() > -9999.1) Object3d::set_frame(target->get_frame());
  if (target->get_timestamp() > -9999.1) Object3d::set_timestamp(target->get_timestamp());
}

// Take the target and apply its fields as changes
void ObjectDocument::merge(ObjectInterface *target) {
  // Copy String values
  write_string_attributes(target);
  // Apply transforms
  if (target->has_transform()) {
    Object3d::transform(target->get_transform());
  }
  // Move over asset ids
  for (int i = 0; i < target->num_assets(); i++) {
    RelatedData::add_asset(target->get_asset(i));
  }
}

// Take a target object and overwrite this object's fields with it
void ObjectDocument::overwrite(ObjectInterface *target) {
  // Copy String values
  write_string_attributes(target);
  // update transform
  if (target->has_transform()) {
    Object3d::set_transform(target->get_transform());
  }
  // update asset ids
  if (target->num_assets() > 0) {
    RelatedData::clear_assets();
    int i = 0;
    for (i = 0; i < target->num_assets(); i++) {
      RelatedData::add_asset(target->get_asset(i));
    }
  }
}

std::string ObjectDocument::to_transform_json() {
  // Initialize the string buffer and writer
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  // Start writing the object
  // Syntax taken directly from
  // simplewriter.cpp in rapidjson examples
  writer.StartObject();

  // Write string attributes
  write_json_string_elt(writer, "msg_type", std::string("object"));
  write_json_string_elt(writer, "key", RelatedData::get_key());
  write_json_string_elt(writer, "name", name);
  write_json_string_elt(writer, "scene", RelatedData::get_scene());
  write_json_int_elt(writer, "frame", get_frame());
  write_json_int_elt(writer, "timestamp", get_timestamp());

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

  // Write Properties array
  if (properties.size() > 0) {
    writer.Key("properties");
    writer.StartArray();
    for (AnimationProperty* prop : properties) {
      writer.StartObject();
      // Write basic attributes
      write_json_string_elt(writer, "key", prop->get_key());
      write_json_string_elt(writer, "name", prop->get_name());
      write_json_string_elt(writer, "parent", prop->get_parent());
      write_json_string_elt(writer, "asset_sub_id", prop->get_asset_sub_id());
      write_json_string_elt(writer, "scene", prop->get_scene());
      write_json_int_elt(writer, "frame", prop->get_frame());
      write_json_int_elt(writer, "timestamp", prop->get_timestamp());

      // Write values array
      if (properties.size() > 0) {
        writer.Key("values");
        writer.StartArray();
        for (int i = 0; i < prop->num_values(); i++) {
          auto val = prop->get_value(i);
          AnimationGraphHandle* hnd = prop->get_handle(i);
          writer.StartObject();
          writer.Key("value");
          writer.String(val.c_str(), (rapidjson::SizeType)val.length());
          write_json_graph_handle(writer, hnd);
          writer.EndObject();
        }
        writer.EndArray();
      }
      writer.EndObject();
    }
    writer.EndArray();
  }

  // Write Animation Graph Handles
  if (FrameableData::get_animation_frame()) {
    // Translation
    writer.Key("translation_handle");
    writer.StartArray();
    for (int i = 0; i < 3; i++) {
      write_json_graph_handle(writer, FrameableData::get_animation_frame()->get_translation(i));
    }
    writer.EndArray();

    // Rotation
    writer.Key("rotation_handle");
    writer.StartArray();
    for (int i = 0; i < 4; i++) {
      write_json_graph_handle(writer, FrameableData::get_animation_frame()->get_rotation(i));
    }
    writer.EndArray();

    // Scale
    writer.Key("scale_handle");
    writer.StartArray();
    for (int i = 0; i < 3; i++) {
      write_json_graph_handle(writer, FrameableData::get_animation_frame()->get_scale(i));
    }
    writer.EndArray();
  }

  writer.EndObject();

  // The Stringbuffer now contains a json message
  // of the object
  transform_cstr_val = s.GetString();
  // Build the expected format for an Object Change Stream
  transform_str_val.assign(transform_cstr_val);
  return transform_str_val;
}
