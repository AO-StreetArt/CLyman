#include "transforms.h"
#include "obj3_list.h"
#include <assert.h>
#include <math.h>

#include "aossl/logging/include/logging_interface.h"
#include "aossl/logging/include/factory_logging.h"

int main( int argc, char** argv )
{

  LoggingComponentFactory *logging_factory = new LoggingComponentFactory;

  std::string initFileName = "tests/log4cpp.properties";
  logging = logging_factory->get_logging_interface(initFileName);

  start_logging_submodules();

  main_logging->debug("Setup");

  //Setup
  Obj3 *test_object = new Obj3;
  Obj3 *test_object2 = new Obj3;
  Translation *trans = new Translation;
  EulerRotation *erot = new EulerRotation;
  QuaternionRotation *qrot = new QuaternionRotation;
  Scale *scl = new Scale;

  std::string key1 = "abcdef";
  std::string key2 = "abcdefg";
  std::string name1 = "abcdefgh";
  std::string name2 = "abcdefghi";
  std::string scene1 = "abcdefghij";
  std::string scene2 = "abcdefghijk";
  std::string type1 = "abcdefghijkl";
  std::string type2 = "abcdefghijklm";
  std::string subtype1 = "abcdefghijklmn";
  std::string subtype2 = "abcdefghijklmno";
  std::string owner1 = "abcdefghijklmnop";
  std::string owner2 = "abcdefghijklmnopq";

  test_object->set_key(key1);
  test_object->set_name(name1);
  test_object->set_scene(scene1);
  test_object->set_type(type1);
  test_object->set_subtype(subtype1);
  test_object->set_owner(owner1);

  test_object2->set_key(key2);
  test_object2->set_name(name2);
  test_object2->set_scene(scene2);
  test_object2->set_type(type2);
  test_object2->set_subtype(subtype2);
  test_object2->set_owner(owner2);

  assert ( test_object->get_key() == "abcdef" );
  assert ( test_object->get_name() == "abcdefgh" );
  assert ( test_object->get_scene() == "abcdefghij" );
  assert ( test_object->get_type() == "abcdefghijkl" );
  assert ( test_object->get_subtype() == "abcdefghijklmn" );
  assert ( test_object->get_owner() == "abcdefghijklmnop" );

  assert ( test_object2->get_key() == "abcdefg" );
  assert ( test_object2->get_name() == "abcdefghi" );
  assert ( test_object2->get_scene() == "abcdefghijk" );
  assert ( test_object2->get_type() == "abcdefghijklm" );
  assert ( test_object2->get_subtype() == "abcdefghijklmno" );
  assert ( test_object2->get_owner() == "abcdefghijklmnopq" );

  std::string asset1 = "12345";
  std::string asset2 = "12346";
  std::string asset3 = "12347";
  test_object->add_asset(asset1);
  test_object->add_asset(asset2);
  test_object2->add_asset(asset3);

  trans->add(1.0, 2.0, 3.0);
  erot->add(3 * PI, 3.5 * PI, 4.02 * PI);
  qrot->add(0.0, sqrt(3.0)/3.0, sqrt(3.0)/3.0, sqrt(3.0)/3.0);
  scl->add(2.0, 4.0, 8.0);
  test_object->transform(trans);
  test_object2->transform(erot);
  test_object->transform(qrot);
  test_object2->transform(scl);

  //Constructor Test
  main_logging->debug("Basic Tests");
  Obj3List *olist = new Obj3List;
  assert( olist->get_msg_type() == -1 );
  assert( olist->get_error_code() == 100 );
  assert( olist->get_error_message() == "" );
  assert( olist->get_transaction_id() == "" );
  assert( olist->get_num_records() == 0 );
  assert( olist->num_objects() == 0 );

  //Getter/Setter tests
  olist->set_msg_type(1);
  assert( olist->get_msg_type() == 1 );
  olist->set_error_code(110);
  assert( olist->get_error_code() == 110 );
  std::string new_err_msg = "Test";
  olist->set_error_message(new_err_msg);
  assert( olist->get_error_message() == "Test" );
  std::string tran_id = "123456789";
  olist->set_transaction_id(tran_id);
  assert( olist->get_transaction_id() == "123456789" );
  olist->set_num_records(3);
  assert( olist->get_num_records() == 3 );
  olist->add_object(test_object);
  assert( olist->num_objects() == 1 );
  olist->add_object(test_object2);
  assert( olist->num_objects() == 2 );


  //Protocol Buffer Tests
  std::string proto_string = olist->to_protobuf();
  protoObj3::Obj3List new_proto;
  new_proto.ParseFromString(proto_string);
  Obj3List *parsed_olist = new Obj3List (new_proto);

  assert( parsed_olist->get_msg_type() == 1 );
  //assert( parsed_olist->get_error_code() == 110 );
  //assert( parsed_olist->get_error_message() == "Test" );
  assert( parsed_olist->get_transaction_id() == "123456789" );
  assert( parsed_olist->get_num_records() == 2 );
  assert( parsed_olist->num_objects() == 2 );

  assert ( parsed_olist->get_object(0)->get_key() == "abcdef" );
  assert ( parsed_olist->get_object(0)->get_name() == "abcdefgh" );
  assert ( parsed_olist->get_object(0)->get_scene() == "abcdefghij" );
  assert ( parsed_olist->get_object(0)->get_type() == "abcdefghijkl" );
  assert ( parsed_olist->get_object(0)->get_subtype() == "abcdefghijklmn" );
  assert ( parsed_olist->get_object(0)->get_owner() == "abcdefghijklmnop" );

  assert ( parsed_olist->get_object(1)->get_key() == "abcdefg" );
  assert ( parsed_olist->get_object(1)->get_name() == "abcdefghi" );
  assert ( parsed_olist->get_object(1)->get_scene() == "abcdefghijk" );
  assert ( parsed_olist->get_object(1)->get_type() == "abcdefghijklm" );
  assert ( parsed_olist->get_object(1)->get_subtype() == "abcdefghijklmno" );
  assert ( parsed_olist->get_object(1)->get_owner() == "abcdefghijklmnopq" );

  assert ( parsed_olist->get_object(0)->get_translation()->get_w() - 0.0 < 0.001 );
  assert ( parsed_olist->get_object(0)->get_translation()->get_x() - 1.0 < 0.001 );
  assert ( parsed_olist->get_object(0)->get_translation()->get_y() - 2.0 < 0.001 );
  assert ( parsed_olist->get_object(0)->get_translation()->get_z() - 3.0 < 0.001 );
  assert ( parsed_olist->get_object(1)->get_erotation()->get_w() - 0.0 < 0.001 );
  assert ( parsed_olist->get_object(1)->get_erotation()->get_x() - PI < 0.001 );
  assert ( parsed_olist->get_object(1)->get_erotation()->get_y() - (1.5 * PI) < 0.001 );
  assert ( parsed_olist->get_object(1)->get_erotation()->get_z() - (0.02 * PI) < 0.001 );

  assert ( parsed_olist->get_object(0)->get_qrotation()->get_w() - 0.0 < 0.001 );
  assert ( parsed_olist->get_object(0)->get_qrotation()->get_x() - sqrt(3.0)/3.0 < 0.001 );
  assert ( parsed_olist->get_object(0)->get_qrotation()->get_y() - sqrt(3.0)/3.0 < 0.001 );
  assert ( parsed_olist->get_object(0)->get_qrotation()->get_z() - sqrt(3.0)/3.0 < 0.001 );
  assert ( parsed_olist->get_object(1)->get_scale()->get_w() - 0.0 < 0.001 );
  assert ( parsed_olist->get_object(1)->get_scale()->get_w() - 2.0 < 0.001 );
  assert ( parsed_olist->get_object(1)->get_scale()->get_w() - 4.0 < 0.001 );
  assert ( parsed_olist->get_object(1)->get_scale()->get_w() - 8.0 < 0.001 );


  //JSON Tests
  main_logging->debug("JSON Tests");
  rapidjson::Document d;

  std::string json_string = olist->to_json();
  const char * json_cstr = json_string.c_str();
  d.Parse(json_cstr);

  Obj3List *jparsed_olist = new Obj3List (d);

  assert( jparsed_olist->get_msg_type() == 1 );
  //assert( jparsed_olist->get_error_code() == 110 );
  //assert( jparsed_olist->get_error_message() == "Test" );
  assert( jparsed_olist->get_transaction_id() == "123456789" );
  assert( jparsed_olist->get_num_records() == 2 );
  assert( jparsed_olist->num_objects() == 2 );

  assert ( jparsed_olist->get_object(0)->get_key() == "abcdef" );
  assert ( jparsed_olist->get_object(0)->get_name() == "abcdefgh" );
  assert ( jparsed_olist->get_object(0)->get_scene() == "abcdefghij" );
  assert ( jparsed_olist->get_object(0)->get_type() == "abcdefghijkl" );
  assert ( jparsed_olist->get_object(0)->get_subtype() == "abcdefghijklmn" );
  assert ( jparsed_olist->get_object(0)->get_owner() == "abcdefghijklmnop" );

  assert ( jparsed_olist->get_object(1)->get_key() == "abcdefg" );
  assert ( jparsed_olist->get_object(1)->get_name() == "abcdefghi" );
  assert ( jparsed_olist->get_object(1)->get_scene() == "abcdefghijk" );
  assert ( jparsed_olist->get_object(1)->get_type() == "abcdefghijklm" );
  assert ( jparsed_olist->get_object(1)->get_subtype() == "abcdefghijklmno" );
  assert ( jparsed_olist->get_object(1)->get_owner() == "abcdefghijklmnopq" );

  assert ( jparsed_olist->get_object(0)->get_translation()->get_w() - 0.0 < 0.001 );
  assert ( jparsed_olist->get_object(0)->get_translation()->get_x() - 1.0 < 0.001 );
  assert ( jparsed_olist->get_object(0)->get_translation()->get_y() - 2.0 < 0.001 );
  assert ( jparsed_olist->get_object(0)->get_translation()->get_z() - 3.0 < 0.001 );
  assert ( jparsed_olist->get_object(1)->get_erotation()->get_w() - 0.0 < 0.001 );
  assert ( jparsed_olist->get_object(1)->get_erotation()->get_x() - PI < 0.001 );
  assert ( jparsed_olist->get_object(1)->get_erotation()->get_y() - (1.5 * PI) < 0.001 );
  assert ( jparsed_olist->get_object(1)->get_erotation()->get_z() - (0.02 * PI) < 0.001 );

  assert ( jparsed_olist->get_object(0)->get_qrotation()->get_w() - 0.0 < 0.001 );
  assert ( jparsed_olist->get_object(0)->get_qrotation()->get_x() - sqrt(3.0)/3.0 < 0.001 );
  assert ( jparsed_olist->get_object(0)->get_qrotation()->get_y() - sqrt(3.0)/3.0 < 0.001 );
  assert ( jparsed_olist->get_object(0)->get_qrotation()->get_z() - sqrt(3.0)/3.0 < 0.001 );
  assert ( jparsed_olist->get_object(1)->get_scale()->get_w() - 0.0 < 0.001 );
  assert ( jparsed_olist->get_object(1)->get_scale()->get_w() - 2.0 < 0.001 );
  assert ( jparsed_olist->get_object(1)->get_scale()->get_w() - 4.0 < 0.001 );
  assert ( jparsed_olist->get_object(1)->get_scale()->get_w() - 8.0 < 0.001 );

  //Teardown
  delete olist;
  delete parsed_olist;
  delete jparsed_olist;
  delete trans;
  delete erot;
  delete qrot;
  delete scl;

  shutdown_logging_submodules();

  delete logging;
  delete logging_factory;
}
