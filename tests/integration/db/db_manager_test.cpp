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

#include <string>
#include <iostream>

#include "aossl/profile/include/network_app_profile.h"

#include "include/object_list_interface.h"
#include "include/object_list_factory.h"
#include "include/object_interface.h"
#include "include/object_factory.h"
#include "include/transforms.h"

#include "include/clyman_utils.h"
#include "include/database_manager.h"

#include "catch.hpp"

#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"

#include "Poco/Logger.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/Message.h"
#include "Poco/NumberParser.h"
#include "Poco/AutoPtr.h"
#include "Poco/AsyncChannel.h"
#include "Poco/ErrorHandler.h"

ObjectInterface* build_test_document(std::string key, std::string name, \
  std::string scene, std::string type, std::string subtype, std::string owner, \
  int frame, int tstamp) {
    ObjectFactory object_factory;
    ObjectInterface *test_object = object_factory.build_object();
    test_object->set_key(key);
    test_object->set_name(name);
    test_object->set_scene(scene);
    test_object->set_type(type);
    test_object->set_subtype(subtype);
    test_object->set_owner(owner);
    test_object->set_frame(frame);
    test_object->set_timestamp(tstamp);
    return test_object;
}

TEST_CASE( "Test Database Manager", "[integration]" ) {
  std::cout << "Database Manager Test" << std::endl;
  // Build the test profile
  std::vector<std::string> cli_args;
  std::string app_name = "tests";
  cli_args.push_back(std::string(app_name));
  AOSSL::NetworkApplicationProfile prof(cli_args);

  // Build the test app and logger
  // Poco::Util::Application& app = Poco::Util::Application::instance();
  // Poco::AutoPtr<Poco::ConsoleChannel> pCons(new Poco::ConsoleChannel);
  // Poco::AutoPtr<Poco::AsyncChannel> pAsync(new Poco::AsyncChannel(pCons));
  // Poco::FormattingChannel* console_channel = \
  //   new Poco::FormattingChannel(new Poco::PatternFormatter("%Y-%m-%d %H:%M:%S.%c %N[%P]:%s:%q:%t"));
  // console_channel->setChannel(pAsync);
  // console_channel->open();
  // int log_priority = Poco::Message::PRIO_DEBUG;
  // Poco::Logger& database_manager_logger = Poco::Logger::create("DatabaseManager", \
  //   console_channel, log_priority);

  // Build the DB Manager
  DatabaseManager db(&prof, std::string("mongodb://localhost:27017"), \
      std::string("test"), std::string("test"));

  ObjectListFactory object_list_factory;
  ObjectFactory object_factory;

  // Setup test objects
  std::string key1 = "zabcdef";
  std::string key2 = "zabcdefg";
  std::string name1 = "zabcdefgh";
  std::string name2 = "zabcdefghi";
  std::string scene1 = "zabcdefghij";
  std::string scene2 = "zabcdefghijk";
  std::string type1 = "zabcdefghijkl";
  std::string type2 = "zabcdefghijklm";
  std::string subtype1 = "zabcdefghijklmn";
  std::string subtype2 = "zabcdefghijklmno";
  std::string owner1 = "z1234567";
  std::string owner2 = "z123456789";

  ObjectInterface *test_object = build_test_document(std::string(""), name1, scene1, \
    type1, subtype1, std::string(""), 0, 12345678);
  ObjectInterface *test_object2 = build_test_document(std::string(""), name2, scene2, \
    type2, subtype2, std::string(""), 1, 43214567);

  std::string asset1 = "12345";
  std::string asset2 = "12346";
  std::string asset3 = "12347";
  test_object->add_asset(asset1);
  test_object->add_asset(asset2);
  test_object2->add_asset(asset3);

  // Creation test
  DatabaseResponse db_response;
  std::string new_key;
  db.create_object(db_response, test_object, new_key);
  std::cout << db_response.error_message << std::endl;
  // Test that we have a successful result
  REQUIRE(db_response.success);
  // Test that the returned key is not empty
  REQUIRE(!(new_key.empty()));

  // Retrieve test
  ObjectListInterface *get_response = object_list_factory.build_json_object_list();
  db.get_object(get_response, new_key);
  std::cout << get_response->get_error_message() << std::endl;
  // Test that we found a return object from the DB
  REQUIRE(get_response->num_objects() > 0);
  // Validate that the object saved is what is returned
  REQUIRE(get_response->get_object(0)->get_name() == name1);
  REQUIRE(get_response->get_object(0)->get_scene() == scene1);
  REQUIRE(get_response->get_object(0)->get_type() == type1);
  REQUIRE(get_response->get_object(0)->get_subtype() == subtype1);
  REQUIRE(get_response->get_object(0)->get_frame() == 0);
  REQUIRE(get_response->get_object(0)->get_timestamp() == 12345678);
  REQUIRE(get_response->get_object(0)->num_assets() == 2);
  // Test that we have a success error code
  REQUIRE(get_response->get_error_code() == NO_ERROR);
  delete get_response;

  // Update test
  DatabaseResponse upd_response;
  db.update_object(upd_response, test_object2, new_key);
  std::cout << upd_response.error_message << std::endl;
  // Test that we have a successful result
  REQUIRE(upd_response.success);

  // Query Test
  ObjectListInterface *query_response = object_list_factory.build_json_object_list();
  ObjectInterface *query_object = object_factory.build_object();
  query_object->set_name(name2);
  db.query(query_response, query_object, 2);
  std::cout << get_response->get_error_message() << std::endl;
  // Test that we found a return object from the DB
  REQUIRE(query_response->num_objects() > 0);
  // Validate that the object saved is what is returned
  REQUIRE(query_response->get_object(0)->get_name() == name2);
  REQUIRE(query_response->get_object(0)->get_scene() == scene2);
  REQUIRE(query_response->get_object(0)->get_type() == type2);
  REQUIRE(query_response->get_object(0)->get_subtype() == subtype2);
  REQUIRE(query_response->get_object(0)->get_frame() == 1);
  REQUIRE(query_response->get_object(0)->get_timestamp() == 43214567);
  REQUIRE(query_response->get_object(0)->num_assets() == 3);
  // Test that we have a success error code
  REQUIRE(query_response->get_error_code() == NO_ERROR);
  delete query_object;
  delete query_response;

  // Lock Test
  DatabaseResponse lock_response;
  db.lock_object(lock_response, new_key, owner1);
  std::cout << lock_response.error_message << std::endl;
  REQUIRE(lock_response.success);

  // Try to obtain the lock with another device
  DatabaseResponse bad_lock_response;
  db.lock_object(bad_lock_response, new_key, owner2);
  std::cout << bad_lock_response.error_message << std::endl;
  REQUIRE(!(bad_lock_response.success));

  // Unlock Test
  DatabaseResponse unlock_response;
  db.unlock_object(unlock_response, new_key, owner1);
  std::cout << unlock_response.error_message << std::endl;
  REQUIRE(unlock_response.success);

  // Try to obtain the lock with another device
  DatabaseResponse second_lock_response;
  db.lock_object(second_lock_response, new_key, owner2);
  std::cout << second_lock_response.error_message << std::endl;
  REQUIRE(!(second_lock_response.success));

  // Delete Test
  DatabaseResponse del_response;
  db.delete_object(del_response, new_key);
  std::cout << del_response.error_message << std::endl;
  // Test that we have a successful result
  REQUIRE(del_response.success);

  // Validate the object is no longer present
  ObjectListInterface *get2_response = object_list_factory.build_json_object_list();
  db.get_object(get2_response, new_key);
  std::cout << get2_response->get_error_message() << std::endl;
  // Test that we found a return object from the DB
  REQUIRE(get2_response->num_objects() == 0);
  // Test that we have a success error code
  REQUIRE(get2_response->get_error_code() == NO_ERROR);
  delete get2_response;

  delete test_object;
  delete test_object2;
}
