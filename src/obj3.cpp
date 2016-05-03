//This is the Obj3 Cpp file
//This contains matrix and trig
//logic used in the Obj3 class

#include "obj3.h"
#include <math.h>

#define PI 3.14159265358979323846

using namespace Eigen;
using namespace rapidjson;

bool Obj3::transform_object(Matrix4d trans_matrix)
{
	logging->info("Obj3:Transform Object Called with Matrix4d");
	Matrix4d result_matrix;
	result_matrix = trans_matrix * transform_buffer;
	transform_buffer = result_matrix;
	return true;
}

bool Obj3::transform_object(double trans_matrix[])
{
	logging->info("Obj3:Transform Object called with double[]");
	Matrix4d tran_matrix;
	int i, j;
	for (i=0;i<4;i=i+1)
	{
		for (j=0;j<4;j=j+1)
		{
			tran_matrix(i, j) = trans_matrix[(4 * i) + j];
		}
	}

	Matrix4d result_matrix;
        result_matrix = tran_matrix * transform_buffer;
	transform_buffer = result_matrix;
	return true;
}

void Obj3::translate_object(double x, double y, double z, std::string locality)
{
	logging->info("Obj3:Translate Object called");
	//Variable Declarations
	Matrix4d tran_matrix;
	Matrix4d result_matrix;

	//Set up the transformation matrix
	tran_matrix = Matrix4d::Zero(4, 4);
	tran_matrix(0, 0) = 1.0;
	tran_matrix(1, 1) = 1.0;
	tran_matrix(2, 2) = 1.0;
	tran_matrix(3, 3) = 1.0;

	tran_matrix(0, 3) = x;
	tran_matrix(1, 3) = y;
	tran_matrix(2, 3) = z;

	double r_x;
        double r_y;
        double r_z;

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

}

void Obj3::rotateq_object(double x, double y, double z, double theta, std::string locality)
{
logging->info("Obj3:RotateQ Object Called");
Matrix4d tran_matrix;
Matrix4d result_matrix;

//Set up the transformation matrix
tran_matrix = Matrix4d::Zero(4, 4);
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

}

void Obj3::rotatee_object(double x, double y, double z, std::string locality)
{
logging->info("Obj3:RotateE Object Called");
//Variable Declarations
Matrix4d xtran_matrix;
Matrix4d ytran_matrix;
Matrix4d ztran_matrix;
Matrix4d tran_matrix;
Matrix4d result_matrix;

//Set up the transformation matrix
xtran_matrix = Matrix4d::Zero(4, 4);
xtran_matrix(1, 1) = cos (x * (PI/180));
xtran_matrix(2, 1) = sin (x * (PI/180));
xtran_matrix(1, 2) = sin (x * (PI/180)) * -1.0;
xtran_matrix(2, 2) = cos (x * (PI/180));
xtran_matrix(0, 0) = 1.0;
xtran_matrix(3, 3) = 1.0;

ytran_matrix = Matrix4d::Zero(4, 4);
ytran_matrix(0, 0) = cos (y * (PI/180));
ytran_matrix(0, 3) = sin (y * (PI/180));
ytran_matrix(3, 0) = sin (y * (PI/180)) * -1.0;
ytran_matrix(2, 2) = cos (y * (PI/180));
ytran_matrix(1, 1) = 1.0;
ytran_matrix(3, 3) = 1.0;

ztran_matrix = Matrix4d::Zero(4, 4);
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

}

void Obj3::scale_object(double x, double y, double z)
{
	logging->info("Obj3:Scale Object Called");
	//Variable Declarations
        Matrix4d tran_matrix;
        Matrix4d result_matrix;

        //Set up the transformation matrix
        tran_matrix = Matrix4d::Zero(4, 4);
        tran_matrix(0, 0) = x;
        tran_matrix(1, 1) = y;
        tran_matrix(2, 2) = z;
        tran_matrix(3, 3) = 1.0;

        //Perform the matrix multiplication
        result_matrix = tran_matrix * transform_buffer;

        transform_buffer = result_matrix;

}

void Obj3::initialize_buffers()
{
	logging->info("Obj3:Initialize Buffers Called");
	transform_buffer = Matrix4d::Zero(4, 4);
	transform_buffer(0, 0) = 1.0;
        transform_buffer(1, 1) = 1.0;
        transform_buffer(2, 2) = 1.0;
        transform_buffer(3, 3) = 1.0;
}

void Obj3::initialize_matrices()
{
	logging->info("Obj3:Intialize Matrices Called");
	//Set initial values with function calls
	bounding_box = MatrixXd::Zero(4, 8);
	location = Vector3d::Zero(3);
	rotation_euler = Vector3d::Zero(3);
	rotation_quaternion = Vector4d::Zero(4);
	scaling = Vector3d::Zero(3);
	transform_matrix = Matrix4d::Zero(4, 4);

	initialize_buffers();
}

void Obj3::apply_transforms()
{
	logging->info("Obj3:Apply Transforms Called");

	//Update the transformation matrix
	Matrix4d result;
	result = transform_buffer * transform_matrix;
	transform_matrix = result;

	//Update the location
	Vector4d loc4;
	loc4 = Vector4d::Constant(4, 1.0);
	loc4(0) = location(0);
	loc4(1) = location(1);
	loc4(2) = location(2);

	Vector4d res_loc;
	res_loc = transform_buffer * loc4;

	//Perform the necessary transforms on the bounding box
	MatrixXd res_bb;
	res_bb = MatrixXd::Zero(4, 8);
	res_bb = transform_buffer * bounding_box;
	bounding_box = res_bb;

	//Reset the buffers
	initialize_buffers();
}

std::string Obj3::to_json() const
{
        logging->info("Obj3:To JSON Called on object");
        logging->info(get_key());
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

	writer.Key("owner");
        std::string owner_dev = get_owner();
        writer.String( owner_dev.c_str(), (SizeType)owner_dev.length() );

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
                for (j=0; j<4; j++) {
                        writer.Double( static_cast<double>(transform_matrix(i, j) ));
                }
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
		writer.Bool(is_locked);

        writer.EndObject();

        //The Stringbuffer now contains a json message
        //of the object
		const char* ret_val = s.GetString();
		std::string ret_string (ret_val);
	logging->debug("JSON Returned:");
	logging->debug(ret_val);
        return ret_string;
}

std::string Obj3::to_json_msg(int msg_type) const
{
        logging->info("Obj3:To JSON message Called on object");
        logging->info(get_key());
        //Initialize the string buffer and writer
        StringBuffer s;
        Writer<StringBuffer> writer(s);

        //Start writing the object
        //Syntax taken directly from
        //simplewriter.cpp in rapidjson examples

        writer.StartObject();

        writer.Key("message_type");
        writer.Uint(msg_type);

        writer.Key("key");
        std::string key = get_key();
        writer.String( key.c_str(), (SizeType)key.length() );

	writer.Key("owner");
        std::string owner_dev = get_owner();
        writer.String( owner_dev.c_str(), (SizeType)owner_dev.length() );

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
                for (j=0; j<4; j++) {
                        writer.Double( static_cast<double>(transform_matrix(i, j) ));
                }
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
		writer.Bool(is_locked);

        writer.EndObject();

        //The Stringbuffer now contains a json message
        //of the object
		const char* ret_val = s.GetString();
		std::string ret_string (ret_val);
        return ret_string;
}
