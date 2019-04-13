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
#include "model/factory/include/data_list_factory.h"
#include "model/list/include/object_list_interface.h"
#include "model/factory/include/data_factory.h"
#include "model/object/include/object_interface.h"
#include "model/object/include/transforms.h"
#include "model/object/include/object_frame.h"
#include "model/property/include/property_frame.h"

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/exception/exception.hpp>

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include "app/include/clyman_utils.h"

#include "core_database_manager.h"
#include "property_database_manager.h"

#include "Poco/Logger.h"
#include "Poco/RWLock.h"

#ifndef SRC_APPLICATION_INCLUDE_OBJ3_DATABASE_MANAGER_H_
#define SRC_APPLICATION_INCLUDE_OBJ3_DATABASE_MANAGER_H_

//! Encapsulates the Mongocxx client, ensuring that
//! we can safely update the connection on failure
class ObjectDatabaseManager : public PropertyDatabaseManager {
  Poco::Logger& logger = Poco::Logger::get("DatabaseManager");
  std::string db_name;
  std::string coll_name;
  int max_retries = 11;
  // Factories
  DataFactory object_factory;

  // Internal methods for getting data model classes out of BSON documents
  void get_frame_from_doc(bsoncxx::document::element &elt, ObjectFrame *aframe, int frame_index);

  void get_action_from_doc(bsoncxx::document::element &elt, AnimationAction<ObjectFrame> *action, std::string& action_name);

  // Internal methods for constructing BSON documents of data model children
  void add_handles_to_doc(bsoncxx::builder::stream::document &builder, ObjectFrame *aframe);

  void add_obj_frame_to_doc(bsoncxx::builder::stream::document &builder, ObjectFrame *aframe);

  void add_obj_action_to_doc(bsoncxx::builder::stream::document &builder, AnimationAction<ObjectFrame> *action);

  // Build a Bson document to use for creation
  void build_create_doc(bsoncxx::builder::stream::document &builder, \
      ObjectInterface *obj);

  // Build a Bson document to use for updates
  void build_update_doc(bsoncxx::builder::stream::document &builder, \
      ObjectInterface *obj, bool is_append_operation);

  void build_query_doc(bsoncxx::builder::stream::document &builder, \
      ObjectInterface *obj);

  // Execute a Creation or Update Transaction
  void transaction(DatabaseResponse &response, ObjectInterface *obj, \
      AnimationAction<ObjectFrame> *action, ObjectFrame *aframe, \
      std::string& key, int transaction_type, bool is_append_operation);

  // Execute a transaction, with default value for is_append_operation
  void transaction(DatabaseResponse &response, ObjectInterface *obj, \
      std::string& key, int transaction_type, bool is_append_operation) {
    transaction(response, obj, nullptr, nullptr, key, transaction_type, true);
  }

  // Execute a transaction, with default value for is_append_operation
  void transaction(DatabaseResponse &response, \
      AnimationAction<ObjectFrame> *action, std::string& key, \
      int transaction_type) {
    transaction(response, nullptr, action, nullptr, key, transaction_type, true);
  }

  // Execute a transaction, with default value for is_append_operation
  void transaction(DatabaseResponse &response, \
      AnimationAction<ObjectFrame> *action, ObjectFrame *frame, \
      std::string& key, int transaction_type) {
    transaction(response, nullptr, action, frame, key, transaction_type, true);
  }

  // Execute a transaction, with default value for is_append_operation
  void transaction(DatabaseResponse &response, ObjectInterface *obj, \
      std::string& key, int transaction_type) {
    transaction(response, obj, key, transaction_type, true);
  }

  // Convert a BSON Document View to an Object Interface
  void bson_to_obj3(bsoncxx::document::view& result, ObjectInterface *obj);

 public:
  ObjectDatabaseManager(AOSSL::NetworkApplicationProfile *profile, std::string conn, \
      std::string db, std::string collection, std::string props_collection) : \
      PropertyDatabaseManager(profile, conn, db, props_collection) {
    db_name.assign(db);
    coll_name.assign(collection);
  }
  ObjectDatabaseManager(AOSSL::NetworkApplicationProfile *profile, std::string conn, \
      std::string db, std::string collection, std::string props_collection, bool is_ssl_active, \
      bool validate_server_cert, std::string ssl_pem_file, std::string ssl_pem_passwd, \
      std::string ssl_ca_file, std::string ssl_ca_dir, std::string ssl_crl_file) : \
      PropertyDatabaseManager(profile, conn, db, props_collection, is_ssl_active, validate_server_cert, \
        ssl_pem_file, ssl_pem_passwd, ssl_ca_file, ssl_ca_dir, ssl_crl_file) {
    db_name.assign(db);
    coll_name.assign(collection);
  }
  ~ObjectDatabaseManager() {}

  //! Create an obj3 in the Mongo Database
  //! The newly generated key for the object will be populated
  //! into the key parameter.
  void create_object(DatabaseResponse &response, ObjectInterface *obj, std::string& key) {
    logger.debug("Attempting to create object in Mongo");
    transaction(response, obj, key, _DB_MONGO_INSERT_);
  }

  //! Update an existing obj3 in the Mongo Database
  //! The supplied key will be used as the key to update in the DB
  void update_object(DatabaseResponse &response, ObjectInterface *obj, std::string& key) {
    logger.debug("Attempting to update object in Mongo");
    transaction(response, obj, key, _DB_MONGO_UPDATE_);
  }

  //! Update an existing obj3 in the Mongo Database
  //! The supplied key will be used as the key to update in the DB
  void update_object(DatabaseResponse &response, ObjectInterface *obj, std::string& key, bool is_append_operation) {
    logger.debug("Attempting to update object in Mongo");
    transaction(response, obj, key, _DB_MONGO_UPDATE_, is_append_operation);
  }

  void get_object(ObjectListInterface *response, std::string& key);

  //! Query for Obj3 documents matching the input
  void query(ObjectListInterface *response, ObjectInterface *obj, int max_results);

  //! Delete an Object in Mongo
  void delete_object(DatabaseResponse& response, std::string& key);

  //! Lock an Object to a particular device
  void lock_object(DatabaseResponse& response, std::string& object_id, std::string& device_id) {
    ObjectInterface *query_obj = object_factory.build_object();
    query_obj->set_owner(device_id);
    transaction(response, query_obj, object_id, _DB_MONGO_LOCK_);
  }

  //! Unlock an object from a particular device
  void unlock_object(DatabaseResponse& response, std::string& object_id, std::string& device_id) {
    ObjectInterface *query_obj = object_factory.build_object();
    query_obj->set_owner(device_id);
    transaction(response, query_obj, object_id, _DB_MONGO_UNLOCK_);
  }

  //! Create an Object Action
  void create_action(DatabaseResponse& response, std::string& parent_key, AnimationAction<ObjectFrame> *action, std::string& name) {
    action->set_name(name);
    transaction(response, action, parent_key, _DB_MONGO_ACTION_INSERT_);
  }

  //! Update an Object Action
  //! The supplied key will be used as the key to update in the DB
  void update_action(DatabaseResponse& response, std::string& parent_key, AnimationAction<ObjectFrame> *action, std::string& name) {
    action->set_name(name);
    transaction(response, action, parent_key, _DB_MONGO_ACTION_UPDATE_);
  }

  //! Delete an Object Action
  void delete_object_action(DatabaseResponse& response, std::string& parent_key, std::string& name) {
    AnimationAction<ObjectFrame> action;
    action.set_name(name);
    transaction(response, &action, parent_key, _DB_MONGO_ACTION_REMOVE_);
  }

  //! Create an Object Keyframe
  void create_keyframe(DatabaseResponse& response, std::string& object_key, std::string& action_name, ObjectFrame *frame, int frame_index) {
    AnimationAction<ObjectFrame> action;
    action.set_name(action_name);
    frame->set_frame(frame_index);
    transaction(response, &action, frame, object_key, _DB_MONGO_FRAME_INSERT_);
  }

  void update_keyframe(DatabaseResponse& response, std::string& object_key, std::string& action_name, ObjectFrame *frame, int frame_index) {
    AnimationAction<ObjectFrame> action;
    action.set_name(action_name);
    frame->set_frame(frame_index);
    transaction(response, &action, frame, object_key, _DB_MONGO_FRAME_UPDATE_);
  }

  //! Delete an Object Keyframe
  void delete_object_keyframe(DatabaseResponse& response, std::string& object_key, std::string& action_name, int frame_index) {
    AnimationAction<ObjectFrame> action;
    action.set_name(action_name);
    ObjectFrame frame;
    frame.set_frame(frame_index);
    transaction(response, &action, &frame, object_key, _DB_MONGO_FRAME_REMOVE_);
  }
};

#endif  // SRC_APPLICATION_INCLUDE_OBJ3_DATABASE_MANAGER_H_
