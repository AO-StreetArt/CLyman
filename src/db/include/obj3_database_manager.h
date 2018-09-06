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
#include "model/include/object_interface.h"
#include "model/include/transforms.h"

#include "api/include/object_list_factory.h"
#include "api/include/object_list_interface.h"

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
  ObjectFactory object_factory;

  void add_handles_to_doc(bsoncxx::builder::stream::document &builder, AnimationFrameInterface *aframe);

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
      std::string& key, int transaction_type, bool is_append_operation);

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
};

#endif  // SRC_APPLICATION_INCLUDE_OBJ3_DATABASE_MANAGER_H_
