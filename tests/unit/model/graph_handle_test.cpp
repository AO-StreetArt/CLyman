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

#include "include/animation_graph_handle.h"

#include "catch.hpp"

TEST_CASE( "Test Animation Graph Handle Data Structure", "[unit]" ) {
  //Tolerance
  const float TOLERANCE = 0.1f;

  // Build test graph handle
  AnimationGraphHandle test_handle;
  test_handle.set_lh_type(std::string("vector"));
  test_handle.set_lh_x(10.0);
  test_handle.set_lh_y(5.0);
  test_handle.set_rh_type(std::string("free"));
  test_handle.set_rh_x(4.0);
  test_handle.set_rh_y(3.0);
  REQUIRE(test_handle.get_lh_type() == "vector");
  REQUIRE(std::abs(test_handle.get_lh_x()) - 10.0 < TOLERANCE);
  REQUIRE(std::abs(test_handle.get_lh_y()) - 5.0 < TOLERANCE);
  REQUIRE(test_handle.get_rh_type() == "free");
  REQUIRE(std::abs(test_handle.get_rh_x()) - 4.0 < TOLERANCE);
  REQUIRE(std::abs(test_handle.get_rh_y()) - 3.0 < TOLERANCE);
}
