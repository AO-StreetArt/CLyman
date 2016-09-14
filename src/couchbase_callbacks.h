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
#include <Eigen/Dense>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "obj3.h"
#include "configuration_manager.h"
#include "lyman_log.h"
#include "lyman_utils.h"

#include "aossl/factory/logging_interface.h"
#include "aossl/factory/response_interface.h"
#include "aossl/factory/callbacks.h"
#include "globals.h"

//-----------------------
//-----Base Methods------
//-----------------------

//Parse the Redis Response string into the provided Obj3, and return the message type
//Returns a message type of -1 in case of failure (ie. object not found in redis)
inline Obj3* set_redis_response_object(Request *r, std::string response_key)
{
  Obj3 *redis_object = NULL;
  callback_logging->debug("Setting up Rapidjson Objects");
  rapidjson::Document temp_d2;

  //Check Redis for transaction information
  callback_logging->debug("Checking if the Redis transaction exists");
  if ( !(xRedis->exists(response_key.c_str())) ) {
    callback_logging->error("Callback Returned with no Redis Information");
  }
  else
  {
    callback_logging->debug("Object found in Smart Update Buffer");
    //Let's get the object out of the active update list
    std::string strValue = xRedis->load(response_key.c_str());
    if (strValue.empty()) {
      callback_logging->error("Unable to load object from Redis");
    }
    else
    {
      //Are we writing to redis with json?
      if (cm->get_rfjson()) {
        callback_logging->debug("Parsing a JSON Object from Redis");
        try {
          temp_d2.Parse(strValue.c_str());
          redis_object = new Obj3 (temp_d2);
        }
        catch (std::exception& e) {
          callback_logging->error("Exception Occurred parsing message from Couchbase");
          callback_logging->error(e.what());
        }
      }

      //Or are we writing to redis with Protocol buffers?
      else if (cm->get_rfprotobuf()) {
        callback_logging->debug("Parsing a Protocol Buffer Object from Redis");
        protoObj3::Obj3 pobj;
        try {
          pobj.ParseFromString(strValue);
          redis_object = new Obj3 (pobj);
        }
        catch (std::exception& e) {
          callback_logging->error("Exception Occurred parsing message from Couchbase");
          callback_logging->error(e.what());
        }
      }
    }
  }
  return redis_object;
}

//Return an Obj3 pointer built from the json string passed in, assumed to be from the DB
inline Obj3* set_db_response_object(std::string object_string)
{
  callback_logging->debug("Parsing DB Object");
  rapidjson::Document temp_d;
  if (!object_string.empty()) {
    //Process the DB Object
    try {
      temp_d.Parse(object_string.c_str());
      return new Obj3 (temp_d);
    }
    catch (std::exception& e) {
      callback_logging->error("Exception Occurred parsing message from DB");
      callback_logging->error(e.what());
    }
  }
  return NULL;
}

//create an error response and return the string
inline std::string create_error_response(int msg_type, std::string transaction_id, std::string response_key, std::string resp_err_string)
{
  callback_logging->debug("Creating an Error Response");
  std::string object_string = "";
  Obj3 *new_obj = new Obj3;
  new_obj->set_key(response_key);
  new_obj->set_error(resp_err_string);

  //Build the outbound message
  if (cm->get_mfjson()) {
    object_string = new_obj->to_json_msg(msg_type, transaction_id);
  }
  else if (cm->get_mfprotobuf()) {
    object_string = new_obj->to_protobuf_msg(msg_type, transaction_id);
  }

  delete new_obj;
  return object_string;
}

//Create a standard response and return the string
inline std::string create_response(Obj3 *obj, int msg_type, std::string transaction_id)
{
  std::string object_string = "";
  if (cm->get_mfjson()) {
    object_string = obj->to_json_msg(msg_type, transaction_id);
  }
  else if (cm->get_mfprotobuf()) {
    object_string = obj->to_protobuf_msg(msg_type, transaction_id);
  }
  return object_string;
}

//Create a response without transaction ID's and return the string
inline std::string create_notran_response(Obj3 *obj, int msg_type)
{
  std::string object_string = "";
  if (cm->get_mfjson()) {
    object_string = obj->to_json_msg(msg_type);
  }
  else if (cm->get_mfprotobuf()) {
    object_string = obj->to_protobuf_msg(msg_type);
  }
  return object_string;
}

//Send an outbound message and return the response
inline std::string send_outbound_msg(std::string message_string)
{
  std::string out_resp = "";
  if (!message_string.empty()) {
    //output the message on the ZMQ Port
    callback_logging->debug("Sending Outbound Message:");
    callback_logging->debug(message_string);
    zmqo->send(message_string);

    //Get the response
    out_resp = zmqo->recv();
  }
  return out_resp;
}

inline std::string perform_smart_update(Obj3 *redis_object, Obj3 *db_object, std::string object_string, int msg_type, int callback_type, std::string transaction_id)
{

  //We have a get message
  if (cm->get_smartupdatesactive())
  {
    //Smart Updates are active, and we have an update message type on a get callback (which triggers the smart update logic)
    if (msg_type == OBJ_UPD && callback_type == OBJ_GET)
    {
      //We have an update message
      //We need to update the object in the DB, then output the object
      //On the Outbound ZeroMQ port.
      callback_logging->debug("Smart Update Logic Activated");
      //Now, we can compare the two and apply any updates from the
      //object list to the object returned from the database
      db_object->transform( redis_object );
      std::string key_string = db_object->get_key();

      //Remove the element from the smart updbate buffer
      if (xRedis->exists(key_string.c_str())) {
        xRedis->del(key_string.c_str());
      }

      //Replace the element in the smart update buffer
      dm->put_to_redis(redis_object, OBJ_UPD, transaction_id);

      //Save the resulting object back to the DB
      cb->save_object (redis_object);
      cb->wait();
      return "";
    }
  }
  return object_string;
}

inline std::string default_callback (Request *r, int inp_msg_type)
{
  callback_logging->info("Default Couchbase Callback Triggered");

  //Build base objects
  Obj3 *new_obj = NULL;
  Obj3 *db_object = NULL;
  std::string object_string;
  int *msg_type = NULL;
  int message_type = -1;
  std::string transaction_id = "";
  std::string out_resp = "";
  rapidjson::Document d;
  std::string resp_err_string;
  std::string obj_string;
  std::string response_key;

  callback_logging->debug("Pulling Request Data");

  //Get the Object String from the Request Data
  if (inp_msg_type == OBJ_GET) {
    obj_string = r->req_data;
  }
  else {
    obj_string = r->req_addr;
  }

  //Clean the object string from the DB
  std::size_t obj_char_position = obj_string.find("{");
  if (obj_char_position != std::string::npos && obj_char_position != 0) {
    obj_string = obj_string.substr(obj_string.find("{"), obj_string.length());
  }

  std::size_t obj_char_position = obj_string.find("}");
  if (obj_char_position != std::string::npos && obj_char_position != obj_string.length() {
    obj_string = obj_string.substr(0, obj_char_position);
  }

  //Actions when the storage operation is successful
  if (r->req_err->err_code == NOERROR)
  {

    //When we have a create message, we can take the response string and parse it to find our DB Object
    if (inp_msg_type == OBJ_CRT) {
      callback_logging->debug("Stored:");
      db_object = set_db_response_object(obj_string);
    }
    else if (inp_msg_type == OBJ_DEL) {
      callback_logging->debug("Deleted:");
    }
    else if (inp_msg_type == OBJ_GET) {
      callback_logging->debug("Retrieved:");
      db_object = set_db_response_object(obj_string);
    }
    callback_logging->debug(obj_string);

    //Determine if we have a DB Response Object.  If not, then we either have an error
    //Or we have a delete message
    if (!db_object)
    {
      callback_logging->error("Null DB Response Detected");
    }
    else
    {
      if (cm->get_transactionidsactive()) {
        if (inp_msg_type == OBJ_CRT) {
          response_key = db_object->get_key();
        }
      }
      //Transaction ID's are inactive
      else
      {
        int message_type = inp_msg_type;
        object_string = create_notran_response(db_object, message_type);
      }
    }

    callback_logging->debug("Checking Redis for transaction information on key:");
    callback_logging->debug(response_key);

    //Check Redis for transaction information
    if (!response_key.empty()) {
      if (cm->get_transactionidsactive()) {
        new_obj = set_redis_response_object(r, response_key);

        if (!new_obj) {
          callback_logging->debug("No Redis Response Object found");
        }
        else {
          if (new_obj->get_message_type() == -1) {
            callback_logging->debug("No Message Type found");
          }
          else {
            message_type = new_obj->get_message_type();
          }
        }
      }
    }
    else
    {
      callback_logging->debug("No Response Key Detected");
      message_type = ERR;
      resp_err_string = "No Response Key Detected";
    }

    //If the Redis update failed, set the message type back to error
    if (message_type == -1)
    {
      callback_logging->error("Redis Update Failed");
      message_type = ERR;
      resp_err_string = "Redis Update Failed";
    }

    //either the Redis update was successful or never done
    else if (message_type != ERR)
    {
      //redis update never done
      if (!new_obj)
      {
        callback_logging->error("Redis Update not performed");
      }

      //Redis update successful
      else
      {
        //Use the Couchbase message to populate transaction ID
        transaction_id = new_obj->get_transaction_id();
        callback_logging->debug("Transaction ID pulled");
        callback_logging->debug(transaction_id);
        //Build the outbound message
        object_string = create_response(db_object, message_type, transaction_id);
        object_string = perform_smart_update(db_object, new_obj, object_string, message_type, inp_msg_type, transaction_id);
      }
    }

    //If we have any errors, then we send back an appropriate error response
    else {
      if (cm->get_sendobfailuresactive()) {
        //Set up a failure response
        object_string = create_error_response(message_type, transaction_id, response_key, resp_err_string);
      }
    }

  }
  else
  {
    message_type = ERR;
    resp_err_string = r->req_err->err_message;
    if (inp_msg_type == OBJ_CRT) {
      callback_logging->error("Failed to Create");
    }
    else if (inp_msg_type == OBJ_DEL) {
      callback_logging->error("Failed to Delete");
    }
    else if (inp_msg_type == OBJ_GET) {
      callback_logging->error("Failed to Retrieve");
    }
    callback_logging->error(response_key);
    callback_logging->error(resp_err_string);

    //If configuration for OB Failure Responses is active, we build a failure response
    if (cm->get_sendobfailuresactive()) {
      //Set up a failure response
      object_string = create_error_response(message_type, transaction_id, response_key, resp_err_string);
    }
  }

  //Send an OB Message if our response string isn't empty
  if (!object_string.empty()) {
    out_resp = send_outbound_msg(object_string);
    //Process the response
    callback_logging->debug("Response to Outbound Message:");
    callback_logging->debug(out_resp);
  }

  //Remove the element from the smart updbate buffer
  if (cm->get_transactionidsactive() && !(response_key.empty())) {
    if (xRedis->exists(response_key.c_str())) {
      xRedis->del(response_key.c_str());
    }
  }

  if (!msg_type)
  {
    callback_logging->debug("No Message Type found for deletion");
  }
  else {
    delete msg_type;
  }

  if (!db_object)
  {
    callback_logging->debug("No DB Object found for deletion");
  }
  else {
    delete db_object;
  }

  if (!new_obj)
  {
    callback_logging->debug("No Redis Object found for deletion");
  }
  else {
    delete new_obj;
  }

  return r->req_addr;
}

//-----------------------
//---Callback Methods----
//-----------------------

//Couchbase Callbacks

//The storage callback is simple, just callback_logging the storage act and result
inline std::string my_storage_callback (Request *r)
{
  return default_callback(r, OBJ_CRT);
}

//The delete callback is also simple, just logging the storage act and result
inline std::string my_delete_callback (Request *r)
{
  return default_callback(r, OBJ_DEL);
}

//The get callback has the complex logic for the smart updates
std::string my_retrieval_callback (Request *r)
{
  return default_callback(r, OBJ_GET);
}
