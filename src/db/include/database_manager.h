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

#include "aossl/profile/include/network_app_profile.h"
#include "aossl/consul/include/consul_interface.h"

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

#include "Poco/Logger.h"
#include "Poco/RWLock.h"

#ifndef SRC_APPLICATION_INCLUDE_DATABASE_MANAGER_H_
#define SRC_APPLICATION_INCLUDE_DATABASE_MANAGER_H_

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
};

//! Encapsulates the Mongocxx client, ensuring that
//! we can safely update the connection on failure
class DatabaseManager {
  mongocxx::instance inst{};
  mongocxx::pool *pool = nullptr;
  std::string db_name;
  std::string coll_name;
  AOSSL::NetworkApplicationProfile *internal_profile = nullptr;
  Poco::Logger& logger;
  std::string last_connection_string;
  bool secured = false;
  AOSSL::ServiceInterface *connected_service = nullptr;
  std::atomic<int> failures{0};
  std::atomic<bool> initialized{false};
  std::string service_name = "mongo";
  // We use a RW lock to let any number of queries execute
  // simultaneously, XOR let a connection get updated
  Poco::RWLock conn_usage_lock;
  int max_failures = 5;
  int max_retries = 11;
  // Factories
  ObjectFactory object_factory;

  // Discover a new Mongo connection from Consul
  void find_new_connection();

  // Set a new connection with a scoped lock on the connection
  void set_new_connection();

  // Initialize the database manager with a connection
  void init_with_connection(std::string connection_string, \
      std::string db, std::string coll);
  void init();

  // Insert a Bson Document into a Mongo Collection
  void insert_doc(mongocxx::collection &coll, \
      bsoncxx::document::value &doc_value, std::string& key, \
      DatabaseResponse &response);

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
  DatabaseManager(AOSSL::NetworkApplicationProfile *profile, std::string conn, \
      std::string db, std::string collection) : logger(Poco::Logger::get("DatabaseManager")) {
    internal_profile = profile;
    init_with_connection(conn, db, collection);
  }
  ~DatabaseManager() {
    if (connected_service) delete connected_service;
    if (pool) delete pool;
  }

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

#endif  // SRC_APPLICATION_INCLUDE_DATABASE_MANAGER_H_
