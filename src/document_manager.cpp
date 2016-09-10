#include "document_manager.h"

//Write an object to Redis
void DocumentManager::put_to_redis(Obj3 *temp_obj, int msg_type, std::string transaction_id) {
  const char * temp_key = temp_obj->get_key().c_str();
  bool bRet;
  if (cm->get_rfjson()) {
    bRet = xRedis->save(temp_key, temp_obj->to_json_msg(msg_type, transaction_id));
  }
  else if (cm->get_rfprotobuf()) {
    bRet = xRedis->save(temp_key, temp_obj->to_protobuf_msg(msg_type, transaction_id));
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
std::string DocumentManager::cr_obj_global(Obj3 *new_obj, std::string transaction_id) {

  //Set the new key on the new object
  std::string object_key = ua->generate();
  new_obj->set_key( object_key );

  //See if we need to write the transaction to Redis
  if (cm->get_transactionidsactive()) {
    put_to_redis(new_obj, OBJ_CRT, transaction_id);
  }

  //Return the object key
  return object_key;
}

//Create Object from a Rapidjson Document
std::string DocumentManager::create_objectd(rapidjson::Document& d, std::string transaction_id, Obj3 *new_obj) {
  doc_logging->info("Create object called with document: ");

  if (d.HasMember("location") && d.HasMember("bounding_box") && d.HasMember("scenes")) {

    //Build the object and the key
    new_obj = new Obj3 (d);
    return cr_obj_global(new_obj, transaction_id);
  }
  else {
    doc_logging->error("Create Message recieved without location, bounding box, or scene");
  }
  return "";
}

//Create Object from a Protobuffer object
std::string DocumentManager::create_objectpb(protoObj3::Obj3 p_obj, std::string transaction_id, Obj3 *new_obj) {
  doc_logging->info("Create object called with buffer: ");

  if (p_obj.has_location() && p_obj.has_bounding_box() && p_obj.scenes_size() > 0) {

    //Build the object and the key
    new_obj = new Obj3 (p_obj);
    return cr_obj_global(new_obj, transaction_id);
  }
  else {
    doc_logging->error("Create Message recieved without location, bounding box, or scene");
    doc_logging->debug(p_obj.key());
    doc_logging->debug(p_obj.name());
    for (int m = 0; m < p_obj.scenes_size(); ++m)
    {
      doc_logging->debug(p_obj.scenes(m));
    }
  }
  return "";
}

//Global Update Object
std::string DocumentManager::upd_obj_global(Obj3 *temp_obj, std::string transaction_id) {
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
      put_to_redis(temp_obj, OBJ_UPD, transaction_id);
    }
    else {
      doc_logging->error("Collision in Active Update Buffer Detected");

      doc_logging->error(temp_obj->to_json());
      //Get the object from the DB
      //If it's in the active update buffer, then this will
      //force through the update prior to returning the value

      if (cm->get_transactionidsactive()) {
        while (xRedis->exists(temp_key) == true) {
          cb->load_object( temp_key );
          cb->wait();
        }
      }

      std::string su_key = temp_obj->get_key();

      put_to_redis(temp_obj, OBJ_UPD, transaction_id);

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

//Update Object called with a Rapidjson Document
std::string DocumentManager::update_objectd(rapidjson::Document& d, std::string transaction_id, Obj3 *new_obj) {
  doc_logging->info("Update object called with document: ");
  if (d.HasMember("key")) {
    new_obj = new Obj3 (d);
    return upd_obj_global(new_obj, transaction_id);
  }
  return "";
}

//Update Object called with a Protobuffer object
std::string DocumentManager::update_objectpb(protoObj3::Obj3 p_obj, std::string transaction_id, Obj3 *new_obj) {
  doc_logging->info("Update object called with buffer: ");
  if (p_obj.has_key()) {
    new_obj = new Obj3 (p_obj);
    return upd_obj_global(new_obj, transaction_id);
  }
  return "";
}

//Get Object Global
void DocumentManager::get_obj_global(std::string rk_str, std::string transaction_id, Obj3 *new_obj) {
  const char * rkc_str = rk_str.c_str();

  //Generate a new Obj3 to put to Redis
  new_obj = new Obj3;
  new_obj->set_key(rk_str);

  if (cm->get_transactionidsactive()) {
    while (xRedis->exists(rkc_str) == true) {
      cb->load_object( rkc_str );
      cb->wait();
    }

    put_to_redis(new_obj, OBJ_UPD_GLOBAL, transaction_id);
  }
}

//Get object Protobuffer
std::string DocumentManager::get_objectpb(protoObj3::Obj3 p_obj, std::string transaction_id, Obj3 *new_obj) {
  doc_logging->info("Get object called with buffer: ");
  if (p_obj.has_key()) {
    std::string rk_str = p_obj.key();
    get_obj_global(rk_str, transaction_id, new_obj);
    return rk_str;
  }
  else {
    doc_logging->error("Message Recieved without key");
  }
  return "";
}

//Get Object Rapidson Document
std::string DocumentManager::get_objectd(rapidjson::Document& d, std::string transaction_id, Obj3 *new_obj) {
  doc_logging->info("Get object called with document: ");
  if (d.HasMember("key")) {
    rapidjson::Value *rkey;
    rkey = &d["key"];
    //Check the Active Update Buffer for inflight transactions
    //If we have any, then we should pull the value from there
    //And return it.
    std::string rk_str = rkey->GetString();
    get_obj_global(rk_str, transaction_id, new_obj);
    return rk_str;
  }
  else {
    doc_logging->error("Message Recieved without key");
  }
  return "";
}

//Delete Object Global
void DocumentManager::del_obj_global(std::string key, std::string transaction_id, Obj3 *new_obj) {

  //Output a delete message on the outbound ZMQ Port
  doc_logging->debug("Building Delete Message");

  new_obj = new Obj3;
  new_obj->set_key(key);

  //See if we need to write the transaction to Redis
  if (cm->get_transactionidsactive()) {
    put_to_redis(new_obj, OBJ_DEL, transaction_id);
  }
}

//Delete Object Protobuffer
std::string DocumentManager::delete_objectpb(protoObj3::Obj3 p_obj, std::string transaction_id, Obj3 *new_obj) {
  doc_logging->info("Delete object called with buffer: ");
  if (p_obj.has_key()) {
    std::string k = p_obj.key();
    del_obj_global(k, transaction_id, new_obj);
    return k;
  }
  else {
    doc_logging->error("Message Recieved without key");
  }
  return "";
}

//Delete Object Rapidjson Document
std::string DocumentManager::delete_objectd(rapidjson::Document& d, std::string transaction_id, Obj3 *new_obj) {
  doc_logging->info("Delete object called with document: ");
  if (d.HasMember("key")) {
    rapidjson::Value *val;
    val = &d["key"];
    std::string key = val->GetString();
    del_obj_global(key, transaction_id, new_obj);
    return key;
  }
  else {
    doc_logging->error("Message Recieved without key");
  }
  return "";
}
