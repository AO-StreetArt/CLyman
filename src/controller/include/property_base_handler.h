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

#ifndef SRC_CONTROLLER_INCLUDE_PROPERTY_BASE_HANDLER_H_
#define SRC_CONTROLLER_INCLUDE_PROPERTY_BASE_HANDLER_H_

class PropertyBaseRequestHandler: public Poco::Net::HTTPRequestHandler {
  AOSSL::KeyValueStoreInterface *config = nullptr;
  DatabaseManagerInterface *db_manager = nullptr;
  int msg_type = -1;
  ObjectListFactory object_list_factory;
  ObjectFactory object_factory;
  EventStreamPublisher *publisher = nullptr;
  ClusterManager *cluster_manager = nullptr;
  Poco::Logger& logger;
  std::string object_id;
  void process_create_message(PropertyInterface* in_doc, PropertyInterface* out_doc, PropertyListInterface *response_body) {
    logger.information("Processing Creation Message");
    // Persist the creation message
    std::string new_object_key;
    logger.information("Creating Property with Name: " + in_doc->get_name());
    DatabaseResponse response;
    db_manager->create_property(response, in_doc, new_object_key);
    if (response.success) {
      out_doc->set_key(new_object_key);
    } else {
      response_body->set_error_code(PROCESSING_ERROR);
      response_body->set_error_message(response.error_message);
    }
  }
  void process_update_message(PropertyInterface* in_doc, PropertyListInterface *response_body) {
    logger.information("Processing Update Message");
    // Persist the update message
    logger.information("Updating Property with Name: " + in_doc->get_name());
    DatabaseResponse response;
    std::string key = in_doc->get_key();
    db_manager->update_property(response, in_doc, key);
    if (!(response.success)) {
      response_body->set_error_code(PROCESSING_ERROR);
      response_body->set_error_message(response.error_message);
    }
  }
  void process_query_message(PropertyInterface* in_doc, PropertyListInterface *response_body, int max_results) {
    logger.information("Processing Query Message");
    // Execute the query message and build a result
    db_manager->property_query(response_body, in_doc, max_results);
  }
 public:
  PropertyBaseRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype) : logger(Poco::Logger::get("Data")) \
      {config=conf;msg_type=mtype;db_manager=db;publisher=pub;cluster_manager=cluster;}
  PropertyBaseRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype, std::string id) : logger(Poco::Logger::get("Data")) \
      {config=conf;msg_type=mtype;db_manager=db;publisher=pub;cluster_manager=cluster;object_id.assign(id);}
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    logger.debug("Responding to Property Request");
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    // parse the post input data into a Scene List
    rapidjson::Document doc;
    char *tmpStr = clyman_request_body_to_json_document(request, doc);
    PropertyListInterface *response_body = object_list_factory.build_json_property_list();
    response_body->set_msg_type(msg_type);
    response_body->set_error_code(NO_ERROR);
    if (doc.HasParseError()) {
      logger.debug("Parsing Error Detected");
      // Set up parse error response
      response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
      response_body->set_error_code(TRANSLATION_ERROR);
      response_body->set_error_message(rapidjson::GetParseError_En(doc.GetParseError()));
      std::ostream& ostr = response.send();
      std::string response_body_string;
      response_body->to_msg_string(response_body_string);
      ostr << response_body_string;
      ostr.flush();
    } else {
      // Convert the rapidjson doc to a scene list
      response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
      PropertyListInterface *inp_doc = nullptr;
      try {
        inp_doc = object_list_factory.build_property_list(doc);
      } catch (std::exception& e) {
        logger.error("Exception encountered building Property List");
        logger.error(e.what());
        response_body->set_error_code(TRANSLATION_ERROR);
        response_body->set_error_message(e.what());
      }
      if (inp_doc) {
        // update the message type and process the post input data
        inp_doc->set_msg_type(msg_type);

        // Process the input objects
        // send downstream updates, and persist the result
        for (int i = 0; i < inp_doc->num_props(); i++) {
          // Add to the output message list
          PropertyInterface *new_out_doc = object_factory.build_property();
          // get the object out of the input message list
          PropertyInterface* in_doc = inp_doc->get_prop(i);
          if (!(object_id.empty())) in_doc->set_key(object_id);
          // Execute the Mongo Queries
          try {
            if (msg_type == PROP_CRT) {
              process_create_message(in_doc, new_out_doc, response_body);
              response_body->add_prop(new_out_doc);
            } else if (msg_type == PROP_UPD) {
              process_update_message(in_doc, response_body);
            } else if (msg_type == PROP_QUERY) {
              process_query_message(in_doc, response_body, inp_doc->get_num_records());
            }
          } catch (std::exception& e) {
            logger.error("Exception encountered during DB Operation");
            response_body->set_error_message(e.what());
            logger.error(response_body->get_error_message());
            response_body->set_error_code(PROCESSING_ERROR);
            break;
          }
          // Send an update to downstream services
          if (msg_type == PROP_CRT || msg_type == PROP_UPD) {
            AOSSL::ServiceInterface *downstream = cluster_manager->get_ivan();
            if (downstream) {
              std::string transform_str;
              in_doc->to_json(transform_str);
              std::string message = in_doc->get_scene() + \
                  std::string("\n") + transform_str;
              logger.debug("Sending Event: " + message);
              publisher->publish_event(message.c_str(), \
                  downstream->get_address(), stoi(downstream->get_port()));
            }
          }
        }
      }

      if (response_body->get_error_code() == TRANSLATION_ERROR) {
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
      delete inp_doc;
    }
    delete response_body;
    delete[] tmpStr;
  }
};

#endif  // SRC_CONTROLLER_INCLUDE_PROPERTY_BASE_HANDLER_H_
