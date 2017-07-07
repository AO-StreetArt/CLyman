#include "include/app_log.h"

LoggingCategoryInterface *config_logging = NULL;
LoggingCategoryInterface *redis_logging = NULL;
LoggingCategoryInterface *main_logging = NULL;
LoggingCategoryInterface *uuid_logging = NULL;
LoggingCategoryInterface *obj_logging = NULL;

void start_logging_submodules()
{
  if (!config_logging) {config_logging = logging->get_category("configuration");}
  if (!redis_logging) {redis_logging = logging->get_category("redis");}
  if (!main_logging) {main_logging = logging->get_category("main");}
  if (!uuid_logging) {uuid_logging = logging->get_category("uuid");}
  if (!obj_logging) {obj_logging = logging->get_category("obj");}
}

void shutdown_logging_submodules()
{
  if (config_logging) delete config_logging;
  if (redis_logging) delete redis_logging;
  if (main_logging) delete main_logging;
  if (uuid_logging) delete uuid_logging;
  if (obj_logging) delete obj_logging;
}
