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
    get_object(a)->to_json_writer(writer, get_msg_type());
  }

  writer.EndArray();

  writer.EndObject();

  // The Stringbuffer now contains a json message
  // of the object
  std::string return_string(s.GetString());
  out_string.assign(return_string);
}
