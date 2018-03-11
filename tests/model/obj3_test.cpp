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
#include <iostream>
#include <string>

#include "aossl/logging/include/logging_interface.h"
#include "aossl/logging/include/factory_logging.h"
#include "aossl/mongo/include/mongo_interface.h"
#include "aossl/mongo/include/factory_mongo.h"
#include "aossl/mongo/include/mongo_buffer_interface.h"

#include "transforms.h"
#include "object_document.h"
#include "app_log.h"

#include "object_interface.h"
#include "object_factory.h"

int main(int argc, char** argv) {
  const float PI = 3.1415f;
  //Tolerance
  const float TOLERANCE = 0.001f;
  LoggingComponentFactory *logging_factory = new LoggingComponentFactory;

  std::string initFileName = "tests/log4cpp.properties";
  logging = logging_factory->get_logging_interface(initFileName);

  start_logging_submodules();

  main_logging->debug("Basic Tests");

  // Constructor tests
  ObjectDocument test_object;
  assert(test_object.get_key() == "");
  assert(test_object.get_name() == "");
  assert(test_object.get_scene() == "");
  assert(test_object.get_type() == "");
  assert(test_object.get_subtype() == "");
  assert(test_object.get_owner() == "");
  assert(test_object.num_assets() == 0);

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

  assert(test_object.get_key() == "abcdefg");
  assert(test_object.get_name() == "abcdefgh");
  assert(test_object.get_scene() == "abcdefghi");
  assert(test_object.get_type() == "abcdefghij");
  assert(test_object.get_subtype() == "abcdefghijk");
  assert(test_object.get_owner() == "abcdefghijkl");

  // Asset tests
  main_logging->debug("Asset Tests");
  std::string asset1 = "12345";
  std::string asset2 = "12346";
  test_object.add_asset(asset1);
  test_object.add_asset(asset2);
  assert(test_object.num_assets() == 2);
  assert(test_object.get_asset(0) == "12345");
  assert(test_object.get_asset(1) == "12346");

  // Transform tests
  main_logging->debug("Transform Tests");
  Translation *trans = new Translation(1.0, 1.0, 1.0);
  EulerRotation *erot = new EulerRotation(3 * PI, 1.0f, 0.0f, 0.0f);
  Scale *scl = new Scale(2.0, 2.0, 2.0);

  main_logging->debug(test_object.get_transform()->to_string());
  test_object.transform(trans);
  main_logging->debug(test_object.get_transform()->to_string());
  assert(test_object.get_transform()->get_transform_element(0, 0) - 1.0f \
    < TOLERANCE);
  assert(test_object.get_transform()->get_transform_element(1, 1) - 1.0f \
    < TOLERANCE);
  assert(test_object.get_transform()->get_transform_element(2, 2) - 1.0f \
    < TOLERANCE);
  assert(test_object.get_transform()->get_transform_element(3, 3) - 1.0f \
    < TOLERANCE);
  assert(test_object.get_transform()->get_transform_element(0, 3) - 1.0f \
    < TOLERANCE);
  assert(test_object.get_transform()->get_transform_element(1, 3) - 1.0f \
    < TOLERANCE);
  assert(test_object.get_transform()->get_transform_element(2, 3) - 1.0f \
    < TOLERANCE);

  test_object.transform(erot);
  main_logging->debug(test_object.get_transform()->to_string());
  assert(test_object.get_transform()->get_transform_element(0, 0) - 1.0f \
    < TOLERANCE);
  assert(std::abs(test_object.get_transform()->get_transform_element(1, 1) \
    + 1.0f) < TOLERANCE);
  assert(std::abs(test_object.get_transform()->get_transform_element(2, 2) \
    + 1.0f) < TOLERANCE);
  assert(test_object.get_transform()->get_transform_element(3, 3) - 1.0f \
    < TOLERANCE);
  assert(test_object.get_transform()->get_transform_element(0, 3) - 1.0f \
    < TOLERANCE);
  assert(std::abs(test_object.get_transform()->get_transform_element(1, 3) \
    + 1.0f) < TOLERANCE);
  assert(std::abs(test_object.get_transform()->get_transform_element(2, 3) \
    + 1.0f) < TOLERANCE);

  test_object.transform(scl);
  main_logging->debug(test_object.get_transform()->to_string());
  assert(test_object.get_transform()->get_transform_element(0, 0) - 2.0f \
    < TOLERANCE);
  assert(test_object.get_transform()->get_transform_element(1, 1) + 2.0f \
    < TOLERANCE);
  assert(test_object.get_transform()->get_transform_element(2, 2) + 2.0f \
    < TOLERANCE);
  assert(test_object.get_transform()->get_transform_element(3, 3) - 1.0f \
    < TOLERANCE);
  assert(test_object.get_transform()->get_transform_element(0, 3) - 2.0f \
    < TOLERANCE);
  assert(test_object.get_transform()->get_transform_element(1, 3) + 2.0f \
    < TOLERANCE);
  assert(test_object.get_transform()->get_transform_element(2, 3) + 2.0f \
    < TOLERANCE);

  // BSON Tests
  MongoComponentFactory *mongo_factory = new MongoComponentFactory;
  AOSSL::MongoBufferInterface *bson = mongo_factory->get_mongo_buffer();
  test_object.to_bson_update(bson);

  // Can't really do any other tests until we expose some get methods
  // on the MongoBufferInterface

  delete bson;
  delete mongo_factory;

  // JSON Tests
  main_logging->debug("JSON Tests");

  std::string obj_json = test_object.to_json();
  main_logging->debug(obj_json);
  rapidjson::Document d;
  d.Parse<rapidjson::kParseStopWhenDoneFlag>(obj_json.c_str());

  ObjectFactory ofactory;
  ObjectInterface *translated_object = ofactory.build_object(d);

  //ObjectDocument *translated_object = new ObjectDocument(d);

  assert(translated_object->get_key() == "");
  assert(translated_object->get_name() == "abcdefgh");
  assert(translated_object->get_scene() == "abcdefghi");
  assert(translated_object->get_type() == "abcdefghij");
  assert(translated_object->get_subtype() == "abcdefghijk");
  assert(translated_object->get_owner() == "abcdefghijkl");

  assert(translated_object->num_assets() == 2);
  assert(translated_object->get_asset(0) == "12345");
  assert(translated_object->get_asset(1) == "12346");

  main_logging->debug(translated_object->get_transform()->to_string());

  assert(translated_object->get_transform()->get_transform_element(0, 0) - \
    2.0f < TOLERANCE);
  assert(translated_object->get_transform()->get_transform_element(1, 1) + \
    2.0f < TOLERANCE);
  assert(translated_object->get_transform()->get_transform_element(2, 2) + \
    2.0f < TOLERANCE);
  assert(translated_object->get_transform()->get_transform_element(3, 3) - \
    1.0f < TOLERANCE);
  assert(translated_object->get_transform()->get_transform_element(0, 3) - \
    2.0f < TOLERANCE);
  assert(translated_object->get_transform()->get_transform_element(1, 3) + \
    2.0f < TOLERANCE);
  assert(translated_object->get_transform()->get_transform_element(2, 3) + \
    2.0f < TOLERANCE);

  // Merge Test
  main_logging->debug("Merge Tests");
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

  assert(translated_object->get_key() == "new_key");
  assert(translated_object->get_name() == "new_name");
  assert(translated_object->get_scene() == "abcdefghi");
  assert(translated_object->get_type() == "abcdefghij");
  assert(translated_object->get_subtype() == "abcdefghijk");
  assert(translated_object->get_owner() == "new_owner");

  assert(translated_object->num_assets() == 3);
  assert(translated_object->get_asset(0) == "12345");
  assert(translated_object->get_asset(1) == "12346");
  assert(translated_object->get_asset(2) == "another_asset");

  assert(translated_object->get_transform()->get_transform_element(0, 0) - \
    2.0f < TOLERANCE);
  assert(translated_object->get_transform()->get_transform_element(1, 1) + \
    2.0f < TOLERANCE);
  assert(translated_object->get_transform()->get_transform_element(2, 2) + \
    2.0f < TOLERANCE);
  assert(translated_object->get_transform()->get_transform_element(3, 3) - \
    1.0f < TOLERANCE);
  assert(translated_object->get_transform()->get_transform_element(0, 3) - \
    3.0f < TOLERANCE);
  assert(translated_object->get_transform()->get_transform_element(1, 3) + \
    1.0f < TOLERANCE);
  assert(translated_object->get_transform()->get_transform_element(2, 3) + \
    1.0f < TOLERANCE);

  delete obj_update;

  delete translated_object;

  // Asset removal tests
  main_logging->debug("Asset Removal Tests");
  test_object.remove_asset(0);
  assert(test_object.num_assets() == 1);
  assert(test_object.get_asset(0) == "12346");
  test_object.clear_assets();
  assert(test_object.num_assets() == 0);

  delete trans;
  delete erot;
  delete scl;

  shutdown_logging_submodules();

  delete logging;
  delete logging_factory;
}
