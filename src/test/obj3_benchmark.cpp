//Obj3 Benchmarks

#include <hayai/hayai.hpp>
#include <iostream>
#include "../obj3.h"
#include <string>
#include <math.h>
#include "aossl/factory/logging_interface.h"
#include "aossl/factory.h"

Obj3 *obj1;

std::string json_string;
std::string pb_string;

//----------------------------------------------------------------------------//
//------------------------------Benchmarks------------------------------------//
//----------------------------------------------------------------------------//

BENCHMARK(Access, Key, 10, 100)
{

std::string key = obj1->get_key();

}

BENCHMARK(Access, Name, 10, 100)
{

std::string name = obj1->get_name();

}

BENCHMARK(Access, Type, 10, 100)
{

std::string type = obj1->get_type();

}

BENCHMARK(Access, Subtype, 10, 100)
{

std::string subtype = obj1->get_subtype();

}

BENCHMARK(Access, Owner, 10, 100)
{

std::string owner = obj1->get_owner();

}

BENCHMARK(Transform, Translate, 5, 50)
{

obj1->translate(1.0, 1.0, 1.0);

}

BENCHMARK(Transform, RotateEuler, 5, 50)
{

obj1->rotatee(45.0, 45.0, 45.0);

}

BENCHMARK(Transform, RotateQuaternion, 5, 50)
{

obj1->rotateq( (sqrt (3.0) / 3.0), (sqrt (3.0) / 3.0), (sqrt (3.0) / 3.0), 45.0);

}

BENCHMARK(Transform, Scale, 5, 50)
{

obj1->resize(2.0, 2.0, 2.0);

}

BENCHMARK(ToMessage, JSONDocument, 5, 50)
{

std::string json = obj1->to_json();

}

BENCHMARK(ToMessage, JSONMessage, 5, 50)
{

std::string json = obj1->to_json_msg(0);

}

BENCHMARK(ToMessage, ProtoBufMessage, 5, 50)
{

std::string pb = obj1->to_protobuf_msg(0);

}

BENCHMARK(FromMessage, JSONMessage, 5, 50)
{

rapidjson::Document d2;
d2.Parse(json_string.c_str());

Obj3 obj7 (d2);

}

BENCHMARK(FromMessage, ProtoBufferMessage, 5, 50)
{

protoObj3::Obj3 new_proto;
new_proto.ParseFromString(pb_string);

Obj3 obj8 (new_proto);

}

//----------------------------------------------------------------------------//
//------------------------------Main Method-----------------------------------//
//----------------------------------------------------------------------------//

int main()
{

ServiceComponentFactory *factory = new ServiceComponentFactory;

//-------------------------------Logging--------------------------------------//
//----------------------------------------------------------------------------//

std::string initFileName = "src/test/log4cpp_test.properties";
logging = factory->get_logging_interface(initFileName);

//---------------------------Pre-Test Setup-----------------------------------//
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

obj1 = new Obj3 (name, key, type, subtype, owner, scns, new_location, new_rotatione, new_rotationq, new_scale, new_transform, new_bounding_box);

json_string = obj1->to_json_msg(0);
pb_string = obj1->to_protobuf_msg(0);

//------------------------------Run Tests-------------------------------------//
//----------------------------------------------------------------------------//

hayai::ConsoleOutputter consoleOutputter;

hayai::Benchmarker::AddOutputter(consoleOutputter);
hayai::Benchmarker::RunAllTests();

//-------------------------Post-Test Teardown---------------------------------//
//----------------------------------------------------------------------------//

delete obj1;
delete logging;
delete factory;

return 0;
}
