//The Obj3 object acts as an ORM for the JSON Documents
//In the Database, and is loaded/saved by the respective
//implementation of the db_admin.h interface

//OBJ3.H
#ifndef OBJ3_H
#define OBJ3_H

#include <string>
#include <Eigen/Dense>
#include "lyman_utils.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <iostream>
#include <cmath>
#include <vector>
#include "Obj3.pb.h"
#include "lyman_log.h"

#include "aossl/factory/writeable.h"

class Obj3: public Writeable
{
	private:
		//String Attributes
		std::string name;
		std::string key;
		std::string type;
		std::string subtype;
		std::string owner;
		std::string app_transaction_id;
		std::string mesh_id;
		std::string err_string;
		int mes_type;

		//Externally Referenceable data
		//Float Matrix for location
		Eigen::Vector3d location;
		bool locn_flag = false;
		Eigen::Vector3d rotation_euler;
		bool rote_flag = false;
		Eigen::Vector4d rotation_quaternion;
		bool rotq_flag = false;
		Eigen::Vector3d scaling;
		bool scl_flag = false;

		//Transform Matrix
    Eigen::Matrix4d transform_matrix;
		bool trns_flag = false;

		//Bounding Box
    Eigen::MatrixXd bounding_box;
		bool boun_flag;

		//Transform Buffers
		Eigen::Matrix4d transform_buffer;

		//Private constructor for internal matrices
		void initialize_matrices();
		void initialize_buffers();

		//A list of scenes
		std::vector<std::string> scene_list;

		//The event dispatcher for the object

		//Locking Attributes
		bool is_locked;
		std::string lock_owner;

		//Internal Transformation methods
		void translate_object(double x, double y, double z);
		void rotate_objectx(double x);
		void rotate_objecty(double y);
		void rotate_objectz(double z);
		void rotate_object(double x, double y, double z);
		void rotate_object(double x, double y, double z, double theta);
		void scale_object(double x, double y, double z);

		//Apply Transforms
		void apply_transforms(Eigen::Matrix4d trans_matrix);

		//Smart Update
		void transform_object(Obj3 *obj);

		//Transform
		void transform_object(double trans_matrix[]);

		//Base protobuffer message
		void to_base_protobuf_msg(protoObj3::Obj3 *new_proto) const;

	public:
		//Constructors & Destructor

		//Basic constructor with no params
		Obj3() {name = ""; key = ""; type = ""; subtype = ""; initialize_matrices(); owner=""; is_locked=false; lock_owner="";}

		//Constructor accepting Protocol Buffer
		Obj3(protoObj3::Obj3 buffer);

		//Constructor accepting Rapidjson Document
		Obj3(const rapidjson::Document& d);

		//String-Only Constructors
		Obj3(std::string iname, std::string ikey){name = iname; key = ikey; type = ""; subtype = ""; initialize_matrices();owner="";is_locked=false; lock_owner="";}
		Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype) {name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner="";is_locked=false; lock_owner="";}
		Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner) {name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";}

		//Matrix Constructors
		//Location only
		Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner, Eigen::Vector3d ilocation)
{name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";location=ilocation;locn_flag = true;}

		//Location & Bounding Box
		Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner, Eigen::Vector3d ilocation, Eigen::MatrixXd ibounding_box)
{name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";location=ilocation;bounding_box=ibounding_box;locn_flag = true;boun_flag=true;}

		//Location, Transform, & Bounding Box
                Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner, Eigen::Vector3d ilocation, Eigen::Matrix4d itransform, Eigen::MatrixXd ibounding_box)
{name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";location=ilocation;transform_matrix=itransform;bounding_box=ibounding_box;locn_flag = true;boun_flag=true;trns_flag=true;}

		//Location, Rotation, Scale, Transform, & Bounding Box
                Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner, Eigen::Vector3d ilocation, Eigen::Vector3d irotatione, Eigen::Vector4d irotationq, Eigen::Vector3d iscale, Eigen::Matrix4d itransform, Eigen::MatrixXd ibounding_box)
{name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";location=ilocation;rotation_euler=irotatione;rotation_quaternion=irotationq;scaling=iscale;transform_matrix=itransform;bounding_box=ibounding_box;locn_flag = true;boun_flag=true;trns_flag=true;rote_flag=true;rotq_flag=true;scl_flag=true;}

                //All elements
                Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner, std::vector<std::string> scns, Eigen::Vector3d ilocation, Eigen::Vector3d irotatione, Eigen::Vector4d irotationq, Eigen::Vector3d iscale, Eigen::Matrix4d itransform, Eigen::MatrixXd ibounding_box)
{name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";scene_list = scns;location=ilocation;rotation_euler=irotatione;rotation_quaternion=irotationq;scaling=iscale;transform_matrix=itransform;bounding_box=ibounding_box;locn_flag = true;boun_flag=true;trns_flag=true;rote_flag=true;rotq_flag=true;scl_flag=true;}

		//Transformation Methods

		//Smart Update
		bool transform(Obj3 *obj) {if (is_locked==false) {transform_object(obj); return true;} else {return false;}}

		bool transform(Obj3 *obj, std::string device_id) {if (is_locked==false || lock_owner==device_id) {transform_object(obj); return true;} else {return false;}}

		//Transform
		bool transform(double trans_matrix[]) {if (is_locked==false) {transform_object(trans_matrix); return true;} else {return false;}}

		bool transform(double trans_matrix[], std::string device_id) {if (is_locked==false || lock_owner==device_id) {transform_object(trans_matrix); return true;} else {return false;}}

		//Translation
		bool translate(double x, double y, double z, std::string device_id) {if (is_locked==false || lock_owner==device_id) {translate_object(x, y, z); return true;} else {return false;}}

		bool translate(double x, double y, double z) {if (is_locked==false) {translate_object(x, y, z); return true;} else {return false;}}

		//Rotation Quaternion
		bool rotate(double x, double y, double z, float theta, std::string device_id) {if (is_locked==false || lock_owner==device_id) {rotate_object(x, y, z, theta);return true;} else {return false;}}

		bool rotate(double x, double y, double z, float theta) {if (is_locked==false) {rotate_object(x, y, z, theta);return true;} else {return false;}}

		//Rotation Euler
		bool rotate(double x, double y, double z, std::string device_id) {if (is_locked==false || lock_owner==device_id) {rotate_object(x, y, z); return true;} else {return false;}}

		bool rotate(double x, double y, double z) {if (is_locked==false) {rotate_object(x, y, z); return true;} else {return false;}}

		//Scale
		bool resize(double x, double y, double z, std::string device_id) {if (is_locked==false || lock_owner==device_id) {scale_object(x, y, z); return true;} else {return false;}}

		bool resize(double x, double y, double z) {if (is_locked==false) {scale_object(x, y, z); return true;} else {return false;}}

		//Methods for controlling scene list
		//Not included in locks as the scene list
		//is not expected to be updated by user devices,
		//but rather by the scene module

		//Get a scene
		std::string get_scene(int index) const {return scene_list[index];}

		//Add a scene
		bool add_scene(std::string scene_id){scene_list.push_back(scene_id);return true;}

		//Remove a scene
		bool clear_scenes(){scene_list.clear(); return true;}

		//How many scenes are there?
		int num_scenes() const {return scene_list.size();}

		void set_scenes(std::vector<std::string> vec) {scene_list = vec;}

		std::vector<std::string> get_scenes() {return scene_list;}

		//Getters & Setters for string attributes

		//Set the name
		bool set_name(std::string new_name, std::string device_id){if (is_locked==false || lock_owner == device_id) {name=new_name; return true;} else {return false;}}

		bool set_name(std::string new_name){if (is_locked==false) {name=new_name; return true;} else {return false;}}

		//Set the key
		bool set_key(std::string new_key){if (is_locked==false) {key=new_key; return true;} else {return false;}}

		bool set_key(std::string new_key, std::string device_id){if (is_locked==false || lock_owner==device_id) {key=new_key; return true;} else {return false;}}

		//Set the type
		bool set_type(std::string new_type){if (is_locked==false) {type=new_type; return true;} else {return false;}}

		bool set_type(std::string new_type, std::string device_id){if (is_locked==false || lock_owner==device_id) {type=new_type; return true;} else {return false;}}

		//Set the subtype
		bool set_subtype(std::string new_subtype){if (is_locked==false) {subtype=new_subtype; return true;} else {return false;}}

		bool set_subtype(std::string new_subtype, std::string device_id){if (is_locked==false || lock_owner==device_id) {subtype=new_subtype; return true;} else {return false;}}

		//Set the owner
		bool set_owner(std::string new_owner){if (is_locked==false) {owner=new_owner; return true;} else {return false;}}

		bool set_owner(std::string new_owner, std::string device_id){if (is_locked==false || lock_owner==device_id) {owner=new_owner; return true;} else {return false;}}

		//Set the Transaction ID
		void set_transaction_id(std::string new_tran_id){app_transaction_id=new_tran_id;}

		//Set the Mesh ID
		bool set_mesh_id(std::string new_mesh){if (is_locked==false) {mesh_id=new_mesh; return true;} else {return false;}}

		bool set_mesh_id(std::string new_mesh, std::string device_id){if (is_locked==false || lock_owner==device_id) {mesh_id=new_mesh; return true;} else {return false;}}

		//message type
		bool set_message_type(int new_mes_type) {if (is_locked==false) {mes_type=new_mes_type; return true;} else {return false;}}
		bool set_message_type(int new_mes_type, std::string device_id){if (is_locked==false || lock_owner==device_id) {mes_type=new_mes_type; return true;} else {return false;}}

		//Set an error
		void set_error(std::string err_msg) {err_string = err_msg;}

		//Exist methods
		bool has_location() {return locn_flag;}
		bool has_rotatione() {return locn_flag;}
		bool has_rotationq() {return locn_flag;}
		bool has_scaling() {return locn_flag;}
		bool has_transforms() {return locn_flag;}
		bool has_bounds() {return locn_flag;}

		//Getters
    std::string get_owner() const {return owner;}
		std::string get_name() const {return name;}
		std::string get_key() {return key;}
		std::string get_type() const {return type;}
		std::string get_subtype() const {return subtype;}
		std::string get_transaction_id() const {return app_transaction_id;}
		std::string get_mesh_id() const {return mesh_id;}
		std::string get_error() const {return err_string;}
		int get_message_type() const {return mes_type;}
		double get_locx() const {return location(0);}
		double get_locy() const {return location(1);}
		double get_locz() const {return location(2);}
		double get_loc(int xyz) const {return location(xyz);}
		double get_rotex() const {return rotation_euler(0);}
    double get_rotey() const {return rotation_euler(1);}
    double get_rotez() const {return rotation_euler(2);}
    double get_rote(int xyz) const {return rotation_euler(xyz);}
		double get_rotqw() const {return rotation_quaternion(0);}
		double get_rotqx() const {return rotation_quaternion(1);}
    double get_rotqy() const {return rotation_quaternion(2);}
    double get_rotqz() const {return rotation_quaternion(3);}
    double get_rotq(int wxyz) const {return rotation_quaternion(wxyz);}
		double get_sclx() const {return scaling(0);}
    double get_scly() const {return scaling(1);}
    double get_sclz() const {return scaling(2);}
    double get_scl(int xyz) const {return scaling(xyz);}
		Eigen::Vector3d get_loc() const {return location;}
		Eigen::Vector3d get_rote() const {return rotation_euler;}
		Eigen::Vector4d get_rotq() const {return rotation_quaternion;}
		Eigen::Vector3d get_scl() const {return scaling;}
		Eigen::Matrix4d get_transform() const {return transform_matrix;}
		Eigen::MatrixXd get_bounding_box() const {return bounding_box;}

		//Lock Methods
		bool lock(std::string device_id) {is_locked=true;lock_owner=device_id;return true;}
		bool unlock(std::string device_id) {if (lock_owner==device_id) {is_locked=false;lock_owner="";return true;} else {return false;}}
		bool locked() const {if (is_locked==false) {return false;} else {return true;}}
		std::string get_lock_id() const {return lock_owner;}

		//Convert the object to JSON
		std::string to_json();
		//Convert the object to JSON Message
    std::string to_json_msg(int msg_type) const;

		//Convert the object to a protocol buffer message
		std::string to_protobuf_msg(int msg_type) const;

		//Convert the object to JSON Message
    std::string to_json_msg(int msg_type, std::string trans_id) const;

		//Convert the object to a protocol buffer message
		std::string to_protobuf_msg(int msg_type, std::string trans_id) const;
};
#endif
