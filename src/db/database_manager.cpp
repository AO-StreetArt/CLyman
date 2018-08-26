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

#include "include/database_manager.h"

void DatabaseManager::find_new_connection() {
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

void DatabaseManager::set_new_connection() {
  if (failures.load() > max_failures) {
    Poco::ScopedWriteRWLock scoped_lock(conn_usage_lock);
    logger.debug("Max Mongo Failures reached, identifying new instance");
    // Attempt to find a new Neo4j instance to use
    if (connected_service) delete connected_service;
    find_new_connection();
    failures = 0;
  }
}

void DatabaseManager::init_with_connection(std::string connection_string, \
    std::string db, std::string coll) {
  db_name.assign(db);
  coll_name.assign(coll);
  if (!(connection_string.empty())) {
    mongocxx::uri uri(connection_string);
    pool = new mongocxx::pool{uri};
    initialized = true;
  }
}

void DatabaseManager::init() {
  bool expected_init_value = false;
  if (initialized.compare_exchange_strong(expected_init_value, true)) {
    find_new_connection();
  }
}

void DatabaseManager::insert_doc(mongocxx::collection &coll, bsoncxx::document::value &doc_value, \
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

void DatabaseManager::build_create_doc(bsoncxx::builder::stream::document &builder, ObjectInterface *obj) {
  // Creation Document
  builder << "name" << obj->get_name();
  builder << "type" << obj->get_type();
  builder << "subtype" << obj->get_subtype();
  builder << "owner" << obj->get_owner();
  builder << "scene" << obj->get_scene();
  builder << "frame" << obj->get_frame();
  builder << "timestamp" << obj->get_timestamp();
  auto asset_array = bsoncxx::builder::stream::array{};
  for (int i = 0; i < obj->num_assets(); i++) {
    asset_array << obj->get_asset(i);
  }
  builder << "assets" << asset_array;
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

void DatabaseManager::build_update_doc(bsoncxx::builder::stream::document &builder, ObjectInterface *obj, bool is_append_operation) {
  // Update Document
  auto set_doc = bsoncxx::builder::stream::document{};
  if (!(obj->get_name().empty())) {
    set_doc << "name" << obj->get_name();
  }
  if (!(obj->get_type().empty())) {
    set_doc << "type" << obj->get_type();
  }
  if (!(obj->get_subtype().empty())) {
    set_doc << "subtype" << obj->get_subtype();
  }
  if (!(obj->get_owner().empty())) {
    set_doc << "owner" << obj->get_owner();
  }
  if (!(obj->get_scene().empty())) {
    set_doc << "scene" << obj->get_scene();
  }
  if (obj->get_frame() > -1) {
    set_doc << "frame" << obj->get_frame();
  }
  if (obj->get_timestamp() > -1) {
    set_doc << "timestamp" << obj->get_timestamp();
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
    set_doc << "transform" << transform_array;
  }
  builder << "$set" << set_doc;
  auto push_doc = bsoncxx::builder::stream::document{};
  auto asset_array = bsoncxx::builder::stream::array{};
  for (int i = 0; i < obj->num_assets(); i++) {
    asset_array << obj->get_asset(i);
  }
  push_doc << "assets" << asset_array;
  std::string update_opt_key;
  if (is_append_operation) {
    update_opt_key = "$push";
  } else {
    update_opt_key = "$pull";
  }
  builder << update_opt_key << push_doc;
}

void DatabaseManager::transaction(DatabaseResponse &response, ObjectInterface *obj, std::string& key, int transaction_type, bool is_append_operation) {
  int retries = 0;
  while (retries < max_retries) {
    // Initialize the connection for the first time
    init();
    bool failure = false;
    try {
      Poco::ScopedReadRWLock scoped_lock(conn_usage_lock);
      // Find the DB and Collection we're going to write into
      auto client = pool->acquire();
      mongocxx::database db = (*client)[db_name];
      mongocxx::collection coll = db[coll_name];
      // Use a BSON Builder to construct the main document
      auto builder = bsoncxx::builder::stream::document{};
      if (transaction_type == _DB_MONGO_INSERT_) {
        build_create_doc(builder, obj);
      } else if (transaction_type == _DB_MONGO_UPDATE_ \
          || transaction_type == _DB_MONGO_LOCK_) {
        build_update_doc(builder, obj, is_append_operation);
      } else if (transaction_type == _DB_MONGO_UNLOCK_) {
        auto set_doc = bsoncxx::builder::stream::document{};
        set_doc << "owner" << "";
        builder << "$set" << set_doc;
      }
      // Execute an Insert Transaction
      if (transaction_type == _DB_MONGO_INSERT_) {
        bsoncxx::document::value doc_value = \
          builder << bsoncxx::builder::stream::finalize;
        insert_doc(coll, doc_value, key, response);
      // Execute an Update Transaction
      } else if (transaction_type == _DB_MONGO_UPDATE_ \
          || transaction_type == _DB_MONGO_LOCK_ \
          || transaction_type == _DB_MONGO_UNLOCK_) {
        auto query_builder = bsoncxx::builder::stream::document{};
        query_builder << "_id" << bsoncxx::oid(key.c_str(), key.length());
        if (transaction_type == _DB_MONGO_LOCK_) {
          query_builder << "owner" << "";
        } else if (transaction_type == _DB_MONGO_UNLOCK_) {
          query_builder << "owner" << obj->get_owner();
        }
        auto result = coll.update_one(query_builder << bsoncxx::builder::stream::finalize, \
            builder << bsoncxx::builder::stream::finalize);
        if (result) {
          if (result->modified_count() > 0) {
            logger.debug("Document Successfully updated in DB");
            response.success = true;
          } else {
            response.error_message = std::string("No Documents Modified in DB");
          }
        } else {
          response.error_message = std::string("Null Result returned from DB");
        }
      }
    } catch (mongocxx::exception& me) {
      logger.error("Mongo Exception Encountered");
      logger.error(me.what());
      response.error_message = std::string(me.what());
      break;
    } catch (std::exception& e) {
      logger.error("Exception executing Mongo Query");
      logger.error(e.what());
      failures++;
      failure = true;
    }
    if (failure) set_new_connection();
    retries++;
    if (response.success && !(failure)) {
      break;
    }
  }
}

void DatabaseManager::bson_to_obj3(bsoncxx::document::view& result, ObjectInterface *obj) {
  // Parse basic values
  bsoncxx::document::element name_element = result["name"];
  obj->set_name(name_element.get_utf8().value.to_string());
  bsoncxx::document::element type_element = result["type"];
  obj->set_type(type_element.get_utf8().value.to_string());
  bsoncxx::document::element subtype_element = result["subtype"];
  obj->set_subtype(subtype_element.get_utf8().value.to_string());
  bsoncxx::document::element scene_element = result["scene"];
  obj->set_scene(scene_element.get_utf8().value.to_string());
  bsoncxx::document::element owner_element = result["owner"];
  obj->set_owner(owner_element.get_utf8().value.to_string());
  bsoncxx::document::element frame_element = result["frame"];
  obj->set_frame(frame_element.get_int32().value);
  bsoncxx::document::element timestamp_element = result["timestamp"];
  obj->set_timestamp(timestamp_element.get_int32().value);
  // Parse the assets array
  bsoncxx::document::element assets_element = result["assets"];
  bsoncxx::array::view assets_view = assets_element.get_array().value;
  for (unsigned int i = 0; i < assets_view.length(); i++) {
    bsoncxx::array::element asset_elt = assets_view[i];
    obj->add_asset(asset_elt.get_utf8().value.to_string());
  }
  // Parse the transform array
  bsoncxx::document::element transform_element = result["transform"];
  bsoncxx::array::view transform_view = transform_element.get_array().value;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int index = (4 * i) + j;
      bsoncxx::array::element transform_elt = transform_view[index];
      obj->get_transform()->set_transform_element(i, j, transform_elt.get_double().value);
    }
  }
}

void DatabaseManager::get_object(ObjectListInterface *response, std::string& key) {
  logger.debug("Attempting to retrieve object from Mongo");
  int retries = 0;
  while (retries < max_retries) {
    // Initialize the connection for the first time
    init();
    bool failure = false;
    try {
      Poco::ScopedReadRWLock scoped_lock(conn_usage_lock);
      // Find the DB and Collection we're going to write into
      auto client = pool->acquire();
      mongocxx::database db = (*client)[db_name];
      mongocxx::collection coll = db[coll_name];
      auto query_builder = bsoncxx::builder::stream::document{};
      bsoncxx::oid db_id(key);
      query_builder << "_id" << db_id;
      auto result = coll.find_one(query_builder << bsoncxx::builder::stream::finalize);
      // TO-DO: Not returning any values
      // TO-DO: result still resolves to true below, which causes an exception
      //        when calling bson_to_obj3
      if (result) {
        auto view = result->view();
        ObjectInterface *obj = object_factory.build_object();
        bson_to_obj3(view, obj);
        response->add_object(obj);
      } else {
        response->set_error_code(PROCESSING_ERROR);
        response->set_error_message(std::string("No results returned from query"));
      }
    } catch (mongocxx::exception& me) {
      logger.error("Mongo Exception Encountered");
      logger.error(me.what());
      response->set_error_message(std::string(me.what()));
      break;
    } catch (std::exception& e) {
      logger.error("Exception executing Mongo Query");
      logger.error(e.what());
      failures++;
      failure = true;
    }
    if (failure) set_new_connection();
    retries++;
    if (!failure) break;
  }
}

void DatabaseManager::query(ObjectListInterface *response, ObjectInterface *obj, int max_results) {
  logger.debug("Attempting to query Mongo");
  int retries = 0;
  while (retries < max_retries) {
    // Initialize the connection for the first time
    init();
    bool failure = false;
    try {
      Poco::ScopedReadRWLock scoped_lock(conn_usage_lock);
      // Find the DB and Collection we're going to write into
      auto client = pool->acquire();
      mongocxx::database db = (*client)[db_name];
      mongocxx::collection coll = db[coll_name];
      auto query_builder = bsoncxx::builder::stream::document{};
      build_create_doc(query_builder, obj);
      auto results = coll.find(query_builder << bsoncxx::builder::stream::finalize);
      int results_processed = 0;
      for (auto result : results) {
        if (results_processed > max_results) break;
        ObjectInterface *obj = object_factory.build_object();
        bson_to_obj3(result, obj);
        response->add_object(obj);
        results_processed++;
      }
    } catch (mongocxx::exception& me) {
      logger.error("Mongo Exception Encountered");
      logger.error(me.what());
      response->set_error_message(std::string(me.what()));
      break;
    } catch (std::exception& e) {
      logger.error("Exception executing Mongo Query");
      logger.error(e.what());
      failures++;
      failure = true;
    }
    if (failure) set_new_connection();
    retries++;
    if (!failure) break;
  }
}

void DatabaseManager::delete_object(DatabaseResponse& response, std::string& key) {
  int retries = 0;
  while (retries < max_retries) {
    // Initialize the connection for the first time
    init();
    bool failure = false;
    try {
      Poco::ScopedReadRWLock scoped_lock(conn_usage_lock);
      // Find the DB and Collection we're going to write into
      auto client = pool->acquire();
      mongocxx::database db = (*client)[db_name];
      mongocxx::collection coll = db[coll_name];
      auto query_builder = bsoncxx::builder::stream::document{};
      query_builder << "_id" << bsoncxx::oid(key.c_str(), key.length());
      auto result = coll.delete_one(query_builder << bsoncxx::builder::stream::finalize);
      if (result) {
        if (result->deleted_count() > 0) {
          logger.debug("Document Successfully removed from DB");
          response.success = true;
        } else {
          response.error_message = std::string("No Documents Modified in DB");
        }
      } else {
        response.error_message = std::string("Null Result returned from DB");
      }
    } catch (mongocxx::exception& me) {
      logger.error("Mongo Exception Encountered");
      logger.error(me.what());
      response.error_message = std::string(me.what());
      break;
    } catch (std::exception& e) {
      logger.error("Exception executing Mongo Query");
      logger.error(e.what());
      failures++;
      failure = true;
    }
    if (failure) set_new_connection();
    retries++;
    if (!failure) break;
  }
}
