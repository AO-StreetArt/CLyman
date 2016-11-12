#include "lyman_log.h"
#include "obj3.h"
#include "redis_locking.h"
#include "configuration_manager.h"

#include "rapidjson/document.h"

#include "aossl/factory/redis_interface.h"
#include "aossl/factory/mongo_interface.h"
#include "aossl/factory/zmq_interface.h"

//The class containing core logic for CLyman
//Accepts an Obj3 (assuming it is an inbound message)
//And performs any and all processing on it,
//Includes Redis Locks, DB Interactions, and OB ZMQ messaging

#ifndef MSG_PROCESSOR
#define MSG_PROCESSOR

class MessageProcessor {

MongoInterface *m = NULL;
Zmqio *z = NULL;
RedisInterface *r = NULL;
ConfigurationManager *config = NULL;
RedisLocker *redis_locks = NULL;

  //Send a message on the Outbound ZMQ Port
  //Send an outbound message and return the response
  inline void send_outbound_msg(std::string message_string)
  {
    if (!message_string.empty()) {
      //output the message on the ZMQ Port
      processor_logging->debug("Sending Outbound Message:");
      processor_logging->debug(message_string);
      z->send(message_string);
    }
  }

  //Retrieve an object from the DB and load it into an Obj3
  inline Obj3* load_db_object(std::string key) {
    processor_logging->debug("Loading DB Object");
    std::string object_string = m->load_document(key);
    Obj3 *temp_obj = NULL;
    rapidjson::Document temp_d;
    if ( !(object_string.empty()) ) {
      //Process the DB Object
      processor_logging->debug("Parsing DB Object");
      try {
        temp_d.Parse(object_string.c_str());
        temp_obj = new Obj3 (temp_d, config->get_objectlockingenabled());
      }
      catch (std::exception& e) {
        processor_logging->error("Exception Occurred parsing message from DB");
        processor_logging->error(e.what());
      }
    }
    else {
      processor_logging->error("No DB Object Found");
    }
    return temp_obj;
  }

  //Insert the Obj3 into to the DB, return the key
  inline std::string insert_db_object(Obj3* obj_msg) {
    std::string ret_str =  m->create_document( obj_msg->to_json() );
    if (ret_str.empty()) {
      return "";
    }
    return ret_str;
  }

  //Save the Obj3 back to the DB
  inline bool save_db_object(Obj3* obj_msg) {
    return m->save_document( obj_msg->to_json(), obj_msg->get_key() );
  }

  //Delete the Obj3 from to the DB
  inline bool delete_db_object(std::string obj_key) {
    return m->delete_document( obj_key );
  }

  //Establish a redis mutex lock on an object
  //This is used to ensure atomic updates
  inline bool get_objmutex_lock(std::string obj_key, std::string node_id) {
    if ( config->get_atomictransactions() ) {
      std::string key = "MUT-" + obj_key;
      try {
        redis_locks->get_lock(key, node_id);
        return true;
      }
      catch (std::exception& e) {
        processor_logging->error("Exception occurred while establishing Object Mutex Lock");
        processor_logging->error(e.what());
      }
      return false;
    }
    //If locking is not enabled, then we just pretend like the lock is established
    else {
      return true;
    }
  }

  //Establish a user device lock on an object
  //This is used in user-level locking
  inline bool get_ud_lock(std::string obj_key, std::string ud_key) {
    if ( config->get_objectlockingenabled() ) {
      std::string key = "UD-" + obj_key;
      try {
        return r->setnx( key, ud_key);
      }
      catch (std::exception& e) {
        processor_logging->error("Exception occurred while establishing User Device Lock");
        processor_logging->error(e.what());
      }
      return false;
    }
    //If object locking is disabled, just pretend like the lock was established
    else {
      return true;
    }
  }

  //Release a redis mutex lock on an object
  //This is used to ensure atomic updates, and is called at end of individual message cycle
  inline bool release_objmutex_lock(std::string obj_key) {
    if ( config->get_atomictransactions() ) {
      std::string key = "MUT-" + obj_key;
      return redis_locks->release_lock(key, "");
    }
    else {
      return true;
    }
  }

  inline bool check_objmutex_lock(std::string obj_key) {
    std::string key = "MUT-" + obj_key;
    return r->exists(key);
  }

  //Release a user device lock on an object
  //This is used in user-level locking, called upon recieving an unlock message
  inline bool release_ud_lock(std::string obj_key, std::string ud_key) {
    if ( config->get_objectlockingenabled() ) {
      std::string key = "UD-" + obj_key;
      if (r->exists(key)) {
        std::string current_mutex_key = r->load(key);
        if (current_mutex_key == ud_key) {
          return r->del( key );
        }
      }
      return false;
    }
    else {
      return true;
    }
  }

  inline bool check_ud_lock(std::string obj_key, std::string ud_key) {
    std::string key = "UD-" + obj_key;
    if (r->exists(key)) {
      std::string current_mutex_key = r->load(key);
      if (current_mutex_key == ud_key) {
        return false;
      }
      return true;
    }
    return false;
  }

  inline std::string process_create_message(Obj3 *obj_msg) {
    //No Lock Needed
    //Just generate a document and insert it
    std::string key = insert_db_object(obj_msg);
    processor_logging->debug("Created new document with key: ");
    processor_logging->debug(key);

    if ( !(key.empty()) ) {
    //Send OB Message
      processor_logging->debug("Return Key not empty, sending OB message");
      obj_msg->set_key(key);
      send_outbound_msg(obj_msg->to_json());
    }
    else {
      processor_logging->error("Error writing to Mongo");
      key = "-1";
    }

    //Return document key
    return key;
  }

  inline std::string process_update_message(Obj3 *obj_msg) {
    //Deal with locks
    std::string key = obj_msg->get_key();
    processor_logging->debug("Processing Update Message on key: " + key);

    //-User Device Lock
    if (check_ud_lock(key, obj_msg->get_lock_id())) {
      processor_logging->debug("User Device Lock Encountered");
      return "locked";
    }

    //-Object Mutex Lock
    if ( !(get_objmutex_lock(key, config->get_nodeid())) ) {
      processor_logging->error("Failed to establish object mutex lock");
      return "-1";
    }

    //Update
    //-Load DB Object
    bool update_success = false;
    Obj3 *db_object = NULL;
    db_object = load_db_object(key);
    processor_logging->debug("DB Object Loaded:");
    processor_logging->debug(db_object->get_key());

    obj_msg->set_global_transform_type( config->get_globaltransforms() );

    //-Run Transformations
    if (db_object) {
      update_success = db_object->transform( obj_msg, obj_msg->get_lock_id() );
    }

    std::string obj_json = db_object->to_json();
    processor_logging->debug("Update Performed, resulting Obj3:");
    processor_logging->debug(obj_json);

    //-Save Object to DB
    if (update_success) {
      update_success = m->save_document(obj_json, key);
    }

    //Release Mutex Lock
    bool release_lock_success = release_objmutex_lock(key);
    if (!release_lock_success) {
      processor_logging->error("Error releasing Object Mutex Lock");
    }
    //Send OB Message
    if (update_success) {
      processor_logging->debug("Update Persistence Confirmed");
      send_outbound_msg(obj_json);
      return "";
    }
    return "-1";
  }

  inline std::string process_retrieve_message(Obj3 *obj_msg) {
    //Load the DB Object
    std::string key = obj_msg->get_key();
    processor_logging->debug("Processing Retrieve Message on key: " + key);
    std::string object_string = m->load_document(key);
    if (object_string.empty()) {
      processor_logging->debug("No Object Found");
      return "-1";
    }
    processor_logging->debug("Document Retrieved:");
    processor_logging->debug(object_string);
    return object_string;

  }

  inline std::string process_delete_message(Obj3 *obj_msg) {
    //Deal with locks
    std::string key = obj_msg->get_key();
    processor_logging->debug("Processing Delete message on key: " + key);

    //-User Device Lock
    if (check_ud_lock(key, obj_msg->get_lock_id())) {
      processor_logging->debug("User Device Lock Encountered");
      return "locked";
    }

    //-Object Mutex Lock
    if ( !(get_objmutex_lock(key, config->get_nodeid())) ) {
      processor_logging->error("Failed to establish object mutex lock");
      return "-1";
    }

    //Delete
    bool delete_success = m->delete_document(key);

    //Release Mutex Lock
    bool release_lock_success = release_objmutex_lock(key);
    if (!release_lock_success) {
      processor_logging->error("Error releasing Object Mutex Lock");
    }

    //Send OB Message
    if (delete_success) {
      processor_logging->debug("Deletion Confirmed");
      send_outbound_msg(obj_msg->to_json());
      return "";
    }
    return "-1";
  }

  inline std::string process_lock_message(Obj3 *obj_msg) {
    //Establish User Device Lock
    if ( get_ud_lock(obj_msg->get_key(), obj_msg->get_lock_id()) ) {
      processor_logging->debug("User Device Lock Established");
      return "";
    }
    return "-1";
  }

  inline std::string process_unlock_message(Obj3 *obj_msg) {
    //Release User Device Lock
    if ( release_ud_lock(obj_msg->get_key(), obj_msg->get_lock_id()) ) {
      processor_logging->debug("User Device Lock Released");
      return "";
    }
    return "-1";
  }

  inline std::string process_ping_message(Obj3 *obj_msg) {
    processor_logging->debug("Ping Pong");
    return "";
  }

  inline std::string process_kill_message(Obj3 *obj_msg) {
    return "";
  }
public:

  //Constructor & Destructor
  MessageProcessor(MongoInterface *mo, Zmqio *zq, RedisInterface *rd, ConfigurationManager *con) {m = mo; z = zq; r = rd;config = con;redis_locks = new RedisLocker(rd);}
  ~MessageProcessor() {delete redis_locks;}

  //Process a message in the form of an Obj3
  //In the case of a get message, return the retrieved document back to the main method
  //In the case of a create message, return the key of the created object back to the main method
  //Otherwise, return a blank string in the case of success, and "-1" in the case of failure
  inline std::string process_message(Obj3 *obj_msg) {

    //Determine what type of message we have, and act accordingly
    int msg_type = obj_msg->get_message_type();
    processor_logging->debug("Message Type: " + std::to_string(msg_type));
    std::string process_return;

    if (msg_type == OBJ_CRT) {
      process_return = process_create_message(obj_msg);
    }
    else if (msg_type == OBJ_UPD) {
      process_return = process_update_message(obj_msg);
    }
    else if (msg_type == OBJ_GET) {
      process_return = process_retrieve_message(obj_msg);
    }
    else if (msg_type == OBJ_DEL) {
      process_return = process_delete_message(obj_msg);
    }
    else if (msg_type == OBJ_LOCK) {
      process_return = process_lock_message(obj_msg);
    }
    else if (msg_type == OBJ_UNLOCK) {
      process_return = process_unlock_message(obj_msg);
    }
    else if (msg_type == KILL) {
      process_return = process_kill_message(obj_msg);
    }
    else if (msg_type == PING) {
      process_return = process_ping_message(obj_msg);
    }
    else {
      process_return = "-1";
    }

    return process_return;
  }
};

#endif
