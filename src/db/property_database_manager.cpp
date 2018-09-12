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

#include "include/property_database_manager.h"

void PropertyDatabaseManager::build_create_prop_doc(bsoncxx::builder::stream::document &builder, PropertyInterface *obj) {
  // Creation Document
  builder << "name" << obj->get_name();
  builder << "parent" << obj->get_parent();
  builder << "scene" << obj->get_scene();
  builder << "asset_sub_id" << obj->get_asset_sub_id();
  builder << "frame" << obj->get_frame();
  builder << "timestamp" << obj->get_timestamp();
  auto value_outer_doc = bsoncxx::builder::stream::document{};
  for (int i = 0; i < 4; i++) {
    auto val_doc = bsoncxx::builder::stream::document{};
    // Write the Value
    if (i < obj->num_values()) {
      val_doc << "active" << true;
      val_doc << "value" << obj->get_value(i);
      // Write the Graph Handle
      CoreDatabaseManager::add_graph_handle_to_document(val_doc, obj->get_handle(i));
    } else {
      val_doc << "active" << false;
      val_doc << "value" << 0.0;
    }
    // Write the document into the outer doc
    if (i == 0) value_outer_doc << "w" << val_doc;
    if (i == 1) value_outer_doc << "x" << val_doc;
    if (i == 2) value_outer_doc << "y" << val_doc;
    if (i == 3) value_outer_doc << "z" << val_doc;
  }
  builder << "values" << value_outer_doc;
}

void PropertyDatabaseManager::build_query_prop_doc(bsoncxx::builder::stream::document &builder, PropertyInterface *obj) {
  // Creation Document
  if (!(obj->get_name().empty())) {
    builder << "name" << obj->get_name();
  }
  if (!(obj->get_parent().empty())) {
    builder << "parent" << obj->get_parent();
  }
  if (!(obj->get_asset_sub_id().empty())) {
    builder << "asset_sub_id" << obj->get_asset_sub_id();
  }
  if (!(obj->get_scene().empty())) {
    builder << "scene" << obj->get_scene();
  }
  if (obj->get_frame() > -999) {
    builder << "frame" << obj->get_frame();
  }
  if (obj->get_timestamp() > -999) {
    builder << "timestamp" << obj->get_timestamp();
  }
}

void PropertyDatabaseManager::build_update_prop_doc(bsoncxx::builder::stream::document &builder, PropertyInterface *obj, bool is_append_operation) {
  // Update Document
  auto set_doc = bsoncxx::builder::stream::document{};
  if (!(obj->get_name().empty())) {
    set_doc << "name" << obj->get_name();
  }
  if (!(obj->get_parent().empty())) {
    set_doc << "parent" << obj->get_parent();
  }
  if (!(obj->get_asset_sub_id().empty())) {
    set_doc << "asset_sub_id" << obj->get_asset_sub_id();
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
  auto value_outer_doc = bsoncxx::builder::stream::document{};
  for (int i = 0; i < 4; i++) {
    auto val_doc = bsoncxx::builder::stream::document{};
    // Write the Value
    if (i < obj->num_values()) {
      val_doc << "active" << true;
      val_doc << "value" << obj->get_value(i);
      // Write the Graph Handle
      CoreDatabaseManager::add_graph_handle_to_document(val_doc, obj->get_handle(i));
    } else {
      val_doc << "active" << false;
      val_doc << "value" << 0.0;
    }
    // Write the document into the outer doc
    if (i == 0) value_outer_doc << "w" << val_doc;
    if (i == 1) value_outer_doc << "x" << val_doc;
    if (i == 2) value_outer_doc << "y" << val_doc;
    if (i == 3) value_outer_doc << "z" << val_doc;
  }
  set_doc << "values" << value_outer_doc;
  builder << "$set" << set_doc;
}

void PropertyDatabaseManager::prop_transaction(DatabaseResponse &response, PropertyInterface *obj, std::string& key, int transaction_type, bool is_append_operation) {
  int retries = 0;
  while (retries < max_retries) {
    // Initialize the connection for the first time
    init();
    bool failure = false;
    try {
      Poco::ScopedReadRWLock scoped_lock(CoreDatabaseManager::get_lock());
      // Find the DB and Collection we're going to write into
      auto client = CoreDatabaseManager::get_connection_pool()->acquire();
      mongocxx::database db = (*client)[db_name];
      mongocxx::collection coll = db[coll_name];
      // Use a BSON Builder to construct the main document
      auto builder = bsoncxx::builder::stream::document{};
      if (transaction_type == _DB_MONGO_INSERT_) {
        build_create_prop_doc(builder, obj);
      } else if (transaction_type == _DB_MONGO_UPDATE_) {
        build_update_prop_doc(builder, obj, is_append_operation);
      }
      // Execute an Insert Transaction
      if (transaction_type == _DB_MONGO_INSERT_) {
        bsoncxx::document::value doc_value = \
          builder << bsoncxx::builder::stream::finalize;
        CoreDatabaseManager::insert_doc(coll, doc_value, key, response);
      // Execute an Update Transaction
      } else if (transaction_type == _DB_MONGO_UPDATE_) {
        auto query_builder = bsoncxx::builder::stream::document{};
        bsoncxx::oid db_id(key);
        query_builder << "_id" << db_id;
        auto result = coll.update_one(query_builder << bsoncxx::builder::stream::finalize, \
            builder << bsoncxx::builder::stream::finalize);
        if (result) {
          if (result->modified_count() > 0) {
            logger.debug("Document Successfully updated in DB");
            response.success = true;
          } else {
            response.error_code = NOT_FOUND;
            response.error_message = std::string("No Documents Modified in DB");
          }
        } else {
          response.error_code = PROCESSING_ERROR;
          response.error_message = std::string("Null Result returned from DB");
        }
      }
    } catch (mongocxx::exception& me) {
      logger.error("Mongo Exception Encountered");
      logger.error(me.what());
      response.error_message = std::string(me.what());
      response.error_code = PROCESSING_ERROR;
      break;
    } catch (std::exception& e) {
      logger.error("Exception executing Mongo Query");
      logger.error(e.what());
      CoreDatabaseManager::add_failure();
      failure = true;
    }
    if (failure) CoreDatabaseManager::set_new_connection();
    retries++;
    if (response.success && !(failure)) {
      break;
    }
  }
}

void PropertyDatabaseManager::bson_to_prop(bsoncxx::document::view& result, PropertyInterface *obj) {
  // Parse basic values
  bsoncxx::document::element name_element = result["name"];
  obj->set_name(name_element.get_utf8().value.to_string());
  bsoncxx::document::element parent_element = result["parent"];
  obj->set_parent(parent_element.get_utf8().value.to_string());
  bsoncxx::document::element asset_sub_id_element = result["asset_sub_id"];
  obj->set_asset_sub_id(asset_sub_id_element.get_utf8().value.to_string());
  bsoncxx::document::element scene_element = result["scene"];
  obj->set_scene(scene_element.get_utf8().value.to_string());
  bsoncxx::document::element frame_element = result["frame"];
  obj->set_frame(frame_element.get_int32().value);
  bsoncxx::document::element timestamp_element = result["timestamp"];
  obj->set_timestamp(timestamp_element.get_int32().value);
  // Parse the values array
  auto values_element = result["values"];
  const char* key_values[4] = {"w", "x", "y", "z"};
  for (int i = 0; i < 4; i++) {
    // Get the inner value object
    auto value_elt = values_element[key_values[i]];
    auto active_elt = value_elt["active"];
    if (active_elt.get_bool().value) {
      // Get the actual double value associated
      auto val_value_elt = value_elt["value"];
      obj->add_value(val_value_elt.get_double().value);
      CoreDatabaseManager::get_handle_from_element(value_elt, obj->get_handle(i));
    }
  }
}

void PropertyDatabaseManager::get_property(PropertyListInterface *response, std::string& key) {
  logger.debug("Attempting to retrieve object from Mongo");
  int retries = 0;
  while (retries < max_retries) {
    // Initialize the connection for the first time
    init();
    bool failure = false;
    try {
      Poco::ScopedReadRWLock scoped_lock(CoreDatabaseManager::get_lock());
      // Find the DB and Collection we're going to write into
      auto client = CoreDatabaseManager::get_connection_pool()->acquire();
      mongocxx::database db = (*client)[db_name];
      mongocxx::collection coll = db[coll_name];
      auto query_builder = bsoncxx::builder::stream::document{};
      bsoncxx::oid db_id(key);
      query_builder << "_id" << db_id;
      auto result = coll.find_one(query_builder << bsoncxx::builder::stream::finalize);
      if (result) {
        auto view = result->view();
        PropertyInterface *obj = object_factory.build_property();
        bson_to_prop(view, obj);
        response->add_prop(obj);
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
      CoreDatabaseManager::add_failure();
      failure = true;
    }
    if (failure) CoreDatabaseManager::set_new_connection();
    retries++;
    if (!failure) break;
  }
}

void PropertyDatabaseManager::property_query(PropertyListInterface *response, PropertyInterface *obj, int max_results) {
  logger.debug("Attempting to query Mongo");
  int retries = 0;
  while (retries < max_retries) {
    // Initialize the connection for the first time
    init();
    bool failure = false;
    try {
      Poco::ScopedReadRWLock scoped_lock(CoreDatabaseManager::get_lock());
      // Find the DB and Collection we're going to write into
      auto client = CoreDatabaseManager::get_connection_pool()->acquire();
      mongocxx::database db = (*client)[db_name];
      mongocxx::collection coll = db[coll_name];
      auto query_builder = bsoncxx::builder::stream::document{};
      build_query_prop_doc(query_builder, obj);
      auto results = coll.find(query_builder << bsoncxx::builder::stream::finalize);
      int results_processed = 0;
      for (auto result : results) {
        if (results_processed > max_results) break;
        PropertyInterface *obj = object_factory.build_property();
        bson_to_prop(result, obj);
        response->add_prop(obj);
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
      CoreDatabaseManager::add_failure();
      failure = true;
    }
    if (failure) CoreDatabaseManager::set_new_connection();
    retries++;
    if (!failure) break;
  }
}

void PropertyDatabaseManager::delete_property(DatabaseResponse& response, std::string& key) {
  int retries = 0;
  while (retries < max_retries) {
    // Initialize the connection for the first time
    init();
    bool failure = false;
    try {
      Poco::ScopedReadRWLock scoped_lock(CoreDatabaseManager::get_lock());
      // Find the DB and Collection we're going to write into
      auto client = CoreDatabaseManager::get_connection_pool()->acquire();
      mongocxx::database db = (*client)[db_name];
      mongocxx::collection coll = db[coll_name];
      auto query_builder = bsoncxx::builder::stream::document{};
      bsoncxx::oid db_id(key);
      query_builder << "_id" << db_id;
      auto result = coll.delete_one(query_builder << bsoncxx::builder::stream::finalize);
      if (result) {
        if (result->deleted_count() > 0) {
          logger.debug("Document Successfully removed from DB");
          response.success = true;
        } else {
          response.error_message = std::string("No Documents Modified in DB");
          response.error_code = NOT_FOUND;
        }
      } else {
        response.error_message = std::string("Null Result returned from DB");
        response.error_code = PROCESSING_ERROR;
      }
    } catch (mongocxx::exception& me) {
      logger.error("Mongo Exception Encountered");
      logger.error(me.what());
      response.error_message = std::string(me.what());
      response.error_code = PROCESSING_ERROR;
      break;
    } catch (std::exception& e) {
      logger.error("Exception executing Mongo Query");
      logger.error(e.what());
      CoreDatabaseManager::add_failure();
      failure = true;
    }
    if (failure) CoreDatabaseManager::set_new_connection();
    retries++;
    if (!failure) break;
  }
}
