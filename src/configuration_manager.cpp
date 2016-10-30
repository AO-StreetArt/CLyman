#include "configuration_manager.h"

ConfigurationManager::~ConfigurationManager() {
  if (isConsulActive)
  {
    ca->deregister_service(*s);
    delete s;
    delete ca;
  }
  delete consul_factory;
  delete props_factory;
}

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
  config_logging->info("Opening properties file:");
  config_logging->info(file_path);

  //Get a properties file manager, which will give us access to the file in a hashmap
  PropertiesReaderInterface *props = props_factory->get_properties_reader_interface(file_path);

  if (props->opt_exist("DB_ConnectionString")) {
    DB_ConnStr=props->get_opt("DB_ConnectionString");
    config_logging->info("DB Connection String:");
    config_logging->info(DB_ConnStr);
  }
  if (props->opt_exist("Smart_Update_Buffer_Duration")) {
    SUB_Duration=std::stoi(props->get_opt("Smart_Update_Buffer_Duration"));
    config_logging->info("Smart Update Buffer Duration:");
    config_logging->info(props->get_opt("Smart_Update_Buffer_Duration"));
  }
  if (props->opt_exist("DB_AuthenticationActive")) {
    if (props->get_opt("DB_AuthenticationActive")=="True") {
      DB_AuthActive=true;
      config_logging->info("DB Authentication Active");
    }
    else {
      DB_AuthActive=false;
      config_logging->info("DB Authentication Inactive");
    }
  }
  if (props->opt_exist("DB_Password")) {
    DB_Pswd=props->get_opt("DB_Password");
    config_logging->info("DB Password:");
    config_logging->info(DB_Pswd);
  }
  if (props->opt_exist("0MQ_OutboundConnectionString")) {
    OMQ_OBConnStr = props->get_opt("0MQ_OutboundConnectionString");
    config_logging->info("Outbound 0MQ Connection:");
    config_logging->info(OMQ_OBConnStr);
  }
  if (props->opt_exist("0MQ_InboundConnectionString")) {
    OMQ_IBConnStr = props->get_opt("0MQ_InboundConnectionString");
    config_logging->info("Inbound 0MQ Connection:");
    config_logging->info(OMQ_IBConnStr);
  }
  if (props->opt_exist("SmartUpdatesActive")) {
    if (props->get_opt("SmartUpdatesActive")=="True") {
      SmartUpdatesActive=true;
      config_logging->info("Smart Updates Active");
    }
    else {
      SmartUpdatesActive=false;
      config_logging->info("Smart Updates Inactive");
    }
  }
  if (props->opt_exist("MessageFormat")) {
    if (props->get_opt("MessageFormat")=="json") {
      MessageFormatJSON=true;
      MessageFormatProtoBuf=false;
      config_logging->info("Message Format set to JSON");
    }
    else if (props->get_opt("MessageFormat") == "protocol-buffer") {
      MessageFormatJSON=false;
      MessageFormatProtoBuf=true;
      config_logging->info("Message Format set to Protocol Buffers");
    }
  }
  if (props->opt_exist("RedisBufferFormat")) {
    if (props->get_opt("RedisBufferFormat")=="json") {
      RedisFormatJSON=true;
      RedisFormatProtoBuf=false;
      config_logging->info("Redis Buffer Format set to JSON");
    }
    else if (props->get_opt("RedisBufferFormat") == "protocol-buffer") {
      RedisFormatJSON=false;
      RedisFormatProtoBuf=true;
      config_logging->info("Redis Buffer Format set to Protocol Buffers");
    }
  }
  if (props->opt_exist("StampTransactionId")) {
    if (props->get_opt("StampTransactionId") == "True") {
      StampTransactionId = true;
      config_logging->info("Transaction ID's Enabled");
    }
    else {
      StampTransactionId = false;
      config_logging->info("Transaction ID's Disabled");
    }
  }
  if (props->opt_exist("SendOutboundFailureMsg")) {
    if (props->get_opt("SendOutboundFailureMsg") == "True") {
      SendOutboundFailureMsg = true;
      config_logging->info("Sending Outbound Failure Messages Enabled");
    }
    else {
      SendOutboundFailureMsg = false;
      config_logging->info("Sending Outbound Failure Messages Disabled");
    }
  }
  if (props->opt_exist("EnableObjectLocking")) {
    if (props->get_opt("EnableObjectLocking") == "True") {
      EnableObjectLocking = true;
      config_logging->info("Object Locking Enabled");
    }
    else {
      EnableObjectLocking = false;
      config_logging->info("Object Locking Disabled");
    }
  }

  if (props->list_exist("RedisConnectionString")) {
    std::vector<std::string> conn_list = props->get_list("RedisConnectionString");
    for (std::size_t i = 0; i < conn_list.size(); i++)
    {

      std::string var_value = conn_list[i];
      config_logging->info("Redis Connection:");
      config_logging->debug(var_value);

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
  delete props;
  return true;
}

//---------------------------Configure from Consul----------------------------//

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
  std::string resp_str;
  //Get a JSON List of the responses
  std::string config_json = "";
  std::string query_key = "clyman/" + key;
  try {
    config_json = ca->get_config_value(query_key);
    config_logging->debug("Configuration JSON Retrieved:");
    config_logging->debug(config_json);
  }
  catch (std::exception& e) {
    config_logging->error("Exception encountered during Consul Configuration Retrieval");
    config_logging->error(e.what());
    throw e;
  }

  //Parse the JSON Response
  rapidjson::Document d;

  if (!config_json.empty()) {
    try {
      config_logging->debug("Config Value retrieved from Consul:");
      config_logging->debug(key);
      config_logging->debug(config_json);
      const char * config_cstr = config_json.c_str();
      d.Parse(config_cstr);
    }
    //Catch a possible error and write to logs
    catch (std::exception& e) {
      config_logging->error("Exception occurred while parsing Consul Service Response:");
      config_logging->error(e.what());
    }
  }
  else {
    config_logging->error("Configuration Value not found");
    config_logging->error(key);
    return resp_str;
  }

  //Get the object out of the array
  const rapidjson::Value& v = d[0];

  // if (v.IsObject())
  // {
    const rapidjson::Value& resp = v["Value"];
    if (resp.IsString()){
      resp_str = resp.GetString();
      //Transform the object from base64
      return ca->base64_decode(resp_str);
    }
  // }
  return "";
}

//Configure based on the Services List and Key/Value store from Consul
bool ConfigurationManager::configure_from_consul (std::string consul_path, std::string ip, std::string port)
{

  ca = consul_factory->get_consul_interface( consul_path );
  config_logging->info ("Connecting to Consul");
  config_logging->info (consul_path);

  //Now, use the Consul Admin to configure the app

  std::string internal_address;

  //Step 1a: Generate new connectivity information for the inbound service from command line arguments
  if (ip == "localhost"){
    internal_address = "tcp://*:";
  }
  else {
    internal_address = "tcp://" + ip + ":";
  }

  OMQ_IBConnStr = internal_address + port;

  //Step 1b: Register the Service with Consul

  //Build a new service definition for this currently running instance of clyman
  std::string name = "CLyman";
  s = consul_factory->get_service_interface(node_id, name, internal_address, port);
  s->add_tag("ZMQ");

  //Register the service
  bool register_success = false;
  try {
    register_success = ca->register_service(*s);
  }
  catch (std::exception& e) {
    config_logging->error("Exception encountered during Service Registration");
    config_logging->error(e.what());
    throw e;
  }

  if (!register_success) {
    config_logging->error("Failed to register with Consul");
    return false;
  }

  //Step 2: Get the key-value information for deployment-wide config (Including OB ZeroMQ Connectivity)
  DB_ConnStr=get_consul_config_value("DB_ConnectionString");
  config_logging->debug("Database Connection String:");
  config_logging->debug(DB_ConnStr);
  std::string sub_dur_str = get_consul_config_value("Smart_Update_Buffer_Duration");
  if (!sub_dur_str.empty()) {
    SUB_Duration=std::stoi(sub_dur_str);
    config_logging->debug("Smart Update Buffer Duration:");
    config_logging->debug(sub_dur_str);
  }
  else {
    config_logging->error("No Smart Update Buffer duration found");
  }
  DB_Pswd = get_consul_config_value("DB_Password");
  config_logging->debug("Database Password:");
  config_logging->debug(DB_Pswd);
  DB_AuthActive=false;
  if (!DB_Pswd.empty()) {
    DB_AuthActive=true;
  }
  OMQ_OBConnStr = get_consul_config_value("0MQ_OutboundConnectionString");
  config_logging->debug("Outbound 0MQ Connection String:");
  config_logging->debug(OMQ_OBConnStr);
  std::string sua = get_consul_config_value("Smart_Updates_Active");
  config_logging->debug("Smart Updates Active:");
  config_logging->debug(sua);
  if (sua == "True") {
    SmartUpdatesActive=true;
  }
  else {
    SmartUpdatesActive=false;
  }
  std::string msg_format_str = get_consul_config_value("MessageFormat");
  config_logging->debug("Message Format:");
  config_logging->debug(msg_format_str);
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
  config_logging->debug("Redis Buffer Format:");
  config_logging->debug(redis_format_str);
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

  std::string tran_ids_active = get_consul_config_value("StampTransactionId");
  config_logging->debug("Transaction ID & Atomic Transactions Enabled:");
  config_logging->debug(tran_ids_active);
  if (tran_ids_active == "True") {
    StampTransactionId = true;
  }
  else {
    StampTransactionId = false;
  }

  std::string ob_failure_msg = get_consul_config_value("SendOutboundFailureMsg");
  config_logging->debug("Sending Outbound Failure Messages Enabled:");
  config_logging->debug(ob_failure_msg);
  if (ob_failure_msg == "True") {
    SendOutboundFailureMsg = true;
  }
  else {
    SendOutboundFailureMsg = false;
  }

  std::string enable_locking_message = get_consul_config_value("EnableObjectLocking");
  config_logging->debug("Object Locking Enabled:");
  config_logging->debug(enable_locking_message);
  if (enable_locking_message == "True") {
    EnableObjectLocking = true;
  }
  else {
    EnableObjectLocking = false;
  }

  //Read from a set of global config values in consul
  //This value is stored the same way as in a properties file, but is stored in
  //one key and are delimited by the character ';'
  std::string redis_conn_str = get_consul_config_value("RedisConnectionString");
  char delim (';');
  std::vector<std::string> redis_chains = split( redis_conn_str,  delim);
  std::string var_value;
  config_logging->debug("Redis Connections:");
  config_logging->debug(redis_conn_str);
  for (std::size_t i = 0; i < redis_chains.size(); i++)
	{
    //Read a string in the format 127.0.0.1--7000----2--5--0
    RedisConnChain chain;

    var_value = redis_chains[i];

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

  return true;
}

//----------------------External Configuration Methods------------------------//

//The publicly exposed configure function that determines where configs need to come from
bool ConfigurationManager::configure ()
{
  //Null Check
  if (!cli)
  {
    config_logging->error("Configure called with null pointer to Command Line Interpreter");
    return false;
  }
  else {

    //Check if we have a configuration file specified
    if ( cli->opt_exist("-config-file") ) {
      return configure_from_file( cli->get_opt("-config-file") );
    }

    //Check if we have a consul address specified

    else if ( cli->opt_exist("-consul-addr") && cli->opt_exist("-ip") && cli->opt_exist("-port") && cli->opt_exist("couchbase-addr") && cli->opt_exist("couchbase-pswd"))
    {
      bool suc = configure_from_consul( cli->get_opt("-consul-addr"), cli->get_opt("-ip"), cli->get_opt("-port") );
      if (suc) {
        DB_ConnStr = cli->get_opt("-couchbase-addr");
        DB_AuthActive = true;
        DB_Pswd = cli->get_opt("-couchbase-pswd");
        isConsulActive = true;
      }
      else {
        config_logging->error("Configuration from Consul failed, keeping defaults");
      }
    }

    else if ( cli->opt_exist("-consul-addr") && cli->opt_exist("-ip") && cli->opt_exist("-port") && cli->opt_exist("couchbase-addr"))
    {
      bool succ = configure_from_consul( cli->get_opt("-consul-addr"), cli->get_opt("-ip"), cli->get_opt("-port") );
      if (succ) {
        DB_ConnStr = cli->get_opt("-couchbase-addr");
        DB_AuthActive = false;
        isConsulActive = true;
      }
      else {
      config_logging->error("Configuration from Consul failed, keeping defaults");
      }
    }

    else if ( cli->opt_exist("-consul-addr") && cli->opt_exist("-ip") && cli->opt_exist("-port"))
    {
      bool ret_val =  configure_from_consul( cli->get_opt("-consul-addr"), cli->get_opt("-ip"), cli->get_opt("-port") );
      if (ret_val) {
        isConsulActive = true;
      }
      return ret_val;
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
  return false;
}
