//The Obj3 object acts as an ORM for the JSON Documents
//In the Database, and is loaded/saved by the respective
//implementation of the db_admin.h interface

//OBJ3.H
#ifndef OBJ3_H
#define OBJ3_H

#include <string>
#include <Eigen/Dense>
#include "list.h"
#include "event_dispatcher.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <iostream>
#include <vector>

#include "logging.h"

class Obj3
{
	private:
		//String Attributes
		std::string name;
		std::string key;
		std::string type;
		std::string subtype;
		std::string owner;

		//Externally Referenceable data
		//Float Matrix for location
		Eigen::Vector3d location;
		Eigen::Vector3d rotation_euler;
		Eigen::Vector4d rotation_quaternion;
		Eigen::Vector3d scaling;

		//Transform Matrix
                Eigen::Matrix4d transform_matrix;

		//Bounding Box
                Eigen::MatrixXd bounding_box;

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
		void translate_object(double x, double y, double z, std::string locality);
		void rotatee_object(double x, double y, double z, std::string locality);
		void rotateq_object(double x, double y, double z, double theta, std::string locality);
		void scale_object(double x, double y, double z);

	public:
		//Constructors & Destructor

		//Basic constructor with no params
		Obj3() {name = ""; key = ""; type = ""; subtype = ""; initialize_matrices(); owner=""; is_locked=false; lock_owner="";}

		//String-Only Constructors
		Obj3(std::string iname, std::string ikey){name = iname; key = ikey; type = ""; subtype = ""; initialize_matrices();owner="";is_locked=false; lock_owner="";}
		Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype) {name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner="";is_locked=false; lock_owner="";}
		Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner) {name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";}

		//Matrix Constructors
		//Location only
		Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner, Eigen::Vector3d ilocation)
{name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";location=ilocation;}

		//Location & Bounding Box
		Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner, Eigen::Vector3d ilocation, Eigen::MatrixXd ibounding_box)
{name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";location=ilocation;bounding_box=ibounding_box;}

		//Location, Transform, & Bounding Box
                Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner, Eigen::Vector3d ilocation, Eigen::Matrix4d itransform, Eigen::MatrixXd ibounding_box)
{name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";location=ilocation;transform_matrix=itransform;bounding_box=ibounding_box;}

		//Location, Rotation, Scale, Transform, & Bounding Box
                Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner, Eigen::Vector3d ilocation, Eigen::Vector3d irotatione, Eigen::Vector4d irotationq, Eigen::Vector3d iscale, Eigen::Matrix4d itransform, Eigen::MatrixXd ibounding_box)
{name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";location=ilocation;rotation_euler=irotatione;rotation_quaternion=irotationq;scaling=iscale;transform_matrix=itransform;bounding_box=ibounding_box;}

                //All elements
                Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner, std::vector<std::string> scns, Eigen::Vector3d ilocation, Eigen::Vector3d irotatione, Eigen::Vector4d irotationq, Eigen::Vector3d iscale, Eigen::Matrix4d itransform, Eigen::MatrixXd ibounding_box)
{name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";scene_list = scns;location=ilocation;rotation_euler=irotatione;rotation_quaternion=irotationq;scaling=iscale;transform_matrix=itransform;bounding_box=ibounding_box;}

		//Transformation Methods

		//Transform
		bool transform_object(Eigen::Matrix4d trans_matrix);

		bool transform_object(double trans_matrix[]);

		//Translation
		bool translate(double x, double y, double z, std::string locality, std::string device_id) {if (is_locked==false || lock_owner==device_id) {translate_object(x, y, z, locality); return true;} else {return false;}}

		bool translate(double x, double y, double z, std::string locality) {if (is_locked==false) {translate_object(x, y, z, locality); return true;} else {return false;}}

		//Rotation Quaternion
		bool rotateq(double x, double y, double z, float theta, std::string locality, std::string device_id) {if (is_locked==false || lock_owner==device_id) {rotateq_object(x, y, z, theta, locality);return true;} else {return false;}}

		bool rotateq(double x, double y, double z, float theta, std::string locality) {if (is_locked==false) {rotateq_object(x, y, z, theta, locality);return true;} else {return false;}}

		//Rotation Euler
		bool rotatee(double x, double y, double z, std::string locality, std::string device_id) {if (is_locked==false || lock_owner==device_id) {rotatee_object(x, y, z, locality); return true;} else {return false;}}

		bool rotatee(double x, double y, double z, std::string locality) {if (is_locked==false) {rotatee_object(x, y, z, locality); return true;} else {return false;}}

		//Scale
		bool resize(double x, double y, double z, std::string device_id) {if (is_locked==false || lock_owner==device_id) {scale_object(x, y, z); return true;} else {return false;}}

		bool resize(double x, double y, double z) {if (is_locked==false) {scale_object(x, y, z); return true;} else {return false;}}

		//Apply Transforms in Buffers
		void apply_transforms();

		//Methods for controlling scene list
		//Not included in locks as the scene list
		//is not expected to be updated by user devices,
		//but rather by the scene module

		//Get a scene
		std::string get_scene(int index) const {return scene_list[index];}

		//Add a scene
		bool add_scene(std::string scene_id){scene_list.push_back(scene_id);return true;}

		//Remove a scene
		bool clear_scenes(int index){scene_list.clear(); return true;}

		//How many scenes are there?
		int num_scenes() const {return scene_list.size();}

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

		//Getters
                std::string get_owner() const {return owner;}
		std::string get_name() const {return name;}
		std::string get_key() const {return key;}
		std::string get_type() const {return type;}
		std::string get_subtype() const {return subtype;}
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

		//Convert the object to JSON
		const char* to_json() const;
		//Convert the object to JSON Message
                const char* to_json_msg(int msg_type) const;
};
#endif
