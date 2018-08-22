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

#include "aossl/profile/include/tiered_app_profile.h"
#include "aossl/consul/include/consul_interface.h"
#include "aossl/consul/include/factory_consul.h"

#include <atomic>
#include <string>

#ifndef SRC_APP_INCLUDE_CLUSTER_MANAGER_H_
#define SRC_APP_INCLUDE_CLUSTER_MANAGER_H_

class ClusterManager {
  std::vector<AOSSL::ServiceInterface*> cluster_members1;
  std::vector<AOSSL::ServiceInterface*> cluster_members2;
  AOSSL::TieredApplicationProfile *profile = nullptr;
  std::atomic<int> current_vector{1};
  std::atomic<int> current_return_index{0};
  std::string service_registration_name = "CrazyIvan_Udp";
  AOSSL::ConsulComponentFactory consul_factory;
public:
  ClusterManager(AOSSL::TieredApplicationProfile *prof) {profile=prof;}
  ~ClusterManager() {for (auto service : cluster_members1) {delete service;}\
      for (auto service : cluster_members2) {delete service;}}
  inline AOSSL::ServiceInterface* get_ivan() {
    // Return a Service from the in-memory cluster information
    if (current_vector == 1) {
      if (cluster_members1.size() == 0) return nullptr;
      if (current_return_index > cluster_members1.size()) {
        current_return_index = 0;
      }
      return cluster_members1[current_return_index];
    } else {
      if (cluster_members2.size() == 0) return nullptr;
      if (current_return_index > cluster_members2.size()) {
        current_return_index = 0;
      }
      return cluster_members2[current_return_index];
    }
    current_return_index++;
  }
  // Should only be called by one thread at a time,
  // with atleast a few seconds in between
  inline void update_cluster_info() {
    // Update the cluster information from Consul
    // Delete then update the vector not currently in use
    // with information from Consul, then update the
    // current_vector variable
    AOSSL::StringBuffer *services_buf = \
        profile->get_consul()->services();

    // Parse the Services List that we get back from the agent
    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseStopWhenDoneFlag>(services_buf->val.c_str());
    if (doc.HasParseError()) {
      if (services_buf) delete services_buf;
      throw std::invalid_argument(GetParseError_En(doc.GetParseError()));
    } else if (doc.IsObject()) {
      // Clear out the previous cluster members in memory
      if (current_vector == 1) {
        for (auto member : cluster_members2) {
          delete member;
        }
        cluster_members2.clear();
      } else {
        for (auto member : cluster_members1) {
          delete member;
        }
        cluster_members1.clear();
      }
      // We now have a parsed JSON Object which contains
      // a list of known services to our local Consul Agent
      for (auto& itr : doc.GetObject()) {
        std::vector<std::string> current_obj_tags;
        rapidjson::Value::ConstMemberIterator service_itr = \
            itr.value.FindMember("Service");
        if (service_itr != itr.value.MemberEnd()) {
          if (!(service_itr->value.IsNull())) {
            std::string service_name(service_itr->value.GetString());
            if ((service_name == service_registration_name)) {
              AOSSL::ServiceInterface *return_service = consul_factory.get_service_interface();
              bool cluster_tag_found = false;
              for (auto& tag : current_obj_tags) {
                return_service->add_tag(tag);
                if (tag == profile->get_cluster_name()) cluster_tag_found = true;
              }
              rapidjson::Value::ConstMemberIterator address_itr = \
                  itr.value.FindMember("Address");
              rapidjson::Value::ConstMemberIterator port_itr = \
                  itr.value.FindMember("Port");
              rapidjson::Value::ConstMemberIterator id_itr = \
                  itr.value.FindMember("ID");
              return_service->set_address(address_itr->value.GetString());
              return_service->set_port(std::to_string(port_itr->value.GetInt()));
              return_service->set_id(id_itr->value.GetString());
              return_service->set_name(service_registration_name);
              if (cluster_tag_found) {
                if (current_vector == 1) {
                  cluster_members2.push_back(return_service);
                } else {
                  cluster_members1.push_back(return_service);
                }
              }
            }
          }
        }
      }
      if (current_vector == 1) {
        current_vector = 2;
      } else {
        current_vector = 1;
      }
    }
    if (services_buf) delete services_buf;
  }
};

#endif  // SRC_APP_INCLUDE_CLUSTER_MANAGER_H_
