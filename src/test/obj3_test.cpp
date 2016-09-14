//3-Space Object tests

#include <iostream>
#include "../obj3.h"
#include <string>
#include <math.h>
#include "aossl/factory/logging_interface.h"
#include "aossl/factory.h"

#include <assert.h>

using namespace Eigen;

//----------------------------------------------------------------------------//
//----------------------------Utility Methods---------------------------------//
//----------------------------------------------------------------------------//

void print_obj_attributes(Obj3& obj)
{
std::cout << obj.get_owner() << std::endl;
std::cout << obj.get_name() << std::endl;
std::cout << obj.get_key() << std::endl;
std::cout << obj.get_type() << std::endl;
std::cout << obj.get_subtype() << std::endl;

std::cout << "Location:" << std::endl;
std::cout << obj.get_loc() << std::endl;
std::cout << "Rotation Euler:" << std::endl;
std::cout << obj.get_rote() << std::endl;
std::cout << "Rotation Quaternion:" << std::endl;
std::cout << obj.get_rotq() << std::endl;
std::cout << "Scale:" << std::endl;
std::cout << obj.get_scl() << std::endl;
std::cout << obj.get_transform() << std::endl;
std::cout << obj.get_bounding_box() << std::endl;

std::cout << "Object in JSON Format" << std::endl;
std::cout << obj.to_json() << std::endl;

std::cout << "Object in JSON Message" << std::endl;
std::cout << obj.to_json_msg(0) << std::endl;
}

//----------------------------------------------------------------------------//
//-----------------------------Main Method------------------------------------//
//----------------------------------------------------------------------------//

int main()
{

ServiceComponentFactory *factory = new ServiceComponentFactory;

//-------------------------------Logging--------------------------------------//
//----------------------------------------------------------------------------//

std::string initFileName = "src/test/log4cpp_test.properties";
logging = factory->get_logging_interface(initFileName);

//----------------------------Basic Tests-------------------------------------//
//----------------------------------------------------------------------------//

std::string name;
std::string key;
std::string type;
std::string subtype;
std::string owner;

name = "Test Object";
key = "abcdef-9876543";
type = "Mesh";
subtype = "Cube";
owner = "zxywvut-1234567";

//Set up some objects
Obj3 obj2;
Obj3 obj3 (name, key);
Obj3 obj4 (name, key, type, subtype);

//Set up an object with some base matrices
Eigen::Vector3d new_location=Eigen::Vector3d::Zero(3);
Eigen::Vector3d new_rotatione=Eigen::Vector3d::Zero(3);
Eigen::Vector4d new_rotationq=Eigen::Vector4d::Zero(4);
Eigen::Vector3d new_scale=Eigen::Vector3d::Zero(3);
Eigen::Matrix4d new_transform=Eigen::Matrix4d::Zero(4, 4);
Eigen::MatrixXd new_bounding_box=Eigen::MatrixXd::Zero(4, 8);

//scale
new_scale(0) = 1.0;
new_scale(1) = 1.0;
new_scale(2) = 1.0;

//Transform and buffer
new_transform(0, 0) = 1.0;
new_transform(1, 1) = 1.0;
new_transform(2, 2) = 1.0;
new_transform(3, 3) = 1.0;

//Bounding Box
//Vertex 1
new_bounding_box(0, 0) = 1.0;
new_bounding_box(1, 0) = 1.0;
new_bounding_box(2, 0) = 1.0;
new_bounding_box(3, 0) = 1.0;

//Vertex 2
new_bounding_box(0, 1) = -1.0;
new_bounding_box(1, 1) = 1.0;
new_bounding_box(2, 1) = 1.0;
new_bounding_box(3, 1) = 1.0;

//Vertex 3
new_bounding_box(0, 2) = 1.0;
new_bounding_box(1, 2) = -1.0;
new_bounding_box(2, 2) = 1.0;
new_bounding_box(3, 2) = 1.0;

//Vertex 4
new_bounding_box(0, 3) = -1.0;
new_bounding_box(1, 3) = -1.0;
new_bounding_box(2, 3) = 1.0;
new_bounding_box(3, 3) = 1.0;

//Vertex 5
new_bounding_box(0, 4) = 1.0;
new_bounding_box(1, 4) = 1.0;
new_bounding_box(2, 4) = -1.0;
new_bounding_box(3, 4) = 1.0;

//Vertex 6
new_bounding_box(0, 5) = -1.0;
new_bounding_box(1, 5) = 1.0;
new_bounding_box(2, 5) = -1.0;
new_bounding_box(3, 5) = 1.0;

//Vertex 7
new_bounding_box(0, 6) = 1.0;
new_bounding_box(1, 6) = -1.0;
new_bounding_box(2, 6) = -1.0;
new_bounding_box(3, 6) = 1.0;

//Vertex 8
new_bounding_box(0, 7) = -1.0;
new_bounding_box(1, 7) = -1.0;
new_bounding_box(2, 7) = -1.0;
new_bounding_box(3, 7) = 1.0;

//Set up a scenes vector
std::vector<std::string> scns;
scns.push_back("12345");

Obj3 obj5 (name, key, type, subtype, owner, scns, new_location, new_rotatione, new_rotationq, new_scale, new_transform, new_bounding_box);

//Test the getters and initializers
assert (obj2.get_name() == "");
assert (obj2.get_key() == "");
assert (obj2.get_type() == "");
assert (obj2.get_subtype() == "");
assert (obj2.get_owner() == "");

assert (obj3.get_name() == "Test Object");
assert (obj3.get_key() == "abcdef-9876543");
assert (obj3.get_type() == "");
assert (obj3.get_subtype() == "");
assert (obj3.get_owner() == "");

assert (obj4.get_name() == "Test Object");
assert (obj4.get_key() == "abcdef-9876543");
assert (obj4.get_type() == "Mesh");
assert (obj4.get_subtype() == "Cube");
assert (obj4.get_owner() == "");

assert (obj5.get_name() == "Test Object");
assert (obj5.get_key() == "abcdef-9876543");
assert (obj5.get_type() == "Mesh");
assert (obj5.get_subtype() == "Cube");
assert (obj5.get_owner() == "zxywvut-1234567");

assert (obj5.get_locx() == 0.0);
assert (obj5.get_locy() == 0.0);
assert (obj5.get_locz() == 0.0);

assert (obj5.get_rotex() == 0.0);
assert (obj5.get_rotey() == 0.0);
assert (obj5.get_rotez() == 0.0);

assert (obj5.get_rotqw() == 0.0);
assert (obj5.get_rotqx() == 0.0);
assert (obj5.get_rotqy() == 0.0);
assert (obj5.get_rotqz() == 0.0);

assert (obj5.get_sclx() == 1.0);
assert (obj5.get_scly() == 1.0);
assert (obj5.get_sclz() == 1.0);

Eigen::Matrix4d tr = obj5.get_transform();
assert( tr(0, 0) == 1.0 );
assert( tr(1, 1) == 1.0 );
assert( tr(2, 2) == 1.0 );
assert( tr(3, 3) == 1.0 );

assert( tr(0, 1) == 0.0 );
assert( tr(1, 0) == 0.0 );
assert( tr(2, 0) == 0.0 );
assert( tr(0, 2) == 0.0 );

std::cout << "Object 5" << std::endl;
print_obj_attributes(obj5);

//Apply some transforms to the objects
obj5.translate(1.0, 1.0, 1.0);
std::cout << "Object 5 after transform of 1 on x, y, and z axis:" << std::endl;
print_obj_attributes(obj5);

assert (obj5.get_locx() == 1.0);
assert (obj5.get_locy() == 1.0);
assert (obj5.get_locz() == 1.0);

Eigen::Matrix4d tr2 = obj5.get_transform();
assert( tr2(0, 0) == 1.0 );
assert( tr2(1, 1) == 1.0 );
assert( tr2(2, 2) == 1.0 );
assert( tr2(3, 3) == 1.0 );

assert( tr2(0, 3) == 1.0 );
assert( tr2(1, 3) == 1.0 );
assert( tr2(2, 3) == 1.0 );
assert( tr2(3, 3) == 1.0 );

obj5.resize(2.0, 2.0, 2.0);
std::cout << "Object 5 after scale of 2 on x, y, and z axis:" << std::endl;
print_obj_attributes(obj5);

obj5.rotate(45.0, 0.0, 0.0);
std::cout << "Object 5 after rotation of 45 degrees about x axis:" << std::endl;
print_obj_attributes(obj5);

obj5.rotate(0.0, 0.0, 45.0);
std::cout << "Object 5 after rotation of 45 degrees about y axis:" << std::endl;
print_obj_attributes(obj5);

obj5.rotate(0.0, 45.0, 0.0);
std::cout << "Object 5 after rotation of 45 degrees about z axis:" << std::endl;
print_obj_attributes(obj5);

obj5.rotate( (sqrt (3.0) / 3.0), (sqrt (3.0) / 3.0), (sqrt (3.0) / 3.0), 45.0);
std::cout << "Object 5 after rotation of 45 degrees about unit vector" << std::endl;
print_obj_attributes(obj5);

//TO-DO: Asserts to check the transformations

//------------------------JSON & Protocol Buffer Tests------------------------//
//----------------------------------------------------------------------------//

//Let's translate object 5 into a Proto Buf String and a JSON String
std::string proto_string = obj5.to_protobuf_msg(0);
std::string json_string = obj5.to_json_msg(0);
std::string json_str = obj5.to_json();

Eigen::Matrix4d tr3 = obj5.get_transform();

//Now, we translate each of these back to a form we can pass to another constructor
rapidjson::Document d;
d.Parse(json_string.c_str());

rapidjson::Document d2;
d2.Parse(json_str.c_str());

protoObj3::Obj3 new_proto;
new_proto.ParseFromString(proto_string);

//Build new objects from the converted data
Obj3 obj6 (d);
Obj3 obj7 (d2);
Obj3 obj8 (new_proto);

// assert (obj6.get_name() == "Test Object");
// assert (obj6.get_key() == "abcdef-9876543");
// assert (obj6.get_type() == "Mesh");
// assert (obj6.get_subtype() == "Cube");
// assert (obj6.get_owner() == "zxywvut-1234567");
//
// assert (obj7.get_name() == "Test Object");
// assert (obj7.get_key() == "abcdef-9876543");
// assert (obj7.get_type() == "Mesh");
// assert (obj7.get_subtype() == "Cube");
// assert (obj7.get_owner() == "zxywvut-1234567");
//
// assert (obj8.get_name() == "Test Object");
// assert (obj8.get_key() == "abcdef-9876543");
// assert (obj8.get_type() == "Mesh");
// assert (obj8.get_subtype() == "Cube");
// assert (obj8.get_owner() == "zxywvut-1234567");

//TO-DO: Asserts to check the matrix attributes

std::cout << "Objects Translated From Obj5" << std::endl;

std::cout << "From JSON Message" << std::endl;
print_obj_attributes(obj6);
std::cout << "From JSON Document" << std::endl;
print_obj_attributes(obj7);
std::cout << "From Protobuffer" << std::endl;
print_obj_attributes(obj8);

delete logging;
delete factory;

return 0;

}
