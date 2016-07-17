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

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <aossl/couchbase_admin.h>
#include <aossl/xredis_admin.h>
#include "obj3.h"
#include "configuration_manager.h"

#include <aossl/logging.h>
#include <aossl/zmqio.h>
#include "globals.h"

//-----------------------
//---Callback Methods----
//-----------------------

//Couchbase Callbacks

//The storage callback is simple, just logging the storage act and result
inline static void storage_callback(lcb_t instance, const void *cookie, lcb_storage_t op,
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
inline static void del_callback(lcb_t instance, const void *cookie, lcb_error_t err, const lcb_remove_resp_t *resp)
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
inline static void get_callback(lcb_t instance, const void *cookie, lcb_error_t err,
  const lcb_get_resp_t *resp)
  {
    if (err == LCB_SUCCESS) {
      logging->info("Retrieved: ");
      logging->info( (char*)resp->v.v0.key );
      logging->info( (char*)resp->v.v0.bytes );
      const char *k = (char*)resp->v.v0.key;
      const char *resp_obj = (char*)resp->v.v0.bytes;
      if (cm->get_smartupdatesactive()) {
        logging->debug("Smart Update Logic Activated");
        //Then, let's get and parse the response from the database
        //We need to clean the response since Couchbase gives dirty responses

        rapidjson::Document temp_d;
        Obj3 *new_obj;
        bool go_ahead=false;
        try {
          temp_d.Parse(resp_obj);
          go_ahead=true;
        }
        catch (std::exception& e) {
          //Catch a possible exception and write it to the logs
          logging->error("Exception Occurred parsing message from Couchbase");
          logging->error(e.what());
        }
        if (go_ahead) {
          new_obj = new Obj3 (temp_d);
        }
        if (new_obj)
        {
          const char *temp_key;
          std::string no_key;
          no_key = new_obj->get_key();
          if (no_key != "")
          {
            temp_key = no_key.c_str();
            logging->debug("Database Object Parsed");
            bool is_key_in_buf = xRedis->exists(temp_key);
            if (is_key_in_buf) {
              //We need to update the object in the DB, then output the object
              //On the Outbound ZeroMQ port.

              logging->debug("Object found in Smart Update Buffer");

              //Let's get the object out of the active update list

              std::string strValue = xRedis->load(temp_key);
              if (!strValue.empty()) {
                logging->debug(strValue);

                Obj3 *temp_obj;
                Obj3 tobj;

                if (cm->get_rfjson()) {

                  rapidjson::Document temp_d;
                  try {
                    temp_d.Parse(strValue.c_str());
                    go_ahead=true;
                  }
                  catch (std::exception& e) {
                    //Catch a possible exception and write it to the logs
                    logging->error("Exception Occurred parsing message from Couchbase");
                    logging->error(e.what());
                  }
                  if (go_ahead) {
                    temp_obj = new Obj3 (temp_d);
                  }

                }
                else if (cm->get_rfprotobuf()) {

                  protoObj3::Obj3 pobj;
                  try {
                    pobj.ParseFromString(strValue);
                    go_ahead=true;
                  }
                  catch (std::exception& e) {
                    //Catch a possible exception and write it to the logs
                    logging->error("Exception Occurred parsing message from Couchbase");
                    logging->error(e.what());
                  }
                  if (go_ahead) {
                    temp_obj = new Obj3 (pobj);
                  }

                }

                if (temp_obj) {

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
                    std::string nowner = temp_obj->get_owner();
                    new_obj->set_owner(nowner);
                  }

                  if (temp_obj->get_name() != "") {
                    std::string nname = temp_obj->get_name();
                    new_obj->set_name(nname);
                  }

                  if (temp_obj->get_type() != "") {
                    std::string ntype = temp_obj->get_type();
                    new_obj->set_type(ntype);
                  }

                  if (temp_obj->get_subtype() != "") {
                    std::string nsubtype = temp_obj->get_subtype();
                    new_obj->set_subtype(nsubtype);
                  }

                  std::string obj_string;

                  //output the message on the ZMQ Port
                  if (cm->get_mfjson()) {
                    obj_string = new_obj->to_json_msg(OBJ_UPD);
                  }
                  else if (cm->get_mfprotobuf()) {
                    obj_string = new_obj->to_protobuf_msg(OBJ_UPD);
                  }
                  zmqo->send_str(obj_string);
                }

                //Remove the element from the smart updbate buffer
                bool is_key_still_in_buf = xRedis->exists(temp_key);
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
              std::string object_string;
              if (cm->get_mfjson()) {
                object_string = new_obj->to_json_msg(OBJ_UPD);
              }
              else if (cm->get_mfprotobuf()) {
                object_string = new_obj->to_protobuf_msg(OBJ_UPD);
              }
              zmqo->send_str(object_string);
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
        bool go_ahead=false;
        try {
          temp_d.Parse((char*)resp->v.v0.bytes);
          go_ahead=true;
        }
        catch (std::exception& e) {
          //Catch a possible exception and write it to the logs
          logging->error("Exception Occurred parsing message from DB");
          logging->error(e.what());
        }
        if (go_ahead) {
          Obj3 *new_obj = new Obj3 (temp_d);
          std::string new_obj_str;
          if (cm->get_mfjson()) {
            new_obj_str = new_obj->to_json_msg(OBJ_GET);
          }
          else if (cm->get_mfprotobuf()) {
            new_obj_str = new_obj->to_protobuf_msg(OBJ_GET);
          }
          zmqo->send_str(new_obj_str);
          delete new_obj;
        }
      }
    }
    else {
      logging->error("Couldn't retrieve item:");
      logging->error(lcb_strerror(instance, err));
    }
  }
