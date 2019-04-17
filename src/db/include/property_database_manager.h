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


#include <iostream>
#include <boost/cstdint.hpp>

#include "model/core/include/animation_action.h"
#include "model/factory/include/data_factory.h"
#include "model/property/include/property_interface.h"
#include "model/property/include/property_frame.h"
#include "model/factory/include/data_list_factory.h"
#include "model/list/include/property_list_interface.h"

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/exception/exception.hpp>

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include "app/include/clyman_utils.h"
#include "model/core/include/animation_action.h"
#include "model/object/include/object_frame.h"
#include "model/property/include/property_frame.h"


#include "core_database_manager.h"

#include "Poco/Logger.h"
#include "Poco/RWLock.h"

#ifndef SRC_APPLICATION_INCLUDE_PROPERTY_DATABASE_MANAGER_H_
#define SRC_APPLICATION_INCLUDE_PROPERTY_DATABASE_MANAGER_H_

//! Encapsulates the Mongocxx client, ensuring that
//! we can safely update the connection on failure
class PropertyDatabaseManager : public CoreDatabaseManager {
  Poco::Logger& logger = Poco::Logger::get("DatabaseManager");
  std::string db_name;
  std::string coll_name;
  int max_retries = 11;
  // Factories
  DataFactory object_factory;

  void add_prop_frame_to_doc(bsoncxx::builder::stream::document &builder, PropertyFrame *aframe);

  void add_prop_action_to_doc(bsoncxx::builder::stream::document &action_doc, AnimationAction<PropertyFrame> *action);

  // Execute a Creation or Update Transaction
  void prop_transaction(DatabaseResponse &response, PropertyInterface *obj, \
      AnimationAction<PropertyFrame> *action, PropertyFrame *aframe, \
      std::string& key, int transaction_type, bool is_append_operation);

  // Execute a transaction, with default value for is_append_operation
  void prop_transaction(DatabaseResponse &response, PropertyInterface *obj, \
      std::string& key, int transaction_type, bool is_append_operation) {
    prop_transaction(response, obj, nullptr, nullptr, key, transaction_type, true);
  }

  // Execute a transaction, with default value for is_append_operation
  void prop_transaction(DatabaseResponse &response, \
      AnimationAction<PropertyFrame> *action, std::string& key, \
      int transaction_type) {
    prop_transaction(response, nullptr, action, nullptr, key, transaction_type, true);
  }

  // Execute a transaction, with default value for is_append_operation
  void prop_transaction(DatabaseResponse &response, \
      AnimationAction<PropertyFrame> *action, PropertyFrame *frame, \
      std::string& key, int transaction_type) {
    prop_transaction(response, nullptr, action, frame, key, transaction_type, true);
  }

  // Execute a transaction, with default value for is_append_operation
  void prop_transaction(DatabaseResponse &response, PropertyInterface *obj, \
      std::string& key, int transaction_type) {
    prop_transaction(response, obj, key, transaction_type, true);
  }

  // Convert a BSON Document View to an Object Interface
  void bson_to_prop(bsoncxx::document::view& result, PropertyInterface *obj);

  void build_prop_doc(bsoncxx::builder::stream::document &builder, PropertyInterface *obj, bool include_actions);

 public:
  PropertyDatabaseManager(AOSSL::NetworkApplicationProfile *profile, std::string conn, \
      std::string db, std::string collection) : CoreDatabaseManager(profile, conn) {
    db_name.assign(db);
    coll_name.assign(collection);
  }
  PropertyDatabaseManager(AOSSL::NetworkApplicationProfile *profile, std::string conn, \
      std::string db, std::string collection, bool is_ssl_active, \
      bool validate_server_cert, std::string ssl_pem_file, std::string ssl_pem_passwd, \
      std::string ssl_ca_file, std::string ssl_ca_dir, std::string ssl_crl_file) : \
      CoreDatabaseManager(profile, conn, is_ssl_active, validate_server_cert, \
        ssl_pem_file, ssl_pem_passwd, ssl_ca_file, ssl_ca_dir, ssl_crl_file) {
    db_name.assign(db);
    coll_name.assign(collection);
  }
  virtual ~PropertyDatabaseManager() {}

  // Build a Bson document to use for creation
  void build_create_prop_doc(bsoncxx::builder::stream::document &builder, \
      PropertyInterface *obj);

  // Build a Bson document to use for updates
  void build_update_prop_doc(bsoncxx::builder::stream::document &builder, \
      PropertyInterface *obj, bool is_append_operation);

  void build_query_prop_doc(bsoncxx::builder::stream::document &builder, \
      PropertyInterface *obj);

  //! Create an obj3 in the Mongo Database
  //! The newly generated key for the object will be populated
  //! into the key parameter.
  void create_property(DatabaseResponse &response, PropertyInterface *obj, std::string& key) {
    logger.debug("Attempting to create object in Mongo");
    prop_transaction(response, obj, key, _DB_MONGO_INSERT_);
  }

  //! Update an existing obj3 in the Mongo Database
  //! The supplied key will be used as the key to update in the DB
  void update_property(DatabaseResponse &response, PropertyInterface *obj, std::string& key) {
    logger.debug("Attempting to update object in Mongo");
    prop_transaction(response, obj, key, _DB_MONGO_UPDATE_);
  }

  //! Update an existing obj3 in the Mongo Database
  //! The supplied key will be used as the key to update in the DB
  void update_property(DatabaseResponse &response, PropertyInterface *obj, std::string& key, bool is_append_operation) {
    logger.debug("Attempting to update object in Mongo");
    prop_transaction(response, obj, key, _DB_MONGO_UPDATE_, is_append_operation);
  }

  void get_property(PropertyListInterface *response, std::string& key);

  //! Query for Obj3 documents matching the input
  void property_query(PropertyListInterface *response, PropertyInterface *obj, int max_results);

  //! Delete an Object in Mongo
  void delete_property(DatabaseResponse& response, std::string& key);

  //! Create a Property Action
  //! The newly generated key for the action will be populated
  //! into the key parameter.
  void create_action(DatabaseResponse& response, std::string& parent_key, AnimationAction<PropertyFrame> *action, std::string& action_name) {
    action->set_name(action_name);
    prop_transaction(response, action, parent_key, _DB_MONGO_ACTION_INSERT_);
  }

  //! Update a Property Action
  void update_action(DatabaseResponse& response, std::string& parent_key, AnimationAction<PropertyFrame> *action, std::string& action_name) {
    action->set_name(action_name);
    prop_transaction(response, action, parent_key, _DB_MONGO_ACTION_UPDATE_);
  }

  //! Delete a Property Action
  void delete_property_action(DatabaseResponse& response, std::string& parent_key, std::string& action_name) {
    AnimationAction<PropertyFrame> action;
    action.set_name(action_name);
    prop_transaction(response, &action, parent_key, _DB_MONGO_ACTION_REMOVE_);
  }

  //! Create a Property Keyframe
  void create_keyframe(DatabaseResponse& response, std::string& property_key, std::string& action_name, PropertyFrame *frame, int frame_index) {
    AnimationAction<PropertyFrame> action;
    action.set_name(action_name);
    frame->set_frame(frame_index);
    prop_transaction(response, &action, frame, property_key, _DB_MONGO_FRAME_INSERT_);
  }

  void update_keyframe(DatabaseResponse& response, std::string& property_key, std::string& action_name, PropertyFrame *frame, int frame_index) {
    AnimationAction<PropertyFrame> action;
    action.set_name(action_name);
    frame->set_frame(frame_index);
    prop_transaction(response, &action, frame, property_key, _DB_MONGO_FRAME_UPDATE_);
  }

  //! Delete an Property Keyframe
  void delete_property_keyframe(DatabaseResponse& response, std::string& property_key, std::string& action_name, int frame_index) {
    AnimationAction<PropertyFrame> action;
    action.set_name(action_name);
    PropertyFrame frame;
    frame.set_frame(frame_index);
    prop_transaction(response, &action, &frame, property_key, _DB_MONGO_FRAME_REMOVE_);
  }
};

#endif  // SRC_APPLICATION_INCLUDE_PROPERTY_DATABASE_MANAGER_H_
