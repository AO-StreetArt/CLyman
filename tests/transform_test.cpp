#include "transforms.h"
#include <assert.h>
#include <math.h>

int main( int argc, char** argv )
{
  //Constructor tests
  Translation trans;
  EulerRotation erot;
  QuaternionRotation qrot;
  Scale scl;

  assert ( trans.get_w() == 0.0 );
  assert ( trans.get_x() == 0.0 );
  assert ( trans.get_y() == 0.0 );
  assert ( trans.get_z() == 0.0 );

  assert ( erot.get_w() == 0.0 );
  assert ( erot.get_x() == 0.0 );
  assert ( erot.get_y() == 0.0 );
  assert ( erot.get_z() == 0.0 );

  assert ( qrot.get_w() == 1.0 );
  assert ( qrot.get_x() == 0.0 );
  assert ( qrot.get_y() == 0.0 );
  assert ( qrot.get_z() == 0.0 );

  assert ( scl.get_w() == 0.0 );
  assert ( scl.get_x() == 1.0 );
  assert ( scl.get_y() == 1.0 );
  assert ( scl.get_z() == 1.0 );

  //Add Tests
  trans.add(1.0, 2.0, 3.0);
  erot.add(3 * PI, 3.5 * PI, 4.02 * PI);
  qrot.add(0.0, sqrt(3.0)/3.0, sqrt(3.0)/3.0, sqrt(3.0)/3.0);
  scl.add(2.0, 4.0, 8.0);

  assert ( trans.get_w() - 0.0 < 0.001 );
  assert ( trans.get_x() - 1.0 < 0.001 );
  assert ( trans.get_y() - 2.0 < 0.001 );
  assert ( trans.get_z() - 3.0 < 0.001 );

  assert ( erot.get_w() - 0.0 < 0.001 );
  assert ( erot.get_x() - PI < 0.001 );
  assert ( erot.get_y() - (1.5 * PI) < 0.001 );
  assert ( erot.get_z() - (0.02 * PI) < 0.001 );

  assert ( qrot.get_w() - 0.0 < 0.001 );
  assert ( qrot.get_x() - sqrt(3.0)/3.0 < 0.001 );
  assert ( qrot.get_y() - sqrt(3.0)/3.0 < 0.001 );
  assert ( qrot.get_z() - sqrt(3.0)/3.0 < 0.001 );

  assert ( scl.get_w() - 0.0 < 0.001 );
  assert ( scl.get_x() - 2.0 < 0.001 );
  assert ( scl.get_y() - 4.0 < 0.001 );
  assert ( scl.get_z() - 8.0 < 0.001 );

  //Type tests
  assert ( trans.get_type() == TRANSLATE );
  assert ( erot.get_type() == EROTATE );
  assert ( qrot.get_type() == QROTATE );
  assert ( scl.get_type() == SCALE );

  return 0;
}
