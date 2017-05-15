#include "include/obj3.h"

// Default Constructor
Obj3::Obj3() {
  //Initialize Empty String elements
  key="";
  name="";
  scene_id="";
  type="";
  subtype="";
  owner="";
}

//Constructor to parse a JSON from Mongo
Obj3::Obj3(const rapidjson::Document &d) {
  obj_logging->debug("Building Obj3 from JSON Document");
  //Initialize Empty String elements
  key="";
  name="";
  scene_id="";
  type="";
  subtype="";
  owner="";
  //Start parsing the JSON Object
  if (d.IsObject()) {

    obj_logging->debug("Object-Format Message Detected");

    const rapidjson::Value *key_val = &d["_id"];
    key = key_val->GetString();
    if (d.HasMember("name")) {
      const rapidjson::Value *name_val = &d["name"];
      name = name_val->GetString();
    }
    if (d.HasMember("scene")) {
      const rapidjson::Value *scene_val = &d["scene"];
      scene_id = scene_val->GetString();
    }
    if (d.HasMember("owner")) {
      const rapidjson::Value *owner_val = &d["owner"];
      owner = owner_val->GetString();
    }
    if (d.HasMember("type")) {
      const rapidjson::Value *type_val = &d["type"];
      type = type_val->GetString();
    }
    if (d.HasMember("subtype")) {
      const rapidjson::Value *subtype_val = &d["subtype"];
      subtype = subtype_val->GetString();
    }

    //Transformations
    if (d.HasMember("translation")) {
      const rapidjson::Value& trans_val = d["translation"];
      if (trans_val.IsArray()) {
        trans = new Translation;
        trans->set_x(trans_val[0].GetDouble());
        trans->set_y(trans_val[1].GetDouble());
        trans->set_z(trans_val[2].GetDouble());
      }
    }

    if (d.HasMember("euler_rotation")) {
      const rapidjson::Value& erot_val = d["euler_rotation"];
      if (erot_val.IsArray()) {
        erot = new EulerRotation;
        erot->set_x(erot_val[0].GetDouble());
        erot->set_y(erot_val[1].GetDouble());
        erot->set_z(erot_val[2].GetDouble());
      }
    }

    if (d.HasMember("quaternion_rotation")) {
      const rapidjson::Value& qrot_val = d["quaternion_rotation"];
      if (qrot_val.IsArray()) {
        qrot = new QuaternionRotation;
        qrot->set_w(qrot_val[0].GetDouble());
        qrot->set_x(qrot_val[1].GetDouble());
        qrot->set_y(qrot_val[2].GetDouble());
        qrot->set_z(qrot_val[3].GetDouble());
      }
    }

    if (d.HasMember("scale")) {
      const rapidjson::Value& scl_val = d["scale"];
      if (scl_val.IsArray()) {
        scl = new Scale;
        scl->set_x(scl_val[0].GetDouble());
        scl->set_y(scl_val[1].GetDouble());
        scl->set_z(scl_val[2].GetDouble());
      }
    }

    if (d.HasMember("assets")) {
      //Read the array values and stuff them into new_location
      const rapidjson::Value& sc = d["assets"];
      if (sc.IsArray()) {
        for (rapidjson::SizeType i = 0; i < sc.Size();i++) {
          asset_ids.push_back(sc[i].GetString());
        }
      }
    }

  }
}

//Copy Constructor
Obj3::Obj3(const Obj3 &o) {
  //Copy String values
  key = o.get_key();
  name = o.get_name();
  scene_id = o.get_scene();
  type = o.get_type();
  subtype = o.get_subtype();
  owner = o.get_owner();
  //Apply transforms
  if (o.has_translation()) {
    trans = new Translation;
    transform(o.get_translation());
  }
  if (o.has_translation()) {
    erot = new EulerRotation;
    transform(o.get_erotation());
  }
  if (o.has_translation()) {
    qrot = new QuaternionRotation;
    transform(o.get_qrotation());
  }
  if (o.has_translation()) {
    scl = new Scale;
    transform(o.get_scale());
  }
  //Move over asset ids
  for (int i=0;i<o.num_assets();i++) {
    add_asset(o.get_asset(i));
  }
}

// Destructor
Obj3::~Obj3() {
  if (trans) delete trans;
  if (erot) delete erot;
  if (qrot) delete qrot;
  if (scl) delete scl;
}

// Apply a transformation to the object
//Depending on the type of transformation, we allocate the needed memory or add to existing
void Obj3::transform(Transformation *t) {
  if (t->get_type() == TRANSLATE) {
    if (!trans) trans = new Translation;
    trans->add(t->get_x(), t->get_y(), t->get_z());
  } else if (t->get_type() == EROTATE) {
    if (!erot) erot = new EulerRotation;
    erot->add(t->get_x(), t->get_y(), t->get_z());
  } else if (t->get_type() == QROTATE) {
    if (!qrot) qrot = new QuaternionRotation;
    qrot->add(t->get_w(), t->get_x(), t->get_y(), t->get_z());
  } else if (t->get_type() == SCALE) {
    if (!scl) scl = new Scale;
    scl->add(t->get_x(), t->get_y(), t->get_z());
  } else {
    throw Obj3Exception("Attempted to apply transformation without a valid type");
  }
}

//Take the target and apply its fields as changes
void Obj3::merge(Obj3 *target) {
  //Copy String values
  if ( !(target->get_key().empty()) ) key = target->get_key();
  if ( !(target->get_name().empty()) ) name = target->get_name();
  if ( !(target->get_scene().empty()) ) scene_id = target->get_scene();
  if ( !(target->get_type().empty()) ) type = target->get_type();
  if ( !(target->get_subtype().empty()) ) subtype = target->get_subtype();
  if ( !(target->get_owner().empty()) ) owner = target->get_owner();
  //Apply transforms
  if (target->has_translation()) {
    if (!trans) trans = new Translation;
    transform(target->get_translation());
  }
  if (target->has_erotation()) {
    if (!erot) erot = new EulerRotation;
    transform(target->get_erotation());
  }
  if (target->has_qrotation()) {
    if (!qrot) qrot = new QuaternionRotation;
    transform(target->get_qrotation());
  }
  if (target->has_scale()) {
    if (!scl) scl = new Scale;
    transform(target->get_scale());
  }
  //Move over asset ids
  for (int i=0;i<target->num_assets();i++) {
    add_asset(target->get_asset(i));
  }
}

//Write the Object to JSON
std::string Obj3::to_json() {
  //Initialize the string buffer and writer
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  //Start writing the object
  //Syntax taken directly from
  //simplewriter.cpp in rapidjson examples
  writer.StartObject();

  //Write string attributes
  if ( !(key.empty()) ) {
  	writer.Key("_id");
  	writer.String( key.c_str(), (rapidjson::SizeType)key.length() );
  }

  if ( !(name.empty()) ) {
  	writer.Key("name");
  	writer.String( name.c_str(), (rapidjson::SizeType)name.length() );
  }

  if ( !(scene_id.empty()) ) {
  	writer.Key("scene");
  	writer.String( scene_id.c_str(), (rapidjson::SizeType)scene_id.length() );
  }

  if ( !(type.empty()) ) {
  	writer.Key("type");
  	writer.String( type.c_str(), (rapidjson::SizeType)type.length() );
  }

  if ( !(subtype.empty()) ) {
  	writer.Key("subtype");
  	writer.String( subtype.c_str(), (rapidjson::SizeType)subtype.length() );
  }

  if ( !(owner.empty()) ) {
  	writer.Key("owner");
  	writer.String( owner.c_str(), (rapidjson::SizeType)owner.length() );
  }

  //Write Transforms
  if (trans) {
    writer.Key("translation");
    writer.StartArray();
    writer.Double( trans->get_x() );
    writer.Double( trans->get_y() );
    writer.Double( trans->get_z() );
    writer.EndArray();
  }

  if (erot) {
    writer.Key("euler_rotation");
    writer.StartArray();
    writer.Double( erot->get_x() );
    writer.Double( erot->get_y() );
    writer.Double( erot->get_z() );
    writer.EndArray();
  }

  if (qrot) {
    writer.Key("quaternion_rotation");
    writer.StartArray();
    writer.Double( qrot->get_w() );
    writer.Double( qrot->get_x() );
    writer.Double( qrot->get_y() );
    writer.Double( qrot->get_z() );
    writer.EndArray();
  }

  if (scl) {
    writer.Key("scale");
    writer.StartArray();
    writer.Double( scl->get_x() );
    writer.Double( scl->get_y() );
    writer.Double( scl->get_z() );
    writer.EndArray();
  }

  writer.Key("assets");
  writer.StartArray();
  for (int i=0; i<num_assets(); i++) {
    std::string ast = get_asset(i);
    writer.String( ast.c_str(), (rapidjson::SizeType)ast.length() );
  }
  writer.EndArray();

  writer.EndObject();

  //The Stringbuffer now contains a json message
  //of the object
  json_cstr_val = s.GetString();
  json_str_val.assign(json_cstr_val);
  return json_str_val;

}
