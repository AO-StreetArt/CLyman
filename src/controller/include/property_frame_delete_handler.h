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

// This handles create and update messages for Property Frames

#include <iostream>
#include <boost/cstdint.hpp>

#include "app/include/clyman_utils.h"
#include "app/include/event_sender.h"
#include "app/include/cluster_manager.h"
#include "db/include/db_manager_interface.h"
#include "model/property/include/property_interface.h"
#include "model/list/include/property_list_interface.h"
#include "model/factory/include/json_factory.h"
#include "model/factory/include/data_list_factory.h"
#include "model/factory/include/data_factory.h"
#include "clyman_handler.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

#ifndef SRC_CONTROLLER_INCLUDE_PROPERTY_FRAME_DELETE_HANDLER_H_
#define SRC_CONTROLLER_INCLUDE_PROPERTY_FRAME_DELETE_HANDLER_H_

class PropertyFrameDeleteRequestHandler: public ClymanHandler, public Poco::Net::HTTPRequestHandler {
  std::string property_id;
  std::string actn_name;
  std::string frame_ind_str;
  int frame_index = 0;
  void process_delete_message(std::string& property_key, std::string& act_name, int frame_num, PropertyListInterface *response_body) {
    // Persist the deletion message
    logger.information("Deleting Property Frame with Name: " + act_name);
    DatabaseResponse response;
    db_manager->delete_property_keyframe(response, property_key, act_name, frame_num);
    if (!(response.success)) {
      response_body->set_error_code(PROCESSING_ERROR);
      response_body->set_error_message(response.error_message);
    }
  }
 public:
  PropertyFrameDeleteRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype, std::string prop_id, std::string action_name, std::string frame_ind) : ClymanHandler(conf, db, pub, cluster, mtype) {
    property_id.assign(prop_id);
    actn_name.assign(action_name);
    frame_ind_str.assign(frame_ind);
  }
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    logger.debug("Responding to Property Frame Request");
    PropertyListInterface *response_body = object_list_factory.build_json_property_list();
    ClymanHandler::init_response(response, response_body);

    // Parse the frame index specified
    bool should_continue = true;
    try {
      frame_index = std::stoi(frame_ind_str);
    } catch (std::exception& e) {
      logger.error("Exception encountered converting input frame to integer");
      logger.error(e.what());
      response_body->set_error_message(e.what());
      should_continue = false;
    }

    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    PropertyInterface *new_out_doc = object_factory.build_property();
    // Process the input properties
    // Execute the Mongo Queries
    if (should_continue) {
      try {
        process_delete_message(property_id, actn_name, frame_index, response_body);
      } catch (std::exception& e) {
        logger.error("Exception encountered during DB Operation");
        response_body->set_error_message(e.what());
        logger.error(response_body->get_error_message());
        response_body->set_error_code(PROCESSING_ERROR);
        should_continue = false;
      }
    }

    // Get the scene
    PropertyListInterface *in_doc_list = object_list_factory.build_json_property_list();

    if (should_continue) {
      try {
        db_manager->get_property(in_doc_list, property_id);
      } catch (std::exception& e) {
        logger.error("Exception encountered during DB Operation");
        response_body->set_error_message(e.what());
        logger.error(response_body->get_error_message());
        response_body->set_error_code(PROCESSING_ERROR);
        should_continue = false;
      }
    }

    // Send an update to downstream services
    AOSSL::ServiceInterface *downstream = cluster_manager->get_ivan();
    if (should_continue) {
      if (downstream && response_body->get_error_code() > NO_ERROR && in_doc_list->get_num_records() > 0) {
        AnimationAction<PropertyFrame> *tmp_action = new AnimationAction<PropertyFrame>();
        PropertyFrame *tmp_frame = new PropertyFrame();
        new_out_doc->add_action(actn_name, tmp_action);
        new_out_doc->get_action(actn_name)->add_keyframe(frame_index, tmp_frame);
        new_out_doc->get_action(actn_name)->set_name(actn_name);
        new_out_doc->get_action(actn_name)->get_keyframe(frame_index)->set_frame(frame_index);
        new_out_doc->set_key(property_id);
        new_out_doc->set_scene(in_doc_list->get_prop(0)->get_scene());
        std::string json_str;
        new_out_doc->to_json(json_str, msg_type);
        std::string message = in_doc_list->get_prop(0)->get_scene() + \
            std::string("\n") + json_str;
        logger.debug("Sending Event: " + message);
        publisher->publish_event(message.c_str(), \
            downstream->get_address(), stoi(downstream->get_port()));
      }
    }

    if (response_body->get_error_code() == TRANSLATION_ERROR) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    } else if (response_body->get_error_code() == NOT_FOUND) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
    } else if (response_body->get_error_code() != NO_ERROR) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
    }

    // Set up the response
    std::ostream& ostr = response.send();

    // Process the result
    std::string response_body_string;
    response_body->to_msg_string(response_body_string);
    ostr << response_body_string;
    ostr.flush();
    delete new_out_doc;
    delete response_body;
    delete in_doc_list;
  }
};

#endif  // SRC_CONTROLLER_INCLUDE_PROPERTY_FRAME_DELETE_HANDLER_H_
