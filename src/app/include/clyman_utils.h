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

#include <strings.h>
#include <string.h>
#include <stdint.h>
#include <sstream>
#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#include "Poco/Net/HTTPServerRequest.h"

#include "model/include/animation_graph_handle.h"

#ifndef SRC_APP_INCLUDE_IVAN_UTILS_H_
#define SRC_APP_INCLUDE_IVAN_UTILS_H_

// Universal Event Types
const int OBJ_CRT = 0;
const int OBJ_UPD = 1;
const int OBJ_GET = 2;
const int OBJ_DEL = 3;
const int OBJ_QUERY = 4;
const int OBJ_LOCK = 5;
const int OBJ_UNLOCK = 6;
const int OBJ_OVERWRITE = 7;
const int PROP_CRT = 8;
const int PROP_UPD = 9;
const int PROP_GET = 10;
const int PROP_DEL = 11;
const int PROP_QUERY = 12;
const int ASSET_ADD = 13;
const int ASSET_DEL = 14;
const int KILL = 999;
const int PING = 555;

// Error Response Codes
const int NO_ERROR = 100;
const int ERROR = 101;
const int NOT_FOUND = 102;
const int TRANSLATION_ERROR = 110;
const int PROCESSING_ERROR = 120;
const int BAD_MSG_TYPE_ERROR = 121;
const int INSUFF_DATA_ERROR = 122;
const int LOCK_EXISTS_ERROR = 123;

// Operation Types
const int APPEND = 10;
const int REMOVE = 11;

// Query Types
const int CREATE_QUERY_TYPE = 0;
const int GET_QUERY_TYPE = 1;
const int UPDATE_QUERY_TYPE = 2;
const int DELETE_QUERY_TYPE = 3;

// Convert Request Contents into a Rapidjson Document
// Returns a char* which must be deleted only after using the Rapidjson Doc
static inline char* clyman_request_body_to_json_document(Poco::Net::HTTPServerRequest& request, \
    rapidjson::Document& doc) {
  // Pull the request body out of a stream and into a character buffer
  int length = request.getContentLength();
  std::istream &request_stream = request.stream();
  char *buffer = new char[length];
  request_stream.read(buffer, length);

  // Parse the buffer containing the request body into the rapidjson document
  doc.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer);

  // Return the buffer, as it needs to be valid while we're using the document
  return buffer;
};

// JSON Writing
inline void write_json_string_elt(rapidjson::Writer<rapidjson::StringBuffer>& writer, \
    const char* key, std::string value) {
  if (!(value.empty())) {
    writer.Key(key);
    writer.String(value.c_str(), (rapidjson::SizeType)value.length());
  }
}
inline void write_json_int_elt(rapidjson::Writer<rapidjson::StringBuffer>& writer, \
    const char* key, int value) {
  if (value > -999) {
    writer.Key(key);
    writer.Uint(value);
  }
}
inline void write_json_graph_handle(rapidjson::Writer<rapidjson::StringBuffer>& writer, \
    AnimationGraphHandle *handle) {
  writer.Key("left_type");
  writer.String(handle->get_lh_type().c_str(), \
      (rapidjson::SizeType)(handle->get_lh_type().length()));
  writer.Key("left_x");
  writer.Double(handle->get_lh_x());
  writer.Key("left_y");
  writer.Double(handle->get_lh_y());
  writer.Key("right_type");
  writer.String(handle->get_rh_type().c_str(), \
      (rapidjson::SizeType)(handle->get_rh_type().length()));
  writer.Key("right_x");
  writer.Double(handle->get_rh_x());
  writer.Key("right_y");
  writer.Double(handle->get_rh_y());
}

// JSON Parsing
inline void find_json_string_elt_in_array(rapidjson::GenericArray<true, rapidjson::GenericValue<rapidjson::UTF8<> > >::ValueType& itr, \
    const char* key, std::string& out) {
  auto key_itr = itr.FindMember(key);
  if (key_itr != itr.MemberEnd()) {
    if (key_itr->value.IsString()) {
      out.assign(key_itr->value.GetString());
    }
  }
}
inline int find_json_int_elt_in_array(rapidjson::GenericArray<true, rapidjson::GenericValue<rapidjson::UTF8<> > >::ValueType& itr, \
    const char* key) {
  auto key_itr = itr.FindMember(key);
  if (key_itr != itr.MemberEnd()) {
    if (key_itr->value.IsNumber()) {
      return key_itr->value.GetInt();
    }
  }
  return -9999;
}
inline void find_json_string_elt_in_val(rapidjson::Value::ConstValueIterator &itr, \
    const char* key, std::string& out) {
  auto key_itr = itr->FindMember(key);
  if (key_itr != itr->MemberEnd()) {
    if (key_itr->value.IsString()) {
      out.assign(key_itr->value.GetString());
    }
  }
}
inline int find_json_int_elt_in_val(rapidjson::Value::ConstValueIterator &itr, \
    const char* key) {
  auto key_itr = itr->FindMember(key);
  if (key_itr != itr->MemberEnd()) {
    if (key_itr->value.IsNumber()) {
      return key_itr->value.GetInt();
    }
  }
  return -9999;
}
inline void find_json_string_elt_in_doc(const rapidjson::Document &d, \
    const char* key, std::string& out) {
  auto key_itr = d.FindMember(key);
  if (key_itr != d.MemberEnd()) {
    if (key_itr->value.IsString()) {
      out.assign(key_itr->value.GetString());
    }
  }
}
inline int find_json_int_elt_in_doc(const rapidjson::Document &d, \
    const char* key) {
  auto elt_itr = d.FindMember(key);
  if (elt_itr != d.MemberEnd()) {
    if (elt_itr->value.IsNumber()) {
      return elt_itr->value.GetInt();
    }
  }
  return -9999;
}
inline void parse_json_graph_handle(rapidjson::GenericArray<true, rapidjson::GenericValue<rapidjson::UTF8<> > >::ValueType& itr, \
    AnimationGraphHandle *handle) {
  // Left Handle

  // Get the type of handle
  auto lt_iter = itr.FindMember("left_type");
  if (lt_iter != itr.MemberEnd()) {
    if (lt_iter->value.IsString()) {
      handle->set_lh_type(lt_iter->value.GetString());
    }
  }
  auto lx_itr = itr.FindMember("left_x");
  if (lx_itr != itr.MemberEnd()) {
    if (lx_itr->value.IsNumber()) {
      handle->set_lh_x(lx_itr->value.GetDouble());
    }
  }
  auto ly_itr = itr.FindMember("left_y");
  if (ly_itr != itr.MemberEnd()) {
    if (ly_itr->value.IsNumber()) {
      handle->set_lh_y(ly_itr->value.GetDouble());
    }
  }

  // Right Handle

  // Get the type of handle
  auto rt_itr = itr.FindMember("right_type");
  if (rt_itr != itr.MemberEnd()) {
    if (rt_itr->value.IsString()) {
      handle->set_rh_type(rt_itr->value.GetString());
    }
  }
  auto rx_itr = itr.FindMember("right_x");
  if (rx_itr != itr.MemberEnd()) {
    if (rx_itr->value.IsNumber()) {
      handle->set_rh_x(rx_itr->value.GetDouble());
    }
  }
  auto ry_itr = itr.FindMember("right_y");
  if (ry_itr != itr.MemberEnd()) {
    if (ry_itr->value.IsNumber()) {
      handle->set_rh_y(ry_itr->value.GetDouble());
    }
  }
}

// String Manipulation
inline void split_from_index(const std::string& input, std::vector<std::string>& output, char delim, int start) {
  std::stringstream ss(input.substr(start, input.size()-1));
  std::string item;
  while (std::getline(ss, item, delim)) {
    output.push_back(item);
  }
}
inline void split(const std::string& input, std::vector<std::string>& output, char delim) {
  split_from_index(input, output, delim, 1);
}

#endif  // SRC_APP_INCLUDE_IVAN_UTILS_H_
