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
#include "model/object/include/object_interface.h"
#include "model/list/include/object_list_interface.h"
#include "model/factory/include/json_factory.h"
#include "model/factory/include/data_list_factory.h"
#include "model/factory/include/data_factory.h"
#include "clyman_handler.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

#ifndef SRC_CONTROLLER_INCLUDE_OBJECT_ACTION_DELETE_HANDLER_H_
#define SRC_CONTROLLER_INCLUDE_OBJECT_ACTION_DELETE_HANDLER_H_

class ObjectActionDeleteRequestHandler: public ClymanHandler, public Poco::Net::HTTPRequestHandler {
  std::string object_id;
  std::string actn_name;
  void process_delete_message(std::string& object_key, std::string& act_name, ObjectListInterface *response_body) {
    // Persist the deletion message
    logger.information("Deleting Object Action with Name: " + act_name);
    DatabaseResponse response;
    db_manager->delete_object_action(response, object_key, act_name);
    if (!(response.success)) {
      response_body->set_error_code(PROCESSING_ERROR);
      response_body->set_error_message(response.error_message);
    }
  }
 public:
  ObjectActionDeleteRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype, std::string obj_id, std::string action_name) : ClymanHandler(conf, db, pub, cluster, mtype) {
    object_id.assign(obj_id);
    actn_name.assign(action_name);
  }
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    logger.debug("Responding to Object Action Request");
    ObjectListInterface *response_body = object_list_factory.build_json_object_list();
    ClymanHandler::init_response(response, response_body);
    // Convert the rapidjson doc to a scene list
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    ObjectInterface *new_out_doc = object_factory.build_object();
    // Process the input objects
    // Execute the Mongo Queries
    try {
      process_delete_message(object_id, actn_name, response_body);
    } catch (std::exception& e) {
      logger.error("Exception encountered during DB Operation");
      response_body->set_error_message(e.what());
      logger.error(response_body->get_error_message());
      response_body->set_error_code(PROCESSING_ERROR);
    }
    // Get the scene
    ObjectListInterface *in_doc_list = object_list_factory.build_json_object_list();

    try {
      db_manager->get_object(in_doc_list, object_id);
    } catch (std::exception& e) {
      logger.error("Exception encountered during DB Operation");
      response_body->set_error_message(e.what());
      logger.error(response_body->get_error_message());
      response_body->set_error_code(PROCESSING_ERROR);
    }

    // Send an update to downstream services
    AOSSL::ServiceInterface *downstream = cluster_manager->get_ivan();
    if (downstream && response_body->get_error_code() > NO_ERROR && in_doc_list->get_num_records() > 0) {
      AnimationAction<ObjectFrame> *tmp_action = new AnimationAction<ObjectFrame>();
      new_out_doc->add_action(actn_name, tmp_action);
      new_out_doc->get_action(actn_name)->set_name(actn_name);
      new_out_doc->set_key(object_id);
      new_out_doc->set_scene(in_doc_list->get_object(0)->get_scene());
      std::string message = in_doc_list->get_object(0)->get_scene() + \
          std::string("\n") + new_out_doc->to_transform_json(msg_type);
      logger.debug("Sending Event: " + message);
      publisher->publish_event(message.c_str(), \
          downstream->get_address(), stoi(downstream->get_port()));
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

#endif  // SRC_CONTROLLER_INCLUDE_OBJECT_ACTION_DELETE_HANDLER_H_
