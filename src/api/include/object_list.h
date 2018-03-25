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

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "Obj3.pb.h"

#include "object_document.h"
#include "object_interface.h"
#include "object_list_interface.h"

#ifndef SRC_API_INCLUDE_OBJECT_LIST_H_
#define SRC_API_INCLUDE_OBJECT_LIST_H_

// An ObjectList stores a vector of pointers to ObjectDocuments
// It is responsible for parsing requests from external clients,
// and writing the responses to go back to those external clients.
class ObjectList : public ObjectListInterface {
  int msg_type;
  int op_type;
  int err_code;
  std::string err_msg;
  std::string transaction_id;
  int num_records;
  std::vector<ObjectInterface*> objects;
  // String Return Value
  const char* json_cstr_val;
  std::string json_str_val;
  std::string protobuf_string = "";

 public:
  // Constructor
  inline ObjectList() {
    msg_type = -1;
    err_code = 100;
    err_msg = "";
    transaction_id = "";
    num_records = 0;
    op_type = 10;
  }
  // Destructor
  virtual inline ~ObjectList() {
    for (unsigned int i = 0; i < objects.size(); i++) {
      if (objects[i]) delete objects[i];
    }
  }
  // Copy Constructor
  inline ObjectList(const ObjectList &olist) {
    obj_logging->debug("Copy Constructor Called");
    msg_type = olist.get_msg_type();
    err_code = olist.get_error_code();
    err_msg = olist.get_error_message();
    transaction_id = olist.get_transaction_id();
    num_records = olist.get_num_records();
    op_type = olist.get_op_type();
    for (int i = 0; i < olist.num_objects(); i++) {
      // Invoke the ObjectDocument Copy Constructor
      ObjectDocument *o = new ObjectDocument(*(olist.get_object(i)));
      objects.push_back(o);
    }
  }
  // Getters
  int get_msg_type() const {return msg_type;}
  int get_op_type() const {return op_type;}
  int get_error_code() const {return err_code;}
  std::string get_error_message() const {return err_msg;}
  std::string get_transaction_id() const {return transaction_id;}
  int get_num_records() const {return num_records;}
  // Setters
  void set_msg_type(int nt) {msg_type = nt;}
  void set_op_type(int nt) {op_type = nt;}
  void set_error_code(int nc) {err_code = nc;}
  void set_error_message(std::string nm) {err_msg.assign(nm);}
  void set_transaction_id(std::string nid) {transaction_id.assign(nid);}
  void set_num_records(int nr) {num_records = nr;}
  // Object list methods
  int num_objects() const {return objects.size();}
  void add_object(ObjectInterface *o) {objects.push_back(o);}
  ObjectInterface* get_object(int index) const {return objects[index];}
  void remove_object(int index) {objects.erase(objects.begin()+index);}
  void clear_objects() {objects.clear();}
  // Message generation methods
  virtual void to_msg_string(std::string &out_string) = 0;
};

#endif  // SRC_API_INCLUDE_OBJECT_LIST_H_
