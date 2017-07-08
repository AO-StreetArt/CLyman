/*
Apache2 License Notice
Copyright 2017 Alex Barry

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "include/obj3_list.h"

// Constructor
Obj3List::Obj3List() {
  msg_type = -1;
  err_code = 100;
  err_msg = "";
  transaction_id = "";
  num_records = 0;
}

Obj3List::~Obj3List() {
  for (unsigned int i = 0; i < objects.size(); i++) {
    if (objects[i]) delete objects[i];
  }
}

// Constructor from a parsed JSON Document
// Not going to parse error code or message as we dont care about them coming
// in, only going out
Obj3List::Obj3List(const rapidjson::Document& d) {
  obj_logging->debug("Building Obj3List from JSON Document");
  // Initialize Empty String elements
  msg_type = -1;
  err_code = 100;
  err_msg = "";
  transaction_id = "";
  num_records = 0;
  // Start parsing the JSON Object
  if (d.IsObject()) {
    obj_logging->debug("Object-Format Message Detected");

    // Parse the base elements
    if (!(d.HasMember("msg_type"))) throw Obj3Exception("No Msg Type Found");
    const rapidjson::Value *mtype_val = &d["msg_type"];
    msg_type = mtype_val->GetInt();
    if (d.HasMember("transaction_id")) {
      const rapidjson::Value *tid_val = &d["transaction_id"];
      transaction_id = tid_val->GetString();
      obj_logging->debug("Transaction ID Pulled");
    }
    if (d.HasMember("num_records")) {
      const rapidjson::Value *nr_val = &d["num_records"];
      num_records = nr_val->GetInt();
      obj_logging->debug("Num Records Pulled");
    }

    // Parse the object list
    if (d.HasMember("objects")) {
      const rapidjson::Value *objs_val = &d["objects"];
      obj_logging->debug("Objects Array pulled");
      if (objs_val->IsArray()) {
        for (auto& itr : objs_val->GetArray()) {
          obj_logging->debug("Object Returned from objects array");
          // Create a new object
          Obj3 *new_obj = new Obj3;

          // Parse the string attributes
          rapidjson::Value::ConstMemberIterator key_iter = \
            itr.FindMember("key");
          if (key_iter != itr.MemberEnd()) {
            new_obj->set_key(key_iter->value.GetString());
          }

          rapidjson::Value::ConstMemberIterator name_iter = \
            itr.FindMember("name");
          if (name_iter != itr.MemberEnd()) {
            new_obj->set_name(name_iter->value.GetString());
          }

          rapidjson::Value::ConstMemberIterator scn_iter = \
            itr.FindMember("scene");
          if (scn_iter != itr.MemberEnd()) {
            new_obj->set_scene(scn_iter->value.GetString());
          }

          rapidjson::Value::ConstMemberIterator owner_iter = \
            itr.FindMember("owner");
          if (owner_iter != itr.MemberEnd()) {
            new_obj->set_owner(owner_iter->value.GetString());
          }

          rapidjson::Value::ConstMemberIterator type_iter = \
            itr.FindMember("type");
          if (type_iter != itr.MemberEnd()) {
            new_obj->set_type(type_iter->value.GetString());
          }

          rapidjson::Value::ConstMemberIterator subtype_iter = \
            itr.FindMember("subtype");
          if (subtype_iter != itr.MemberEnd()) {
            new_obj->set_subtype(subtype_iter->value.GetString());
          }

          obj_logging->debug("Basic string values pulled");

          // Parse the transform elements
          rapidjson::Value::ConstMemberIterator translation_iter = \
            itr.FindMember("translation");
          if (translation_iter != itr.MemberEnd()) {
            obj_logging->debug("Translation pulled");
            const rapidjson::Value& translation_val = translation_iter->value;
            Translation *trans = new Translation;
            int i = 0;
            for (auto& trans_itr : translation_val.GetArray()) {
              if (i == 0) {
                trans->set_x(trans_itr.GetDouble());
              } else if (i == 1) {
                trans->set_y(trans_itr.GetDouble());
              } else if (i == 2) {trans->set_z(trans_itr.GetDouble());}
              i++;
            }
            new_obj->transform(trans);
            delete trans;
          }

          rapidjson::Value::ConstMemberIterator erot_iter = \
            itr.FindMember("euler_rotation");
          if (erot_iter != itr.MemberEnd()) {
            obj_logging->debug("Euler Rotation pulled");
            const rapidjson::Value& erot_val = erot_iter->value;
            EulerRotation *erot = new EulerRotation;
            int i = 0;
            for (auto& erot_itr : erot_val.GetArray()) {
              if (i == 0) {
                erot->set_x(erot_itr.GetDouble());
              } else if (i == 1) {
                erot->set_y(erot_itr.GetDouble());
              } else if (i == 2) {erot->set_z(erot_itr.GetDouble());}
              i++;
            }
            new_obj->transform(erot);
            delete erot;
          }

          rapidjson::Value::ConstMemberIterator qrot_iter = \
            itr.FindMember("quaternion_rotation");
          if (qrot_iter != itr.MemberEnd()) {
            obj_logging->debug("Quaternion Rotation pulled");
            const rapidjson::Value& qrot_val = qrot_iter->value;
            QuaternionRotation *qrot = new QuaternionRotation;
            int i = 0;
            for (auto& qrot_itr : qrot_val.GetArray()) {
              if (i == 0) {
                qrot->set_w(qrot_itr.GetDouble());
              } else if (i == 1) {
                qrot->set_x(qrot_itr.GetDouble());
              } else if (i == 2) {qrot->set_y(qrot_itr.GetDouble());
              } else if (i == 3) {qrot->set_z(qrot_itr.GetDouble());}
              i++;
            }
            new_obj->transform(qrot);
            delete qrot;
          }

          rapidjson::Value::ConstMemberIterator scale_iter = \
            itr.FindMember("scale");
          if (scale_iter != itr.MemberEnd()) {
            obj_logging->debug("Scale pulled");
            const rapidjson::Value& scale_val = scale_iter->value;
            Scale *scl = new Scale;
            int i = 0;
            for (auto& scale_itr : scale_val.GetArray()) {
              if (i == 0) {
                scl->set_x(scale_itr.GetDouble());
              } else if (i == 1) {
                scl->set_y(scale_itr.GetDouble());
              } else if (i == 2) {scl->set_z(scale_itr.GetDouble());}
              i++;
            }
            new_obj->transform(scl);
            delete scl;
          }

          objects.push_back(new_obj);
        }
      }
    // d.HasMember(objects)
    }

  // d.IsObject
  }
}

// Constructor from a protocol buffer object
Obj3List::Obj3List(protoObj3::Obj3List proto_list) {
  obj_logging->debug("Building OBj3List from Protocol Buffer String");

  // Basic Message attributes
  if (!(proto_list.has_message_type())) {
    throw Obj3Exception("No Message Type Found");
  }
  msg_type = proto_list.message_type();
  if (proto_list.has_transaction_id()) {
    transaction_id = proto_list.transaction_id();
  }
  if (proto_list.has_num_records()) {
    num_records = proto_list.num_records();
  }

  // Object List
  for (int i = 0; i < proto_list.objects_size(); i++) {
    obj_logging->debug("Building object in Objects List");

    Obj3 *o = new Obj3;

    // Retrieve the object from the list
    protoObj3::Obj3List_Obj3 *obj = proto_list.mutable_objects(i);

    // Basic attributes
    if (obj->has_key()) o->set_key(obj->key());
    if (obj->has_name()) o->set_name(obj->name());
    if (obj->has_scene_id()) o->set_scene(obj->scene_id());
    if (obj->has_type()) o->set_type(obj->type());
    if (obj->has_subtype()) o->set_subtype(obj->subtype());
    if (obj->has_owner()) o->set_owner(obj->owner());

    obj_logging->debug("Basic Attributes Pulled");

    // Transforms
    if (obj->has_translation()) {
      obj_logging->debug("Pulling Translation");
      protoObj3::Obj3List_Vertex3 *msg_trans = obj->mutable_translation();
      Translation *int_translation = new Translation;
      int_translation->set_x(msg_trans->x());
      int_translation->set_y(msg_trans->y());
      int_translation->set_z(msg_trans->z());
      o->transform(int_translation);
      delete int_translation;
    }

    if (obj->has_rotation_euler()) {
      obj_logging->debug("Pulling Euler Rotation");
      protoObj3::Obj3List_Vertex3 *msg_rote = obj->mutable_rotation_euler();
      EulerRotation *int_erotation = new EulerRotation;
      int_erotation->set_x(msg_rote->x());
      int_erotation->set_y(msg_rote->y());
      int_erotation->set_z(msg_rote->z());
      o->transform(int_erotation);
      delete int_erotation;
    }

    if (obj->has_rotation_quaternion()) {
      obj_logging->debug("Pulling Quaternion Rotation");
      protoObj3::Obj3List_Vertex4 *msg_rotq = \
        obj->mutable_rotation_quaternion();
      QuaternionRotation *int_qrotation = new QuaternionRotation;
      int_qrotation->set_w(msg_rotq->w());
      int_qrotation->set_x(msg_rotq->x());
      int_qrotation->set_y(msg_rotq->y());
      int_qrotation->set_z(msg_rotq->z());
      o->transform(int_qrotation);
      delete int_qrotation;
    }

    if (obj->has_scale()) {
      obj_logging->debug("Pulling Scale");
      protoObj3::Obj3List_Vertex3 *msg_scale = obj->mutable_scale();
      Scale *int_scale = new Scale;
      int_scale->set_x(msg_scale->x());
      int_scale->set_y(msg_scale->y());
      int_scale->set_z(msg_scale->z());
      o->transform(int_scale);
      delete int_scale;
    }

    // Assets
    for (int j = 0; j < obj->asset_ids_size(); j++) {
      o->add_asset(obj->asset_ids(j));
    }

    objects.push_back(o);
  }
}

// Copy Constructor
Obj3List::Obj3List(const Obj3List &olist) {
  obj_logging->debug("Copy Constructor Called");
  msg_type = olist.get_msg_type();
  err_code = olist.get_error_code();
  err_msg = olist.get_error_message();
  transaction_id = olist.get_transaction_id();
  num_records = olist.get_num_records();
  for (int i = 0; i < olist.num_objects(); i++) {
    // Invoke the Obj3 Copy Constructor
    Obj3 *o = new Obj3(*(olist.get_object(i)));
    objects.push_back(o);
  }
}

// write a JSON string from the object list
std::string Obj3List::to_json() {
  obj_logging->debug("Writing Object to JSON");

  // Initialize the string buffer and writer
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  // Start writing the object
  // Syntax taken directly from
  // simplewriter.cpp in rapidjson examples
  writer.StartObject();

  // Add integer elements
  writer.Key("msg_type");
  writer.Uint(msg_type);
  writer.Key("err_code");
  writer.Uint(err_code);
  writer.Key("num_records");
  writer.Uint(objects.size());

  // Add the error message
  if (!err_msg.empty()) {
  writer.Key("err_msg");
  writer.String(err_msg.c_str(), (rapidjson::SizeType)err_msg.length());
  }
  // Add the Transaction ID
  if (!(transaction_id.empty())) {
  writer.Key("transaction_id");
  writer.String(transaction_id.c_str(), \
      (rapidjson::SizeType)transaction_id.length());
  }

  obj_logging->debug("Basic attributes written");

  // Write the object array
  writer.Key("objects");
  writer.StartArray();

  for (unsigned int a = 0; a < objects.size(); a++) {
    obj_logging->debug("Writing Object into Objects Array");

    writer.StartObject();

    // Write string attributes
    if (!(objects[a]->get_key().empty())) {
      writer.Key("key");
      writer.String(objects[a]->get_key().c_str(), \
        (rapidjson::SizeType)objects[a]->get_key().length());
    }

    if (!(objects[a]->get_name().empty())) {
      writer.Key("name");
      writer.String(objects[a]->get_name().c_str(), \
        (rapidjson::SizeType)objects[a]->get_name().length());
    }

    if (!(objects[a]->get_scene().empty())) {
      writer.Key("scene");
      writer.String(objects[a]->get_scene().c_str(), \
        (rapidjson::SizeType)objects[a]->get_scene().length());
    }

    if (!(objects[a]->get_type().empty())) {
      writer.Key("type");
      writer.String(objects[a]->get_type().c_str(), \
        (rapidjson::SizeType)objects[a]->get_type().length());
    }

    if (!(objects[a]->get_subtype().empty())) {
      writer.Key("subtype");
      writer.String(objects[a]->get_subtype().c_str(), \
        (rapidjson::SizeType)objects[a]->get_subtype().length());
    }

    if (!(objects[a]->get_owner().empty())) {
      writer.Key("owner");
      writer.String(objects[a]->get_owner().c_str(), \
        (rapidjson::SizeType)objects[a]->get_owner().length());
    }

    obj_logging->debug("Basic Object Attributes written");

    // Write transforms
    // Write Transforms
    if (objects[a]->has_translation()) {
      obj_logging->debug("Writing Translation");
      writer.Key("translation");
      writer.StartArray();
      writer.Double(objects[a]->get_translation()->get_x());
      writer.Double(objects[a]->get_translation()->get_y());
      writer.Double(objects[a]->get_translation()->get_z());
      writer.EndArray();
    }

    if (objects[a]->has_erotation()) {
      obj_logging->debug("Writing Euler Rotation");
      writer.Key("euler_rotation");
      writer.StartArray();
      writer.Double(objects[a]->get_erotation()->get_x());
      writer.Double(objects[a]->get_erotation()->get_y());
      writer.Double(objects[a]->get_erotation()->get_z());
      writer.EndArray();
    }

    if (objects[a]->has_qrotation()) {
      obj_logging->debug("Writing Quaternion Rotation");
      writer.Key("quaternion_rotation");
      writer.StartArray();
      writer.Double(objects[a]->get_qrotation()->get_w());
      writer.Double(objects[a]->get_qrotation()->get_x());
      writer.Double(objects[a]->get_qrotation()->get_y());
      writer.Double(objects[a]->get_qrotation()->get_z());
      writer.EndArray();
    }

    if (objects[a]->has_scale()) {
      obj_logging->debug("Writing Scale");
      writer.Key("scale");
      writer.StartArray();
      writer.Double(objects[a]->get_scale()->get_x());
      writer.Double(objects[a]->get_scale()->get_y());
      writer.Double(objects[a]->get_scale()->get_z());
      writer.EndArray();
    }

    writer.EndObject();
  }

  writer.EndArray();

  writer.EndObject();

  // The Stringbuffer now contains a json message
  // of the object
  json_cstr_val = s.GetString();
  json_str_val.assign(json_cstr_val);
  return json_str_val;
}

// Write a protocol buffer string from the object list
std::string Obj3List::to_protobuf() {
  obj_logging->debug("Writing to Protocol Buffer");

  // Create a new protocol buffer object
  protoObj3::Obj3List *new_proto = new protoObj3::Obj3List;

  // Set basic list attributes
  new_proto->set_message_type(msg_type);
  new_proto->set_err_code(err_code);
  new_proto->set_num_records(objects.size());
  if (!(transaction_id.empty())) new_proto->set_transaction_id(transaction_id);
  if (!(err_msg.empty())) new_proto->set_err_msg(err_msg);

  obj_logging->debug("Basic Attributes Written");

  // Set object list
  for (unsigned int i = 0; i < objects.size(); i++) {
    obj_logging->debug("Writing Object in Objects Array");

    // Add a new object to the list
    protoObj3::Obj3List_Obj3 *new_obj = new_proto->add_objects();

    // Set basic object attributes
    if (!(objects[i]->get_key().empty())) {
      new_obj->set_key(objects[i]->get_key());
    }
    if (!(objects[i]->get_name().empty())) {
      new_obj->set_name(objects[i]->get_name());
    }
    if (!(objects[i]->get_scene().empty())) {
      new_obj->set_scene_id(objects[i]->get_scene());
    }
    if (!(objects[i]->get_type().empty())) {
      new_obj->set_type(objects[i]->get_type());
    }
    if (!(objects[i]->get_subtype().empty())) {
      new_obj->set_subtype(objects[i]->get_subtype());
    }
    if (!(objects[i]->get_owner().empty())) {
      new_obj->set_owner(objects[i]->get_owner());
    }

    obj_logging->debug("Basic attributes written");

    // Set transform attributes
    if (objects[i]->has_translation()) {
      obj_logging->debug("Writing Translation");
      protoObj3::Obj3List_Vertex3 *new_trans = new_obj->mutable_translation();
      new_trans->set_x(objects[i]->get_translation()->get_x());
      new_trans->set_y(objects[i]->get_translation()->get_y());
      new_trans->set_z(objects[i]->get_translation()->get_z());
    }

    if (objects[i]->has_erotation()) {
      obj_logging->debug("Writing Euler Rotation");
      protoObj3::Obj3List_Vertex3 *new_erot = new_obj->mutable_rotation_euler();
      new_erot->set_x(objects[i]->get_erotation()->get_x());
      new_erot->set_y(objects[i]->get_erotation()->get_y());
      new_erot->set_z(objects[i]->get_erotation()->get_z());
    }

    if (objects[i]->has_qrotation()) {
      obj_logging->debug("Writing Quaternion Rotation");
      protoObj3::Obj3List_Vertex4 *new_qrot = \
        new_obj->mutable_rotation_quaternion();
      new_qrot->set_w(objects[i]->get_qrotation()->get_w());
      new_qrot->set_x(objects[i]->get_qrotation()->get_x());
      new_qrot->set_y(objects[i]->get_qrotation()->get_y());
      new_qrot->set_z(objects[i]->get_qrotation()->get_z());
    }

    if (objects[i]->has_scale()) {
      obj_logging->debug("Writing Scale");
      protoObj3::Obj3List_Vertex3 *new_scl = new_obj->mutable_scale();
      new_scl->set_x(objects[i]->get_scale()->get_x());
      new_scl->set_y(objects[i]->get_scale()->get_y());
      new_scl->set_z(objects[i]->get_scale()->get_z());
    }

    // Set Asset list
    for (int j = 0; j < objects[i]->num_assets(); j++)
      {new_obj->add_asset_ids(objects[i]->get_asset(j));}
  }

  new_proto->SerializeToString(&protobuf_string);
  delete new_proto;
  return protobuf_string;
}
