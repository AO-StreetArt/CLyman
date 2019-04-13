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
#include "model/factory/include/data_factory.h"
#include "model/factory/include/data_list_factory.h"
#include "model/list/include/object_list_interface.h"
#include "clyman_handler.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

#ifndef SRC_CONTROLLER_INCLUDE_ASSET_HANDLER_H_
#define SRC_CONTROLLER_INCLUDE_ASSET_HANDLER_H_

class AssetRequestHandler: public ClymanHandler, public Poco::Net::HTTPRequestHandler {
  std::string object_id;
  std::string asset_id;
  void process_add_message(std::string obj_key, std::string asset_key, ObjectListInterface *response_body) {
    logger.information("Processing Asset Add Message");
    // Persist the asset
    DatabaseResponse db_response;
    ObjectInterface *upd_obj = object_factory.build_object();
    upd_obj->add_asset(asset_key);
    db_manager->update_object(db_response, upd_obj, obj_key, true);
    if (!(db_response.success)) {
      if (db_response.error_code > NO_ERROR) {
        response_body->set_error_code(db_response.error_code);
      } else {
        response_body->set_error_code(PROCESSING_ERROR);
      }
      response_body->set_error_message(db_response.error_message);
    }
    delete upd_obj;
  }
  void process_remove_message(std::string obj_key, std::string asset_key, ObjectListInterface *response_body) {
    logger.information("Processing Asset Remove Message");
    // Remove the asset
    DatabaseResponse db_response;
    ObjectInterface *upd_obj = object_factory.build_object();
    upd_obj->add_asset(asset_key);
    db_manager->update_object(db_response, upd_obj, obj_key, false);
    if (!(db_response.success)) {
      if (db_response.error_code > NO_ERROR) {
        response_body->set_error_code(db_response.error_code);
      } else {
        response_body->set_error_code(PROCESSING_ERROR);
      }
      response_body->set_error_message(db_response.error_message);
    }
    delete upd_obj;
  }
 public:
  AssetRequestHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      ClusterManager *cluster, int mtype, std::string object, std::string asset) : ClymanHandler(conf, db, nullptr, cluster, mtype) {
    object_id.assign(object);
    asset_id.assign(asset);
  }
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
    logger.debug("Responding to Asset Request");
    ObjectListInterface *response_body = object_list_factory.build_json_object_list();
    ClymanHandler::init_response(response, response_body);

    try {
      if (msg_type == ASSET_ADD) {
        process_add_message(object_id, asset_id, response_body);
      } else if (msg_type == ASSET_DEL) {
        process_remove_message(object_id, asset_id, response_body);
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
