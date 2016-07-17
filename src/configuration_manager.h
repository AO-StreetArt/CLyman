//This implements the Configuration Manager, which parses a configuration file
//and stores the results for use later

#include <string>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <vector>


#include <aossl/cli.h>
#include <aossl/consul_admin.h>
#include <aossl/logging.h>
#include <aossl/uuid_admin.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#ifndef CONFIG_MANAGER
#define CONFIG_MANAGER

struct RedisConnChain
{
  std::string ip;
  int port;
  std::string elt4;
  int elt5;
  int elt6;
  int elt7;
};

class ConfigurationManager
{
//Internal Consul Administrator
ConsulAdmin *ca;
Service *s;

//Configuration Variables
std::string DB_ConnStr;
bool DB_AuthActive;
std::string DB_Pswd;
std::string OMQ_OBConnStr;
std::string OMQ_IBConnStr;
bool SmartUpdatesActive;
bool MessageFormatJSON;
bool MessageFormatProtoBuf;
bool RedisFormatJSON;
bool RedisFormatProtoBuf;
int SUB_Duration;
std::vector<RedisConnChain> RedisConnectionList;

//Internal Configuration Methods
bool file_exists (std::string name);
void configure_from_file (std::string file_path);
std::string base64_decode(std::string const& s);
std::vector<std::string> split(std::string inp_string, char delim);
std::string get_consul_config_value(std::string key);
void configure_from_consul (std::string consul_path, std::string ip, std::string port);

public:
  //Constructor
  //Provides a set of default values that allow CLyman to run locally in a 'dev' mode
  ConfigurationManager() {DB_ConnStr="couchbase://localhost/default"; DB_AuthActive=false; DB_Pswd=""; OMQ_OBConnStr="tcp://localhost:5556";OMQ_IBConnStr="tcp://*:5555"; SmartUpdatesActive=false; MessageFormatJSON=true; MessageFormatProtoBuf=false; SUB_Duration=1;}
  ~ConfigurationManager() {if (ca) {ca->deregister_service(s); delete s; delete ca;}}

  //Populate the configuration variables
  bool configure(CommandLineInterpreter *cli);

  //Get configuration values
  std::string get_dbconnstr() {return DB_ConnStr;}
  bool get_dbauthactive() {return DB_AuthActive;}
  std::string get_dbpswd() {return DB_Pswd;}
  std::string get_obconnstr() {return OMQ_OBConnStr;}
  std::string get_ibconnstr() {return OMQ_IBConnStr;}
  bool get_smartupdatesactive() {return SmartUpdatesActive;};
  bool get_mfjson() {return MessageFormatJSON;}
  bool get_mfprotobuf() {return MessageFormatProtoBuf;}
  bool get_rfjson() {return RedisFormatJSON;}
  bool get_rfprotobuf() {return RedisFormatProtoBuf;}
  int get_subduration() {return SUB_Duration;}
  std::vector<RedisConnChain> get_redisconnlist() {return RedisConnectionList;}
};

#endif
