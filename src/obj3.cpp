//This is the Obj3 Cpp file
//This contains matrix and trig
//logic used in the Obj3 class

#include "obj3.h"
#include <math.h>

#define PI 3.14159265358979323846

using namespace Eigen;
using namespace rapidjson;

Obj3::Obj3(protoObj3::Obj3 buffer)
{
	logging->debug("Build Proto-Object Called");
  std::string new_name="";
  std::string new_key="";
  std::string new_owner="";
  std::string new_type="";
  std::string new_subtype="";
  std::string new_lock_id="";
  Eigen::Vector3d new_location=Eigen::Vector3d::Zero(3);
  Eigen::Vector3d new_rotatione=Eigen::Vector3d::Zero(3);
  Eigen::Vector4d new_rotationq=Eigen::Vector4d::Zero(4);
  Eigen::Vector3d new_scale=Eigen::Vector3d::Zero(3);
  Eigen::Matrix4d new_transform=Eigen::Matrix4d::Zero(4, 4);
  Eigen::MatrixXd new_bounding_box=Eigen::MatrixXd::Zero(4, 8);
  std::vector<std::string> scn_list;
  logging->debug("New Variables Declared");

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

  //Perform the Conversion
  if (buffer.has_name()) {
    new_name = buffer.name();
  }
  if (buffer.has_key()) {
    new_key = buffer.key();
  }
  if (buffer.has_owner()) {
    new_owner = buffer.owner();
  }
  if (buffer.has_type()) {
    new_type = buffer.type();
  }
  if (buffer.has_subtype()) {
    new_subtype = buffer.subtype();
  }
  if (buffer.has_lock_device_id()) {
    new_lock_id = buffer.lock_device_id();
  }
  if (buffer.has_location()) {
    protoObj3::Obj3_Vertex3 loc = buffer.location();
    new_location(0) = loc.x();
    new_location(1) = loc.y();
    new_location(2) = loc.z();
  }
  if (buffer.has_rotation_euler()) {
    protoObj3::Obj3_Vertex3 rote = buffer.rotation_euler();
    new_rotatione(0) = rote.x();
    new_rotatione(1) = rote.y();
    new_rotatione(2) = rote.z();

  }
  if (buffer.has_rotation_quaternion()) {
    protoObj3::Obj3_Vertex4 rotq = buffer.rotation_quaternion();
    new_rotationq(0) = rotq.w();
    new_rotationq(1) = rotq.x();
    new_rotationq(2) = rotq.y();
    new_rotationq(3) = rotq.z();
  }
  if (buffer.has_scale()) {
    protoObj3::Obj3_Vertex3 scl = buffer.scale();
    new_scale(0) = scl.x();
    new_scale(1) = scl.y();
    new_scale(2) = scl.z();
  }
  if (buffer.has_transform()) {
    protoObj3::Obj3_Matrix4 trn = buffer.transform();
    int i = 0;
    for (i=0; i < trn.col_size(); i++) {
      protoObj3::Obj3_Vertex4 c = trn.col(i);
      new_transform(0, i) = c.w();
      new_transform(1, i) = c.x();
      new_transform(2, i) = c.y();
      new_transform(3, i) = c.z();
    }
    logging->debug("Transform Matrix Parsed");
  }
  if (buffer.has_bounding_box()) {
    protoObj3::Obj3_Matrix4 bb = buffer.transform();
    int i = 0;
    for (i=0; i < bb.col_size(); i++) {
      protoObj3::Obj3_Vertex4 c = bb.col(i);
      new_bounding_box(0, i) = c.w();
      new_bounding_box(1, i) = c.x();
      new_bounding_box(2, i) = c.y();
      new_bounding_box(3, i) = c.z();
    }
    logging->debug("Bounding Box Parsed");
  }
  if (buffer.scenes_size() > 0) {
    int j = 0;
    for (j=0; j< buffer.scenes_size(); j++) {
      scn_list.push_back(buffer.scenes(j));
    }
  }

  logging->debug("Variables Filled");

	//Set the String Attributes
	name = new_name;
	key = new_key;
	type = new_type;
	subtype = new_subtype;
	owner = new_owner;

	//Create Matrices
	initialize_matrices();

	//Lock Attributes
	if (new_lock_id == "") {
		is_locked=false;
		lock_owner="";
	}
	else {
		is_locked = true;
		lock_owner = new_lock_id;
	}

	//Scenes
	scene_list.reserve(scn_list.size());
	copy(scn_list.begin(), scn_list.end(), back_inserter(scene_list));

	//Matrix Attributes
	location=new_location;
	rotation_euler=new_rotatione;
	rotation_quaternion=new_rotationq;
	scaling=new_scale;
	transform_matrix=new_transform;
	bounding_box=new_bounding_box;

  logging->debug("Obj3 Built");
}

Obj3::Obj3(const rapidjson::Document& d)
{
	logging->debug("Build Object Called");

  //Building replacement variables
  std::string new_name="";
  std::string new_key="";
  std::string new_owner="";
  std::string new_type="";
  std::string new_subtype="";
  std::string new_lock_id="";
  std::vector<std::string> scn_list;
  Eigen::Vector3d new_location=Eigen::Vector3d::Zero(3);
  Eigen::Vector3d new_rotatione=Eigen::Vector3d::Zero(3);
  Eigen::Vector4d new_rotationq=Eigen::Vector4d::Zero(4);
  Eigen::Vector3d new_scale=Eigen::Vector3d::Zero(3);
  Eigen::Matrix4d new_transform=Eigen::Matrix4d::Zero(4, 4);
  Eigen::MatrixXd new_bounding_box=Eigen::MatrixXd::Zero(4, 8);
  logging->debug("New Variables Declared");

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

	if (d.IsObject()) {

		logging->debug("Object-Format Message Detected");

    if (d.HasMember("name")) {
      const rapidjson::Value *name_val;
      name_val = &d["name"];
      new_name = name_val->GetString();
    }
    if (d.HasMember("key")) {
      const rapidjson::Value *key_val;
      key_val = &d["key"];
      new_key = key_val->GetString();
    }
    if (d.HasMember("owner")) {
      const rapidjson::Value *owner_val;
      owner_val = &d["owner"];
      new_owner = owner_val->GetString();
    }
    if (d.HasMember("type")) {
      const rapidjson::Value *type_val;
      type_val = &d["type"];
      new_type = type_val->GetString();
    }
    if (d.HasMember("subtype")) {
      const rapidjson::Value *subtype_val;
      subtype_val = &d["subtype"];
      new_subtype = subtype_val->GetString();
    }
    if (d.HasMember("lock_device_id")) {
      const rapidjson::Value *lock_val;
      lock_val = &d["lock_device_id"];
      new_lock_id = lock_val->GetString();
    }
    if (d.HasMember("location")) {
      //Read the array values and stuff them into new_location
      const rapidjson::Value& loc = d["location"];
      if (loc.IsArray()) {
        int j=0;
        for (rapidjson::SizeType i = 0; i < loc.Size();i++) {
          new_location(j) = loc[i].GetDouble();
          j++;
        }
      }
    }
    if (d.HasMember("rotation_euler")) {
      //Read the array values and stuff them into new_location
      const rapidjson::Value& rote = d["rotation_euler"];
      if (rote.IsArray()) {
        int j=0;
        for (rapidjson::SizeType i = 0; i < rote.Size();i++) {
          new_rotatione(j) = rote[i].GetDouble();
          j++;
        }
      }
    }
    if (d.HasMember("rotation_quaternion")) {
      //Read the array values and stuff them into new_location
      const rapidjson::Value& rotq = d["rotation_quaternion"];
      if (rotq.IsArray()) {
        int j=0;
        for (rapidjson::SizeType i = 0; i < rotq.Size();i++) {
          new_rotationq(j) = rotq[i].GetDouble();
          j++;
        }
      }
    }
    if (d.HasMember("scale")) {
      //Read the array values and stuff them into new_location
      const rapidjson::Value& scl = d["scale"];
      if (scl.IsArray()) {
        int j=0;
        for (rapidjson::SizeType i = 0; i < scl.Size();i++) {
          new_scale(j) = scl[i].GetDouble();
          j++;
        }
      }
    }
    if (d.HasMember("transform")) {
      //Read the array values and stuff them into new_transform
      const rapidjson::Value& tran = d["transform"];
      if (tran.IsArray()) {
        int j=0;
        for (rapidjson::SizeType i = 0; i < tran.Size();i++) {
          new_transform(i%4, j) = tran[i].GetDouble();
          if (i == 3 || i % 4 == 3) {
            j++;
          }
        }
      }
      logging->debug("Transform Matrix Parsed");
    }
    if (d.HasMember("bounding_box")) {
      //Read the array values and stuff them into new_bounding_box
      const rapidjson::Value& bb = d["bounding_box"];
      if (bb.IsArray()) {
        int j=0;
        for (rapidjson::SizeType i = 0; i < bb.Size();i++) {
          if (j < 8 && i ) {
            new_bounding_box(i%4, j) = bb[i].GetDouble();
            if (i == 3 || i % 4 == 3 ) {
              j++;
            }
          }
        }
      }
      logging->debug("Bounding Box Parsed");
    }

    if (d.HasMember("scenes")) {
      //Read the array values and stuff them into new_location
      const rapidjson::Value& sc = d["scenes"];
      if (sc.IsArray()) {
        for (rapidjson::SizeType i = 0; i < sc.Size();i++) {
          scn_list.push_back(sc[i].GetString());
        }
      }
    }

	}


    logging->debug("Variables Filled");

		//Set the String Attributes
		name = new_name;
		key = new_key;
		type = new_type;
		subtype = new_subtype;
		owner = new_owner;

		//Create Matrices
		initialize_matrices();

		//Lock Attributes
		if (new_lock_id == "") {
			is_locked=false;
			lock_owner="";
		}
		else {
			is_locked = true;
			lock_owner = new_lock_id;
		}

		//Scenes
		scene_list.reserve(scn_list.size());
		copy(scn_list.begin(), scn_list.end(), back_inserter(scene_list));

		//Matrix Attributes
		location=new_location;
		rotation_euler=new_rotatione;
		rotation_quaternion=new_rotationq;
		scaling=new_scale;
		transform_matrix=new_transform;
		bounding_box=new_bounding_box;

	  logging->debug("Obj3 Built");
}

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
	location(0) = res_loc(0);
	location(1) = res_loc(1);
	location(2) = res_loc(2);

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

std::string Obj3::to_protobuf_msg(int msg_type) const {
	logging->info("Obj3:To Proto message Called on object");
	logging->info(key);
	protoObj3::Obj3 new_proto;
	new_proto.set_message_type(msg_type);
	new_proto.set_key(key);
	logging->debug("Obj3: Key = ");
	logging->debug(key);
	new_proto.set_name(name);
	logging->debug("Obj3: Name = ");
	logging->debug(name);
	new_proto.set_type(type);
	logging->debug("Obj3: Type = ");
	logging->debug(type);
	new_proto.set_subtype(subtype);
	logging->debug("Obj3: Subtype = ");
	logging->debug(subtype);
	new_proto.set_owner(owner);
	logging->debug("Obj3: Owner = ");
	logging->debug(owner);
	new_proto.set_lock_device_id(lock_owner);
	logging->debug("Obj3: Lock Owner = ");
	logging->debug(lock_owner);
	protoObj3::Obj3_Vertex3 loc = new_proto.location();
	loc.set_x(get_locx());
	loc.set_y(get_locy());
	loc.set_z(get_locz());
	// protoObj3::Obj3_Vertex3 rote = new_proto.rotation_euler();
	// rote.set_x(get_rotex());
	// rote.set_y(get_rotey());
	// rote.set_z(get_rotez());
	// protoObj3::Obj3_Vertex4 rotq = new_proto.rotation_quaternion();
	// rotq.set_w(get_rotqw());
	// rotq.set_x(get_rotqx());
	// rotq.set_y(get_rotqy());
	// rotq.set_z(get_rotqz());
	// protoObj3::Obj3_Vertex3 scl = new_proto.scale();
	// scl.set_x(get_sclx());
	// scl.set_y(get_scly());
	// scl.set_z(get_sclz());
	protoObj3::Obj3_Matrix4 trn = new_proto.transform();
	int i = 0;
	for (i = 0; i < 4; i++) {
		protoObj3::Obj3_Vertex4* c1 = trn.add_col();
		c1->set_w(transform_matrix(0, i));
		c1->set_x(transform_matrix(1, i));
		c1->set_y(transform_matrix(2, i));
		c1->set_z(transform_matrix(3, i));
	}
	int j = 0;
	for (j = 0; j < num_scenes(); j++) {
		new_proto.add_scenes(get_scene(j));
	}
	std::string wstr;
  new_proto.SerializeToString(&wstr);
	logging->debug("Protocol Buffer Serialized to String");
	logging->debug(wstr);
	return wstr;
}
