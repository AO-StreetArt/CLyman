//3-Space Object tests

#include <iostream>
#include "obj3.h"
#include <string>
#include <math.h>

using namespace Eigen;

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
}

int main()
{
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
Obj3 obj5 (name, key, type, subtype, owner);

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

return 0;

}
