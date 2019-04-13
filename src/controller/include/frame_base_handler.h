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

// This is the base class for Frame create/update controllers

#include <iostream>
#include <boost/cstdint.hpp>

#include "app/include/clyman_utils.h"
#include "app/include/event_sender.h"
#include "app/include/cluster_manager.h"
#include "db/include/db_manager_interface.h"
#include "model/core/include/animation_action.h"
#include "model/object/include/object_interface.h"
#include "model/object/include/object_frame.h"
#include "model/list/include/object_list_interface.h"
#include "model/factory/include/json_factory.h"
#include "model/factory/include/data_list_factory.h"
#include "model/factory/include/data_factory.h"
#include "clyman_handler.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#ifndef SRC_CONTROLLER_INCLUDE_FRAME_BASE_HANDLER_H_
#define SRC_CONTROLLER_INCLUDE_FRAME_BASE_HANDLER_H_

template <class T>
class FrameBaseRequestHandler: public ClymanHandler {
 protected:
  std::string object_id;
  std::string actn_name;
  std::string frame_ind_str;
  int frame_index = 0;

  void process_response(DatabaseResponse& response, DataListInterface *response_body) {
    if (!(response.success)) {
      if (response.error_code > NO_ERROR) {
        response_body->set_error_code(response.error_code);
      } else {
        response_body->set_error_code(PROCESSING_ERROR);
      }
      response_body->set_error_message(response.error_message);
    }
  }

  void process_create_message(std::string& object_key, std::string& action_name, T* in_doc, DataListInterface *response_body) {
    // Persist the creation message
    frame_index = in_doc->get_frame();
    logger.information("Creating Object Frame with Index: " + frame_index);
    DatabaseResponse response;
    db_manager->create_keyframe(response, object_key, action_name, in_doc, frame_index);
    process_response(response, response_body);
  }

  void process_update_message(std::string& object_key, std::string& action_name, int frame_indx, T* in_doc, DataListInterface *response_body) {
    // Persist the update message
    logger.information("Updating Object Frame with Index: " + frame_indx);
    DatabaseResponse response;
    db_manager->update_keyframe(response, object_key, action_name, in_doc, frame_indx);
    process_response(response, response_body);
  }
  FrameBaseRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype, std::string obj_id, \
      std::string action_name) : ClymanHandler(conf, db, pub, cluster, mtype) {
    object_id.assign(obj_id);
    actn_name.assign(action_name);
  }
  FrameBaseRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype, std::string id, std::string action_name, \
      std::string frame_ind) : ClymanHandler(conf, db, pub, cluster, mtype) {
    object_id.assign(id);
    actn_name.assign(action_name);
    frame_ind_str.assign(frame_ind);
  }
};

#endif  // SRC_CONTROLLER_INCLUDE_FRAME_BASE_HANDLER_H_
