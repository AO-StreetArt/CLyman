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

#include "include/core_database_manager.h"

void CoreDatabaseManager::find_new_connection() {
  logger.information("Discovering Mongo Connection");
  if (internal_profile->get_consul()) {
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
  } else {
    logger.error("No Consul instance found, unable to discover Mongo instances");
  }
}

void CoreDatabaseManager::set_new_connection() {
  if (failures.load() > max_failures) {
    Poco::ScopedWriteRWLock scoped_lock(conn_usage_lock);
    logger.debug("Max Mongo Failures reached, identifying new instance");
    // Attempt to find a new Neo4j instance to use
    if (connected_service) delete connected_service;
    find_new_connection();
    failures = 0;
  }
}

void CoreDatabaseManager::init_with_connection(std::string connection_string) {
  if (!(connection_string.empty())) {
    mongocxx::uri uri(connection_string);
    pool = new mongocxx::pool{uri};
    initialized = true;
  }
}

void CoreDatabaseManager::init() {
  bool expected_init_value = false;
  if (initialized.compare_exchange_strong(expected_init_value, true)) {
    find_new_connection();
  }
}

void CoreDatabaseManager::add_graph_handle_to_document(\
        bsoncxx::builder::stream::document &builder, AnimationGraphHandle *handle) {
    builder << "left_type" << handle->get_lh_type();
    builder << "right_type" << handle->get_rh_type();
    builder << "left_x" << handle->get_lh_x();
    builder << "left_y" << handle->get_lh_y();
    builder << "right_x" << handle->get_rh_x();
    builder << "right_y" << handle->get_rh_y();
}

void CoreDatabaseManager::insert_doc(mongocxx::collection &coll, bsoncxx::document::value &doc_value, \
    std::string& key, DatabaseResponse &response) {
  // Execute the insert
  auto view = doc_value.view();
  auto result = coll.insert_one(view);
  // Pull the generated ID out of the response
  if (result->result().inserted_count() > 0) {
    bsoncxx::oid oid = result->inserted_id().get_oid().value;
    key.assign(oid.to_string());
    logger.debug(key);
    response.success = true;
  } else {
    response.error_message = std::string("No Documents Inserted into DB");
  }
}
