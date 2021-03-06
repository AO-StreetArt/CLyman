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
#include "db_manager_interface.h"

#include "Poco/Logger.h"
#include "Poco/RWLock.h"

#ifndef SRC_APPLICATION_INCLUDE_CORE_DATABASE_MANAGER_H_
#define SRC_APPLICATION_INCLUDE_CORE_DATABASE_MANAGER_H_

//! Encapsulates the Mongocxx client, ensuring that
//! we can safely update the connection on failure
class CoreDatabaseManager {
  mongocxx::instance inst{};
  mongocxx::pool *pool = nullptr;
  AOSSL::NetworkApplicationProfile *internal_profile = nullptr;
  Poco::Logger& logger;
  std::string last_connection_string;
  bool secured = false;
  AOSSL::ServiceInterface *connected_service = nullptr;
  std::atomic<int> failures{0};
  std::atomic<bool> initialized{false};
  std::string service_name = "mongo";
  mongocxx::options::pool *pool_options = nullptr;
  mongocxx::options::client client_options;
  mongocxx::options::ssl ssl_options;
  bool ssl_active = false;
  bool allow_invalid_certs = false;
  std::string pem_file;
  std::string pem_password;
  std::string ca_file;
  std::string ca_dir;
  std::string crl_file;
  // We use a RW lock to let any number of queries execute
  // simultaneously, XOR let a connection get updated
  Poco::RWLock conn_usage_lock;
  int max_failures = 5;

public:
  int get_failures() {return failures;}
  void add_failure() {failures++;}
  // Get a Read lock on the DB Connection
  Poco::RWLock& get_lock() {return conn_usage_lock;}

  // Get the Database Logger
  Poco::Logger& get_logger() {return logger;}

  // Get the Mongo Connection Pool
  mongocxx::pool* get_connection_pool() {return pool;}

  // Discover a new Mongo connection from Consul
  void find_new_connection();

  // Set a new connection with a scoped lock on the connection
  void set_new_connection();

  // Initialize the database manager with a connection
  void init_with_connection(std::string connection_string);
  void init();

  // Insert a Bson Document into a Mongo Collection
  void insert_doc(mongocxx::collection &coll, \
      bsoncxx::document::value &doc_value, std::string& key, \
      DatabaseResponse &response);

  // Add an Animation Graph Handle to a Document
  void add_graph_handle_to_document(bsoncxx::builder::stream::document &builder, \
        AnimationGraphHandle *handle);

  void get_handle_from_element(bsoncxx::document::element elt, AnimationGraphHandle *handle);
  void get_handle_from_element(bsoncxx::array::element elt, AnimationGraphHandle *handle);

  CoreDatabaseManager(AOSSL::NetworkApplicationProfile *profile, \
      std::string conn) : logger(Poco::Logger::get("DatabaseManager")) {
    internal_profile = profile;
    init_with_connection(conn);
  }
  CoreDatabaseManager(AOSSL::NetworkApplicationProfile *profile, \
      std::string conn, bool is_ssl_active) : logger(Poco::Logger::get("DatabaseManager")) {
    internal_profile = profile;
    ssl_active = is_ssl_active;
    init_with_connection(conn);
  }
  CoreDatabaseManager(AOSSL::NetworkApplicationProfile *profile, \
      std::string conn, bool is_ssl_active, bool validate_server_cert, \
      std::string ssl_pem_file, std::string ssl_pem_passwd, std::string ssl_ca_file, \
      std::string ssl_ca_dir, std::string ssl_crl_file) : logger(Poco::Logger::get("DatabaseManager")) {
    internal_profile = profile;
    ssl_active = is_ssl_active;
    allow_invalid_certs = !validate_server_cert;
    pem_file.assign(ssl_pem_file);
    pem_password.assign(ssl_pem_passwd);
    ca_file.assign(ssl_ca_file);
    ca_dir.assign(ssl_ca_dir);
    crl_file.assign(ssl_crl_file);
    init_with_connection(conn);
  }
  ~CoreDatabaseManager() {
    if (connected_service) delete connected_service;
    if (pool_options) delete pool_options;
    if (pool) delete pool;
  }
};

#endif  // SRC_APPLICATION_INCLUDE_CORE_DATABASE_MANAGER_H_
