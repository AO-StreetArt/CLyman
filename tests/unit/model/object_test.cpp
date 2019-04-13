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

#include "include/transforms.h"
#include "include/object_interface.h"
#include "include/json_object.h"
#include "include/json_factory.h"
#include "include/animation_action.h"
#include "include/animation_graph_handle.h"
#include "include/object_frame.h"

#include "catch.hpp"

TEST_CASE( "Test Object Data Structure", "[unit]" ) {
  //Tolerance
  const float TOLERANCE = 0.1f;
  const int PI = 3.1415f;
  // Constructor tests
  JsonObject test_object;
  REQUIRE(test_object.get_key() == "");
  REQUIRE(test_object.get_name() == "");
  REQUIRE(test_object.get_scene() == "");
  REQUIRE(test_object.get_type() == "");
  REQUIRE(test_object.get_subtype() == "");
  REQUIRE(test_object.get_owner() == "");
  REQUIRE(test_object.num_assets() == 0);

  std::string test_key = "abcdefg";
  test_object.set_key(test_key);
  std::string test_name = "abcdefgh";
  test_object.set_name(test_name);
  std::string test_scene = "abcdefghi";
  test_object.set_scene(test_scene);
  std::string test_type = "abcdefghij";
  test_object.set_type(test_type);
  std::string test_subtype = "abcdefghijk";
  test_object.set_subtype(test_subtype);
  std::string test_owner = "abcdefghijkl";
  test_object.set_owner(test_owner);

  REQUIRE(test_object.get_key() == "abcdefg");
  REQUIRE(test_object.get_name() == "abcdefgh");
  REQUIRE(test_object.get_scene() == "abcdefghi");
  REQUIRE(test_object.get_type() == "abcdefghij");
  REQUIRE(test_object.get_subtype() == "abcdefghijk");
  REQUIRE(test_object.get_owner() == "abcdefghijkl");

  // Asset tests
  std::string asset1 = "12345";
  std::string asset2 = "12346";
  test_object.add_asset(asset1);
  test_object.add_asset(asset2);
  REQUIRE(test_object.num_assets() == 2);
  REQUIRE(test_object.get_asset(0) == "12345");
  REQUIRE(test_object.get_asset(1) == "12346");

  // Transform tests
  Translation *trans = new Translation(1.0, 1.0, 1.0);
  EulerRotation *erot = new EulerRotation(3 * PI, 0.0f, 0.0f);
  Scale *scl = new Scale(2.0, 2.0, 2.0);

  std::cout << test_object.get_transform()->to_string() << std::endl;
  test_object.transform(trans);
  std::cout << test_object.get_transform()->to_string() << std::endl;
  REQUIRE(test_object.get_transform()->get_transform_element(0, 0) - 1.0f \
    < TOLERANCE);
  REQUIRE(test_object.get_transform()->get_transform_element(1, 1) - 1.0f \
    < TOLERANCE);
  REQUIRE(test_object.get_transform()->get_transform_element(2, 2) - 1.0f \
    < TOLERANCE);
  REQUIRE(test_object.get_transform()->get_transform_element(3, 3) - 1.0f \
    < TOLERANCE);
  REQUIRE(test_object.get_transform()->get_transform_element(0, 3) - 1.0f \
    < TOLERANCE);
  REQUIRE(test_object.get_transform()->get_transform_element(1, 3) - 1.0f \
    < TOLERANCE);
  REQUIRE(test_object.get_transform()->get_transform_element(2, 3) - 1.0f \
    < TOLERANCE);

  test_object.transform(erot);
  std::cout << test_object.get_transform()->to_string() << std::endl;
  REQUIRE(test_object.get_transform()->get_transform_element(0, 0) - 1.0f \
    < TOLERANCE);
  REQUIRE(std::abs(test_object.get_transform()->get_transform_element(1, 1) \
    + 0.91f) < TOLERANCE);
  REQUIRE(std::abs(test_object.get_transform()->get_transform_element(2, 2) \
    + 0.91f) < TOLERANCE);
  REQUIRE(test_object.get_transform()->get_transform_element(3, 3) - 1.0f \
    < TOLERANCE);
  REQUIRE(test_object.get_transform()->get_transform_element(0, 3) - 1.0f \
    < TOLERANCE);
  REQUIRE(std::abs(test_object.get_transform()->get_transform_element(1, 3) \
    + 1.32f) < TOLERANCE);
  REQUIRE(std::abs(test_object.get_transform()->get_transform_element(2, 3) \
    + 0.49f) < TOLERANCE);

  test_object.transform(scl);
  std::cout << test_object.get_transform()->to_string() << std::endl;
  REQUIRE(test_object.get_transform()->get_transform_element(0, 0) - 2.0f \
    < TOLERANCE);
  REQUIRE(test_object.get_transform()->get_transform_element(1, 1) + 1.8f \
    < TOLERANCE);
  REQUIRE(test_object.get_transform()->get_transform_element(2, 2) + 1.8f \
    < TOLERANCE);
  REQUIRE(test_object.get_transform()->get_transform_element(3, 3) - 1.0f \
    < TOLERANCE);
  REQUIRE(test_object.get_transform()->get_transform_element(0, 3) - 2.0f \
    < TOLERANCE);
  REQUIRE(test_object.get_transform()->get_transform_element(1, 3) + 2.64f \
    < TOLERANCE);
  REQUIRE(test_object.get_transform()->get_transform_element(2, 3) + 1.0f \
    < TOLERANCE);

  // Action Tests
  AnimationAction<ObjectFrame> *action = new AnimationAction<ObjectFrame>;
  ObjectFrame *frame = new ObjectFrame;
  frame->set_frame(1);
  REQUIRE(frame->get_translation(0)->get_lh_type().empty());
  std::string aftype("test");
  frame->get_translation(0)->set_lh_type(aftype);

  std::string action_name("testAction");
  std::string action_desc("This is a test action");
  action->set_name(action_name);
  action->set_description(action_desc);
  action->add_keyframe(1, frame);
  test_object.add_action(action->get_name(), action);
  REQUIRE(test_object.get_action(action_name)->get_name() == "testAction");
  REQUIRE(test_object.get_action(action_name)->get_description() == "This is a test action");
  REQUIRE(test_object.get_action(action_name)->get_keyframe(1)->get_translation(0)->get_lh_type() == "test");

  // Json Tests
  std::string obj_json = test_object.to_transform_json();
  std::cout << obj_json << std::endl;
  rapidjson::Document d;
  d.Parse<rapidjson::kParseStopWhenDoneFlag>(obj_json.c_str());

  JsonFactory ofactory;
  ObjectInterface *translated_object = ofactory.build_object(d);

  REQUIRE(translated_object->get_key() == "abcdefg");
  REQUIRE(translated_object->get_name() == "abcdefgh");
  REQUIRE(translated_object->get_scene() == "abcdefghi");
  REQUIRE(translated_object->get_action(action_name)->get_name() == "testAction");
  REQUIRE(translated_object->get_action(action_name)->get_description() == "This is a test action");
  REQUIRE(translated_object->get_action(action_name));
  REQUIRE(translated_object->get_action(action_name)->get_keyframe(1));
  REQUIRE(translated_object->get_action(action_name)->get_keyframe(1)->get_translation(0)->get_lh_type() == "test");

  std::cout << translated_object->get_transform()->to_string() << std::endl;
  REQUIRE(translated_object->get_transform()->get_transform_element(0, 0) - 2.0f \
    < TOLERANCE);
  REQUIRE(translated_object->get_transform()->get_transform_element(1, 1) + 1.8f \
    < TOLERANCE);
  REQUIRE(translated_object->get_transform()->get_transform_element(2, 2) + 1.8f \
    < TOLERANCE);
  REQUIRE(translated_object->get_transform()->get_transform_element(3, 3) - 1.0f \
    < TOLERANCE);
  REQUIRE(translated_object->get_transform()->get_transform_element(0, 3) - 2.0f \
    < TOLERANCE);
  REQUIRE(translated_object->get_transform()->get_transform_element(1, 3) + 2.64f \
    < TOLERANCE);
  REQUIRE(translated_object->get_transform()->get_transform_element(2, 3) + 1.0f \
    < TOLERANCE);

  delete translated_object;

  // Asset removal tests
  test_object.remove_asset(0);
  REQUIRE(test_object.num_assets() == 1);
  REQUIRE(test_object.get_asset(0) == "12346");
  test_object.clear_assets();
  REQUIRE(test_object.num_assets() == 0);

  delete trans;
  delete erot;
  delete scl;
}
