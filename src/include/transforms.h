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
#include <Eigen/Dense>
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
  bool is_local = false;
  Eigen::Vector4d transform;
  int type;
  void init() {type = NO_TRANSFORM_TYPE; transform = Eigen::Vector4d::Zero(4);}

 public:
  // Destructor
  ~Transformation() {}
  // Constructors
  inline Transformation(double iw, double ix, double iy, double iz) {
    init();
    transform(0) = iw;
    transform(1) = ix;
    transform(2) = iy;
    transform(3) = iz;
  }
  Transformation(double ix, double iy, double iz) : Transformation(0.0, \
    ix, iy, iz) {}
  Transformation() : Transformation(0.0, 0.0, 0.0, 0.0) {}
  // Copy Constructor
  inline Transformation(const Transformation &t) {
    init();
    is_local = t.get_local();
    transform = t.get_transform_vector();
    type = t.get_type();
  }
  int get_type() const {return type;}
  void set_type(int ntype) {type = ntype;}
  void set_local(bool local) {is_local = local;}
  bool get_local() const {return is_local;}
  double get_w() const {return transform(0);}
  double get_x() const {return transform(1);}
  double get_y() const {return transform(2);}
  double get_z() const {return transform(3);}
  void set_w(double iw) {transform(0) = iw;}
  void set_x(double ix) {transform(1) = ix;}
  void set_y(double iy) {transform(2) = iy;}
  void set_z(double iz) {transform(3) = iz;}
  Eigen::Vector4d get_transform_vector() const {return transform;}
  void set_transform_vector(Eigen::Vector4d v) {transform = v;}
  void add_transform(double iw, double ix, double iy, double iz);
  void mult_transform(double iw, double ix, double iy, double iz);
  void mult_transform(double ix, double iy, double iz);
};

class Translation: public Transformation {
 public:
  Translation(double ix, double iy, double iz) : Transformation(ix, \
    iy, iz) {Transformation::set_type(TRANSLATE);}
  Translation() : Translation(0.0, 0.0, 0.0) {}
  inline void add(double ix, double iy, double iz) {
    Transformation::add_transform(0.0, ix, iy, iz);
  }
};

class EulerRotation: public Transformation {
 public:
  EulerRotation(double ix, double iy, double iz) : Transformation(ix, \
    iy, iz) {Transformation::set_type(EROTATE);}
  EulerRotation() : EulerRotation(0.0, 0.0, 0.0) {}
  void add(double ix, double iy, double iz);
};

class QuaternionRotation: public Transformation {
 public:
  QuaternionRotation(double iw, double ix, double iy, double iz) : \
    Transformation(iw, ix, iy, iz) {Transformation::set_type(QROTATE);}
  QuaternionRotation() : QuaternionRotation(1.0, 0.0, 0.0, 0.0) {}
  inline void add(double iw, double ix, double iy, double iz) {
    Transformation::mult_transform(iw, ix, iy, iz);
  }
};

class Scale: public Transformation {
 public:
  Scale(double ix, double iy, double iz) : Transformation(ix, \
    iy, iz) {Transformation::set_type(SCALE);}
  Scale() : Scale(1.0, 1.0, 1.0) {}
  inline void add(double ix, double iy, double iz) {
    Transformation::mult_transform(ix, iy, iz);
  }
};

#endif  // SRC_INCLUDE_TRANSFORMS_H_
