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

#include "include/transforms.h"

// Translation (addition)
void Transformation::add_transform(double iw, double ix, double iy, double iz) {
  transform(0) = transform(0) + iw;
  transform(1) = transform(1) + ix;
  transform(2) = transform(2) + iy;
  transform(3) = transform(3) + iz;
}

// Rotation (quaternion multiplication)
void Transformation::mult_transform(double iw, double ix, double iy, double iz) {
  Eigen::Vector4d new_transform = Eigen::Vector4d::Zero(4);
  new_transform(0) = transform(0) * iw - transform(1) * ix - \
    transform(2) * iy - transform(3) * iz;
  new_transform(1) = transform(0) * ix + transform(1) * iw + \
    transform(2) * iz - transform(3) * iy;
  new_transform(2) = transform(0) * iy - transform(1) * iz + \
    transform(2) * iw + transform(3) * ix;
  new_transform(3) = transform(0) * iz + transform(1) * iy - \
    transform(2) * ix + transform(3) * iw;
  transform = new_transform;
}

// Scale (Multiplication)
void Transformation::mult_transform(double ix, double iy, double iz) {
  transform(1) = transform(1) * ix;
  transform(2) = transform(2) * iy;
  transform(3) = transform(3) * iz;
}

// Add an Euler Rotation
void EulerRotation::add(double ix, double iy, double iz) {
  Transformation::add_transform(0.0, ix, iy, iz);
  double curx = Transformation::get_x();
  double cury = Transformation::get_y();
  double curz = Transformation::get_z();
  //Return the resulting transform, modulus 2pi
  if (curx > 2 * PI) Transformation::set_x(std::fmod(curx, (2 * PI)));
  if (cury > 2 * PI) Transformation::set_y(std::fmod(cury, (2 * PI)));
  if (curz > 2 * PI) Transformation::set_z(std::fmod(curz, (2 * PI)));
}
