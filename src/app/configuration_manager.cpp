/*
Apache2 License Notice
Copyright 2017 Alex Barry

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "include/configuration_manager.h"

ConfigurationManager::~ConfigurationManager() {
  if (isConsulActive) {
    ca->deregister_service(*s);
    delete s;
    delete ca;
  }
  delete consul_factory;
  delete props_factory;
}

// ---------------------Internal Configuration Methods----------------------- //

// ---------------------------Configure from File---------------------------- //

// Configure based on a properties file
bool ConfigurationManager::configure_from_file(std::string file_path) {
  // Open the file
  config_logging->info("Opening properties file:");
  config_logging->info(file_path);

  // Get a properties file manager, which will give us
  // access to the file in a hashmap
  PropertiesReaderInterface *props = \
    props_factory->get_properties_reader_interface(file_path);

  if (props->opt_exist("Mongo_ConnectionString")) {
    Mongo_ConnStr = props->get_opt("Mongo_ConnectionString");
    config_logging->info("Mongo Connection String:");
    config_logging->info(DB_ConnStr);
  } else {throw ConfigurationException("No Mongo Connection Specified");}
  if (props->opt_exist("Mongo_DbName")) {
    Mongo_DbName = props->get_opt("Mongo_DbName");
    config_logging->info("Mongo DB Name:");
    config_logging->info(Mongo_DbName);
  } else {throw ConfigurationException("No Mongo DB Name Specified");}
  if (props->opt_exist("Mongo_DbCollection")) {
    Mongo_DbCollection = props->get_opt("Mongo_DbCollection");
    config_logging->info("Mongo DB Collection:");
    config_logging->info(Mongo_DbCollection);
  } else {throw ConfigurationException("No Mongo DB Collection Specified");}
  if (props->opt_exist("KafkaBrokerAddress")) {
    KafkaBrokerList = props->get_opt("KafkaBrokerAddress");
    config_logging->info("Kafka Address:");
    config_logging->info(KafkaBrokerList);
  } else {throw ConfigurationException("No Kafka Address Specified");}
  if (props->opt_exist("0MQ_InboundConnectionString")) {
    OMQ_IBConnStr = props->get_opt("0MQ_InboundConnectionString");
    config_logging->info("Inbound 0MQ Connection:");
    config_logging->info(OMQ_IBConnStr);
  } else {throw ConfigurationException("No ZMQ Connection Specified");}
  if (props->opt_exist("0MQ_Hostname")) {
    hostname = props->get_opt("0MQ_Hostname");
    config_logging->info("Inbound 0MQ Hostname:");
    config_logging->info(hostname);
  } else {throw ConfigurationException("No ZMQ Host Specified");}
  if (props->opt_exist("0MQ_Port")) {
    port = props->get_opt("0MQ_Port");
    config_logging->info("Inbound 0MQ Port:");
    config_logging->info(port);
  } else {throw ConfigurationException("No ZMQ Port Specified");}
  if (props->opt_exist("DataFormatType")) {
    std::string param_value = props->get_opt("DataFormatType");
    if (param_value == "1" || param_value == "JSON" || param_value == "json" \
      || param_value == "Json") {
      format_type = JSON_FORMAT;
    } else if (param_value == "0" || param_value == "Protobuf" \
      || param_value == "protobuf") {
      format_type = PROTO_FORMAT;
    } else {throw ConfigurationException("No Data Format Specified");}
    config_logging->info("Inbound 0MQ Connection:");
    config_logging->info(OMQ_IBConnStr);
  }
  if (props->opt_exist("StampTransactionId")) {
    if (props->get_opt("StampTransactionId") == "True") {
      StampTransactionId = true;
      config_logging->info("Transaction ID's Enabled");
    } else {
      StampTransactionId = false;
      config_logging->info("Transaction ID's Disabled");
    }
  } else {throw ConfigurationException("No Transaction ID Instruction");}
  if (props->opt_exist("AtomicTransactions")) {
    if (props->get_opt("AtomicTransactions") == "True") {
      AtomicTransactions = true;
      config_logging->info("Atomic Transactions Enabled");
    } else {
      AtomicTransactions = false;
      config_logging->info("Atomic Transactions Disabled");
    }
  } else {throw ConfigurationException("No Atomic Transactions Instruction");}
  if (props->opt_exist("ObjectLockingActive")) {
    if (props->get_opt("ObjectLockingActive") == "True") {
      ObjectLockingActive = true;
      config_logging->info("Object Locking Enabled");
    } else {
      ObjectLockingActive = false;
      config_logging->info("Object Locking Disabled");
    }
  } else {throw ConfigurationException("No Object Locking Instruction");}

  if (props->list_exist("RedisConnectionString")) {
    std::vector<std::string> conn_list = \
      props->get_list("RedisConnectionString");
    for (std::size_t i = 0; i < conn_list.size(); i++) {
      std::string var_value = conn_list[i];
      config_logging->info("Redis Connection:");
      config_logging->debug(var_value);

      // Read a string in the format 127.0.0.1--7000----2--5--0
      RedisConnChain chain;

      // Retrieve the first value
      int spacer_position = var_value.find("--", 0);
      std::string str1 = var_value.substr(0, spacer_position);
      chain.ip = str1;

      // Retrieve the second value
      std::string new_value = var_value.substr(spacer_position+2, \
        var_value.length() - 1);
      spacer_position = new_value.find("--", 0);
      str1 = new_value.substr(0, spacer_position);
      chain.port = std::stoi(str1);

      // Retrieve the third value
      new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
      spacer_position = new_value.find("--", 0);
      str1 = new_value.substr(0, spacer_position);
      chain.password = str1;

      // Retrieve the fourth value
      new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
      spacer_position = new_value.find("--", 0);
      str1 = new_value.substr(0, spacer_position);
      chain.pool_size = std::stoi(str1);

      // Retrieve the fifth value
      new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
      spacer_position = new_value.find("--", 0);
      str1 = new_value.substr(0, spacer_position);
      chain.timeout = std::stoi(str1);

      // Retrieve the final value
      new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
      spacer_position = new_value.find("--", 0);
      str1 = new_value.substr(0, spacer_position);
      chain.role = std::stoi(str1);

      RedisConnectionList.push_back(chain);
    }
  } else {throw ConfigurationException("No Redis Connection Specified");}
  delete props;
  return true;
}

// --------------------------Configure from Consul--------------------------- //

std::vector<std::string> ConfigurationManager::split(std::string inp_string, \
  char delim) {
  std::vector<std::string> elems;
  std::stringstream ss(inp_string);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::string ConfigurationManager::get_consul_config_value(std::string key) {
  config_resp_str = "__NULLSTR__";
  // Get a JSON List of the responses
  std::string config_json = "";
  std::string query_key = "clyman/" + key;
  try {
    config_json = ca->get_config_value(query_key);
    config_logging->debug("Configuration JSON Retrieved:");
    config_logging->debug(config_json);
  }
  catch (std::exception& e) {
    config_logging->error("Exception during Consul Configuration Retrieval");
    config_logging->error(e.what());
    throw e;
  }

  // Parse the JSON Response
  rapidjson::Document d;

  if (!config_json.empty()) {
    try {
      config_logging->debug("Config Value retrieved from Consul:");
      config_logging->debug(key);
      config_logging->debug(config_json);
      const char * config_cstr = config_json.c_str();
      d.Parse(config_cstr);
    }
    // Catch a possible error and write to logs
    catch (std::exception& e) {
      config_logging->error("Exception while parsing Consul Service Response:");
      config_logging->error(e.what());
    }
  } else {
    config_logging->error("Configuration Value not found");
    config_logging->error(key);
    return config_resp_str;
  }

  // Get the object out of the array
  const rapidjson::Value& v = d[0];

  // if (v.IsObject())
  // {
    const rapidjson::Value& resp = v["Value"];
    if (resp.IsString()) {
      config_resp_str = resp.GetString();
      // Transform the object from base64
      return ca->base64_decode(config_resp_str);
    }
  // }
  return config_resp_str;
}

// Configure based on the Services List and Key/Value store from Consul
bool ConfigurationManager::configure_from_consul(std::string consul_path, \
  std::string ip, std::string port, std::string advertised_host) {
  std::string internal_address;

  // Step 1a: Generate new connectivity information for the inbound service
  // from command line arguments
  if (ip == "localhost") {
    internal_address = "*";
  } else {
    internal_address = ip;
  }

  OMQ_IBConnStr = "tcp://" + internal_address + ":" + port;

  return configure_from_consul(consul_path, OMQ_IBConnStr, internal_address, port, advertised_host);
}

// Configure based on the Services List and Key/Value store from Consul
bool ConfigurationManager::configure_from_consul(std::string consul_path, \
  std::string conn_str, std::string addr, std::string port_str, std::string advertised_host) {
  ca = consul_factory->get_consul_interface(consul_path);
  config_logging->info("Connecting to Consul");
  config_logging->info(consul_path);

  // Now, use the Consul Admin to configure the app

  // Step 1b: Register the Service with Consul
  // Build a new service definition for this currently running instance
  std::string name = "Clyman";
  if (!(advertised_host.empty())) {addr = advertised_host;}
  s = consul_factory->get_service_interface(node_id, name, addr, port_str);
  s->add_tag("ZMQ");

  // Register the service
  bool register_success = false;
  try {
    register_success = ca->register_service(*s);
  }
  catch (std::exception& e) {
    config_logging->error("Exception encountered during Service Registration");
    config_logging->error(e.what());
    return false;
  }

  if (!register_success) {
    config_logging->error("Failed to register with Consul");
    return false;
  }

  // Step 2: Get the key-value information for deployment-wide
  // config (Including OB ZeroMQ Connectivity)
  Mongo_ConnStr = get_consul_config_value("Mongo_ConnectionString");
  config_logging->debug("Mongo Connection String:");
  config_logging->debug(Mongo_ConnStr);
  if (Mongo_ConnStr == "__NULLSTR__") return false;

  Mongo_DbName = get_consul_config_value("Mongo_DbName");
  config_logging->debug("Mongo DB Name:");
  config_logging->debug(Mongo_DbName);
  if (Mongo_DbName == "__NULLSTR__") return false;

  Mongo_DbCollection = get_consul_config_value("Mongo_DbCollection");
  config_logging->debug("Mongo DB Collection:");
  config_logging->debug(Mongo_DbCollection);
  if (Mongo_DbCollection == "__NULLSTR__") return false;

  KafkaBrokerList = get_consul_config_value("KafkaBrokerAddress");
  config_logging->debug("Kafka Broker Address:");
  config_logging->debug(KafkaBrokerList);
  if (KafkaBrokerList == "__NULLSTR__") return false;

  std::string tran_ids_active = get_consul_config_value("StampTransactionId");
  config_logging->debug("Transaction IDs Enabled:");
  config_logging->debug(tran_ids_active);
  if (tran_ids_active == "__NULLSTR__") return false;
  if (tran_ids_active == "True") {
    StampTransactionId = true;
  } else {
    StampTransactionId = false;
  }

  std::string atomic = get_consul_config_value("AtomicTransactions");
  config_logging->debug("Atomic Transactions Enabled:");
  config_logging->debug(atomic);
  if (atomic == "__NULLSTR__") return false;
  if (atomic == "True") {
    AtomicTransactions = true;
  } else {
    AtomicTransactions = false;
  }

  std::string olocking = get_consul_config_value("ObjectLockingActive");
  config_logging->debug("Object Locking Enabled:");
  config_logging->debug(olocking);
  if (olocking == "__NULLSTR__") return false;
  if (olocking == "True") {
    ObjectLockingActive = true;
  } else {
    ObjectLockingActive = false;
  }

  std::string format_type_str = get_consul_config_value("DataFormatType");
  config_logging->debug("Data Format:");
  config_logging->debug(format_type_str);
  if (format_type_str == "__NULLSTR__") return false;
  if (format_type_str == "JSON" || format_type_str == "Json" \
    || format_type_str == "json" || format_type_str == "1") {
    format_type = JSON_FORMAT;
  } else if (format_type_str == "ProtoBuf" || format_type_str == "Protobuf" \
    || format_type_str == "protobuf" || format_type_str == "PROTOBUF" \
    || format_type_str == "0") {
    format_type = PROTO_FORMAT;
  }

  // Read from a set of global config values in consul
  // This value is stored the same way as in a properties file, but is stored in
  // one key and are delimited by the character ';'
  std::string redis_conn_str = get_consul_config_value("RedisConnectionString");
  if (redis_conn_str == "__NULLSTR__") return false;
  char delim(';');
  std::vector<std::string> redis_chains = split(redis_conn_str,  delim);
  std::string var_value;
  config_logging->debug("Redis Connections:");
  config_logging->debug(redis_conn_str);
  for (std::size_t i = 0; i < redis_chains.size(); i++) {
    // Read a string in the format 127.0.0.1--7000----2--5--0
    RedisConnChain chain;

    var_value = redis_chains[i];

    // Retrieve the first value
    int spacer_position = var_value.find("--", 0);
    std::string str1 = var_value.substr(0, spacer_position);
    chain.ip = str1;

    // Retrieve the second value
    std::string new_value = \
      var_value.substr(spacer_position+2, var_value.length() - 1);
    spacer_position = new_value.find("--", 0);
    str1 = new_value.substr(0, spacer_position);
    chain.port = std::stoi(str1);

    // Retrieve the third value
    new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
    spacer_position = new_value.find("--", 0);
    str1 = new_value.substr(0, spacer_position);
    chain.password = str1;

    // Retrieve the fourth value
    new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
    spacer_position = new_value.find("--", 0);
    str1 = new_value.substr(0, spacer_position);
    chain.pool_size = std::stoi(str1);

    // Retrieve the fifth value
    new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
    spacer_position = new_value.find("--", 0);
    str1 = new_value.substr(0, spacer_position);
    chain.timeout = std::stoi(str1);

    // Retrieve the final value
    new_value = new_value.substr(spacer_position+2, new_value.length() - 1);
    spacer_position = new_value.find("--", 0);
    str1 = new_value.substr(0, spacer_position);
    chain.role = std::stoi(str1);

    RedisConnectionList.push_back(chain);
  }

  return true;
}

// ---------------------External Configuration Methods----------------------- //

// The publicly exposed configure function that determines where
// configs need to come from
bool ConfigurationManager::configure() {
  // Null Check
  if (!cli) {
    config_logging->error("null Command Line Interpreter detected");
    return false;
  } else {
    bool configured = false;
    bool ret_val = false;

    // See if we have any environment variables specified
    const char * env_consul_addr = std::getenv("CLYMAN_CONSUL_ADDR");
    const char * env_ip = std::getenv("CLYMAN_IP");
    const char * env_port = std::getenv("CLYMAN_PORT");
    const char * env_db_addr = std::getenv("CLYMAN_DB_ADDR");
    const char * env_mongo_addr = std::getenv("CLYMAN_MONGO_ADDR");
    const char * env_mongo_db = std::getenv("CLYMAN_MONGO_DB");
    const char * env_mongo_col = std::getenv("CLYMAN_MONGO_COL");
    const char * env_conf_file = std::getenv("CLYMAN_CONFIG_FILE");

    // Check if we have a configuration file specified
    if (env_conf_file) {
      std::string env_conf_loc(env_conf_file);
      ret_val = configure_from_file(env_conf_loc);
      configured = true;
    } else if (cli->opt_exist("-config-file")) {
      ret_val =  configure_from_file(cli->get_opt("-config-file"));
      configured = true;
    }

    // String variables to hold the hostname, port, and consul connection info
    std::string env_ip_str = "";
    std::string env_port_str = "";
    std::string env_consul_addr_str = "";
    std::string advertised_host = "";

    // Pull any command line and environment variables for ip, port,
    // and consul address
    if (env_consul_addr) env_consul_addr_str.assign(env_consul_addr);
    if (env_ip) env_ip_str.assign(env_ip);
    if (env_port) env_port_str.assign(env_port);
    if (cli->opt_exist("-ip")) env_ip_str.assign(cli->get_opt("-ip"));
    if (cli->opt_exist("-port")) env_port_str.assign(cli->get_opt("-port"));
    if (cli->opt_exist("-consul-addr")) {
      env_consul_addr_str.assign(cli->get_opt("-consul-addr"));
    }
    if (cli->opt_exist("-advertised-host")) {
      advertised_host.assign(cli->get_opt("-advertised-host"));
    }

    // If we had a hostname and port specified in the configuration file,
    // then we override to that
    if (!(hostname.empty())) env_ip_str.assign(hostname);
    if (!(port.empty())) env_port_str.assign(port);

    // Execute Consul Configuration
    if (!(env_consul_addr_str.empty() || env_ip_str.empty() \
      || env_port_str.empty()))  {
      ret_val = configure_from_consul(env_consul_addr_str, env_ip_str, \
        env_port_str, advertised_host);
      if (ret_val) {
        isConsulActive = true;
        configured = true;
      } else {
        config_logging->error("Configuration from Consul failed");
      }
    } else {
      config_logging->error("Insufficient information to register with Consul");
    }

    // If we have nothing specified, look for an app.properties file
    if (!configured) {
      ret_val = configure_from_file("app.properties");
    }

    // Override DB options with command line/env variables
    if (cli->opt_exist("-db-addr")) {
      DB_ConnStr = cli->get_opt("-db-addr");
    } else if (env_db_addr) {
      std::string env_db_addr_str(env_db_addr);
      DB_ConnStr = env_db_addr_str;
    }

    if (cli->opt_exist("-mongo-addr")) {
      Mongo_ConnStr = cli->get_opt("-mongo-addr");
    } else if (env_mongo_addr) {
      std::string env_mongo_addr_str(env_mongo_addr);
      Mongo_ConnStr = env_mongo_addr_str;
    }

    if (cli->opt_exist("-mongo-db")) {
      Mongo_DbName = cli->get_opt("-mongo-db");
    } else if (env_mongo_db) {
      std::string env_mongo_db_str(env_mongo_db);
      Mongo_DbName = env_mongo_db_str;
    }

    if (cli->opt_exist("-mongo-col")) {
      Mongo_DbCollection = cli->get_opt("-mongo-col");
    } else if (env_mongo_col) {
      std::string env_mongo_col_str(env_mongo_col);
      Mongo_DbCollection = env_mongo_col_str;
    }
    return ret_val;
  }
  return false;
}
