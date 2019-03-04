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

#include "json_object.h"
#include "object_interface.h"
#include "data_factory.h"
#include "data_list.h"
#include "object_list_interface.h"

#include "app/include/clyman_utils.h"

#ifndef SRC_API_INCLUDE_JSON_OBJECT_LIST_H_
#define SRC_API_INCLUDE_JSON_OBJECT_LIST_H_

using ObjectDataList = DataList<ObjectInterface*>;

// An ObjectList stores a vector of pointers to ObjectDocuments
// It is responsible for parsing requests from external clients,
// and writing the responses to go back to those external clients.
class JsonObjectList : public ObjectDataList, public ObjectListInterface {
  // Data Factory
  DataFactory ofactory;

 public:
  // Constructor
  JsonObjectList() : ObjectDataList() {}
  ~JsonObjectList() {}
  // Copy Constructor
  JsonObjectList(const JsonObjectList &olist) : ObjectDataList(olist) {}
  // Message Type
  // Integer constant representing the acion type
  // Defined in app_utils.h
  int get_msg_type() const override {return ObjectDataList::get_msg_type();}
  void set_msg_type(int nt) override {ObjectDataList::set_msg_type(nt);}
  // Error Code
  // Integer code representing the failure/success of an operation
  // Values are defined in app_utils.h
  int get_error_code() const override {return ObjectDataList::get_error_code();}
  void set_error_code(int nc) override {ObjectDataList::set_error_code(nc);}
  // Operation Type
  // Integer code representing the type of operation
  // Values are defined in app_utils.h
  int get_op_type() const override {return ObjectDataList::get_op_type();}
  void set_op_type(int nt) override {ObjectDataList::set_op_type(nt);}
  // Error Message
  // A String value containing a human-readable Error Message
  // Will be empty if no error is encountered
  std::string get_error_message() const override {return ObjectDataList::get_error_message();}
  void set_error_message(std::string nm) override {ObjectDataList::set_error_message(nm);}
  // Transaction ID
  // Transaction ID's can be used to trace a single transaction across services
  // It can either be passed in, or generated upon reciept of a message
  std::string get_transaction_id() const override {return ObjectDataList::get_transaction_id();}
  void set_transaction_id(std::string nid) override {ObjectDataList::set_transaction_id(nid);}
  // Number of Records
  // Allows a device to specify the number of records to
  // be returned from a query.
  int get_num_records() const override {return ObjectDataList::get_num_records();}
  void set_num_records(int nr) override {ObjectDataList::set_num_records(nr);}
  // Object List
  // An array of Object Interfaces
  int num_objects() const override {return ObjectDataList::num_objects();}
  void add_object(ObjectInterface *o) override {ObjectDataList::add_object(o);}
  ObjectInterface* get_object(int index) const override {return ObjectDataList::get_object(index);}
  void remove_object(int index) override {ObjectDataList::remove_object(index);}
  void clear_objects() override {ObjectDataList::clear_objects();}
  // Message generation methods
  void to_msg_string(std::string &out_string) override;
};

#endif  // SRC_API_INCLUDE_JSON_OBJECT_LIST_H_
