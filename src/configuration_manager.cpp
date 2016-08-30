#include "configuration_manager.h"

ConfigurationManager::~ConfigurationManager() {
  if (isConsulActive)
  {
    ca->deregister_service(*s);
    delete s;
    delete ca;
  }
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
  logging->info("CONFIGURE: Opening properties file:");
  logging->info(file_path);

  //Get a properties file manager, which will give us access to the file in a hashmap
  PropertiesReaderInterface *props = factory->get_properties_reader_interface(file_path);

  if (props->opt_exist("DB_ConnectionString")) {
    DB_ConnStr=props->get_opt("DB_ConnectionString");
    logging->info("CONFIGURE: DB Connection String:");
    logging->info(DB_ConnStr);
  }
  if (props->opt_exist("Smart_Update_Buffer_Duration")) {
    SUB_Duration=std::stoi(props->get_opt("Smart_Update_Buffer_Duration"));
    logging->info("CONFIGURE: Smart Update Buffer Duration:");
    logging->info(props->get_opt("Smart_Update_Buffer_Duration"));
  }
  if (props->opt_exist("DB_AuthenticationActive")) {
    if (props->get_opt("DB_AuthenticationActive")=="True") {
      DB_AuthActive=true;
      logging->info("CONFIGURE: DB Authentication Active");
    }
    else {
      DB_AuthActive=false;
      logging->info("CONFIGURE: DB Authentication Inactive");
    }
  }
  if (props->opt_exist("DB_Password")) {
    DB_Pswd=props->get_opt("DB_Password");
    logging->info("CONFIGURE: DB Password:");
    logging->info(DB_Pswd);
  }
  if (props->opt_exist("0MQ_OutboundConnectionString")) {
    OMQ_OBConnStr = props->get_opt("0MQ_OutboundConnectionString");
    logging->info("CONFIGURE: Outbound 0MQ Connection:");
    logging->info(OMQ_OBConnStr);
  }
  if (props->opt_exist("0MQ_InboundConnectionString")) {
    OMQ_IBConnStr = props->get_opt("0MQ_InboundConnectionString");
    logging->info("CONFIGURE: Inbound 0MQ Connection:");
    logging->info(OMQ_IBConnStr);
  }
  if (props->opt_exist("SmartUpdatesActive")) {
    if (props->get_opt("SmartUpdatesActive")=="True") {
      SmartUpdatesActive=true;
      logging->info("CONFIGURE: Smart Updates Active");
    }
    else {
      SmartUpdatesActive=false;
      logging->info("CONFIGURE: Smart Updates Inactive");
    }
  }
  if (props->opt_exist("MessageFormat")) {
    if (props->get_opt("MessageFormat")=="json") {
      MessageFormatJSON=true;
      MessageFormatProtoBuf=false;
      logging->info("CONFIGURE: Message Format set to JSON");
    }
    else if (props->get_opt("MessageFormat") == "protocol-buffer") {
      MessageFormatJSON=false;
      MessageFormatProtoBuf=true;
      logging->info("CONFIGURE: Message Format set to Protocol Buffers");
    }
  }
  if (props->opt_exist("RedisBufferFormat")) {
    if (props->get_opt("RedisBufferFormat")=="json") {
      RedisFormatJSON=true;
      RedisFormatProtoBuf=false;
      logging->info("CONFIGURE: Redis Buffer Format set to JSON");
    }
    else if (props->get_opt("RedisBufferFormat") == "protocol-buffer") {
      RedisFormatJSON=false;
      RedisFormatProtoBuf=true;
      logging->info("CONFIGURE: Redis Buffer Format set to Protocol Buffers");
    }
  }
  if (props->list_exist("RedisConnectionString")) {
    std::vector<std::string> conn_list = props->get_list("RedisConnectionString");
    for (std::size_t i = 0; i < conn_list.size(); i++)
    {

      std::string var_value = conn_list[i];
      logging->info("CONFIGURE: Redis Connection:");
      logging->debug(var_value);

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
  std::string config_json = ca->get_config_value(key);
  const char * config_cstr = config_json.c_str();

  //Parse the JSON Response
  rapidjson::Document d;

  if (!config_json.empty()) {
    try {
      logging->debug("Config Value retrieved from Consul:");
      logging->debug(key);
      logging->debug(config_json);
      d.Parse(config_cstr);
    }
    //Catch a possible error and write to logs
    catch (std::exception& e) {
      logging->error("Exception occurred while parsing Consul Service Response:");
      logging->error(e.what());
    }
  }
  else {
    logging->error("Configuration Value not found");
    logging->error(key);
    return resp_str;
  }

  //Get the object out of the array
  const rapidjson::Value& v = d[0];

  if (v.IsObject())
  {
    const rapidjson::Value& resp = v["Value"];
    if (resp.IsString()){
      resp_str = resp.GetString();
      //Transform the object from base64
      return ca->base64_decode(resp_str);
    }
    else{
      return "";
    }
  }
}

//Configure based on the Services List and Key/Value store from Consul
bool ConfigurationManager::configure_from_consul (std::string consul_path, std::string ip, std::string port, uuidInterface *ua)
{

  ca = factory->get_consul_interface( consul_path );
  logging->info ("CONFIGURE: Connecting to Consul");
  logging->info (consul_path);

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

  //Go get the heartbeat script from Consul
  HealthCheckScript = get_consul_config_value("HealthCheckScript");

  std::string id = ua->generate();

  //Build a new service definition for this currently running instance of clyman
  std::string name = "CLyman";
  s = factory->get_service_interface(id, name, internal_address, port);
  s->add_tag("ZMQ");

  //Add the check
  if (!HealthCheckScript.empty()) {

    //Set up the overall heartbeat folder location
    int hb_loc_exist = mkdir ( "heartbeat_scripts", S_IRWXU | S_IRWXG );
    if (hb_loc_exist < 0) {
      logging->error("Overall Heartbeat location not created");
      logging->error(strerror(errno));
    }
    else {
      logging->info("Overall Heartbeat location created");
    }

    //Set up the instance heartbeat folder location
    std::string id = "CLyman-" + ua->generate();
    std::string my_hb_loc = "heartbeat_scripts/" + id;
    int my_hb_loc_exist = mkdir ( my_hb_loc.c_str(), S_IRWXU | S_IRWXG );
    if (my_hb_loc_exist == 0) {

      logging->info("Instance Heartbeat location created");

      //Copy the heartbeat script into the instance folder
      try {
        std::ifstream inp_file1 ("scripts/CLyman_Heartbeat_Protobuf.py", std::fstream::binary);
        std::ifstream inp_file2 ("scripts/CLyman_Heartbeat_Json.py", std::fstream::binary);
        std::ifstream inp_file3 ("scripts/ConfigManager.py", std::fstream::binary);

        std::string dest_name1 = my_hb_loc + "/CLyman_Heartbeat_Protobuf.py";
        std::string dest_name2 = my_hb_loc + "/CLyman_Heartbeat_Json.py";
        std::string dest_name3 = my_hb_loc + "/ConfigManager.py";

        std::ofstream out_file1 (dest_name1, std::fstream::trunc|std::fstream::binary);
        std::ofstream out_file2 (dest_name2, std::fstream::trunc|std::fstream::binary);
        std::ofstream out_file3 (dest_name3, std::fstream::trunc|std::fstream::binary);

        out_file1 << inp_file1.rdbuf();
        out_file2 << inp_file2.rdbuf();
        out_file3 << inp_file3.rdbuf();
      }
      //Catch a possible error and write to logs
      catch (std::exception& e) {
        logging->error("Exception occurred while copying heartbeat script to instance folder:");
        logging->error(e.what());
      }

      //Generate the configuration file for the heartbeat
      std::string hbc_name = my_hb_loc + "/heartbeat_config.txt";
      std::string hbc_text = "Destination_Address=" + OMQ_IBConnStr;
      std::ofstream hb_config;

      hb_config.open(hbc_name);
      hb_config << hbc_text;
      hb_config.close();

    }
    else {
      logging->error("Instance Heartbeat location not created");
      logging->error(strerror(errno));
    }

    //Set the health check on the service object
    std::string hcs_path = my_hb_loc + HealthCheckScript;
    s->set_check(hcs_path, HealthCheckInterval);
    HealthCheckInterval = std::stoi(get_consul_config_value("HealthCheckInterval"));
  }

  //Register the service
  bool register_success = ca->register_service(*s);

  if (!register_success) {
    logging->error("Failed to register with Consul");
    return false;
  }

  //Step 2: Get the key-value information for deployment-wide config (Including OB ZeroMQ Connectivity)
  DB_ConnStr=get_consul_config_value("DB_ConnectionString");
  logging->debug("CONFIGURE: Database Connection String:");
  logging->debug(DB_ConnStr);
  std::string sub_dur_str = get_consul_config_value("Smart_Update_Buffer_Duration");
  if (!sub_dur_str.empty()) {
    SUB_Duration=std::stoi(sub_dur_str);
    logging->debug("CONFIGURE: Smart Update Buffer Duration:");
    logging->debug(sub_dur_str);
  }
  else {
    logging->error("No Smart Update Buffer duration found");
  }
  DB_Pswd = get_consul_config_value("DB_Password");
  logging->debug("CONFIGURE: Database Password:");
  logging->debug(DB_Pswd);
  DB_AuthActive=false;
  if (!DB_Pswd.empty()) {
    DB_AuthActive=true;
  }
  OMQ_OBConnStr = get_consul_config_value("0MQ_OutboundConnectionString");
  logging->debug("CONFIGURE: Outbound 0MQ Connection String:");
  logging->debug(OMQ_OBConnStr);
  std::string sua = get_consul_config_value("Smart_Updates_Active");
  logging->debug("CONFIGURE: Smart Updates Active:");
  logging->debug(sua);
  if (sua == "True") {
    SmartUpdatesActive=true;
  }
  else {
    SmartUpdatesActive=false;
  }
  std::string msg_format_str = get_consul_config_value("MessageFormat");
  logging->debug("CONFIGURE: Message Format:");
  logging->debug(msg_format_str);
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
  logging->debug("CONFIGURE: Redis Buffer Format:");
  logging->debug(redis_format_str);
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
  logging->debug("CONFIGURE: Redis Connections:");
  logging->debug(redis_conn_str);
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
    logging->error("CONFIGURE: Configure called with null pointer to Command Line Interpreter");
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
      bool suc = configure_from_consul( cli->get_opt("-consul-addr"), cli->get_opt("-ip"), cli->get_opt("-port"), ua );
      if (suc) {
        DB_ConnStr = cli->get_opt("-couchbase-addr");
        DB_AuthActive = true;
        DB_Pswd = cli->get_opt("-couchbase-pswd");
        isConsulActive = true;
      }
      else {
        logging->error("Configuration from Consul failed, keeping defaults");
      }
    }

    else if ( cli->opt_exist("-consul-addr") && cli->opt_exist("-ip") && cli->opt_exist("-port") && cli->opt_exist("couchbase-addr"))
    {
      bool succ = configure_from_consul( cli->get_opt("-consul-addr"), cli->get_opt("-ip"), cli->get_opt("-port"), ua );
      if (succ) {
        DB_ConnStr = cli->get_opt("-couchbase-addr");
        DB_AuthActive = false;
        isConsulActive = true;
      }
      else {
      logging->error("Configuration from Consul failed, keeping defaults");
      }
    }

    else if ( cli->opt_exist("-consul-addr") && cli->opt_exist("-ip") && cli->opt_exist("-port"))
    {
      bool ret_val =  configure_from_consul( cli->get_opt("-consul-addr"), cli->get_opt("-ip"), cli->get_opt("-port"), ua );
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
