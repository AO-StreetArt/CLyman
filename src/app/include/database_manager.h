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

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include "app/include/clyman_utils.h"

#ifndef SRC_APPLICATION_INCLUDE_DATABASE_MANAGER_H_
#define SRC_APPLICATION_INCLUDE_DATABASE_MANAGER_H_

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
  inline void find_new_connection() {
    logger.information("Discovering Mongo Connection");
    connected_service = internal_profile->get_service(service_name);
    AOSSL::StringBuffer mongo_un_buf;
    AOSSL::StringBuffer mongo_pw_buf;
    AOSSL::StringBuffer mongo_ssl_ca_buf;
    AOSSL::StringBuffer mongo_ssl_ca_dir_buf;
    internal_profile->get_opt(std::string("mongo.auth.un"), mongo_un_buf);
    internal_profile->get_opt(std::string("mongo.auth.pw"), mongo_pw_buf);
    std::string mongo_conn_str = std::string("mongodb://") + mongo_un_buf.val\
        + std::string(":") + mongo_pw_buf.val + std::string("@")\
        + connected_service->get_address() + std::string(":")\
        + connected_service->get_port();
    // Check for TLS Configuration
    internal_profile->get_opt(std::string("mongo.ssl.ca.file"), mongo_ssl_ca_buf);
    internal_profile->get_opt(std::string("mongo.ssl.ca.dir"), mongo_ssl_ca_dir_buf);
    // TO-DO: Add TLS configuration to Mongo Driver
    // Reset the internal connection
    logger.information("Connecting to Mongo instance: %s", mongo_conn_str);
    mongocxx::uri uri(mongo_conn_str);
    if (pool) delete pool;
    pool = new mongocxx::pool{uri};
  }
  inline void set_new_connection() {
    if (failures.load() > max_failures) {
      Poco::ScopedWriteRWLock scoped_lock(conn_usage_lock);
      logger.debug("Max Mongo Failures reached, identifying new instance");
      // Attempt to find a new Neo4j instance to use
      if (connected_service) delete connected_service;
      find_new_connection();
      failures = 0;
    }
  }
  inline void init_with_connection(std::string connection_string, \
      std::string db, std::string coll) {
    db_name.assign(db);
    coll_name.assign(coll);
    if (!(connection_string.empty())) {
      mongocxx::uri uri(connection_string);
      pool = new mongocxx::pool{uri};
      initialized = true;
    }
  }
 public:
  DatabaseManager(AOSSL::NetworkApplicationProfile *profile, std::string conn, \
      std::string db, std::string collection) : logger(Poco::Logger::get("DatabaseManager")) \
      {internal_profile = profile;init_with_connection(conn, db, collection);}
  ~DatabaseManager() {if (connected_service) delete connected_service;if (pool) delete pool;}

  //! Create an obj3 in the Mongo Database
  //! The newly generated key for the object will be populated
  //! into the key parameter.
  inline void create_object(ObjectInterface *obj, std::string& key) {
    int retries = 0;
    logger.debug("Attempting to create object in Mongo");
    while (retries < max_retries) {
      // Initialize the connection for the first time
      bool expected_init_value = false;
      if (initialized.compare_exchange_strong(expected_init_value, true)) {
        find_new_connection();
      }
      bool failure = false;
      try {
        Poco::ScopedReadRWLock scoped_lock(conn_usage_lock);
        // Find the DB and Collection we're going to write into
        auto client = pool->acquire();
        mongocxx::database db = (*client)[db_name];
        mongocxx::collection coll = db[coll_name];
        // Use a BSON Builder to construct the document
        auto builder = bsoncxx::builder::stream::document{};
        builder << "key" << obj->get_key();
        if (!(obj->get_name().empty())) {
          builder << "name" << obj->get_name();
        }
        if (!(obj->get_type().empty())) {
          builder << "type" << obj->get_type();
        }
        if (!(obj->get_subtype().empty())) {
          builder << "subtype" << obj->get_subtype();
        }
        if (!(obj->get_owner().empty())) {
          builder << "owner" << obj->get_owner();
        }
        if (!(obj->get_scene().empty())) {
          builder << "scene" << obj->get_scene();
        }
        if (obj->get_frame() > -1) {
          builder << "frame" << obj->get_frame();
        }
        if (obj->get_timestamp() > -1) {
          builder << "timestamp" << obj->get_timestamp();
        }
        auto asset_array = bsoncxx::builder::stream::array{};
        for (int i = 0; i < obj->num_assets(); i++) {
          asset_array << obj->get_asset(i);
        }
        if (obj->num_assets() > 0) {
          builder << "assets" << asset_array;
        }
        if (obj->has_transform()) {
          auto transform_array = bsoncxx::builder::stream::array{};
          for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
              if (j < 4 && k < 4) {
                transform_array << std::to_string(obj->get_transform()->get_transform_element(j, k));
              }
            }
          }
          builder << "transform" << transform_array;
        }
        bsoncxx::document::value doc_value = \
          builder << bsoncxx::builder::stream::finalize;
        // Execute the insert
        auto view = doc_value.view();
        auto result = coll.insert_one(view);
        // Pull the generated ID out of the response
        if (result->result().inserted_count() > 0) {
          key.assign(result->inserted_id().get_utf8().value.to_string());
        }
      } catch (std::exception& e) {
        logger.error("Exception executing Mongo Query");
        logger.error(e.what());
        failures++;
        failure = true;
      }
      if (failure) set_new_connection();
      retries++;
    }
  }

  // TO-DO: Get, Update, Delete, Query operations
};

#endif  // SRC_APPLICATION_INCLUDE_DATABASE_MANAGER_H_
