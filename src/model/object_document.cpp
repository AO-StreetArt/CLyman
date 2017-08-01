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
  obj_logging->debug("Building Obj3 from JSON Document");
  // Message Transforms
  // Start parsing the JSON Object
  if (d.IsObject()) {
    obj_logging->debug("Object-Format Message Detected");

    if (d.HasMember("_id")) {
      const rapidjson::Value *key_val = &d["_id"];
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
        for (rapidjson::SizeType i = 0; i < sc.Size(); i++) {
          RelatedObject::add_asset(sc[i].GetString());
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
  // Apply transforms
  Object3d::transform(o.get_transform());
  // Move over asset ids
  for (int i = 0; i < o.num_assets(); i++) {
    RelatedObject::add_asset(o.get_asset(i));
  }
}

// Take the target and apply its fields as changes
void ObjectDocument::merge(ObjectInterface *target) {
  // Copy String values
  if (!(target->get_key().empty())) RelatedObject::set_key(target->get_key());
  if (!(target->get_name().empty())) name = target->get_name();
  if (!(target->get_scene().empty())) RelatedObject::set_scene(target->get_scene());
  if (!(target->get_type().empty())) type = target->get_type();
  if (!(target->get_subtype().empty())) subtype = target->get_subtype();
  if (!(target->get_owner().empty())) owner = target->get_owner();
  // Apply transforms
  if (target->has_transform()) {
    Object3d::transform(target->get_transform());
  }
  // Move over asset ids
  for (int i = 0; i < target->num_assets(); i++) {
    RelatedObject::add_asset(target->get_asset(i));
  }
}

// Write the Object to JSON
std::string ObjectDocument::to_json() {
  // Initialize the string buffer and writer
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  // Start writing the object
  // Syntax taken directly from
  // simplewriter.cpp in rapidjson examples
  writer.StartObject();

  // Write string attributes

  if (!(name.empty())) {
    writer.Key("name");
    writer.String(name.c_str(), (rapidjson::SizeType)name.length());
  }

  if (!(RelatedObject::get_scene().empty())) {
    writer.Key("scene");
    writer.String(RelatedObject::get_scene().c_str(), (rapidjson::SizeType)RelatedObject::get_scene().length());
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

  writer.Key("assets");
  writer.StartArray();
  for (int i = 0; i < RelatedObject::num_assets(); i++) {
    std::string ast = RelatedObject::get_asset(i);
    writer.String(ast.c_str(), (rapidjson::SizeType)ast.length());
  }
  writer.EndArray();

  writer.EndObject();

  // The Stringbuffer now contains a json message
  // of the object
  json_cstr_val = s.GetString();
  json_str_val.assign(json_cstr_val);
  return json_str_val;
}
