#ifndef LYMAN_LOGGING
#define LYMAN_LOGGING

#include "aossl/factory/logging_interface.h"

extern LoggingCategoryInterface *uuid_logging;
extern LoggingCategoryInterface *config_logging;
extern LoggingCategoryInterface *obj_logging;
extern LoggingCategoryInterface *doc_logging;
extern LoggingCategoryInterface *callback_logging;
extern LoggingCategoryInterface *main_logging;

void start_logging_submodules();
void shutdown_logging_submodules();

#endif
