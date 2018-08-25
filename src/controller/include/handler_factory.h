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

#include "user/include/account_manager_interface.h"

#include "heartbeat_handler.h"
#include "object_base_handler.h"
#include "object_lock_handler.h"
#include "app/include/event_sender.h"
#include "app/include/database_manager.h"
#include "app/include/cluster_manager.h"

#include "aossl/profile/include/tiered_app_profile.h"

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Base64Decoder.h"

#ifndef SRC_CONTROLLER_INCLUDE_SCENE_HANDLER_FACTORY_H_
#define SRC_CONTROLLER_INCLUDE_SCENE_HANDLER_FACTORY_H_

class ObjectHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory {
  AOSSL::TieredApplicationProfile *config = NULL;
  DatabaseManager *db_manager = nullptr;
  AccountManagerInterface *accounts = NULL;
  EventStreamPublisher *publisher = nullptr;
  ClusterManager *cluster_info = nullptr;
 public:
  ObjectHandlerFactory(AOSSL::TieredApplicationProfile *conf, AccountManagerInterface *accts, DatabaseManager *db, EventStreamPublisher *pub, ClusterManager *cluster) \
    {config=conf;accounts = accts;db_manager=db;publisher=pub;cluster_info=cluster;}
  Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) {
    // Authentication Routine
    char *buffer = NULL;
    // Determine if we have any credentials
    bool authentication_failure = false;
    if (accounts) {
      if (request.hasCredentials()) {
        std::string scheme;
        std::string req_auth_info;
        std::string username;
        std::string password;
        request.getCredentials(scheme, req_auth_info);
        Poco::Logger::get("Controller").debug("Authentication Type: %s", scheme);
        if (accounts->auth_type() == IVAN_BASIC_AUTH) {
          if (scheme == "Basic") {
            // Pull and decode the authentication info
            std::istringstream istr(req_auth_info);
            Poco::Base64Decoder b64in(istr);
            int length = 100;
            buffer = new char[length];
            b64in.read(buffer, length);
            std::string auth_info_str(buffer);
            // Split the username and password string
            std::stringstream auth_info_stream(auth_info_str);
            std::getline(auth_info_stream, username, ':');
            std::getline(auth_info_stream, password);
          } else {
            // invalid auth type
            Poco::Logger::get("Controller").error("Invalid authentication type");
            authentication_failure = true;
          }
        }

        // Actually authenticate against the Account Manager Interface
        if (!(accounts->authenticate_user(username, password))) {
          // User has failed authentication, return error
          Poco::Logger::get("Controller").error("Authentication failed");
          authentication_failure = true;
        } else {
          Poco::Logger::get("Controller").debug("Authentication success");
        }
      } else {
        // We require credentials but haven't been provided any
        Poco::Logger::get("Controller").error("Credentials not provided");
        authentication_failure = true;
      }
    }
    if (buffer) delete buffer;
    if (authentication_failure) return NULL;

    // Parse the URI Path
    Poco::Logger::get("Controller").information("Responding to Request at: %s", request.getURI());
    Poco::URI request_uri(request.getURI());
    std::vector<std::string> uri_path;
    request_uri.getPathSegments(uri_path);

    for (std::string elt : uri_path) {
      Poco::Logger::get("Controller").debug("URI Element: %s", elt);
    }

    auto query_params = request_uri.getQueryParameters();

    // Build a request handler for the message
    if (uri_path.size() > 1 && uri_path[0] == "v1" && request.getMethod() == "POST") {
      if (uri_path.size() == 2 && uri_path[1] == "object") {
        // Create
        return new ObjectBaseRequestHandler(config, db_manager, publisher, cluster_info, OBJ_CRT);
      } else if (uri_path.size() == 3 && uri_path[1] == "object" && uri_path[2] == "query") {
        // Query
        return new ObjectBaseRequestHandler(config, db_manager, publisher, cluster_info, OBJ_QUERY);
      } else if (uri_path.size() > 3 && uri_path[1] == "object" && uri_path[3] == "lock") {
        // Lock
        for (auto param : query_params) {
          if (param.first == "device") {
            return new ObjectLockRequestHandler(config, db_manager, cluster_info, OBJ_LOCK, uri_path[2], param.second);
          }
        }
      } else if (uri_path.size() == 3 && uri_path[1] == "object") {
        // Update
        return new ObjectBaseRequestHandler(config, db_manager, publisher, cluster_info, OBJ_UPD, uri_path[2]);
      }
    } else if (uri_path.size() > 1 && uri_path[0] == "v1" && request.getMethod() == "GET") {
      if (uri_path.size() == 3 && uri_path[1] == "object") {
        // Get
        return new ObjectKeyRequestHandler(config, db_manager, cluster_info, OBJ_GET, uri_path[2]);
      }
    } else if (uri_path.size() > 1 && uri_path[0] == "v1" && request.getMethod() == "DELETE") {
      if (uri_path.size() > 3 && uri_path[1] == "object" && uri_path[3] == "lock") {
        // Unlock
        for (auto param : query_params) {
          if (param.first == "device") {
            return new ObjectLockRequestHandler(config, db_manager, cluster_info, OBJ_UNLOCK, uri_path[2], param.second);
          }
        }
      } else if (uri_path.size() == 3 && uri_path[1] == "object") {
        // Delete
        return new ObjectKeyRequestHandler(config, db_manager, cluster_info, OBJ_DEL);
      }
    } else if (uri_path.size() == 1 && uri_path[0] == "health" && \
        request.getMethod() == "GET") {
      return new HeartbeatHandler();
    }
    return NULL;
  }
};

#endif  // SRC_CONTROLLER_INCLUDE_SCENE_HANDLER_FACTORY_H_
