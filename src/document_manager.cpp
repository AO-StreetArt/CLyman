#include "document_manager.h"

//Global Object Creation
void DocumentManager::cr_obj_global(Obj3 *new_obj) {

  //Generate a new key for the object
  std::string new_key = ua->generate();

  //Set the new key on the new object
  new_obj->set_key(new_key);

  //Output a message on the outbound ZMQ Port
  std::string new_obj_string;
  if (cm->get_mfjson()) {
    new_obj_string = new_obj->to_json_msg(OBJ_CRT);
  }
  else if (cm->get_mfprotobuf()) {
    new_obj_string = new_obj->to_protobuf_msg(OBJ_CRT);
  }
  zmqo->send(new_obj_string);

  //Save the object to the couchbase DB
  cb->create_object (new_obj);
  cb->wait();
}

//Create Object from a Rapidjson Document
void DocumentManager::create_objectd(rapidjson::Document& d) {
  logging->info("Create object called with document: ");

  if (d.HasMember("location") && d.HasMember("bounding_box") && d.HasMember("scenes")) {

    //Build the object and the key
    Obj3 *new_obj = new Obj3 (d);
    cr_obj_global(new_obj);
  }
  else {
    logging->error("Create Message recieved without location, bounding box, or scene");
  }
}

//Create Object from a Protobuffer object
void DocumentManager::create_objectpb(protoObj3::Obj3 p_obj) {
  logging->info("Create object called with buffer: ");

  if (p_obj.has_location() && p_obj.has_bounding_box() && p_obj.scenes_size() > 0) {

    //Build the object and the key
    Obj3 *new_obj = new Obj3 (p_obj);
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
void DocumentManager::upd_obj_global(Obj3 *temp_obj) {
  if (cm->get_smartupdatesactive()) {
    //We start by writing the object into the smart update buffer
    //then, we can issue a get call

    //upon returning, the get callback should
    //check the smart update buffer for a matching key.

    //If it is found, we update the DB Entry.
    //Else, we simply output the value retrieved from the DB

    //Check if the object already exists in the smart update buffer.
    //If so, reject the update.
    const char * temp_key = temp_obj->get_key().c_str();
    if (xRedis->exists(temp_key) == false) {
  bool bRet;
      if (cm->get_rfjson()) {
        bRet = xRedis->save(temp_key, temp_obj->to_json_msg(OBJ_UPD));
      }
      else if (cm->get_rfprotobuf()) {
        bRet = xRedis->save(temp_key, temp_obj->to_protobuf_msg(OBJ_UPD));
      }
      if (!bRet) {
        logging->error("Error putting object to Redis Smart Update Buffer");
      }
      bool bRet2 = xRedis->expire(temp_key, cm->get_subduration());
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
      Obj3 *sub_obj;
      bool go_ahead = false;
      if (cm->get_rfprotobuf()) {
        protoObj3::Obj3 pobj;
        try {
          pobj.ParseFromString(strValue);
          go_ahead=true;
        }
        catch (std::exception& e) {
          //Catch a possible exception and write it to the logs
          logging->error("Exception Occurred parsing message from Redis");
          logging->error(e.what());
        }
        if (go_ahead) {
          sub_obj = new Obj3 (pobj);
        }
      }
      else if (cm->get_rfjson()) {
        rapidjson::Document doc;
        try {
          doc.Parse(strValue.c_str());
          go_ahead=true;
        }
        catch (std::exception& e) {
          //Catch a possible exception and write it to the logs
          logging->error("Exception Occurred parsing message from Redis");
          logging->error(e.what());
        }
        if (go_ahead) {
          sub_obj = new Obj3 (doc);
        }
      }
      if (go_ahead) {
        logging->error(sub_obj->to_json());
        delete sub_obj;
      }
    }
  }
  else {
    //If smart updates are disabled, we can just write the value directly
    //To the DB

    std::string temp_obj_str;

    if (cm->get_mfjson()) {
      temp_obj_str = temp_obj->to_json_msg(OBJ_UPD);
    }
    else if (cm->get_mfprotobuf()) {
      temp_obj_str = temp_obj->to_protobuf_msg(OBJ_UPD);
    }

    zmqo->send(temp_obj_str);

    cb->save_object (temp_obj);
    delete temp_obj;
    cb->wait();
  }
}

//Update Object called with a Rapidjson Document
void DocumentManager::update_objectd(rapidjson::Document& d) {
  logging->info("Update object called with document: ");
  if (d.HasMember("key")) {
    Obj3 *temp_obj = new Obj3 (d);
    upd_obj_global(temp_obj);
  }
}

//Update Object called with a Protobuffer object
void DocumentManager::update_objectpb(protoObj3::Obj3 p_obj) {
  logging->info("Update object called with buffer: ");
  if (p_obj.has_key()) {
    Obj3 *temp_obj = new Obj3 (p_obj);
    upd_obj_global(temp_obj);
  }
}

//Get Object Global
void DocumentManager::get_obj_global(std::string rk_str) {
  const char * rkc_str = rk_str.c_str();
  //Get the object from the DB
  //If it's in the active update buffer, then this will
  //force through the update prior to returning the value
  cb->load_object( rkc_str );
  cb->wait();
  //}
}

//Get object Protobuffer
void DocumentManager::get_objectpb(protoObj3::Obj3 p_obj) {
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
void DocumentManager::get_objectd(rapidjson::Document& d) {
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
void DocumentManager::del_obj_global(std::string key) {
  const char * kc_str = key.c_str();

  //Output a delete message on the outbound ZMQ Port

  logging->debug("Building Delete Message");

  Obj3 obj;
  obj.set_key(key);

  //Return the object on the outbound ZMQ Port
  std::string nobj_str;

  if (cm->get_mfjson()) {
    nobj_str = obj.to_json_msg(OBJ_DEL);
  }
  else if (cm->get_mfprotobuf()) {
    nobj_str = obj.to_protobuf_msg(OBJ_DEL);
  }
  zmqo->send(nobj_str);

  cb->delete_object( kc_str );
  cb->wait();
}

//Delete Object Protobuffer
void DocumentManager::delete_objectpb(protoObj3::Obj3 p_obj) {
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
void DocumentManager::delete_objectd(rapidjson::Document& d) {
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
