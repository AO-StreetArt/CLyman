#include "document_manager.h"

//Write an object to Redis
void DocumentManager::put_to_redis(Obj3 *temp_obj, int msg_type, std::string transaction_id) {
  const char * temp_key = temp_obj->get_key().c_str();
  bool bRet;
  if (cm->get_rfjson()) {
    bRet = xRedis->save(temp_key, temp_obj->to_json_msg(msg_type));
  }
  else if (cm->get_rfprotobuf()) {
    bRet = xRedis->save(temp_key, temp_obj->to_protobuf_msg(msg_type));
  }
  if (!bRet) {
    doc_logging->error("Error putting object to Redis Smart Update Buffer");
  }
  bool bRet2 = xRedis->expire(temp_key, cm->get_subduration());
  if (!bRet2) {
    doc_logging->error("Error expiring object in Redis Smart Update Buffer");
  }
}

//Global Object Creation
std::string DocumentManager::create_object(Obj3 *new_obj, std::string transaction_id) {
  std::string object_key = new_obj->get_key();

  //See if we need to write the transaction to Redis
  if (cm->get_transactionidsactive()) {
    put_to_redis(new_obj, OBJ_CRT, transaction_id);
  }

  //Return the object key
  return object_key;
}

//Global Update Object
std::string DocumentManager::update_object(Obj3 *temp_obj, std::string transaction_id, int m_type) {
  std::string object_key = temp_obj->get_key();
  if (cm->get_smartupdatesactive()) {
    //We start by writing the object into the smart update buffer
    //then, we can issue a get call

    //upon returning, the get callback should
    //check the smart update buffer for a matching key.

    //If it is found, we update the DB Entry.
    //Else, we simply output the value retrieved from the DB

    //Check if the object already exists in the smart update buffer.
    const char * temp_key = object_key.c_str();
    if (xRedis->exists(temp_key) == false) {
      put_to_redis(temp_obj, m_type, transaction_id);
    }
    else {
      doc_logging->error("Collision in Active Update Buffer Detected");

      doc_logging->error(temp_obj->to_json());
      //Get the object from the DB
      //If it's in the active update buffer, then this will
      //force through the update prior to returning the value

      if (cm->get_transactionidsactive()) {
        // while (xRedis->exists(temp_key) == true) {
        //   cb->load_object( temp_key );
        //   cb->wait();
        // }
        doc_logging->debug("Collision detected in Update Buffer");
      }

      std::string su_key = temp_obj->get_key();

      put_to_redis(temp_obj, m_type, transaction_id);

    }
  }
  else {
    //If smart updates are disabled, we can just write the value directly
    //To the DB

    //See if we need to write the transaction to Redis
    if (cm->get_transactionidsactive()) {
      put_to_redis(temp_obj, OBJ_UPD_GLOBAL, transaction_id);
    }

  }
  return object_key;
}

//Get Object Global
std::string DocumentManager::get_object(Obj3 *new_obj, std::string transaction_id) {
  std::string object_key = new_obj->get_key();
  const char * rkc_str = object_key.c_str();

  //Transaction ID's are active
  //Clear the active update buffer for this object prior to executing the get
  if (cm->get_transactionidsactive()) {
    if (xRedis->exists(rkc_str) == true) {
      // cb->load_object( rkc_str );
      // cb->wait();
      doc_logging->debug("Collision detected in Update Buffer");
    }

    put_to_redis(new_obj, OBJ_GET, transaction_id);
  }

  return object_key;
}

//Delete Object Global
std::string DocumentManager::delete_object( Obj3 *new_obj, std::string transaction_id ) {

  std::string object_key = new_obj->get_key();

  //Output a delete message on the outbound ZMQ Port
  doc_logging->debug("Building Delete Message");

  //See if we need to write the transaction to Redis
  if (cm->get_transactionidsactive()) {
    put_to_redis(new_obj, OBJ_DEL, transaction_id);
  }

  return object_key;
}
