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
  } else if (param_name.compare("name") == 0) {
    new_prop->set_name(param_value);
  } else if (param_name.compare("parent") == 0) {
    new_prop->set_parent(param_value);
  } else if (param_name.compare("scene") == 0) {
    new_prop->set_scene(param_value);
  } else if (param_name.compare("asset_sub_id") == 0) {
    new_prop->set_asset_sub_id(param_value);
  }
}

void JsonFactory::parse_obj_body(std::string& param_name, std::string& param_value, JsonObject *new_obj) {
  if (param_name.compare("key") == 0) {
    new_obj->set_key(param_value);
  } else if (param_name.compare("name") == 0) {
    new_obj->set_name(param_value);
  } else if (param_name.compare("parent") == 0) {
    new_obj->set_parent(param_value);
  } else if (param_name.compare("scene") == 0) {
    new_obj->set_scene(param_value);
  } else if (param_name.compare("asset_sub_id") == 0) {
    new_obj->set_asset_sub_id(param_value);
  } else if (param_name.compare("owner") == 0) {
    new_obj->set_owner(param_value);
  } else if (param_name.compare("type") == 0) {
    new_obj->set_type(param_value);
  } else if (param_name.compare("subtype") == 0) {
    new_obj->set_subtype(param_value);
  }
}

void JsonFactory::parse_list_string_elements(std::string& param_name, std::string& param_value, DataListInterface *new_list) {
  if (param_name.compare("transaction_id") == 0) {
    new_list->set_transaction_id(param_value);
  } else if (param_name.compare("err_msg") == 0) {
    new_list->set_error_message(param_value);
  }
}

void JsonFactory::parse_list_int_elements(std::string& param_name, int param_value, DataListInterface *new_list) {
  if (param_name.compare("msg_type") == 0) {
    new_list->set_msg_type(param_value);
  } else if (param_name.compare("operation") == 0) {
    new_list->set_op_type(param_value);
  } else if (param_name.compare("num_records") == 0) {
    new_list->set_num_records(param_value);
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
          new_obj->add_asset(elt_itr.GetString());
        }
      }

    // Handle a transform array on the object
    } else if (param_name.compare("transform") == 0) {
      int index = 0;
      for (auto& elt_itr : itr.value.GetArray()) {
        int i = index / 4;
        int j = index % 4;
        if (elt_itr.IsDouble()) {
          new_obj->get_transform()->set_transform_element(i, j, \
            elt_itr.GetDouble());
        } else if (elt_itr.IsInt()) {
          new_obj->get_transform()->set_transform_element(i, j, \
            static_cast<double>(elt_itr.GetInt()));
        }
        index++;
      }

    // Handle an Animations array
    } else if (param_name.compare("animations") == 0) {
      for (auto& elt_itr : itr.value.GetArray()) {
        if (elt_itr.IsObject()) {

          // Create a new action
          AnimationAction<ObjectFrame> *new_action = \
            new AnimationAction<ObjectFrame>();

          // Iterate over the elements in the action
          for (auto& anim_itr : elt_itr.GetObject()) {
            std::string anim_name = anim_itr.name.GetString();

            // Assign basic String Values to the new action
            if (anim_itr.value.IsString()) {
              std::string param_value = anim_itr.value.GetString();
              if (anim_name.compare("name") == 0) {
                new_action->set_name(param_value);
              } else if (anim_name.compare("description") == 0) {
                new_action->set_description(param_value);
              } else if (anim_name.compare("owner") == 0) {
                new_action->set_owner(param_value);
              }

            // Assign array values (ie. keyframes) to the new action
          } else if (anim_itr.value.IsArray()) {
              if (anim_name.compare("keyframes") == 0) {
                for (auto& frame_itr : anim_itr.value.GetArray()) {
                  std::string frame_attr_name = anim_itr.name.GetString();
                  ObjectFrame *new_frame = new ObjectFrame();
                  for (auto& frame_attr_itr : frame_itr.GetObject()) {

                    // Handle integer arguments
                    if (frame_attr_itr.value.IsInt()) {
                      int param_value = frame_attr_itr.value.GetInt();
                      if (frame_attr_name.compare("frame") == 0) {
                        new_frame->set_frame(param_value);
                      }
                    } else if (frame_attr_itr.value.IsString()) {
                      std::string param_value = frame_attr_itr.value.GetString();
                      if (frame_attr_name.compare("owner") == 0) {
                        new_frame->set_owner(param_value);
                      }
                    } else if (frame_attr_itr.value.IsArray()) {
                      if (frame_attr_name.compare("transform") == 0) {
                        int index = 0;
                        for (auto& trans_elt_itr : frame_attr_itr.value.GetArray()) {
                          int i = index / 4;
                          int j = index % 4;
                          if (trans_elt_itr.IsDouble()) {
                            new_frame->get_transform()->set_transform_element(i, j, \
                              trans_elt_itr.GetDouble());
                          } else if (trans_elt_itr.IsInt()) {
                            new_frame->get_transform()->set_transform_element(i, j, \
                              static_cast<double>(trans_elt_itr.GetInt()));
                          }
                          index++;
                        }

                      // Animation Graph Handles - Translation
                      } else if (frame_attr_name.compare("translation_handle") == 0) {
                        if (frame_attr_itr.value.IsArray()) {
                          int elt_indx = 0;
                          for (auto& thandle_elt_itr : frame_attr_itr.value.GetArray()) {
                            parse_json_graph_handle(thandle_elt_itr, \
                              new_frame->get_translation(elt_indx));
                            elt_indx++;
                          }
                        }

                    // Animation Graph Handles - Rotation
                    } else if (frame_attr_name.compare("rotation_handle") == 0) {
                        if (frame_attr_itr.value.IsArray()) {
                          int elt_indx = 0;
                          for (auto& rhandle_elt_itr : frame_attr_itr.value.GetArray()) {
                            parse_json_graph_handle(rhandle_elt_itr, \
                              new_frame->get_rotation(elt_indx));
                            elt_indx++;
                          }
                        }

                      // Animation Graph Handles - Scale
                    } else if (frame_attr_name.compare("scale_handle") == 0) {
                        if (frame_attr_itr.value.IsArray()) {
                          int elt_indx = 0;
                          for (auto& shandle_elt_itr : frame_attr_itr.value.GetArray()) {
                            parse_json_graph_handle(shandle_elt_itr, \
                              new_frame->get_scale(elt_indx));
                            elt_indx++;
                          }
                        }
                      }
                    }
                  }

                  // Add the keyframe to the action
                  new_action->add_keyframe(new_frame->get_frame(), new_frame);
                }
              }
            }

            // Add the animation to the object
            new_obj->add_action(new_action->get_name(), new_action);
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
          new_prop->add_value(param_value);
        }
      }
    } else if (param_name.compare("actions") == 0) {
      // Iterate over each object in the array
      for (auto& anim_itr : itr.value.GetArray()) {
        // Create a new action
        AnimationAction<PropertyFrame> *new_action = \
          new AnimationAction<PropertyFrame>();

        // For each object in the array, parse all attributes
        for (auto& anim_attr_itr : anim_itr.GetObject()) {
          std::string anim_attr_name = anim_attr_itr.name.GetString();

          if (anim_attr_itr.value.IsArray()) {
            if (anim_attr_name.compare("keyframes") == 0) {
              for (auto& frame_itr : anim_attr_itr.value.GetArray()) {
                // Create a new frame
                PropertyFrame *new_frame = new PropertyFrame();

                // Iterate over each attribute in the frame
                for (auto& frame_attr_itr : frame_itr.GetObject()) {
                  std::string frame_attr_name = frame_attr_itr.name.GetString();
                  if (frame_attr_name.compare("frame") == 0) {
                    // Handle integer arguments
                    if (frame_attr_itr.value.IsInt()) {
                      int param_value = frame_attr_itr.value.GetInt();
                      new_frame->set_frame(param_value);
                    }
                  } else if (frame_attr_itr.value.IsArray()) {
                    // Frame values
                    if (frame_attr_name.compare("values") == 0) {
                      for (auto& values_itr : frame_attr_itr.value.GetArray()) {

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
                  }
                }
                new_action->add_keyframe(new_frame->get_frame(), new_frame);
              }
            }
          } else if (anim_attr_itr.value.IsString()) {
            std::string param_value = anim_attr_itr.value.GetString();
            if (anim_attr_name.compare("name") == 0) {
              new_action->set_name(param_value);
            } else if (anim_attr_name.compare("description") == 0) {
              new_action->set_description(param_value);
            }
          }
        }
        new_prop->add_action(new_action->get_name(), new_action);
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
