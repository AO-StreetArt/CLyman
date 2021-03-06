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

void ObjectDatabaseManager::add_handles_to_doc(bsoncxx::builder::stream::document &builder, ObjectFrame *aframe) {
    auto aframe_doc = bsoncxx::builder::stream::document{};
    if (aframe) {
      aframe_doc << "active" << true;
      auto trans_handle_outer_doc = bsoncxx::builder::stream::document{};
      for (int k = 0; k < 3; k++) {
        auto trans_handle_doc = bsoncxx::builder::stream::document{};
        CoreDatabaseManager::add_graph_handle_to_document(trans_handle_doc, \
            aframe->get_translation(k));
        if (k == 0) trans_handle_outer_doc << "x" << trans_handle_doc;
        if (k == 1) trans_handle_outer_doc << "y" << trans_handle_doc;
        if (k == 2) trans_handle_outer_doc << "z" << trans_handle_doc;
      }
      aframe_doc << "translation_handle" << trans_handle_outer_doc;
      auto rot_handle_outer_doc = bsoncxx::builder::stream::document{};
      for (int m = 0; m < 4; m++) {
        auto rot_handle_doc = bsoncxx::builder::stream::document{};
        CoreDatabaseManager::add_graph_handle_to_document(rot_handle_doc, \
            aframe->get_rotation(m));
        if (m == 0) rot_handle_outer_doc << "w" << rot_handle_doc;
        if (m == 1) rot_handle_outer_doc << "x" << rot_handle_doc;
        if (m == 2) rot_handle_outer_doc << "y" << rot_handle_doc;
        if (m == 3) rot_handle_outer_doc << "z" << rot_handle_doc;
      }
      aframe_doc << "rotation_handle" << rot_handle_outer_doc;
      auto scale_handle_outer_doc = bsoncxx::builder::stream::document{};
      for (int n = 0; n < 3; n++) {
        auto scale_handle_doc = bsoncxx::builder::stream::document{};
        CoreDatabaseManager::add_graph_handle_to_document(scale_handle_doc, \
            aframe->get_scale(n));
        if (n == 0) scale_handle_outer_doc << "x" << scale_handle_doc;
        if (n == 1) scale_handle_outer_doc << "y" << scale_handle_doc;
        if (n == 2) scale_handle_outer_doc << "z" << scale_handle_doc;
      }
      aframe_doc << "scale_handle" << scale_handle_outer_doc;
    } else {
      aframe_doc << "active" << false;
    }
    builder << "animation_frame" << aframe_doc;
}

void ObjectDatabaseManager::add_obj_frame_to_doc(bsoncxx::builder::stream::document &builder, ObjectFrame *aframe) {
  // Add frame elements
  builder << "owner" << aframe->get_owner();
  builder << "frame" << aframe->get_frame();

  // Add the Transform
  auto transform_doc = bsoncxx::builder::stream::document{};
  std::string key_values[16] = {"r1_c1", "r1_c2", "r1_c3", "r1_c4", \
      "r2_c1", "r2_c2", "r2_c3", "r2_c4", "r3_c1", "r3_c2", "r3_c3", "r3_c4", \
      "r4_c1", "r4_c2", "r4_c3", "r4_c4"};
  for (int j = 0; j < 4; j++) {
    for (int k = 0; k < 4; k++) {
      int index = (4 * j) + k;
      transform_doc << key_values[index] << aframe->get_transform()->get_transform_element(j, k);
    }
  }
  builder << "transform" << transform_doc;

  // Write animation frame
  add_handles_to_doc(builder, aframe);
}

void ObjectDatabaseManager::add_obj_action_to_doc(bsoncxx::builder::stream::document &action_doc, AnimationAction<ObjectFrame> *action) {
  action_doc << "description" << action->get_description();
  action_doc << "owner" << action->get_owner();
  action_doc << "name" << action->get_name();
  auto frames_outer_doc = bsoncxx::builder::stream::document{};
  for (auto frame_itr = action->get_keyframes()->begin(); frame_itr != action->get_keyframes()->end(); ++frame_itr) {
    auto frame_doc = bsoncxx::builder::stream::document{};
    frame_doc << "frame" << frame_itr->first;

    add_obj_frame_to_doc(frame_doc, frame_itr->second);

    // Add the frame doc to the outer frame doc
    frames_outer_doc << std::to_string(frame_itr->first) << frame_doc;
  }

  // Add the frames outer doc to the action doc
  action_doc << "frames" << frames_outer_doc;
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

  // Add the Assets
  auto asset_array = bsoncxx::builder::stream::array{};
  for (int i = 0; i < obj->num_assets(); i++) {
    asset_array << obj->get_asset(i);
  }
  builder << "assets" << asset_array;

  // Add the Transform
  auto transform_doc = bsoncxx::builder::stream::document{};
  std::string key_values[16] = {"r1_c1", "r1_c2", "r1_c3", "r1_c4", \
      "r2_c1", "r2_c2", "r2_c3", "r2_c4", "r3_c1", "r3_c2", "r3_c3", "r3_c4", \
      "r4_c1", "r4_c2", "r4_c3", "r4_c4"};
  for (int j = 0; j < 4; j++) {
    for (int k = 0; k < 4; k++) {
      int index = (4 * j) + k;
      transform_doc << key_values[index] << obj->get_transform()->get_transform_element(j, k);
    }
  }
  builder << "transform" << transform_doc;

  // Write Actions entries
  auto actions_outer_doc = bsoncxx::builder::stream::document{};
  for (auto action_itr = obj->get_actions()->begin(); action_itr != obj->get_actions()->end(); ++action_itr) {
    auto action_doc = bsoncxx::builder::stream::document{};
    action_doc << "name" << action_itr->first;

    add_obj_action_to_doc(action_doc, action_itr->second);

    // Add the action document to the outer actions document
    actions_outer_doc << action_itr->first << action_doc;
  }
  builder << "actions" << actions_outer_doc;
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
  if (obj->has_transform()) {
    // Add the Transform
    auto transform_doc = bsoncxx::builder::stream::document{};
    std::string key_values[16] = {"r1_c1", "r1_c2", "r1_c3", "r1_c4", \
        "r2_c1", "r2_c2", "r2_c3", "r2_c4", "r3_c1", "r3_c2", "r3_c3", "r3_c4", \
        "r4_c1", "r4_c2", "r4_c3", "r4_c4"};
    for (int j = 0; j < 4; j++) {
      for (int k = 0; k < 4; k++) {
        int index = (4 * j) + k;
        transform_doc << key_values[index] << obj->get_transform()->get_transform_element(j, k);
      }
    }
    set_doc << "transform" << transform_doc;
  }
  builder << "$set" << set_doc;

  // Add the Asset Array
  auto asset_array = bsoncxx::builder::stream::array{};
  for (int i = 0; i < obj->num_assets(); i++) {
    asset_array << obj->get_asset(i);
  }
  if (is_append_operation) {
    auto push_doc = bsoncxx::builder::stream::document{};
    auto each_doc = bsoncxx::builder::stream::document{};
    each_doc << "$each" << asset_array;
    push_doc << "assets" << each_doc;
    builder << "$push" << push_doc;
  } else {
    auto pull_doc = bsoncxx::builder::stream::document{};
    auto in_doc = bsoncxx::builder::stream::document{};
    pull_doc << "$in" << asset_array;
    in_doc << "assets" << pull_doc;
    builder << "$pull" << in_doc;
  }
}

void ObjectDatabaseManager::transaction(DatabaseResponse &response, ObjectInterface *obj, AnimationAction<ObjectFrame> *action, ObjectFrame *aframe, std::string& key, int transaction_type, bool is_append_operation) {
  int retries = 0;
  // Initialize the connection for the first time
  init();

  // Keep trying to send query to different Mongo instances until the
  // max amount of retries has been recieved.
  while (retries < max_retries) {
    bool failure = false;
    try {
      // Get a scoped read lock on the connection pool
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
      } else if (transaction_type == _DB_MONGO_ACTION_INSERT_) {
        auto inner_push_doc = bsoncxx::builder::stream::document{};
        auto action_doc = bsoncxx::builder::stream::document{};
        add_obj_action_to_doc(action_doc, action);
        std::string key_name("actions.");
        key_name.append(action->get_name());
        inner_push_doc << key_name << action_doc;
        builder << "$set" << inner_push_doc;
      } else if (transaction_type == _DB_MONGO_ACTION_REMOVE_) {
        auto action_doc = bsoncxx::builder::stream::document{};
        std::string key_name("actions.");
        key_name.append(action->get_name());
        action_doc << key_name << 1;
        builder << "$unset" << action_doc;
      } else if (transaction_type == _DB_MONGO_ACTION_UPDATE_) {
        auto action_doc = bsoncxx::builder::stream::document{};
        std::string key_name("actions.");
        key_name.append(action->get_name());
        action_doc << (key_name + ".description") << action->get_description();
        action_doc << (key_name + ".owner") << action->get_owner();
        builder << "$set" << action_doc;
      } else if (transaction_type == _DB_MONGO_FRAME_INSERT_) {
        std::string key_name("actions.");
        key_name.append(action->get_name());
        key_name.append(".frames.");
        key_name.append(std::to_string(aframe->get_frame()));
        auto inner_push_doc = bsoncxx::builder::stream::document{};
        auto frame_doc = bsoncxx::builder::stream::document{};
        add_obj_frame_to_doc(frame_doc, aframe);
        inner_push_doc << key_name << frame_doc;
        builder << "$set" << inner_push_doc;
      } else if (transaction_type == _DB_MONGO_FRAME_UPDATE_) {
        std::string key_start("actions.");
        key_start.append(action->get_name());
        key_start.append(".frames.");
        key_start.append(std::to_string(aframe->get_frame()));
        auto frame_doc = bsoncxx::builder::stream::document{};

        // Add frame elements
        frame_doc << (key_start + ".owner") << aframe->get_owner();
        frame_doc << (key_start + ".frame") << aframe->get_frame();

        // Add the Transform
        std::string trn_key_start = key_start + ".transform.";
        std::string key_values[16] = {"r1_c1", "r1_c2", "r1_c3", "r1_c4", \
            "r2_c1", "r2_c2", "r2_c3", "r2_c4", "r3_c1", "r3_c2", "r3_c3", "r3_c4", \
            "r4_c1", "r4_c2", "r4_c3", "r4_c4"};
        for (int j = 0; j < 4; j++) {
          for (int k = 0; k < 4; k++) {
            int index = (4 * j) + k;
            frame_doc << (trn_key_start + key_values[index]) << aframe->get_transform()->get_transform_element(j, k);
          }
        }

        // Write animation frame
        std::string aframe_key_start = key_start + ".frames." + std::to_string(aframe->get_frame()) + ".animation_frame.";
        if (aframe) {
          frame_doc << (aframe_key_start + "active") << true;
          for (int k = 0; k < 3; k++) {
            std::string translation_key_start = aframe_key_start + "translation_handle.";
            if (k == 0) translation_key_start.append("x.");
            if (k == 1) translation_key_start.append("y.");
            if (k == 2) translation_key_start.append("z.");

            frame_doc << (translation_key_start + "left_type") << aframe->get_translation(k)->get_lh_type();
            frame_doc << (translation_key_start + "right_type") << aframe->get_translation(k)->get_rh_type();
            frame_doc << (translation_key_start + "left_x") << aframe->get_translation(k)->get_lh_x();
            frame_doc << (translation_key_start + "left_y") << aframe->get_translation(k)->get_lh_y();
            frame_doc << (translation_key_start + "right_x") << aframe->get_translation(k)->get_rh_x();
            frame_doc << (translation_key_start + "right_y") << aframe->get_translation(k)->get_rh_y();
          }
          for (int m = 0; m < 4; m++) {
            std::string rotation_key_start = aframe_key_start + "rotation_handle.";
            if (m == 0) rotation_key_start.append("w.");
            if (m == 1) rotation_key_start.append("x.");
            if (m == 2) rotation_key_start.append("y.");
            if (m == 3) rotation_key_start.append("z.");

            frame_doc << (rotation_key_start + "left_type") << aframe->get_rotation(m)->get_lh_type();
            frame_doc << (rotation_key_start + "right_type") << aframe->get_rotation(m)->get_rh_type();
            frame_doc << (rotation_key_start + "left_x") << aframe->get_rotation(m)->get_lh_x();
            frame_doc << (rotation_key_start + "left_y") << aframe->get_rotation(m)->get_lh_y();
            frame_doc << (rotation_key_start + "right_x") << aframe->get_rotation(m)->get_rh_x();
            frame_doc << (rotation_key_start + "right_y") << aframe->get_rotation(m)->get_rh_y();
          }
          for (int n = 0; n < 3; n++) {
            std::string scale_key_start = aframe_key_start + "scale_handle.";
            if (n == 0) scale_key_start.append("x.");
            if (n == 1) scale_key_start.append("y.");
            if (n == 2) scale_key_start.append("z.");

            frame_doc << (scale_key_start + "left_type") << aframe->get_scale(n)->get_lh_type();
            frame_doc << (scale_key_start + "right_type") << aframe->get_scale(n)->get_rh_type();
            frame_doc << (scale_key_start + "left_x") << aframe->get_scale(n)->get_lh_x();
            frame_doc << (scale_key_start + "left_y") << aframe->get_scale(n)->get_lh_y();
            frame_doc << (scale_key_start + "right_x") << aframe->get_scale(n)->get_rh_x();
            frame_doc << (scale_key_start + "right_y") << aframe->get_scale(n)->get_rh_y();
          }
        } else {
          frame_doc << (aframe_key_start + "active") << false;
        }

        builder << "$set" << frame_doc;
      } else if (transaction_type == _DB_MONGO_FRAME_REMOVE_) {
        std::string key_name("actions.");
        key_name.append(action->get_name());
        key_name.append(".frames.");
        key_name.append(std::to_string(aframe->get_frame()));
        auto inner_remove = bsoncxx::builder::stream::document{};
        auto action_doc = bsoncxx::builder::stream::document{};
        action_doc << key_name << 1;
        builder << "$unset" << action_doc;
      }

      // Execute an Insert Transaction
      if (transaction_type == _DB_MONGO_INSERT_) {
        bsoncxx::document::value doc_value = \
          builder << bsoncxx::builder::stream::finalize;
        CoreDatabaseManager::insert_doc(coll, doc_value, key, response);
      // Execute an Update Transaction
      } else if (transaction_type == _DB_MONGO_UPDATE_ \
          || transaction_type == _DB_MONGO_LOCK_ \
          || transaction_type == _DB_MONGO_UNLOCK_ \
          || (transaction_type >= _DB_MONGO_ACTION_INSERT_ \
          && transaction_type <= _DB_MONGO_FRAME_REMOVE_)) {
        auto query_builder = bsoncxx::builder::stream::document{};
        try {
          bsoncxx::oid db_id(key);
          query_builder << "_id" << db_id;
        } catch (std::exception& e) {
          logger.error("Exception parsing Mongo OID");
          logger.error(e.what());
          response.error_message = std::string(e.what());
          break;
        }
        if (transaction_type == _DB_MONGO_LOCK_) {
          query_builder << "owner" << "";
        } else if (transaction_type == _DB_MONGO_UNLOCK_) {
          query_builder << "owner" << obj->get_owner();
        } else if (transaction_type >= _DB_MONGO_ACTION_UPDATE_ \
            && transaction_type <= _DB_MONGO_FRAME_REMOVE_) {
          std::string key_name("actions.");
          key_name.append(action->get_name());
          auto inner_exists_doc = bsoncxx::builder::stream::document{};
          inner_exists_doc << "$exists" << true;
          query_builder << key_name << inner_exists_doc;
          if (transaction_type == _DB_MONGO_FRAME_UPDATE_) {
            std::string key_name("actions.");
            key_name.append(action->get_name());
            key_name.append(".frames.");
            key_name.append(std::to_string(aframe->get_frame()));
            auto inner_exists_doc = bsoncxx::builder::stream::document{};
            inner_exists_doc << "$exists" << true;
            query_builder << key_name << inner_exists_doc;
          }
        }
        auto result = coll.update_one(query_builder << bsoncxx::builder::stream::finalize, \
            builder << bsoncxx::builder::stream::finalize);
        if (result) {
          if (result->modified_count() > 0) {
            logger.debug("Document Successfully updated in DB");
            response.success = true;
          } else {
            response.error_message = std::string("No Documents Modified in DB");
            response.error_code = NOT_FOUND;
          }
        } else {
          response.error_message = std::string("Null Result returned from DB");
          response.error_code = PROCESSING_ERROR;
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
      response.error_message = std::string(e.what());
      CoreDatabaseManager::add_failure();
      failure = true;
    }

    // Reset to a new connection on failure (if possible, to alternate server)
    if (failure) CoreDatabaseManager::set_new_connection();
    retries++;
    if (response.success && !(failure)) {
      break;
    }
  }
}

void ObjectDatabaseManager::get_frame_from_doc(bsoncxx::document::element &elt, ObjectFrame *aframe, int frame_index) {
  aframe->set_frame(frame_index);
  auto frame_owner_elt = elt["owner"];
  if (frame_owner_elt && frame_owner_elt.type() == bsoncxx::type::k_utf8) {
    aframe->set_owner(frame_owner_elt.get_utf8().value.to_string());
  }

  // Parse the transform array
  const char* key_values[16] = {"r1_c1", "r1_c2", "r1_c3", "r1_c4", \
      "r2_c1", "r2_c2", "r2_c3", "r2_c4", "r3_c1", "r3_c2", "r3_c3", "r3_c4", \
      "r4_c1", "r4_c2", "r4_c3", "r4_c4"};
  bsoncxx::document::element transform_element = elt["transform"];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int index = (4 * i) + j;
      bsoncxx::document::element transform_elt = transform_element[key_values[index]];
      aframe->get_transform()->set_transform_element(i, j, \
          transform_elt.get_double().value);
    }
  }

  // Parse the Animation Frame
  bsoncxx::document::element aframe_elt = elt["animation_frame"];
  auto aframe_active_elt = aframe_elt["active"];
  if (aframe_active_elt.get_bool().value) {
    auto aft_element = aframe_elt["translation_handle"];
    for (int i = 0; i < 3; i++) {
      bsoncxx::document::element aft_inner_elt;
      if (i == 0) aft_inner_elt = aft_element["x"];
      if (i == 1) aft_inner_elt = aft_element["y"];
      if (i == 2) aft_inner_elt = aft_element["z"];
      CoreDatabaseManager::get_handle_from_element(aft_inner_elt, \
          aframe->get_translation(i));
    }
    auto afr_elt = aframe_elt["rotation_handle"];
    for (int i = 0; i < 4; i++) {
      bsoncxx::document::element afr_inner_elt;
      if (i == 0) afr_inner_elt = afr_elt["w"];
      if (i == 1) afr_inner_elt = afr_elt["x"];
      if (i == 2) afr_inner_elt = afr_elt["y"];
      if (i == 3) afr_inner_elt = afr_elt["z"];
      CoreDatabaseManager::get_handle_from_element(afr_inner_elt, \
          aframe->get_rotation(i));
    }
    auto afs_element = aframe_elt["scale_handle"];
    for (int i = 0; i < 3; i++) {
      bsoncxx::document::element afs_inner_elt;
      if (i == 0) afs_inner_elt = afs_element["x"];
      if (i == 1) afs_inner_elt = afs_element["y"];
      if (i == 2) afs_inner_elt = afs_element["z"];
      CoreDatabaseManager::get_handle_from_element(afs_inner_elt, \
          aframe->get_scale(i));
    }
  }
}

void ObjectDatabaseManager::get_action_from_doc(bsoncxx::document::element &elt, AnimationAction<ObjectFrame> *action, std::string& action_name) {
  action->set_name(action_name);
  auto desc_element = elt["description"];
  if (desc_element && desc_element.type() == bsoncxx::type::k_utf8) {
    auto desc_elt_val_str = desc_element.get_utf8().value.to_string();
    action->set_description(desc_elt_val_str);
  }
  auto owner_element = elt["owner"];
  if (owner_element && owner_element.type() == bsoncxx::type::k_utf8) {
    auto owner_elt_val_str = owner_element.get_utf8().value.to_string();
    action->set_owner(owner_elt_val_str);
  }
  // Iterate over keyframes
  auto frames_element = elt["frames"];
  if (frames_element && frames_element.type() == bsoncxx::type::k_document) {
    auto frames_view = frames_element.get_document().view();
    for (auto frame_elt: frames_view) {
      if (frame_elt.type() == bsoncxx::type::k_document) {
        // Add frame attributes
        auto frame_int_elt = frame_elt["frame"];
        if (frame_int_elt && frame_int_elt.type() == bsoncxx::type::k_int32) {
          int frame_index = frame_int_elt.get_int32().value;
          ObjectFrame *new_frame = new ObjectFrame();
          action->add_keyframe(frame_index, new_frame);
          get_frame_from_doc(frame_elt, action->get_keyframe(frame_index), frame_index);
        }
      }
    }
  }
}

void ObjectDatabaseManager::bson_to_obj3(bsoncxx::document::view& result, ObjectInterface *obj) {
  // Parse basic values
  bsoncxx::document::element key_element = result["_id"];
  if (key_element && key_element.type() == bsoncxx::type::k_oid) {
    auto key_elt_val_str = key_element.get_oid().value.to_string();
    obj->set_key(key_elt_val_str);
  }
  bsoncxx::document::element name_element = result["name"];
  if (name_element && name_element.type() == bsoncxx::type::k_utf8) {
    auto name_elt_val_str = name_element.get_utf8().value.to_string();
    obj->set_name(name_elt_val_str);
  }
  bsoncxx::document::element parent_element = result["parent"];
  if (parent_element && parent_element.type() == bsoncxx::type::k_utf8) {
    auto parent_elt_val_str = parent_element.get_utf8().value.to_string();
    obj->set_parent(parent_elt_val_str);
  }
  bsoncxx::document::element asset_sub_id_element = result["asset_sub_id"];
  if (asset_sub_id_element && asset_sub_id_element.type() == bsoncxx::type::k_utf8) {
    auto asset_sid_val_str = asset_sub_id_element.get_utf8().value.to_string();
    obj->set_asset_sub_id(asset_sid_val_str);
  }
  bsoncxx::document::element type_element = result["type"];
  if (parent_element && parent_element.type() == bsoncxx::type::k_utf8) {
    auto parent_elt_val_str = parent_element.get_utf8().value.to_string();
    obj->set_type(type_element.get_utf8().value.to_string());
  }
  bsoncxx::document::element subtype_element = result["subtype"];
  if (parent_element && parent_element.type() == bsoncxx::type::k_utf8) {
    auto parent_elt_val_str = parent_element.get_utf8().value.to_string();
    obj->set_subtype(subtype_element.get_utf8().value.to_string());
  }
  bsoncxx::document::element scene_element = result["scene"];
  if (scene_element && scene_element.type() == bsoncxx::type::k_utf8) {
    auto scene_elt_val_str = scene_element.get_utf8().value.to_string();
    obj->set_scene(scene_elt_val_str);
  }
  bsoncxx::document::element owner_element = result["owner"];
  if (owner_element && owner_element.type() == bsoncxx::type::k_utf8) {
    auto owner_elt_val_str = owner_element.get_utf8().value.to_string();
    obj->set_owner(owner_elt_val_str);
  }

  // Parse the assets array
  bsoncxx::document::element assets_element = result["assets"];
  if (assets_element && assets_element.type() == bsoncxx::type::k_array) {
    bsoncxx::array::view assets_view = assets_element.get_array().value;
    int assets_array_size = std::distance(assets_view.begin(), assets_view.end());
    for (int i = 0; i < assets_array_size; i++) {
      bsoncxx::array::element asset_elt = assets_view[i];
      obj->add_asset(asset_elt.get_utf8().value.to_string());
    }
  }

  // Parse the transform array
  const char* key_values[16] = {"r1_c1", "r1_c2", "r1_c3", "r1_c4", \
      "r2_c1", "r2_c2", "r2_c3", "r2_c4", "r3_c1", "r3_c2", "r3_c3", "r3_c4", \
      "r4_c1", "r4_c2", "r4_c3", "r4_c4"};
  bsoncxx::document::element transform_element = result["transform"];
  if (transform_element && transform_element.type() == bsoncxx::type::k_array) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        int index = (4 * i) + j;
        bsoncxx::document::element transform_elt = transform_element[key_values[index]];
        obj->get_transform()->set_transform_element(i, j, \
            transform_elt.get_double().value);
      }
    }
  }

  // Parse the actions array
  auto actions_elt = result["actions"];
  if (actions_elt && actions_elt.type() == bsoncxx::type::k_document) {
    auto actions_view = actions_elt.get_document().view();
    for (auto element: actions_view) {
      if (element.type() == bsoncxx::type::k_document) {
        auto name_element = element["name"];
        if (name_element && name_element.type() == bsoncxx::type::k_utf8) {
          auto action_name = name_element.get_utf8().value.to_string();
          AnimationAction<ObjectFrame> *new_action = new AnimationAction<ObjectFrame>();
          obj->add_action(action_name, new_action);
          get_action_from_doc(element, obj->get_action(action_name), action_name);
        }
      }
    }
  }
}

void ObjectDatabaseManager::get_object(ObjectListInterface *response, std::string& key) {
  logger.debug("Attempting to retrieve object from Mongo");
  int retries = 0;
  // Initialize the connection for the first time
  init();
  response->set_num_records(0);
  // Attempt to retrieve the object
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
          ObjectInterface *obj = object_factory.build_object();
          bson_to_obj3(view, obj);
          response->add_object(obj);
          response->set_num_records(1);
        } else {
          response->set_error_code(NOT_FOUND);
          response->set_error_message(std::string("No results returned from query"));
        }
      } catch (mongocxx::exception& me) {
        logger.error("Mongo Exception Encountered");
        logger.error(me.what());
        response->set_error_code(PROCESSING_ERROR);
        response->set_error_message(std::string(me.what()));
        break;
      } catch (std::exception& e) {
        logger.error("Exception executing Mongo Query");
        logger.error(e.what());
        CoreDatabaseManager::add_failure();
        failure = true;
      }
    } catch (std::exception& e) {
      logger.error("Exception executing Mongo Query");
      logger.error(e.what());
      CoreDatabaseManager::add_failure();
      failure = true;
      break;
    }
    if (failure) CoreDatabaseManager::set_new_connection();
    retries++;
    if (!failure) break;
  }
}

void ObjectDatabaseManager::query(ObjectListInterface *response, ObjectInterface *obj, int max_results) {
  logger.debug("Attempting to query Mongo");
  int retries = 0;
  // Initialize the connection for the first time
  init();
  while (retries < max_retries) {
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
  // Initialize the connection for the first time
  init();
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
    } catch (std::exception& e) {
      logger.error("Exception parsing Mongo OID");
      logger.error(e.what());
      CoreDatabaseManager::add_failure();
      failure = true;
      break;
    }
  }
}
