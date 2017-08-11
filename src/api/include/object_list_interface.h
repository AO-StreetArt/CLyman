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

#include "object_interface.h"

#ifndef SRC_API_INCLUDE_OBJECT_LIST_INTERFACE_H_
#define SRC_API_INCLUDE_OBJECT_LIST_INTERFACE_H_

// An ObjectList stores a vector of pointers to ObjectDocuments
// It is responsible for parsing requests from external clients,
// and writing the responses to go back to those external clients.
class ObjectListInterface {
 public:
  // Destructor
  virtual ~ObjectListInterface() {}
  // Message Type
  // Integer constant representing the acion type
  // Defined in app_utils.h
  virtual int get_msg_type() const = 0;
  virtual void set_msg_type(int nt) = 0;
  // Error Code
  // Integer code representing the failure/success of an operation
  // Values are defined in app_utils.h
  virtual int get_error_code() const = 0;
  virtual void set_error_code(int nc) = 0;
  // Error Message
  // A String value containing a human-readable Error Message
  // Will be empty if no error is encountered
  virtual std::string get_error_message() const = 0;
  virtual void set_error_message(std::string nm) = 0;
  // Transaction ID
  // Transaction ID's can be used to trace a single transaction across services
  // It can either be passed in, or generated upon reciept of a message
  virtual std::string get_transaction_id() const = 0;
  virtual void set_transaction_id(std::string nid) = 0;
  // Number of Records
  // Allows a device to specify the number of records to
  // be returned from a query.
  virtual int get_num_records() const = 0;
  virtual void set_num_records(int nr) = 0;
  // Object List
  // An array of Object Interfaces
  virtual int num_objects() const = 0;
  virtual void add_object(ObjectInterface *o) = 0;
  virtual ObjectInterface* get_object(int index) const = 0;
  virtual void remove_object(int index) = 0;
  virtual void clear_objects() = 0;
  // Generate a string message from the object list
  virtual void to_msg_string(std::string &out_string) = 0;
};

#endif  // SRC_API_INCLUDE_OBJECT_LIST_INTERFACE_H_
