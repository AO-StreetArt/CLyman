//This is the Obj3 Cpp file
//This contains matrix and trig
//logic used in the Obj3 class

#include "obj3.h"
#include <math.h>

#define PI 3.14159265358979323846

using namespace Eigen;
using namespace rapidjson;

bool Obj3::transform_object(Matrix4f trans_matrix)
{
	logging->info("Obj3:Transform Object Called with Matrix4f");
	Matrix4f result_matrix;
	result_matrix = trans_matrix * transform_buffer;
	transform_buffer = result_matrix;
	return true;
}

bool Obj3::transform_object(float trans_matrix[])
{
	logging->info("Obj3:Transform Object called with float[]");
	Matrix4f tran_matrix;
	int i, j;
	for (i=0;i<4;i=i+1)
	{
		for (j=0;j<4;j=j+1)
		{
			tran_matrix(i, j) = trans_matrix[(4 * i) + j];
		}
	}

	Matrix4f result_matrix;
        result_matrix = tran_matrix * transform_buffer;
	transform_buffer = result_matrix;
	return true;
}

void Obj3::translate_object(float x, float y, float z, std::string locality)
{
	logging->info("Obj3:Translate Object called");
	//Variable Declarations
	Matrix4f tran_matrix;
	Matrix4f result_matrix;

	//Set up the transformation matrix
	tran_matrix = Matrix4f::Zero(4, 4);
	tran_matrix(0, 0) = 1.0;
	tran_matrix(1, 1) = 1.0;
	tran_matrix(2, 2) = 1.0;
	tran_matrix(3, 3) = 1.0;

	tran_matrix(0, 3) = x;
	tran_matrix(1, 3) = y;
	tran_matrix(2, 3) = z;

	float r_x;
        float r_y;
        float r_z;

	//Perform the matrix multiplication
	if (locality=="Global")
	{
	result_matrix = tran_matrix * transform_buffer;
	}
	else if (locality=="Local")
	{
	result_matrix = transform_buffer * tran_matrix;
	}

	transform_buffer = result_matrix;

	//Update the Translation buffer
	r_x = location_buffer(0) + x;
	r_y = location_buffer(1) + y;
	r_z = location_buffer(2) + z;
	location_buffer(0) = r_x;
	location_buffer(1) = r_y;
	location_buffer(2) = r_z;
}

void Obj3::rotateq_object(float x, float y, float z, float theta, std::string locality)
{
logging->info("Obj3:RotateQ Object Called");
Matrix4f tran_matrix;
Matrix4f result_matrix;

//Set up the transformation matrix
tran_matrix = Matrix4f::Zero(4, 4);
tran_matrix(0, 0) = cos (theta*(PI/180)) + (x*x) * (1-cos(theta*(PI/180)));
tran_matrix(1, 1) = cos (theta*(PI/180)) + (y*y) * (1-cos(theta*(PI/180)));
tran_matrix(2, 2) = cos (theta*(PI/180)) + (z*z) * (1-cos(theta*(PI/180)));
tran_matrix(3, 3) = 1.0;

tran_matrix(0, 2) = z*x*(1-cos (theta*(PI/180)) - y*sin (theta*(PI/180)));
tran_matrix(1, 2) = z*y*(1-cos (theta*(PI/180)) - x*sin (theta*(PI/180)));
tran_matrix(0, 1) = y*x*(1-cos (theta*(PI/180)) - z*sin (theta*(PI/180)));
tran_matrix(2, 1) = y*z*(1-cos (theta*(PI/180)) - x*sin (theta*(PI/180)));
tran_matrix(1, 0) = x*y*(1-cos (theta*(PI/180)) - z*sin (theta*(PI/180)));
tran_matrix(2, 0) = x*z*(1-cos (theta*(PI/180)) - y*sin (theta*(PI/180)));

//Perform the matrix multiplication
if (locality == "Global")
{
        result_matrix = tran_matrix * transform_buffer;
}
else if (locality=="Local")
{
	result_matrix = transform_buffer * tran_matrix;
}

transform_buffer = result_matrix;

//Update the Quaternion Rotation buffer
//Here we need to multiply quaternions, not just add
//See http://www.gamedev.net/topic/526024-quaternion-angles-incorrect/?whichpage=1%25EF%25BF%25BD
//for full explanation
Vector4f quat;
Vector4f result;
quat = Vector4f::Zero(4);
quat(0) = sin ( (theta/2) * (PI/180) ) * x;
quat(1) = sin ( (theta/2) * (PI/180) ) * y;
quat(2) = sin ( (theta/2) * (PI/180) ) * z;
quat(3) = cos ( (theta/2) * (PI/180) );

//Perform the quaternion multiplication
result = Vector4f::Zero(4);
result(0)=( quat(0) * rotationq_buffer(0) ) - ( quat(1) * rotationq_buffer(1) ) - ( quat(2) * rotationq_buffer(2) ) - ( quat(3) * rotationq_buffer(3) );
result(1)=( quat(0) * rotationq_buffer(0) ) + ( quat(1) * rotationq_buffer(1) ) - ( quat(2) * rotationq_buffer(2) ) + ( quat(3) * rotationq_buffer(3) );
result(2)=( quat(0) * rotationq_buffer(0) ) + ( quat(1) * rotationq_buffer(1) ) + ( quat(2) * rotationq_buffer(2) ) - ( quat(3) * rotationq_buffer(3) );
result(3)=( quat(0) * rotationq_buffer(0) ) - ( quat(1) * rotationq_buffer(1) ) + ( quat(2) * rotationq_buffer(2) ) + ( quat(3) * rotationq_buffer(3) ); 
rotationq_buffer = result;
}

void Obj3::rotatee_object(float x, float y, float z, std::string locality)
{
logging->info("Obj3:RotateE Object Called");
//Variable Declarations
Matrix4f xtran_matrix;
Matrix4f ytran_matrix;
Matrix4f ztran_matrix;
Matrix4f tran_matrix;
Matrix4f result_matrix;

//Set up the transformation matrix 
xtran_matrix = Matrix4f::Zero(4, 4);
xtran_matrix(1, 1) = cos (x * (PI/180));
xtran_matrix(2, 1) = sin (x * (PI/180));
xtran_matrix(1, 2) = sin (x * (PI/180)) * -1.0;
xtran_matrix(2, 2) = cos (x * (PI/180));
xtran_matrix(0, 0) = 1.0;
xtran_matrix(3, 3) = 1.0;

ytran_matrix = Matrix4f::Zero(4, 4);
ytran_matrix(0, 0) = cos (y * (PI/180));
ytran_matrix(0, 3) = sin (y * (PI/180));
ytran_matrix(3, 0) = sin (y * (PI/180)) * -1.0;
ytran_matrix(2, 2) = cos (y * (PI/180));
ytran_matrix(1, 1) = 1.0;
ytran_matrix(3, 3) = 1.0;

ztran_matrix = Matrix4f::Zero(4, 4);
ztran_matrix(0, 0) = cos (z * (PI/180));
ztran_matrix(1, 0) = sin (z * (PI/180));
ztran_matrix(0, 1) = sin (z * (PI/180)) * -1.0;
ztran_matrix(1, 1) = cos (z * (PI/180));
ztran_matrix(2, 2) = 1.0;
ztran_matrix(3, 3) = 1.0;

//Perform the matrix multiplication
tran_matrix = ztran_matrix * ytran_matrix * xtran_matrix;
if (locality == "Global")
{
	result_matrix = tran_matrix * transform_buffer;
}
else if (locality=="Local")
{
	result_matrix = transform_buffer * tran_matrix;
}

transform_buffer = result_matrix;

//Update the Rotation buffer
float r_x;
float r_y;
float r_z;
r_x = rotatione_buffer(0) + x;
r_y = rotatione_buffer(1) + y;
r_z = rotatione_buffer(2) + z;
rotatione_buffer(0) = r_x;
rotatione_buffer(1) = r_y;
rotatione_buffer(2) = r_z;
}

void Obj3::scale_object(float x, float y, float z)
{
	logging->info("Obj3:Scale Object Called");
	//Variable Declarations
        Matrix4f tran_matrix;
        Matrix4f result_matrix;

        //Set up the transformation matrix 
        tran_matrix = Matrix4f::Zero(4, 4);
        tran_matrix(0, 0) = x;
        tran_matrix(1, 1) = y;
        tran_matrix(2, 2) = z;
        tran_matrix(3, 3) = 1.0;

        //Perform the matrix multiplication
        result_matrix = tran_matrix * transform_buffer;

        transform_buffer = result_matrix;

        //Update the Translation buffer
        float s_x;
        float s_y;
        float s_z;
        s_x = scale_buffer(0) + x;
        s_y = scale_buffer(1) + y;
        s_z = scale_buffer(2) + z;
        scale_buffer(0) = s_x;
        scale_buffer(1) = s_y;
        scale_buffer(2) = s_z;
}

void Obj3::initialize_buffers()
{
	logging->info("Obj3:Initialize Buffers Called");
	transform_buffer = Matrix4f::Zero(4, 4);
	transform_buffer(0, 0) = 1.0;
        transform_buffer(1, 1) = 1.0;
        transform_buffer(2, 2) = 1.0;
        transform_buffer(3, 3) = 1.0;

	scale_buffer = Vector3f::Constant(3, 1.0);
        location_buffer = Vector3f::Zero(3);
        rotatione_buffer = Vector3f::Zero(3);
        rotationq_buffer = Vector4f::Zero(4);
	rotationq_buffer(3) = 1.0;
}

void Obj3::initialize_matrices()
{
	logging->info("Obj3:Intialize Matrices Called");
	//Set initial values with function calls
	bounding_box = MatrixXf::Zero(4, 8);
	scale = Vector3f::Constant(3, 1.0);
	location = Vector3f::Zero(3);
	rotation_euler = Vector3f::Zero(3);
	rotation_quaternion = Vector4f::Zero(4);
	transform_matrix = Matrix4f::Zero(4, 4);

	//Set the specific values needed after setting initial ones
	
	//Transform and buffer
	transform_matrix(0, 0) = 1.0;
	transform_matrix(1, 1) = 1.0;
	transform_matrix(2, 2) = 1.0;
	transform_matrix(3, 3) = 1.0;

	//Bounding Box
	bounding_box(0, 1) = 1.0;
	bounding_box(1, 2) = 1.0;
	bounding_box(0, 3) = 1.0;
	bounding_box(1, 3) = 1.0;
	bounding_box(2, 4) = 1.0;
	bounding_box(0, 5) = 1.0;
	bounding_box(2, 5) = 1.0;
	bounding_box(1, 6) = 1.0;
	bounding_box(2, 6) = 1.0;
	bounding_box(0, 7) = 1.0;
	bounding_box(1, 7) = 1.0;
	bounding_box(2, 7) = 1.0;

	initialize_buffers();
}

void Obj3::apply_transforms()
{
	logging->info("Obj3:Apply Transforms Called");
	//Update the transformation matrix
	Matrix4f result;
	result = transform_buffer * transform_matrix;
	transform_matrix = result;

	//Update the location
	Vector4f loc4;
	loc4 = Vector4f::Constant(4, 1.0);
	loc4(0) = location(0);
	loc4(1) = location(1);
	loc4(2) = location(2);
	
	Vector4f res_loc;
	res_loc = transform_buffer * loc4;

	//Update the Rotation, and Scaling attributes
	Vector3f res_rote;
	res_rote = Vector3f::Zero(3);
	res_rote(0) = rotation_euler(0) + rotatione_buffer(0);	
	res_rote(1) = rotation_euler(1) + rotatione_buffer(1);
	res_rote(2) = rotation_euler(2) + rotatione_buffer(2);
	rotation_euler = res_rote;

	Vector4f res_rotq;
	res_rotq = Vector4f::Zero(4);
	res_rotq(0)=( rotation_quaternion(0) * rotationq_buffer(0) ) - ( rotation_quaternion(1) * rotationq_buffer(1) ) - ( rotation_quaternion(2) * rotationq_buffer(2) ) - ( rotation_quaternion(3) * rotationq_buffer(3) );
        res_rotq(1)=( rotation_quaternion(0) * rotationq_buffer(0) ) + ( rotation_quaternion(1) * rotationq_buffer(1) ) - ( rotation_quaternion(2) * rotationq_buffer(2) ) + ( rotation_quaternion(3) * rotationq_buffer(3) );
        res_rotq(2)=( rotation_quaternion(0) * rotationq_buffer(0) ) + ( rotation_quaternion(1) * rotationq_buffer(1) ) + ( rotation_quaternion(2) * rotationq_buffer(2) ) - ( rotation_quaternion(3) * rotationq_buffer(3) );
        res_rotq(3)=( rotation_quaternion(0) * rotationq_buffer(0) ) - ( rotation_quaternion(1) * rotationq_buffer(1) ) + ( rotation_quaternion(2) * rotationq_buffer(2) ) + ( rotation_quaternion(3) * rotationq_buffer(3) );
	rotation_quaternion = res_rotq;	

	Vector3f res_scl;
	res_scl = Vector3f::Zero(3);
	res_scl(0) = scale(0) * scale_buffer(0);
	res_scl(1) = scale(1) * scale_buffer(1);
	res_scl(2) = scale(2) * scale_buffer(2);

	//Perform the necessary transforms on the bounding box
	MatrixXf res_bb;
	res_bb = MatrixXf::Zero(4, 8);
	res_bb = transform_buffer * bounding_box;
	bounding_box = res_bb;

	//Reset the buffers
	initialize_buffers();
}

const char* Obj3::to_json() const
{
	logging->info("Obj3:To JSON Called");
	//Initialize the string buffer and writer
	StringBuffer s;
	Writer<StringBuffer> writer(s);

	//Start writing the object
	//Syntax taken directly from 
	//simplewriter.cpp in rapidjson examples

	writer.StartObject();
	writer.Key("key");
	std::string key = get_key();
	writer.String( key.c_str(), (SizeType)key.length() );

	writer.Key("name");
	std::string name = get_name();
	writer.String( name.c_str(), (SizeType)name.length() );

	writer.Key("type");
	std::string type = get_type();
	writer.String( type.c_str(), (SizeType)type.length() );

	writer.Key("subtype");
	std::string subtype = get_subtype();
	writer.String( subtype.c_str(), (SizeType)subtype.length() );

	int i;
	int j;

	writer.Key("location");
	writer.StartArray();
	for (i=0; i<3; i++) {
		writer.Double( static_cast<double>(get_loc(i)) );
	}
	writer.EndArray();

	writer.Key("transform");
	writer.StartArray();
	
        for (i=0; i<4; i++) {
		writer.StartArray();
		for (j=0; j<4; j++) {
                	writer.Double( static_cast<double>(transform_matrix(i, j) ));
		}
		writer.EndArray();
        }

        writer.EndArray();

	writer.Key("scenes");
	writer.StartArray();
        for (i=0; i<num_scenes(); i++) {
		std::string sc = get_scene(i);
                writer.String( sc.c_str(), (SizeType)sc.length() );
        }
        writer.EndArray();

	writer.Key("locked");
	if (is_locked == true) {
		std::string l = "True";
		writer.String(l.c_str(), (SizeType) l.length());
	}
	else {
		std::string l = "False";
		writer.String(l.c_str(), (SizeType) l.length());
	}

	writer.EndObject();

	//The Stringbuffer now contains a json message
	//of the object
	return s.GetString();
}
