#include "configuration_manager.h"

//----------------------Internal Configuration Methods------------------------//

//----------------------------Configure from File-----------------------------//

//Does a file exist?
//bool file_exists( std::string name )
//{
  //struct stat buffer;
  //return (stat (name.c_str(), &buffer) == 0);
//}

//Configure based on a properties file
bool ConfigurationManager::configure_from_file (std::string file_path)
{

  //Open the file
  logging->info("CONFIGURE: Opening properties file:");
  logging->info(file_path);
  std::string line;

  struct stat buffer;
  if (stat (file_path.c_str(), &buffer) == 0) {
  //if ( file_exists( file_path ) ) {

  std::ifstream file (file_path);

  if (file.is_open()) {
    while (getline (file, line) ) {
      //Read a line from the property file
      logging->debug("CONFIGURE: Line read from configuration file:");
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
          logging->debug("CONFIGURE: IP Address Recovered");
          logging->debug(str1);
          chain.ip = str1;

          //Retrieve the second value
          std::string new_value = var_value.substr(spacer_position+2, var_value.length() - 1);
          logging->debug("CONFIGURE: New Search String");
          logging->debug(new_value);
          spacer_position = new_value.find("--", 0);
          str1 = new_value.substr(0, spacer_position);
          logging->debug("CONFIGURE: Port Recovered");
          logging->debug(str1);
          chain.port = std::stoi(str1);

          //Retrieve the third value
          new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
          logging->debug("CONFIGURE: New Search String");
          logging->debug(new_value);
          spacer_position = new_value.find("--", 0);
          str1 = new_value.substr(0, spacer_position);
          logging->debug("CONFIGURE: Password Recovered");
          logging->debug(str1);
          chain.elt4 = str1;

          //Retrieve the fourth value
          new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
          logging->debug("CONFIGURE: New Search String");
          logging->debug(new_value);
          spacer_position = new_value.find("--", 0);
          str1 = new_value.substr(0, spacer_position);
          logging->debug("CONFIGURE: Value Recovered");
          logging->debug(str1);
          chain.elt5 = std::stoi(str1);

          //Retrieve the fifth value
          new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
          logging->debug("CONFIGURE: New Search String");
          logging->debug(new_value);
          spacer_position = new_value.find("--", 0);
          str1 = new_value.substr(0, spacer_position);
          logging->debug("CONFIGURE: Value Recovered");
          logging->debug(str1);
          chain.elt6 = std::stoi(str1);

          //Retrieve the final value
          new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
          logging->debug("CONFIGURE: New Search String");
          logging->debug(new_value);
          spacer_position = new_value.find("--", 0);
          str1 = new_value.substr(0, spacer_position);
          logging->debug("CONFIGURE: Value Recovered");
          logging->debug(str1);
          chain.elt7 = std::stoi(str1);

          RedisConnectionList.push_back(chain);
        }
      }
    }
    file.close();
  }
}
else
{
  logging->error("CONFIGURE: Configuration File not found");
  return false;
}
return true;
}

//---------------------------Configure from Consul----------------------------//

std::string ConfigurationManager::base64_decode(std::string const& encoded_string) {

  static const std::string base64_chars =
               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
               "abcdefghijklmnopqrstuvwxyz"
               "0123456789+/";

  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

std::vector<std::string> ConfigurationManager::split(std::string inp_string, char delim)
{
std::vector<std::string> elems;
std::stringstream ss(inp_string);
std::string item;
while (std::getline(ss, item, delim)) {
elems.push_back(item);
}
return elems;
}

std::string ConfigurationManager::get_consul_config_value(std::string key)
{
  //Get a JSON List of the responses
  std::string config_json = ca->get_config_value(key);
  const char * config_cstr = config_json.c_str();

  //Parse the JSON Response
  rapidjson::Document d;
  rapidjson::Value *s;

  try {
    d.Parse(config_cstr);
  }
  //Catch a possible error and write to logs
  catch (std::exception& e) {
    logging->error("Exception occurred while parsing Consul Service Response:");
    logging->error(e.what());
  }

  //Get the object out of the array
  const rapidjson::Value& v = d[0];

  std::string resp_str;

  if (v.IsObject())
  {
    const rapidjson::Value& resp = v["Value"];
    resp_str = resp.GetString();
  }

  //Transform the object from base64
  return base64_decode(resp_str);
}

//Configure based on the Services List and Key/Value store from Consul
bool ConfigurationManager::configure_from_consul (std::string consul_path, std::string ip, std::string port, uuidAdmin *ua)
{
  //Check if we already have a consul admin, if not initialize one with the given consul path
  if (!ca)
  {
    ca = new ConsulAdmin ( consul_path );
    logging->info ("CONFIGURE: Connecting to Consul");
    logging->info (consul_path);
  }

  //Now, use the Consul Admin to configure the app

  std::string internal_address;

  //Step 1b: Generate new connectivity information for the inbound service from command line arguments
  if (ip == "localhost"){
    internal_address = "tcp://*:";
  }
  else {
    internal_address = "tcp://" + ip + ":";
  }

  OMQ_IBConnStr = internal_address + port;

  //Step 1c: Register the Service with Consul

  //Build a new service definition for this currently running instance of clyman
  std::string id = "CLyman-" + ua->generate();
  std::string name = "CLyman";
  s = new Service (id, name, internal_address, port);
  s->add_tag("ZMQ");

  //Register the service
  ca->register_service(*s);

  //Step 2: Get the key-value information for deployment-wide config (Including OB ZeroMQ Connectivity)
  DB_ConnStr=get_consul_config_value("DB_ConnectionString");
  SUB_Duration=std::stoi(get_consul_config_value("Smart_Update_Buffer_Duration"));
  DB_Pswd = get_consul_config_value("DB_Password");
  DB_AuthActive=false;
  if (!DB_Pswd.empty()) {
    DB_AuthActive=true;
  }
  OMQ_OBConnStr = get_consul_config_value("0MQ_OutboundConnectionString");
  std::string sua = get_consul_config_value("Smart_Updates_Active");
  if (sua == "True") {
    SmartUpdatesActive=true;
  }
  else {
    SmartUpdatesActive=false;
  }
  std::string msg_format_str = get_consul_config_value("MessageFormat");
  if (msg_format_str == "json")
  {
    MessageFormatJSON=true;
    MessageFormatProtoBuf=false;
  }
  else
  {
    MessageFormatJSON=false;
    MessageFormatProtoBuf=true;
  }
  std::string redis_format_str = get_consul_config_value("RedisBufferFormat");
  if (redis_format_str == "json")
  {
    RedisFormatJSON=true;
    RedisFormatProtoBuf=false;
  }
  else
  {
    RedisFormatJSON=false;
    RedisFormatProtoBuf=true;
  }

  //Read from a set of global config values in consul
  //This value is stored the same way as in a properties file, but is stored in
  //one key and are delimited by the character ';'
  std::string redis_conn_str = get_consul_config_value("RedisConnectionString");
  char delim (';');
  std::vector<std::string> redis_chains = split( redis_conn_str,  delim);
  std::string var_value;

  for (int i = 0; i < redis_chains.size(); i++)
	{
    //Read a string in the format 127.0.0.1--7000----2--5--0
    RedisConnChain chain;

    var_value = redis_chains[i];

    //Retrieve the first value
    int spacer_position = var_value.find("--", 0);
    std::string str1 = var_value.substr(0, spacer_position);
    logging->debug("CONFIGURE: IP Address Recovered");
    logging->debug(str1);
    chain.ip = str1;

    //Retrieve the second value
    std::string new_value = var_value.substr(spacer_position+2, var_value.length() - 1);
    logging->debug("CONFIGURE: New Search String");
    logging->debug(new_value);
    spacer_position = new_value.find("--", 0);
    str1 = new_value.substr(0, spacer_position);
    logging->debug("CONFIGURE: Port Recovered");
    logging->debug(str1);
    chain.port = std::stoi(str1);

    //Retrieve the third value
    new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
    logging->debug("CONFIGURE: New Search String");
    logging->debug(new_value);
    spacer_position = new_value.find("--", 0);
    str1 = new_value.substr(0, spacer_position);
    logging->debug("CONFIGURE: Password Recovered");
    logging->debug(str1);
    chain.elt4 = str1;

    //Retrieve the fourth value
    new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
    logging->debug("CONFIGURE: New Search String");
    logging->debug(new_value);
    spacer_position = new_value.find("--", 0);
    str1 = new_value.substr(0, spacer_position);
    logging->debug("CONFIGURE: Value Recovered");
    logging->debug(str1);
    chain.elt5 = std::stoi(str1);

    //Retrieve the fifth value
    new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
    logging->debug("CONFIGURE: New Search String");
    logging->debug(new_value);
    spacer_position = new_value.find("--", 0);
    str1 = new_value.substr(0, spacer_position);
    logging->debug("CONFIGURE: Value Recovered");
    logging->debug(str1);
    chain.elt6 = std::stoi(str1);

    //Retrieve the final value
    new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
    logging->debug("CONFIGURE: New Search String");
    logging->debug(new_value);
    spacer_position = new_value.find("--", 0);
    str1 = new_value.substr(0, spacer_position);
    logging->debug("CONFIGURE: Value Recovered");
    logging->debug(str1);
    chain.elt7 = std::stoi(str1);

    RedisConnectionList.push_back(chain);
  }
}

//----------------------External Configuration Methods------------------------//

//The publicly exposed configure function that determines where configs need to come from
bool ConfigurationManager::configure (CommandLineInterpreter *cli, uuidAdmin *ua)
{
  //Null Check
  if (!cli)
  {
    logging->error("CONFIGURE: Configure called with null pointer to Command Line Interpreter");
    return false;
  }
  else {

    //Check if we have a configuration file specified
    if ( cli->opt_exist("-config-file") ) {
      return configure_from_file( cli->get_opt("-config-file") );
    }

    //Check if we have a consul address specified
    else if ( cli->opt_exist("-consul-addr") && cli->opt_exist("-ip") && cli->opt_exist("-port"))
    {
      return configure_from_consul( cli->get_opt("-consul-addr"), cli->get_opt("-ip"), cli->get_opt("-port"), ua );
    }

    //Check for the dev flag, which starts up with default ports and no consul connection
    else if ( cli->opt_exist("-dev") ) {
      return true;
    }

    //If we have nothing specified, look for a lyman.properties file
    else
    {
	  bool file_success;
      file_success = configure_from_file( "lyman.properties" );
      return file_success;
	}

  }
}
