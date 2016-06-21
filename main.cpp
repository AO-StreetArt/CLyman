//This is the  main structural code of the module
//It is built to allow for real-time messaging without the need
//For polling, using a Dispatcher Pattern

#include <zmq.hpp>
#include <sstream>
#include <string>
#include <string.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>
#include <exception>
#include <uuid/uuid.h>
#include <Eigen/Dense>

#include "src/event_dispatcher.h"
#include "src/obj3.h"
#include "src/couchbase_admin.h"
#include "src/xredis_admin.h"
#include "src/lyman_utils.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "src/logging.h"

//Declare our global config variables
std::string DB_ConnStr;
bool DB_AuthActive;
std::string DB_Pswd;
std::string OMQ_OBConnStr;
std::string OMQ_IBConnStr;
bool SmartUpdatesActive;
bool MessageFormatJSON;
bool MessageFormatProtoBuf;
int SUB_Duration;

struct RedisConnChain
{
  std::string ip;
  int port;
  std::string elt4;
  int elt5;
  int elt6;
  int elt7;
};

//Global Couchbase Admin Object
CouchbaseAdmin *cb;

//Global Outbound ZMQ Dispatcher
zmq::socket_t *zmqo;

//Smart Update Buffer
xRedisAdmin *xRedis;

//-----------------------
//----Utility Methods----
//-----------------------

enum {
 CACHE_TYPE_1,
 CACHE_TYPE_2,
 CACHE_TYPE_MAX,
};

void send_zmqo_message(const char * msg)
{
  //Get the size of the buffer being passed in
  int buffer_size;
  buffer_size = strlen(msg);

  //Set up the message to go out on 0MQ
  zmq::message_t req (buffer_size);
  memcpy (req.data (), msg, buffer_size);

  //Send the message
  zmqo->send (req);

  //  Get the reply.
  zmq::message_t rep;
  zmq::message_t *rep_ptr;
  //rep_ptr = &reply;
  zmqo->recv (&rep);

  //Process the reply
  std::string r_str = hexDump(rep);

  logging->info("ZMQ:Message Sent:");
  logging->info(msg);
  logging->info("ZMQ:Response Recieved:");
  logging->info(r_str);
}

void send_zmqo_str_message(std::string msg) {
  send_zmqo_message(msg.c_str());
}

//Is a key present in the smart update buffer?
bool is_key_in_smart_update_buffer(const char * key) {
	return xRedis->exists(key);
}

//Build Obj3 from a protocol buffer
Obj3* build_proto_object(protoObj3::Obj3 buffer) {
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
  std::vector<std::string> scene_list;
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
      scene_list.push_back(buffer.scenes(j));
    }
  }

  logging->debug("Variables Filled");

  //Build the Obj3 and return it from the populated values
  Obj3* object = new Obj3 (new_name, new_key, new_type, new_subtype, new_owner, scene_list, new_location, new_rotatione, new_rotationq, new_scale, new_transform, new_bounding_box);
  logging->debug("Obj3 Built");
  //new_obj.set_name(new_name);
	//new_obj.set_key(new_key);
	//new_obj.set_type(new_type);
	//new_obj.set_subtype(new_subtype);
	//new_obj.set_owner(new_owner);
	//new_obj.set_loc(new_location);
	//new_obj.set_rote(new_rotatione);
	//new_obj.set_rotq(new_rotationq);
	//new_obj.set_scl(new_scale);
	//new_obj.set_transform(new_transform);
	//new_obj.set_bounds(new_bounding_box);
	//new_obj.set_scenes(scene_list);
  return object;
}

//Build Obj3 from a Rapidjson Document
Obj3* build_object(const rapidjson::Document& d) {
  logging->debug("Build Object Called");
  if (d.IsObject()) {
    logging->debug("Object-Format Message Detected");

	//Building replacement variables
    std::string new_name="";
    std::string new_key="";
    std::string new_owner="";
    std::string new_type="";
    std::string new_subtype="";
    std::string new_lock_id="";
    std::vector<std::string> scene_list;
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
          scene_list.push_back(sc[i].GetString());
        }
      }
    }


    logging->debug("Variables Filled");

    //Build the Obj3 and return it from the populated values
    Obj3 *object = new Obj3 (new_name, new_key, new_type, new_subtype, new_owner, scene_list, new_location, new_rotatione, new_rotationq, new_scale, new_transform, new_bounding_box);
	//new_obj.set_name(new_name);
	//new_obj.set_key(new_key);
	//new_obj.set_type(new_type);
	//new_obj.set_subtype(new_subtype);
	//new_obj.set_owner(new_owner);
	//new_obj.set_loc(new_location);
	//new_obj.set_rote(new_rotatione);
	//new_obj.set_rotq(new_rotationq);
	//new_obj.set_scl(new_scale);
	//new_obj.set_transform(new_transform);
	//new_obj.set_bounds(new_bounding_box);
	//new_obj.set_scenes(scene_list);
    logging->debug("Obj3 Built");
    return object;
  }
}

//-----------------------
//---Callback Methods----
//-----------------------

//Couchbase Callbacks

//The storage callback is simple, just logging the storage act and result
static void storage_callback(lcb_t instance, const void *cookie, lcb_storage_t op,
  lcb_error_t err, const lcb_store_resp_t *resp)
  {
    if (err == LCB_SUCCESS) {
      logging->info("Stored:");
      logging->info( (char*)resp->v.v0.key );
    }
    else {
      logging->error("Couldn't store item:");
      logging->error(lcb_strerror(instance, err));
    }
  }

  //The delete callback is also simple, just logging the storage act and result
  static void del_callback(lcb_t instance, const void *cookie, lcb_error_t err, const lcb_remove_resp_t *resp)
    {
      if (err == LCB_SUCCESS) {
        logging->info("Removed:");
        logging->info( (char*)resp->v.v0.key );
      }
      else {
        logging->error("Couldn't remove item:");
        logging->error(lcb_strerror(instance, err));
      }
    }

  //The get callback has the complex logic for the smart updates
  static void get_callback(lcb_t instance, const void *cookie, lcb_error_t err,
    const lcb_get_resp_t *resp)
    {
      if (err == LCB_SUCCESS) {
        logging->info("Retrieved: ");
        logging->info( (char*)resp->v.v0.key );
        logging->info( (char*)resp->v.v0.bytes );
        const char *k = (char*)resp->v.v0.key;
        const char *resp_obj = (char*)resp->v.v0.bytes;
        if (SmartUpdatesActive) {
          logging->debug("Smart Update Logic Activated");
          //Then, let's get and parse the response from the database
		  //We need to clean the response since Couchbase gives dirty responses

          rapidjson::Document temp_d;
          temp_d.Parse(resp_obj);
		  Obj3 *new_obj = build_object (temp_d);
      if (new_obj)
      {
            const char *temp_key;
  		  std::string no_key;
  		  no_key = new_obj->get_key();
  		  if (no_key != "")
  		  {
    		  temp_key = no_key.c_str();
              logging->debug("Database Object Parsed");
              bool is_key_in_buf = is_key_in_smart_update_buffer(temp_key);
              if (is_key_in_buf) {
                //We need to update the object in the DB, then output the object
                //On the Outbound ZeroMQ port.

                logging->debug("Object found in Smart Update Buffer");

                //Let's get the object out of the active update list
                Obj3 *temp_obj;
                Obj3 tobj;

    			std::string strValue = xRedis->load(temp_key);
    			if (strValue != NULL) {
      			protoObj3::Obj3 pobj;
    			std::string stringval (strValue, strlen(strValue));
      			pobj.ParseFromString(stringval);
    			Obj3 *temp_obj = build_proto_object(pobj);

                  //tobj = smart_update_buffer[k];
                  //temp_obj = &tem_obj;

                  //Now, we can compare the two and apply any updates from the
                  //object list to the object returned from the database

                  //First, we apply any matrix transforms present
                  if (temp_obj->get_locx() > 0.0001 || temp_obj->get_locy() > 0.0001 || temp_obj->get_locz() > 0.0001) {
                    logging->debug("Location Transformation Detected");
                    new_obj->translate(temp_obj->get_locx(), temp_obj->get_locy(), temp_obj->get_locz(), "Global");
                  }

                  if (temp_obj->get_rotex() > 0.0001 || temp_obj->get_rotey() > 0.0001 || temp_obj->get_rotez() > 0.0001) {
                    logging->debug("Euler Rotation Transformation Detected");
                    new_obj->rotatee(temp_obj->get_rotex(), temp_obj->get_rotey(), temp_obj->get_rotez(), "Global");
                  }

                  if (temp_obj->get_rotqw() > 0.0001 || temp_obj->get_rotqx() > 0.0001 || temp_obj->get_rotqy() > 0.0001 || temp_obj->get_rotqz() > 0.0001) {
                    logging->debug("Quaternion Rotation Transformation Detected");
                    new_obj->rotateq(temp_obj->get_rotqw(), temp_obj->get_rotqx(), temp_obj->get_rotqy(), temp_obj->get_rotqz(), "Global");
                  }

                  if (temp_obj->get_sclx() > 0.0001 || temp_obj->get_scly() > 0.0001 || temp_obj->get_sclz() > 0.0001) {
                    logging->debug("Scale Transformation Detected");
                    new_obj->resize(temp_obj->get_sclx(), temp_obj->get_scly(), temp_obj->get_sclz());
                  }

                  logging->debug("Applying Transform Matrix and full transform stack");
                  new_obj->transform_object(temp_obj->get_transform());

                  new_obj->apply_transforms();

                  //Next, we write any string attributes
                  if (temp_obj->get_owner() != "") {
                    new_obj->set_owner(temp_obj->get_owner());
                  }

                  if (temp_obj->get_name() != "") {
                    new_obj->set_name(temp_obj->get_name());
                  }

                  if (temp_obj->get_type() != "") {
                    new_obj->set_type(temp_obj->get_type());
                  }

                  if (temp_obj->get_subtype() != "") {
                    new_obj->set_subtype(temp_obj->get_subtype());
                  }

                  //Finally, we write the result back to the database
                  //Obj3 *obj_ptr = &new_obj;

                  //And output the message on the ZMQ Port
                  if (MessageFormatJSON) {
                    send_zmqo_str_message(new_obj->to_json_msg(OBJ_UPD));
                  }
                  else if (MessageFormatProtoBuf) {
                    send_zmqo_str_message(new_obj->to_protobuf_msg(OBJ_UPD));
                  }

                  //Remove the element from the smart updbate buffer
                  bool is_key_still_in_buf = is_key_in_smart_update_buffer(temp_key);
                  if (is_key_still_in_buf) {
      			           xRedis->del(temp_key);
                  }
                  else {
                    logging->debug("Key already expired from update buffer, not deleting");
                  }
                  //smart_update_buffer.erase(k);

                  cb->save_object (new_obj);
    			  delete new_obj;
    			  delete temp_obj;
                  cb->wait();
                }
                else {
                  logging->error("Unable to load object and perform smart update");
                }
              }
              else {
                logging->error("Active Updates enabled but object not found in smart update buffer, outputting DB Object on OB ZMQ Port");
                //And output the message on the ZMQ Port
                if (MessageFormatJSON) {
                  send_zmqo_str_message(new_obj->to_json_msg(OBJ_UPD));
                }
                else if (MessageFormatProtoBuf) {
                  send_zmqo_str_message(new_obj->to_protobuf_msg(OBJ_UPD));
                }
              }
            }
            else {
              logging->error("No Key Found in DB Object");
            }
          }
          else {
            logging->error("Null Pointer Object detected from DB");
          }
        }
        else {
          //Output the object on the Outbound ZeroMQ port
          rapidjson::Document temp_d;
          try {
            temp_d.Parse((char*)resp->v.v0.bytes);
          }
          catch (std::exception& e) {
            //Catch a possible exception and write it to the logs
            logging->error("Exception Occurred parsing message from DB");
            logging->error(e.what());
          }
		  Obj3 *new_obj = build_object (temp_d);
          if (MessageFormatJSON) {
            send_zmqo_str_message(new_obj->to_json_msg(OBJ_GET));
          }
          else if (MessageFormatProtoBuf) {
            send_zmqo_str_message(new_obj->to_protobuf_msg(OBJ_GET));
          }
		  delete new_obj;
        }
      }
      else {
        logging->error("Couldn't retrieve item:");
        logging->error(lcb_strerror(instance, err));
      }
    }

    //Callbacks for the main loop to hit directly

    //Document Event Callbacks

    //Global Object Creation
    void cr_obj_global(Obj3 *new_obj) {

	  //Generate a new key for the object
	  int uuid_gen_result = 0;
	  uuid_t uuid;
	  uuid_gen_result = uuid_generate_time_safe(uuid);

	  if (uuid_gen_result == -1) {
		logging->error("UUID Generated in an unsafe manner that exposes a potential security risk");
		logging->error("http://linux.die.net/man/3/uuid_generate");
		logging->error("Please take the needed actions to allow uuid generation with a safe generator");
	  }

	  char uuid_str[37];
	  uuid_unparse_lower(uuid, uuid_str);

	  //Set the new key on the new object
	  new_obj->set_key(uuid_str);

      //Output a message on the outbound ZMQ Port
      if (MessageFormatJSON) {
        send_zmqo_str_message(new_obj->to_json_msg(OBJ_CRT));
      }
      else if (MessageFormatProtoBuf) {
        send_zmqo_str_message(new_obj->to_protobuf_msg(OBJ_CRT));
      }

      //Save the object to the couchbase DB
      cb->create_object (new_obj);
	  cb->wait();
    }

    //Create Object from a Rapidjson Document
    void create_objectd(rapidjson::Document& d) {
      logging->info("Create object called with document: ");

      if (d.HasMember("location") && d.HasMember("bounding_box") && d.HasMember("scenes")) {

        //Build the object and the key
		Obj3 *new_obj = build_object (d);
        cr_obj_global(new_obj);
      }
      else {
        logging->error("Create Message recieved without location, bounding box, or scene");
      }
    }

    //Create Object from a Protobuffer object
    void create_objectpb(protoObj3::Obj3 p_obj) {
      logging->info("Create object called with buffer: ");

      if (p_obj.has_location() && p_obj.has_bounding_box() && p_obj.scenes_size() > 0) {

        //Build the object and the key
		Obj3 *new_obj = build_proto_object (p_obj);
        cr_obj_global(new_obj);
      }
      else {
        logging->error("Create Message recieved without location, bounding box, or scene");
        logging->debug(p_obj.key());
        logging->debug(p_obj.name());
        for (int m = 0; m < p_obj.scenes_size(); ++m)
        {
          logging->debug(p_obj.scenes(m));
        }
      }
    }

    //Global Update Object
    void upd_obj_global(Obj3 *temp_obj) {
      if (SmartUpdatesActive) {
        //We start by writing the object into the smart update buffer
        //then, we can issue a get call

        //upon returning, the get callback should
        //check the smart update buffer for a matching key.

        //If it is found, we update the DB Entry.
        //Else, we simply output the value retrieved from the DB

        //Check if the object already exists in the smart update buffer.
        //If so, reject the update.
        const char * temp_key = temp_obj->get_key().c_str();
        if (is_key_in_smart_update_buffer(temp_key) == false) {
		  bool bRet = xRedis->save(temp_key, temp_obj->to_protobuf_msg(OBJ_UPD));
		  if (!bRet) {
			logging->error("Error putting object to Redis Smart Update Buffer");
		  }
		  bool bRet2 = xRedis->expire(temp_key, SUB_Duration);
		  if (!bRet2) {
			logging->error("Error expiring object in Redis Smart Update Buffer");
		  }
          cb->load_object(temp_key);
          cb->wait();
        }
        else {
          logging->error("Collision in Active Update Buffer Detected");
		  std::string strValue;
		  strValue = xRedis->load(temp_key);
		  protoObj3::Obj3 pobj;
		  pobj.ParseFromString(strValue);
		  Obj3 *sub_obj = build_proto_object(pobj);
          logging->error(sub_obj->to_json());
		  delete sub_obj;
        }
      }
      else {
        //If smart updates are disabled, we can just write the value directly
        //To the DB
        //Obj3 *obj_ptr = &temp_obj;

        if (MessageFormatJSON) {
          send_zmqo_str_message(temp_obj->to_json_msg(OBJ_UPD));
        }
        else if (MessageFormatProtoBuf) {
          send_zmqo_str_message(temp_obj->to_protobuf_msg(OBJ_UPD));
        }

        cb->save_object (temp_obj);
		delete temp_obj;
		cb->wait();
      }
    }

    //Update Object called with a Rapidjson Document
    void update_objectd(rapidjson::Document& d) {
      logging->info("Update object called with document: ");
      if (d.HasMember("key")) {
	    Obj3 *temp_obj = build_object (d);
        upd_obj_global(temp_obj);
      }
    }

    //Update Object called with a Protobuffer object
    void update_objectpb(protoObj3::Obj3 p_obj) {
      logging->info("Update object called with buffer: ");
      if (p_obj.has_key()) {
	    Obj3 *temp_obj = build_proto_object (p_obj);
        upd_obj_global(temp_obj);
      }
    }

    //Get Object Global
    void get_obj_global(std::string rk_str) {
      const char * rkc_str = rk_str.c_str();
      //Check the Active Update Buffer for inflight transactions
      //If we have any, then we should pull the value from there
      //And return it.
    //   if (SmartUpdatesActive) {
    //     //int key_index = find_key_in_active_updates(rkey->GetString());
    //     if (!is_update)
    //     {
    //
    //     }
    //     else {
    //     if (is_key_in_smart_update_buffer(rkc_str)) {
    //
    //           //Pull the value from the update buffer
    // 		  const char * strValue = xRedis->load(rkc_str);
    // 		  if (strValue != NULL)
    // 		  {
    // 		  protoObj3::Obj3 pobj;
    //  		  pobj.ParseFromString(strValue);
    // 		  Obj3 *tobj = build_proto_object(pobj);
    //
    //           //Return the object on the outbound ZMQ Port
    //           if (MessageFormatJSON) {
    //             send_zmqo_str_message(tobj->to_json_msg(OBJ_UPD));
    //           }
    //           else if (MessageFormatProtoBuf) {
    //             send_zmqo_str_message(tobj->to_protobuf_msg(OBJ_UPD));
    //           }
    // 		  }
    // 		  else {
    // 			logging->error("Unable to load object from Redis Cache, trying to retrieve from DB");
    // 			cb->load_object(rkc_str);
    // 			cb->wait();
    // 		  }
    //     }
    //     else {
    //       //Otherwise, Get the object from the DB
    //       cb->load_object( rkc_str );
		//       cb->wait();
    //     }
    //   }
    // }
    //   else {
        //Otherwise, Get the object from the DB
        cb->load_object( rkc_str );
		cb->wait();
      //}
    }

    //Get object Protobuffer
    void get_objectpb(protoObj3::Obj3 p_obj) {
      logging->info("Get object called with buffer: ");
      if (p_obj.has_key()) {
        std::string rk_str = p_obj.key();
        get_obj_global(rk_str);
      }
      else {
        logging->error("Message Recieved without key");
      }
    }

    //Get Object Rapidson Document
    void get_objectd(rapidjson::Document& d) {
      logging->info("Get object called with document: ");
      if (d.HasMember("key")) {
        rapidjson::Value *rkey;
        rkey = &d["key"];
        //Check the Active Update Buffer for inflight transactions
        //If we have any, then we should pull the value from there
        //And return it.
        std::string rk_str = rkey->GetString();
        get_obj_global(rk_str);
      }
      else {
        logging->error("Message Recieved without key");
      }
    }

    //Delete Object Global
    void del_obj_global(std::string key) {
      const char * kc_str = key.c_str();

      //Output a delete message on the outbound ZMQ Port

      logging->debug("Building Delete Message");

      Obj3 obj;
      obj.set_key(key);

      //Return the object on the outbound ZMQ Port
      if (MessageFormatJSON) {
        send_zmqo_str_message(obj.to_json_msg(OBJ_DEL));
      }
      else if (MessageFormatProtoBuf) {
        send_zmqo_str_message(obj.to_protobuf_msg(OBJ_DEL));
      }

      cb->delete_object( kc_str );
	    cb->wait();
    }

    //Delete Object Protobuffer
    void delete_objectpb(protoObj3::Obj3 p_obj) {
      logging->info("Delete object called with buffer: ");
      if (p_obj.has_key()) {
        std::string k = p_obj.key();
        del_obj_global(k);
      }
      else {
        logging->error("Message Recieved without key");
      }
    }

    //Delete Object Rapidjson Document
    void delete_objectd(rapidjson::Document& d) {
      logging->info("Delete object called with document: ");
      if (d.HasMember("key")) {
        rapidjson::Value *val;
        val = &d["key"];
        del_obj_global(val->GetString());
      }
      else {
        logging->error("Message Recieved without key");
      }
    }

    //-----------------------
    //------Main Method------
    //-----------------------

    int main()
    {
	  //Set up the Redis Connection List to catch values from config files
	  std::vector<RedisConnChain> RedisConnectionList;

      //Set up logging
      //This reads the logging configuration file
      std::string initFileName = "log4cpp.properties";
      try {
        log4cpp::PropertyConfigurator::configure(initFileName);
      }
      catch ( log4cpp::ConfigureFailure &e ) {
        std::cout << "[log4cpp::ConfigureFailure] caught while reading" << initFileName << std::endl;
        std::cout << e.what();
        exit(1);
      }

      log4cpp::Category& root = log4cpp::Category::getRoot();

      log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("sub1"));

      log4cpp::Category& log = log4cpp::Category::getInstance(std::string("sub1.log"));

      logging = &log;

      //Read the application configuration file

      //Default Values for Configuration Variables
      DB_ConnStr="";
      DB_AuthActive=false;
      DB_Pswd="";
      OMQ_OBConnStr="";
      OMQ_IBConnStr="";
      SmartUpdatesActive=false;
      MessageFormatJSON=true;
      MessageFormatProtoBuf=false;

      //Open the file
      logging->info("Opening lyman.properties");
      std::string line;
      std::ifstream file ("lyman.properties");

      if (file.is_open()) {
        while (getline (file, line) ) {
          //Read a line from the property file
          logging->debug("Line read from configuration file:");
          logging->debug(line);

          //Figure out if we have a blank or comment line
          bool keep_going = true;
          if (line.length() > 0) {
            if (line[0] == '/' && line[1] == '/') {
              keep_going=false;
            }
          }
          else {
            keep_going=false;
          }

          if (keep_going==true) {
            int eq_pos = line.find("=", 0);
            std::string var_name = line.substr(0, eq_pos);
            std::string var_value = line.substr(eq_pos+1, line.length() - eq_pos);
            logging->debug(var_name);
            logging->debug(var_value);
            if (var_name=="DB_ConnectionString") {
              DB_ConnStr=var_value;
            }
			if (var_name=="Smart_Update_Buffer_Duration") {
			  SUB_Duration=std::stoi(var_value);
			}
            else if (var_name=="DB_AuthenticationActive") {
              if (var_value=="True") {
                DB_AuthActive=true;
              }
              else {
                DB_AuthActive=false;
              }
            }
            else if (var_name=="DB_Password") {
              DB_Pswd=var_value;
            }
            else if (var_name=="0MQ_OutboundConnectionString") {
              OMQ_OBConnStr = var_value;
            }
            else if (var_name=="0MQ_InboundConnectionString") {
              OMQ_IBConnStr = var_value;
            }
            else if (var_name=="SmartUpdatesActive") {
              if (var_value=="True") {
                SmartUpdatesActive=true;
              }
              else {
                SmartUpdatesActive=false;
              }
            }
            else if (var_name=="MessageFormat") {
              if (var_value=="json") {
                MessageFormatJSON=true;
                MessageFormatProtoBuf=false;
              }
              else if (var_value=="protocol-buffer") {
                MessageFormatJSON=false;
                MessageFormatProtoBuf=true;
              }
            }
			else if (var_name=="RedisConnectionString") {
			  //Read a string in the format 127.0.0.1--7000----2--5--0
			  RedisConnChain chain;

			  //Retrieve the first value
			  int spacer_position = var_value.find("--", 0);
			  std::string str1 = var_value.substr(0, spacer_position);
        logging->debug("IP Address Recovered");
        logging->debug(str1);
			  chain.ip = str1;

			  //Retrieve the second value
			  std::string new_value = var_value.substr(spacer_position+2, var_value.length() - 1);
        logging->debug("New Search String");
        logging->debug(new_value);
			  spacer_position = new_value.find("--", 0);
			  str1 = new_value.substr(0, spacer_position);
        logging->debug("Port Recovered");
        logging->debug(str1);
			  chain.port = std::stoi(str1);

			  //Retrieve the third value
			  new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
        logging->debug("New Search String");
        logging->debug(new_value);
			  spacer_position = new_value.find("--", 0);
			  str1 = new_value.substr(0, spacer_position);
        logging->debug("Password Recovered");
        logging->debug(str1);
			  chain.elt4 = str1;

			  //Retrieve the fourth value
			  new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
        logging->debug("New Search String");
        logging->debug(new_value);
			  spacer_position = new_value.find("--", 0);
			  str1 = new_value.substr(0, spacer_position);
        logging->debug("Value Recovered");
        logging->debug(str1);
			  chain.elt5 = std::stoi(str1);

			  //Retrieve the fifth value
			  new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
        logging->debug("New Search String");
        logging->debug(new_value);
  			  spacer_position = new_value.find("--", 0);
  			  str1 = new_value.substr(0, spacer_position);
          logging->debug("Value Recovered");
          logging->debug(str1);
			  chain.elt6 = std::stoi(str1);

			  //Retrieve the final value
			  new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
        logging->debug("New Search String");
        logging->debug(new_value);
			  spacer_position = new_value.find("--", 0);
			  str1 = new_value.substr(0, spacer_position);
        logging->debug("Value Recovered");
        logging->debug(str1);
			  chain.elt7 = std::stoi(str1);

			  RedisConnectionList.push_back(chain);
			}
          }
        }
        file.close();
      }

      //Set up internal variables
      int current_event_type;
      int msg_type;
      rapidjson::Document d;
      rapidjson::Value *s;
      char resp[8]={'n','i','l','r','e','s','p','\0'};
      logging->info("Internal Variables Intialized");
      protoObj3::Obj3 new_proto;

	  //Set up our Redis Connection List
	  int conn_list_size = RedisConnectionList.size();
	  RedisNode RedisList1[conn_list_size];
	  {
		int y = 0;
		for (int y = 0; y < conn_list_size; ++y)
		{
		  //Pull the values from RedisConnectionList
		  RedisNode redis_n;
		  redis_n.dbindex = y;
		  RedisConnChain redis_chain = RedisConnectionList[y];
		  redis_n.host = redis_chain.ip.c_str();
		  redis_n.port = redis_chain.port;
		  redis_n.passwd = redis_chain.elt4.c_str();
		  redis_n.poolsize = redis_chain.elt5;
		  redis_n.timeout = redis_chain.elt6;
		  redis_n.role = redis_chain.elt7;
      logging->debug("Line added to Redis Configuration List with IP:");
      logging->debug(redis_n.host);

		  RedisList1[y] = redis_n;
		}
	  }
    logging->info("Redis Connection List Built");

	  //Set up Redis Connection
	  if (SmartUpdatesActive) {
	  	//xRedisAdmin x (RedisList1, conn_list_size);
		//xRedis = &x;
		xRedis = new xRedisAdmin (RedisList1, conn_list_size);
		//xRedis.ConnectRedisCache(RedisList2, 5, CACHE_TYPE_2);
		logging->info("Connected to Redis");
		}

      //Set up the Couchbase Connection
      //CouchbaseAdmin c ( DB_ConnStr.c_str() );
      //cb = &c;
	  cb = new CouchbaseAdmin ( DB_ConnStr.c_str() );
      logging->info("Connected to Couchbase DB");

      //Bind Couchbase Callbacks
      lcb_set_store_callback(cb->get_instance(), storage_callback);
      lcb_set_get_callback(cb->get_instance(), get_callback);
      lcb_set_remove_callback(cb->get_instance(), del_callback);

      zmq::context_t context(1, 2);

      //Set up the outbound ZMQ Client
      //zmq::socket_t zout(context, ZMQ_REQ);
	  zmqo = new zmq::socket_t (context, ZMQ_REQ);
      logging->info("0MQ Constructor Called");
      zmqo->connect(OMQ_OBConnStr);
      //zmqo = &zout;
      logging->info("Connected to Outbound OMQ Socket");

      //Connect to the inbound ZMQ Socket
      zmq::socket_t socket(context, ZMQ_REP);
      socket.bind(OMQ_IBConnStr);
      logging->info("ZMQ Socket Open, opening request loop");


      //Main Request Loop


      while (true) {
        zmq::message_t request;

        //  Wait for next request from client
        socket.recv (&request);
        logging->info("Request Recieved");

        //Convert the OMQ message into a string to be passed on the event
        std::string req_string;
        req_string = left_trim_string (hexDump (request));
        const char * req_ptr = req_string.c_str();
        logging->debug("Conversion to C String performed with result: ");
        logging->debug(req_ptr);

        if (MessageFormatJSON) {
          //Process the message header and set current_event_type
          try {
            d.Parse(req_ptr);
          }
          catch (std::exception& e) {
            logging->error("Exception occurred while parsing inbound document:");
            logging->error(e.what());
          }
          //Catch a possible error and write to logs
          s = &d["message_type"];
          msg_type = s->GetInt();
        }
        else if (MessageFormatProtoBuf) {
          try {
            new_proto.Clear();
            new_proto.ParseFromString(req_string);
          }
          catch (std::exception& e) {
            logging->error("Exception occurred while parsing inbound document:");
            logging->error(e.what());
          }
          //Catch a possible error and write to logs
          msg_type = new_proto.message_type();
        }

        if (msg_type == OBJ_UPD) {
          current_event_type=OBJ_UPD;
          logging->debug("Current Event Type set to Object Update");
        }
        else if (msg_type == OBJ_CRT) {
          current_event_type=OBJ_CRT;
          logging->debug("Current Event Type set to Object Create");
        }
        else if (msg_type == OBJ_GET) {
          current_event_type=OBJ_GET;
          logging->debug("Current Event Type set to Object Get");
        }
        else if (msg_type == OBJ_DEL) {
          current_event_type=OBJ_DEL;
          logging->debug("Current Event Type set to Object Delete");
        }
        //Shutdown Message
        else if (msg_type == 999) {
          end_log();

          //Delete objects off the heap
		  delete xRedis;
		  delete cb;
		  delete zmqo;

          resp[0]='s';
          resp[1]='u';
          resp[2]='c';
          resp[3]='c';
          resp[4]='e';
          resp[5]='s';
          resp[6]='s';

          //  Send reply back to client
          zmq::message_t reply (8);

          //Prepare return data
          memcpy (reply.data (), resp, 8);
          //Send the response
          socket.send (reply);
          // Optional:  Delete all global objects allocated by libprotobuf.
          google::protobuf::ShutdownProtobufLibrary();
          logging->debug("Response Sent, terminating");
          return 0;
        }
        else {
          current_event_type=-1;
          logging->error("Current Event Type not found");
        }


        //Emit an event based on the event type & build the response message
        if (current_event_type==OBJ_UPD) {
          resp[0]='s';
          resp[1]='u';
          resp[2]='c';
          resp[3]='c';
          resp[4]='e';
          resp[5]='s';
          resp[6]='s';
          logging->debug("Object Update Event Emitted, response:");
          logging->debug(resp);
          //  Send reply back to client
          zmq::message_t reply (8);

          //Prepare return data
          memcpy (reply.data (), resp, 8);

          //Send the response
          socket.send (reply);
          logging->debug("Response Sent");
          if (MessageFormatJSON) {
            update_objectd( d );
          }
          else if (MessageFormatProtoBuf) {
            update_objectpb(new_proto);
          }

        }
        else if (current_event_type==OBJ_CRT) {
          resp[0]='s';
          resp[1]='u';
          resp[2]='c';
          resp[3]='c';
          resp[4]='e';
          resp[5]='s';
          resp[6]='s';
          logging->debug("Object Create Event Emitted, response: ");
          logging->debug(resp);
          //  Send reply back to client
          zmq::message_t reply (8);

          //Prepare return data
          memcpy (reply.data (), resp, 8);

          //Send the response
          socket.send (reply);
          logging->debug("Response Sent");
          if (MessageFormatJSON) {
            create_objectd( d );
          }
          else if (MessageFormatProtoBuf) {
            create_objectpb(new_proto);
          }
        }
        else if (current_event_type==OBJ_GET) {
          resp[0]='s';
          resp[1]='u';
          resp[2]='c';
          resp[3]='c';
          resp[4]='e';
          resp[5]='s';
          resp[6]='s';
          logging->debug("Object Get Event Emitted, response: ");
          logging->debug(resp);
          //  Send reply back to client
          zmq::message_t reply (8);

          //Prepare return data
          memcpy (reply.data (), resp, 8);

          //Send the response
          socket.send (reply);
          logging->debug("Response Sent");
          if (MessageFormatJSON) {
            get_objectd( d );
          }
          else if (MessageFormatProtoBuf) {
            get_objectpb (new_proto);
          }
        }
        else if (current_event_type==OBJ_DEL) {
          resp[0]='s';
          resp[1]='u';
          resp[2]='c';
          resp[3]='c';
          resp[4]='e';
          resp[5]='s';
          resp[6]='s';
          logging->debug("Object Delete Event Emitted, response: ");
          logging->debug(resp);
          //  Send reply back to client
          zmq::message_t reply (8);

          //Prepare return data
          memcpy (reply.data (), resp, 8);

          //Send the response
          socket.send (reply);
          logging->debug("Response Sent");
          if (MessageFormatJSON) {
            delete_objectd( d );
          }
          else if (MessageFormatProtoBuf) {
            delete_objectpb(new_proto);
          }
        }
        else
        {
          resp[0]='f';
          resp[1]='a';
          resp[2]='i';
          resp[3]='l';
          resp[4]='u';
          resp[5]='r';
          resp[6]='e';
          logging->error("Object Event not Emitted, response: ");
          logging->error(resp);
          //  Send reply back to client
          zmq::message_t reply (8);

          //Prepare return data
          memcpy (reply.data (), resp, 8);

          //Send the response
          socket.send (reply);
          logging->debug("Response Sent");
        }
      }

      return 0;
    }
