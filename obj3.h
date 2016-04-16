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
#include "FastDelegate.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <iostream>
#include <vector>

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
		Eigen::Vector3f location;

		//Transform Matrix
                Eigen::Matrix4f transform_matrix;

		//Bounding Box
                Eigen::MatrixXf bounding_box;

		//Attributes that should not be referenced
		//if object is transformed by matrix directly
		//Float Matrices for rotation, & scaling
		Eigen::Vector3f rotation_euler;
		Eigen::Vector4f rotation_quaternion;
		Eigen::Vector3f scale;

		//Transform Buffers
		Eigen::Vector3f location_buffer;
                Eigen::Vector3f rotatione_buffer;
                Eigen::Vector4f rotationq_buffer;
                Eigen::Vector3f scale_buffer;
		Eigen::Matrix4f transform_buffer;

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
		void translate_object(float x, float y, float z, std::string locality);
		void rotatee_object(float x, float y, float z, std::string locality);
		void rotateq_object(float x, float y, float z, float theta, std::string locality);
		void scale_object(float x, float y, float z);
		
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
		Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner, Eigen::Vector3f ilocation)
{name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";location=ilocation;}

		//Location & Bounding Box
		Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner, Eigen::Vector3f ilocation, Eigen::MatrixXf ibounding_box)
{name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";location=ilocation;bounding_box=ibounding_box;}

		//Location, Rotation, Scale
		Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner, Eigen::Vector3f ilocation, Eigen::Vector3f irotatione, Eigen::Vector4f irotationq, Eigen::Vector3f iscale) 
{name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";location=ilocation;rotation_euler=irotatione;rotation_quaternion=irotationq;scale=iscale;}

		//Location, Rotation, Scale, Bounding Box
		Obj3(std::string iname, std::string ikey, std::string itype, std::string isubtype, std::string iowner, Eigen::Vector3f ilocation, Eigen::Vector3f irotatione, Eigen::Vector4f irotationq, Eigen::Vector3f iscale, Eigen::MatrixXf ibounding_box)
{name = iname; key = ikey; type = itype; subtype = isubtype; initialize_matrices();owner=iowner;is_locked=false; lock_owner="";location=ilocation;rotation_euler=irotatione;rotation_quaternion=irotationq;scale=iscale;bounding_box=ibounding_box;}

		//Transformation Methods

		//Transform
		bool transform_object(Eigen::Matrix4f trans_matrix);
		
		bool transform_object(float trans_matrix[]);

		//Translation
		bool translate(float x, float y, float z, std::string locality, std::string device_id) {if (is_locked==false || lock_owner==device_id) {translate_object(x, y, z, locality); return true;} else {return false;}}

		bool translate(float x, float y, float z, std::string locality) {if (is_locked==false) {translate_object(x, y, z, locality); return true;} else {return false;}}

		//Rotation Quaternion
		bool rotateq(float x, float y, float z, float theta, std::string locality, std::string device_id) {if (is_locked==false || lock_owner==device_id) {rotateq_object(x, y, z, theta, locality);return true;} else {return false;}}

		bool rotateq(float x, float y, float z, float theta, std::string locality) {if (is_locked==false) {rotateq_object(x, y, z, theta, locality);return true;} else {return false;}}

		//Rotation Euler
		bool rotatee(float x, float y, float z, std::string locality, std::string device_id) {if (is_locked==false || lock_owner==device_id) {rotatee_object(x, y, z, locality); return true;} else {return false;}}

		bool rotatee(float x, float y, float z, std::string locality) {if (is_locked==false) {rotatee_object(x, y, z, locality); return true;} else {return false;}}

		//Scale
		bool resize(float x, float y, float z, std::string device_id) {if (is_locked==false || lock_owner==device_id) {scale_object(x, y, z); return true;} else {return false;}}

		bool resize(float x, float y, float z) {if (is_locked==false) {scale_object(x, y, z); return true;} else {return false;}}
		
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
		float get_locx() const {return location(0);}
		float get_locy() const {return location(1);}
		float get_locz() const {return location(2);}
		float get_loc(int xyz) const {return location(xyz);}
		float get_rotex() const {return rotation_euler(0);}
		float get_rotey() const {return rotation_euler(1);}
		float get_rotez() const {return rotation_euler(2);}
		float get_rote(int xyz) const {return rotation_euler(xyz);}
		float get_rotqw() const {return rotation_quaternion(0);}
		float get_rotqx() const {return rotation_quaternion(1);}
		float get_rotqy() const {return rotation_quaternion(2);}
		float get_rotqz() const {return rotation_quaternion(3);}
		float get_rotq(int wxyz) const {return rotation_quaternion(wxyz);}
		float get_sclx() const {return scale(0);}
		float get_scly() const {return scale(1);}
		float get_sclz() const {return scale(2);}
		float get_scl(int xyz) const {return scale(xyz);}
		Eigen::Vector3f get_loc() const {return location;}
		Eigen::Vector3f get_rote() const {return rotation_euler;}
		Eigen::Vector4f get_rotq() const {return rotation_quaternion;}
		Eigen::Vector3f get_scl() const {return scale;}
		Eigen::Matrix4f get_transform() const {return transform_matrix;}
		Eigen::MatrixXf get_bounding_box() const {return bounding_box;}

		//Lock Methods
		bool lock(std::string device_id) {is_locked=true;lock_owner=device_id;return true;}
		bool unlock(std::string device_id) {if (lock_owner==device_id) {is_locked=false;lock_owner="";return true;} else {return false;}}
		bool locked() const {if (is_locked==false) {return false;} else {return true;}}

		//Convert the object to JSON
		const char* to_json() const;

};
#endif
