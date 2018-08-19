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

#include "Poco/ErrorHandler.h"
#include "Poco/Exception.h"
#include "Poco/Logger.h"

#ifndef SRC_APP_INCLUDE_THREAD_ERROR_HANDLER_H_
#define SRC_APP_INCLUDE_THREAD_ERROR_HANDLER_H_

class ClymanErrorHandler: public Poco::ErrorHandler {
public:
  void exception(const Poco::Exception& exc) {
    Poco::Logger::get("Event").error(exc.displayText());
  }
  void exception(const std::exception& exc) {
    Poco::Logger::get("Event").error(exc.what());
  }
  void exception() {
    Poco::Logger::get("Event").error("Unknown exception in background thread");
  }
};

#endif  // SRC_APP_INCLUDE_THREAD_ERROR_HANDLER_H_
