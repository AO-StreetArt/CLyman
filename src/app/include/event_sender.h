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
#include "db/include/database_manager.h"

#ifndef SRC_APP_INCLUDE_EVENT_SENDER_H_
#define SRC_APP_INCLUDE_EVENT_SENDER_H_

/*
 * Event Stream Publisher is responsible for handling any incoming transactions
 * and events.  It is responsible for ensure that all updates are passed to
 * downstream, and has a thread pool to allow for multi-threaded processing.
 * It is responsible for transmission of events to all downstream sources,
 * including both the DB and Event Publishing Service (ie. Crazy Ivan).
 */
class EventStreamPublisher {
  boost::asio::io_service io_service;
  bool encryption_active = false;
  std::string encrypt_key;
  std::string encrypt_iv;
  Poco::Logger& logger;
  int sender_index = 0;
 public:
  EventStreamPublisher(std::string& ekey, std::string& eiv) \
      : logger(Poco::Logger::get("Event")) {
    encryption_active = true;
    encrypt_key.assign(ekey);
    encrypt_iv.assign(eiv);
  }
  EventStreamPublisher() : logger(Poco::Logger::get("Event")) {}
  ~EventStreamPublisher() {}
  /*
   * Publish an event to a remote sender
   */
  inline void publish_event(const char *event_msg, std::string host, int port) {
    logger.debug("Sending Object Updates");
    Poco::Crypto::CipherFactory& factory = Poco::Crypto::CipherFactory::defaultFactory();
    // Creates a 256-bit AES cipher (one for encryption, one for decryption)
    std::vector<unsigned char> encrypt_key_vect(encrypt_key.begin(), encrypt_key.end());
    std::vector<unsigned char> encrypt_iv_vect(encrypt_iv.begin(), encrypt_iv.end());
    Poco::Crypto::Cipher* eCipher = factory.createCipher(Poco::Crypto::CipherKey("aes-256-cbc", encrypt_key_vect, encrypt_iv_vect));
    // Build a UDP Socket to send our messages
    boost::asio::ip::udp::socket socket(io_service);
    socket.open(boost::asio::ip::udp::v4());

    // Construct the outgoing change stream message
    std::string outgoing_event(event_msg);

    // Send the message to a downstream service
    try {
      boost::asio::ip::udp::endpoint remote_endpoint;
      remote_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(host), port);
      boost::system::error_code err;
      if (encryption_active) {
        std::string encryption_active = eCipher->encryptString(outgoing_event, Poco::Crypto::Cipher::ENC_BASE64);
        socket.send_to(boost::asio::buffer(encryption_active, encryption_active.size()), remote_endpoint, 0, err);
      } else {
        socket.send_to(boost::asio::buffer(outgoing_event, outgoing_event.size()), remote_endpoint, 0, err);
      }
    } catch (std::exception& e) {
      logger.error(e.what());
    }

    // Memory cleanup
    delete eCipher;
  }
};

#endif  // SRC_APP_INCLUDE_EVENT_SENDER_H_
