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
 public:
  ObjectBaseRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManager *db, int mtype) \
    {config=conf;msg_type=mtype;db_manager=db;}
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    Poco::Logger::get("Controller").debug("Responding to Object Request");
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    // parse the post input data into a Scene List
    rapidjson::Document doc;
    char *tmpStr = clyman_request_body_to_json_document(request, doc);
    ObjectListInterface *response_body = object_list_factory.build_json_object_list();
    response_body->set_msg_type(msg_type);
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
      // Set up the response
      std::ostream& ostr = response.send();

      // TO-DO: Process the input message and persist the result
      bool result = true;

      // TO-DO: Process the result
      if (result) {
        if (msg_type == OBJ_GET) {
          // ostr << result->get_return_string();
          ostr.flush();
        } else {
          ObjectInterface *key_obj = object_factory.build_object();
          response_body->set_error_code(NO_ERROR);
          // key_scn->set_key(result->get_return_string());
          response_body->add_object(key_obj);
          std::string response_body_string;
          response_body->to_msg_string(response_body_string);
          ostr << response_body_string;
          ostr.flush();
        }
      } else {
        // response_body->set_err_code(result->get_error_code());
        // response_body->set_err_msg(result->get_error_description());
        std::string response_body_string;
        response_body->to_msg_string(response_body_string);
        ostr << response_body_string;
        ostr.flush();
      }
      delete inp_doc;
      // delete result;
    }
    delete response_body;
    delete[] tmpStr;
  }
};

#endif  // SRC_CONTROLLER_INCLUDE_SCENE_BASE_HANDLER_H_
