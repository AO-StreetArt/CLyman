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

#include "include/property_list_interface.h"
#include "include/object_list_factory.h"
#include "include/property_interface.h"
#include "include/object_factory.h"

#include "rapidjson/document.h"

#include "catch.hpp"

TEST_CASE( "Test Property List Data Structure", "[unit]" ) {
  std::cout << "Test Property List" << std::endl;
  const float TOLERANCE = 0.1f;
  ObjectListFactory olfactory;
  ObjectFactory ofactory;

  PropertyInterface *test_property = ofactory.build_property();
  test_property->set_key(std::string("12345"));
  test_property->set_name(std::string("testName"));
  test_property->set_parent(std::string("testParent"));
  test_property->set_scene(std::string("testScene"));
  test_property->set_asset_sub_id(std::string("testAssetSubId"));
  test_property->set_frame(1);
  test_property->set_timestamp(123456789);
  test_property->add_value(100.0);
  test_property->get_handle(0)->set_lh_type(std::string("vector"));
  test_property->get_handle(0)->set_lh_x(10.0);
  test_property->get_handle(0)->set_lh_y(5.0);
  test_property->get_handle(0)->set_rh_type(std::string("free"));
  test_property->get_handle(0)->set_rh_x(4.0);
  test_property->get_handle(0)->set_rh_y(3.0);

  PropertyListInterface *test_list = olfactory.build_json_property_list();
  test_list->add_prop(test_property);
  test_list->set_msg_type(1);
  test_list->set_transaction_id("123456789");
  test_list->set_num_records(2);

  // JSON Tests
  rapidjson::Document d;

  std::string json_string;
  test_list->to_msg_string(json_string);
  std::cout << json_string << std::endl;
  const char * json_cstr = json_string.c_str();
  d.Parse(json_cstr);

  PropertyListInterface *jparsed_list = olfactory.build_property_list(d);

  REQUIRE(jparsed_list->get_msg_type() == 1);
  REQUIRE(jparsed_list->get_transaction_id() == "123456789");
  REQUIRE(jparsed_list->get_num_records() == 1);
  REQUIRE(jparsed_list->num_props() == 1);

  REQUIRE(jparsed_list->get_prop(0)->num_values() == 1);
  REQUIRE(jparsed_list->get_prop(0)->get_key() == "12345");
  REQUIRE(jparsed_list->get_prop(0)->get_name() == "testName");
  REQUIRE(jparsed_list->get_prop(0)->get_parent() == "testParent");
  REQUIRE(jparsed_list->get_prop(0)->get_scene() == "testScene");
  REQUIRE(jparsed_list->get_prop(0)->get_asset_sub_id() == "testAssetSubId");
  REQUIRE(jparsed_list->get_prop(0)->get_frame() == 1);
  REQUIRE(jparsed_list->get_prop(0)->get_timestamp() == 123456789);
  REQUIRE(std::abs(jparsed_list->get_prop(0)->get_value(0)) - 100.0 < TOLERANCE);
  REQUIRE(jparsed_list->get_prop(0)->get_handle(0)->get_lh_type() == "vector");
  REQUIRE(std::abs(jparsed_list->get_prop(0)->get_handle(0)->get_lh_x()) - 10.0 < TOLERANCE);
  REQUIRE(std::abs(jparsed_list->get_prop(0)->get_handle(0)->get_lh_y()) - 5.0 < TOLERANCE);
  REQUIRE(jparsed_list->get_prop(0)->get_handle(0)->get_rh_type() == "free");
  REQUIRE(std::abs(jparsed_list->get_prop(0)->get_handle(0)->get_rh_x()) - 4.0 < TOLERANCE);
  REQUIRE(std::abs(jparsed_list->get_prop(0)->get_handle(0)->get_rh_y()) - 3.0 < TOLERANCE);

  // Teardown
  delete test_list;
  delete jparsed_list;
}
