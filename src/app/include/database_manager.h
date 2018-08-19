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

#include "api/include/object_list_factory.h"
#include "api/include/object_list_interface.h"

#include "aossl/profile/include/network_app_profile.h"
#include "aossl/consul/include/consul_interface.h"

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
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
  mongocxx::client *internal_connection;
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
    if (internal_connection) delete internal_connection;
    internal_connection = new mongocxx::client(uri);
  }
  inline void set_new_connection() {
    if (failures.load() > max_failures) {
      Poco::ScopedWriteRWLock scoped_lock(conn_usage_lock);
      logger.debug("Max Neo4j Failures reached, identifying new instance");
      // Attempt to find a new Neo4j instance to use
      if (connected_service) delete connected_service;
      find_new_connection();
      failures = 0;
    }
  }
 public:
  DatabaseManager(AOSSL::NetworkApplicationProfile *profile) : \
      logger(Poco::Logger::get("DatabaseManager")) \
      {internal_profile = profile;}
  ~DatabaseManager() {if (connected_service) delete connected_service;if (internal_connection) delete internal_connection;}

  //! Create an obj3 in the Mongo Database
  inline void create_object(ObjectInterface *obj) {
    int retries = 0;
    while (retries < max_retries) {
      // Initialize the connection for the first time
      bool expected_init_value = false;
      if (initialized.compare_exchange_strong(expected_init_value, true)) {
        find_new_connection();
      }
      bool failure = false;
      try {
        Poco::ScopedReadRWLock scoped_lock(conn_usage_lock);
        // TO-DO: Create and execute the Mongo Query
      } catch (std::exception& e) {
        logger.error("Exception executing Neo4j Query");
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
