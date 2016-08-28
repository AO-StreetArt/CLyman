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
#include "aossl/factory/callbacks.h"
#include "globals.h"

//-----------------------
//---Callback Methods----
//-----------------------

//Couchbase Callbacks

//The storage callback is simple, just logging the storage act and result
inline std::string my_storage_callback (Request *r)
{
  if (r->req_err->err_code == NOERROR)
  {
    logging->debug("stored:");
    logging->debug(r->req_addr);
  }
  else
  {
    logging->error("Failed to store:");
    logging->error(r->req_addr);
    logging->error(r->req_err->err_message);
  }
  return r->req_addr;
}

//The delete callback is also simple, just logging the storage act and result
inline std::string my_delete_callback (Request *r)
{
  if (r->req_err->err_code == NOERROR)
  {
    logging->debug("delete:");
    logging->debug(r->req_addr);
  }
  else
  {
    logging->error("Failed to delete:");
    logging->error(r->req_addr);
    logging->error(r->req_err->err_message);
  }
  return r->req_addr;
}

//The get callback has the complex logic for the smart updates
std::string my_retrieval_callback (Request *r)
{
  //Set up our base objects
  Obj3 *new_obj = NULL;
  std::string out_resp = "";
  std::string obj_string = "";

  //Are there any errors coming back from Couchbase?
  if (r->req_err->err_code != NOERROR) {
    logging->error("Failed to retrieve:");
    logging->error(r->req_addr);
    logging->error(r->req_err->err_message);
  }
  else
  {
    logging->info("Retrieved: ");
    std::string key_string = r->req_addr;
    const char *k = key_string.c_str();
    std::string obj_string = r->req_data;
    const char *resp_obj = obj_string.c_str();
    logging->info( key_string );
    logging->info( obj_string );

    //Smart updates are not active
    if (!cm->get_smartupdatesactive()) {
      //Parse the document from the DB
      rapidjson::Document temp_d;
      try {
        temp_d.Parse(resp_obj);
        new_obj = new Obj3 (temp_d);
      }
      catch (std::exception& e) {
        logging->error("Exception Occurred parsing message from DB");
        logging->error(e.what());
      }

      //Output the object on the Outbound ZeroMQ port
      if (!new_obj) {
        logging->error("Null Pointer Object detected from DB");
      }
      else
      {
        if (cm->get_mfjson()) {
          obj_string = new_obj->to_json_msg(OBJ_GET);
        }
        else if (cm->get_mfprotobuf()) {
          obj_string = new_obj->to_protobuf_msg(OBJ_GET);
        }
        zmqo->send(obj_string);
        out_resp = zmqo->recv();
        logging->debug("Response Recieved:");
        logging->debug(out_resp);
        delete new_obj;
      }
    }

    //Smart Updates are active
    //We need to update the object in the DB, then output the object
    //On the Outbound ZeroMQ port.
    else
    {
      logging->debug("Smart Update Logic Activated");
      //Then, let's get and parse the response from the database
      //We need to clean the response since Couchbase gives dirty responses

      rapidjson::Document temp_d;
      Obj3 *temp_obj = NULL;
      try {
        temp_d.Parse(resp_obj);
        new_obj = new Obj3 (temp_d);
      }
      catch (std::exception& e) {
        //Catch a possible exception and write it to the logs
        logging->error("Exception Occurred parsing message from Couchbase");
        logging->error(e.what());
      }
      if (!new_obj)
      {
        logging->error("Null Pointer Object detected from DB");
      }
      else
      {
        logging->debug("Database Object Parsed");

        if ( !(xRedis->exists(k)) ) {
          logging->error("Active Updates enabled but object not found in smart update buffer, outputting DB Object on OB ZMQ Port");
          //And output the message on the ZMQ Port
          std::string object_string;
          if (cm->get_mfjson()) {
            object_string = new_obj->to_json_msg(OBJ_UPD);
          }
          else if (cm->get_mfprotobuf()) {
            object_string = new_obj->to_protobuf_msg(OBJ_UPD);
          }
          zmqo->send(object_string);
          out_resp = zmqo->recv();
        }
        else
        {
          logging->debug("Object found in Smart Update Buffer");
          //Let's get the object out of the active update list
          std::string strValue = xRedis->load(k);
          if (strValue.empty()) {
            logging->error("Unable to load object and perform smart update");
          }
          else
          {
            logging->debug(strValue);

            //Are we writing to redis with json?
            if (cm->get_rfjson()) {
              rapidjson::Document temp_d;
              try {
                temp_d.Parse(strValue.c_str());
                temp_obj = new Obj3 (temp_d);
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
                temp_obj = new Obj3 (pobj);
              }
              catch (std::exception& e) {
                logging->error("Exception Occurred parsing message from Couchbase");
                logging->error(e.what());
              }
            }

            if (temp_obj) {

              //Now, we can compare the two and apply any updates from the
              //object list to the object returned from the database
              new_obj->transform_object( temp_obj );

              //Generate the message to be output
              if (cm->get_mfjson()) {
                obj_string = new_obj->to_json_msg(OBJ_UPD);
              }
              else if (cm->get_mfprotobuf()) {
                obj_string = new_obj->to_protobuf_msg(OBJ_UPD);
              }

              //output the message on the ZMQ Port
              zmqo->send(obj_string);
              out_resp = zmqo->recv();
              logging->debug("Response Recieved:");
              logging->debug(out_resp);
            }

            //Remove the element from the smart updbate buffer
            if (xRedis->exists(temp_key)) {
              xRedis->del(temp_key);
            }
            else {
              logging->debug("Key already expired from update buffer, not deleting");
            }

            //Save the resulting object back to the DB
            cb->save_object (new_obj);
            cb->wait();

            //Cleanup
            delete new_obj;
            delete temp_obj;
          }
        }
      }
    }
  }
  return out_resp;
}
