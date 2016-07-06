#include <hayai/hayai.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <uuid/uuid.h>

#include "logging.h"

#include "xredis_admin.h"

xRedisAdmin *xRedis;
std::vector<std::string> uuid_list;
int savecounter = 0;
int getcounter = 0;
int delcounter = 0;
int existcounter = 0;

//----------------------------------------------------------------------------//
//------------------------------Benchmarks------------------------------------//
//----------------------------------------------------------------------------//

BENCHMARK(Redis, Save, 10, 100)
{

uuid_str = uuid_list[savecounter];

//save
bool bRet = xRedis->save(uuid_str, "123");
if (!bRet) {
logging->error("Error putting object to Redis Smart Update Buffer");
}

savecounter=savecounter+1;

}

BENCHMARK(Redis, ExistsTrue, 10, 100)
{

uuid_str = uuid_list[existcounter];

//exists
bool eRet = xRedis->exists(uuid_str);

existcounter=existcounter+1;

}

BENCHMARK(Redis, ExistsFalse, 10, 100)
{

uuid_str = "TEST";

//exists
bool eRet = xRedis->exists(uuid_str);

}

BENCHMARK(Redis, Load, 10, 100)
{

uuid_str = uuid_list[getcounter];

//load
std::string strValue = xRedis->load(uuid_str);

getcounter=getcounter+1;

}

BENCHMARK(Redis, Delete, 10, 100)
{

uuid_str = uuid_list[delcounter];

//Delete
xRedis->del(uuid_str);

delcounter=delcounter+1;

}

//----------------------------------------------------------------------------//
//------------------------------Main Method-----------------------------------//
//----------------------------------------------------------------------------//

struct RedisConnChain
{
  std::string ip;
  int port;
  std::string elt4;
  int elt5;
  int elt6;
  int elt7;
};

std::vector<RedisConnChain> RedisConnectionList;

int main()
{

//Application Setup

//Read the Redis Configuration File
//Open the file
logging->info("Opening redis.properties");
std::string line;
std::ifstream file ("redis.properties");

if (file.is_open()) {
  while (getline (file, line) ) {
    //Read a line from the property file
    logging->debug("Line read from configuration file:");
    logging->debug(line);

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
      logging->debug(var_name);
      logging->debug(var_value);
      if (var_name=="RedisConnectionString") {
        //Read a string in the format 127.0.0.1--7000----2--5--0
        RedisConnChain chain;

        //Retrieve the first value
        int spacer_position = var_value.find("--", 0);
        std::string str1 = var_value.substr(0, spacer_position);
        logging->debug("IP Address Recovered");
        logging->debug(str1);
        chain.ip = str1;

        //Retrieve the second value
        std::string new_value = var_value.substr(spacer_position+2, var_value.length() - 1);
        logging->debug("New Search String");
        logging->debug(new_value);
        spacer_position = new_value.find("--", 0);
        str1 = new_value.substr(0, spacer_position);
        logging->debug("Port Recovered");
        logging->debug(str1);
        chain.port = std::stoi(str1);

        //Retrieve the third value
        new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
        logging->debug("New Search String");
        logging->debug(new_value);
        spacer_position = new_value.find("--", 0);
        str1 = new_value.substr(0, spacer_position);
        logging->debug("Password Recovered");
        logging->debug(str1);
        chain.elt4 = str1;

        //Retrieve the fourth value
        new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
        logging->debug("New Search String");
        logging->debug(new_value);
        spacer_position = new_value.find("--", 0);
        str1 = new_value.substr(0, spacer_position);
        logging->debug("Value Recovered");
        logging->debug(str1);
        chain.elt5 = std::stoi(str1);

        //Retrieve the fifth value
        new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
        logging->debug("New Search String");
        logging->debug(new_value);
          spacer_position = new_value.find("--", 0);
          str1 = new_value.substr(0, spacer_position);
          logging->debug("Value Recovered");
          logging->debug(str1);
        chain.elt6 = std::stoi(str1);

        //Retrieve the final value
        new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
        logging->debug("New Search String");
        logging->debug(new_value);
        spacer_position = new_value.find("--", 0);
        str1 = new_value.substr(0, spacer_position);
        logging->debug("Value Recovered");
        logging->debug(str1);
        chain.elt7 = std::stoi(str1);

        RedisConnectionList.push_back(chain);
      }
    }
  }
  file.close();
}


//Read the Logging Configuration File
try {
  log4cpp::PropertyConfigurator::configure("log4cpp_test.properties");
}
catch ( log4cpp::ConfigureFailure &e ) {
  std::cout << "[log4cpp::ConfigureFailure] caught while reading logging.properties" << std::endl;
  std::cout << e.what();
  exit(1);
}

log4cpp::Category& root = log4cpp::Category::getRoot();

log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("sub1"));

log4cpp::Category& log = log4cpp::Category::getInstance(std::string("sub1.log"));

logging = &log;

//Set up internal variables
log.info("Internal Variables Intialized");

//Set up the Redis Admin
//Set up our Redis Connection List
int conn_list_size = RedisConnectionList.size();
RedisNode RedisList1[conn_list_size];
{
int y = 0;
for (int y = 0; y < conn_list_size; ++y)
{
  //Pull the values from RedisConnectionList
  RedisNode redis_n;
  redis_n.dbindex = y;
  RedisConnChain redis_chain = RedisConnectionList[y];
  redis_n.host = redis_chain.ip.c_str();
  redis_n.port = redis_chain.port;
  redis_n.passwd = redis_chain.elt4.c_str();
  redis_n.poolsize = redis_chain.elt5;
  redis_n.timeout = redis_chain.elt6;
  redis_n.role = redis_chain.elt7;
  logging->debug("Line added to Redis Configuration List with IP:");
  logging->debug(redis_n.host);

  RedisList1[y] = redis_n;
}
}
logging->info("Redis Connection List Built");

//Generate the UUID's for the benchmarks
int i=0;
int uuid_gen_result;
uuid_t uuid;
char uuid_str[37];
for (i=0; i< 1001; i++) {
  //Generate a new key for the object
  uuid_gen_result = 0;
  uuid_gen_result = uuid_generate_time_safe(uuid);

  if (uuid_gen_result == -1) {
    logging->error("UUID Generated in an unsafe manner that exposes a potential security risk");
    logging->error("http://linux.die.net/man/3/uuid_generate");
    logging->error("Please take the needed actions to allow uuid generation with a safe generator");
  }

  uuid_unparse_lower(uuid, uuid_str);
  uuid_list.push_back(uuid_str);
}

//Set up Redis Connection
xRedis = new xRedisAdmin (RedisList1, conn_list_size);
logging->info("Connected to Redis");

//------------------------------Run Tests-------------------------------------//
//----------------------------------------------------------------------------//

hayai::ConsoleOutputter consoleOutputter;

hayai::Benchmarker::AddOutputter(consoleOutputter);
hayai::Benchmarker::RunAllTests();

//-------------------------Post-Test Teardown---------------------------------//
//----------------------------------------------------------------------------//

delete xRedis;

return 0;

}
