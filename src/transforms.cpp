#include "include/transforms.h"

void Transformation::add_transform (double iw, double ix, double iy, double iz) {
  transform(0) = transform(0) + iw;
  transform(1) = transform(1) + iw;
  transform(2) = transform(2) + iw;
  transform(3) = transform(3) + iw;
}

//Quaternion multiplication
void Transformation::mult_transform (double iw, double ix, double iy, double iz) {
  Eigen::Vector4d new_transform = Eigen::Vector4d::Zero(4);
  new_transform(0) = transform(0) * iw - transform(1) * ix - transform(2) * iy - transform(3) * iz;
  new_transform(1) = transform(0) * ix + transform(1) * iw + transform(2) * iz - transform(3) * iy;
  new_transform(2) = transform(0) * iy - transform(1) * iz + transform(2) * iw + transform(3) * ix;
  new_transform(3) = transform(0) * iz + transform(1) * iy - transform(2) * ix + transform(3) * iw;
  transform = new_transform;
}

void Transformation::mult_transform(double ix, double iy, double iz) {
  transform(1) = transform(1) * ix;
  transform(2) = transform(2) * iy;
  transform(3) = transform(3) * iz;
}

void EulerRotation::add(double ix, double iy, double iz) {
  Transformation::add_transform(0.0, ix, iy, iz);
  double curx = Transformation::get_x();
  double cury = Transformation::get_y();
  double curz = Transformation::get_z();
  if ( curx > 2 * PI ) Transformation::set_x( curx - (2 * PI) );
  if ( cury > 2 * PI ) Transformation::set_y( cury - (2 * PI) );
  if ( curz > 2 * PI ) Transformation::set_z( curz - (2 * PI) );
}
