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

    if (d.HasMember("_id")) {
      const rapidjson::Value *id_val = &d["_id"]["$oid"];
      RelatedObject::set_key(id_val->GetString());
    }
    if (d.HasMember("key")) {
      const rapidjson::Value *key_val = &d["key"];
      RelatedObject::set_key(key_val->GetString());
    }
    if (d.HasMember("name")) {
      const rapidjson::Value *name_val = &d["name"];
      name = name_val->GetString();
    }
    if (d.HasMember("scene")) {
      const rapidjson::Value *scene_val = &d["scene"];
      RelatedObject::set_scene(scene_val->GetString());
    }
    if (d.HasMember("owner")) {
      const rapidjson::Value *owner_val = &d["owner"];
      owner = owner_val->GetString();
    }
    if (d.HasMember("type")) {
      const rapidjson::Value *type_val = &d["type"];
      type = type_val->GetString();
    }
    if (d.HasMember("subtype")) {
      const rapidjson::Value *subtype_val = &d["subtype"];
      subtype = subtype_val->GetString();
    }
    if (d.HasMember("frame")) {
      const rapidjson::Value *frame_val = &d["frame"];
      Object3d::set_frame(frame_val->GetInt());
    }

    // Transformations
    if (d.HasMember("transform")) {
      const rapidjson::Value& trans_val = d["transform"];
      if (trans_val.IsArray()) {
        // Update the transformation elements
        for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
            int index = (4 * i) + j;
            double tran_elt = trans_val[index].GetDouble();
            Object3d::get_transform()->set_transform_element(i, j, tran_elt);
          }
        }
      }
    }

    // Assets
    if (d.HasMember("assets")) {
      // Read the array values and stuff them into new_location
      const rapidjson::Value& sc = d["assets"];
      if (sc.IsArray()) {
        for (auto& asset_itr : sc.GetArray()) {
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

// Write the Object to JSON
std::string ObjectDocument::to_json() {
  return to_json(false);
}

// Write the Object to JSON
// is_query flag used to control whether we are writing a full document
// or a query string.  We can only query on specific fields, so we don't want
// to write out things like transform matrix if we are sending a query to mongo
std::string ObjectDocument::to_json(bool is_query) {
  // Initialize the string buffer and writer
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  // Start writing the object
  // Syntax taken directly from
  // simplewriter.cpp in rapidjson examples
  writer.StartObject();

  // Write string attributes

  if (!(name.empty())) {
    writer.Key("key");
    writer.String(RelatedObject::get_key().c_str(), \
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

  if (!(type.empty())) {
    writer.Key("type");
    writer.String(type.c_str(), (rapidjson::SizeType)type.length());
  }

  if (!(subtype.empty())) {
    writer.Key("subtype");
    writer.String(subtype.c_str(), (rapidjson::SizeType)subtype.length());
  }

  if (!(owner.empty())) {
    writer.Key("owner");
    writer.String(owner.c_str(), (rapidjson::SizeType)owner.length());
  }

  if (Object3d::get_frame() > -9999) {
    writer.Key("frame");
    writer.Uint(Object3d::get_frame());
  }

  // Write Transform
  if (Object3d::has_transform() && (!is_query)) {
    writer.Key("transform");
    writer.StartArray();
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        writer.Double(Object3d::get_transform()->get_transform_element(i, j));
      }
    }
    writer.EndArray();
  }

  if ((is_query && num_assets() > 0) || (!is_query)) {
    writer.Key("assets");
    writer.StartArray();
    for (int i = 0; i < RelatedObject::num_assets(); i++) {
      std::string ast = RelatedObject::get_asset(i);
      writer.String(ast.c_str(), (rapidjson::SizeType)ast.length());
    }
    writer.EndArray();
  }

  writer.EndObject();

  // The Stringbuffer now contains a json message
  // of the object
  json_cstr_val = s.GetString();
  json_str_val.assign(json_cstr_val);
  return json_str_val;
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

  if (Object3d::get_frame() > -9999.1) {
    writer.Key("frame");
    writer.Uint(Object3d::get_frame());
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

  writer.EndObject();

  // The Stringbuffer now contains a json message
  // of the object
  transform_cstr_val = s.GetString();
  // Build the expected format for an Object Change Stream
  transform_str_val.assign(RelatedObject::get_scene());
  transform_str_val.append("\n");
  transform_str_val.append(transform_cstr_val);
  return transform_str_val;
}
