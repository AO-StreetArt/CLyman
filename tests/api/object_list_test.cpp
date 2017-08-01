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

#include <assert.h>
#include <math.h>
#include <string>

#include "transforms.h"
#include "object_list.h"

#include "object_list_interface.h"
#include "object_list_factory.h"

#include "aossl/logging/include/logging_interface.h"
#include "aossl/logging/include/factory_logging.h"

ObjectDocument* build_test_document(std::string key, std::string name, \
  std::string scene, std::string type, std::string subtype, std::string owner) {
    ObjectDocument *test_object = new ObjectDocument;
    test_object->set_key(key);
    test_object->set_name(name);
    test_object->set_scene(scene);
    test_object->set_type(type);
    test_object->set_subtype(subtype);
    test_object->set_owner(owner);
    return test_object;
}

int main(int argc, char** argv) {
  const float PI = 3.1415f;

  LoggingComponentFactory *logging_factory = new LoggingComponentFactory;
  ObjectListFactory ofactory;

  std::string initFileName = "tests/log4cpp.properties";
  logging = logging_factory->get_logging_interface(initFileName);

  start_logging_submodules();

  main_logging->debug("Setup");

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
  EulerRotation *erot = new EulerRotation(PI, 1.0f, 0.0f, 0.0f);
  Scale *scl = new Scale(2.0, 2.0, 2.0);
  test_object->transform(trans);
  test_object2->transform(trans);
  test_object2->transform(erot);
  test_object2->transform(scl);
  test_object3->transform(trans);
  test_object4->transform(trans);
  test_object4->transform(erot);
  test_object4->transform(scl);

  main_logging->debug(test_object->get_transform()->to_string());
  main_logging->debug(test_object2->get_transform()->to_string());
  main_logging->debug(test_object3->get_transform()->to_string());
  main_logging->debug(test_object4->get_transform()->to_string());

  // Constructor Test
  main_logging->debug("Basic Tests");
  ObjectListInterface *olist = ofactory.build_json_object_list();
  ObjectListInterface *pblist = ofactory.build_proto_object_list();
  assert(olist->get_msg_type() == -1);
  assert(olist->get_error_code() == 100);
  assert(olist->get_error_message() == "");
  assert(olist->get_transaction_id() == "");
  assert(olist->get_num_records() == 0);
  assert(olist->num_objects() == 0);

  // Getter/Setter tests
  olist->set_msg_type(1);
  assert(olist->get_msg_type() == 1);
  olist->set_error_code(110);
  assert(olist->get_error_code() == 110);
  std::string new_err_msg = "Test";
  olist->set_error_message(new_err_msg);
  assert(olist->get_error_message() == "Test");
  std::string tran_id = "123456789";
  olist->set_transaction_id(tran_id);
  assert(olist->get_transaction_id() == "123456789");
  olist->set_num_records(3);
  assert(olist->get_num_records() == 3);
  olist->add_object(test_object);
  assert(olist->num_objects() == 1);
  olist->add_object(test_object2);
  assert(olist->num_objects() == 2);

  pblist->set_msg_type(1);
  pblist->set_error_code(110);
  std::string new_err_msg2 = "Test";
  pblist->set_error_message(new_err_msg2);
  std::string tran_id2 = "123456789";
  pblist->set_transaction_id(tran_id2);
  pblist->set_num_records(3);
  pblist->add_object(test_object3);
  pblist->add_object(test_object4);


  // Protocol Buffer Tests
  std::string proto_string;
  pblist->to_msg_string(proto_string);
  main_logging->debug(proto_string);
  protoObj3::Obj3List new_proto;
  new_proto.ParseFromString(proto_string);

  ObjectListInterface *parsed_olist = ofactory.build_object_list(new_proto);
  //ObjectList *parsed_olist = new ObjectList(new_proto);

  assert(parsed_olist->get_msg_type() == 1);
  assert(parsed_olist->get_transaction_id() == "123456789");
  assert(parsed_olist->get_num_records() == 2);
  assert(parsed_olist->num_objects() == 2);

  assert(parsed_olist->get_object(0)->get_key() == "abcdef");
  assert(parsed_olist->get_object(0)->get_name() == "abcdefgh");
  assert(parsed_olist->get_object(0)->get_scene() == "abcdefghij");
  assert(parsed_olist->get_object(0)->get_type() == "abcdefghijkl");
  assert(parsed_olist->get_object(0)->get_subtype() == "abcdefghijklmn");
  assert(parsed_olist->get_object(0)->get_owner() == "abcdefghijklmnop");

  assert(parsed_olist->get_object(1)->get_key() == "abcdefg");
  assert(parsed_olist->get_object(1)->get_name() == "abcdefghi");
  assert(parsed_olist->get_object(1)->get_scene() == "abcdefghijk");
  assert(parsed_olist->get_object(1)->get_type() == "abcdefghijklm");
  assert(parsed_olist->get_object(1)->get_subtype() == "abcdefghijklmno");
  assert(parsed_olist->get_object(1)->get_owner() == "abcdefghijklmnopq");

  main_logging->debug(parsed_olist->get_object(0)-> \
    get_transform()->to_string());
  main_logging->debug(parsed_olist->get_object(1)-> \
    get_transform()->to_string());

  // JSON Tests
  main_logging->debug("JSON Tests");
  rapidjson::Document d;

  std::string json_string;
  olist->to_msg_string(json_string);
  const char * json_cstr = json_string.c_str();
  d.Parse(json_cstr);

  ObjectListInterface *jparsed_olist = ofactory.build_object_list(d);
  //ObjectList *jparsed_olist = new ObjectList(d);

  assert(jparsed_olist->get_msg_type() == 1);
  assert(jparsed_olist->get_transaction_id() == "123456789");
  assert(jparsed_olist->get_num_records() == 2);
  assert(jparsed_olist->num_objects() == 2);

  assert(jparsed_olist->get_object(0)->get_key() == "abcdef");
  assert(jparsed_olist->get_object(0)->get_name() == "abcdefgh");
  assert(jparsed_olist->get_object(0)->get_scene() == "abcdefghij");
  assert(jparsed_olist->get_object(0)->get_type() == "abcdefghijkl");
  assert(jparsed_olist->get_object(0)->get_subtype() == "abcdefghijklmn");
  assert(jparsed_olist->get_object(0)->get_owner() == "abcdefghijklmnop");

  assert(jparsed_olist->get_object(1)->get_key() == "abcdefg");
  assert(jparsed_olist->get_object(1)->get_name() == "abcdefghi");
  assert(jparsed_olist->get_object(1)->get_scene() == "abcdefghijk");
  assert(jparsed_olist->get_object(1)->get_type() == "abcdefghijklm");
  assert(jparsed_olist->get_object(1)->get_subtype() == "abcdefghijklmno");
  assert(jparsed_olist->get_object(1)->get_owner() == "abcdefghijklmnopq");

  // Teardown
  delete olist;
  delete parsed_olist;
  delete jparsed_olist;
  delete trans;
  delete erot;
  delete scl;

  shutdown_logging_submodules();

  delete logging;
  delete logging_factory;
}
