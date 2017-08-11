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

#include "include/protobuf_object_list.h"

// Constructor from a protocol buffer object
PbObjectList::PbObjectList(protoObj3::Obj3List proto_list) {
  obj_logging->debug("Building ObjectList from Protocol Buffer String");

  // Basic Message attributes
  if (!(proto_list.has_message_type())) {
    throw Object3dException("No Message Type Found");
  }
  set_msg_type(proto_list.message_type());
  if (proto_list.has_transaction_id()) {
    set_transaction_id(proto_list.transaction_id());
  }
  if (proto_list.has_num_records()) {
    set_num_records(proto_list.num_records());
  }

  // Object List
  for (int i = 0; i < proto_list.objects_size(); i++) {
    obj_logging->debug("Building object in Objects List");

    ObjectInterface *o = ofactory.build_object();

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
      Translation *int_translation = \
        new Translation(msg_trans->x(), msg_trans->y(), msg_trans->z());
      o->transform(int_translation);
      delete int_translation;
    }

    if (obj->has_rotation_euler()) {
      obj_logging->debug("Pulling Euler Rotation");
      protoObj3::Obj3List_Vertex4 *msg_rote = obj->mutable_rotation_euler();
      EulerRotation *int_erotation = new EulerRotation(msg_rote->theta(), \
        msg_rote->x(), msg_rote->y(), msg_rote->z());
      o->transform(int_erotation);
      delete int_erotation;
    }

    if (obj->has_scale()) {
      obj_logging->debug("Pulling Scale");
      protoObj3::Obj3List_Vertex3 *msg_scale = obj->mutable_scale();
      Scale *int_scale = \
        new Scale(msg_scale->x(), msg_scale->y(), msg_scale->z());
      o->transform(int_scale);
      delete int_scale;
    }

    // Assets
    for (int j = 0; j < obj->asset_ids_size(); j++) {
      o->add_asset(obj->asset_ids(j));
    }

    add_object(o);
  }
}

// Write a protocol buffer string from the object list
void PbObjectList::to_msg_string(std::string &out_string) {
  obj_logging->debug("Writing to Protocol Buffer");

  // Create a new protocol buffer object
  protoObj3::Obj3List *new_proto = new protoObj3::Obj3List;

  // Set basic list attributes
  new_proto->set_message_type(get_msg_type());
  new_proto->set_err_code(get_error_code());
  new_proto->set_num_records(num_objects());
  if (!(get_transaction_id().empty())) \
    {new_proto->set_transaction_id(get_transaction_id());}
  if (!(get_error_message().empty())) \
    {new_proto->set_err_msg(get_error_message());}

  obj_logging->debug("Basic Attributes Written");

  // Set object list
  for (int i = 0; i < num_objects(); i++) {
    obj_logging->debug("Writing Object in Objects Array");

    // Add a new object to the list
    protoObj3::Obj3List_Obj3 *new_obj = new_proto->add_objects();

    // Set basic object attributes
    if (!(get_object(i)->get_key().empty())) {
      new_obj->set_key(get_object(i)->get_key());
    }
    if (!(get_object(i)->get_name().empty())) {
      new_obj->set_name(get_object(i)->get_name());
    }
    if (!(get_object(i)->get_scene().empty())) {
      new_obj->set_scene_id(get_object(i)->get_scene());
    }
    if (!(get_object(i)->get_type().empty())) {
      new_obj->set_type(get_object(i)->get_type());
    }
    if (!(get_object(i)->get_subtype().empty())) {
      new_obj->set_subtype(get_object(i)->get_subtype());
    }
    if (!(get_object(i)->get_owner().empty())) {
      new_obj->set_owner(get_object(i)->get_owner());
    }

    obj_logging->debug("Basic attributes written");

    // Set transform attributes
    obj_logging->debug("Writing Transform Matrix");
    protoObj3::Obj3List_Matrix4 *new_transform = new_obj->mutable_transform();
    for (int j = 0; j < 4; j++) {
      protoObj3::Obj3List_MatrixRow *mr = new_transform->add_row();
      mr->set_w(get_object(i)->get_transform()->get_transform_element(j, 0));
      mr->set_x(get_object(i)->get_transform()->get_transform_element(j, 1));
      mr->set_y(get_object(i)->get_transform()->get_transform_element(j, 2));
      mr->set_z(get_object(i)->get_transform()->get_transform_element(j, 3));
    }

    // Set Asset list
    for (int k = 0; k < get_object(i)->num_assets(); k++)
      {new_obj->add_asset_ids(get_object(i)->get_asset(k));}
  }

  new_proto->SerializeToString(&out_string);
  delete new_proto;
}
