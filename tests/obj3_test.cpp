#include "transforms.h"
#include "obj3.h"
#include <assert.h>
#include <math.h>
#include <iostream>

#include "aossl/logging/include/logging_interface.h"
#include "aossl/logging/include/factory_logging.h"

#include "app_log.h"

int main( int argc, char** argv )
{

  LoggingComponentFactory *logging_factory = new LoggingComponentFactory;

  std::string initFileName = "tests/log4cpp.properties";
  logging = logging_factory->get_logging_interface(initFileName);

  start_logging_submodules();

  main_logging->debug("Basic Tests");

  //Constructor tests
  Obj3 test_object;
  assert( test_object.get_key() == "" );
  assert( test_object.get_name() == "" );
  assert( test_object.get_scene() == "" );
  assert( test_object.get_type() == "" );
  assert( test_object.get_subtype() == "" );
  assert( test_object.get_owner() == "" );
  assert( test_object.num_assets() == 0 );

  std::string test_key = "abcdefg";
  test_object.set_key(test_key);
  std::string test_name = "abcdefgh";
  test_object.set_name(test_name);
  std::string test_scene = "abcdefghi";
  test_object.set_scene(test_scene);
  std::string test_type = "abcdefghij";
  test_object.set_type(test_type);
  std::string test_subtype = "abcdefghijk";
  test_object.set_subtype(test_subtype);
  std::string test_owner = "abcdefghijkl";
  test_object.set_owner(test_owner);

  assert( test_object.get_key() == "abcdefg" );
  assert( test_object.get_name() == "abcdefgh" );
  assert( test_object.get_scene() == "abcdefghi" );
  assert( test_object.get_type() == "abcdefghij" );
  assert( test_object.get_subtype() == "abcdefghijk" );
  assert( test_object.get_owner() == "abcdefghijkl" );

  //Asset tests
  main_logging->debug("Asset Tests");
  std::string asset1 = "12345";
  std::string asset2 = "12346";
  test_object.add_asset(asset1);
  test_object.add_asset(asset2);
  assert( test_object.num_assets() == 2 );
  assert( test_object.get_asset(0) == "12345" );
  assert( test_object.get_asset(1) == "12346" );

  //Transform tests
  main_logging->debug("Transform Tests");
  Translation *trans = new Translation;
  EulerRotation *erot = new EulerRotation;
  QuaternionRotation *qrot = new QuaternionRotation;
  Scale *scl = new Scale;
  trans->add(1.0, 2.0, 3.0);
  erot->add(3 * PI, 3.5 * PI, 4.02 * PI);
  qrot->add(0.0, sqrt(3.0)/3.0, sqrt(3.0)/3.0, sqrt(3.0)/3.0);
  scl->add(2.0, 4.0, 8.0);

  test_object.transform(trans);
  assert ( test_object.get_translation()->get_w() - 0.0 < 0.001 );
  assert ( test_object.get_translation()->get_x() - 1.0 < 0.001 );
  assert ( test_object.get_translation()->get_y() - 2.0 < 0.001 );
  assert ( test_object.get_translation()->get_z() - 3.0 < 0.001 );

  test_object.transform(erot);
  assert ( test_object.get_erotation()->get_w() - 0.0 < 0.001 );
  assert ( test_object.get_erotation()->get_x() - PI < 0.001 );
  assert ( test_object.get_erotation()->get_y() - (1.5 * PI) < 0.001 );
  assert ( test_object.get_erotation()->get_z() - (0.02 * PI) < 0.001 );

  test_object.transform(qrot);
  assert ( test_object.get_qrotation()->get_w() - 0.0 < 0.001 );
  assert ( test_object.get_qrotation()->get_x() - sqrt(3.0)/3.0 < 0.001 );
  assert ( test_object.get_qrotation()->get_y() - sqrt(3.0)/3.0 < 0.001 );
  assert ( test_object.get_qrotation()->get_z() - sqrt(3.0)/3.0 < 0.001 );

  test_object.transform(scl);
  assert ( test_object.get_scale()->get_w() - 0.0 < 0.001 );
  assert ( test_object.get_scale()->get_w() - 2.0 < 0.001 );
  assert ( test_object.get_scale()->get_w() - 4.0 < 0.001 );
  assert ( test_object.get_scale()->get_w() - 8.0 < 0.001 );

  main_logging->debug("JSON Tests");

  std::string obj_json = test_object.to_json();
  rapidjson::Document d;
  d.Parse<rapidjson::kParseStopWhenDoneFlag>(obj_json.c_str());
  Obj3 *translated_object = new Obj3 (d);

  assert( translated_object->get_key() == "abcdefg" );
  assert( translated_object->get_name() == "abcdefgh" );
  assert( translated_object->get_scene() == "abcdefghi" );
  assert( translated_object->get_type() == "abcdefghij" );
  assert( translated_object->get_subtype() == "abcdefghijk" );
  assert( translated_object->get_owner() == "abcdefghijkl" );

  assert ( translated_object->get_translation()->get_w() - 0.0 < 0.001 );
  assert ( translated_object->get_translation()->get_x() - 1.0 < 0.001 );
  assert ( translated_object->get_translation()->get_y() - 2.0 < 0.001 );
  assert ( translated_object->get_translation()->get_z() - 3.0 < 0.001 );
  assert ( translated_object->get_erotation()->get_w() - 0.0 < 0.001 );
  assert ( translated_object->get_erotation()->get_x() - PI < 0.001 );
  assert ( translated_object->get_erotation()->get_y() - (1.5 * PI) < 0.001 );
  assert ( translated_object->get_erotation()->get_z() - (0.02 * PI) < 0.001 );
  assert ( translated_object->get_qrotation()->get_w() - 0.0 < 0.001 );
  assert ( translated_object->get_qrotation()->get_x() - sqrt(3.0)/3.0 < 0.001 );
  assert ( translated_object->get_qrotation()->get_y() - sqrt(3.0)/3.0 < 0.001 );
  assert ( translated_object->get_qrotation()->get_z() - sqrt(3.0)/3.0 < 0.001 );
  assert ( translated_object->get_scale()->get_w() - 0.0 < 0.001 );
  assert ( translated_object->get_scale()->get_w() - 2.0 < 0.001 );
  assert ( translated_object->get_scale()->get_w() - 4.0 < 0.001 );
  assert ( translated_object->get_scale()->get_w() - 8.0 < 0.001 );

  assert( translated_object->num_assets() == 2 );
  assert( translated_object->get_asset(0) == "12345" );
  assert( translated_object->get_asset(1) == "12346" );

  //Merge Test
  main_logging->debug("Merge Tests");
  Obj3 *obj_update = new Obj3;
  std::string upd_key = "new_key";
  obj_update->set_key(upd_key);
  std::string upd_name = "new_name";
  obj_update->set_name(upd_name);
  std::string upd_owner = "new_owner";
  obj_update->set_owner(upd_owner);

  Translation *trans2 = new Translation;
  trans2->add(1.0, 2.0, 3.0);
  obj_update->transform(trans2);

  std::string upd_asset = "another_asset";
  obj_update->add_asset(upd_asset);

  translated_object->merge(obj_update);

  assert( translated_object->get_key() == "new_key" );
  assert( translated_object->get_name() == "new_name" );
  assert( translated_object->get_scene() == "abcdefghi" );
  assert( translated_object->get_type() == "abcdefghij" );
  assert( translated_object->get_subtype() == "abcdefghijk" );
  assert( translated_object->get_owner() == "new_owner" );

  assert ( translated_object->get_translation()->get_w() - 0.0 < 0.001 );
  assert ( translated_object->get_translation()->get_x() - 2.0 < 0.001 );
  assert ( translated_object->get_translation()->get_y() - 4.0 < 0.001 );
  assert ( translated_object->get_translation()->get_z() - 6.0 < 0.001 );
  assert ( translated_object->get_erotation()->get_w() - 0.0 < 0.001 );
  assert ( translated_object->get_erotation()->get_x() - PI < 0.001 );
  assert ( translated_object->get_erotation()->get_y() - (1.5 * PI) < 0.001 );
  assert ( translated_object->get_erotation()->get_z() - (0.02 * PI) < 0.001 );
  assert ( translated_object->get_qrotation()->get_w() - 0.0 < 0.001 );
  assert ( translated_object->get_qrotation()->get_x() - sqrt(3.0)/3.0 < 0.001 );
  assert ( translated_object->get_qrotation()->get_y() - sqrt(3.0)/3.0 < 0.001 );
  assert ( translated_object->get_qrotation()->get_z() - sqrt(3.0)/3.0 < 0.001 );
  assert ( translated_object->get_scale()->get_w() - 0.0 < 0.001 );
  assert ( translated_object->get_scale()->get_w() - 2.0 < 0.001 );
  assert ( translated_object->get_scale()->get_w() - 4.0 < 0.001 );
  assert ( translated_object->get_scale()->get_w() - 8.0 < 0.001 );

  assert( translated_object->num_assets() == 3 );
  assert( translated_object->get_asset(0) == "12345" );
  assert( translated_object->get_asset(1) == "12346" );
  assert( translated_object->get_asset(2) == "another_asset" );

  delete obj_update;

  delete translated_object;

  //Asset removal tests
  main_logging->debug("Asset Removal Tests");
  test_object.remove_asset(0);
  assert( test_object.num_assets() == 1 );
  assert( test_object.get_asset(0) == "12346" );
  test_object.clear_assets();
  assert( test_object.num_assets() == 0 );

  delete trans;
  delete erot;
  delete qrot;
  delete scl;

  shutdown_logging_submodules();

  delete logging;
  delete logging_factory;
}
