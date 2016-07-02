//3-Space Object tests

#include <iostream>
#include "obj3.h"
#include <string>
#include <math.h>
#include "logging.h"

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

std::cout << obj.get_loc() << std::endl;
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

//-------------------------------Logging--------------------------------------//
//----------------------------------------------------------------------------//

std::string initFileName = "log4cpp.properties";
try {
        log4cpp::PropertyConfigurator::configure(initFileName);
}
catch ( log4cpp::ConfigureFailure &e ) {
        std::cout << "[log4cpp::ConfigureFailure] caught while reading" << initFileName << std::endl;
        std::cout << e.what();
        exit(1);
}

log4cpp::Category& root = log4cpp::Category::getRoot();

log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("sub1"));

log4cpp::Category& log = log4cpp::Category::getInstance(std::string("sub1.log"));

logging = &log;

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
new_transform(0, 3) = 1.0;
new_transform(1, 1) = 1.0;
new_transform(1, 3) = 1.0;
new_transform(2, 2) = 1.0;
new_transform(2, 3) = 1.0;
new_transform(3, 3) = 1.0;

//Bounding Box
new_bounding_box(0, 1) = 1.0;
new_bounding_box(1, 2) = 1.0;
new_bounding_box(0, 3) = 1.0;
new_bounding_box(1, 3) = 1.0;
new_bounding_box(2, 4) = 1.0;
new_bounding_box(0, 5) = 1.0;
new_bounding_box(2, 5) = 1.0;
new_bounding_box(1, 6) = 1.0;
new_bounding_box(2, 6) = 1.0;
new_bounding_box(0, 7) = 1.0;
new_bounding_box(1, 7) = 1.0;
new_bounding_box(2, 7) = 1.0;

//Set up a scenes vector
std::vector<std::string> scns;
scns.push_back("12345");

Obj3 obj5 (name, key, type, subtype, owner, scns, new_location, new_rotatione, new_rotationq, new_scale, new_transform, new_bounding_box);

//Print out the object attributes to test the getters and initializers

std::cout << "Object 2" << std::endl;
print_obj_attributes(obj2);

std::cout << "Object 3" << std::endl;
print_obj_attributes(obj3);

std::cout << "Object 4" << std::endl;
print_obj_attributes(obj4);

std::cout << "Object 5" << std::endl;
print_obj_attributes(obj5);

//Apply some transforms to the objects
obj5.translate(1.0, 1.0, 1.0, "Global");
obj5.apply_transforms();
std::cout << "Object 5 after transform of 1 on x, y, and z axis:" << std::endl;
print_obj_attributes(obj5);

obj5.rotatee(45.0, 45.0, 45.0, "Global");
obj5.apply_transforms();
std::cout << "Object 5 after euler rotation of 45 degrees about x, y, and z axis:" << std::endl;
print_obj_attributes(obj5);

obj5.rotateq( (sqrt (3.0) / 3.0), (sqrt (3.0) / 3.0), (sqrt (3.0) / 3.0), 45.0, "Global");
obj5.apply_transforms();
std::cout << "Object 5 after quaternion rotation of 45 degrees about vector <1,1,1>:" << std::endl;
print_obj_attributes(obj5);

obj5.resize(2.0, 2.0, 2.0);
obj5.apply_transforms();
std::cout << "Object 5 after scale of 2 on x, y, and z axis:" << std::endl;
print_obj_attributes(obj5);

//------------------------JSON & Protocol Buffer Tests------------------------//
//----------------------------------------------------------------------------//

//Let's translate object 5 into a Proto Buf String and a JSON String
std::string proto_string = obj5.to_protobuf_msg(0);
std::string json_string = obj5.to_json_msg(0);
std::string json_str = obj5.to_json();

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

std::cout << "Objects Translated From Obj5" << std::endl;

std::cout << "From JSON Message" << std::endl;
print_obj_attributes(obj6);
std::cout << "From JSON Document" << std::endl;
print_obj_attributes(obj7);
std::cout << "From Protobuffer" << std::endl;
print_obj_attributes(obj8);

return 0;

}
