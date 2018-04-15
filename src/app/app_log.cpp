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

#include "include/app_log.h"

LoggingCategoryInterface *config_logging = NULL;
LoggingCategoryInterface *main_logging = NULL;
LoggingCategoryInterface *uuid_logging = NULL;
LoggingCategoryInterface *obj_logging = NULL;

void start_logging_submodules() {
  if (!config_logging) config_logging = logging->get_category("configuration");
  if (!main_logging) main_logging = logging->get_category("main");
  if (!uuid_logging) uuid_logging = logging->get_category("uuid");
  if (!obj_logging) obj_logging = logging->get_category("obj");
}

void shutdown_logging_submodules() {
  if (config_logging) delete config_logging;
  if (main_logging) delete main_logging;
  if (uuid_logging) delete uuid_logging;
  if (obj_logging) delete obj_logging;
}
