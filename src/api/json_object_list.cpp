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
  obj_logging->debug("Building ObjectList from JSON Document");
  // Initialize Empty String elements
  set_msg_type(-1);
  set_error_code(100);
  set_num_records(0);
  // Start parsing the JSON Object
  if (d.IsObject()) {
    obj_logging->debug("Object-Format Message Detected");

    // Parse the base elements
    if (!(d.HasMember("msg_type"))) \
      {throw Object3dException("No Msg Type Found");}
    const rapidjson::Value *mtype_val = &d["msg_type"];
    set_msg_type(mtype_val->GetInt());
    if (d.HasMember("transaction_id")) {
      const rapidjson::Value *tid_val = &d["transaction_id"];
      if (!(tid_val->IsNull())) {
        set_transaction_id(tid_val->GetString());
        obj_logging->debug("Transaction ID Pulled");
      }
    }
    if (d.HasMember("num_records")) {
      const rapidjson::Value *nr_val = &d["num_records"];
      if (!(nr_val->IsNull())) {
        set_num_records(nr_val->GetInt());
        obj_logging->debug("Num Records Pulled");
      }
    }

    // Parse the object list
    if (d.HasMember("objects")) {
      const rapidjson::Value *objs_val = &d["objects"];
      if (!(objs_val->IsNull())) {
        obj_logging->debug("Objects Array pulled");
        if (objs_val->IsArray()) {
          for (auto& itr : objs_val->GetArray()) {
            obj_logging->debug("Object Returned from objects array");
            // Create a new object
            ObjectInterface *new_obj = ofactory.build_object();

            // Parse the string attributes
            rapidjson::Value::ConstMemberIterator key_iter = \
              itr.FindMember("key");
            if (key_iter != itr.MemberEnd()) {
              if (!(key_iter->value.IsNull())) {
                new_obj->set_key(key_iter->value.GetString());
              }
            }

            rapidjson::Value::ConstMemberIterator name_iter = \
              itr.FindMember("name");
            if (name_iter != itr.MemberEnd()) {
              if (!(name_iter->value.IsNull())) {
                new_obj->set_name(name_iter->value.GetString());
              }
            }

            rapidjson::Value::ConstMemberIterator scn_iter = \
              itr.FindMember("scene");
            if (scn_iter != itr.MemberEnd()) {
              if (!(scn_iter->value.IsNull())) {
                new_obj->set_scene(scn_iter->value.GetString());
              }
            }

            rapidjson::Value::ConstMemberIterator owner_iter = \
              itr.FindMember("owner");
            if (owner_iter != itr.MemberEnd()) {
              if (!(owner_iter->value.IsNull())) {
                new_obj->set_owner(owner_iter->value.GetString());
              }
            }

            rapidjson::Value::ConstMemberIterator type_iter = \
              itr.FindMember("type");
            if (type_iter != itr.MemberEnd()) {
              if (!(type_iter->value.IsNull())) {
                new_obj->set_type(type_iter->value.GetString());
              }
            }

            rapidjson::Value::ConstMemberIterator subtype_iter = \
              itr.FindMember("subtype");
            if (subtype_iter != itr.MemberEnd()) {
              if (!(subtype_iter->value.IsNull())) {
                new_obj->set_subtype(subtype_iter->value.GetString());
              }
            }

            rapidjson::Value::ConstMemberIterator assets_iter = \
              itr.FindMember("assets");
            if (assets_iter != itr.MemberEnd()) {
              const rapidjson::Value& asset_val = assets_iter->value;
              if (!(asset_val.IsNull())) {
                for (auto& asset_itr : asset_val.GetArray()) {
                  new_obj->add_asset(asset_itr.GetString());
                }
              }
            }

            obj_logging->debug("Basic string values pulled");

            // Parse the transform elements
            rapidjson::Value::ConstMemberIterator translation_iter = \
              itr.FindMember("translation");
            if (translation_iter != itr.MemberEnd()) {
              obj_logging->debug("Translation pulled");
              const rapidjson::Value& translation_val = translation_iter->value;
              if (!(translation_val.IsNull())) {
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
                Translation *trans = new Translation(x, y, z);
                new_obj->transform(trans);
                delete trans;
              }
            }

            rapidjson::Value::ConstMemberIterator erot_iter = \
              itr.FindMember("euler_rotation");
            if (erot_iter != itr.MemberEnd()) {
              obj_logging->debug("Euler Rotation pulled");
              const rapidjson::Value& erot_val = erot_iter->value;
              if (!(erot_val.IsNull())) {
                int i = 0;
                double theta = 0.0;
                double x = 0.0;
                double y = 0.0;
                double z = 0.0;
                for (auto& erot_itr : erot_val.GetArray()) {
                  if (i == 0) {
                    theta = erot_itr.GetDouble();
                  } else if (i == 1) {
                    x = erot_itr.GetDouble();
                  } else if (i == 2) {
                    y = erot_itr.GetDouble();
                  } else {z = erot_itr.GetDouble();}
                  i++;
                }
                EulerRotation *erot = new EulerRotation(theta, x, y, z);
                new_obj->transform(erot);
                delete erot;
              }
            }

            rapidjson::Value::ConstMemberIterator scale_iter = \
              itr.FindMember("scale");
            if (scale_iter != itr.MemberEnd()) {
              obj_logging->debug("Scale pulled");
              const rapidjson::Value& scale_val = scale_iter->value;
              if (!(scale_val.IsNull())) {
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
                Scale *scl = new Scale(x, y, z);
                new_obj->transform(scl);
                delete scl;
              }
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
  obj_logging->debug("Writing Object to JSON");

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

  obj_logging->debug("Basic attributes written");

  // Write the object array
  writer.Key("objects");
  writer.StartArray();

  for (int a = 0; a < num_objects(); a++) {
    obj_logging->debug("Writing Object into Objects Array");

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

    if (!(get_object(a)->get_owner().empty())) {
      writer.Key("owner");
      writer.String(get_object(a)->get_owner().c_str(), \
        (rapidjson::SizeType)get_object(a)->get_owner().length());
    }

    obj_logging->debug("Basic Object Attributes written");

    // Write transforms
    obj_logging->debug("Writing Transform Matrix");
    writer.Key("transform");
    writer.StartArray();
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        writer.Double(get_object(a)->get_transform()->get_transform_element(i, \
          j));
      }
    }
    writer.EndArray();

    writer.EndObject();
  }

  writer.EndArray();

  writer.EndObject();

  // The Stringbuffer now contains a json message
  // of the object
  json_cstr_val = s.GetString();
  out_string.assign(json_cstr_val);
}
