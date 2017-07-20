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

#include <math.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>

#ifndef SRC_INCLUDE_TRANSFORMS_H_
#define SRC_INCLUDE_TRANSFORMS_H_

const int NO_TRANSFORM_TYPE = -1;
const int TRANSLATE = 0;
const int EROTATE = 1;
const int QROTATE = 2;
const int SCALE = 3;

const double PI = 3.14159265358979;

// Parent class for all transformations
class Transformation {
  glm::mat4 transform;
  int type;
  void init() {type = NO_TRANSFORM_TYPE; transform = glm::mat4(1.0);}

 public:
  // Destructor
  ~Transformation() {}
  // Constructors
  Transformation() {init();}
  // Copy Constructor
  inline Transformation(const Transformation &t) {
    init();
    transform = t.get_transform_vector();
    type = t.get_type();
  }
  int get_type() const {return type;}
  void set_type(int ntype) {type = ntype;}
  glm::mat4 get_transform_vector() const {return transform;}
  void set_transform_vector(glm::mat4 v) {transform = v;}
  void multiply(const Transformation &t) \
    {transform = transform * t.get_transform_vector();}
};

class Translation: public Transformation {
 public:
  inline Translation(float ix, float iy, float iz) : Transformation() {
    type = TRANSLATE;
    transform = glm::translate(glm::mat4(1.0f), glm::vec3(ix, iy, iz));
  }
  inline Translation(double ix, double iy, double iz) : \
    Translation((float) ix, (float) iy, (float) iz) {}
};

class EulerRotation: public Transformation {
 public:
  inline EulerRotation(float theta, float ix, float iy, float iz) :
    Transformation() {
      type = EROTATE;
      transform = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(ix, iy, iz));
    }
  EulerRotation(double theta, double ix, double iy, double iz) : \
    EulerRotation((float) theta, (float) ix, (float) iy, (float) iz) {}
};

class Scale: public Transformation {
 public:
  inline Scale(float ix, float iy, float iz) : Transformation() {
    type = SCALE;
    transform = glm::scale(glm::mat4(1.0f), glm::vec3(ix, iy, iz));
  }
  Scale(double ix, double iy, double iz) : \
    Scale((float) ix, (float) iy, (float) iz) {}
};

#endif  // SRC_INCLUDE_TRANSFORMS_H_
