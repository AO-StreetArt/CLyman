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

// Functions to assist with running queries against Mongo

#include <string>
#include "app_log.h"
#include "object_list_interface.h"
#include "object_list_factory.h"

#ifndef SRC_APP_INCLUDE_QUERY_HELPER_H_
#define SRC_APP_INCLUDE_QUERY_HELPER_H_

// Use a set of Obj3s to query the DB and return a list of results
ObjectListInterface* batch_query(ObjectListInterface *inp_list, \
  ObjectListInterface *out_list, MongoInterface *m) {
  // Determine # of results to return based on input
  int max_results = inp_list->get_num_records();
  int num_results = 0;
  ObjectFactory objfactory;

  // Iterate over the input list
  for (int i = 0; i < inp_list->num_objects(); i++) {
    if (num_results > max_results) {
      main_logging->debug("Max Query Results Reached");
      break;
    }

    // Generate a Query from each object in the list
    std::string query_string = inp_list->get_object(i)->to_json();
    main_logging->debug("Mongo Query:");
    main_logging->debug(query_string);

    // Execute the Query with Mongo
    MongoIteratorInterface *iter = m->query(query_string);

    // Add the results to the return list until we reach our limit or find none
    if (iter) {
      MongoResponseInterface *resp = iter->next();
      while (resp) {
        if (num_results > max_results) {
          main_logging->debug("Max Query Results Reached");
          break;
        }
        if (!(resp->get_err_msg().empty())) {
          main_logging->error("Error detected in cursor:");
          main_logging->error(resp->get_err_msg());
          break;
        }
        main_logging->debug("Adding response document to return list");
        rapidjson::Document resp_doc;
        resp_doc.Parse(resp->get_value().c_str());
        main_logging->debug(resp->get_value().c_str());
        ObjectInterface *resp_obj = objfactory.build_object();
        out_list->add_object(resp_obj);
        num_results++;
        delete resp;
        resp = iter->next();
      }
      delete iter;
    } else {
      main_logging->debug("Mongo Query returned no results");
    }
  }
  return out_list;
}

#endif  // SRC_APP_INCLUDE_QUERY_HELPER_H_
