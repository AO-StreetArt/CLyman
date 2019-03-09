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
#include "model/list/include/data_list_interface.h"
#include "model/factory/include/data_factory.h"
#include "model/factory/include/data_list_factory.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

#ifndef SRC_CONTROLLER_INCLUDE_OBJECT_LOCK_HANDLER_H_
#define SRC_CONTROLLER_INCLUDE_OBJECT_LOCK_HANDLER_H_

class ObjectLockRequestHandler: public Poco::Net::HTTPRequestHandler {
  AOSSL::KeyValueStoreInterface *config = nullptr;
  DatabaseManagerInterface *db_manager = nullptr;
  int msg_type = -1;
  DataListFactory object_list_factory;
  DataFactory object_factory;
  ClusterManager *cluster_manager = nullptr;
  Poco::Logger& logger;
  std::string object_id;
  std::string device_id;
  void process_lock_message(std::string obj_key, std::string dev_key, ObjectListInterface *response_body) {
    logger.information("Processing Lock Message");
    // Persist the lock message
    DatabaseResponse db_response;
    db_manager->lock_object(db_response, obj_key, dev_key);
    if (!(db_response.success)) {
      if (db_response.error_code > NO_ERROR) {
        response_body->set_error_code(db_response.error_code);
      } else {
        response_body->set_error_code(PROCESSING_ERROR);
      }
      response_body->set_error_message(db_response.error_message);
    }
  }
  void process_unlock_message(std::string obj_key, std::string dev_key, ObjectListInterface *response_body) {
    logger.information("Processing Unlock Message");
    // Persist the unlock message
    DatabaseResponse db_response;
    db_manager->unlock_object(db_response, obj_key, dev_key);
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
  ObjectLockRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      ClusterManager *cluster, int mtype, std::string id, std::string dev_id) : logger(Poco::Logger::get("Data")) \
      {config=conf;msg_type=mtype;db_manager=db;cluster_manager=cluster;object_id.assign(id);device_id.assign(dev_id);}
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    logger.debug("Responding to Object Request");
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    ObjectListInterface *response_body = object_list_factory.build_json_object_list();
    response_body->set_msg_type(msg_type);
    response_body->set_error_code(NO_ERROR);

    try {
      if (msg_type == OBJ_LOCK) {
        process_lock_message(object_id, device_id, response_body);
      } else if (msg_type == OBJ_UNLOCK) {
        process_unlock_message(object_id, device_id, response_body);
      }
    } catch (std::exception& e) {
      logger.error("Exception encountered during DB Operation");
      response_body->set_error_message(e.what());
      logger.error(response_body->get_error_message());
      response_body->set_error_code(PROCESSING_ERROR);
    }

    if (response_body->get_error_code() == NOT_FOUND) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
    } else if (response_body->get_error_code() != NO_ERROR) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
    } else {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
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

#endif  // SRC_CONTROLLER_INCLUDE_OBJECT_LOCK_HANDLER_H_
