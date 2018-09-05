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

#include "model/include/object_interface.h"
#include "model/include/transforms.h"

#include "api/include/object_list_interface.h"

#include "aossl/profile/include/network_app_profile.h"
#include "aossl/consul/include/consul_interface.h"

#include "obj3_database_manager.h"
#include "property_database_manager.h"
#include "db_manager_interface.h"

#ifndef SRC_APPLICATION_INCLUDE_DATABASE_MANAGER_H_
#define SRC_APPLICATION_INCLUDE_DATABASE_MANAGER_H_

//! Encapsulates the Mongocxx client, ensuring that
//! we can safely update the connection on failure
class DatabaseManager : public ObjectDatabaseManager, public DatabaseManagerInterface {
 public:
  DatabaseManager(AOSSL::NetworkApplicationProfile *profile, std::string conn, \
      std::string db, std::string obj_collection, std::string props_collection) \
      : ObjectDatabaseManager(profile, conn, db, obj_collection, props_collection) {}
  ~DatabaseManager() {}

  //! Create a Property in the Mongo Database
  //! The newly generated key for the object will be populated
  //! into the key parameter.
  void create_property(DatabaseResponse &response, PropertyInterface *obj, std::string& key) {
    PropertyDatabaseManager::create_property(response, obj, key);
  }

  //! Update an existing Property in the Mongo Database
  //! The supplied key will be used as the key to update in the DB
  void update_property(DatabaseResponse &response, PropertyInterface *obj, std::string& key) {
    PropertyDatabaseManager::update_property(response, obj, key);
  }

  //! Update an existing Property in the Mongo Database
  //! The supplied key will be used as the key to update in the DB
  void update_property(DatabaseResponse &response, PropertyInterface *obj, std::string& key, bool is_append_operation) {
    PropertyDatabaseManager::update_property(response, obj, key, is_append_operation);
  }

  //! Get a Property by ID
  void get_property(PropertyListInterface *response, std::string& key) {
    PropertyDatabaseManager::get_property(response, key);
  }

  //! Query for Property documents matching the input
  void property_query(PropertyListInterface *response, PropertyInterface *obj, int max_results) {
    PropertyDatabaseManager::property_query(response, obj, max_results);
  }

  //! Delete a Property in Mongo
  void delete_property(DatabaseResponse& response, std::string& key) {
    PropertyDatabaseManager::delete_property(response, key);
  }

  //! Create an obj3 in the Mongo Database
  //! The newly generated key for the object will be populated
  //! into the key parameter.
  void create_object(DatabaseResponse &response, ObjectInterface *obj, std::string& key) {

  }

  //! Update an existing obj3 in the Mongo Database
  //! The supplied key will be used as the key to update in the DB
  void update_object(DatabaseResponse &response, ObjectInterface *obj, std::string& key) {

  }

  //! Update an existing obj3 in the Mongo Database
  //! The supplied key will be used as the key to update in the DB
  void update_object(DatabaseResponse &response, ObjectInterface *obj, std::string& key, bool is_append_operation) {

  }

  void get_object(ObjectListInterface *response, std::string& key) {

  }

  //! Query for Obj3 documents matching the input
  void object_query(ObjectListInterface *response, ObjectInterface *obj, int max_results) {

  }

  //! Delete an Object in Mongo
  void delete_object(DatabaseResponse& response, std::string& key) {

  }

  //! Lock an Object to a particular device
  void lock_object(DatabaseResponse& response, std::string& object_id, std::string& device_id) {

  }

  //! Unlock an object from a particular device
  void unlock_object(DatabaseResponse& response, std::string& object_id, std::string& device_id) {

  }
};

#endif  // SRC_APPLICATION_INCLUDE_DATABASE_MANAGER_H_
