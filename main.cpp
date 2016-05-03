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
#include <Eigen/Dense>

#include "src/event_dispatcher.h"
#include "src/obj3.h"
#include "src/couchbase_admin.h"
#include "src/list.h"
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
int AUB_StartSize;
int AUB_StepSize;
std::string hex_counter;
int key_counter;

//Global Object List
//Necessary to implement smart updates
List<Obj3> *active_updates;

//Global Couchbase Admin Object
CouchbaseAdmin *cb;

//Global Outbound ZMQ Dispatcher
zmq::socket_t *zmqo;

//-----------------------
//----Utility Methods----
//-----------------------

void send_zmqo_message(const char * msg)
{
        //Get the size of the buffer being passed in
        int buffer_size;
        buffer_size = strlen(msg);

        //Set up the message to go out on 0MQ
        zmq::message_t request (buffer_size);
        memcpy (request.data (), msg, buffer_size);

        //Send the message
        zmqo->send (request);

        //  Get the reply.
        zmq::message_t reply;
        zmq::message_t *rep_ptr;
        rep_ptr = &reply;
        zmqo->recv (rep_ptr);

        //Process the reply
        std::string r_str = hexDump(reply);

        logging->info("ZMQ:Message Sent:");
        logging->info(msg);
        logging->info("ZMQ:Response Recieved:");
        logging->info(r_str);
}

void send_zmqo_str_message(std::string msg) {
	send_zmqo_message(msg.c_str());
}

//Is a key present in the smart update buffer?
int find_key_in_active_updates(const char * key) {
  int list_length = active_updates->length();
  int i;
  Obj3 tobj;
  Obj3 *temp_obj;
  for (i = 0; i < list_length; i=i+1) {
    tobj = active_updates->get(i);
    temp_obj = &tobj;
    if (strcmp(temp_obj->get_key().c_str(), key) == 0) {
      return i;
    }
  }
  return -1;
}

//Build Obj3 from a Rapidjson Document
Obj3 build_object(const rapidjson::Document& d) {
    logging->debug("Build Object Called");
    if (d.IsObject()) {
    logging->debug("Object-Format Message Detected");
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
    logging->debug("New Variables Declared");

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
        std::vector<std::string> scene_list;
        if (sc.IsArray()) {
                for (rapidjson::SizeType i = 0; i < sc.Size();i++) {
                        scene_list.push_back(sc[i].GetString());
                }
        }
    }


    logging->debug("Variables Filled");

    //Build the Obj3 and return it from the populated values
    Obj3 object (new_name, new_key, new_type, new_subtype, new_owner, new_location, new_rotatione, new_rotationq, new_scale, new_transform, new_bounding_box);
    logging->debug("Obj3 Built");
    return object;
}
}

//-----------------------
//---Callback Methods----
//-----------------------

//Couchbase Callbacks

//The storage callback is simple and outputs a message on the ZMQ
//Port assuming that an update has been made
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
    int key_index = find_key_in_active_updates(k);
    if (key_index > -1) {
      //We need to update the object in the DB, then output the object
      //On the Outbound ZeroMQ port.

      //Let's get the object out of the active update list
      Obj3 *temp_obj;
      Obj3 tobj;
      tobj = active_updates->get(key_index);
      temp_obj = &tobj;

      //Then, let's get and parse the response from the database
      rapidjson::Document temp_d;
      temp_d.Parse(resp_obj);
      Obj3 new_obj = build_object (temp_d);

      //Now, we can compare the two and apply any updates from the
      //object list to the object returned from the database

      //First, we apply any matrix transforms present
      if (temp_obj->get_locx() > 0.0001 || temp_obj->get_locy() > 0.0001 || temp_obj->get_locz() > 0.0001) {
        new_obj.translate(temp_obj->get_locx(), temp_obj->get_locy(), temp_obj->get_locz(), "Global");
      }
	
      if (temp_obj->get_rotex() > 0.0001 || temp_obj->get_rotey() > 0.0001 || temp_obj->get_rotez() > 0.0001) {
       new_obj.rotatee(temp_obj->get_rotex(), temp_obj->get_rotey(), temp_obj->get_rotez(), "Global");
      }

      if (temp_obj->get_rotqw() > 0.0001 || temp_obj->get_rotqx() > 0.0001 || temp_obj->get_rotqy() > 0.0001 || temp_obj->get_rotqz() > 0.0001) {
       new_obj.rotateq(temp_obj->get_rotqw(), temp_obj->get_rotqx(), temp_obj->get_rotqy(), temp_obj->get_rotqz(), "Global");
      }

      if (temp_obj->get_sclx() > 0.0001 || temp_obj->get_scly() > 0.0001 || temp_obj->get_sclz() > 0.0001) {
        new_obj.resize(temp_obj->get_sclx(), temp_obj->get_scly(), temp_obj->get_sclz());
      }

      new_obj.transform_object(temp_obj->get_transform());

      new_obj.apply_transforms();

      //Next, we write any string attributes
      if (temp_obj->get_owner() != "") {
        new_obj.set_owner(temp_obj->get_owner());
      }

      if (temp_obj->get_name() != "") {
        new_obj.set_name(temp_obj->get_name());
      }

      if (temp_obj->get_type() != "") {
        new_obj.set_type(temp_obj->get_type());
      }

      if (temp_obj->get_subtype() != "") {
        new_obj.set_subtype(temp_obj->get_subtype());
      }     

      //Finally, we write the result back to the database
      Obj3 *obj_ptr = &new_obj;

      //And output the message on the ZMQ Port
      send_zmqo_str_message(new_obj.to_json_msg(2));

      cb->save_object (obj_ptr);

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
      Obj3 new_obj = build_object (temp_d);
      send_zmqo_str_message(new_obj.to_json_msg(1));
    }
	}
	else {
		logging->error("Couldn't retrieve item:");
        	logging->error(lcb_strerror(instance, err));
	}
}

//Document Event Callbacks
void create_objectd(rapidjson::Document& d) {
        logging->info("Create object called with document: ");

        if (d.HasMember("location") && d.HasMember("bounding_box") && d.HasMember("scenes")) {

		//Iterate the Hex Counter value
		key_counter++;

          //Build the object and the key
          Obj3 new_obj = build_object (d);
		  std::ostringstream ss;
		  ss << key_counter;
	  new_obj.set_key(ss.str());

	  //Output a message on the outbound ZMQ Port
          send_zmqo_str_message(new_obj.to_json_msg(0));
          
          //Save the object to the couchbase DB
//          Obj3 *obj_ptr = &new_obj;
          cb->create_object (new_obj);
        }
        else {
          logging->error("Create Message recieved without location, bounding box, or scene");
        }
}

void update_objectd(rapidjson::Document& d) {
        logging->info("Update object called with document: ");
        if (d.HasMember("key")) {
          //Update the object in the DB
          if (SmartUpdatesActive) {
            //We start by writing the object into the smart update buffer
            //then, we can issue a get call
            
            //upon returning, the get callback should 
            //check the smart update buffer for a matching key.

            //If it is found, we update the DB Entry.  
            //Else, we simply output the value retrieved from the DB

            //Check if the object already exists in the smart update buffer.
            //If so, reject the update.
	    Obj3 temp_obj = build_object (d);
            const char *temp_key = temp_obj.get_key().c_str();
            if (find_key_in_active_updates(temp_key) == -1) {
              active_updates->append(temp_obj);

              cb->load_object(temp_key);
            }
            else {
              logging->error("Collision in Active Update Buffer Detected");
              logging->error("Key:");
              logging->error(temp_obj.get_key());
            }
          }
          else {
            //If smart updates are disabled, we can just write the value directly
            //To the DB
            Obj3 new_obj = build_object (d);
            Obj3 *obj_ptr = &new_obj;

            send_zmqo_str_message(new_obj.to_json_msg(2));

            cb->save_object (obj_ptr);
          }
        }
        else {
          logging->error("Message Recieved without key");
        }
}

void get_objectd(rapidjson::Document& d) {
        logging->info("Get object called with document: ");
        if (d.HasMember("key")) {
	  rapidjson::Value *rkey;
          rkey = &d["key"];
          //Check the Active Update Buffer for inflight transactions
          //If we have any, then we should pull the value from there
          //And return it. 
          if (SmartUpdatesActive) {
            int key_index = find_key_in_active_updates(rkey->GetString());
            if (key_index > -1) {

              //:Pull the value from the update buffer
              Obj3 tobj = active_updates->get(key_index);

              //Return the object on the outbound ZMQ Port
              send_zmqo_str_message(tobj.to_json_msg(2));
            }
            else {
              //Otherwise, Get the object from the DB
              cb->load_object( rkey->GetString() );
            }
          }
          else {
            //Otherwise, Get the object from the DB
            cb->load_object( rkey->GetString() );
          }
        }
        else {
          logging->error("Message Recieved without key");
        }
}

void delete_objectd(rapidjson::Document& d) {
        logging->info("Delete object called with document: ");
        if (d.HasMember("key")) {

          rapidjson::Value *val;
          val = &d["key"];
          //Delete the object from the DB
          cb->delete_object( val->GetString() );

          //Output a delete message on the outbound ZMQ Port

          //Initialize the string buffer and writer
          rapidjson::StringBuffer s;
          rapidjson::Writer<rapidjson::StringBuffer> writer(s);

          writer.StartObject();

          writer.Key("message_type");
          writer.Uint(3);

          writer.Key("key");
          std::string key = val->GetString();
          writer.String( key.c_str(), (rapidjson::SizeType)key.length() );

          writer.EndObject();

          //The Stringbuffer now contains a json message
          //of the object
          send_zmqo_message(val->GetString());

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
AUB_StartSize=25;
AUB_StepSize=15;

hex_counter="0x00";
key_counter=0x00;

//Open the key counter file
logging->info("Opening counter.properties");
std::string counter_line;
std::ifstream c_file ("counter.properties");

if (c_file.is_open()) {
        while (getline (c_file, counter_line) ) {
                //Read a line from the property file
                logging->debug("Line read from configuration file:");
                logging->debug(counter_line);

                //Figure out if we have a blank or comment line
                bool k_going = true;
                if (counter_line.length() > 0) {
                        if (counter_line[0] == '/' && counter_line[1] == '/') {
                                k_going=false;
                        }
                }
                else {
                        k_going=false;
                }

                if (k_going==true) {
			try {
				hex_counter=counter_line;
				key_counter=std::stoi(hex_counter, 0, hex_counter.length());
			}
			catch (std::exception& e) {
				logging->error("Exception encountered parsing hex counter value");
				logging->error(e.what());
			}
		}
	}
        c_file.close();
}


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
			else if (var_name=="ActiveUpdateBuffer_StartSize") {
				AUB_StartSize = atoi(var_value.c_str());
			}
			else if (var_name=="ActiveUpdateBuffer_StepSize") {
				AUB_StepSize = atoi(var_value.c_str());
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

//Active Update List
List<Obj3> up_list (AUB_StartSize, AUB_StepSize);
active_updates = &up_list;

//Set up the Couchbase Connection
CouchbaseAdmin c ( DB_ConnStr.c_str() );
cb = &c;
logging->info("Connected to Couchbase DB");

//Bind Couchbase Callbacks
lcb_set_store_callback(cb->get_instance(), storage_callback);
lcb_set_get_callback(cb->get_instance(), get_callback);

zmq::context_t context(1);

//Set up the outbound ZMQ Client
zmq::socket_t zout(context, ZMQ_REQ);
logging->info("0MQ Constructor Called");
zout.connect(OMQ_OBConnStr);
zmqo = &zout;
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

	//Process the message header and set current_event_type
        try {
	    d.Parse(req_ptr);
        }
        catch (std::exception& e) {
            logging->error("Exception occurred while parsing inbound document:");
            logging->error(e.what());
        }
        //TO-DO: Catch a possible error and write to logs
	s = &d["message_type"];
	msg_type = s->GetInt();

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
		std::ofstream myfile;
		myfile.open("counters.tmp");

		std::ostringstream ss;
		ss << key_counter;

		myfile << ss.str();
		myfile << "\n";
		int result;
		result = rename("counters.tmp", "counters.properties");
		if (result == 0) {
			logging->info("File Renamed");
		}
		else {
			logging->error("Counter File Rename unsuccessful");
		}
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
		logging->debug("Response Sent, terminating");
		return 0;
	}
        else {
                current_event_type=-1;
		logging->error("Current Event Type not found");
        }


        //Emit an event based on the event type & build the response message
        if (current_event_type==OBJ_UPD) {
		update_objectd( d );
                resp[0]='s';
		resp[1]='u';
		resp[2]='c';
		resp[3]='c';
		resp[4]='e';
		resp[5]='s';
		resp[6]='s';
		logging->debug("Object Update Event Emitted, response:");
		logging->debug(resp);
        }
        else if (current_event_type==OBJ_CRT) {
		create_objectd( d );
		resp[0]='s';
                resp[1]='u';
                resp[2]='c';
                resp[3]='c';
                resp[4]='e';
                resp[5]='s';
                resp[6]='s';
		logging->debug("Object Create Event Emitted, response: ");
		logging->debug(resp);
        }
	else if (current_event_type==OBJ_GET) {
		get_objectd( d );
		resp[0]='s';
                resp[1]='u';
                resp[2]='c';
                resp[3]='c';
                resp[4]='e';
                resp[5]='s';
                resp[6]='s';
		logging->debug("Object Get Event Emitted, response: ");
		logging->debug(resp);
        }
        else if (current_event_type==OBJ_DEL) {
		delete_objectd( d );
		resp[0]='s';
                resp[1]='u';
                resp[2]='c';
                resp[3]='c';
                resp[4]='e';
                resp[5]='s';
                resp[6]='s';
		logging->debug("Object Delete Event Emitted, response: ");
		logging->debug(resp);
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
        }

        //  Send reply back to client
        zmq::message_t reply (8);

        //Prepare return data
        memcpy (reply.data (), resp, 8);

	//Send the response
        socket.send (reply);
	logging->debug("Response Sent");
		cb->wait();
        }

return 0;
}
