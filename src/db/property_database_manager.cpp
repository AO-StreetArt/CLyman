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
  build_prop_doc(builder, obj, true);
}

void PropertyDatabaseManager::build_prop_doc(bsoncxx::builder::stream::document &builder, PropertyInterface *obj, bool include_actions) {
  // Creation Document
  builder << "name" << obj->get_name();
  builder << "parent" << obj->get_parent();
  builder << "scene" << obj->get_scene();
  builder << "asset_sub_id" << obj->get_asset_sub_id();

  // Write Values entries
  auto value_outer_doc = bsoncxx::builder::stream::document{};
  for (int i = 0; i < 4; i++) {
    auto val_doc = bsoncxx::builder::stream::document{};
    // Write the Value
    if (i < obj->num_values()) {
      val_doc << "active" << true;
      val_doc << "value" << obj->get_value(i);
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

  // Write Actions entries
  if (include_actions) {
    auto actions_outer_doc = bsoncxx::builder::stream::document{};
    for (auto action_itr = obj->get_actions()->begin(); action_itr != obj->get_actions()->end(); ++action_itr) {
      auto action_doc = bsoncxx::builder::stream::document{};
      action_doc << "name" << action_itr->first;
      action_doc << "description" << action_itr->second->get_description();
      action_doc << "owner" << action_itr->second->get_owner();
      auto frames_outer_doc = bsoncxx::builder::stream::document{};
      for (auto frame_itr = action_itr->second->get_keyframes()->begin(); frame_itr != action_itr->second->get_keyframes()->end(); ++frame_itr) {
        auto frame_doc = bsoncxx::builder::stream::document{};

        // Add frame elements
        frame_doc << "owner" << frame_itr->second->get_owner();
        frame_doc << "frame" << frame_itr->second->get_frame();

        // Write Values entries
        auto value_outer_doc = bsoncxx::builder::stream::document{};
        for (int i = 0; i < 4; i++) {
          auto val_doc = bsoncxx::builder::stream::document{};
          // Write the Value
          if (i < frame_itr->second->num_values()) {
            val_doc << "active" << true;
            val_doc << "value" << frame_itr->second->get_value(i);
            // Write the Graph Handle
            if (frame_itr->second->num_handles() > i) {
              CoreDatabaseManager::add_graph_handle_to_document(val_doc, frame_itr->second->get_handle(i));
            }
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

        // Add the frame doc to the outer frame doc
        frames_outer_doc << std::to_string(frame_itr->first) << frame_doc;
      }

      // Add the frames outer doc to the action doc
      action_doc << "frames" << frames_outer_doc;

      // Add the action document to the outer actions document
      actions_outer_doc << action_itr->first << action_doc;
    }
    builder << "actions" << actions_outer_doc;
  }
}

void PropertyDatabaseManager::build_query_prop_doc(bsoncxx::builder::stream::document &builder, PropertyInterface *obj) {
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
}

void PropertyDatabaseManager::build_update_prop_doc(bsoncxx::builder::stream::document &builder, PropertyInterface *obj, bool is_append_operation) {
  // Update Document
  auto set_doc = bsoncxx::builder::stream::document{};
  build_prop_doc(set_doc, obj, false);
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
        try {
          bsoncxx::oid db_id(key);
          query_builder << "_id" << db_id;
        } catch (std::exception& e) {
          logger.error("Exception parsing Mongo OID");
          logger.error(e.what());
          response.error_message = std::string(e.what());
          response.error_code = PROCESSING_ERROR;
          break;
        }
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
  auto key_element = result["_id"];
  if (key_element) {
    obj->set_key(key_element.get_oid().value.to_string());
  }
  auto name_element = result["name"];
  if (name_element) {
    obj->set_name(name_element.get_utf8().value.to_string());
  }
  auto parent_element = result["parent"];
  if (parent_element) {
    obj->set_parent(parent_element.get_utf8().value.to_string());
  }
  auto asset_sub_id_element = result["asset_sub_id"];
  if (asset_sub_id_element) {
    obj->set_asset_sub_id(asset_sub_id_element.get_utf8().value.to_string());
  }
  auto scene_element = result["scene"];
  if (scene_element) {
    obj->set_scene(scene_element.get_utf8().value.to_string());
  }

  // Parse the values array
  auto values_element = result["values"];
  if (values_element) {
    const char* key_values[4] = {"w", "x", "y", "z"};
    for (int i = 0; i < 4; i++) {
      // Get the inner value object
      auto value_elt = values_element[key_values[i]];
      auto active_elt = value_elt["active"];
      if (active_elt && active_elt.get_bool().value) {
        // Get the actual double value associated
        auto val_value_elt = value_elt["value"];
        if (val_value_elt) {
          obj->add_value(val_value_elt.get_double().value);
        }
      }
    }
  }

  // Parse the actions array
  for (auto element : result["actions"].get_array().value) {
    AnimationAction<PropertyFrame> *new_action = new AnimationAction<PropertyFrame>();

    if (element.type() == bsoncxx::type::k_document) {
      auto action_doc = element.get_document().view();
      auto name_element = action_doc["name"];
      if (name_element && name_element.type() == bsoncxx::type::k_utf8) {
        auto name_elt_val_str = name_element.get_utf8().value.to_string();
        new_action->set_name(name_elt_val_str);
      }
      auto desc_element = action_doc["description"];
      if (desc_element && desc_element.type() == bsoncxx::type::k_utf8) {
        auto desc_elt_val_str = desc_element.get_utf8().value.to_string();
        new_action->set_description(desc_elt_val_str);
      }
      auto owner_element = action_doc["owner"];
      if (owner_element && owner_element.type() == bsoncxx::type::k_utf8) {
        auto owner_elt_val_str = owner_element.get_utf8().value.to_string();
        new_action->set_owner(owner_elt_val_str);
      }
      // Iterate over keyframes
      auto frames_element = action_doc["keyframes"];
      if (frames_element && frames_element.type() == bsoncxx::type::k_array) {
        for (auto frame_elt : frames_element.get_array().value) {
          PropertyFrame *new_frame = new PropertyFrame();

          // Add frame attributes
          auto frame_int_elt = frame_elt["frame"];
          if (frame_int_elt && frame_int_elt.type() == bsoncxx::type::k_int32) {
            new_frame->set_frame(frame_int_elt.get_int32().value);
          }
          auto frame_owner_elt = frame_elt["owner"];
          if (frame_owner_elt && frame_owner_elt.type() == bsoncxx::type::k_utf8) {
            new_frame->set_owner(frame_owner_elt.get_utf8().value.to_string());
          }

          // Parse the values array
          auto frame_values_element = frame_elt["values"];
          if (frame_values_element) {
            const char* key_values[4] = {"w", "x", "y", "z"};
            for (int i = 0; i < 4; i++) {
              // Get the inner value object
              auto frame_value_elt = values_element[key_values[i]];
              auto frame_active_elt = frame_value_elt["active"];
              if (frame_active_elt && frame_active_elt.get_bool().value) {
                // Get the actual double value associated
                auto fval_value_elt = frame_value_elt["value"];
                if (fval_value_elt) {
                  new_frame->add_value(fval_value_elt.get_double().value);
                }
                CoreDatabaseManager::get_handle_from_element(frame_value_elt, new_frame->get_handle(i));
              }
            }
          }

          new_action->add_keyframe(new_frame->get_frame(), new_frame);
        }
      }
    }
    obj->add_action(new_action->get_name(), new_action);
  }
}

void PropertyDatabaseManager::get_property(PropertyListInterface *response, std::string& key) {
  logger.debug("Attempting to retrieve object from Mongo");
  int retries = 0;
  // Initialize the connection for the first time
  init();
  response->set_num_records(0);
  while (retries < max_retries) {
    bool failure = false;
    try {
      bsoncxx::oid db_id(key);
      try {
        Poco::ScopedReadRWLock scoped_lock(CoreDatabaseManager::get_lock());
        // Find the DB and Collection we're going to write into
        auto client = CoreDatabaseManager::get_connection_pool()->acquire();
        mongocxx::database db = (*client)[db_name];
        mongocxx::collection coll = db[coll_name];
        auto query_builder = bsoncxx::builder::stream::document{};
        query_builder << "_id" << db_id;
        auto result = coll.find_one(query_builder << bsoncxx::builder::stream::finalize);
        if (result) {
          auto view = result->view();
          PropertyInterface *obj = object_factory.build_property();
          bson_to_prop(view, obj);
          response->add_prop(obj);
          response->set_num_records(1);
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
    } catch (std::exception& e) {
      logger.error("Exception parsing Object ID");
      logger.error(e.what());
      response->set_error_message(std::string(e.what()));
      break;
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
