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
    if (d.HasMember("msg_type")) {
      const rapidjson::Value *mtype_val = &d["msg_type"];
      if (!(mtype_val->IsNull())) {
        set_msg_type(mtype_val->GetInt());
      }
    }
    if (d.HasMember("operation")) {
      const rapidjson::Value *opid_val = &d["operation"];
      if (!(opid_val->IsNull())) {
        set_op_type(opid_val->GetInt());
      }
    }
    if (d.HasMember("transaction_id")) {
      const rapidjson::Value *tid_val = &d["transaction_id"];
      if (!(tid_val->IsNull())) {
        set_transaction_id(tid_val->GetString());
      }
    }
    if (d.HasMember("num_records")) {
      const rapidjson::Value *nr_val = &d["num_records"];
      if (!(nr_val->IsNull())) {
        set_num_records(nr_val->GetInt());
      }
    }

    // Parse the object list
    if (d.HasMember("objects")) {
      const rapidjson::Value *objs_val = &d["objects"];
      if (!(objs_val->IsNull())) {
        if (objs_val->IsArray()) {
          for (auto& itr : objs_val->GetArray()) {
            // Create a new object
            ObjectInterface *new_obj = ofactory.build_object();

            // Parse the string attributes
            auto key_iter = itr.FindMember("key");
            if (key_iter != itr.MemberEnd()) {
              if (key_iter->value.IsString()) {
                new_obj->set_key(key_iter->value.GetString());
              }
            }
            auto parent_itr = itr.FindMember("parent");
            if (parent_itr != itr.MemberEnd()) {
              if (parent_itr->value.IsString()) {
                RelatedObject::set_parent(parent_itr->value.GetString());
              }
            }
            auto asi_itr = itr.FindMember("asset_sub_id");
            if (asi_itr != itr.MemberEnd()) {
              if (asi_itr->value.IsString()) {
                RelatedObject::set_asset_sub_id(asi_itr->value.GetString());
              }
            }
            auto name_itr = itr.FindMember("name");
            if (name_itr != itr.MemberEnd()) {
              if (name_itr->value.IsString()) {
                name = name_itr->value.GetString();
              }
            }
            auto scene_itr = itr.FindMember("scene");
            if (scene_itr != itr.MemberEnd()) {
              if (scene_itr->value.IsString()) {
                RelatedObject::set_scene(scene_itr->value.GetString());
              }
            }
            auto owner_itr = itr.FindMember("owner");
            if (owner_itr != itr.MemberEnd()) {
              if (owner_itr->value.IsString()) {
                owner = owner_itr->value.GetString();
              }
            }
            auto type_itr = itr.FindMember("type");
            if (type_itr != itr.MemberEnd()) {
              if (type_itr->value.IsString()) {
                type = type_itr->value.GetString();
              }
            }
            auto subtype_itr = itr.FindMember("subtype");
            if (subtype_itr != itr.MemberEnd()) {
              if (subtype_itr->value.IsString()) {
                subtype = subtype_itr->value.GetString();
              }
            }
            auto frame_itr = itr.FindMember("frame");
            if (frame_itr != itr.MemberEnd()) {
              if (frame_itr->value.IsNumber()) {
                Object3d::set_frame(frame_itr->value.GetInt());
              }
            }
            auto timestamp_itr = itr.FindMember("timestamp");
            if (timestamp_itr != itr.MemberEnd()) {
              if (timestamp_itr->value.IsNumber()) {
                Object3d::set_timestamp(timestamp_itr->value.GetInt());
              }
            }

            // Parse the Animation Graph Handles
            AnimationFrame aframe = nullptr;
            // Animation Graph Handles - Translation
            auto th_itr = itr.FindMember("translation_handle");
            if (th_itr != itr.MemberEnd()) {
              if (th_itr->value.IsArray()) {
                if (!aframe) aframe = new AnimationFrame;
                int elt_indx = 0;
                for (auto& handle_elt_itr : th_itr->value.GetArray()) {
                  // Here we iterate over each object in the translation handle
                  // there should be three, 0 for x, 1 for y, and 2 for z

                  // Left Handle

                  // Get the type of handle
                  auto lhtype_itr = handle_elt_itr.FindMember("left_type");
                  if (lhtype_itr != handle_elt_itr.MemberEnd()) {
                    if (lhtype_itr->value.IsString()) {
                      aframe->get_translation(elt_indx)->set_lh_type(lhtype_itr->value.GetString());
                    }
                  }
                  auto lhx_itr = handle_elt_itr.FindMember("left_x");
                  if (lhx_itr != handle_elt_itr.MemberEnd()) {
                    if (lhx_itr->value.IsNumber()) {
                      aframe->get_translation(elt_indx)->set_lh_x(lhx_itr->value.GetDouble());
                    }
                  }
                  auto lhy_itr = handle_elt_itr.FindMember("left_y");
                  if (lhy_itr != handle_elt_itr.MemberEnd()) {
                    if (lhy_itr->value.IsNumber()) {
                      aframe->get_translation(elt_indx)->set_lh_y(lhy_itr->value.GetDouble());
                    }
                  }

                  // Right Handle

                  // Get the type of handle
                  auto rhtype_itr = handle_elt_itr.FindMember("right_type");
                  if (rhtype_itr != handle_elt_itr.MemberEnd()) {
                    if (rhtype_itr->value.IsString()) {
                      aframe->get_translation(elt_indx)->set_rh_type(rhtype_itr->value.GetString());
                    }
                  }
                  auto rhx_itr = handle_elt_itr.FindMember("right_x");
                  if (rhx_itr != handle_elt_itr.MemberEnd()) {
                    if (rhx_itr->value.IsNumber()) {
                      aframe->get_translation(elt_indx)->set_rh_x(rhx_itr->value.GetDouble());
                    }
                  }
                  auto rhy_itr = handle_elt_itr.FindMember("right_y");
                  if (rhy_itr != handle_elt_itr.MemberEnd()) {
                    if (rhy_itr->value.IsNumber()) {
                      aframe->get_translation(elt_indx)->set_rh_y(rhy_itr->value.GetDouble());
                    }
                  }

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
                  // there should be three, 0 for w, 1 for x, and 2 for y, and 3 for z

                  // Left Handle

                  // Get the type of handle
                  auto lhtype_itr = handle_elt_itr.FindMember("left_type");
                  if (lhtype_itr != handle_elt_itr.MemberEnd()) {
                    if (lhtype_itr->value.IsString()) {
                      aframe->get_rotation(elt_indx)->set_lh_type(lhtype_itr->value.GetString());
                    }
                  }
                  auto lhx_itr = handle_elt_itr.FindMember("left_x");
                  if (lhx_itr != handle_elt_itr.MemberEnd()) {
                    if (lhx_itr->value.IsNumber()) {
                      aframe->get_rotation(elt_indx)->set_lh_x(lhx_itr->value.GetDouble());
                    }
                  }
                  auto lhy_itr = handle_elt_itr.FindMember("left_y");
                  if (lhy_itr != handle_elt_itr.MemberEnd()) {
                    if (lhy_itr->value.IsNumber()) {
                      aframe->get_rotation(elt_indx)->set_lh_y(lhy_itr->value.GetDouble());
                    }
                  }

                  // Right Handle

                  // Get the type of handle
                  auto rhtype_itr = handle_elt_itr.FindMember("right_type");
                  if (rhtype_itr != handle_elt_itr.MemberEnd()) {
                    if (rhtype_itr->value.IsString()) {
                      aframe->get_rotation(elt_indx)->set_rh_type(rhtype_itr->value.GetString());
                    }
                  }
                  auto rhx_itr = handle_elt_itr.FindMember("right_x");
                  if (rhx_itr != handle_elt_itr.MemberEnd()) {
                    if (rhx_itr->value.IsNumber()) {
                      aframe->get_rotation(elt_indx)->set_rh_x(rhx_itr->value.GetDouble());
                    }
                  }
                  auto rhy_itr = handle_elt_itr.FindMember("right_y");
                  if (rhy_itr != handle_elt_itr.MemberEnd()) {
                    if (rhy_itr->value.IsNumber()) {
                      aframe->get_rotation(elt_indx)->set_rh_y(rhy_itr->value.GetDouble());
                    }
                  }

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
                  // there should be three, 0 for x, 1 for y, and 2 for z

                  // Left Handle

                  // Get the type of handle
                  auto lhtype_itr = handle_elt_itr.FindMember("left_type");
                  if (lhtype_itr != handle_elt_itr.MemberEnd()) {
                    if (lhtype_itr->value.IsString()) {
                      aframe->get_scale(elt_indx)->set_lh_type(lhtype_itr->value.GetString());
                    }
                  }
                  auto lhx_itr = handle_elt_itr.FindMember("left_x");
                  if (lhx_itr != handle_elt_itr.MemberEnd()) {
                    if (lhx_itr->value.IsNumber()) {
                      aframe->get_scale(elt_indx)->set_lh_x(lhx_itr->value.GetDouble());
                    }
                  }
                  auto lhy_itr = handle_elt_itr.FindMember("left_y");
                  if (lhy_itr != handle_elt_itr.MemberEnd()) {
                    if (lhy_itr->value.IsNumber()) {
                      aframe->get_scale(elt_indx)->set_lh_y(lhy_itr->value.GetDouble());
                    }
                  }

                  // Right Handle

                  // Get the type of handle
                  auto rhtype_itr = handle_elt_itr.FindMember("right_type");
                  if (rhtype_itr != handle_elt_itr.MemberEnd()) {
                    if (rhtype_itr->value.IsString()) {
                      aframe->get_scale(elt_indx)->set_rh_type(rhtype_itr->value.GetString());
                    }
                  }
                  auto rhx_itr = handle_elt_itr.FindMember("right_x");
                  if (rhx_itr != handle_elt_itr.MemberEnd()) {
                    if (rhx_itr->value.IsNumber()) {
                      aframe->get_scale(elt_indx)->set_rh_x(rhx_itr->value.GetDouble());
                    }
                  }
                  auto rhy_itr = handle_elt_itr.FindMember("right_y");
                  if (rhy_itr != handle_elt_itr.MemberEnd()) {
                    if (rhy_itr->value.IsNumber()) {
                      aframe->get_scale(elt_indx)->set_rh_y(rhy_itr->value.GetDouble());
                    }
                  }

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
    if (!(get_object(a)->get_key().empty())) {
      writer.Key("key");
      writer.String(get_object(a)->get_key().c_str(), \
        (rapidjson::SizeType)get_object(a)->get_key().length());
    }

    if (!(get_object(a)->get_name().empty())) {
      writer.Key("name");
      writer.String(get_object(a)->get_name().c_str(), \
        (rapidjson::SizeType)get_object(a)->get_name().length());
    }

    if (!(get_object(a)->get_scene().empty())) {
      writer.Key("scene");
      writer.String(get_object(a)->get_scene().c_str(), \
        (rapidjson::SizeType)get_object(a)->get_scene().length());
    }

    if (!(get_object(a)->get_type().empty())) {
      writer.Key("type");
      writer.String(get_object(a)->get_type().c_str(), \
        (rapidjson::SizeType)get_object(a)->get_type().length());
    }

    if (!(get_object(a)->get_subtype().empty())) {
      writer.Key("subtype");
      writer.String(get_object(a)->get_subtype().c_str(), \
        (rapidjson::SizeType)get_object(a)->get_subtype().length());
    }

    writer.Key("owner");
    writer.String(get_object(a)->get_owner().c_str(), \
      (rapidjson::SizeType)get_object(a)->get_owner().length());

    if (get_object(a)->get_frame() > -9999) {
      writer.Key("frame");
      writer.Uint(get_object(a)->get_frame());
    }

    if (get_object(a)->get_timestamp() > -9999) {
      writer.Key("timestamp");
      writer.Uint(get_object(a)->get_timestamp());
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
