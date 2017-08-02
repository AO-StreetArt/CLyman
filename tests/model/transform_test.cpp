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

#include <iostream>
#include <cmath>
#include <assert.h>
#include <math.h>
#include "transforms.h"

int main(int argc, char** argv) {

  //Tolerance
  const float TOLERANCE = 0.001f;
  // Identity Transformation tests
  Translation Itrans;
  EulerRotation Ierot;
  Scale Iscl;

  assert(Itrans.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  assert(Itrans.get_transform_element(1, 1) - 1.0f < TOLERANCE);
  assert(Itrans.get_transform_element(2, 2) - 1.0f < TOLERANCE);
  assert(Itrans.get_transform_element(3, 3) - 1.0f < TOLERANCE);
  assert(Itrans.get_transform_element(0, 3) - 0.0f < TOLERANCE);
  assert(Itrans.get_transform_element(1, 3) - 0.0f < TOLERANCE);
  assert(Itrans.get_transform_element(2, 3) - 0.0f < TOLERANCE);

  assert(Ierot.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  assert(Ierot.get_transform_element(1, 1) - 1.0f < TOLERANCE);
  assert(Ierot.get_transform_element(2, 2) - 1.0f < TOLERANCE);
  assert(Ierot.get_transform_element(3, 3) - 1.0f < TOLERANCE);

  assert(Iscl.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  assert(Iscl.get_transform_element(1, 1) - 1.0f < TOLERANCE);
  assert(Iscl.get_transform_element(2, 2) - 1.0f < TOLERANCE);
  assert(Iscl.get_transform_element(3, 3) - 1.0f < TOLERANCE);

  // Type tests
  assert(Itrans.get_type() == TRANSLATE);
  assert(Ierot.get_type() == EROTATE);
  assert(Iscl.get_type() == SCALE);

  // Non-Identity Transformation Tests
  Translation trans(1.0f, 1.0f, 1.0f);
  EulerRotation rot(3.1415f, 1.0f, 0.0f, 0.0f);
  Scale scl(2.0f, 2.0f, 2.0f);

  assert(trans.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  assert(trans.get_transform_element(1, 1) - 1.0f < TOLERANCE);
  assert(trans.get_transform_element(2, 2) - 1.0f < TOLERANCE);
  assert(trans.get_transform_element(3, 3) - 1.0f < TOLERANCE);
  assert(trans.get_transform_element(0, 3) - 1.0f < TOLERANCE);
  assert(trans.get_transform_element(1, 3) - 1.0f < TOLERANCE);
  assert(trans.get_transform_element(2, 3) - 1.0f < TOLERANCE);

  assert(rot.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  assert(std::abs(rot.get_transform_element(1, 1) + 1.0f) < TOLERANCE);
  assert(std::abs(rot.get_transform_element(2, 2) + 1.0f) < TOLERANCE);
  assert(rot.get_transform_element(3, 3) - 1.0f < TOLERANCE);

  assert(scl.get_transform_element(0, 0) - 2.0f < TOLERANCE);
  assert(scl.get_transform_element(1, 1) - 2.0f < TOLERANCE);
  assert(scl.get_transform_element(2, 2) - 2.0f < TOLERANCE);
  assert(scl.get_transform_element(3, 3) - 1.0f < TOLERANCE);

  // Type tests
  assert(trans.get_type() == TRANSLATE);
  assert(rot.get_type() == EROTATE);
  assert(scl.get_type() == SCALE);

  // Multiplication Tests

  // Create a base identity transformation
  Transformation base_transform;
  std::cout << base_transform.to_string() << std::endl;
  assert(base_transform.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  assert(base_transform.get_transform_element(1, 1) - 1.0f < TOLERANCE);
  assert(base_transform.get_transform_element(2, 2) - 1.0f < TOLERANCE);
  assert(base_transform.get_transform_element(3, 3) - 1.0f < TOLERANCE);

  // Translate the base transformation
  base_transform.multiply(trans);
  std::cout << base_transform.to_string() << std::endl;
  assert(base_transform.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  assert(base_transform.get_transform_element(1, 1) - 1.0f < TOLERANCE);
  assert(base_transform.get_transform_element(2, 2) - 1.0f < TOLERANCE);
  assert(base_transform.get_transform_element(3, 3) - 1.0f < TOLERANCE);
  assert(base_transform.get_transform_element(0, 3) - 1.0f < TOLERANCE);
  assert(base_transform.get_transform_element(1, 3) - 1.0f < TOLERANCE);
  assert(base_transform.get_transform_element(2, 3) - 1.0f < TOLERANCE);

  // Rotate the base transformation
  base_transform.multiply(rot);
  std::cout << base_transform.to_string() << std::endl;
  assert(base_transform.get_transform_element(0, 0) - 1.0f < TOLERANCE);
  assert(std::abs(base_transform.get_transform_element(1, 1) + 1.0f) \
    < TOLERANCE);
  assert(std::abs(base_transform.get_transform_element(2, 2) + 1.0f) \
    < TOLERANCE);
  assert(base_transform.get_transform_element(3, 3) - 1.0f < TOLERANCE);
  assert(base_transform.get_transform_element(0, 3) - 1.0f < TOLERANCE);
  assert(std::abs(base_transform.get_transform_element(1, 3) + 1.0f) \
    < TOLERANCE);
  assert(std::abs(base_transform.get_transform_element(2, 3) + 1.0f) \
    < TOLERANCE);

  // Scale the base transformation
  base_transform.multiply(scl);
  std::cout << base_transform.to_string() << std::endl;
  assert(base_transform.get_transform_element(0, 0) - 2.0f < TOLERANCE);
  assert(base_transform.get_transform_element(1, 1) + 2.0f < TOLERANCE);
  assert(base_transform.get_transform_element(2, 2) + 2.0f < TOLERANCE);
  assert(base_transform.get_transform_element(3, 3) - 1.0f < TOLERANCE);
  assert(base_transform.get_transform_element(0, 3) - 2.0f < TOLERANCE);
  assert(base_transform.get_transform_element(1, 3) + 2.0f < TOLERANCE);
  assert(base_transform.get_transform_element(2, 3) + 2.0f < TOLERANCE);

  return 0;
}
