#include "lyman_log.h"

LoggingCategoryInterface *config_logging = NULL;
LoggingCategoryInterface *obj_logging = NULL;
LoggingCategoryInterface *doc_logging = NULL;
LoggingCategoryInterface *callback_logging = NULL;
LoggingCategoryInterface *main_logging = NULL;

void start_logging_submodules()
{
  if (!config_logging) {config_logging = logging->get_category("configuration");}
  if (!obj_logging) {obj_logging = logging->get_category("obj3");}
  if (!doc_logging) {doc_logging = logging->get_category("document");}
  if (!callback_logging) {callback_logging = logging->get_category("callbacks");}
  if (!main_logging) {main_logging = logging->get_category("main");}
}

void shutdown_logging_submodules()
{
  if (!config_logging)
  {
    logging->debug("Configuration Logging Module delete called without initialized object");
  }
  else
  {
    delete config_logging;
  }
  if (!obj_logging)
  {
    logging->debug("Object3 Logging Module delete called without initialized object");
  }
  else
  {
    delete obj_logging;
  }
  if (!doc_logging)
  {
    logging->debug("Document Manager Logging Module delete called without initialized object");
  }
  else
  {
    delete doc_logging;
  }
  if (!callback_logging)
  {
    logging->debug("Couchbase Callback Logging Module delete called without initialized object");
  }
  else
  {
    delete callback_logging;
  }
  if (!main_logging)
  {
    logging->debug("Main Logging Module delete called without initialized object");
  }
  else
  {
    delete main_logging;
  }
  shutdown_framework_logging();
}
