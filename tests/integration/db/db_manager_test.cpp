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
#include "include/property_list_interface.h"
#include "include/data_list_factory.h"
#include "include/object_interface.h"
#include "include/property_interface.h"
#include "include/object_frame.h"
#include "include/property_frame.h"
#include "include/data_factory.h"
#include "include/json_factory.h"
#include "include/animation_action.h"
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

ObjectInterface* build_test_obj3(std::string key, std::string name, \
  std::string scene, std::string type, std::string subtype, std::string owner) {
    DataFactory object_factory;
    ObjectInterface *test_object = object_factory.build_object();
    test_object->set_key(key);
    test_object->set_name(name);
    test_object->set_scene(scene);
    test_object->set_type(type);
    test_object->set_subtype(subtype);
    test_object->set_owner(owner);
    return test_object;
}

PropertyInterface* build_test_property(std::string key, std::string name, \
    std::string scene, std::string parent, std::string asset_sub_id) {
  DataFactory object_factory;
  PropertyInterface *prop = object_factory.build_property();
  prop->set_key(key);
  prop->set_name(name);
  prop->set_parent(parent);
  prop->set_scene(scene);
  prop->set_asset_sub_id(asset_sub_id);
  return prop;
}

void build_obj_frame(ObjectFrame *frame, int frame_indx) {
  frame->set_frame(frame_indx);
  frame->get_translation(0)->set_lh_type(std::string("vector"));
  frame->get_translation(0)->set_lh_x(10.0);
  frame->get_translation(0)->set_lh_y(5.0);
  frame->get_translation(0)->set_rh_type(std::string("free"));
  frame->get_translation(0)->set_rh_x(4.0);
  frame->get_translation(0)->set_rh_y(3.0);
}

void build_obj_action(AnimationAction<ObjectFrame> *action, std::string& action_name, int frame_indx) {
  ObjectFrame *frame = new ObjectFrame;
  action->add_keyframe(frame_indx, frame);
  build_obj_frame(action->get_keyframe(frame_indx), frame_indx);
  action->set_name(action_name);
}

void build_prop_frame(PropertyFrame *frame, int frame_indx, double frame_value) {
  frame->set_frame(frame_indx);
  frame->add_value(frame_value);
  AnimationGraphHandle *handle = new AnimationGraphHandle;
  frame->add_handle(handle);
  frame->get_handle(0)->set_lh_type(std::string("vector"));
  frame->get_handle(0)->set_lh_x(10.0);
  frame->get_handle(0)->set_lh_y(5.0);
  frame->get_handle(0)->set_rh_type(std::string("free"));
  frame->get_handle(0)->set_rh_x(4.0);
  frame->get_handle(0)->set_rh_y(3.0);
}

void build_prop_action(AnimationAction<PropertyFrame> *action, std::string& action_name, int frame_indx, double frame_value) {
  PropertyFrame *frame = new PropertyFrame;
  action->add_keyframe(frame_indx, frame);
  build_prop_frame(action->get_keyframe(frame_indx), frame_indx, frame_value);
  action->set_name(action_name);
}

ObjectListInterface* get_obj_for_validation(DatabaseManager& db, std::string& new_key) {
  DataListFactory object_list_factory;
  ObjectListInterface *new_prop = object_list_factory.build_json_object_list();
  db.get_object(new_prop, new_key);
  std::cout << new_prop->get_error_message() << std::endl;
  // Test that we have a success error code
  REQUIRE(new_prop->get_error_code() == NO_ERROR);
  // Test that we found a return object from the DB
  REQUIRE(new_prop->num_objects() > 0);
  return new_prop;
}

PropertyListInterface* get_prop_for_validation(DatabaseManager& db, std::string& new_key) {
  DataListFactory object_list_factory;
  PropertyListInterface *new_prop = object_list_factory.build_json_property_list();
  db.get_property(new_prop, new_key);
  std::cout << new_prop->get_error_message() << std::endl;
  std::cout << new_prop->get_num_records() << std::endl;
  // Test that we have a success error code
  REQUIRE(new_prop->get_error_code() == NO_ERROR);
  // Test that we found a return object from the DB
  REQUIRE(new_prop->num_props() > 0);
  return new_prop;
}

TEST_CASE( "Test Database Manager", "[integration]" ) {
  //Tolerance
  const float TOLERANCE = 0.1f;
  std::cout << "Database Manager Test" << std::endl;
  // Build the test profile
  std::vector<std::string> cli_args;
  std::string app_name = "tests";
  cli_args.push_back(std::string(app_name));
  AOSSL::NetworkApplicationProfile prof(cli_args);

  // Build the DB Manager
  DatabaseManager db(&prof, std::string("mongodb://localhost:27017"), \
      std::string("test"), std::string("test"), std::string("testprop"));

  DataListFactory object_list_factory;
  JsonFactory json_factory;
  DataFactory object_factory;

  // Setup test objects
  std::string key1 = "vwxyzabcd";
  std::string key2 = "vwxyzabcde";
  std::string name1 = "vwxyzabcdef";
  std::string name2 = "vwxyzabcdefg";
  std::string scene1 = "vwxyzabcdefgh";
  std::string scene2 = "vwxyzabcdefghi";
  std::string parent1 = "vwxy12bcdefghij";
  std::string parent2 = "vwxy12bcdefghijk";
  std::string asset_sub_id1 = "vwx89abcdefghij";
  std::string asset_sub_id2 = "vwx89abcdefghijk";
  std::string type1 = "vwxyzabcdefghij";
  std::string type2 = "vwxyzabcdefghijk";
  std::string subtype1 = "vwxyzabcdefghijkl";
  std::string subtype2 = "vwxyzabcdefghijklm";
  std::string owner1 = "vwxyz12345";
  std::string owner2 = "vwxyz1234567";

    // Property Tests
  if (true) {
    std::cout << "Property Tests" << std::endl;
    PropertyInterface *prop1 = build_test_property(key1, name1, scene1, parent1, asset_sub_id1);
    PropertyInterface *prop2 = build_test_property(key2, name2, scene2, parent2, asset_sub_id2);
    prop1->add_value(1.0);
    prop2->add_value(2.0);
    prop2->add_value(3.0);

    // Add test property sub-objects
    std::string action_name("testAction");
    AnimationAction<PropertyFrame> *action = new AnimationAction<PropertyFrame>;
    prop1->add_action(action_name, action);
    build_prop_action(prop1->get_action(action_name), action_name, 1, 10.0);
    std::string new_key;

    // Create
    std::cout << "Creation Test" << std::endl;
    DatabaseResponse db_response;
    db.create_property(db_response, prop1, new_key);
    std::cout << db_response.error_message << std::endl;
    // Test that we have a successful result
    REQUIRE(db_response.success);
    // Test that the returned key is not empty
    REQUIRE(!(new_key.empty()));
    std::cout << "New OID for Property: " << new_key << std::endl;

    // Get
    std::cout << "Retrieval Test" << std::endl;
    PropertyListInterface *get_response = get_prop_for_validation(db, new_key);
    // Validate that the object saved is what is returned
    REQUIRE(get_response->get_prop(0)->get_name() == name1);
    REQUIRE(get_response->get_prop(0)->get_scene() == scene1);
    REQUIRE(get_response->get_prop(0)->get_parent() == parent1);
    REQUIRE(get_response->get_prop(0)->get_asset_sub_id() == asset_sub_id1);
    REQUIRE(get_response->get_prop(0)->num_values() == 1);
    REQUIRE(get_response->get_prop(0)->get_action(action_name));
    REQUIRE(get_response->get_prop(0)->get_action(action_name)->get_keyframe(1));
    REQUIRE(get_response->get_prop(0)->get_action(action_name)->get_keyframe(1)->num_values() > 0);
    REQUIRE(get_response->get_prop(0)->get_action(action_name)->get_keyframe(1)->get_handle(0)->get_lh_x() - 10.0 < TOLERANCE);
    delete get_response;

    // Update
    std::cout << "Update Test" << std::endl;
    DatabaseResponse upd_response;
    db.update_property(upd_response, prop2, new_key);
    std::cout << upd_response.error_message << std::endl;
    // Test that we have a successful result
    REQUIRE(upd_response.success);

    // Query Test
    std::cout << "Query Test" << std::endl;
    PropertyListInterface *query_response = object_list_factory.build_json_property_list();
    PropertyInterface *query_object = object_factory.build_property();
    query_object->set_name(name2);
    db.property_query(query_response, query_object, 2);
    // Test that we found a return object from the DB
    REQUIRE(query_response->num_props() > 0);
    REQUIRE(query_response->get_prop(0)->get_name() == name2);
    REQUIRE(query_response->get_prop(0)->get_scene() == scene2);
    REQUIRE(query_response->get_prop(0)->get_parent() == parent2);
    REQUIRE(query_response->get_prop(0)->get_asset_sub_id() == asset_sub_id2);
    REQUIRE(query_response->get_prop(0)->num_values() == 2);
    // Test that we have a success error code
    REQUIRE(query_response->get_error_code() == NO_ERROR);
    delete query_response;
    delete query_object;

    // Add Action test
    std::cout << "Action Creation Test" << std::endl;
    std::string action_name2("testAction2");
    AnimationAction<PropertyFrame> *action2 = new AnimationAction<PropertyFrame>;
    build_prop_action(action2, action_name2, 5, 20.0);
    DatabaseResponse action_insert_response;
    db.create_action(action_insert_response, new_key, action2, action_name2);
    // Test that we have a successful result
    std::cout << action_insert_response.error_message << std::endl;
    REQUIRE(action_insert_response.success);
    delete action2;

    // Validate the action insert by retrieving and validating the record
    PropertyListInterface *get_response2 = get_prop_for_validation(db, new_key);
    REQUIRE(get_response2->get_prop(0)->get_action(action_name2));
    REQUIRE(get_response2->get_prop(0)->get_action(action_name2)->get_keyframe(5));
    REQUIRE(get_response2->get_prop(0)->get_action(action_name2)->get_keyframe(5)->get_value(0) - 20.0 < TOLERANCE);
    delete get_response2;

    // Update Action Test
    std::cout << "Action Update Test" << std::endl;
    std::string action_desc3("Test Action Description!");
    AnimationAction<PropertyFrame> *action3 = new AnimationAction<PropertyFrame>;
    action3->set_name(action_name2);
    action3->set_description(action_desc3);
    DatabaseResponse action_upd_response;
    db.update_action(action_upd_response, new_key, action3, action_name2);
    // Test that we have a successful result
    std::cout << action_upd_response.error_message << std::endl;
    REQUIRE(action_upd_response.success);
    delete action3;

    // Validate the action update by retrieving and validating the record
    PropertyListInterface *get_response3 = get_prop_for_validation(db, new_key);
    REQUIRE(get_response3->get_prop(0)->get_action(action_name2));
    REQUIRE(get_response3->get_prop(0)->get_action(action_name2)->get_description() == "Test Action Description!");
    delete get_response3;

    // Add Keyframe Test
    std::cout << "Keyframe Creation Test" << std::endl;
    PropertyFrame *new_frame = new PropertyFrame;
    build_prop_frame(new_frame, 7, 23.4);
    DatabaseResponse frame_add_response;
    db.create_keyframe(frame_add_response, new_key, action_name2, new_frame, 7);
    // Test that we have a successful result
    std::cout << frame_add_response.error_message << std::endl;
    REQUIRE(frame_add_response.success);
    delete new_frame;

    // Validate the keyframe insert by retrieving and validating the record
    PropertyListInterface *get_response4 = get_prop_for_validation(db, new_key);
    REQUIRE(get_response4->get_prop(0)->get_action(action_name2));
    REQUIRE(get_response4->get_prop(0)->get_action(action_name2)->get_keyframe(7));
    REQUIRE(get_response4->get_prop(0)->get_action(action_name2)->get_keyframe(7)->get_value(0) - 23.4 < TOLERANCE);
    delete get_response4;

    // Update Keyframe Test
    std::cout << "Keyframe Update Test" << std::endl;
    PropertyFrame *upd_frame = new PropertyFrame;
    build_prop_frame(upd_frame, 7, 25.6);
    DatabaseResponse frame_upd_response;
    db.update_keyframe(frame_upd_response, new_key, action_name2, upd_frame, 7);
    // Test that we have a successful result
    std::cout << frame_upd_response.error_message << std::endl;
    REQUIRE(frame_upd_response.success);
    delete upd_frame;

    // Validate the keyframe update by retrieving and validating the record
    PropertyListInterface *get_response5 = get_prop_for_validation(db, new_key);
    REQUIRE(get_response5->get_prop(0)->get_action(action_name2));
    REQUIRE(get_response5->get_prop(0)->get_action(action_name2)->get_keyframe(7));
    REQUIRE(get_response5->get_prop(0)->get_action(action_name2)->get_keyframe(7)->get_value(0) - 25.6 < TOLERANCE);
    delete get_response5;

    // Remove Keyframe Test
    std::cout << "Keyframe Deletion Test" << std::endl;
    DatabaseResponse frame_rm_response;
    db.delete_property_keyframe(frame_rm_response, new_key, action_name2, 7);
    // Test that we have a successful result
    std::cout << frame_rm_response.error_message << std::endl;
    REQUIRE(frame_rm_response.success);

    // Validate the keyframe removal by retrieving and validating the record
    PropertyListInterface *get_response6 = get_prop_for_validation(db, new_key);
    REQUIRE(get_response6->get_prop(0)->get_action(action_name2));
    REQUIRE(!(get_response6->get_prop(0)->get_action(action_name2)->get_keyframe(7)));
    delete get_response6;

    // Delete Action Test
    std::cout << "Action Deletion Test" << std::endl;
    DatabaseResponse action_rm_response;
    db.delete_property_action(action_rm_response, new_key, action_name2);
    // Test that we have a successful result
    std::cout << action_rm_response.error_message << std::endl;
    REQUIRE(action_rm_response.success);

    // Validate the action delete by retrieving and validating the record
    PropertyListInterface *get_response7 = get_prop_for_validation(db, new_key);
    // Validate that the object saved is what is returned
    REQUIRE(!(get_response7->get_prop(0)->get_action(action_name2)));
    delete get_response7;

    // Delete Test
    std::cout << "Deletion Test" << std::endl;
    DatabaseResponse del_response;
    db.delete_property(del_response, new_key);
    std::cout << del_response.error_message << std::endl;
    // Test that we have a successful result
    REQUIRE(del_response.success);

    // Validate the object is no longer present
    PropertyListInterface *get_response8 = object_list_factory.build_json_property_list();
    db.get_property(get_response8, new_key);
    std::cout << get_response8->get_error_message() << std::endl;
    // Test that we found a return object from the DB
    REQUIRE(get_response8->num_props() == 0);
    // Test that we have a success error code
    REQUIRE(get_response8->get_error_code() > NO_ERROR);
    delete get_response8;

    delete prop2;
    delete prop1;
  }

  // Object Tests
  if (true) {
    std::cout << "Object Test" << std::endl;
    ObjectInterface *test_object = build_test_obj3(std::string(""), name1, scene1, \
      type1, subtype1, std::string(""));
    ObjectInterface *test_object2 = build_test_obj3(std::string(""), name2, scene2, \
      type2, subtype2, std::string(""));

    std::string asset1 = "12345";
    std::string asset2 = "12346";
    std::string asset3 = "12347";
    test_object->add_asset(asset1);
    test_object->add_asset(asset2);
    test_object2->add_asset(asset3);
    std::string new_key;

    // Add test property sub-objects
    std::string action_name("testAction");
    AnimationAction<ObjectFrame> *action = new AnimationAction<ObjectFrame>;
    ObjectFrame *frame = new ObjectFrame;
    test_object->add_action(action_name, action);
    test_object->get_action(action_name)->set_name(action_name);
    test_object->get_action(action_name)->add_keyframe(1, frame);
    test_object->get_action(action_name)->get_keyframe(1)->set_frame(1);
    test_object->get_action(action_name)->get_keyframe(1)->get_translation(0)->set_lh_type(std::string("vector"));
    test_object->get_action(action_name)->get_keyframe(1)->get_translation(0)->set_lh_x(10.0);
    test_object->get_action(action_name)->get_keyframe(1)->get_translation(0)->set_lh_y(5.0);
    test_object->get_action(action_name)->get_keyframe(1)->get_translation(0)->set_rh_type(std::string("free"));
    test_object->get_action(action_name)->get_keyframe(1)->get_translation(0)->set_rh_x(4.0);
    test_object->get_action(action_name)->get_keyframe(1)->get_translation(0)->set_rh_y(3.0);

    // Create
    std::cout << "Creation Test" << std::endl;
    DatabaseResponse db_response;
    db.create_object(db_response, test_object, new_key);
    std::cout << db_response.error_message << std::endl;
    // Test that we have a successful result
    REQUIRE(db_response.success);
    // Test that the returned key is not empty
    REQUIRE(!(new_key.empty()));
    std::cout << "New OID for Object: " << new_key << std::endl;

    // Get
    std::cout << "Retrieval Test" << std::endl;
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
    REQUIRE(get_response->get_object(0)->get_action(action_name));
    REQUIRE(get_response->get_object(0)->get_action(action_name)->get_keyframe(1));
    REQUIRE(get_response->get_object(0)->get_action(action_name)->get_keyframe(1)->get_translation(0)->get_lh_x() - 10.0 < TOLERANCE);
    REQUIRE(get_response->get_object(0)->num_assets() == 2);
    // Test that we have a success error code
    REQUIRE(get_response->get_error_code() == NO_ERROR);
    delete get_response;

    // Update
    std::cout << "Update Test" << std::endl;
    DatabaseResponse upd_response;
    db.update_object(upd_response, test_object2, new_key);
    std::cout << upd_response.error_message << std::endl;
    // Test that we have a successful result
    REQUIRE(upd_response.success);

    // Query
    std::cout << "Query Test" << std::endl;
    ObjectListInterface *query_response = object_list_factory.build_json_object_list();
    ObjectInterface *query_object = object_factory.build_object();
    query_object->set_name(name2);
    db.query(query_response, query_object, 2);
    std::cout << query_response->get_error_message() << std::endl;
    // Test that we found a return object from the DB
    REQUIRE(query_response->num_objects() > 0);
    // Validate that the object saved is what is returned
    REQUIRE(query_response->get_object(0)->get_name() == name2);
    REQUIRE(query_response->get_object(0)->get_scene() == scene2);
    REQUIRE(query_response->get_object(0)->get_type() == type2);
    REQUIRE(query_response->get_object(0)->get_subtype() == subtype2);
    REQUIRE(query_response->get_object(0)->num_assets() == 3);
    // Test that we have a success error code
    REQUIRE(query_response->get_error_code() == NO_ERROR);
    delete query_object;
    delete query_response;

    // Add Action test
    std::cout << "Action Creation Test" << std::endl;
    std::string action_name2("testAction2");
    AnimationAction<ObjectFrame> *action2 = new AnimationAction<ObjectFrame>;
    build_obj_action(action2, action_name2, 5);
    DatabaseResponse action_insert_response;
    db.create_action(action_insert_response, new_key, action2, action_name2);
    // Test that we have a successful result
    std::cout << action_insert_response.error_message << std::endl;
    REQUIRE(action_insert_response.success);
    delete action2;

    // Validate the action insert by retrieving and validating the record
    ObjectListInterface *get_response2 = get_obj_for_validation(db, new_key);
    REQUIRE(get_response2->get_object(0)->get_action(action_name2));
    REQUIRE(get_response2->get_object(0)->get_action(action_name2)->get_keyframe(5));
    delete get_response2;

    // Update Action Test
    std::cout << "Action Update Test" << std::endl;
    std::string action_desc3("Test Action Description!");
    AnimationAction<ObjectFrame> *action3 = new AnimationAction<ObjectFrame>;
    action3->set_name(action_name2);
    action3->set_description(action_desc3);
    DatabaseResponse action_upd_response;
    db.update_action(action_upd_response, new_key, action3, action_name2);
    // Test that we have a successful result
    std::cout << action_upd_response.error_message << std::endl;
    REQUIRE(action_upd_response.success);
    delete action3;

    // Validate the action update by retrieving and validating the record
    ObjectListInterface *get_response3 = get_obj_for_validation(db, new_key);
    REQUIRE(get_response3->get_object(0)->get_action(action_name2));
    REQUIRE(get_response3->get_object(0)->get_action(action_name2)->get_description() == "Test Action Description!");
    delete get_response3;

    // Add Keyframe Test
    std::cout << "Keyframe Creation Test" << std::endl;
    ObjectFrame *new_frame = new ObjectFrame;
    build_obj_frame(new_frame, 7);
    DatabaseResponse frame_add_response;
    db.create_keyframe(frame_add_response, new_key, action_name2, new_frame, 7);
    // Test that we have a successful result
    std::cout << frame_add_response.error_message << std::endl;
    REQUIRE(frame_add_response.success);
    delete new_frame;

    // Validate the keyframe insert by retrieving and validating the record
    ObjectListInterface *get_response4 = get_obj_for_validation(db, new_key);
    REQUIRE(get_response4->get_object(0)->get_action(action_name2));
    REQUIRE(get_response4->get_object(0)->get_action(action_name2)->get_keyframe(7));
    REQUIRE(get_response4->get_object(0)->get_action(action_name2)->get_keyframe(7)->get_translation(0)->get_lh_x() - 10.0 < TOLERANCE);
    delete get_response4;

    // Update Keyframe Test
    std::cout << "Keyframe Update Test" << std::endl;
    ObjectFrame *upd_frame = new ObjectFrame;
    build_obj_frame(upd_frame, 7);
    upd_frame->get_translation(0)->set_lh_x(14.4);
    DatabaseResponse frame_upd_response;
    db.update_keyframe(frame_upd_response, new_key, action_name2, upd_frame, 7);
    // Test that we have a successful result
    std::cout << frame_upd_response.error_message << std::endl;
    REQUIRE(frame_upd_response.success);
    delete upd_frame;

    // Validate the keyframe update by retrieving and validating the record
    ObjectListInterface *get_response5 = get_obj_for_validation(db, new_key);
    REQUIRE(get_response5->get_object(0)->get_action(action_name2));
    REQUIRE(get_response5->get_object(0)->get_action(action_name2)->get_keyframe(7));
    REQUIRE(get_response5->get_object(0)->get_action(action_name2)->get_keyframe(7)->get_translation(0)->get_lh_x() - 14.4 < TOLERANCE);
    delete get_response5;

    // Remove Keyframe Test
    std::cout << "Keyframe Deletion Test" << std::endl;
    DatabaseResponse frame_rm_response;
    db.delete_object_keyframe(frame_rm_response, new_key, action_name2, 7);
    // Test that we have a successful result
    std::cout << frame_rm_response.error_message << std::endl;
    REQUIRE(frame_rm_response.success);

    // Validate the keyframe removal by retrieving and validating the record
    ObjectListInterface *get_response6 = get_obj_for_validation(db, new_key);
    REQUIRE(get_response6->get_object(0)->get_action(action_name2));
    REQUIRE(!(get_response6->get_object(0)->get_action(action_name2)->get_keyframe(7)));
    delete get_response6;

    // Delete Action Test
    std::cout << "Action Deletion Test" << std::endl;
    DatabaseResponse action_rm_response;
    db.delete_object_action(action_rm_response, new_key, action_name2);
    // Test that we have a successful result
    std::cout << action_rm_response.error_message << std::endl;
    REQUIRE(action_rm_response.success);

    // Validate the action delete by retrieving and validating the record
    ObjectListInterface *get_response7 = get_obj_for_validation(db, new_key);
    // Validate that the object saved is what is returned
    REQUIRE(!(get_response7->get_object(0)->get_action(action_name2)));
    delete get_response7;

    // Lock
    std::cout << "Lock Test" << std::endl;
    DatabaseResponse lock_response;
    db.lock_object(lock_response, new_key, owner1);
    std::cout << lock_response.error_message << std::endl;
    REQUIRE(lock_response.success);

    // Try to obtain the lock with another device
    DatabaseResponse bad_lock_response;
    db.lock_object(bad_lock_response, new_key, owner2);
    std::cout << bad_lock_response.error_message << std::endl;
    REQUIRE(!(bad_lock_response.success));

    // Unlock
    std::cout << "Unlock Test" << std::endl;
    DatabaseResponse unlock_response;
    db.unlock_object(unlock_response, new_key, owner1);
    std::cout << unlock_response.error_message << std::endl;
    REQUIRE(unlock_response.success);

    // Try to obtain the lock with another device
    DatabaseResponse second_lock_response;
    db.lock_object(second_lock_response, new_key, owner2);
    std::cout << second_lock_response.error_message << std::endl;
    REQUIRE(second_lock_response.success);

    // Delete
    std::cout << "Deletion Test" << std::endl;
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
    REQUIRE(get2_response->get_error_code() > NO_ERROR);
    delete get2_response;

    delete test_object;
    delete test_object2;
  }
}
