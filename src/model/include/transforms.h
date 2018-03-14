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

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <string>

#ifndef SRC_MODEL_INCLUDE_TRANSFORMS_H_
#define SRC_MODEL_INCLUDE_TRANSFORMS_H_

const int NO_TRANSFORM_TYPE = 0;
const int TRANSLATE = 1;
const int EROTATE = 2;
const int SCALE = 3;

const double PI = 3.14159265358979;

// Parent class for all transformations, abstracts a 4x4 transform matrix
// Transformations are set with constructors and/or getters/setters
// Then multiplied together to apply them
// Subclasses are different types of transforms (translation, rotation, etc)
class Transformation {
  glm::mat4 transform;
  int type;
  std::string string_representation;
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
  // Getter/Setter for type
  int get_type() const {return type;}
  void set_type(int ntype) {type = ntype;}
  // Getter/Setter for transform matrix
  glm::mat4 get_transform_vector() const {return transform;}
  float get_transform_element(int i, int j) const {return transform[j][i];}
  void set_transform_vector(glm::mat4 v) {transform = v;}
  void set_transform_element(int i, int j, float val) {transform[j][i] = val;}
  void set_transform_element(int i, int j, double val) \
    {set_transform_element(i, j, (float)val);}
  // Apply a transformation onto this transformation using LHS
  // Matrix Multiplication
  void multiply(const Transformation &t) \
    {transform = t.get_transform_vector() * transform;}
  inline std::string to_string() {
    string_representation = glm::to_string(transform);
    return string_representation;
  }
};

// Translation Class
class Translation: public Transformation {
 public:
  inline Translation(float ix, float iy, float iz) : Transformation() {
    set_type(TRANSLATE);
    set_transform_vector(glm::translate(glm::mat4(1.0f), \
      glm::vec3(ix, iy, iz)));
  }
  Translation(double ix, double iy, double iz) : \
    Translation((float) ix, (float) iy, (float) iz) {}
  // Identity Transform
  Translation() : Translation(0.0f, 0.0f, 0.0f) {}
};

// Rotation Class
class EulerRotation: public Transformation {
 public:
  inline EulerRotation(float theta, float ix, float iy, float iz) :
    Transformation() {
      set_type(EROTATE);
      // Rotate theta degrees about the vector <ix, iy, iz>
      set_transform_vector(glm::rotate(glm::mat4(1.0f), \
        theta, glm::vec3(ix, iy, iz)));
    }
  EulerRotation(double theta, double ix, double iy, double iz) : \
    EulerRotation((float) theta, (float) ix, (float) iy, (float) iz) {}
  // Identity Transform
  EulerRotation() : EulerRotation(0.0f, 1.0f, 0.0f, 0.0f) {}
};

// Scale Class
class Scale: public Transformation {
 public:
  inline Scale(float ix, float iy, float iz) : Transformation() {
    set_type(SCALE);
    set_transform_vector(glm::scale(glm::mat4(1.0f), glm::vec3(ix, iy, iz)));
  }
  Scale(double ix, double iy, double iz) : \
    Scale((float) ix, (float) iy, (float) iz) {}
  // Identity Transform
  Scale() : Scale(1.0f, 1.0f, 1.0f) {}
};

#endif  // SRC_MODEL_INCLUDE_TRANSFORMS_H_
