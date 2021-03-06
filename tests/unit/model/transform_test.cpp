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
  //Tolerance
  const int PI = 3.1415f;
  const float TOLERANCE = 0.1f;
  // Identity Transformation tests
  Translation Itrans;
  EulerRotation Ierot;
  Scale Iscl;

  REQUIRE(Itrans.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  REQUIRE(Itrans.get_transform_element(1, 1) - 1.0f < TOLERANCE);
  REQUIRE(Itrans.get_transform_element(2, 2) - 1.0f < TOLERANCE);
  REQUIRE(Itrans.get_transform_element(3, 3) - 1.0f < TOLERANCE);
  REQUIRE(Itrans.get_transform_element(0, 3) - 0.0f < TOLERANCE);
  REQUIRE(Itrans.get_transform_element(1, 3) - 0.0f < TOLERANCE);
  REQUIRE(Itrans.get_transform_element(2, 3) - 0.0f < TOLERANCE);

  REQUIRE(Ierot.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  REQUIRE(Ierot.get_transform_element(1, 1) - 1.0f < TOLERANCE);
  REQUIRE(Ierot.get_transform_element(2, 2) - 1.0f < TOLERANCE);
  REQUIRE(Ierot.get_transform_element(3, 3) - 1.0f < TOLERANCE);

  REQUIRE(Iscl.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  REQUIRE(Iscl.get_transform_element(1, 1) - 1.0f < TOLERANCE);
  REQUIRE(Iscl.get_transform_element(2, 2) - 1.0f < TOLERANCE);
  REQUIRE(Iscl.get_transform_element(3, 3) - 1.0f < TOLERANCE);

  // Type tests
  REQUIRE(Itrans.get_type() == TRANSLATE);
  REQUIRE(Ierot.get_type() == EROTATE);
  REQUIRE(Iscl.get_type() == SCALE);

  // Non-Identity Transformation Tests
  Translation trans(1.0f, 1.0f, 1.0f);
  EulerRotation rot(PI, 0.0f, 0.0f);
  Scale scl(2.0f, 2.0f, 2.0f);
  std::cout << "Transformations" << std::endl;
  std::cout << trans.to_string() << std::endl;
  std::cout << rot.to_string() << std::endl;
  std::cout << scl.to_string() << std::endl;

  REQUIRE(trans.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  REQUIRE(trans.get_transform_element(1, 1) - 1.0f < TOLERANCE);
  REQUIRE(trans.get_transform_element(2, 2) - 1.0f < TOLERANCE);
  REQUIRE(trans.get_transform_element(3, 3) - 1.0f < TOLERANCE);
  REQUIRE(trans.get_transform_element(0, 3) - 1.0f < TOLERANCE);
  REQUIRE(trans.get_transform_element(1, 3) - 1.0f < TOLERANCE);
  REQUIRE(trans.get_transform_element(2, 3) - 1.0f < TOLERANCE);

  REQUIRE(rot.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  REQUIRE(std::abs(rot.get_transform_element(1, 1) + 1.0f) < TOLERANCE);
  REQUIRE(std::abs(rot.get_transform_element(2, 2) + 1.0f) < TOLERANCE);
  REQUIRE(rot.get_transform_element(3, 3) - 1.0f < TOLERANCE);

  REQUIRE(scl.get_transform_element(0, 0) - 2.0f < TOLERANCE);
  REQUIRE(scl.get_transform_element(1, 1) - 2.0f < TOLERANCE);
  REQUIRE(scl.get_transform_element(2, 2) - 2.0f < TOLERANCE);
  REQUIRE(scl.get_transform_element(3, 3) - 1.0f < TOLERANCE);

  // Type tests
  REQUIRE(trans.get_type() == TRANSLATE);
  REQUIRE(rot.get_type() == EROTATE);
  REQUIRE(scl.get_type() == SCALE);

  // Multiplication Tests

  // Create a base identity transformation
  Transformation base_transform;
  std::cout << base_transform.to_string() << std::endl;
  REQUIRE(base_transform.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(1, 1) - 1.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(2, 2) - 1.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(3, 3) - 1.0f < TOLERANCE);

  // Translate the base transformation
  base_transform.multiply(trans);
  std::cout << base_transform.to_string() << std::endl;
  REQUIRE(base_transform.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(1, 1) - 1.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(2, 2) - 1.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(3, 3) - 1.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(0, 3) - 1.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(1, 3) - 1.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(2, 3) - 1.0f < TOLERANCE);

  // Rotate the base transformation
  base_transform.multiply(rot);
  std::cout << base_transform.to_string() << std::endl;
  REQUIRE(base_transform.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  REQUIRE(std::abs(base_transform.get_transform_element(1, 1) + 1.0f) \
    < TOLERANCE);
  REQUIRE(std::abs(base_transform.get_transform_element(2, 2) + 1.0f) \
    < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(3, 3) - 1.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(0, 3) - 2.0f < TOLERANCE);
  REQUIRE(std::abs(base_transform.get_transform_element(1, 3) + 1.13f) \
    < TOLERANCE);
  REQUIRE(std::abs(base_transform.get_transform_element(2, 3) + 0.84f) \
    < TOLERANCE);

  // Scale the base transformation
  base_transform.multiply(scl);
  std::cout << base_transform.to_string() << std::endl;
  REQUIRE(base_transform.get_transform_element(0, 0) - 2.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(1, 1) - 2.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(2, 2) + 2.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(3, 3) - 1.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(0, 3) - 2.0f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(1, 3) + 2.26f < TOLERANCE);
  REQUIRE(base_transform.get_transform_element(2, 3) + 1.69f < TOLERANCE);

}
