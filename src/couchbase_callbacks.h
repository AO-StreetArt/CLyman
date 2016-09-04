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

#include "aossl/factory/logging_interface.h"
#include "aossl/factory/response_interface.h"
#include "aossl/factory/callbacks.h"
#include "globals.h"

//-----------------------
//-----Base Methods------
//-----------------------

//Write an object to Redis
inline void post_to_redis(Obj3 *temp_obj, int msg_type, std::string transaction_id) {
  const char * temp_key = temp_obj->get_key().c_str();
  bool bRet;
  if (cm->get_rfjson()) {
    bRet = xRedis->save(temp_key, temp_obj->to_json_msg(msg_type, transaction_id));
  }
  else if (cm->get_rfprotobuf()) {
    bRet = xRedis->save(temp_key, temp_obj->to_protobuf_msg(msg_type, transaction_id));
  }
  if (!bRet) {
    logging->error("Error putting object to Redis Smart Update Buffer");
  }
  bool bRet2 = xRedis->expire(temp_key, cm->get_subduration());
  if (!bRet2) {
    logging->error("Error expiring object in Redis Smart Update Buffer");
  }
}

//Parse the Redis Response string into the provided Obj3, and return the message type
//Returns a message type of -1 in case of failure (ie. object not found in redis)
inline int set_redis_response_object(Request *r, Obj3 *redis_object)
{
  std::string response_key = r->req_addr;
  rapidjson::Document temp_d2;
  rapidjson::Value *val;
  int msg_type = -1;
  //Check Redis for transaction information
  if ( !(xRedis->exists(response_key.c_str())) ) {
    logging->error("Storage Callback Returned with no Redis Information");
  }
  else
  {
    logging->debug("Object found in Smart Update Buffer");
    //Let's get the object out of the active update list
    std::string strValue = xRedis->load(response_key.c_str());
    if (strValue.empty()) {
      logging->error("Unable to load object from Redis");
    }
    else
    {
      //Are we writing to redis with json?
      if (cm->get_rfjson()) {
        try {
          temp_d2.Parse(strValue.c_str());
          redis_object = new Obj3 (temp_d2);
          val = &temp_d2["message_type"];
          msg_type = val->GetInt();
        }
        catch (std::exception& e) {
          logging->error("Exception Occurred parsing message from Couchbase");
          logging->error(e.what());
        }
      }

      //Or are we writing to redis with Protocol buffers?
      else if (cm->get_rfprotobuf()) {
        protoObj3::Obj3 pobj;
        try {
          pobj.ParseFromString(strValue);
          redis_object = new Obj3 (pobj);
          msg_type = pobj.message_type();
        }
        catch (std::exception& e) {
          logging->error("Exception Occurred parsing message from Couchbase");
          logging->error(e.what());
        }
      }
    }
  }
  return msg_type;
}

//Return an Obj3 pointer built from the json string passed in, assumed to be from the DB
inline Obj3* set_db_response_object(std::string object_string)
{
  rapidjson::Document temp_d;
  if (!object_string.empty()) {
    //Process the DB Object
    try {
      temp_d.Parse(object_string);
      return new Obj3 (temp_d);
    }
    catch (std::exception& e) {
      logging->error("Exception Occurred parsing message from DB");
      logging->error(e.what());
    }
  }
  return NULL;
}

//create an error response and return the string
inline std::string create_error_response(int msg_type, std::string transaction_id, std::string response_key, std::string resp_err_string)
{
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

//Send an outbound message and return the response
inline std::string send_outbound_msg(std::string message_string)
{
  std::string out_resp = "";
  if (!message_string.empty()) {
    //output the message on the ZMQ Port
    logging->debug("Sending Outbound Message:");
    logging->debug(message_string);
    zmqo->send(message_string);

    //Get the response
    out_resp = zmqo->recv();
  }
  return out_resp;
}

inline std::string default_callback (Request *r, std::string operation_error_string)
{
  Obj3 *new_obj = NULL;
  Obj3 *db_object = NULL;
  std::string object_string;
  std::string obj_string = r->req_data;
  int msg_type = ERR;
  std::string transaction_id = "";
  std::string out_resp = "";

  std::string response_key = r->req_addr;

  //Check Redis for transaction information
  msg_type = set_redis_response_object(r, new_obj);

  //If the Redis update failed, set the message type back to error
  if (msg_type == -1)
  {
    msg_type = ERR;
  }
  else
  {
    if (!new_obj)
    {
      logging->error("No error code returned from set_redis_response method, but null pointer detected");
    }
    else
    {
      //Use the Redis message to populate transaction ID
      transaction_id = new_obj->get_transaction_id();
      delete new_obj;
    }
  }

  //Actions when the storage operation is successful
  if (r->req_err->err_code == NOERROR)
  {
    logging->debug("stored:");
    logging->debug(response_key);

    //Build the DB Response Object
    db_object = set_db_response_object(obj_string);

    if (!db_object)
    {
      logging->error("Error parsing DB Response, null pointer detected");
    }
    else
    {
      //Build the outbound message
      object_string = create_response(db_object, msg_type, transaction_id);
      delete db_object;
    }
  }
  else
  {
    std::string resp_err_string = r->req_err->err_message;
    logging->error(operation_error_string);
    logging->error(response_key);
    logging->error(resp_err_string);

    //If configuration for OB Failure Responses is active, we build a failure response
    if (cm->get_sendobfailuresactive()) {
      //Set up a failure response
      object_string = create_error_response(msg_type, transaction_id, response_key, resp_err_string);
    }
  }

  //Send an OB Message if our response string isn't empty
  out_resp = send_outbound_msg(object_string);

  //Process the response
  logging->debug("Response to Outbound Message:");
  logging->debug(out_resp);

  //Remove the element from the smart updbate buffer
  if (xRedis->exists(response_key.c_str())) {
    xRedis->del(response_key.c_str());
  }

  return r->req_addr;
}

//-----------------------
//---Callback Methods----
//-----------------------

//Couchbase Callbacks

//The storage callback is simple, just logging the storage act and result
inline std::string my_storage_callback (Request *r)
{
  return default_callback(r, "Failed to Store:");
}

//The delete callback is also simple, just logging the storage act and result
inline std::string my_delete_callback (Request *r)
{
  return default_callback(r, "Failed to Delete:");
}

//The get callback has the complex logic for the smart updates
std::string my_retrieval_callback (Request *r)
{
  Obj3 *new_obj = NULL;
  Obj3* db_object = NULL;
  std::string object_string;
  std::string obj_string = r->req_data;
  int msg_type = ERR;
  std::string transaction_id = "";
  std::string out_resp = "";

  std::string key_string = r->req_addr;

  //Check Redis for transaction information
  msg_type = set_redis_response_object(r, new_obj);

  //If the Redis update failed, set the message type back to error
  if (msg_type == -1)
  {
    msg_type = ERR;
  }
  else
  {
    if (!new_obj)
    {
      logging->error("No error code returned from set_redis_response method, but null pointer detected");
    }
    else
    {
      //Use the Redis message to populate transaction ID
      transaction_id = new_obj->get_transaction_id();
    }
  }

  //Are there any errors coming back from Couchbase?
  if (r->req_err->err_code != NOERROR) {
    logging->error("Failed to retrieve:");
    logging->error(key_string);
    logging->error(r->req_err->err_message);
    //If configuration for OB Failure Responses is active, we build a failure response
    if (cm->get_sendobfailuresactive()) {
      //Set up a failure response
      object_string = create_error_response(msg_type, transaction_id, key_string, r->req_err->err_message);
      out_resp = send_outbound_msg(object_string);
      logging->debug("Response Recieved:");
      logging->debug(out_resp);
    }
  }
  else
  {
    logging->debug("stored:");
    logging->debug(key_string);

    //Build the DB Response Object
    db_object = set_db_response_object(obj_string);

    if (!db_object)
    {
      logging->error("Error parsing DB Response, null pointer detected");
    }
    else
    {
      //Build the outbound message
      object_string = create_response(db_object, msg_type, transaction_id);
    }

    //Smart updates are not active, so we just have a get request
    if (!cm->get_smartupdatesactive()) {

      //Output the object on the Outbound ZeroMQ port
      if (!db_object) {
        logging->error("Null Pointer Object detected from DB");
      }
      else
      {
        object_string = create_response(db_object, msg_type, transaction_id);
        out_resp = send_outbound_msg(object_string);
        logging->debug("Response Recieved:");
        logging->debug(out_resp);
      }
    }

    //Smart Updates are active
    else
    {
      //We have a get message in the smart update flow
      if (msg_type == OBJ_GET)
      {
        logging->debug("Get response initiated with smart updates active");
        object_string = create_response(db_object, msg_type, transaction_id);
        out_resp = send_outbound_msg(object_string);
        logging->debug("Response Recieved:");
        logging->debug(out_resp);
      }
      //We need to update the object in the DB, then output the object
      //On the Outbound ZeroMQ port.
      logging->debug("Smart Update Logic Activated");
      //Then, let's get and parse the response from the database
      //We need to clean the response since Couchbase gives dirty responses
      if (!db_object)
      {
        logging->error("Null Pointer Object detected from DB");
      }
      else
      {
        if ( !new_obj ) {
          std::string err_msg = "Active Updates enabled but object not found in smart update buffer";
          logging->error(err_msg);
          //And output the message on the ZMQ Port
          //If configuration for OB Failure Responses is active, we build a failure response
          if (cm->get_sendobfailuresactive()) {
            //Set up a failure response
            object_string = create_error_response(msg_type, transaction_id, key_string, err_msg);
            out_resp = send_outbound_msg(object_string);
            logging->debug("Response Recieved:");
            logging->debug(out_resp);
          }
        }
        else
        {
          //Now, we can compare the two and apply any updates from the
          //object list to the object returned from the database
          db_object->transform( new_obj );

          //Remove the element from the smart updbate buffer
          if (xRedis->exists(key_string.c_str())) {
            xRedis->del(key_string.c_str());
          }

          //Replace the element in the smart update buffer
          post_to_redis(new_obj, msg_type, transaction_id);

          //Save the resulting object back to the DB
          cb->save_object (new_obj);
          cb->wait();

        }
      }
    }
  }
  if (!db_object)
  {
    logging->debug("DB Object not instantiated");
  }
  else
  {
    delete db_object;
  }
  if (!new_obj)
  {
    logging->debug("Redis Object not instantiated");
  }
  else
  {
    delete new_obj;
  }
  return out_resp;
}
