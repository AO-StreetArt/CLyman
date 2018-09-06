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

#include "include/obj3_database_manager.h"

void ObjectDatabaseManager::add_handles_to_doc(bsoncxx::builder::stream::document &builder, AnimationFrameInterface *aframe) {
    auto trans_handle_array = bsoncxx::builder::stream::array{};
    if (aframe) {
      for (int k = 0; k < 3; k++) {
        auto trans_handle_doc = bsoncxx::builder::stream::document{};
        CoreDatabaseManager::add_graph_handle_to_document(trans_handle_doc, \
            aframe->get_translation(k));
        trans_handle_array << trans_handle_doc;
      }
    }
    builder << "translation_handle" << trans_handle_array;
    auto rot_handle_array = bsoncxx::builder::stream::array{};
    if (aframe) {
      for (int m = 0; m < 4; m++) {
        auto rot_handle_doc = bsoncxx::builder::stream::document{};
        CoreDatabaseManager::add_graph_handle_to_document(rot_handle_doc, \
            aframe->get_translation(m));
        rot_handle_array << rot_handle_doc;
      }
    }
    builder << "rotation_handle" << rot_handle_array;
    auto scale_handle_array = bsoncxx::builder::stream::array{};
    if (aframe) {
      for (int n = 0; n < 3; n++) {
        auto scale_handle_doc = bsoncxx::builder::stream::document{};
        CoreDatabaseManager::add_graph_handle_to_document(scale_handle_doc, \
            aframe->get_scale(n));
        scale_handle_array << scale_handle_doc;
      }
    }
    builder << "scale_handle" << scale_handle_array;
}

void ObjectDatabaseManager::build_create_doc(bsoncxx::builder::stream::document &builder, ObjectInterface *obj) {
  // Creation Document
  builder << "name" << obj->get_name();
  builder << "parent" << obj->get_parent();
  builder << "asset_sub_id" << obj->get_asset_sub_id();
  builder << "type" << obj->get_type();
  builder << "subtype" << obj->get_subtype();
  builder << "owner" << obj->get_owner();
  builder << "scene" << obj->get_scene();
  builder << "frame" << obj->get_frame();
  builder << "timestamp" << obj->get_timestamp();

  // Add the Assets
  auto asset_array = bsoncxx::builder::stream::array{};
  for (int i = 0; i < obj->num_assets(); i++) {
    asset_array << obj->get_asset(i);
  }
  builder << "assets" << asset_array;

  // Add the Transform
  auto transform_array = bsoncxx::builder::stream::array{};
  for (int j = 0; j < 4; j++) {
    for (int k = 0; k < 4; k++) {
      if (j < 4 && k < 4) {
        transform_array << std::to_string(obj->get_transform()->get_transform_element(j, k));
      }
    }
  }
  builder << "transform" << transform_array;

  // Add the AnimationFrame
  add_handles_to_doc(builder, obj->get_animation_frame());
}

void ObjectDatabaseManager::build_query_doc(bsoncxx::builder::stream::document &builder, ObjectInterface *obj) {
  // Creation Document
  if (!(obj->get_name().empty())) {
    builder << "name" << obj->get_name();
  }
  if (!(obj->get_type().empty())) {
    builder << "type" << obj->get_type();
  }
  if (!(obj->get_parent().empty())) {
    builder << "parent" << obj->get_parent();
  }
  if (!(obj->get_asset_sub_id().empty())) {
    builder << "asset_sub_id" << obj->get_asset_sub_id();
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
  if (obj->num_assets() == 1) {
    // If we have only one element, then search for docs
    // that contain the specified asset
    builder << "assets" << obj->get_asset(0);
  } else if (obj->num_assets() > 1) {
    // If we have more than one element, then search for
    // docs that match exactly the specified assets
    auto asset_array = bsoncxx::builder::stream::array{};
    for (int i = 0; i < obj->num_assets(); i++) {
      asset_array << obj->get_asset(i);
    }
    builder << "assets" << asset_array;
  }
}

void ObjectDatabaseManager::build_update_doc(bsoncxx::builder::stream::document &builder, ObjectInterface *obj, bool is_append_operation) {
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

  // Add the AnimationFrame
  if (obj->get_animation_frame()) {
    add_handles_to_doc(set_doc, obj->get_animation_frame());
  }
  builder << "$set" << set_doc;
  auto push_doc = bsoncxx::builder::stream::document{};
  auto each_doc = bsoncxx::builder::stream::document{};
  auto asset_array = bsoncxx::builder::stream::array{};
  for (int i = 0; i < obj->num_assets(); i++) {
    asset_array << obj->get_asset(i);
  }
  each_doc << "$each" << asset_array;
  push_doc << "assets" << each_doc;
  std::string update_opt_key;
  if (is_append_operation) {
    update_opt_key = "$push";
  } else {
    update_opt_key = "$pull";
  }
  builder << update_opt_key << push_doc;
}

void ObjectDatabaseManager::transaction(DatabaseResponse &response, ObjectInterface *obj, std::string& key, int transaction_type, bool is_append_operation) {
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
        CoreDatabaseManager::insert_doc(coll, doc_value, key, response);
      // Execute an Update Transaction
      } else if (transaction_type == _DB_MONGO_UPDATE_ \
          || transaction_type == _DB_MONGO_LOCK_ \
          || transaction_type == _DB_MONGO_UNLOCK_) {
        auto query_builder = bsoncxx::builder::stream::document{};
        bsoncxx::oid db_id(key);
        query_builder << "_id" << db_id;
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

void ObjectDatabaseManager::bson_to_obj3(bsoncxx::document::view& result, ObjectInterface *obj) {
  // Parse basic values
  bsoncxx::document::element name_element = result["name"];
  obj->set_name(name_element.get_utf8().value.to_string());
  bsoncxx::document::element parent_element = result["parent"];
  obj->set_parent(parent_element.get_utf8().value.to_string());
  bsoncxx::document::element asset_sub_id_element = result["asset_sub_id"];
  obj->set_asset_sub_id(asset_sub_id_element.get_utf8().value.to_string());
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
  int assets_array_size = std::distance(assets_view.begin(), assets_view.end());
  for (int i = 0; i < assets_array_size; i++) {
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
      std::string transform_elt_string = transform_elt.get_utf8().value.to_string();
      obj->get_transform()->set_transform_element(i, j, \
          std::stof(transform_elt_string));
    }
  }
  // Parse the Animation Frame
  bsoncxx::document::element aft_element = result["translation_frame"];
  bsoncxx::array::view tframe_array_view = aft_element.get_array().value;
  int tframe_array_size = std::distance(tframe_array_view.begin(), tframe_array_view.end());
  if (tframe_array_size > 0 && (!obj->get_animation_frame())) {
      obj->set_animation_frame(new AnimationFrame);
  }
  for (int i = 0; i < tframe_array_size; i++) {
      auto tframe_elt = tframe_array_view[i];
      CoreDatabaseManager::get_handle_from_element(tframe_elt, obj->get_animation_frame()->get_translation(i));
  }
  bsoncxx::document::element afr_element = result["rotation_frame"];
  bsoncxx::array::view rframe_array_view = afr_element.get_array().value;
  int rframe_array_size = std::distance(rframe_array_view.begin(), rframe_array_view.end());
  if (rframe_array_size > 0 && (!obj->get_animation_frame())) {
      obj->set_animation_frame(new AnimationFrame);
  }
  for (int i = 0; i < rframe_array_size; i++) {
      auto rframe_elt = rframe_array_view[i];
      CoreDatabaseManager::get_handle_from_element(rframe_elt, obj->get_animation_frame()->get_translation(i));
  }
  bsoncxx::document::element afs_element = result["scale_frame"];
  bsoncxx::array::view sframe_array_view = afs_element.get_array().value;
  int sframe_array_size = std::distance(sframe_array_view.begin(), sframe_array_view.end());
  if (sframe_array_size > 0 && (!obj->get_animation_frame())) {
      obj->set_animation_frame(new AnimationFrame);
  }
  for (int i = 0; i < sframe_array_size; i++) {
      auto sframe_elt = sframe_array_view[i];
      CoreDatabaseManager::get_handle_from_element(sframe_elt, obj->get_animation_frame()->get_translation(i));
  }
}

void ObjectDatabaseManager::get_object(ObjectListInterface *response, std::string& key) {
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
      CoreDatabaseManager::add_failure();
      failure = true;
    }
    if (failure) CoreDatabaseManager::set_new_connection();
    retries++;
    if (!failure) break;
  }
}

void ObjectDatabaseManager::query(ObjectListInterface *response, ObjectInterface *obj, int max_results) {
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
      build_query_doc(query_builder, obj);
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
      CoreDatabaseManager::add_failure();
      failure = true;
    }
    if (failure) CoreDatabaseManager::set_new_connection();
    retries++;
    if (!failure) break;
  }
}

void ObjectDatabaseManager::delete_object(DatabaseResponse& response, std::string& key) {
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
      CoreDatabaseManager::add_failure();
      failure = true;
    }
    if (failure) CoreDatabaseManager::set_new_connection();
    retries++;
    if (!failure) break;
  }
}
