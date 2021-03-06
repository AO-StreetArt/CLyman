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

#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <utility>
#include <exception>
#include <thread>

#include "aossl/profile/include/tiered_app_profile.h"

#include "Poco/ThreadPool.h"
#include "Poco/Runnable.h"
#include "Poco/RunnableAdapter.h"
#include "Poco/Crypto/Cipher.h"
#include "Poco/Crypto/CipherFactory.h"
#include "Poco/Crypto/CipherKey.h"

#include "boost/asio.hpp"

#include "clyman_utils.h"
#include "cluster_manager.h"
#include "event_sender.h"

#include "db/include/database_manager.h"

#include "model/object/include/object_interface.h"
#include "model/property/include/property_interface.h"
#include "model/factory/include/json_factory.h"

#ifndef SRC_APP_INCLUDE_EVENT_STREAM_PROCESS_H_
#define SRC_APP_INCLUDE_EVENT_STREAM_PROCESS_H_

// Global atomic booleans for shutting down
std::atomic<bool> is_app_running(false);
std::atomic<bool> is_sender_running(false);

// Send UDP updates to downstream services
// also responsible for cleaning up the event memory
class EventSender : public Poco::Runnable {
  char *event = NULL;
  boost::asio::io_service *io_service = nullptr;
  EventStreamPublisher *publisher = nullptr;
  DatabaseManager *db_manager = nullptr;
  ClusterManager *cluster_manager = nullptr;
  Poco::Logger& logger;
  JsonFactory object_factory;
public:
  EventSender(char *evt, boost::asio::io_service &ios, DatabaseManager *db, EventStreamPublisher *pub, ClusterManager *cluster) : logger(Poco::Logger::get("Event")) {
    event = evt;
    io_service = &ios;
    db_manager = db;
    publisher = pub;
    cluster_manager = cluster;
  }
  virtual void run() {
    // Convert the input event to an Object
    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseStopWhenDoneFlag>(event);
    if (doc.HasParseError()) {
      logger.error("Error Parsing inbound Event:");
      logger.error(event);
      logger.error(rapidjson::GetParseError_En(doc.GetParseError()));
    } else {
      // Get the message type
      int msg_type = -9999;
      if (doc.IsObject()) {
        msg_type = find_json_int_elt_in_doc(doc, "msg_type");
      }
      if (msg_type > -9999) {
        std::string message;
        ObjectInterface* in_doc = nullptr;
        PropertyInterface* in_prop = nullptr;
        // Build the input and output documents
        if (msg_type == OBJ_UPD || msg_type == OBJ_ACTION_UPD || msg_type == OBJ_FRAME_UPD) {
          in_doc = object_factory.build_object(doc);
          message.assign(in_doc->get_scene() + \
              std::string("\n") + in_doc->to_transform_json());
        } else if (msg_type == PROP_UPD || msg_type == PROP_ACTION_UPD || msg_type == PROP_FRAME_UPD) {
          in_prop = object_factory.build_property(doc);
          in_prop->to_json(message);
          message.assign(in_prop->get_scene() + std::string("\n") + message);
        }
        // Send an update to downstream services
        AOSSL::ServiceInterface *downstream = cluster_manager->get_ivan();
        if (downstream) {
          publisher->publish_event(message.c_str(), \
              downstream->get_address(), stoi(downstream->get_port()));
        }
        // Persist the update message
        std::string new_object_key;
        DatabaseResponse response;
        try {
          if (msg_type == OBJ_UPD) {
            new_object_key = in_doc->get_key();
            db_manager->update_object(response, in_doc, new_object_key);
          } else if (msg_type == PROP_UPD) {
            new_object_key = in_prop->get_key();
            db_manager->update_property(response, in_prop, new_object_key);
          } else if (msg_type == OBJ_ACTION_UPD) {
            new_object_key = in_doc->get_key();
            for (auto action_itr = in_doc->get_actions()->begin(); action_itr != in_doc->get_actions()->end(); ++action_itr) {
              std::string action_name(action_itr->first);
              db_manager->update_action(response, new_object_key, action_itr->second, action_name);
              if (!(response.success)) {
                logger.error("Error Updating Action in Database:");
                logger.error(response.error_message);
              }
            }
          } else if (msg_type == PROP_ACTION_UPD) {
            new_object_key = in_prop->get_key();
            for (auto action_itr = in_prop->get_actions()->begin(); action_itr != in_prop->get_actions()->end(); ++action_itr) {
              std::string action_name(action_itr->first);
              db_manager->update_action(response, new_object_key, action_itr->second, action_name);
              if (!(response.success)) {
                logger.error("Error Updating Action in Database:");
                logger.error(response.error_message);
              }
            }
          } else if (msg_type == OBJ_FRAME_UPD) {
            new_object_key = in_doc->get_key();
            for (auto action_itr = in_doc->get_actions()->begin(); action_itr != in_doc->get_actions()->end(); ++action_itr) {
              std::string action_name(action_itr->first);
              for (auto keyframe_itr = action_itr->second->get_keyframes()->begin(); keyframe_itr != action_itr->second->get_keyframes()->end(); ++keyframe_itr) {
                db_manager->update_keyframe(response, new_object_key, action_name, keyframe_itr->second, keyframe_itr->first);
                if (!(response.success)) {
                  logger.error("Error Updating Frame in Database:");
                  logger.error(response.error_message);
                }
              }
            }
          } else if (msg_type == PROP_FRAME_UPD) {
            new_object_key = in_prop->get_key();
            for (auto action_itr = in_prop->get_actions()->begin(); action_itr != in_prop->get_actions()->end(); ++action_itr) {
              std::string action_name(action_itr->first);
              for (auto keyframe_itr = action_itr->second->get_keyframes()->begin(); keyframe_itr != action_itr->second->get_keyframes()->end(); ++keyframe_itr) {
                db_manager->update_keyframe(response, new_object_key, action_name, keyframe_itr->second, keyframe_itr->first);
                if (!(response.success)) {
                  logger.error("Error Updating Frame in Database:");
                  logger.error(response.error_message);
                }
              }
            }
          }
        } catch (std::exception& e) {
          logger.error("Error Persisting Update: ");
          logger.error(e.what());
        }
        if (!(response.success)) {
          logger.error("Error Persisting Update: ");
          logger.error(response.error_message);
        }
      } else {
          logger.error("Input recieved without message type");
      }
    }

    // Memory cleanup
    delete[] event;
  }
};

// Central Event Stream Process, which listens on a UDP port
void event_stream(AOSSL::TieredApplicationProfile *config, DatabaseManager *db, EventStreamPublisher *publisher, ClusterManager *cluster) {
  Poco::Logger& logger = Poco::Logger::get("Event");
  Poco::ThreadPool tpool(2, 13, 60, 0);
  logger.information("Starting Event Stream");
  is_sender_running = true;
  std::vector<EventSender*> evt_senders {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  try {
    // Get the udp port out of the configuration profile
    AOSSL::StringBuffer udp_port;
    config->get_opt(std::string("udp.port"), udp_port);
    int port = std::stoi(udp_port.val);
    bool aes_enabled = false;
    int sender_index = 0;
    // Open the UDP Socket
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
    // Listen on the UDP Socket
    while (is_app_running.load()) {
      // Get security config, having it inside the loop allows
      // dynamic security updates
      AOSSL::StringBuffer aes_enabled_buffer;
      AOSSL::StringBuffer aesin_key_buffer;
      AOSSL::StringBuffer aesin_iv_buffer;
      config->get_opt(std::string("event.security.aes.enabled"), aes_enabled_buffer);
      config->get_opt(config->get_cluster_name() + \
          std::string(".event.security.in.aes.key"), aesin_key_buffer);
      config->get_opt(config->get_cluster_name() + \
          std::string(".event.security.in.aes.iv"), aesin_iv_buffer);
      if (aes_enabled_buffer.val == "true") aes_enabled = true;
      boost::asio::ip::udp::endpoint remote_endpoint;
      boost::system::error_code error;

      // First, we call recieve_from on the socket with a null buffer,
      // which returns when a message is on the recieve queue with the number
      // of bytes in the message
      socket.receive_from(boost::asio::null_buffers(), remote_endpoint, 0, error);
      int available = socket.available();

      // Build a buffer and recieve a message
      char recv_buf[available];
      boost::asio::mutable_buffers_1 bbuffer = boost::asio::buffer(recv_buf, available);
      int bytes_transferred = socket.receive_from(bbuffer, remote_endpoint, 0, error);
      char* event_data_ptr = boost::asio::buffer_cast<char*>(bbuffer);
      if (!(error && error != boost::asio::error::message_size && bytes_transferred > 0) \
          && is_app_running.load()) {
        logger.debug("Recieved UDP Update");

        // Copy the message buffer into dynamic memory
        char *event_msg = new char[available+1]();
        memcpy(event_msg, event_data_ptr, bytes_transferred);

        // If necessary, decrypt the message
        if (aes_enabled) {
          std::string event_string(event_msg);
          Poco::Crypto::CipherFactory& factory = Poco::Crypto::CipherFactory::defaultFactory();
          std::vector<unsigned char> decrypt_key_vect(aesin_key_buffer.val.begin(), aesin_key_buffer.val.end());
          std::vector<unsigned char> decrypt_iv_vect(aesin_iv_buffer.val.begin(), aesin_iv_buffer.val.end());
          Poco::Crypto::Cipher* dCipher = \
              factory.createCipher(Poco::Crypto::CipherKey("aes-256-cbc", \
              decrypt_key_vect, decrypt_iv_vect));
          std::string decrypted = dCipher->decryptString(event_string, \
              Poco::Crypto::Cipher::ENC_BASE64);
          memcpy(event_msg, decrypted.c_str(), decrypted.size());
        }
        logger.debug(event_msg);

        // Clear out any left-over event sender
        if (evt_senders[sender_index]) delete evt_senders[sender_index];
        // Build the new event sender
        evt_senders[sender_index] = new EventSender(event_msg, io_service, db, publisher, cluster);

        // Send the event
        if (sender_index == 12) {
            // If we have used up all the space in our array of senders,
            // then we should use the main event thread to send and wait
            // for other threads to complete before pulling the next message.
            tpool.joinAll();
            sender_index = 0;
        }
        try {
          // Fire off another thread to actually process events
          tpool.start(*(evt_senders[sender_index]));
          sender_index = sender_index + 1;
        } catch (Poco::NoThreadAvailableException& e) {
          // If no more threads are available, then execute the updates on the
          // main event thread, and wait for other threads to complete before
          // pulling the next message.
          evt_senders[sender_index]->run();
          tpool.joinAll();
          sender_index = 0;
        }
      }
    }
  } catch (std::exception& e) {
    logger.error(e.what());
  }
  for (EventSender *sender : evt_senders) {
    if (sender) delete sender;
  }
  is_sender_running = false;
}

#endif  // SRC_APP_INCLUDE_EVENT_STREAM_PROCESS_H_
