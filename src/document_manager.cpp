#include "document_manager.h"

//Write an object to Redis
void DocumentManager::put_to_redis(Obj3 *temp_obj, int msg_type, std::string transaction_id) {

  std::string node_id = cm->get_nodeid();

  //Generate the object key
  std::string key_str = temp_obj->get_key() + node_id;

  //Determine if another instance of CLyman has a lock on the Redis Mutex
  std::string current_mutex_key;
  bool lock_established = false;

  while (!lock_established) {

    doc_logging->error("Redis Mutex Lock Routine Started");

    if ( xRedis->exists( temp_obj->get_key() ) ) {
      try {
        current_mutex_key = xRedis->load( temp_obj->get_key() );
      }
      catch (std::exception& e) {
        main_logging->error("Exception encountered during Redis Request");
        main_logging->error(e.what());
      }
    }

    if ((current_mutex_key != "") && (current_mutex_key != cm->get_nodeid())) {
      //Another instance of Clyman has a lock on the redis mutex
      //Block until the lock is cleared
      doc_logging->error("Existing Redis Mutex Lock Detected, waiting for lock to be released");
      while (xRedis->exists( temp_obj->get_key() )) {}
    }

    //Try to establish a lock on the Redis Mutex
    doc_logging->error("Attempting to obtain Redis Mutex Lock");
    if ( !(xRedis->exists( temp_obj->get_key() )) ) {
      try {
        lock_established = xRedis->setnx( temp_obj->get_key() , node_id);
      }
      catch (std::exception& e) {
        main_logging->error("Exception encountered during Redis Request");
        main_logging->error(e.what());
      }
    }

  }

  //Write the Object to Redis
  bool bRet;
  if (cm->get_rfjson()) {
    try {
      bRet = xRedis->setex(key_str, temp_obj->to_json_msg(msg_type, transaction_id), cm->get_subduration());
    }
    catch (std::exception& e) {
      main_logging->error("Exception encountered during Redis Request");
      main_logging->error(e.what());
    }
  }
  else if (cm->get_rfprotobuf()) {
    try {
      bRet = xRedis->setex(key_str, temp_obj->to_protobuf_msg(msg_type, transaction_id), cm->get_subduration());
    }
    catch (std::exception& e) {
      main_logging->error("Exception encountered during Redis Request");
      main_logging->error(e.what());
    }
  }

  //Writing the object failed
  if (!bRet) {
    main_logging->error("Error putting object to Redis Smart Update Buffer, Removing Redis Mutex Lock");
    xRedis->del( temp_obj->get_key() );
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
    if (xRedis->exists(object_key) == false) {
      put_to_redis(temp_obj, m_type, transaction_id);
    }
    else {
      doc_logging->error("Collision in Active Update Buffer Detected");

      doc_logging->error(temp_obj->to_json());
      //Get the object from the DB
      //If it's in the active update buffer, then this will
      //force through the update prior to returning the value

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

  //Transaction ID's are active
  //Clear the active update buffer for this object prior to executing the get
  if (cm->get_transactionidsactive()) {

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
