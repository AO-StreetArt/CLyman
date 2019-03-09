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

#include "include/animation_action.h"
#include "include/object_frame.h"
#include "include/property_frame.h"

#include "catch.hpp"

TEST_CASE( "Test Animation Action Data Structure", "[unit]" ) {
  // Build test actions
  AnimationAction<ObjectFrame> test_obj_action;
  std::string oaction_name = "testObjAction";
  std::string oaction_desc = "testObjDesc";
  std::string oaction_owner = "testObjOwner";
  test_obj_action->set_name(oaction_name);
  test_obj_action->set_description(oaction_desc);
  test_obj_action->set_owner(oaction_owner);
  AnimationAction<PropertyFrame> test_prop_action;
  std::string paction_name = "testPropAction";
  std::string paction_desc = "testPropDesc";
  std::string paction_owner = "testPropOwner";
  test_prop_action->set_name(paction_name);
  test_prop_action->set_description(paction_desc);
  test_prop_action->set_owner(paction_owner);

  ObjectFrame *obj_frame = new ObjectFrame();
  obj_frame->set_frame(1);
  test_obj_action->add_keyframe(1, obj_frame);
  PropertyFrame *prop_frame = new PropertyFrame();
  prop_frame->set_frame(2);
  test_prop_action->add_keyframe(1, prop_frame);

  REQUIRE(test_obj_action.get_name() == "testObjAction");
  REQUIRE(test_obj_action.get_description() == "testObjDesc");
  REQUIRE(test_obj_action.get_owner() == "testObjOwner");
  REQUIRE(test_obj_action.get_keyframe(1).get_frame() == 1);
  REQUIRE(test_prop_action.get_name() == "testPropAction");
  REQUIRE(test_prop_action.get_description() == "testPropDesc");
  REQUIRE(test_prop_action.get_owner() == "testPropOwner");
  REQUIRE(test_obj_action.get_keyframe(2).get_frame() == 2);

}
