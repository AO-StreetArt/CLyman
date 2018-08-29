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
#include "include/animation_frame_interface.h"
#include "include/animation_frame.h"
#include "include/animation_graph_handle.h"

// Constructor to parse a JSON from Mongo
ObjectDocument::ObjectDocument(const rapidjson::Document &d) {
  // Message Transforms
  // Start parsing the JSON Object
  if (d.IsObject()) {
    // Basic elements
    auto key_itr = d.FindMember("key");
    if (key_itr != d.MemberEnd()) {
      if (key_itr->value.IsString()) {
        RelatedObject::set_key(key_itr->value.GetString());
      }
    }
    auto parent_itr = d.FindMember("parent");
    if (parent_itr != d.MemberEnd()) {
      if (parent_itr->value.IsString()) {
        RelatedObject::set_parent(parent_itr->value.GetString());
      }
    }
    auto asi_itr = d.FindMember("asset_sub_id");
    if (asi_itr != d.MemberEnd()) {
      if (asi_itr->value.IsString()) {
        RelatedObject::set_asset_sub_id(asi_itr->value.GetString());
      }
    }
    auto name_itr = d.FindMember("name");
    if (name_itr != d.MemberEnd()) {
      if (name_itr->value.IsString()) {
        name = name_itr->value.GetString();
      }
    }
    auto scene_itr = d.FindMember("scene");
    if (scene_itr != d.MemberEnd()) {
      if (scene_itr->value.IsString()) {
        RelatedObject::set_scene(scene_itr->value.GetString());
      }
    }
    auto owner_itr = d.FindMember("owner");
    if (owner_itr != d.MemberEnd()) {
      if (owner_itr->value.IsString()) {
        owner = owner_itr->value.GetString();
      }
    }
    auto type_itr = d.FindMember("type");
    if (type_itr != d.MemberEnd()) {
      if (type_itr->value.IsString()) {
        type = type_itr->value.GetString();
      }
    }
    auto subtype_itr = d.FindMember("subtype");
    if (subtype_itr != d.MemberEnd()) {
      if (subtype_itr->value.IsString()) {
        subtype = subtype_itr->value.GetString();
      }
    }
    auto frame_itr = d.FindMember("frame");
    if (frame_itr != d.MemberEnd()) {
      if (frame_itr->value.IsNumber()) {
        Object3d::set_frame(frame_itr->value.GetInt());
      }
    }
    auto timestamp_itr = d.FindMember("timestamp");
    if (timestamp_itr != d.MemberEnd()) {
      if (timestamp_itr->value.IsNumber()) {
        Object3d::set_timestamp(timestamp_itr->value.GetInt());
      }
    }

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

    // Animation Graph Handles - Translation
    auto th_itr = d.FindMember("translation_handle");
    if (th_itr != d.MemberEnd()) {
      if (th_itr->value.IsArray()) {
        if (!FrameableData::get_animation_frame()) FrameableData::get_animation_frame() = new AnimationFrame;
        int elt_indx = 0;
        for (auto& handle_elt_itr : th_itr->value.GetArray()) {
          // Here we iterate over each object in the translation handle
          // there should be three, 0 for x, 1 for y, and 2 for z

          // Left Handle

          // Get the type of handle
          auto lhtype_itr = handle_elt_itr.FindMember("left_type");
          if (lhtype_itr != handle_elt_itr.MemberEnd()) {
            if (lhtype_itr->value.IsString()) {
              FrameableData::get_animation_frame()->get_translation(elt_indx)->set_lh_type(lhtype_itr->value.GetString());
            }
          }
          auto lhx_itr = handle_elt_itr.FindMember("left_x");
          if (lhx_itr != handle_elt_itr.MemberEnd()) {
            if (lhx_itr->value.IsNumber()) {
              FrameableData::get_animation_frame()->get_translation(elt_indx)->set_lh_x(lhx_itr->value.GetDouble());
            }
          }
          auto lhy_itr = handle_elt_itr.FindMember("left_y");
          if (lhy_itr != handle_elt_itr.MemberEnd()) {
            if (lhy_itr->value.IsNumber()) {
              FrameableData::get_animation_frame()->get_translation(elt_indx)->set_lh_y(lhy_itr->value.GetDouble());
            }
          }

          // Right Handle

          // Get the type of handle
          auto rhtype_itr = handle_elt_itr.FindMember("right_type");
          if (rhtype_itr != handle_elt_itr.MemberEnd()) {
            if (rhtype_itr->value.IsString()) {
              FrameableData::get_animation_frame()->get_translation(elt_indx)->set_rh_type(rhtype_itr->value.GetString());
            }
          }
          auto rhx_itr = handle_elt_itr.FindMember("right_x");
          if (rhx_itr != handle_elt_itr.MemberEnd()) {
            if (rhx_itr->value.IsNumber()) {
              FrameableData::get_animation_frame()->get_translation(elt_indx)->set_rh_x(rhx_itr->value.GetDouble());
            }
          }
          auto rhy_itr = handle_elt_itr.FindMember("right_y");
          if (rhy_itr != handle_elt_itr.MemberEnd()) {
            if (rhy_itr->value.IsNumber()) {
              FrameableData::get_animation_frame()->get_translation(elt_indx)->set_rh_y(rhy_itr->value.GetDouble());
            }
          }

          elt_indx++;
        }
      }
    }

    auto rh_itr = d.FindMember("rotation_handle");
    if (rh_itr != d.MemberEnd()) {
      if (rh_itr->value.IsArray()) {
        if (!FrameableData::get_animation_frame()) FrameableData::get_animation_frame() = new AnimationFrame;
        int elt_indx = 0;
        for (auto& handle_elt_itr : rh_itr->value.GetArray()) {
          // Here we iterate over each object in the translation handle
          // there should be three, 0 for w, 1 for x, and 2 for y, and 3 for z

          // Left Handle

          // Get the type of handle
          auto lhtype_itr = handle_elt_itr.FindMember("left_type");
          if (lhtype_itr != handle_elt_itr.MemberEnd()) {
            if (lhtype_itr->value.IsString()) {
              FrameableData::get_animation_frame()->get_rotation(elt_indx)->set_lh_type(lhtype_itr->value.GetString());
            }
          }
          auto lhx_itr = handle_elt_itr.FindMember("left_x");
          if (lhx_itr != handle_elt_itr.MemberEnd()) {
            if (lhx_itr->value.IsNumber()) {
              FrameableData::get_animation_frame()->get_rotation(elt_indx)->set_lh_x(lhx_itr->value.GetDouble());
            }
          }
          auto lhy_itr = handle_elt_itr.FindMember("left_y");
          if (lhy_itr != handle_elt_itr.MemberEnd()) {
            if (lhy_itr->value.IsNumber()) {
              FrameableData::get_animation_frame()->get_rotation(elt_indx)->set_lh_y(lhy_itr->value.GetDouble());
            }
          }

          // Right Handle

          // Get the type of handle
          auto rhtype_itr = handle_elt_itr.FindMember("right_type");
          if (rhtype_itr != handle_elt_itr.MemberEnd()) {
            if (rhtype_itr->value.IsString()) {
              FrameableData::get_animation_frame()->get_rotation(elt_indx)->set_rh_type(rhtype_itr->value.GetString());
            }
          }
          auto rhx_itr = handle_elt_itr.FindMember("right_x");
          if (rhx_itr != handle_elt_itr.MemberEnd()) {
            if (rhx_itr->value.IsNumber()) {
              FrameableData::get_animation_frame()->get_rotation(elt_indx)->set_rh_x(rhx_itr->value.GetDouble());
            }
          }
          auto rhy_itr = handle_elt_itr.FindMember("right_y");
          if (rhy_itr != handle_elt_itr.MemberEnd()) {
            if (rhy_itr->value.IsNumber()) {
              FrameableData::get_animation_frame()->get_rotation(elt_indx)->set_rh_y(rhy_itr->value.GetDouble());
            }
          }

          elt_indx++;
        }
      }
    }

    // Animation Graph Handles - Scale
    auto sh_itr = d.FindMember("scale_handle");
    if (sh_itr != d.MemberEnd()) {
      if (sh_itr->value.IsArray()) {
        if (!FrameableData::get_animation_frame()) FrameableData::get_animation_frame() = new AnimationFrame;
        int elt_indx = 0;
        for (auto& handle_elt_itr : sh_itr->value.GetArray()) {
          // Here we iterate over each object in the translation handle
          // there should be three, 0 for x, 1 for y, and 2 for z

          // Left Handle

          // Get the type of handle
          auto lhtype_itr = handle_elt_itr.FindMember("left_type");
          if (lhtype_itr != handle_elt_itr.MemberEnd()) {
            if (lhtype_itr->value.IsString()) {
              FrameableData::get_animation_frame()->get_scale(elt_indx)->set_lh_type(lhtype_itr->value.GetString());
            }
          }
          auto lhx_itr = handle_elt_itr.FindMember("left_x");
          if (lhx_itr != handle_elt_itr.MemberEnd()) {
            if (lhx_itr->value.IsNumber()) {
              FrameableData::get_animation_frame()->get_scale(elt_indx)->set_lh_x(lhx_itr->value.GetDouble());
            }
          }
          auto lhy_itr = handle_elt_itr.FindMember("left_y");
          if (lhy_itr != handle_elt_itr.MemberEnd()) {
            if (lhy_itr->value.IsNumber()) {
              FrameableData::get_animation_frame()->get_scale(elt_indx)->set_lh_y(lhy_itr->value.GetDouble());
            }
          }

          // Right Handle

          // Get the type of handle
          auto rhtype_itr = handle_elt_itr.FindMember("right_type");
          if (rhtype_itr != handle_elt_itr.MemberEnd()) {
            if (rhtype_itr->value.IsString()) {
              FrameableData::get_animation_frame()->get_scale(elt_indx)->set_rh_type(rhtype_itr->value.GetString());
            }
          }
          auto rhx_itr = handle_elt_itr.FindMember("right_x");
          if (rhx_itr != handle_elt_itr.MemberEnd()) {
            if (rhx_itr->value.IsNumber()) {
              FrameableData::get_animation_frame()->get_scale(elt_indx)->set_rh_x(rhx_itr->value.GetDouble());
            }
          }
          auto rhy_itr = handle_elt_itr.FindMember("right_y");
          if (rhy_itr != handle_elt_itr.MemberEnd()) {
            if (rhy_itr->value.IsNumber()) {
              FrameableData::get_animation_frame()->get_scale(elt_indx)->set_rh_y(rhy_itr->value.GetDouble());
            }
          }

          elt_indx++;
        }
      }
    }

    // Assets
    auto assets_itr = d.FindMember("assets");
    if (assets_itr != d.MemberEnd()) {
      if (assets_itr->value.IsArray()) {
        for (auto& asset_itr : assets_itr->value.GetArray()) {
          RelatedObject::add_asset(asset_itr.GetString());
        }
      }
    }
  }
}

// Copy Constructor
ObjectDocument::ObjectDocument(const ObjectDocument &o) {
  // Copy String values
  RelatedObject::set_key(o.RelatedObject::get_key());
  name = o.get_name();
  RelatedObject::set_scene(o.RelatedObject::get_scene());
  type = o.get_type();
  subtype = o.get_subtype();
  owner = o.get_owner();
  Object3d::set_frame(o.get_frame());
  Object3d::set_timestamp(o.get_timestamp());
  // Apply transforms
  Object3d::transform(o.Object3d::get_transform());
  // Move over asset ids
  for (int i = 0; i < o.RelatedObject::num_assets(); i++) {
    RelatedObject::add_asset(o.RelatedObject::get_asset(i));
  }
}

// Copy Constructor
ObjectDocument::ObjectDocument(const ObjectInterface &o) {
  // Copy String values
  RelatedObject::set_key(o.get_key());
  name = o.get_name();
  RelatedObject::set_scene(o.get_scene());
  type = o.get_type();
  subtype = o.get_subtype();
  owner = o.get_owner();
  Object3d::set_frame(o.get_frame());
  Object3d::set_timestamp(o.get_timestamp());
  // Apply transforms
  Object3d::transform(o.get_transform());
  // Move over asset ids
  for (int i = 0; i < o.num_assets(); i++) {
    RelatedObject::add_asset(o.get_asset(i));
  }
}

void ObjectDocument::write_string_attributes(ObjectInterface *target) {
  // Copy String values
  if (!(target->get_key().empty())) RelatedObject::set_key(target->get_key());
  if (!(target->get_name().empty())) name = target->get_name();
  if (!(target->get_scene().empty())) \
    {RelatedObject::set_scene(target->get_scene());}
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
    RelatedObject::add_asset(target->get_asset(i));
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
    RelatedObject::clear_assets();
    int i = 0;
    for (i = 0; i < target->num_assets(); i++) {
      RelatedObject::add_asset(target->get_asset(i));
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

  if (!(RelatedObject::get_key().empty())) {
    writer.Key("key");
    writer.String(RelatedObject::get_key().c_str(),
      (rapidjson::SizeType)RelatedObject::get_key().length());
  }

  if (!(name.empty())) {
    writer.Key("name");
    writer.String(name.c_str(), (rapidjson::SizeType)name.length());
  }

  if (!(RelatedObject::get_scene().empty())) {
    writer.Key("scene");
    writer.String(RelatedObject::get_scene().c_str(), \
      (rapidjson::SizeType)RelatedObject::get_scene().length());
  }

  if (Object3d::get_frame() > -1) {
    writer.Key("frame");
    writer.Uint(Object3d::get_frame());
  }

  if (Object3d::get_timestamp() > 0) {
    writer.Key("timestamp");
    writer.Uint(Object3d::get_timestamp());
  }

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

  // Write Animation Graph Handles
  if (FrameableData::get_animation_frame()) {
    // Translation
    writer.Key("translation_handle");
    writer.StartArray();
    for (int i = 0; i < 3; i++) {
      writer.StartObject();
      writer.Key("left_type");
      writer.String(FrameableData::get_animation_frame()->get_translation(i)->get_lh_type().c_str(), \
          (rapidjson::SizeType)(FrameableData::get_animation_frame()->get_translation(i)->get_lh_type().length()));
      writer.Key("left_x");
      writer.Double(FrameableData::get_animation_frame()->get_translation(i)->get_lh_x());
      writer.Key("left_y");
      writer.Double(FrameableData::get_animation_frame()->get_translation(i)->get_lh_y());
      writer.Key("right_type");
      writer.String(FrameableData::get_animation_frame()->get_translation(i)->get_rh_type().c_str(), \
          (rapidjson::SizeType)(FrameableData::get_animation_frame()->get_translation(i)->get_rh_type().length()));
      writer.Key("right_x");
      writer.Double(FrameableData::get_animation_frame()->get_translation(i)->get_rh_x());
      writer.Key("right_y");
      writer.Double(FrameableData::get_animation_frame()->get_translation(i)->get_rh_y());
      writer.EndObject();
    }
    writer.EndArray();

    // Rotation
    writer.Key("rotation_handle");
    writer.StartArray();
    for (int i = 0; i < 4; i++) {
      writer.StartObject();
      writer.Key("left_type");
      writer.String(FrameableData::get_animation_frame()->get_rotation(i)->get_lh_type().c_str(), \
          (rapidjson::SizeType)(FrameableData::get_animation_frame()->get_rotation(i)->get_lh_type().length()));
      writer.Key("left_x");
      writer.Double(FrameableData::get_animation_frame()->get_rotation(i)->get_lh_x());
      writer.Key("left_y");
      writer.Double(FrameableData::get_animation_frame()->get_rotation(i)->get_lh_y());
      writer.Key("right_type");
      writer.String(FrameableData::get_animation_frame()->get_rotation(i)->get_rh_type().c_str(), \
          (rapidjson::SizeType)(FrameableData::get_animation_frame()->get_rotation(i)->get_rh_type().length()));
      writer.Key("right_x");
      writer.Double(FrameableData::get_animation_frame()->get_rotation(i)->get_rh_x());
      writer.Key("right_y");
      writer.Double(FrameableData::get_animation_frame()->get_rotation(i)->get_rh_y());
      writer.EndObject();
    }
    writer.EndArray();

    // Scale
    writer.Key("scale_handle");
    writer.StartArray();
    for (int i = 0; i < 3; i++) {
      writer.StartObject();
      writer.Key("left_type");
      writer.String(FrameableData::get_animation_frame()->get_scale(i)->get_lh_type().c_str(), \
          (rapidjson::SizeType)(FrameableData::get_animation_frame()->get_scale(i)->get_lh_type().length()));
      writer.Key("left_x");
      writer.Double(FrameableData::get_animation_frame()->get_scale(i)->get_lh_x());
      writer.Key("left_y");
      writer.Double(FrameableData::get_animation_frame()->get_scale(i)->get_lh_y());
      writer.Key("right_type");
      writer.String(FrameableData::get_animation_frame()->get_scale(i)->get_rh_type().c_str(), \
          (rapidjson::SizeType)(FrameableData::get_animation_frame()->get_scale(i)->get_rh_type().length()));
      writer.Key("right_x");
      writer.Double(FrameableData::get_animation_frame()->get_scale(i)->get_rh_x());
      writer.Key("right_y");
      writer.Double(FrameableData::get_animation_frame()->get_scale(i)->get_rh_y());
      writer.EndObject();
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
