#include "globals.h"
#include "lyman_log.h"

#include <exception>

#include "aossl/factory/uuid_interface.h"

inline std::string generate_uuid()
{
  std::string id_str
  try {
    id_str = ua->generate();
  }
  catch (UuidSecurityException& ue) {
    //Continue and print the security error
    uuid_logging->error("UUID Security Exception");
    uuid_logging->error(ue.what());
  }
  catch (std::exception& e) {
    throw e;
  }
  return id_str;
}
