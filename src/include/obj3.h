#include <string>
#include <vector>
#include <exception>
#include "transforms.h"
#include "Obj3.pb.h"
#include "app_log.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#ifndef OBJ3
#define OBJ3

//! An Obj3 Exception

//! A child class of std::exception
//! which holds error information
struct Obj3Exception: public std::exception
{
  //! An error message passed on initialization
  std::string int_msg;
  const char * int_msg_cstr;

  //! Create a Neo4j Exception, and store the given error message
  Obj3Exception (std::string msg) {int_msg = "Error in Object: " + msg;int_msg_cstr = int_msg.c_str();}

  Obj3Exception () {}
  ~Obj3Exception() throw () {}
  //! Show the error message in readable format
  const char * what() const throw ()
  {
    return int_msg_cstr;
  }
};

class Obj3 {
  //String attributes
  std::string key;
  std::string name;
  std::string scene_id;
  std::string type;
  std::string subtype;
  std::string owner;
  //Transformation attributes
  Translation *trans = NULL;
  EulerRotation *erot = NULL;
  QuaternionRotation *qrot = NULL;
  Scale *scl = NULL;
  //Vector attributes
  std::vector<std::string> asset_ids;
  //String Return Value
  const char* json_cstr_val;
  std::string json_str_val;
public:
  //Constructors
  Obj3();
  //Parse a JSON document from Mongo
  Obj3(const rapidjson::Document &d);
  //Copy Constructor
  Obj3(const Obj3 &o);
  //Destructor
  ~Obj3();
  //String Getters
  std::string get_key() const {return key;}
  std::string get_name() const {return name;}
  std::string get_scene() const {return scene_id;}
  std::string get_type() const {return type;}
  std::string get_subtype() const {return subtype;}
  std::string get_owner() const {return owner;}
  //String Setters
  void set_key(std::string new_key) {key.assign(new_key);}
  void set_name(std::string new_name) {name.assign(new_name);}
  void set_scene(std::string new_scene) {scene_id.assign(new_scene);}
  void set_type(std::string new_type) {type.assign(new_type);}
  void set_subtype(std::string new_subtype) {subtype.assign(new_subtype);}
  void set_owner(std::string new_owner) {owner.assign(new_owner);}
  //Asset methods
  int num_assets() const {return asset_ids.size();}
  void add_asset(std::string id) {asset_ids.push_back(id);}
  std::string get_asset(int index) const {return asset_ids[index];}
  void remove_asset(int index) {asset_ids.erase(asset_ids.begin()+index);}
  void clear_assets() {asset_ids.clear();}
  //Transform methods
  void transform(Transformation *t);
  bool has_translation() const {if (trans) {return true;} else {return false;}}
  Translation* get_translation() const {return trans;}
  bool has_erotation() const {if (erot) {return true;} else {return false;}}
  EulerRotation* get_erotation() const {return erot;}
  bool has_qrotation() const {if (qrot) {return true;} else {return false;}}
  QuaternionRotation* get_qrotation() const {return qrot;}
  bool has_scale() const {if (scl) {return true;} else {return false;}}
  Scale* get_scale() const {return scl;}
  //Take a target object and apply it's fields as changes to this Object
  void merge(Obj3 *target);
  //to_json method to build an object to save to Mongo
  std::string to_json();
};

#endif
