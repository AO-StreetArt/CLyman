#include "logging.h"

int main ()
{
init_log();

logging->debug("Testing Testing");
logging->info("123");
logging->error("Here we go!");

return 0;
}
