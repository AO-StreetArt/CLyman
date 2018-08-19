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

#include "include/object_list_interface.h"
#include "include/object_list_factory.h"
#include "include/object_interface.h"
#include "include/object_factory.h"
#include "include/transforms.h"

#include "rapidjson/document.h"

#include "catch.hpp"

ObjectDocument* build_test_document(std::string key, std::string name, \
  std::string scene, std::string type, std::string subtype, std::string owner) {
    ObjectDocument *test_object = new ObjectDocument;
    test_object->set_key(key);
    test_object->set_name(name);
    test_object->set_scene(scene);
    test_object->set_type(type);
    test_object->set_subtype(subtype);
    test_object->set_owner(owner);
    test_object->set_frame(0);
    test_object->set_timestamp(123456789);
    return test_object;
}

TEST_CASE( "Test Object List Data Structure", "[unit]" ) {
  ObjectListFactory ofactory;

  // Setup
  std::string key1 = "abcdef";
  std::string key2 = "abcdefg";
  std::string name1 = "abcdefgh";
  std::string name2 = "abcdefghi";
  std::string scene1 = "abcdefghij";
  std::string scene2 = "abcdefghijk";
  std::string type1 = "abcdefghijkl";
  std::string type2 = "abcdefghijklm";
  std::string subtype1 = "abcdefghijklmn";
  std::string subtype2 = "abcdefghijklmno";
  std::string owner1 = "abcdefghijklmnop";
  std::string owner2 = "abcdefghijklmnopq";

  ObjectDocument *test_object = build_test_document(key1, name1, scene1, \
    type1, subtype1, owner1);
  ObjectDocument *test_object2 = build_test_document(key2, name2, scene2, \
    type2, subtype2, owner2);
  ObjectDocument *test_object3 = build_test_document(key1, name1, scene1, \
    type1, subtype1, owner1);
  ObjectDocument *test_object4 = build_test_document(key2, name2, scene2, \
    type2, subtype2, owner2);

  std::string asset1 = "12345";
  std::string asset2 = "12346";
  std::string asset3 = "12347";
  test_object->add_asset(asset1);
  test_object->add_asset(asset2);
  test_object2->add_asset(asset3);
  test_object3->add_asset(asset1);
  test_object3->add_asset(asset2);
  test_object4->add_asset(asset3);

  Translation *trans = new Translation(1.0, 1.0, 1.0);
  EulerRotation *erot = new EulerRotation(3.1415f, 0.0f, 0.0f);
  Scale *scl = new Scale(2.0, 2.0, 2.0);
  test_object->transform(trans);
  test_object2->transform(trans);
  test_object2->transform(erot);
  test_object2->transform(scl);
  test_object3->transform(trans);
  test_object4->transform(trans);
  test_object4->transform(erot);
  test_object4->transform(scl);

  std::cout << test_object->get_transform()->to_string() << std::endl;
  std::cout << test_object2->get_transform()->to_string() << std::endl;
  std::cout << test_object3->get_transform()->to_string() << std::endl;
  std::cout << test_object4->get_transform()->to_string() << std::endl;

  // Constructor Test
  ObjectListInterface *olist = ofactory.build_json_object_list();
  REQUIRE(olist->get_msg_type() == -1);
  REQUIRE(olist->get_error_code() == 100);
  REQUIRE(olist->get_error_message() == "");
  REQUIRE(olist->get_transaction_id() == "");
  REQUIRE(olist->get_num_records() == 0);
  REQUIRE(olist->num_objects() == 0);

  // Getter/Setter tests
  olist->set_msg_type(1);
  REQUIRE(olist->get_msg_type() == 1);
  olist->set_error_code(110);
  REQUIRE(olist->get_error_code() == 110);
  std::string new_err_msg = "Test";
  olist->set_error_message(new_err_msg);
  REQUIRE(olist->get_error_message() == "Test");
  std::string tran_id = "123456789";
  olist->set_transaction_id(tran_id);
  REQUIRE(olist->get_transaction_id() == "123456789");
  olist->set_num_records(3);
  REQUIRE(olist->get_num_records() == 3);
  olist->add_object(test_object);
  REQUIRE(olist->num_objects() == 1);
  olist->add_object(test_object2);
  REQUIRE(olist->num_objects() == 2);

  // JSON Tests
  rapidjson::Document d;

  std::string json_string;
  olist->to_msg_string(json_string);
  const char * json_cstr = json_string.c_str();
  d.Parse(json_cstr);

  ObjectListInterface *jparsed_olist = ofactory.build_object_list(d);
  //ObjectList *jparsed_olist = new ObjectList(d);

  REQUIRE(jparsed_olist->get_msg_type() == 1);
  REQUIRE(jparsed_olist->get_transaction_id() == "123456789");
  REQUIRE(jparsed_olist->get_num_records() == 2);
  REQUIRE(jparsed_olist->num_objects() == 2);

  REQUIRE(jparsed_olist->get_object(0)->get_key() == "abcdef");
  REQUIRE(jparsed_olist->get_object(0)->get_name() == "abcdefgh");
  REQUIRE(jparsed_olist->get_object(0)->get_scene() == "abcdefghij");
  REQUIRE(jparsed_olist->get_object(0)->get_type() == "abcdefghijkl");
  REQUIRE(jparsed_olist->get_object(0)->get_subtype() == "abcdefghijklmn");
  REQUIRE(jparsed_olist->get_object(0)->get_owner() == "abcdefghijklmnop");

  REQUIRE(jparsed_olist->get_object(1)->get_key() == "abcdefg");
  REQUIRE(jparsed_olist->get_object(1)->get_name() == "abcdefghi");
  REQUIRE(jparsed_olist->get_object(1)->get_scene() == "abcdefghijk");
  REQUIRE(jparsed_olist->get_object(1)->get_type() == "abcdefghijklm");
  REQUIRE(jparsed_olist->get_object(1)->get_subtype() == "abcdefghijklmno");
  REQUIRE(jparsed_olist->get_object(1)->get_owner() == "abcdefghijklmnopq");

  // Teardown
  delete olist;
  delete jparsed_olist;
  delete trans;
  delete erot;
  delete scl;
}
