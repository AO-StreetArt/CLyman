//This implements the Configuration Manager, which parses a configuration file
//and stores the results for use later

#include <string>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>
#include <aossl/logging.h>

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
public:
  ConfigurationManager() {DB_ConnStr=""; DB_AuthActive=false; DB_Pswd=""; OMQ_OBConnStr="";OMQ_IBConnStr=""; SmartUpdatesActive=false; MessageFormatJSON=true; MessageFormatProtoBuf=false; SUB_Duration=1;}
  void configure(std::string file_path);
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
