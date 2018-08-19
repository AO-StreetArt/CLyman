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

#include "catch.hpp"

TEST_CASE( "Test Transform Data Structure", "[unit]" ) {
  // Tolerance
  const double TOLERANCE = 0.001;

  // Translation tests
  Translation translate(1.0f, 1.0f, 1.0f);
  REQUIRE(std::abs(translate.get_transform_element(0,0)) - 1.0f < TOLERANCE);
  REQUIRE(std::abs(translate.get_transform_element(1,1)) - 1.0f < TOLERANCE);
  REQUIRE(std::abs(translate.get_transform_element(2,2)) - 1.0f < TOLERANCE);
}
