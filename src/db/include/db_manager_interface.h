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

// This implements the Configuration Manager

// This takes in a Command Line Interpreter, and based on the options provided,
// decides how the application needs to be configured.  It may configure either
// from a configuration file, or from a Consul agent

#include <iostream>
#include <boost/cstdint.hpp>

#include "model/include/object_factory.h"
#include "model/include/property_interface.h"
#include "model/include/object_interface.h"
#include "model/include/transforms.h"

#include "api/include/object_list_factory.h"
#include "api/include/object_list_interface.h"
#include "api/include/property_list_interface.h"

#ifndef SRC_APPLICATION_INCLUDE_DB_MANAGER_INTERFACE_H_
#define SRC_APPLICATION_INCLUDE_DB_MANAGER_INTERFACE_H_

const int _DB_MONGO_INSERT_ = 0;
const int _DB_MONGO_UPDATE_ = 1;
const int _DB_MONGO_REMOVE_ = 2;
const int _DB_MONGO_GET_ = 3;
const int _DB_MONGO_QUERY_ = 4;
const int _DB_MONGO_LOCK_ = 5;
const int _DB_MONGO_UNLOCK_ = 6;

//! Encapsulates a response from the DatabaseManager
struct DatabaseResponse {
  bool success = false;
  std::string error_message;
  int error_code = 100;
};

//! Encapsulates the Mongocxx client, ensuring that
//! we can safely update the connection on failure
class DatabaseManagerInterface {
 public:
  virtual ~DatabaseManagerInterface() {}

  //! Create a Property in the Mongo Database
  //! The newly generated key for the object will be populated
  //! into the key parameter.
  virtual void create_property(DatabaseResponse &response, PropertyInterface *obj, std::string& key) = 0;

  //! Update an existing Property in the Mongo Database
  //! The supplied key will be used as the key to update in the DB
  virtual void update_property(DatabaseResponse &response, PropertyInterface *obj, std::string& key) = 0;

  //! Update an existing Property in the Mongo Database
  //! The supplied key will be used as the key to update in the DB
  virtual void update_property(DatabaseResponse &response, PropertyInterface *obj, std::string& key, bool is_append_operation) = 0;

  //! Get a Property by ID
  virtual void get_property(PropertyListInterface *response, std::string& key) = 0;

  //! Query for Property documents matching the input
  virtual void property_query(PropertyListInterface *response, PropertyInterface *obj, int max_results) = 0;

  //! Delete a Property in Mongo
  virtual void delete_property(DatabaseResponse& response, std::string& key) = 0;

  //! Create an obj3 in the Mongo Database
  //! The newly generated key for the object will be populated
  //! into the key parameter.
  virtual void create_object(DatabaseResponse &response, ObjectInterface *obj, std::string& key) = 0;

  //! Update an existing obj3 in the Mongo Database
  //! The supplied key will be used as the key to update in the DB
  virtual void update_object(DatabaseResponse &response, ObjectInterface *obj, std::string& key) = 0;

  //! Update an existing obj3 in the Mongo Database
  //! The supplied key will be used as the key to update in the DB
  virtual void update_object(DatabaseResponse &response, ObjectInterface *obj, std::string& key, bool is_append_operation) = 0;

  virtual void get_object(ObjectListInterface *response, std::string& key) = 0;

  //! Query for Obj3 documents matching the input
  virtual void object_query(ObjectListInterface *response, ObjectInterface *obj, int max_results) = 0;

  //! Delete an Object in Mongo
  virtual void delete_object(DatabaseResponse& response, std::string& key) = 0;

  //! Lock an Object to a particular device
  virtual void lock_object(DatabaseResponse& response, std::string& object_id, std::string& device_id) = 0;

  //! Unlock an object from a particular device
  virtual void unlock_object(DatabaseResponse& response, std::string& object_id, std::string& device_id) = 0;
};

#endif  // SRC_APPLICATION_INCLUDE_DB_MANAGER_INTERFACE_H_
