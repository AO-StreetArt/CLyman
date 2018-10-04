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

#include "include/json_property_list.h"

// Constructor from a parsed JSON Document
// Not going to parse error code or message as we dont care about them coming
// in, only going out
JsonPropertyList::JsonPropertyList(const rapidjson::Document& d) {
  // Initialize Empty String elements
  set_msg_type(-1);
  set_error_code(100);
  set_num_records(10);
  // Start parsing the JSON Object
  if (d.IsObject()) {

    // Parse the base elements
    if (d.HasMember("msg_type")) {
      const rapidjson::Value *mtype_val = &d["msg_type"];
      if (mtype_val->IsInt()) {
        set_msg_type(mtype_val->GetInt());
      }
    }
    if (d.HasMember("operation")) {
      const rapidjson::Value *opid_val = &d["operation"];
      if (opid_val->IsInt()) {
        set_op_type(opid_val->GetInt());
      }
    }
    if (d.HasMember("transaction_id")) {
      const rapidjson::Value *tid_val = &d["transaction_id"];
      if (tid_val->IsString()) {
        set_transaction_id(tid_val->GetString());
      }
    }
    if (d.HasMember("num_records")) {
      const rapidjson::Value *nr_val = &d["num_records"];
      if (nr_val->IsInt()) {
        set_num_records(nr_val->GetInt());
      }
    }

    // Parse the object list
    if (d.HasMember("properties")) {
      const rapidjson::Value *objs_val = &d["properties"];
      if (!(objs_val->IsNull())) {
        if (objs_val->IsArray()) {
          for (auto& itr : objs_val->GetArray()) {
            PropertyInterface *new_prop = ofactory.build_property();
            // Basic elements
            std::string key_value;
            find_json_string_elt_in_array(itr, "key", key_value);
            new_prop->set_key(key_value);
            std::string name_value;
            find_json_string_elt_in_array(itr, "name", name_value);
            new_prop->set_name(name_value);
            std::string parent_value;
            find_json_string_elt_in_array(itr, "parent", parent_value);
            new_prop->set_parent(parent_value);
            std::string scene_value;
            find_json_string_elt_in_array(itr, "scene", scene_value);
            new_prop->set_scene(scene_value);
            std::string asi_value;
            find_json_string_elt_in_array(itr, "asset_sub_id", asi_value);
            new_prop->set_asset_sub_id(asi_value);
            new_prop->set_frame(find_json_int_elt_in_array(itr, "frame"));
            new_prop->set_timestamp(find_json_int_elt_in_array(itr, "timestamp"));
            // Array of Property Values
            auto props_itr = itr.FindMember("values");
            if (props_itr != itr.MemberEnd()) {
              if (props_itr->value.IsArray()) {
                int elt_indx = 0;
                for (auto& prop_elt_itr : props_itr->value.GetArray()) {
                  std::string value;
                  auto val_itr = prop_elt_itr.FindMember("value");
                  if (val_itr != prop_elt_itr.MemberEnd()) {
                    if (val_itr->value.IsDouble()) {
                      new_prop->add_value(val_itr->value.GetDouble());
                      parse_json_graph_handle(prop_elt_itr, new_prop->get_handle(elt_indx));
                    }
                  }
                  elt_indx++;
                }
              }
            }
            add_object(new_prop);
          }
        }
      }
    // d.HasMember(objects)
    }

  // d.IsObject
  }
}

// write a JSON string from the object list
void JsonPropertyList::to_msg_string(std::string &out_string) {

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
  writer.Key("properties");
  writer.StartArray();

  for (int a = 0; a < num_objects(); a++) {

    writer.StartObject();

    // Write basic attributes
    write_json_string_elt(writer, "key", get_object(a)->get_key());
    write_json_string_elt(writer, "name", get_object(a)->get_name());
    write_json_string_elt(writer, "parent", get_object(a)->get_parent());
    write_json_string_elt(writer, "asset_sub_id", get_object(a)->get_asset_sub_id());
    write_json_string_elt(writer, "scene", get_object(a)->get_scene());
    write_json_int_elt(writer, "frame", get_object(a)->get_frame());
    write_json_int_elt(writer, "timestamp", get_object(a)->get_timestamp());

    // Write values array
    if (get_object(a)->num_values() > 0) {
      writer.Key("values");
      writer.StartArray();
      for (int i = 0; i < get_object(a)->num_values(); i++) {
        writer.StartObject();
        writer.Key("value");
        writer.Double(get_object(a)->get_value(i));
        write_json_graph_handle(writer, get_object(a)->get_handle(i));
        writer.EndObject();
      }
      writer.EndArray();
    }
    writer.EndObject();
  }

  writer.EndArray();

  writer.EndObject();

  // The Stringbuffer now contains a json message
  // of the object
  std::string return_string(s.GetString());
  out_string.assign(return_string);
}
