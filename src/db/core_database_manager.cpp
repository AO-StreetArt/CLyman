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
    // Reset the internal connection
    logger.information("Connecting to Mongo instance: %s", mongo_conn_str);
    if (pool) delete pool;
    if (ssl_active) {
      mongocxx::uri uri(mongo_conn_str + "?ssl=true");
      // Set up any SSL Ops
      if (allow_invalid_certs) ssl_options.allow_invalid_certificates(true);
      if (!(pem_file.empty())) ssl_options.pem_file(bsoncxx::string::view_or_value(pem_file));
      if (!(pem_password.empty())) ssl_options.pem_password(bsoncxx::string::view_or_value(pem_password));
      if (!(ca_file.empty())) ssl_options.ca_file(bsoncxx::string::view_or_value(ca_file));
      if (!(ca_dir.empty())) ssl_options.pem_file(bsoncxx::string::view_or_value(ca_dir));
      if (!(crl_file.empty())) ssl_options.pem_file(bsoncxx::string::view_or_value(crl_file));
      client_options.ssl_opts(ssl_options);
      // Initialize the connection pool
      pool_options = new mongocxx::options::pool{client_options};
      pool = new mongocxx::pool{uri, *(pool_options)};
    } else {
      mongocxx::uri uri(mongo_conn_str);
      // Initialize the connection pool
      pool = new mongocxx::pool{uri};
    }
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
    if (ssl_active) {
      mongocxx::uri uri(connection_string + "?ssl=true");
      // Set up any SSL Ops
      if (allow_invalid_certs) ssl_options.allow_invalid_certificates(true);
      if (!(pem_file.empty())) ssl_options.pem_file(bsoncxx::string::view_or_value(pem_file));
      if (!(pem_password.empty())) ssl_options.pem_password(bsoncxx::string::view_or_value(pem_password));
      if (!(ca_file.empty())) ssl_options.ca_file(bsoncxx::string::view_or_value(ca_file));
      if (!(ca_dir.empty())) ssl_options.pem_file(bsoncxx::string::view_or_value(ca_dir));
      if (!(crl_file.empty())) ssl_options.pem_file(bsoncxx::string::view_or_value(crl_file));
      client_options.ssl_opts(ssl_options);
      // Initialize the connection pool
      pool_options = new mongocxx::options::pool{client_options};
      pool = new mongocxx::pool{uri, *(pool_options)};
    } else {
      mongocxx::uri uri(connection_string);
      // Initialize the connection pool
      pool = new mongocxx::pool{uri};
    }
    initialized = true;
  }
}

void CoreDatabaseManager::init() {
  bool expected_init_value = false;
  if (initialized.compare_exchange_strong(expected_init_value, true)) {
    find_new_connection();
  }
}

void CoreDatabaseManager::get_handle_from_element(bsoncxx::document::element value_elt, AnimationGraphHandle *handle) {
    auto val_ltype_elt = value_elt["left_type"];
    auto val_rtype_elt = value_elt["right_type"];
    handle->set_lh_type(val_ltype_elt.get_utf8().value.to_string());
    handle->set_rh_type(val_rtype_elt.get_utf8().value.to_string());
    auto val_lx_elt = value_elt["left_x"];
    handle->set_lh_x(val_lx_elt.get_double().value);
    auto val_ly_elt = value_elt["left_y"];
    handle->set_lh_y(val_ly_elt.get_double().value);
    auto val_rx_elt = value_elt["right_x"];
    handle->set_rh_x(val_rx_elt.get_double().value);
    auto val_ry_elt = value_elt["right_y"];
    handle->set_rh_y(val_ry_elt.get_double().value);
}

void CoreDatabaseManager::get_handle_from_element(bsoncxx::array::element value_elt, AnimationGraphHandle *handle) {
    auto val_ltype_elt = value_elt["left_type"];
    auto val_rtype_elt = value_elt["right_type"];
    handle->set_lh_type(val_ltype_elt.get_utf8().value.to_string());
    handle->set_rh_type(val_rtype_elt.get_utf8().value.to_string());
    auto val_lx_elt = value_elt["left_x"];
    handle->set_lh_x(val_lx_elt.get_double().value);
    auto val_ly_elt = value_elt["left_y"];
    handle->set_lh_y(val_ly_elt.get_double().value);
    auto val_rx_elt = value_elt["right_x"];
    handle->set_rh_x(val_rx_elt.get_double().value);
    auto val_ry_elt = value_elt["right_y"];
    handle->set_rh_y(val_ry_elt.get_double().value);
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
