#include "app_log.h"

//Use a set of Obj3s to query the DB and return a list of results
Obj3List* batch_query(Obj3List *inp_list, MongoInterface *m) {
  //Determine # of results to return based on input
  int max_results = inp_list->get_num_records();
  int num_results = 0;

  //Create the new Obj3List to return
  Obj3List *return_list = new Obj3List;

  //Iterate over the input list
  for (int i = 0; i < inp_list->num_objects(); i++) {

    //Generate a Query from each object in the list
    std::string query_string = inp_list->get_object(i)->to_json();

    //Execute the Query with Mongo
    MongoIteratorInterface *iter = m->query(query_string);

    //Add the results to the return list until we reach our limit or find no more
    if (iter) {
      MongoResponseInterface *resp = iter->next();
      while (resp && num_results < max_results) {
        rapidjson::Document resp_doc;
        resp_doc.Parse(resp->get_value().c_str());
        Obj3 *resp_obj = new Obj3(resp_doc);
        return_list->add_object(resp_obj);
        num_results++;
        delete resp;
        resp = iter->next();
      }

    }

  }

  return return_list;
}
