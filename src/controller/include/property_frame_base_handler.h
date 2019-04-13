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

// This handles create and update messages for Object Actions

#include <iostream>
#include <boost/cstdint.hpp>

#include "app/include/clyman_utils.h"
#include "app/include/event_sender.h"
#include "app/include/cluster_manager.h"
#include "db/include/db_manager_interface.h"
#include "model/core/include/animation_action.h"
#include "model/property/include/property_frame.h"
#include "model/property/include/property_interface.h"
#include "model/list/include/property_list_interface.h"
#include "model/factory/include/json_factory.h"
#include "model/factory/include/data_list_factory.h"
#include "model/factory/include/data_factory.h"
#include "frame_base_handler.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

#ifndef SRC_CONTROLLER_INCLUDE_PROPERTY_FRAME_BASE_HANDLER_H_
#define SRC_CONTROLLER_INCLUDE_PROPERTY_FRAME_BASE_HANDLER_H_

class PropertyFrameBaseRequestHandler: public FrameBaseRequestHandler<PropertyFrame>, public Poco::Net::HTTPRequestHandler {
 public:
  PropertyFrameBaseRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype, std::string prop_id, \
      std::string action_name) : FrameBaseRequestHandler<PropertyFrame>(conf, db, pub, cluster, mtype, prop_id, action_name) {}
  PropertyFrameBaseRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype, std::string id, std::string action_name, \
      std::string frame_ind) : FrameBaseRequestHandler<PropertyFrame>(conf, db, pub, cluster, mtype, id, action_name, frame_ind) {}
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    logger.debug("Responding to Object Frame Request");
    PropertyListInterface *response_body = object_list_factory.build_json_property_list();
    ClymanHandler::init_response(response, response_body);
    bool frame_indx_parse_success = true;
    if (msg_type == PROP_FRAME_UPD) {
      try {
        frame_index = std::stoi(frame_ind_str);
      } catch (std::exception& e) {
        logger.error("Exception encountered converting input frame to integer");
        logger.error(e.what());
        response_body->set_error_message(e.what());
        frame_indx_parse_success = false;
      }
    }
    // parse the post input data into a Scene List
    rapidjson::Document doc;
    char *tmpStr = clyman_request_body_to_json_document(request, doc);
    response_body->set_msg_type(msg_type);
    response_body->set_error_code(NO_ERROR);
    if (doc.HasParseError() || !(frame_indx_parse_success)) {
      logger.debug("Parsing Error Detected");
      // Set up parse error response
      response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
      response.setContentType("text/plain");
      response_body->set_error_code(TRANSLATION_ERROR);
      if (doc.HasParseError()) {
        response_body->set_error_message(rapidjson::GetParseError_En(doc.GetParseError()));
      }
      std::ostream& ostr = response.send();
      std::string response_body_string;
      response_body->to_msg_string(response_body_string);
      ostr << response_body_string;
      ostr.flush();
    } else if (response_body->get_error_code() == NO_ERROR) {
      // Convert the rapidjson doc to a data model object
      response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
      PropertyInterface *new_out_doc = object_factory.build_property();
      PropertyFrame *inp_doc = nullptr;
      try {
        inp_doc = json_factory.build_property_frame(doc);
      } catch (std::exception& e) {
        logger.error("Exception encountered building Object Frame");
        logger.error(e.what());
        response_body->set_error_code(TRANSLATION_ERROR);
        response_body->set_error_message(e.what());
      }
      if (inp_doc) {
        // Process the input objects
        // send downstream updates, and persist the result
        // Execute the Mongo Queries
        try {
          if (msg_type == PROP_FRAME_CRT) {
            process_create_message(object_id, actn_name, inp_doc, response_body);
          } else if (msg_type == PROP_FRAME_UPD) {
            process_update_message(object_id, actn_name, frame_index, inp_doc, response_body);
          }
        } catch (std::exception& e) {
          logger.error("Exception encountered during DB Operation");
          response_body->set_error_message(e.what());
          logger.error(response_body->get_error_message());
          response_body->set_error_code(PROCESSING_ERROR);
        }
        // Send an update to downstream services
        if (msg_type == PROP_FRAME_CRT || msg_type == PROP_FRAME_UPD) {
          AOSSL::ServiceInterface *downstream = cluster_manager->get_ivan();
          if (downstream) {
            AnimationAction<PropertyFrame> *tmp_action = new AnimationAction<PropertyFrame>();
            new_out_doc->add_action(actn_name, tmp_action);
            new_out_doc->get_action(actn_name)->set_name(actn_name);
            new_out_doc->get_action(actn_name)->add_keyframe(frame_index, inp_doc);
            new_out_doc->get_action(actn_name)->get_keyframe(frame_index)->set_frame(frame_index);
            new_out_doc->set_key(object_id);
            new_out_doc->set_scene(inp_doc->get_scene());
            std::string json_str;
            new_out_doc->to_json(json_str, msg_type);
            std::string message = inp_doc->get_scene() + \
                std::string("\n") + json_str;
            logger.debug("Sending Event: " + message);
            publisher->publish_event(message.c_str(), \
                downstream->get_address(), stoi(downstream->get_port()));
          }
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
    }
    delete response_body;
    delete[] tmpStr;
  }
};

#endif  // SRC_CONTROLLER_INCLUDE_PROPERTY_FRAME_BASE_HANDLER_H_
