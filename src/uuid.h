#include "globals.h"
#include "lyman_log.h"

#include <exception>

#include "aossl/factory/uuid_interface.h"

inline std::string generate_uuid()
{
  UuidContainer id_container;
  id_container = ua->generate();
  if (!id_container.err.empty()) {
    uuid_logging->error(id_container.err);
  }
  return id_container.id;
}
