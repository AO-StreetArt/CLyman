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
#include "app/include/database_manager.h"
#include "app/include/event_sender.h"
#include "app/include/cluster_manager.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

#ifndef SRC_CONTROLLER_INCLUDE_SCENE_BASE_HANDLER_H_
#define SRC_CONTROLLER_INCLUDE_SCENE_BASE_HANDLER_H_

// Convert Request Contents into a Rapidjson Document
// Returns a char* which must be deleted only after using the Rapidjson Doc
static inline char* clyman_request_body_to_json_document(Poco::Net::HTTPServerRequest& request, \
    rapidjson::Document& doc) {
  // Pull the request body out of a stream and into a character buffer
  int length = request.getContentLength();
  std::istream &request_stream = request.stream();
  char *buffer = new char[length];
  request_stream.read(buffer, length);

  // Parse the buffer containing the request body into the rapidjson document
  doc.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer);

  // Return the buffer, as it needs to be valid while we're using the document
  return buffer;
};

class ObjectBaseRequestHandler: public Poco::Net::HTTPRequestHandler {
  AOSSL::KeyValueStoreInterface *config = nullptr;
  DatabaseManager *db_manager = nullptr;
  int msg_type = -1;
  ObjectListFactory object_list_factory;
  ObjectFactory object_factory;
  EventStreamPublisher *publisher = nullptr;
  ClusterManager *cluster_manager = nullptr;
 public:
  ObjectBaseRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManager *db, EventStreamPublisher *pub, ClusterManager *cluster, int mtype) \
    {config=conf;msg_type=mtype;db_manager=db;publisher=pub;cluster_manager=cluster;}
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    Poco::Logger::get("Controller").debug("Responding to Object Request");
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    // parse the post input data into a Scene List
    rapidjson::Document doc;
    char *tmpStr = clyman_request_body_to_json_document(request, doc);
    ObjectListInterface *response_body = object_list_factory.build_json_object_list();
    response_body->set_msg_type(msg_type);
    response_body->set_error_code(NO_ERROR);
    if (doc.HasParseError()) {
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
      ObjectListInterface *inp_doc = object_list_factory.build_object_list(doc);
      // update the message type and process the post input data
      inp_doc->set_msg_type(msg_type);

      // Process the input objects
      // send downstream updates, and persist the result
      for (int i = 0; i < inp_doc->num_objects(); i++) {
        // Add to the output message list
        ObjectInterface *new_out_doc = object_factory.build_object();
        // get the object out of the input message list
        ObjectInterface* in_doc = inp_doc->get_object(i);
        // Persist the creation message
        std::string new_object_key;
        try {
          db_manager->create_object(in_doc, new_object_key);
        } catch (std::exception& e) {
          response_body->set_error_message(e.what());
          response_body->set_error_code(PROCESSING_ERROR);
          break;
        }
        new_out_doc->set_key(new_object_key);
        response_body->add_object(new_out_doc);
        // Send an update to downstream services
        AOSSL::ServiceInterface *downstream = cluster_manager->get_ivan();
        if (downstream) {
          std::string message = in_doc->get_scene() + \
              std::string("\n") + in_doc->to_transform_json();
          publisher->publish_event(message.c_str(), \
              downstream->get_address(), stoi(downstream->get_port()));
        }
      }

      // Set up the response
      std::ostream& ostr = response.send();

      // Process the result
      std::string response_body_string;
      response_body->to_msg_string(response_body_string);
      ostr << response_body_string;
      ostr.flush();
      if (response_body->get_error_code() != NO_ERROR) {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
      }
      delete inp_doc;
    }
    delete response_body;
    delete[] tmpStr;
  }
};

#endif  // SRC_CONTROLLER_INCLUDE_SCENE_BASE_HANDLER_H_
