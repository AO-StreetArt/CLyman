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

// This defines some of the global variables that need to be accessed
// in a globally defined callback that takes no input parameters.
// This callback is defined in the main file and, when triggered by a signal,
// ensures that all of the resources allocated from AOSSL are deleted correctly.

#include "aossl/commandline/include/commandline_interface.h"
#include "aossl/commandline/include/factory_cli.h"

#include "aossl/mongo/include/mongo_interface.h"
#include "aossl/mongo/include/factory_mongo.h"

#include "aossl/logging/include/logging_interface.h"
#include "aossl/logging/include/factory_logging.h"

#include "aossl/redis/include/redis_interface.h"
#include "aossl/redis/include/factory_redis.h"

#include "aossl/uuid/include/uuid_interface.h"
#include "aossl/uuid/include/factory_uuid.h"

#include "aossl/zmq/include/zmq_interface.h"
#include "aossl/zmq/include/factory_zmq.h"

#include "configuration_manager.h"
#include "app_log.h"

#include "obj3_list.h"

#ifndef SRC_INCLUDE_GLOBALS_H_
#define SRC_INCLUDE_GLOBALS_H_

// Globals defined within this service
extern ConfigurationManager *config;

// Globals from the AO Shared Service Library
extern MongoInterface *mongo;
extern RedisInterface *red;
extern uuidInterface *uid;
extern Zmqio *zmqi;
extern CommandLineInterface *cli;

// Global Factory Objects
extern CommandLineInterpreterFactory *cli_factory;
extern MongoComponentFactory *mongo_factory;
extern RedisComponentFactory *redis_factory;
extern uuidComponentFactory *uuid_factory;
extern ZmqComponentFactory *zmq_factory;
extern LoggingComponentFactory *logging_factory;

extern Obj3List *inbound_message;
extern Obj3List *response_message;

// Shutdown the application
inline void shutdown() {
  // If necessary delete the inbound message object
  if (inbound_message) delete inbound_message;

  // Delete core objects
  if (mongo) delete mongo;
  if (red) delete red;
  if (zmqi) delete zmqi;
  if (config) delete config;
  if (uid) delete uid;
  if (cli) delete cli;

  // Shutdown logging
  shutdown_logging_submodules();
  if (logging) delete logging;

  // Shut down protocol buffer library
  google::protobuf::ShutdownProtobufLibrary();

  // Delete factories
  if (cli_factory) delete cli_factory;
  if (mongo_factory) delete mongo_factory;
  if (redis_factory) delete redis_factory;
  if (uuid_factory) delete uuid_factory;
  if (zmq_factory) delete zmq_factory;
  if (logging_factory) delete logging_factory;
}

#endif  // SRC_INCLUDE_GLOBALS_H_
