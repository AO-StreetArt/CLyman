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
#include "database_manager.h"

#ifndef SRC_APP_INCLUDE_EVENT_STREAM_PROCESS_H_
#define SRC_APP_INCLUDE_EVENT_STREAM_PROCESS_H_

// 24 bit salt value
// 26 bit scene key
// 1 bit for new line delimiter
// 179 bits for object json
// 1 bit for final new line
const int EVENT_LENGTH = 275;

// Global atomic booleans for shutting down
std::atomic<bool> is_app_running(false);
std::atomic<bool> is_sender_running(false);

// Send UDP updates to downstream services
// also responsible for cleaning up the event memory
// TO-DO: Socket Pool Implementation
class EventSender : public Poco::Runnable {
  char *event = NULL;
  boost::asio::io_service *io_service = nullptr;
  DatabaseManager *db_manager = nullptr;
  bool encrypted = false;
  bool decrypted = false;
  std::string encrypt_key;
  std::string encrypt_salt;
  std::string decrypt_key;
  std::string decrypt_salt;
  Poco::Logger& logger;
public:
  EventSender(char *evt, boost::asio::io_service &ios, DatabaseManager *db) : logger(Poco::Logger::get("Event")) {
    event = evt;
    io_service = &ios;
    db_manager = db;
  }
  EventSender(char *evt, boost::asio::io_service &ios, DatabaseManager *db, std::string& epasswd, std::string& esalt, std::string& dpasswd, std::string& dsalt) : logger(Poco::Logger::get("Event"))  {
    event = evt;
    io_service = &ios;
    db_manager = db;
    encrypted = true;
    decrypted = true;
    encrypt_key.assign(epasswd);
    encrypt_salt.assign(esalt);
    decrypt_key.assign(dpasswd);
    decrypt_salt.assign(dsalt);
  }
  virtual void run() {
    logger.debug("Sending Object Updates");
    Poco::Crypto::CipherFactory& factory = Poco::Crypto::CipherFactory::defaultFactory();
    // Creates a 256-bit AES cipher (one for encryption, one for decryption)
    Poco::Crypto::Cipher* eCipher = factory.createCipher(Poco::Crypto::CipherKey("aes-256-cbc", encrypt_key, encrypt_salt));
    Poco::Crypto::Cipher* dCipher = factory.createCipher(Poco::Crypto::CipherKey("aes-256-cbc", decrypt_key, decrypt_salt));
    // Build the event string
    std::string event_string(event);
    // Build a UDP Socket to send our messages
    boost::asio::ip::udp::socket socket(*io_service);
    socket.open(boost::asio::ip::udp::v4());

    // TO-DO: Construct the outgoing change stream message
    std::string outgoing_event;
    //TO-DO: Get the address of the downstream service
    std::string service_host;
    int service_port;

    // Send the message to a downstream service
    try {
      boost::asio::ip::udp::endpoint remote_endpoint;
      remote_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(service_host), service_port);
      boost::system::error_code err;
      if (encrypted) {
        std::string encrypted = eCipher->encryptString(outgoing_event, Poco::Crypto::Cipher::ENC_BASE64);
        socket.send_to(boost::asio::buffer(encrypted, encrypted.size()), remote_endpoint, 0, err);
      } else {
        socket.send_to(boost::asio::buffer(outgoing_event, outgoing_event.size()), remote_endpoint, 0, err);
      }
    } catch (std::exception& e) {
      logger.error(e.what());
    }

    // TO-DO: Persist the update to the DB

    // Memory cleanup
    delete[] event;
    delete eCipher;
    delete dCipher;
  }
};

// Central Event Stream Process, which listens on a UDP port
void event_stream(AOSSL::TieredApplicationProfile *config, DatabaseManager *db) {
  Poco::Logger& logger = Poco::Logger::get("Event");
  logger.information("Starting Event Stream");
  is_sender_running = true;
  std::vector<EventSender*> evt_senders {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  try {
    // Get the configuration values out of the configuration profile
    AOSSL::StringBuffer aes_enabled_buffer;
    AOSSL::StringBuffer aesin_key_buffer;
    AOSSL::StringBuffer aesin_salt_buffer;
    AOSSL::StringBuffer aesout_key_buffer;
    AOSSL::StringBuffer aesout_salt_buffer;
    AOSSL::StringBuffer udp_port;
    config->get_opt(std::string("udp.port"), udp_port);
    config->get_opt(std::string("event.security.aes.enabled"), aes_enabled_buffer);
    config->get_opt(config->get_cluster_name() + \
        std::string(".event.security.out.aes.key"), aesout_key_buffer);
    config->get_opt(config->get_cluster_name() + \
        std::string(".event.security.out.aes.salt"), aesout_salt_buffer);
    config->get_opt(config->get_cluster_name() + \
        std::string(".event.security.in.aes.key"), aesin_key_buffer);
    config->get_opt(config->get_cluster_name() + \
        std::string(".event.security.in.aes.salt"), aesin_salt_buffer);
    int port = std::stoi(udp_port.val);
    bool aes_enabled = false;
    if (aes_enabled_buffer.val == "true") aes_enabled = true;
    int sender_index = 0;
    // Open the UDP Socket
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
    // Listen on the UDP Socket
    while (is_app_running.load()) {
      // Build a buffer and recieve a message
      char recv_buf[EVENT_LENGTH];
      boost::asio::mutable_buffers_1 bbuffer = boost::asio::buffer(recv_buf);
      boost::asio::ip::udp::endpoint remote_endpoint;
      boost::system::error_code error;
      int bytes_transferred = socket.receive_from(bbuffer, remote_endpoint, 0, error);
      char* event_data_ptr = boost::asio::buffer_cast<char*>(bbuffer);
      if (!(error && error != boost::asio::error::message_size && bytes_transferred > 0) \
          && is_app_running.load()) {
        logger.debug("Recieved UDP Update");
        // Copy the message buffer into dynamic memory
        char *event_msg = new char[EVENT_LENGTH+1]();
        memcpy(event_msg, event_data_ptr, bytes_transferred);
        logger.debug(event_msg);
        // Clear out any left-over event sender
        if (evt_senders[sender_index]) delete evt_senders[sender_index];
        // Build the new event sender
        if (aes_enabled) {
          evt_senders[sender_index] = new EventSender(event_msg, io_service, db, aesout_key_buffer.val, aesout_salt_buffer.val, aesin_key_buffer.val, aesin_salt_buffer.val);
        } else {
          evt_senders[sender_index] = new EventSender(event_msg, io_service, db);
        }
        // Fire off another thread to actually send UDP messages
        try {
          Poco::ThreadPool::defaultPool().start(*(evt_senders[sender_index]));
          sender_index = sender_index + 1;
        } catch (Poco::NoThreadAvailableException& e) {
          // If no more threads are available, then execute the updates on the
          // main thread, and wait for other threads to complete before pulling
          // the next message.
          evt_senders[sender_index]->run();
          Poco::ThreadPool::defaultPool().joinAll();
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
