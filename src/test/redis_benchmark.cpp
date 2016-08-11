#include <hayai/hayai.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <uuid/uuid.h>

#include <aossl/logging.h>

#include <aossl/xredis_admin.h>

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

std::string uuid_str = uuid_list[savecounter];

//save
bool bRet = xRedis->save( uuid_str.c_str(), "123");
if (!bRet) {
logging->error("Error putting object to Redis Smart Update Buffer");
}

savecounter=savecounter+1;

}

BENCHMARK(Redis, ExistsTrue, 10, 100)
{

std::string uuid_str = uuid_list[existcounter];

//exists
bool eRet = xRedis->exists( uuid_str.c_str() );

existcounter=existcounter+1;

}

BENCHMARK(Redis, ExistsFalse, 10, 100)
{

std::string uuid_str = "TEST";

//exists
bool eRet = xRedis->exists( uuid_str.c_str() );

}

BENCHMARK(Redis, Load, 10, 100)
{

std::string uuid_str = uuid_list[getcounter];

//load
std::string strValue = xRedis->load( uuid_str.c_str() );

getcounter=getcounter+1;

}

BENCHMARK(Redis, Delete, 10, 100)
{

std::string uuid_str = uuid_list[delcounter];

//Delete
xRedis->del( uuid_str.c_str() );

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

int main()
{

//Application Setup
std::vector<RedisConnChain> RedisConnectionList;

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
        chain.elt4 = str1;

        //Retrieve the fourth value
        new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
        spacer_position = new_value.find("--", 0);
        str1 = new_value.substr(0, spacer_position);
        chain.elt5 = std::stoi(str1);

        //Retrieve the fifth value
        new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
          spacer_position = new_value.find("--", 0);
          str1 = new_value.substr(0, spacer_position);
        chain.elt6 = std::stoi(str1);

        //Retrieve the final value
        new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
        spacer_position = new_value.find("--", 0);
        str1 = new_value.substr(0, spacer_position);
        chain.elt7 = std::stoi(str1);

        RedisConnectionList.push_back(chain);
      }
    }
  }
  file.close();
}


//Read the Logging Configuration File
std::string initFileName = "src/test/log4cpp_test.properties";
logging = new Logger(initFileName);

//Set up internal variables
logging->info("Internal Variables Intialized");

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
for (i=0; i< 1001; i++) {
  //Generate a new key for the object
  std::string uuid_str = std::to_string(i);
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
delete logging;

return 0;

}
