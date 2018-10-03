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

#ifndef SRC_CONTROLLER_INCLUDE_PROPERTY_KEY_HANDLER_H_
#define SRC_CONTROLLER_INCLUDE_PROPERTY_KEY_HANDLER_H_

class PropertyKeyRequestHandler: public Poco::Net::HTTPRequestHandler {
  AOSSL::KeyValueStoreInterface *config = nullptr;
  DatabaseManagerInterface *db_manager = nullptr;
  int msg_type = -1;
  ObjectListFactory object_list_factory;
  ObjectFactory object_factory;
  ClusterManager *cluster_manager = nullptr;
  EventStreamPublisher *publisher = nullptr;
  Poco::Logger& logger;
  std::string object_id;
  void process_get_message(std::string key, PropertyListInterface *response_body) {
    logger.information("Processing Get Message");
    // Execute the get message and build a result
    db_manager->get_property(response_body, key);
  }
  void process_delete_message(std::string key, PropertyListInterface *response_body) {
    logger.information("Processing Delete Message");
    // Persist the delete message
    DatabaseResponse db_response;
    db_manager->delete_property(db_response, key);
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
  PropertyKeyRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype) : logger(Poco::Logger::get("Data")) \
      {config=conf;msg_type=mtype;db_manager=db;cluster_manager=cluster;publisher=pub;}
  PropertyKeyRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype, std::string id) : logger(Poco::Logger::get("Data")) \
      {config=conf;msg_type=mtype;db_manager=db;cluster_manager=cluster;object_id.assign(id);publisher=pub;}
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    logger.debug("Responding to Property Request");
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    // parse the post input data into a Scene List
    rapidjson::Document doc;
    PropertyListInterface *response_body = object_list_factory.build_json_property_list();
    response_body->set_msg_type(msg_type);
    response_body->set_error_code(NO_ERROR);
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);

    // Add to the output message list
    try {
      if (msg_type == PROP_GET) {
        process_get_message(object_id, response_body);
      } else if (msg_type == PROP_DEL) {
        process_delete_message(object_id, response_body);
      }
    } catch (std::exception& e) {
      logger.error("Exception encountered during DB Operation");
      response_body->set_error_message(e.what());
      logger.error(response_body->get_error_message());
      response_body->set_error_code(PROCESSING_ERROR);
    }

    // Send an update to downstream services
    if (msg_type == PROP_DEL) {
      PropertyListInterface *in_doc_list = object_list_factory.build_json_property_list();
      PropertyInterface *in_doc = object_factory.build_property();

      try {
        // Setup the Event Property
        in_doc->set_key(object_id);
        db_manager->get_property(in_doc_list, object_id);
        if (in_doc_list->get_num_records() > 0) {
          in_doc->set_scene(in_doc_list->get_prop(0)->get_scene());
        }

        // Build and send the event message
        AOSSL::ServiceInterface *downstream = cluster_manager->get_ivan();
        if (downstream) {
          std::string transform_str;
          in_doc->to_json(transform_str, msg_type);
          std::string message = in_doc->get_scene() + \
              std::string("\n") + transform_str;
          logger.debug("Sending Event: " + message);
          publisher->publish_event(message.c_str(), \
              downstream->get_address(), stoi(downstream->get_port()));
        }
      } catch (std::exception& e) {
        logger.error("Exception encountered sending delete event");
        response_body->set_error_message(e.what());
        response_body->set_error_code(PROCESSING_ERROR);
      }

      delete in_doc;
      delete in_doc_list;
    }

    if (response_body->get_error_code() == NOT_FOUND) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
    } else if (response_body->get_error_code() == TRANSLATION_ERROR) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
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
    delete response_body;
  }
};

#endif  // SRC_CONTROLLER_INCLUDE_PROPERTY_KEY_HANDLER_H_
