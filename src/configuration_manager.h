//This implements the Configuration Manager

//This takes in a Command Line Interpreter, and based on the options provided,
//decides how the application needs to be configured.  It may configure either
//from a configuration file, or from a Consul agent

#include <string>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <errno.h>
#include <sys/stat.h>

#include "lyman_log.h"

#include "aossl/factory_consul.h"
#include "aossl/factory_http_client.h"
#include "aossl/factory_props.h"

#include "aossl/factory/commandline_interface.h"
#include "aossl/factory/consul_interface.h"
#include "aossl/factory/logging_interface.h"
#include "aossl/factory/redis_interface.h"
#include "aossl/factory/properties_reader_interface.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#ifndef CONFIG_MANAGER
#define CONFIG_MANAGER

class ConfigurationManager
{

ConsulComponentFactory *consul_factory = NULL;
PropertyReaderFactory *props_factory = NULL;

//Internal Consul Administrator
ConsulInterface *ca = NULL;
bool isConsulActive;

//Command Line Interpreter holding config arguments
CommandLineInterface *cli = NULL;

//Consul Service Definition
ServiceInterface *s = NULL;

//Configuration Variables
std::string DB_ConnStr;
std::string DB_Name;
std::string DB_CollectionName;
std::string OMQ_OBConnStr;
std::string OMQ_IBConnStr;
bool SmartUpdatesActive;
bool MessageFormatJSON;
bool MessageFormatProtoBuf;
bool RedisFormatJSON;
bool RedisFormatProtoBuf;
int SUB_Duration;
std::vector<RedisConnChain> RedisConnectionList;
std::string HealthCheckScript;
int HealthCheckInterval;
bool StampTransactionId;
bool SendOutboundFailureMsg;
bool EnableObjectLocking;

//The Current Node ID
std::string node_id;

//String Manipulations

//Split a string, based on python's split method
std::vector<std::string> split(std::string inp_string, char delim);

//Internal Configuration Methods

//File Config
//bool file_exists (std::string name);
bool configure_from_file (std::string file_path);

//Consul Config
std::string get_consul_config_value(std::string key);
bool configure_from_consul (std::string consul_path, std::string ip, std::string port);

public:
  //Constructor
  //Provides a set of default values that allow CLyman to run locally in a 'dev' mode
  ConfigurationManager(CommandLineInterface *c, std::string instance_id) {cli = c;\
    DB_ConnStr="mongodb://localhost:27017/"; DB_CollectionName="test"; DB_Name="test"; \
      OMQ_OBConnStr="tcp://localhost:5556";OMQ_IBConnStr="tcp://*:5555"; SmartUpdatesActive=false;\
        MessageFormatJSON=true; MessageFormatProtoBuf=false; RedisFormatJSON=false;\
          RedisFormatProtoBuf=false; SUB_Duration=1; HealthCheckScript=""; HealthCheckInterval=0;\
            isConsulActive=false;StampTransactionId=false;SendOutboundFailureMsg=false;\
              EnableObjectLocking=false;node_id=instance_id;consul_factory=new ConsulComponentFactory;\
                props_factory = new PropertyReaderFactory;}
  ~ConfigurationManager();

  //Populate the configuration variables
  bool configure();

  //Get configuration values
  std::string get_dbconnstr() {return DB_ConnStr;}
  bool get_dbcollection() {return DB_CollectionName;}
  std::string get_dbname() {return DB_Name;}
  std::string get_obconnstr() {return OMQ_OBConnStr;}
  std::string get_ibconnstr() {return OMQ_IBConnStr;}
  bool get_smartupdatesactive() {return SmartUpdatesActive;};
  bool get_mfjson() {return MessageFormatJSON;}
  bool get_mfprotobuf() {return MessageFormatProtoBuf;}
  bool get_rfjson() {return RedisFormatJSON;}
  bool get_rfprotobuf() {return RedisFormatProtoBuf;}
  int get_subduration() {return SUB_Duration;}
  std::vector<RedisConnChain> get_redisconnlist() {return RedisConnectionList;}
  bool get_transactionidsactive() {return StampTransactionId;}
  bool get_sendobfailuresactive() {return SendOutboundFailureMsg;}
  bool get_objectlockingenabled() {return EnableObjectLocking;}

  //Get the Current Node ID
  std::string get_nodeid() {return node_id;}
};

#endif
