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
#include <cmath>

#include "include/json_property.h"
#include "include/animation_action.h"
#include "include/animation_graph_handle.h"
#include "include/property_frame.h"
#include "include/json_factory.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include "catch.hpp"

TEST_CASE( "Test Animation Property Data Structure", "[unit]" ) {
  //Tolerance
  const float TOLERANCE = 0.1f;

  // Create a test Property
  JsonProperty test_property;
  test_property.set_key(std::string("12345"));
  test_property.set_name(std::string("testName"));
  test_property.set_parent(std::string("testParent"));
  test_property.set_scene(std::string("testScene"));
  test_property.set_asset_sub_id(std::string("testAssetSubId"));
  test_property.add_value(100.0);
  REQUIRE(test_property.num_values() == 1);
  REQUIRE(test_property.get_key() == "12345");
  REQUIRE(test_property.get_name() == "testName");
  REQUIRE(test_property.get_parent() == "testParent");
  REQUIRE(test_property.get_scene() == "testScene");
  REQUIRE(test_property.get_asset_sub_id() == "testAssetSubId");
  REQUIRE(std::abs(test_property.get_value(0)) - 100.0 < TOLERANCE);

  // Action, Frame, & Graph Handle Tests

  // Construct test objects
  std::string action_name("testAction");
  AnimationAction<PropertyFrame> *action = new AnimationAction<PropertyFrame>;
  PropertyFrame *frame = new PropertyFrame;
  AnimationGraphHandle *handle = new AnimationGraphHandle;
  test_property.add_action(action_name, action);
  test_property.get_action(action_name)->add_keyframe(1, frame);
  test_property.get_action(action_name)->get_keyframe(1)->add_handle(handle);

  // Set values on test objects
  test_property.get_action(action_name)->get_keyframe(1)->set_frame(1);
  test_property.get_action(action_name)->get_keyframe(1)->add_value(1.0);
  test_property.get_action(action_name)->set_name(action_name);
  test_property.get_action(action_name)->get_keyframe(1)->get_handle(0)->set_lh_type(std::string("vector"));
  test_property.get_action(action_name)->get_keyframe(1)->get_handle(0)->set_lh_x(10.0);
  test_property.get_action(action_name)->get_keyframe(1)->get_handle(0)->set_lh_y(5.0);
  test_property.get_action(action_name)->get_keyframe(1)->get_handle(0)->set_rh_type(std::string("free"));
  test_property.get_action(action_name)->get_keyframe(1)->get_handle(0)->set_rh_x(4.0);
  test_property.get_action(action_name)->get_keyframe(1)->get_handle(0)->set_rh_y(3.0);
  REQUIRE(std::abs(test_property.get_action(action_name)->get_keyframe(1)->get_handle(0)->get_lh_x()) - 10.0 < TOLERANCE);
  REQUIRE(std::abs(test_property.get_action(action_name)->get_keyframe(1)->get_handle(0)->get_lh_y()) - 5.0 < TOLERANCE);
  REQUIRE(std::abs(test_property.get_action(action_name)->get_keyframe(1)->get_handle(0)->get_rh_x()) - 4.0 < TOLERANCE);
  REQUIRE(std::abs(test_property.get_action(action_name)->get_keyframe(1)->get_handle(0)->get_rh_y()) - 3.0 < TOLERANCE);
  REQUIRE(test_property.get_action(action_name));
  REQUIRE(test_property.get_action(action_name)->get_keyframe(1));
  REQUIRE(test_property.get_action(action_name)->get_keyframe(1)->get_handle(0)->get_lh_type() == "vector");

  // Test JSON Write
  std::string json_string;
  test_property.to_json(json_string);
  std::cout << json_string << std::endl;

  // Test JSON Read
  rapidjson::Document doc;
  doc.Parse<rapidjson::kParseStopWhenDoneFlag>(json_string.c_str());
  if (doc.HasParseError()) {
    std::cout << rapidjson::GetParseError_En(doc.GetParseError()) << std::endl;
  }
  REQUIRE(!(doc.HasParseError()));
  JsonFactory factory;
  PropertyInterface *parsed_property = factory.build_property(doc);
  REQUIRE(parsed_property->num_values() == 1);
  REQUIRE(parsed_property->get_key() == "12345");
  REQUIRE(parsed_property->get_name() == "testName");
  REQUIRE(parsed_property->get_parent() == "testParent");
  REQUIRE(parsed_property->get_scene() == "testScene");
  REQUIRE(parsed_property->get_asset_sub_id() == "testAssetSubId");
  REQUIRE(std::abs(parsed_property->get_value(0)) - 100.0 < TOLERANCE);
  REQUIRE(parsed_property->get_action(action_name)->get_keyframe(1));
  REQUIRE(parsed_property->get_action(action_name)->get_keyframe(1)->num_handles() > 0);
  REQUIRE(parsed_property->get_action(action_name)->get_keyframe(1)->get_handle(0)->get_lh_type() == "vector");
  REQUIRE(std::abs(parsed_property->get_action(action_name)->get_keyframe(1)->get_handle(0)->get_lh_x()) - 10.0 < TOLERANCE);
  REQUIRE(std::abs(parsed_property->get_action(action_name)->get_keyframe(1)->get_handle(0)->get_lh_y()) - 5.0 < TOLERANCE);
  REQUIRE(parsed_property->get_action(action_name)->get_keyframe(1)->get_handle(0)->get_rh_type() == "free");
  REQUIRE(std::abs(parsed_property->get_action(action_name)->get_keyframe(1)->get_handle(0)->get_rh_x()) - 4.0 < TOLERANCE);
  REQUIRE(std::abs(parsed_property->get_action(action_name)->get_keyframe(1)->get_handle(0)->get_rh_y()) - 3.0 < TOLERANCE);
  delete parsed_property;
}
