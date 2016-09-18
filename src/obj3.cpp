//This is the Obj3 Cpp file
//This contains matrix and trig
//logic used in the Obj3 class

#include "obj3.h"
#include <math.h>

#define PI 3.14159265358979323846

using namespace rapidjson;

//----------------------------------------------------------------------------//
//---------------------------Constructors-------------------------------------//
//----------------------------------------------------------------------------//

void Obj3::initialize_buffers()
{
	obj_logging->info("Obj3:Initialize Buffers Called");
	transform_buffer = Eigen::Matrix4d::Zero(4, 4);
	transform_buffer(0, 0) = 1.0;
  transform_buffer(1, 1) = 1.0;
  transform_buffer(2, 2) = 1.0;
  transform_buffer(3, 3) = 1.0;
}

void Obj3::initialize_matrices()
{
	obj_logging->info("Obj3:Intialize Matrices Called");
	//Set initial values with function calls
	bounding_box = Eigen::MatrixXd::Zero(4, 8);
	location = Eigen::Vector3d::Zero(3);
	rotation_euler = Eigen::Vector3d::Zero(3);
	rotation_quaternion = Eigen::Vector4d::Zero(4);
	scaling = Eigen::Vector3d::Zero(3);
	transform_matrix = Eigen::Matrix4d::Zero(4, 4);

	initialize_buffers();
}

Obj3::Obj3(protoObj3::Obj3 buffer, bool locking_enabled)
{
	global_transform_type=false;
	obj_logging->debug("Build Proto-Object Called");
  std::string new_name="";
  std::string new_key="";
  std::string new_owner="";
  std::string new_type="";
  std::string new_subtype="";
  std::string new_lock_id="";
	std::string new_tran_id="";
	std::string new_mesh_id="";
  std::vector<std::string> scn_list;
  obj_logging->debug("New Variables Declared");
	int new_message_type;

	Eigen::Vector3d new_location=Eigen::Vector3d::Zero(3);
	Eigen::Vector3d new_rotatione=Eigen::Vector3d::Zero(3);
	Eigen::Vector4d new_rotationq=Eigen::Vector4d::Zero(4);
	Eigen::Vector3d new_scale=Eigen::Vector3d::Zero(3);
	Eigen::Matrix4d new_transform=Eigen::Matrix4d::Zero(4, 4);
	Eigen::MatrixXd new_bounding_box=Eigen::MatrixXd::Zero(4, 8);

  //Perform the Conversion
	if (buffer.has_message_type()) {
		new_message_type = buffer.message_type();
	}
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
	if (buffer.has_transaction_id()) {
		new_tran_id = buffer.transaction_id();
	}
	if (buffer.has_mesh_id()) {
		new_mesh_id = buffer.mesh_id();
	}
	if (buffer.has_transform_type()) {
		if (buffer.transform_type() == "global")
		{
			global_transform_type = true;
		}
	}
  if (buffer.has_location()) {
    protoObj3::Obj3_Vertex3 loc = buffer.location();
    new_location(0) = loc.x();
    new_location(1) = loc.y();
    new_location(2) = loc.z();
		location=new_location;
		locn_flag=true;
  }
  if (buffer.has_rotation_euler()) {
    protoObj3::Obj3_Vertex3 rote = buffer.rotation_euler();
    new_rotatione(0) = rote.x();
    new_rotatione(1) = rote.y();
    new_rotatione(2) = rote.z();
		rotation_euler=new_rotatione;
		rote_flag=true;
  }
  if (buffer.has_rotation_quaternion()) {
    protoObj3::Obj3_Vertex4 rotq = buffer.rotation_quaternion();
    new_rotationq(0) = rotq.w();
    new_rotationq(1) = rotq.x();
    new_rotationq(2) = rotq.y();
    new_rotationq(3) = rotq.z();
		rotation_quaternion=new_rotationq;
		rotq_flag=true;
  }
  if (buffer.has_scale()) {
    protoObj3::Obj3_Vertex3 scl = buffer.scale();
    new_scale(0) = scl.x();
    new_scale(1) = scl.y();
    new_scale(2) = scl.z();
		scaling=new_scale;
		scl_flag=true;
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
    obj_logging->debug("Transform Matrix Parsed");
		transform_matrix=new_transform;
		trns_flag=true;
  }
  if (buffer.has_bounding_box()) {
    protoObj3::Obj3_Matrix4 bb = buffer.bounding_box();
    int i = 0;
    for (i=0; i < bb.col_size(); i++) {
      protoObj3::Obj3_Vertex4 c = bb.col(i);
      new_bounding_box(0, i) = c.w();
      new_bounding_box(1, i) = c.x();
      new_bounding_box(2, i) = c.y();
      new_bounding_box(3, i) = c.z();
    }
    obj_logging->debug("Bounding Box Parsed");
		bounding_box=new_bounding_box;
		boun_flag=true;
  }
  if (buffer.scenes_size() > 0) {
    int j = 0;
    for (j=0; j< buffer.scenes_size(); j++) {
      scn_list.push_back(buffer.scenes(j));
    }
  }

  obj_logging->debug("Variables Filled");

	//Set the String Attributes
	name = new_name;
	key = new_key;
	type = new_type;
	subtype = new_subtype;
	owner = new_owner;
	mes_type = new_message_type;
	app_transaction_id = new_tran_id;

	//Lock Attributes
	if ((new_lock_id == "" || new_lock_id == "-1") && locking_enabled) {
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

  obj_logging->debug("Obj3 Built");
}

Obj3::Obj3(const rapidjson::Document& d, bool locking_enabled)
{
	global_transform_type=false;
	obj_logging->debug("Build Object Called");

  //Building replacement variables
  std::string new_name="";
  std::string new_key="";
  std::string new_owner="";
  std::string new_type="";
  std::string new_subtype="";
  std::string new_lock_id="";
	std::string new_tran_id="";
	std::string new_mesh_id="";
	std::string new_app_transaction_id="";
	int new_message_type;
  std::vector<std::string> scn_list;
  obj_logging->debug("New Variables Declared");

	Eigen::Vector3d new_location=Eigen::Vector3d::Zero(3);
	Eigen::Vector3d new_rotatione=Eigen::Vector3d::Zero(3);
	Eigen::Vector4d new_rotationq=Eigen::Vector4d::Zero(4);
	Eigen::Vector3d new_scale=Eigen::Vector3d::Zero(3);
	Eigen::Matrix4d new_transform=Eigen::Matrix4d::Zero(4, 4);
	Eigen::MatrixXd new_bounding_box=Eigen::MatrixXd::Zero(4, 8);

	if (d.IsObject()) {

		obj_logging->debug("Object-Format Message Detected");

		if (d.HasMember("message_type")) {
      const rapidjson::Value *ms_type_val;
      ms_type_val = &d["message_type"];
      new_message_type = ms_type_val->GetInt();
    }
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
		if (d.HasMember("transaction_id")) {
			const rapidjson::Value *tran_id_val;
      tran_id_val = &d["transaction_id"];
      new_app_transaction_id = tran_id_val->GetString();
		}
		if (d.HasMember("mesh_id")) {
			const rapidjson::Value *mesh_id_val;
      mesh_id_val = &d["mesh_id"];
      new_mesh_id = mesh_id_val->GetString();
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
		if (d.HasMember("transform_type")) {
			const rapidjson::Value *tran_type;
      tran_type = &d["lock_device_id"];
      std::string tran_type_str = tran_type->GetString();
			if (tran_type_str == "global") {
				global_transform_type = true;
			}
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
			location=new_location;
			locn_flag=true;
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
			rotation_euler=new_rotatione;
			rote_flag=true;
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
			rotation_quaternion=new_rotationq;
			rotq_flag=true;
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
			scaling=new_scale;
			scl_flag=true;
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
      obj_logging->debug("Transform Matrix Parsed");
			transform_matrix=new_transform;
			trns_flag=true;
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
      obj_logging->debug("Bounding Box Parsed");
			bounding_box=new_bounding_box;
			boun_flag=true;
    }

    if (d.HasMember("scenes")) {
      //Read the array values and stuff them into new_location
      const rapidjson::Value& sc = d["scenes"];
      if (sc.IsArray()) {
        for (rapidjson::SizeType i = 0; i < sc.Size();i++) {
          scene_list.push_back(sc[i].GetString());
        }
      }
    }

	}

	//Set the String Attributes
	name = new_name;
	key = new_key;
	type = new_type;
	subtype = new_subtype;
	owner = new_owner;
	mes_type = new_message_type;
	app_transaction_id = new_tran_id;

  obj_logging->debug("Variables Filled");

	//Lock Attributes
	if ((new_lock_id == "" || new_lock_id == "-1") && locking_enabled) {
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

  obj_logging->debug("Obj3 Built");
}

//----------------------------------------------------------------------------//
//-----------------------Transformation Methods-------------------------------//
//----------------------------------------------------------------------------//

void Obj3::apply_transforms(Eigen::Matrix4d trans_matrix, bool global_transforms_active)
{
	obj_logging->info("Obj3:Apply Transforms Called");

	//Update the transformation matrix
	if (global_transforms_active) {
		transform_matrix = trans_matrix * transform_matrix;
	}
	else {
		transform_matrix = transform_matrix * trans_matrix;
	}
}

void Obj3::transform_object(double trans_matrix[], bool global_transforms_active)
{
	obj_logging->info("Obj3:Transform Object called with double[]");
	Eigen::Matrix4d tran_matrix;
	int i, j;
	for (i=0;i<4;i=i+1)
	{
		for (j=0;j<4;j=j+1)
		{
			tran_matrix(i, j) = trans_matrix[(4 * i) + j];
		}
	}

	apply_transforms( tran_matrix, global_transforms_active );
}

//Translate an object by some amounts x, y, and z on the respective axis
void Obj3::translate_object(double x, double y, double z, bool global_transforms_active)
{
	obj_logging->info("Obj3:Translate Object called");
	//Variable Declarations
	Eigen::Matrix4d tran_matrix;

	//Set up the transformation matrix
	tran_matrix = Eigen::Matrix4d::Zero(4, 4);
	tran_matrix(0, 0) = 1.0;
	tran_matrix(1, 1) = 1.0;
	tran_matrix(2, 2) = 1.0;
	tran_matrix(3, 3) = 1.0;

	tran_matrix(0, 3) = x;
	tran_matrix(1, 3) = y;
	tran_matrix(2, 3) = z;

	//Apply the transformation
	apply_transforms( tran_matrix, global_transforms_active );

}

//Rotate an object by a magnitude theta about the axis x, y, z
void Obj3::rotate_object(double x, double y, double z, double theta, bool global_transforms_active)
{
obj_logging->info("Obj3:RotateQ Object Called");
Eigen::Matrix4d tran_matrix;

//Set up the transformation matrix
tran_matrix = Eigen::Matrix4d::Zero(4, 4);
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

apply_transforms( tran_matrix, global_transforms_active );
}

//Rotate an object about the X Axis
void Obj3::rotate_objectx(double x, bool global_transforms_active)
{
obj_logging->info("Obj3:Rotate Object about X-Axis Called");
//Variable Declarations
Eigen::Matrix4d tran_matrix = Eigen::Matrix4d::Zero(4, 4);

//Set up the transformation matrix
tran_matrix(1, 1) = cos (x * (PI/180));
tran_matrix(2, 1) = sin (x * (PI/180));
tran_matrix(1, 2) = sin (x * (PI/180)) * -1.0;
tran_matrix(2, 2) = cos (x * (PI/180));
tran_matrix(0, 0) = 1.0;
tran_matrix(3, 3) = 1.0;

//Apply the transformation
apply_transforms( tran_matrix, global_transforms_active );
}

//Rotate an object about the Y Axis
void Obj3::rotate_objecty(double y, bool global_transforms_active)
{
obj_logging->info("Obj3:Rotate Object about Y-Axis Called");
//Variable Declarations
Eigen::Matrix4d tran_matrix = Eigen::Matrix4d::Zero(4, 4);

tran_matrix(0, 0) = cos (y * (PI/180));
tran_matrix(0, 2) = sin (y * (PI/180));
tran_matrix(2, 0) = sin (y * (PI/180)) * -1.0;
tran_matrix(2, 2) = cos (y * (PI/180));
tran_matrix(1, 1) = 1.0;
tran_matrix(3, 3) = 1.0;

//Apply the transformation
apply_transforms( tran_matrix, global_transforms_active );
}

//Rotate an object about the Z Axis
void Obj3::rotate_objectz(double z, bool global_transforms_active)
{
obj_logging->info("Obj3:Rotate Object about Z-Axis Called");
//Variable Declarations
Eigen::Matrix4d tran_matrix = Eigen::Matrix4d::Zero(4, 4);

tran_matrix(0, 0) = cos (z * (PI/180));
tran_matrix(1, 0) = sin (z * (PI/180));
tran_matrix(0, 1) = sin (z * (PI/180)) * -1.0;
tran_matrix(1, 1) = cos (z * (PI/180));
tran_matrix(2, 2) = 1.0;
tran_matrix(3, 3) = 1.0;

//Apply the transformation
apply_transforms( tran_matrix, global_transforms_active );

}

void Obj3::rotate_object(double x, double y, double z, bool global_transforms_active)
{
obj_logging->info("Obj3:RotateE Object Called");
if (std::abs(x) > 0.001)
{
	rotate_objectx(x, global_transforms_active);
}
if (std::abs(y) > 0.001)
{
	rotate_objecty(y, global_transforms_active);
}
if (std::abs(z) > 0.001)
{
	rotate_objectz(z, global_transforms_active);
}

}

void Obj3::scale_object(double x, double y, double z, bool global_transforms_active)
{
	obj_logging->info("Obj3:Scale Object Called");
	//Variable Declarations
  Eigen::Matrix4d tran_matrix;

  //Set up the transformation matrix
  tran_matrix = Eigen::Matrix4d::Zero(4, 4);
  tran_matrix(0, 0) = x;
  tran_matrix(1, 1) = y;
  tran_matrix(2, 2) = z;
  tran_matrix(3, 3) = 1.0;

	//Apply the transformation
	apply_transforms( tran_matrix, global_transforms_active );

}

void Obj3::transform_object(Obj3 *temp_obj)
{
	//First, we apply any matrix transforms present

	//Are we doing a matrx transform?
	if (!(temp_obj->has_location()) && !(temp_obj->has_rotatione()) && !(temp_obj->has_rotationq()) && !(temp_obj->has_scaling())) {
		obj_logging->debug("Applying Transform Matrix and full transform stack");
		apply_transforms(temp_obj->get_transform(), temp_obj->is_global_trans_type());
	}
	else
	{
		if (temp_obj->has_location()) {
			obj_logging->debug("Location Transformation Detected");
			translate_object(temp_obj->get_locx(), temp_obj->get_locy(), temp_obj->get_locz(), temp_obj->is_global_trans_type());
		}

		if (temp_obj->has_rotatione()) {
			obj_logging->debug("Euler Rotation Transformation Detected");
			rotate_object(temp_obj->get_rotex(), temp_obj->get_rotey(), temp_obj->get_rotez(), temp_obj->is_global_trans_type());
		}

		if (temp_obj->has_rotationq()) {
			obj_logging->debug("Quaternion Rotation Transformation Detected");
			rotate_object(temp_obj->get_rotqw(), temp_obj->get_rotqx(), temp_obj->get_rotqy(), temp_obj->get_rotqz(), temp_obj->is_global_trans_type());
		}

		if (temp_obj->has_scaling()) {
			obj_logging->debug("Scale Transformation Detected");
			scale_object(temp_obj->get_sclx(), temp_obj->get_scly(), temp_obj->get_sclz(), temp_obj->is_global_trans_type());
		}
	}

	//Next, we write any string attributes
	if (temp_obj->get_owner() != "") {
		std::string nowner = temp_obj->get_owner();
		set_owner(nowner);
	}

	if (temp_obj->get_name() != "") {
		std::string nname = temp_obj->get_name();
		set_name(nname);
	}

	if (temp_obj->get_type() != "") {
		std::string ntype = temp_obj->get_type();
		set_type(ntype);
	}

	if (temp_obj->get_subtype() != "") {
		std::string nsubtype = temp_obj->get_subtype();
		set_subtype(nsubtype);
	}
}

//----------------------------------------------------------------------------//
//-------------------------Messaging Methods----------------------------------//
//----------------------------------------------------------------------------//

//Convert the object to JSON Message
std::string Obj3::to_json_msg(int msg_type, std::string trans_id, bool write_transform_type) const {
	obj_logging->info("Obj3:To JSON message Called on object");
	obj_logging->info(key);
	//Initialize the string buffer and writer
	StringBuffer s;
	Writer<StringBuffer> writer(s);

	//Start writing the object
	//Syntax taken directly from
	//simplewriter.cpp in rapidjson examples

	writer.StartObject();

	if (msg_type != -1) {
		writer.Key("message_type");
		writer.Uint(msg_type);
	}

	if (!key.empty()) {
		writer.Key("key");
		writer.String( key.c_str(), (SizeType)key.length() );
	}

	if (!owner.empty()) {
		writer.Key("owner");
		writer.String( owner.c_str(), (SizeType)owner.length() );
	}

	if (!name.empty()) {
		writer.Key("name");
		writer.String( name.c_str(), (SizeType)name.length() );
	}

	if (!err_string.empty()) {
		writer.Key("error");
		writer.String( err_string.c_str(), (SizeType)err_string.length() );
	}

	if (!type.empty()) {
		writer.Key("type");
		writer.String( type.c_str(), (SizeType)type.length() );
	}

	if (!subtype.empty()) {
		writer.Key("subtype");
		writer.String( subtype.c_str(), (SizeType)subtype.length() );
	}

	writer.Key("transaction_id");
	if (!trans_id.empty()) {
		writer.String(trans_id.c_str(), (SizeType)trans_id.length());
	}
	else {
		writer.String(app_transaction_id.c_str(), (SizeType)app_transaction_id.length());
	}

	int i;
	int j;

	if (locn_flag) {
		writer.Key("location");
		writer.StartArray();
		for (i=0; i<3; i++) {
			writer.Double( static_cast<double>(get_loc(i)) );
		}
		writer.EndArray();
	}

	if (rote_flag) {
		writer.Key("rotation_euler");
		writer.StartArray();
		for (i=0; i<3; i++) {
			writer.Double( static_cast<double>(get_rote(i)) );
		}
		writer.EndArray();
	}

	if (rotq_flag) {
		writer.Key("rotation_quaternion");
		writer.StartArray();
		for (i=0; i<4; i++) {
			writer.Double( static_cast<double>(get_rotq(i)) );
		}
		writer.EndArray();
	}

	if (scl_flag) {
		writer.Key("scale");
		writer.StartArray();
		for (i=0; i<3; i++) {
			writer.Double( static_cast<double>(get_scl(i)) );
		}
		writer.EndArray();
	}

	if (trns_flag) {
		writer.Key("transform");
		writer.StartArray();
		for (i=0; i<4; i++) {
			for (j=0; j<4; j++) {
				writer.Double( static_cast<double>(transform_matrix(i, j) ));
			}
		}
		writer.EndArray();
	}

	if (boun_flag) {
		writer.Key("bounding_box");
		writer.StartArray();
		for (i=0; i<4; i++) {
			for (j=0; j<8; j++) {
				writer.Double( static_cast<double>(bounding_box(i, j) ));
			}
		}
		writer.EndArray();
	}

	writer.Key("scenes");
	writer.StartArray();
	for (i=0; i<num_scenes(); i++) {
					std::string sc = get_scene(i);
					writer.String( sc.c_str(), (SizeType)sc.length() );
	}
	writer.EndArray();

	writer.Key("locked");
	writer.Bool(is_locked);

	if (!mesh_id.empty()) {
		writer.Key("mesh_id");
		writer.String(mesh_id.c_str(), (SizeType)mesh_id.length());
	}

	if (!err_string.empty()) {
		writer.Key("error_message");
		writer.String(err_string.c_str(), (SizeType)err_string.length());
	}

	if (global_transform_type && write_transform_type) {
		writer.Key("transform_type");
		std::string tt_str = "true";
		writer.String( tt_str.c_str(), (SizeType)tt_str.length() );
	}

	writer.EndObject();

	//The Stringbuffer now contains a json message
	//of the object
const char* ret_val = s.GetString();
std::string ret_string (ret_val);
	return ret_string;
}

std::string Obj3::to_json_msg(int msg_type, std::string transact_id) const
{
	std::string trans_id = "";
	return to_json_msg(msg_type, transact_id, true);
}


std::string Obj3::to_json_msg(int msg_type) const
{
	std::string trans_id = "";
	return to_json_msg(msg_type, trans_id, true);
}

std::string Obj3::to_json()
{
	std::string trans_id = "";
	int msg_type = -1;
	return to_json_msg(msg_type, trans_id, false);
}

void Obj3::to_base_protobuf_msg(protoObj3::Obj3 *new_proto, bool write_transform_type) const {
	if (!key.empty()) {
		new_proto->set_key(key);
		obj_logging->debug("Obj3: Key = ");
		obj_logging->debug(key);
	}
	if (!name.empty()) {
		new_proto->set_name(name);
		obj_logging->debug("Obj3: Name = ");
		obj_logging->debug(name);
	}
	if (!type.empty()) {
		new_proto->set_type(type);
		obj_logging->debug("Obj3: Type = ");
		obj_logging->debug(type);
	}
	if (!subtype.empty()) {
		new_proto->set_subtype(subtype);
		obj_logging->debug("Obj3: Subtype = ");
		obj_logging->debug(subtype);
	}
	if (!owner.empty()) {
		new_proto->set_owner(owner);
		obj_logging->debug("Obj3: Owner = ");
		obj_logging->debug(owner);
	}
	if (!lock_owner.empty()) {
		new_proto->set_lock_device_id(lock_owner);
		obj_logging->debug("Obj3: Lock Owner = ");
		obj_logging->debug(lock_owner);
	}
	if (!err_string.empty()) {
		new_proto->set_error_message(err_string);
		obj_logging->debug("Obj3: Lock Owner = ");
		obj_logging->debug(lock_owner);
	}
	if (!mesh_id.empty()) {
		new_proto->set_mesh_id(mesh_id);
		obj_logging->debug("Obj3: Lock Owner = ");
		obj_logging->debug(lock_owner);
	}
	if (global_transform_type && write_transform_type) {
		new_proto->set_transform_type("global");
		obj_logging->debug("Obj3: Global transform type set on message");
	}
	if (locn_flag) {
		protoObj3::Obj3_Vertex3 *loc = new_proto->mutable_location();
		loc->set_x(get_locx());
		loc->set_y(get_locy());
		loc->set_z(get_locz());
	}
	if (rote_flag) {
		protoObj3::Obj3_Vertex3 *rote = new_proto->mutable_rotation_euler();
		rote->set_x(get_rotex());
		rote->set_y(get_rotey());
		rote->set_z(get_rotez());
	}
	if (rotq_flag) {
		protoObj3::Obj3_Vertex4 *rotq = new_proto->mutable_rotation_quaternion();
		rotq->set_w(get_rotqw());
		rotq->set_x(get_rotqx());
		rotq->set_y(get_rotqy());
		rotq->set_z(get_rotqz());
	}
	if (scl_flag) {
		protoObj3::Obj3_Vertex3 *scl = new_proto->mutable_scale();
		scl->set_x(get_sclx());
		scl->set_y(get_scly());
		scl->set_z(get_sclz());
	}
	if (trns_flag) {
		protoObj3::Obj3_Matrix4 *trn = new_proto->mutable_transform();
		int i = 0;
		for (i = 0; i < 4; i++) {
			protoObj3::Obj3_Vertex4* c1 = trn->add_col();
			c1->set_w(transform_matrix(0, i));
			c1->set_x(transform_matrix(1, i));
			c1->set_y(transform_matrix(2, i));
			c1->set_z(transform_matrix(3, i));
		}
	}
	if (boun_flag) {
		protoObj3::Obj3_Matrix4 *bbox = new_proto->mutable_bounding_box();
		int k = 0;
		for (k = 0; k < 8; k++) {
			protoObj3::Obj3_Vertex4* cl = bbox->add_col();
			cl->set_w(bounding_box(0, k));
			cl->set_x(bounding_box(1, k));
			cl->set_y(bounding_box(2, k));
			cl->set_z(bounding_box(3, k));
		}
	}
	int j = 0;
	for (j = 0; j < num_scenes(); j++) {
		new_proto->add_scenes(get_scene(j));
	}
}

//Writes out all object attributes for storage in Smart Update Buffer
std::string Obj3::to_protobuf_msg(int msg_type) const {
	obj_logging->info("Obj3:To Proto message Called on object");
	obj_logging->info(key);
	protoObj3::Obj3 *new_proto = new protoObj3::Obj3;
	if (msg_type != -1) {
		new_proto->set_message_type(msg_type);
	}
	if (!app_transaction_id.empty()) {
		new_proto->set_transaction_id(app_transaction_id);
	}
	to_base_protobuf_msg(new_proto, true);
	std::string wstr;
  new_proto->SerializeToString(&wstr);
	obj_logging->debug("Protocol Buffer Serialized to String");
	obj_logging->debug(wstr);
	delete new_proto;
	return wstr;
}

//Convert the object to a protocol buffer message
std::string Obj3::to_protobuf_msg(int msg_type, std::string trans_id) const
{
	obj_logging->info("Obj3:To Proto message Called on object");
	obj_logging->info(key);
	protoObj3::Obj3 *new_proto = new protoObj3::Obj3;
	if (msg_type != -1) {
		new_proto->set_message_type(msg_type);
	}
	if (!trans_id.empty()) {
		new_proto->set_transaction_id(trans_id);
	}
	to_base_protobuf_msg(new_proto, true);
	std::string wstr;
  new_proto->SerializeToString(&wstr);
	obj_logging->debug("Protocol Buffer Serialized to String");
	obj_logging->debug(wstr);
	delete new_proto;
	return wstr;
}
