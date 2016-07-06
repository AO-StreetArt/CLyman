#include "configuration_manager.h"

void ConfigurationManager::configure (std::string file_path)
{
  //Default Values for Configuration Variables
  DB_ConnStr="";
  DB_AuthActive=false;
  DB_Pswd="";
  OMQ_OBConnStr="";
  OMQ_IBConnStr="";
  SmartUpdatesActive=false;
  MessageFormatJSON=true;
  MessageFormatProtoBuf=false;

  //Open the file
  logging->info("Opening lyman.properties");
  std::string line;
  std::ifstream file ("lyman.properties");

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
        if (var_name=="DB_ConnectionString") {
          DB_ConnStr=var_value;
        }
        if (var_name=="Smart_Update_Buffer_Duration") {
          SUB_Duration=std::stoi(var_value);
        }
        else if (var_name=="DB_AuthenticationActive") {
          if (var_value=="True") {
            DB_AuthActive=true;
          }
          else {
            DB_AuthActive=false;
          }
        }
        else if (var_name=="DB_Password") {
          DB_Pswd=var_value;
        }
        else if (var_name=="0MQ_OutboundConnectionString") {
          OMQ_OBConnStr = var_value;
        }
        else if (var_name=="0MQ_InboundConnectionString") {
          OMQ_IBConnStr = var_value;
        }
        else if (var_name=="SmartUpdatesActive") {
          if (var_value=="True") {
            SmartUpdatesActive=true;
          }
          else {
            SmartUpdatesActive=false;
          }
        }
        else if (var_name=="MessageFormat") {
          if (var_value=="json") {
            MessageFormatJSON=true;
            MessageFormatProtoBuf=false;
          }
          else if (var_value=="protocol-buffer") {
            MessageFormatJSON=false;
            MessageFormatProtoBuf=true;
          }
        }
        else if (var_name=="RedisBufferFormat") {
          if (var_value=="json") {
            RedisFormatJSON=true;
            RedisFormatProtoBuf=false;
          }
          else if (var_value=="protocol-buffer") {
            RedisFormatJSON=false;
            RedisFormatProtoBuf=true;
          }
        }
        else if (var_name=="RedisConnectionString") {
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
}
