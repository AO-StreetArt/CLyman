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

#include <vector>
#include <string>

#include "obj3.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "Obj3.pb.h"

#ifndef SRC_INCLUDE_OBJ3_LIST_H_
#define SRC_INCLUDE_OBJ3_LIST_H_

class Obj3List {
  int msg_type;
  int err_code;
  std::string err_msg;
  std::string transaction_id;
  int num_records;
  std::vector<Obj3*> objects;
  // String Return Value
  const char* json_cstr_val;
  std::string json_str_val;
  std::string protobuf_string = "";

 public:
  // Constructor
  Obj3List();
  ~Obj3List();
  // Inbound Message Translation methods
  Obj3List(const rapidjson::Document& d);
  Obj3List(protoObj3::Obj3List proto_list);
  // Copy Constructor
  Obj3List(const Obj3List &olist);
  // Getters
  int get_msg_type() const {return msg_type;}
  int get_error_code() const {return err_code;}
  std::string get_error_message() const {return err_msg;}
  std::string get_transaction_id() const {return transaction_id;}
  int get_num_records() const {return num_records;}
  // Setters
  void set_msg_type(int nt) {msg_type = nt;}
  void set_error_code(int nc) {err_code = nc;}
  void set_error_message(std::string nm) {err_msg.assign(nm);}
  void set_transaction_id(std::string nid) {transaction_id.assign(nid);}
  void set_num_records(int nr) {num_records = nr;}
  // Object list methods
  int num_objects() const {return objects.size();}
  void add_object(Obj3 *o) {objects.push_back(o);}
  Obj3* get_object(int index) const {return objects[index];}
  void remove_object(int index) {objects.erase(objects.begin()+index);}
  void clear_objects() {objects.clear();}
  // Message generation methods
  std::string to_json();
  std::string to_protobuf();
};

#endif  // SRC_INCLUDE_OBJ3_LIST_H_
