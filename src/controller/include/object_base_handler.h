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

#ifndef SRC_CONTROLLER_INCLUDE_OBJECT_BASE_HANDLER_H_
#define SRC_CONTROLLER_INCLUDE_OBJECT_BASE_HANDLER_H_

class ObjectBaseRequestHandler: public ClymanHandler, public Poco::Net::HTTPRequestHandler {
  // The ID of the Object being handled
  std::string object_id;

  // Execute a create action against the DB
  void process_create_message(ObjectInterface* in_doc, ObjectInterface* out_doc, ObjectListInterface *response_body) {
    logger.information("Processing Creation Message");
    // Persist the creation message
    std::string new_object_key;
    logger.information("Creating Object with Name: " + in_doc->get_name());
    DatabaseResponse response;
    db_manager->create_object(response, in_doc, new_object_key);
    if (response.success) {
      out_doc->set_key(new_object_key);
    } else {
      response_body->set_error_code(PROCESSING_ERROR);
      response_body->set_error_message(response.error_message);
    }
  }

  // Execute an update action against the DB
  void process_update_message(ObjectInterface* in_doc, ObjectInterface* out_doc, ObjectListInterface *response_body) {
    logger.information("Processing Update Message");
    // Persist the update message
    logger.information("Updating Object with Name: " + in_doc->get_name());
    DatabaseResponse response;
    std::string key = in_doc->get_key();
    db_manager->update_object(response, in_doc, key);
    if (!(response.success)) {
      if (response.error_code > NO_ERROR) {
        response_body->set_error_code(response.error_code);
      } else {
        response_body->set_error_code(PROCESSING_ERROR);
      }
      response_body->set_error_message(response.error_message);
    }
  }

  // Execute a Query action against the DB
  void process_query_message(ObjectInterface* in_doc, ObjectListInterface *response_body, int max_results) {
    logger.information("Processing Query Message");
    // Execute the query message and build a result
    db_manager->object_query(response_body, in_doc, max_results);
  }

 public:
  ObjectBaseRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype) : \
      ClymanHandler(conf, db, pub, cluster, mtype) {}
  ObjectBaseRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype, std::string id) : \
      ClymanHandler(conf, db, pub, cluster, mtype) {object_id.assign(id);}
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    logger.debug("Responding to Object Request");
    // parse the post input data into a Scene List
    rapidjson::Document doc;
    char *tmpStr = clyman_request_body_to_json_document(request, doc);
    ObjectListInterface *response_body = object_list_factory.build_json_object_list();
    ClymanHandler::init_response(response, response_body);
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
      ObjectListInterface *inp_doc = nullptr;
      try {
        inp_doc = json_factory.build_object_list(doc);
      } catch (std::exception& e) {
        logger.error("Exception encountered building Object List");
        logger.error(e.what());
        response_body->set_error_code(TRANSLATION_ERROR);
        response_body->set_error_message(e.what());
      }
      if (inp_doc) {
        // update the message type and process the post input data
        inp_doc->set_msg_type(msg_type);

        // Process the input objects
        // send downstream updates, and persist the result
        for (int i = 0; i < inp_doc->num_objects(); i++) {
          // Add to the output message list
          ObjectInterface *new_out_doc = object_factory.build_object();
          // get the object out of the input message list
          ObjectInterface* in_doc = inp_doc->get_object(i);
          if (!(object_id.empty())) in_doc->set_key(object_id);
          // Execute the Mongo Queries
          try {
            if (msg_type == OBJ_CRT) {
              process_create_message(in_doc, new_out_doc, response_body);
              response_body->add_object(new_out_doc);
            } else if (msg_type == OBJ_UPD) {
              process_update_message(in_doc, new_out_doc, response_body);
              response_body->add_object(new_out_doc);
            } else if (msg_type == OBJ_QUERY) {
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
          if (msg_type == OBJ_CRT || msg_type == OBJ_UPD) {
            AOSSL::ServiceInterface *downstream = cluster_manager->get_ivan();
            if (downstream) {
              std::string message = in_doc->get_scene() + \
                  std::string("\n") + in_doc->to_transform_json(msg_type);
              logger.debug("Sending Event: " + message);
              publisher->publish_event(message.c_str(), \
                  downstream->get_address(), stoi(downstream->get_port()));
            }
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
      delete inp_doc;
    }
    delete response_body;
    delete[] tmpStr;
  }
};

#endif  // SRC_CONTROLLER_INCLUDE_OBJECT_BASE_HANDLER_H_
