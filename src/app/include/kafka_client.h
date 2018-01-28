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

#include <strings.h>
#include <string.h>
#include <cppkafka/producer.h>

#include <string>
#include <chrono>

#include "app_log.h"

#ifndef SRC_APP_INCLUDE_KAFKA_CLIENT_H_
#define SRC_APP_INCLUDE_KAFKA_CLIENT_H_

// Basic Kafka Client which lazy connects to a single instance
class KafkaClient {
cppkafka::Producer *kafka_producer = NULL;

 public:
  KafkaClient() {}
  ~KafkaClient() {if (kafka_producer) {delete kafka_producer;}}

  inline bool send(std::string msg, std::string broker_address) {
    main_logging->debug("Sending Update to Kafka:");
    main_logging->debug(broker_address);
    main_logging->debug(msg);
    if (!kafka_producer) {
      try {
        cppkafka::Configuration kafka_config = {
          { "metadata.broker.list", broker_address }
        };
        kafka_producer = new cppkafka::Producer(kafka_config);
        // Update the timeout for the new producer
        std::chrono::milliseconds timeout_duration(5000);
        kafka_producer->set_timeout(timeout_duration);
      }
      catch (std::exception& e) {
        main_logging->error("Exception encountered during Kafka Connection");
        main_logging->error(e.what());
        if (kafka_producer) {delete kafka_producer;}
        kafka_producer = NULL;
        return false;
      }
    }
    kafka_producer->produce(cppkafka::MessageBuilder("_dvs").partition(0).payload(msg));
    kafka_producer->flush();
    return true;
  }

};

#endif  // SRC_APP_INCLUDE_KAFKA_CLIENT_H_
