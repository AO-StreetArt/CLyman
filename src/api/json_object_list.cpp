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

#include "include/json_object_list.h"

// Constructor from a parsed JSON Document
// Not going to parse error code or message as we dont care about them coming
// in, only going out
JsonObjectList::JsonObjectList(const rapidjson::Document& d) {
  // Initialize Empty String elements
  set_msg_type(-1);
  set_error_code(100);
  set_num_records(10);
  // Start parsing the JSON Object
  if (d.IsObject()) {

    // Parse the base elements
    set_msg_type(find_json_int_elt_in_doc(d, "msg_type"));
    set_op_type(find_json_int_elt_in_doc(d, "operation"));
    set_num_records(find_json_int_elt_in_doc(d, "num_records"));
    std::string tid_value;
    find_json_string_elt_in_doc(d, "transaction_id", tid_value);
    set_transaction_id(tid_value);

    // Parse the object list
    if (d.HasMember("objects")) {
      const rapidjson::Value *objs_val = &d["objects"];
      if (!(objs_val->IsNull())) {
        if (objs_val->IsArray()) {
          for (auto& itr : objs_val->GetArray()) {
            // Create a new object
            ObjectInterface *new_obj = ofactory.build_object();

            // Basic elements
            std::string key_value;
            find_json_string_elt_in_array(itr, "key", key_value);
            new_obj->set_key(key_value);
            std::string parent_value;
            find_json_string_elt_in_array(itr, "parent", parent_value);
            new_obj->set_parent(parent_value);
            std::string asi_value;
            find_json_string_elt_in_array(itr, "asset_sub_id", asi_value);
            new_obj->set_asset_sub_id(asi_value);
            std::string scene_value;
            find_json_string_elt_in_array(itr, "scene", scene_value);
            new_obj->set_scene(scene_value);
            std::string name_value;
            find_json_string_elt_in_array(itr, "name", name_value);
            new_obj->set_name(name_value);
            std::string owner_value;
            find_json_string_elt_in_array(itr, "owner", owner_value);
            new_obj->set_owner(owner_value);
            std::string type_value;
            find_json_string_elt_in_array(itr, "type", type_value);
            new_obj->set_type(type_value);
            std::string subtype_value;
            find_json_string_elt_in_array(itr, "subtype", subtype_value);
            new_obj->set_subtype(subtype_value);
            new_obj->set_frame(find_json_int_elt_in_array(itr, "frame"));
            new_obj->set_timestamp(find_json_int_elt_in_array(itr, "timestamp"));

            // Assets
            auto assets_itr = itr.FindMember("assets");
            if (assets_itr != itr.MemberEnd()) {
              if (assets_itr->value.IsArray()) {
                for (auto& asset_itr : assets_itr->value.GetArray()) {
                  new_obj->add_asset(asset_itr.GetString());
                }
              }
            }

            // Properties Array
            auto props_itr = itr.FindMember("properties");
            if (props_itr != itr.MemberEnd()) {
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
                          if (val_itr->value.IsDouble()) {
                            new_prop->add_value(val_itr->value.GetDouble());
                          }
                        }
                        parse_json_graph_handle(prop_elt_itr, new_prop->get_handle(new_prop->num_values() - 1));
                      }
                    }
                  }

                  new_obj->add_prop(new_prop);
                }
              }
            }

            // Parse the Animation Graph Handles
            AnimationFrame *aframe = nullptr;
            // Animation Graph Handles - Translation
            auto th_itr = itr.FindMember("translation_handle");
            if (th_itr != itr.MemberEnd()) {
              if (th_itr->value.IsArray()) {
                if (!aframe) aframe = new AnimationFrame;
                int elt_indx = 0;
                for (auto& handle_elt_itr : th_itr->value.GetArray()) {
                  // Here we iterate over each object in the translation handle
                  parse_json_graph_handle(handle_elt_itr, aframe->get_translation(elt_indx));
                  elt_indx++;
                }
              }
            }

            auto rh_itr = itr.FindMember("rotation_handle");
            if (rh_itr != itr.MemberEnd()) {
              if (rh_itr->value.IsArray()) {
                if (!aframe) aframe = new AnimationFrame;
                int elt_indx = 0;
                for (auto& handle_elt_itr : rh_itr->value.GetArray()) {
                  // Here we iterate over each object in the translation handle
                  parse_json_graph_handle(handle_elt_itr, aframe->get_rotation(elt_indx));
                  elt_indx++;
                }
              }
            }

            // Animation Graph Handles - Scale
            auto sh_itr = itr.FindMember("scale_handle");
            if (sh_itr != itr.MemberEnd()) {
              if (sh_itr->value.IsArray()) {
                if (!aframe) aframe = new AnimationFrame;
                int elt_indx = 0;
                for (auto& handle_elt_itr : sh_itr->value.GetArray()) {
                  // Here we iterate over each object in the translation handle
                  parse_json_graph_handle(handle_elt_itr, aframe->get_scale(elt_indx));
                  elt_indx++;
                }
              }
            }
            new_obj->set_animation_frame(aframe);

            // Parse the transform elements
            Translation *trans = NULL;
            rapidjson::Value::ConstMemberIterator translation_iter = \
              itr.FindMember("translation");
            if (translation_iter != itr.MemberEnd()) {
              const rapidjson::Value& translation_val = translation_iter->value;
              if (!(translation_val.IsNull() || translation_val.Size() == 0)) {
                int i = 0;
                double x = 0.0;
                double y = 0.0;
                double z = 0.0;
                for (auto& trans_itr : translation_val.GetArray()) {
                  if (i == 0) {
                    x = trans_itr.GetDouble();
                  } else if (i == 1) {
                    y = trans_itr.GetDouble();
                  } else if (i == 2) {z = trans_itr.GetDouble();}
                  i++;
                }
                if ((x > 0.001 || x < -0.001) || \
                  (y > 0.001 || y < -0.001) || \
                  (z > 0.001 || z < -0.001)) {
                  trans = new Translation(x, y, z);
                }
              }
            }

            EulerRotation *erot = NULL;
            rapidjson::Value::ConstMemberIterator erot_iter = \
              itr.FindMember("euler_rotation");
            if (erot_iter != itr.MemberEnd()) {
              const rapidjson::Value& erot_val = erot_iter->value;
              if (!(erot_val.IsNull() || erot_val.Size() == 0)) {
                int i = 0;
                double x = 0.0;
                double y = 0.0;
                double z = 0.0;
                for (auto& erot_itr : erot_val.GetArray()) {
                  if (i == 0) {
                    x = erot_itr.GetDouble();
                  } else if (i == 1) {
                    y = erot_itr.GetDouble();
                  } else {z = erot_itr.GetDouble();}
                  i++;
                }
                if ((x > 0.001 || x < -0.001) || \
                  (y > 0.001 || y < -0.001) || \
                  (z > 0.001 || z < -0.001)) {
                  erot = new EulerRotation(x, y, z);
                }
              }
            }

            QuaternionRotation *qrot = NULL;
            rapidjson::Value::ConstMemberIterator qrot_iter = \
              itr.FindMember("quaternion_rotation");
            if (qrot_iter != itr.MemberEnd()) {
              const rapidjson::Value& qrot_val = qrot_iter->value;
              if (!(qrot_val.IsNull() || qrot_val.Size() == 0)) {
                int i = 0;
                double w = 0.0;
                double x = 0.0;
                double y = 0.0;
                double z = 0.0;
                for (auto& qrot_itr : qrot_val.GetArray()) {
                  if (i == 0) {
                    w = qrot_itr.GetDouble();
                  } else if (i == 1) {
                    x = qrot_itr.GetDouble();
                  } else if (i == 2) {
                    y = qrot_itr.GetDouble();
                  } else {z = qrot_itr.GetDouble();}
                  i++;
                }
                if ((w > 0.001 || w < -0.001) &&
                  ((x > 0.001 || x < -0.001) || \
                  (y > 0.001 || y < -0.001) || \
                  (z > 0.001 || z < -0.001))) {
                  qrot = new QuaternionRotation(w, x, y, z);
                }
              }
            }

            Scale *scl = NULL;
            rapidjson::Value::ConstMemberIterator scale_iter = \
              itr.FindMember("scale");
            if (scale_iter != itr.MemberEnd()) {
              const rapidjson::Value& scale_val = scale_iter->value;
              if (!(scale_val.IsNull() || scale_val.Size() == 0)) {
                int i = 0;
                double x = 0.0;
                double y = 0.0;
                double z = 0.0;
                for (auto& scale_itr : scale_val.GetArray()) {
                  if (i == 0) {
                    x = scale_itr.GetDouble();
                  } else if (i == 1) {
                    y = scale_itr.GetDouble();
                  } else if (i == 2) {z = scale_itr.GetDouble();}
                  i++;
                }
                if ((x > 1.001 || x < 0.999) || \
                  (y > 1.001 || y < 0.999) || \
                  (z > 1.001 || z < 0.999)) {
                  scl = new Scale(x, y, z);
                }
              }
            }
            if (scl) {
              new_obj->transform(scl);
              delete scl;
            }
            if (erot) {
              new_obj->transform(erot);
              delete erot;
            } else if (qrot) {
              new_obj->transform(qrot);
              delete qrot;
            }
            if (trans) {
              new_obj->transform(trans);
              delete trans;
            }
            add_object(new_obj);
          }
        }
      }
    // d.HasMember(objects)
    }

  // d.IsObject
  }
}

// write a JSON string from the object list
void JsonObjectList::to_msg_string(std::string &out_string) {

  // Initialize the string buffer and writer
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  // Start writing the object
  // Syntax taken directly from
  // simplewriter.cpp in rapidjson examples
  writer.StartObject();

  // Add integer elements
  writer.Key("msg_type");
  writer.Uint(get_msg_type());
  writer.Key("err_code");
  writer.Uint(get_error_code());
  writer.Key("num_records");
  writer.Uint(num_objects());

  // Add the error message
  if (!get_error_message().empty()) {
  writer.Key("err_msg");
  writer.String(get_error_message().c_str(), \
    (rapidjson::SizeType)get_error_message().length());
  }
  // Add the Transaction ID
  if (!(get_transaction_id().empty())) {
  writer.Key("transaction_id");
  writer.String(get_transaction_id().c_str(), \
      (rapidjson::SizeType)get_transaction_id().length());
  }

  // Write the object array
  writer.Key("objects");
  writer.StartArray();

  for (int a = 0; a < num_objects(); a++) {

    writer.StartObject();

    // Write string attributes
    write_json_string_elt(writer, "key", get_object(a)->get_key());
    write_json_string_elt(writer, "name", get_object(a)->get_name());
    write_json_string_elt(writer, "scene", get_object(a)->get_scene());
    write_json_string_elt(writer, "parent", get_object(a)->get_parent());
    write_json_string_elt(writer, "asset_sub_id", get_object(a)->get_asset_sub_id());
    write_json_string_elt(writer, "owner", get_object(a)->get_owner());
    write_json_string_elt(writer, "type", get_object(a)->get_type());
    write_json_string_elt(writer, "subtype", get_object(a)->get_subtype());
    write_json_int_elt(writer, "frame", get_object(a)->get_frame());
    write_json_int_elt(writer, "timestamp", get_object(a)->get_timestamp());

    // Write Properties array
    if (get_object(a)->num_props() > 0) {
      writer.Key("properties");
      writer.StartArray();
      for (int p = 0; p < get_object(a)->num_props(); p++) {
        AnimationProperty *prop = get_object(a)->get_prop(p);
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
        if (prop->num_values() > 0) {
          writer.Key("values");
          writer.StartArray();
          for (int i = 0; i < prop->num_values(); i++) {
            auto val = prop->get_value(i);
            AnimationGraphHandle* hnd = prop->get_handle(i);
            writer.StartObject();
            writer.Key("value");
            writer.Double(val);
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
    if (get_object(a)->get_animation_frame()) {
      // Translation
      writer.Key("translation_handle");
      writer.StartArray();
      for (int i = 0; i < 3; i++) {
        write_json_graph_handle(writer, get_object(a)->get_animation_frame()->get_translation(i));
      }
      writer.EndArray();

      // Rotation
      writer.Key("rotation_handle");
      writer.StartArray();
      for (int i = 0; i < 4; i++) {
        write_json_graph_handle(writer, get_object(a)->get_animation_frame()->get_rotation(i));
      }
      writer.EndArray();

      // Scale
      writer.Key("scale_handle");
      writer.StartArray();
      for (int i = 0; i < 3; i++) {
        write_json_graph_handle(writer, get_object(a)->get_animation_frame()->get_scale(i));
      }
      writer.EndArray();
    }

    // Write transforms
    writer.Key("transform");
    writer.StartArray();
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        writer.Double(get_object(a)->get_transform()->get_transform_element(i, \
          j));
      }
    }
    writer.EndArray();
    writer.Key("assets");
    writer.StartArray();
    for (int i = 0; i < get_object(a)->num_assets(); i++) {
      writer.String(get_object(a)->get_asset(i).c_str(), \
        (rapidjson::SizeType)get_object(a)->get_asset(i).length());
    }
    writer.EndArray();

    writer.EndObject();
  }

  writer.EndArray();

  writer.EndObject();

  // The Stringbuffer now contains a json message
  // of the object
  std::string return_string(s.GetString());
  out_string.assign(return_string);
}
