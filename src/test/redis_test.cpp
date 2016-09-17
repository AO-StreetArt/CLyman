//Tests for Redis Admin

#include <string>
#include <iostream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "aossl/factory/logging_interface.h"
#include "aossl/factory.h"

#include "aossl/factory/redis_interface.h"

#include "../lyman_log.h"

#include <assert.h>

//Main Method

std::vector<RedisConnChain> RedisConnectionList;

RedisInterface *xRedis;

int main()
{

//Read the Redis Configuration File
//Open the file
std::string line;
std::ifstream file ("src/test/redis.properties");

if (file.is_open()) {
  while (getline (file, line) ) {
    //Read a line from the property file

    //Figure out if we have a blank or comment line
    bool keep_going = true;
    if (line.length() > 0) {
      if (line[0] == '/' && line[1] == '/') {
        keep_going=false;
      }
    }
    else {
      keep_going=false;
    }

    if (keep_going==true) {
      int eq_pos = line.find("=", 0);
      std::string var_name = line.substr(0, eq_pos);
      std::string var_value = line.substr(eq_pos+1, line.length() - eq_pos);
      if (var_name=="RedisConnectionString") {
        //Read a string in the format 127.0.0.1--7000----2--5--0
        RedisConnChain chain;

        //Retrieve the first value
        int spacer_position = var_value.find("--", 0);
        std::string str1 = var_value.substr(0, spacer_position);
        chain.ip = str1;

        //Retrieve the second value
        std::string new_value = var_value.substr(spacer_position+2, var_value.length() - 1);
        spacer_position = new_value.find("--", 0);
        str1 = new_value.substr(0, spacer_position);
        chain.port = std::stoi(str1);

        //Retrieve the third value
        new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
        spacer_position = new_value.find("--", 0);
        str1 = new_value.substr(0, spacer_position);
        chain.password = str1;

        //Retrieve the fourth value
        new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
        spacer_position = new_value.find("--", 0);
        str1 = new_value.substr(0, spacer_position);
        chain.pool_size = std::stoi(str1);

        //Retrieve the fifth value
        new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
          spacer_position = new_value.find("--", 0);
          str1 = new_value.substr(0, spacer_position);
        chain.timeout = std::stoi(str1);

        //Retrieve the final value
        new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
        spacer_position = new_value.find("--", 0);
        str1 = new_value.substr(0, spacer_position);
        chain.role = std::stoi(str1);

        RedisConnectionList.push_back(chain);
      }
    }
  }
  file.close();
}


ServiceComponentFactory *factory = new ServiceComponentFactory;

//-------------------------------Logging--------------------------------------//
//----------------------------------------------------------------------------//

std::string initFileName = "src/test/log4cpp_test.properties";
logging = factory->get_logging_interface(initFileName);

//Set up the logging submodules for each category
start_logging_submodules();

//Set up internal variables
logging->info("Internal Variables Intialized");

//Set up the Redis Admin
//Set up our Redis Connection List
xRedis = factory->get_redis_cluster_interface(RedisConnectionList);
logging->info("Redis Connection List Built");

//save
bool bRet = xRedis->save("Test", "123");
if (!bRet) {
logging->error("Error putting object to Redis Smart Update Buffer");
assert(bRet);
}

//exists
bool eRet = xRedis->exists("Test");
if (!eRet) {
logging->error("Test not found in buffer");
assert(eRet);
}

//load
std::string strValue = xRedis->load("Test");
assert(strValue == "123");
logging->debug(strValue);

//Delete
xRedis->del("Test");

delete xRedis;
shutdown_logging_submodules();
delete logging;
delete factory;

return 0;
}
