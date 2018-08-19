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
#include "include/object_factory.h"

#include "catch.hpp"

TEST_CASE( "Test Transform Data Structure", "[unit]" ) {
  //Tolerance
  const float TOLERANCE = 0.1f;
  const int PI = 3.1415f;
  // Constructor tests
  ObjectDocument test_object;
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
  test_object.set_frame(0);
  test_object.set_timestamp(123456789);

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

  std::string obj_json = test_object.to_json();
  std::cout << obj_json << std::endl;
  rapidjson::Document d;
  d.Parse<rapidjson::kParseStopWhenDoneFlag>(obj_json.c_str());

  ObjectFactory ofactory;
  ObjectInterface *translated_object = ofactory.build_object(d);

  REQUIRE(translated_object->get_key() == "");
  REQUIRE(translated_object->get_name() == "abcdefgh");
  REQUIRE(translated_object->get_scene() == "abcdefghi");
  REQUIRE(translated_object->get_type() == "abcdefghij");
  REQUIRE(translated_object->get_subtype() == "abcdefghijk");
  REQUIRE(translated_object->get_owner() == "abcdefghijkl");
  REQUIRE(translated_object->get_frame() == 0);
  //REQUIRE(translated_object->get_timestamp() == 123456789);

  REQUIRE(translated_object->num_assets() == 2);
  REQUIRE(translated_object->get_asset(0) == "12345");
  REQUIRE(translated_object->get_asset(1) == "12346");

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

  // Merge Test
  ObjectDocument *obj_update = new ObjectDocument;
  std::string upd_key = "new_key";
  obj_update->set_key(upd_key);
  std::string upd_name = "new_name";
  obj_update->set_name(upd_name);
  std::string upd_owner = "new_owner";
  obj_update->set_owner(upd_owner);

  Translation *trans2 = new Translation(1.0, 1.0, 1.0);
  obj_update->transform(trans2);

  delete trans2;

  std::string upd_asset = "another_asset";
  obj_update->add_asset(upd_asset);

  translated_object->merge(obj_update);

  REQUIRE(translated_object->get_key() == "new_key");
  REQUIRE(translated_object->get_name() == "new_name");
  REQUIRE(translated_object->get_scene() == "abcdefghi");
  REQUIRE(translated_object->get_type() == "abcdefghij");
  REQUIRE(translated_object->get_subtype() == "abcdefghijk");
  REQUIRE(translated_object->get_owner() == "new_owner");

  REQUIRE(translated_object->num_assets() == 3);
  REQUIRE(translated_object->get_asset(0) == "12345");
  REQUIRE(translated_object->get_asset(1) == "12346");
  REQUIRE(translated_object->get_asset(2) == "another_asset");

  std::cout << translated_object->get_transform()->to_string() << std::endl;
  REQUIRE(translated_object->get_transform()->get_transform_element(0, 0) - 2.0f \
    < TOLERANCE);
  REQUIRE(translated_object->get_transform()->get_transform_element(1, 1) + 1.8f \
    < TOLERANCE);
  REQUIRE(translated_object->get_transform()->get_transform_element(2, 2) + 1.8f \
    < TOLERANCE);
  REQUIRE(translated_object->get_transform()->get_transform_element(3, 3) - 1.0f \
    < TOLERANCE);
  REQUIRE(translated_object->get_transform()->get_transform_element(0, 3) - 3.0f \
    < TOLERANCE);
  REQUIRE(translated_object->get_transform()->get_transform_element(1, 3) + 1.64f \
    < TOLERANCE);
  REQUIRE(translated_object->get_transform()->get_transform_element(2, 3) + 0.0f \
    < TOLERANCE);

  delete obj_update;

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
