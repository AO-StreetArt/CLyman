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

#include "model/core/include/animation_action.h"
#include "model/factory/include/data_list_factory.h"
#include "model/object/include/object_frame.h"
#include "model/object/include/transforms.h"
#include "model/object/include/object_interface.h"
#include "model/list/include/object_list_interface.h"
#include "model/list/include/property_list_interface.h"
#include "model/property/include/property_frame.h"
#include "model/property/include/property_interface.h"

#ifndef SRC_APPLICATION_INCLUDE_DB_MANAGER_INTERFACE_H_
#define SRC_APPLICATION_INCLUDE_DB_MANAGER_INTERFACE_H_

const int _DB_MONGO_INSERT_ = 0;
const int _DB_MONGO_UPDATE_ = 1;
const int _DB_MONGO_REMOVE_ = 2;
const int _DB_MONGO_GET_ = 3;
const int _DB_MONGO_QUERY_ = 4;
const int _DB_MONGO_LOCK_ = 5;
const int _DB_MONGO_UNLOCK_ = 6;
const int _DB_MONGO_ACTION_INSERT_ = 7;
const int _DB_MONGO_ACTION_UPDATE_ = 8;
const int _DB_MONGO_ACTION_REMOVE_ = 9;
const int _DB_MONGO_ACTION_GET_ = 10;
const int _DB_MONGO_FRAME_INSERT_ = 11;
const int _DB_MONGO_FRAME_UPDATE_ = 12;
const int _DB_MONGO_FRAME_REMOVE_ = 13;
const int _DB_MONGO_FRAME_GET_ = 14;

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

  //! Create an Object Action
  virtual void create_action(DatabaseResponse& response, std::string& parent_key, AnimationAction<ObjectFrame> *action, std::string& name) = 0;

  //! Update an Object Action
  //! The supplied key will be used as the key to update in the DB
  virtual void update_action(DatabaseResponse& response, std::string& parent_key, AnimationAction<ObjectFrame> *action, std::string& name) = 0;

  //! Delete an Object Action
  virtual void delete_object_action(DatabaseResponse& response, std::string& parent_key, std::string& name) = 0;

  //! Create a Property Action
  virtual void create_action(DatabaseResponse& response, std::string& parent_key, AnimationAction<PropertyFrame> *action, std::string& name) = 0;

  //! Update a Property Action
  //! The supplied key will be used as the key to update in the DB
  virtual void update_action(DatabaseResponse& response, std::string& parent_key, AnimationAction<PropertyFrame> *action, std::string& name) = 0;

  //! Delete a Property Action
  virtual void delete_property_action(DatabaseResponse& response, std::string& parent_key, std::string& name) = 0;

  //! Create an Object Keyframe
  virtual void create_keyframe(DatabaseResponse& response, std::string& object_key, std::string& action_name, ObjectFrame *frame, int frame_index) = 0;

  //! Update an Object Keyframe
  virtual void update_keyframe(DatabaseResponse& response, std::string& object_key, std::string& action_name, ObjectFrame *frame, int frame_index) = 0;

  //! Delete an Object Keyframe
  virtual void delete_object_keyframe(DatabaseResponse& response, std::string& object_key, std::string& action_name, int frame_index) = 0;

  //! Create a Property Keyframe
  virtual void create_keyframe(DatabaseResponse& response, std::string& property_key, std::string& action_name, PropertyFrame *frame, int frame_index) = 0;

  //! Update a Property Keyframe
  virtual void update_keyframe(DatabaseResponse& response, std::string& property_key, std::string& action_name, PropertyFrame *frame, int frame_index) = 0;

  //! Delete an Property Keyframe
  virtual void delete_property_keyframe(DatabaseResponse& response, std::string& property_key, std::string& action_name, int frame_index) = 0;
};

#endif  // SRC_APPLICATION_INCLUDE_DB_MANAGER_INTERFACE_H_
