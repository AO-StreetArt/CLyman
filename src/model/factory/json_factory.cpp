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

#include "json_factory.h"

void JsonFactory::parse_property_body(std::string& param_name, std::string& param_value, JsonProperty *new_prop) {
  if (param_name.compare("key") == 0) {
    new_prop->set_key(param_value);
    logger.debug("Setting Key on Property: %s", param_value);
  } else if (param_name.compare("name") == 0) {
    new_prop->set_name(param_value);
    logger.debug("Setting Name on Property: %s", param_value);
  } else if (param_name.compare("parent") == 0) {
    new_prop->set_parent(param_value);
    logger.debug("Setting Parent on Property: %s", param_value);
  } else if (param_name.compare("scene") == 0) {
    new_prop->set_scene(param_value);
    logger.debug("Setting Scene on Property: %s", param_value);
  } else if (param_name.compare("asset_sub_id") == 0) {
    new_prop->set_asset_sub_id(param_value);
    logger.debug("Setting Asset Sub ID on Property: %s", param_value);
  }
}

void JsonFactory::parse_obj_body(std::string& param_name, std::string& param_value, JsonObject *new_obj) {
  if (param_name.compare("key") == 0) {
    new_obj->set_key(param_value);
    logger.debug("Setting Key on Object: %s", param_value);
  } else if (param_name.compare("name") == 0) {
    new_obj->set_name(param_value);
    logger.debug("Setting Name on Object: %s", param_value);
  } else if (param_name.compare("parent") == 0) {
    new_obj->set_parent(param_value);
    logger.debug("Setting Parent on Object: %s", param_value);
  } else if (param_name.compare("scene") == 0) {
    new_obj->set_scene(param_value);
    logger.debug("Setting Scene on Object: %s", param_value);
  } else if (param_name.compare("asset_sub_id") == 0) {
    new_obj->set_asset_sub_id(param_value);
    logger.debug("Setting Asset Sub ID on Object: %s", param_value);
  } else if (param_name.compare("owner") == 0) {
    new_obj->set_owner(param_value);
    logger.debug("Setting Owner on Object: %s", param_value);
  } else if (param_name.compare("type") == 0) {
    new_obj->set_type(param_value);
    logger.debug("Setting Type on Object: %s", param_value);
  } else if (param_name.compare("subtype") == 0) {
    new_obj->set_subtype(param_value);
    logger.debug("Setting Subtype on Object: %s", param_value);
  }
}

void JsonFactory::parse_list_string_elements(std::string& param_name, std::string& param_value, DataListInterface *new_list) {
  if (param_name.compare("transaction_id") == 0) {
    new_list->set_transaction_id(param_value);
    logger.debug("Setting Transaction ID on List: %s", param_value);
  } else if (param_name.compare("err_msg") == 0) {
    new_list->set_error_message(param_value);
    logger.debug("Setting Error Message on List: %s", param_value);
  }
}

void JsonFactory::parse_list_int_elements(std::string& param_name, int param_value, DataListInterface *new_list) {
  if (param_name.compare("msg_type") == 0) {
    new_list->set_msg_type(param_value);
    logger.debug("Setting Message Type on List: %d", param_value);
  } else if (param_name.compare("operation") == 0) {
    new_list->set_op_type(param_value);
    logger.debug("Setting Operation Type on List: %d", param_value);
  } else if (param_name.compare("num_records") == 0) {
    new_list->set_num_records(param_value);
    logger.debug("Setting Number of Records on List: %d", param_value);
  }
}

void JsonFactory::property_action_from_iterator(AnimationAction<PropertyFrame> *new_action, const rapidjson::GenericMember<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<> > &itr) {
  std::string anim_attr_name = itr.name.GetString();

  if (itr.value.IsArray()) {
    if (anim_attr_name.compare("keyframes") == 0) {
      for (auto& frame_itr : itr.value.GetArray()) {
        if (frame_itr.IsObject()) {
          rapidjson::Value::ConstMemberIterator frame_elt_itr = frame_itr.FindMember("frame");
          if (frame_elt_itr != frame_itr.MemberEnd()) {
            if (frame_elt_itr->value.IsInt()) {
              int frame_index = frame_elt_itr->value.GetInt();

              // Create a new frame
              PropertyFrame *new_frame = new PropertyFrame();
              new_action->add_keyframe(frame_index, new_frame);
              new_action->get_keyframe(frame_index)->set_frame(frame_index);

              // Iterate over each attribute in the frame
              for (auto& frame_attr_itr : frame_itr.GetObject()) {
                property_frame_from_iterator(new_action->get_keyframe(frame_index), frame_attr_itr);
              }
            } //
          }
        }
      }
    }
  } else if (itr.value.IsString()) {
    std::string param_name = itr.name.GetString();
    std::string param_value = itr.value.GetString();
    if (param_name.compare("name") == 0) {
      new_action->set_name(param_value);
    } else if (param_name.compare("description") == 0) {
      new_action->set_description(param_value);
    } else if (param_name.compare("key") == 0) {
      new_action->set_key(param_value);
    } else if (param_name.compare("scene") == 0) {
      new_action->set_scene(param_value);
    }
  }
}

void JsonFactory::property_frame_from_iterator(PropertyFrame *new_frame, const rapidjson::GenericMember<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<> > &itr) {
  std::string frame_attr_name = itr.name.GetString();
  if (itr.value.IsArray()) {
    // Frame values
    if (frame_attr_name.compare("values") == 0) {
      for (auto& values_itr : itr.value.GetArray()) {

        // Get the actual double value
        for (auto& value_attr_itr : values_itr.GetObject()) {
          if (value_attr_itr.value.IsDouble()) {
            std::string value_attr_name = value_attr_itr.name.GetString();
            if (value_attr_name.compare("value") == 0) {
              double param_value = value_attr_itr.value.GetDouble();
              new_frame->add_value(param_value);
              break;
            }
          }
        }
        // Parse the graph handle
        AnimationGraphHandle *new_handle = new AnimationGraphHandle();
        parse_json_graph_handle(values_itr, new_handle);
        new_frame->add_handle(new_handle);
      }
    }
  } else if (itr.value.IsString()) {
    if (frame_attr_name.compare("scene") == 0) {
     new_frame->set_scene(itr.value.GetString());
    }
  }
}

void JsonFactory::obj_action_from_iterator(AnimationAction<ObjectFrame> *new_action, const rapidjson::GenericMember<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<> > &itr) {
  std::string anim_name = itr.name.GetString();

  // Assign basic String Values to the new action
  if (itr.value.IsString()) {
    std::string param_value = itr.value.GetString();
    if (anim_name.compare("name") == 0) {
      new_action->set_name(param_value);
      logger.debug("Found Action Name %s", param_value);
    } else if (anim_name.compare("description") == 0) {
      new_action->set_description(param_value);
      logger.debug("Found Action Description %s", param_value);
    } else if (anim_name.compare("owner") == 0) {
      new_action->set_owner(param_value);
      logger.debug("Found Action Owner %s", param_value);
    } else if (anim_name.compare("key") == 0) {
      new_action->set_key(param_value);
      logger.debug("Found Action Key %s", param_value);
    } else if (anim_name.compare("scene") == 0) {
      new_action->set_scene(param_value);
    }

  // Assign array values (ie. keyframes) to the new action
  } else if (itr.value.IsArray()) {
    if (anim_name.compare("keyframes") == 0) {
      for (auto& frame_itr : itr.value.GetArray()) {
        if (frame_itr.IsObject()) {
          rapidjson::Value::ConstMemberIterator frame_elt_itr = frame_itr.FindMember("frame");
          if (frame_elt_itr != frame_itr.MemberEnd()) {
            if (frame_elt_itr->value.IsInt()) {
              int frame_index = frame_elt_itr->value.GetInt();
              ObjectFrame *new_frame = new ObjectFrame();
              // Add the keyframe to the action
              new_action->add_keyframe(frame_index, new_frame);
              new_action->get_keyframe(frame_index)->set_frame(frame_index);

              for (auto& frame_attr_itr : frame_itr.GetObject()) {
                obj_frame_from_iterator(new_action->get_keyframe(frame_index), frame_attr_itr);
              }
            }
          }
        }
      }
    }
  }
}

void JsonFactory::obj_frame_from_iterator(ObjectFrame *new_frame, const rapidjson::GenericMember<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<> > &itr) {
  std::string frame_attr_name = itr.name.GetString();

  if (itr.value.IsString()) {
    std::string param_value = itr.value.GetString();
    if (frame_attr_name.compare("owner") == 0) {
      new_frame->set_owner(param_value);
      logger.debug("Found Keyframe Owner %s", param_value);
    } else if (frame_attr_name.compare("scene") == 0) {
     new_frame->set_scene(param_value);
    }
  } else if (itr.value.IsArray()) {
    if (frame_attr_name.compare("transform") == 0) {
      int index = 0;
      for (auto& trans_elt_itr : itr.value.GetArray()) {
        int i = index / 4;
        int j = index % 4;
        if (trans_elt_itr.IsDouble()) {
          double param_value = trans_elt_itr.GetDouble();
          new_frame->get_transform()->set_transform_element(i, j, \
            param_value);
          logger.debug("Found Keyframe Transform Element %f with index %d-%d", param_value, i, j);
        } else if (trans_elt_itr.IsInt()) {
          double param_value = static_cast<double>(trans_elt_itr.GetInt());
          new_frame->get_transform()->set_transform_element(i, j, \
            param_value);
          logger.debug("Found Keyframe Transform Element %f with index %d-%d", param_value, i, j);
        }
        index++;
      }

    // Animation Graph Handles - Translation
    } else if (frame_attr_name.compare("translation_handle") == 0) {
      if (itr.value.IsArray()) {
        int elt_indx = 0;
        for (auto& thandle_elt_itr : itr.value.GetArray()) {
          parse_json_graph_handle(thandle_elt_itr, \
            new_frame->get_translation(elt_indx));
          elt_indx++;
        }
      }

  // Animation Graph Handles - Rotation
  } else if (frame_attr_name.compare("rotation_handle") == 0) {
      if (itr.value.IsArray()) {
        int elt_indx = 0;
        for (auto& rhandle_elt_itr : itr.value.GetArray()) {
          if (rhandle_elt_itr.IsObject()) {
            parse_json_graph_handle(rhandle_elt_itr, \
              new_frame->get_rotation(elt_indx));
            elt_indx++;
          }
        }
      }

    // Animation Graph Handles - Scale
  } else if (frame_attr_name.compare("scale_handle") == 0) {
      if (itr.value.IsArray()) {
        int elt_indx = 0;
        for (auto& shandle_elt_itr : itr.value.GetArray()) {
          parse_json_graph_handle(shandle_elt_itr, \
            new_frame->get_scale(elt_indx));
          elt_indx++;
        }
      }
    }
  }
}

void JsonFactory::obj_from_iterator(JsonObject *new_obj, const rapidjson::GenericMember<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<> > &itr) {
  std::string param_name = itr.name.GetString();

  // Assign basic String Values
  if (itr.value.IsString()) {
    std::string param_value = itr.value.GetString();
    parse_obj_body(param_name, param_value, new_obj);

  // Handle array elements
  } else if (itr.value.IsArray()) {

    // Handle an assets array on the object
    if (param_name.compare("assets") == 0) {
      for (auto& elt_itr : itr.value.GetArray()) {
        if (elt_itr.IsString()) {
          std::string elt_value = elt_itr.GetString();
          new_obj->add_asset(elt_value);
          logger.debug("Found new asset: %s", elt_value);
        }
      }

    // Handle a transform array on the object
    } else if (param_name.compare("transform") == 0) {
      int index = 0;
      for (auto& elt_itr : itr.value.GetArray()) {
        int i = index / 4;
        int j = index % 4;
        if (elt_itr.IsDouble()) {
          double elt_value = elt_itr.GetDouble();
          new_obj->get_transform()->set_transform_element(i, j, elt_value);
          logger.debug("Found Transform Element %f at Index %d-%d", elt_value, i, j);
        } else if (elt_itr.IsInt()) {
          double elt_value = static_cast<double>(elt_itr.GetInt());
          new_obj->get_transform()->set_transform_element(i, j, \
            static_cast<double>(elt_itr.GetInt()));
          logger.debug("Found Transform Element %f at Index %d-%d", elt_value, i, j);
        }
        index++;
      }

    // Handle an Actions array
    } else if (param_name.compare("actions") == 0) {
      for (auto& elt_itr : itr.value.GetArray()) {
        if (elt_itr.IsObject()) {
          logger.debug("Found Action");
          if (elt_itr["name"].IsString()) {

            std::string param_value = elt_itr["name"].GetString();
            logger.debug("Found Action Name %s", param_value);

            // Create a new action
            AnimationAction<ObjectFrame> *new_action = \
              new AnimationAction<ObjectFrame>();

            // Add the animation to the object
            new_obj->add_action(param_value, new_action);
            new_obj->get_action(param_value)->set_name(param_value);

            // Iterate over the elements in the action
            for (auto& anim_itr : elt_itr.GetObject()) {
              obj_action_from_iterator(new_obj->get_action(param_value), anim_itr);
            }
          }
        }
      }
    }
  }
}

void JsonFactory::property_from_iterator(JsonProperty *new_prop, const rapidjson::GenericMember<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<> > &itr) {
  std::string param_name = itr.name.GetString();

  // Assign basic String Values
  if (itr.value.IsString()) {
    std::string param_value = itr.value.GetString();
    parse_property_body(param_name, param_value, new_prop);
  } else if (itr.value.IsArray()) {
    if (param_name.compare("values") == 0) {
      for (auto& values_itr : itr.value.GetArray()) {
        if (values_itr.IsDouble()) {
          double param_value = values_itr.GetDouble();
          logger.debug("Found Value %f", param_value);
          new_prop->add_value(param_value);
        }
      }
    } else if (param_name.compare("actions") == 0) {
      // Iterate over each object in the array
      for (auto& anim_itr : itr.value.GetArray()) {

        if (anim_itr["name"].IsString()) {
          std::string param_value = anim_itr["name"].GetString();
          // Create a new action
          AnimationAction<PropertyFrame> *new_action = \
            new AnimationAction<PropertyFrame>();

          new_prop->add_action(param_value, new_action);

          // For each object in the array, parse all attributes
          for (auto& anim_attr_itr : anim_itr.GetObject()) {
            property_action_from_iterator(new_prop->get_action(param_value), anim_attr_itr);
          }
        }
      }
    }
  }
}

PropertyInterface* JsonFactory::build_property(const rapidjson::Document &d) {
  JsonProperty *new_prop = new JsonProperty();
  if (d.IsObject()) {
    for (auto& doc_itr : d.GetObject()) {
      property_from_iterator(new_prop, doc_itr);
    }
  }
  return new_prop;
}

PropertyListInterface* JsonFactory::build_property_list(const rapidjson::Document& d) {
  JsonPropertyList *new_list = new JsonPropertyList();
  if (d.IsObject()) {
    for (auto& doc_itr : d.GetObject()) {
      std::string param_name = doc_itr.name.GetString();
      if (doc_itr.value.IsInt()) {
        int param_value = doc_itr.value.GetInt();
        parse_list_int_elements(param_name, param_value, new_list);
      } else if (doc_itr.value.IsString()) {
        std::string param_value = doc_itr.value.GetString();
        parse_list_string_elements(param_name, param_value, new_list);
      } else if (doc_itr.value.IsArray()) {
        for (auto& props_itr : doc_itr.value.GetArray()) {
          JsonProperty *new_prop = new JsonProperty();
          for (auto& props_attr_itr : props_itr.GetObject()) {
            property_from_iterator(new_prop, props_attr_itr);
          }
          new_list->add_prop(new_prop);
        }
      }
    }
  }
  return new_list;
}

ObjectInterface* JsonFactory::build_object(const rapidjson::Document& d) {
  JsonObject *new_obj = new JsonObject();
  if (d.IsObject()) {
    for (auto& doc_itr : d.GetObject()) {
      obj_from_iterator(new_obj, doc_itr);
    }
  }
  return new_obj;
}

ObjectListInterface* JsonFactory::build_object_list(const rapidjson::Document& d) {
  JsonObjectList *new_list = new JsonObjectList();
  if (d.IsObject()) {
    for (auto& doc_itr : d.GetObject()) {
      std::string param_name = doc_itr.name.GetString();
      if (doc_itr.value.IsInt()) {
        int param_value = doc_itr.value.GetInt();
        parse_list_int_elements(param_name, param_value, new_list);
      } else if (doc_itr.value.IsString()) {
        std::string param_value = doc_itr.value.GetString();
        parse_list_string_elements(param_name, param_value, new_list);
      } else if (doc_itr.value.IsArray()) {
        for (auto& obj_itr : doc_itr.value.GetArray()) {
          JsonObject *new_obj = new JsonObject();
          for (auto& obj_attr_itr : obj_itr.GetObject()) {
            obj_from_iterator(new_obj, obj_attr_itr);
          }
          new_list->add_object(new_obj);
        }
      }
    }
  }
  return new_list;
}

// Create a Property Action from a parsed Rapidjson Document
AnimationAction<PropertyFrame>* JsonFactory::build_property_action(const rapidjson::Document& d) {
  // Create a new action
  AnimationAction<PropertyFrame> *new_action = \
    new AnimationAction<PropertyFrame>();
  if (d.IsObject()) {
    auto doc_object = d.GetObject();
    // For each object in the array, parse all attributes
    for (auto& anim_attr_itr : doc_object) {
      property_action_from_iterator(new_action, anim_attr_itr);
    }
  }
  return new_action;
}

// Create an Object Action from a parsed Rapidjson Document
AnimationAction<ObjectFrame>* JsonFactory::build_object_action(const rapidjson::Document& d) {
  // Create a new action
  AnimationAction<ObjectFrame> *new_action = \
    new AnimationAction<ObjectFrame>();
  if (d.IsObject()) {
    auto doc_object = d.GetObject();
    // For each object in the array, parse all attributes
    for (auto& anim_attr_itr : doc_object) {
      obj_action_from_iterator(new_action, anim_attr_itr);
    }
  }
  return new_action;
}

// Create a Property Keyframe from a parsed Rapidjson Document
PropertyFrame* JsonFactory::build_property_frame(const rapidjson::Document& d) {
  PropertyFrame *new_frame = new PropertyFrame();
  if (d.IsObject()) {
    auto doc_object = d.GetObject();
    rapidjson::Value::ConstMemberIterator frame_elt_itr = doc_object.FindMember("frame");
    if (frame_elt_itr != doc_object.MemberEnd()) {
      if (frame_elt_itr->value.IsInt()) {
        int frame_index = frame_elt_itr->value.GetInt();
        new_frame->set_frame(frame_index);

        for (auto& frame_attr_itr : doc_object) {
          property_frame_from_iterator(new_frame, frame_attr_itr);
        }
      }
    }
  }
  return new_frame;
}

// Create an Object Keyframe from a parsed Rapidjson Document
ObjectFrame* JsonFactory::build_object_frame(const rapidjson::Document& d) {
  ObjectFrame *new_frame = new ObjectFrame();
  if (d.IsObject()) {
    auto doc_object = d.GetObject();
    rapidjson::Value::ConstMemberIterator frame_elt_itr = doc_object.FindMember("frame");
    if (frame_elt_itr != doc_object.MemberEnd()) {
      if (frame_elt_itr->value.IsInt()) {
        int frame_index = frame_elt_itr->value.GetInt();
        new_frame->set_frame(frame_index);

        for (auto& frame_attr_itr : doc_object) {
          obj_frame_from_iterator(new_frame, frame_attr_itr);
        }
      }
    }
  }
  return new_frame;
}
