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

// Base Class with core shared data elements of request handlers

#include <iostream>
#include <boost/cstdint.hpp>

#include "aossl/core/include/kv_store_interface.h"

#include "app/include/cluster_manager.h"
#include "app/include/event_sender.h"
#include "db/include/db_manager_interface.h"
#include "model/factory/include/json_factory.h"
#include "model/factory/include/data_list_factory.h"
#include "model/factory/include/data_factory.h"

#include "Poco/Logger.h"
#include "Poco/Net/HTTPServerResponse.h"

#ifndef SRC_CONTROLLER_INCLUDE_CLYMAN_HANDLER_H_
#define SRC_CONTROLLER_INCLUDE_CLYMAN_HANDLER_H_

class ClymanHandler {
 protected:
  // Handler Data Elements
  AOSSL::KeyValueStoreInterface *config = nullptr;
  DatabaseManagerInterface *db_manager = nullptr;
  int msg_type = -1;
  DataListFactory object_list_factory;
  JsonFactory json_factory;
  DataFactory object_factory;
  EventStreamPublisher *publisher = nullptr;
  ClusterManager *cluster_manager = nullptr;
  Poco::Logger& logger;

  // Constructor
  ClymanHandler(AOSSL::KeyValueStoreInterface *conf, DatabaseManagerInterface *db, \
      EventStreamPublisher *pub, ClusterManager *cluster, int mtype) : logger(Poco::Logger::get("Data")) {
    config=conf;
    msg_type=mtype;
    db_manager=db;
    publisher=pub;
    cluster_manager=cluster;
  }

  // Destructor
  virtual ~ClymanHandler() {}

  // Initialize an HTTP Response and the Response Body
  void init_response(Poco::Net::HTTPServerResponse& response, DataListInterface *response_body) {
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    response_body->set_msg_type(msg_type);
    response_body->set_error_code(NO_ERROR);
  }
};
#endif  // SRC_CONTROLLER_INCLUDE_CLYMAN_HANDLER_H_
