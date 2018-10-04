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

// This implements the Configuration Manager

// This takes in a Command Line Interpreter, and based on the options provided,
// decides how the application needs to be configured.  It may configure either
// from a configuration file, or from a Consul agent

#include <iostream>
#include <boost/cstdint.hpp>

#include "app/include/clyman_utils.h"
#include "db/include/db_manager_interface.h"
#include "app/include/event_sender.h"
#include "app/include/cluster_manager.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

#ifndef SRC_CONTROLLER_INCLUDE_OBJECT_KEY_HANDLER_H_
#define SRC_CONTROLLER_INCLUDE_OBJECT_KEY_HANDLER_H_

class ObjectKeyRequestHandler: public Poco::Net::HTTPRequestHandler {
  AOSSL::KeyValueStoreInterface *config = nullptr;
  DatabaseManagerInterface *db_manager = nullptr;
  int msg_type = -1;
  ObjectListFactory object_list_factory;
  ObjectFactory object_factory;
  EventStreamPublisher *publisher = nullptr;
  ClusterManager *cluster_manager = nullptr;
  Poco::Logger& logger;
  std::string object_id;
  void process_get_message(std::string key, ObjectListInterface *response_body) {
    logger.information("Processing Get Message");
    // Execute the get message and build a result
    db_manager->get_object(response_body, key);
  }
  void process_delete_message(std::string key, ObjectListInterface *response_body) {
    logger.information("Processing Delete Message");
    // Persist the delete message
    DatabaseResponse db_response;
    db_manager->delete_object(db_response, key);
    if (!(db_response.success)) {
      if (db_response.error_code > NO_ERROR) {
        response_body->set_error_code(db_response.error_code);
      } else {
        response_body->set_error_code(PROCESSING_ERROR);
      }
      response_body->set_error_message(db_response.error_message);
    }
  }
 public:
  ObjectKeyRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype) : logger(Poco::Logger::get("Data")) \
      {config=conf;msg_type=mtype;db_manager=db;cluster_manager=cluster;publisher=pub;}
  ObjectKeyRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype, std::string id) : logger(Poco::Logger::get("Data")) \
      {config=conf;msg_type=mtype;db_manager=db;cluster_manager=cluster;object_id.assign(id);publisher=pub;}
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    logger.debug("Responding to Object Request");
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    // parse the post input data into a Scene List
    rapidjson::Document doc;
    ObjectListInterface *response_body = object_list_factory.build_json_object_list();
    response_body->set_msg_type(msg_type);
    response_body->set_error_code(NO_ERROR);
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);

    // Send an update to downstream services
    if (msg_type == OBJ_DEL) {
      ObjectInterface *in_doc = object_factory.build_object();
      ObjectListInterface *in_doc_list = object_list_factory.build_json_object_list();

      try {
        // Set up the event object
        db_manager->get_object(in_doc_list, object_id);
        if (in_doc_list->get_num_records() > 0) {
          in_doc->set_scene(in_doc_list->get_object(0)->get_scene());
        }

        // Construct and send the event
        in_doc->set_key(object_id);
        AOSSL::ServiceInterface *downstream = cluster_manager->get_ivan();
        if (downstream) {
          std::string message = in_doc->get_scene() + \
              std::string("\n") + in_doc->to_transform_json(msg_type);
          logger.debug("Sending Event: " + message);
          publisher->publish_event(message.c_str(), \
              downstream->get_address(), stoi(downstream->get_port()));
        }
      } catch (std::exception& e) {
        logger.error("Exception encountered sending delete event");
        response_body->set_error_message(e.what());
        response_body->set_error_code(PROCESSING_ERROR);
      }
      delete in_doc_list;
      delete in_doc;
    }

    // Add to the output message list
    try {
      if (msg_type == OBJ_GET) {
        process_get_message(object_id, response_body);
      } else if (msg_type == OBJ_DEL) {
        process_delete_message(object_id, response_body);
      }
    } catch (std::exception& e) {
      logger.error("Exception encountered during DB Operation");
      response_body->set_error_message(e.what());
      logger.error(response_body->get_error_message());
      response_body->set_error_code(PROCESSING_ERROR);
    }

    // Process the result
    if (response_body->get_error_code() == NOT_FOUND) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
    } else if (response_body->get_error_code() != NO_ERROR) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
    }

    // Set up the response
    std::ostream& ostr = response.send();

    std::string response_body_string;
    response_body->to_msg_string(response_body_string);
    ostr << response_body_string;
    ostr.flush();
    delete response_body;
  }
};

#endif  // SRC_CONTROLLER_INCLUDE_OBJECT_KEY_HANDLER_H_
