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
