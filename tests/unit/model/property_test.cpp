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

#include "include/animation_property.h"
#include "include/animation_graph_handle.h"

#include "catch.hpp"

TEST_CASE( "Test Animation Property Data Structure", "[unit]" ) {
  //Tolerance
  const int PI = 3.1415f;
  const float TOLERANCE = 0.1f;

  // Create a test Property
  AnimationProperty test_property;
  test_property.set_key(std::string("12345"));
  test_property.set_name(std::string("testName"));
  test_property.set_parent(std::string("testParent"));
  test_property.set_scene(std::string("testScene"));
  test_property.set_asset_sub_id(std::string("testAssetSubId"));
  test_property.set_frame(1);
  test_property.set_timestamp(123456789);
  test_property.add_value(100.0);
}
