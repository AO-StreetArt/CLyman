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

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <exception>
#include <thread>

#include "controller/include/handler_factory.h"

#include "db/include/database_manager.h"

#include "user/include/account_manager_interface.h"
#include "user/include/account_manager.h"

#include "event_stream_process.h"
#include "cluster_loader_process.h"
#include "thread_error_handler.h"
#include "cluster_manager.h"
#include "event_sender.h"

#include "aossl/core/include/buffers.h"
#include "aossl/profile/include/network_app_profile.h"
#include "aossl/consul/include/consul_interface.h"
#include "aossl/consul/include/factory_consul.h"
#include "aossl/vault/include/vault_interface.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#include "Poco/Net/Context.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/SecureServerSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/ConsoleCertificateHandler.h"
#include "Poco/Net/KeyConsoleHandler.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"

#include "Poco/Logger.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/Message.h"
#include "Poco/NumberParser.h"
#include "Poco/AutoPtr.h"
#include "Poco/AsyncChannel.h"
#include "Poco/ErrorHandler.h"

#ifndef SRC_APP_INCLUDE_IVAN_APPLICATION_H_
#define SRC_APP_INCLUDE_IVAN_APPLICATION_H_

// The main Clyman Application

// Extends a Poco ServerApplication and let's it handle
// the heavy lifting.  Internally it will spin up a separate
// thread to handle each request.
class Clyman: public Poco::Util::ServerApplication {
  AccountManagerInterface *acct_manager = NULL;
  EventStreamPublisher *publisher = nullptr;
  AOSSL::ServiceInterface *my_app = NULL;
  AOSSL::ServiceInterface *my_app_udp = NULL;
  ClusterManager *cluster = nullptr;
public:
 Clyman() {}
 ~Clyman() {}
protected:

  // Initialize the core components of the application
  void initialize(Poco::Util::Application& self) {
    // Have Poco load the configuration file to provide SSL configuration
    Poco::Util::ServerApplication::initialize(self);
  }

  // App Shutdown Routine
  void uninitialize() {
    Poco::Util::ServerApplication::uninitialize();
    if (acct_manager) delete acct_manager;
    if (publisher) delete publisher;
    if (cluster) delete cluster;
  }

  // Define basic CLI Opts
  void defineOptions(Poco::Util::OptionSet& options) {
    Poco::Util::ServerApplication::defineOptions(options);
  }

  // Main application method
  int main(const std::vector<std::string>& args) {
    Poco::Util::Application& app = Poco::Util::Application::instance();
    app.logger().information("Starting Clyman");
    is_app_running = true;
    for (std::string elt : args) {
      app.logger().information(elt);
    }
    // Initialize the application profile
    AOSSL::NetworkApplicationProfile config(args, std::string("clyman"), std::string("prod"));
    // Print the configuration log
    app.logger().information("Profile Startup Log");
    std::vector<std::string> profile_startup_log = config.get_config_record();
    for (auto& log_line: profile_startup_log) {
      app.logger().information(log_line);
    }
    // Add secure opts
    std::vector<std::string> secure_ops;
    secure_ops.push_back(config.get_cluster_name() + \
        std::string(".transaction.security.auth.user"));
    secure_ops.push_back(config.get_cluster_name() + \
        std::string(".transaction.security.auth.password"));
    secure_ops.push_back(config.get_cluster_name() + \
        std::string(".transaction.security.hash.password"));
    secure_ops.push_back(config.get_cluster_name() + \
        std::string(".event.security.out.aes.key"));
    secure_ops.push_back(config.get_cluster_name() + \
        std::string(".event.security.out.aes.salt"));
    secure_ops.push_back(config.get_cluster_name() + \
        std::string(".event.security.in.aes.key"));
    secure_ops.push_back(config.get_cluster_name() + \
        std::string(".event.security.in.aes.salt"));
    secure_ops.push_back(std::string("mongo.auth.un"));
    secure_ops.push_back(std::string("mongo.auth.pw"));
    for (std::string op: secure_ops) {
      config.add_secure_opt(op);
    }
    // Set default values for configuration
    config.add_opt(std::string("mongo"), std::string(""));
    config.add_opt(std::string("mongo.db"), std::string("clyman"));
    config.add_opt(std::string("mongo.obj.collection"), std::string("obj3"));
    config.add_opt(std::string("mongo.prop.collection"), std::string("property"));
    config.add_opt(std::string("mongo.ssl.pem.file"), std::string(""));
    config.add_opt(std::string("mongo.ssl.pem.password"), std::string(""));
    config.add_opt(std::string("mongo.ssl.crl.file"), std::string(""));
    config.add_opt(std::string("mongo.ssl.ca.file"), std::string(""));
    config.add_opt(std::string("mongo.ssl.ca.dir"), std::string(""));
    config.add_opt(std::string("mongo.ssl.active"), std::string(""));
    config.add_opt(std::string("mongo.ssl.validate_server_cert"), std::string("true"));
    config.add_opt(std::string("transaction.format"), std::string("json"));
    config.add_opt(std::string("transaction.id.stamp"), std::string("true"));
    config.add_opt(std::string("event.stream.method"), std::string("kafka"));
    config.add_opt(std::string("event.format"), std::string("json"));
    config.add_opt(std::string("event.destination.host"), std::string("localhost"));
    config.add_opt(std::string("event.destination.port"), std::string("8764"));
    config.add_opt(std::string("http.host"), std::string("localhost"));
    config.add_opt(std::string("http.port"), std::string("8768"));
    config.add_opt(std::string("udp.port"), std::string("8762"));
    config.add_opt(std::string("log.file"), std::string("clyman.log"));
    config.add_opt(std::string("log.level"), std::string("Info"));
    config.add_opt(std::string("transaction.security.ssl.ca.vault.active"), std::string("false"));
    config.add_opt(std::string("transaction.security.ssl.ca.vault.role_name"), std::string("test"));
    config.add_opt(std::string("transaction.security.ssl.ca.vault.common_name"), std::string("test"));
    config.add_opt(std::string("transaction.security.ssl.enabled"), std::string("false"));
    config.add_opt(std::string("transaction.security.auth.type"), std::string("none"));
    config.add_opt(std::string("event.security.aes.enabled"), std::string("false"));
    // Perform the initial config
    bool config_success = false;
    bool config_tried = false;
    int config_attempts = 0;
    while (!config_success) {
      if (config_attempts > 50) {
        app.logger().error("Max Config Attempts failed, exiting");
        return Poco::Util::Application::EXIT_NOINPUT;
      }
      if (config_tried) {
        app.logger().error("Configuration Failed, trying again in 5 seconds");
        usleep(5000000);
      } else {
        config_tried = true;
      }
      try {
        config.load_config();
        config_success = true;
      }
      catch (std::exception& e) {
        app.logger().error("Exception encountered during Configuration");
        config_attempts++;
      }
    }

    // Set up Poco Logging framework
    // Set up a console channel, and an async channel so that
    // log lines are written on a background thread
    Poco::AutoPtr<Poco::ConsoleChannel> pCons(new Poco::ConsoleChannel);
    Poco::AutoPtr<Poco::AsyncChannel> pAsync(new Poco::AsyncChannel(pCons));
    Poco::FormattingChannel* console_channel = \
      new Poco::FormattingChannel(new Poco::PatternFormatter("%Y-%m-%d %H:%M:%S.%c %N[%P]:%s:%q:%t"));
    console_channel->setChannel(pAsync);
    console_channel->open();

    // Start up the app loggers
    AOSSL::StringBuffer log_level;
    config.get_opt(std::string("log.level"), log_level);
    int log_priority;
    if (log_level.val == "Debug") {
      log_priority = Poco::Message::PRIO_DEBUG;
    } else if (log_level.val == "Warn") {
      log_priority = Poco::Message::PRIO_WARNING;
    } else if (log_level.val == "Error") {
      log_priority = Poco::Message::PRIO_ERROR;
    } else {
      app.logger().error("Unable to read log level: %s, defaulting to info", log_level.val);
      log_priority = Poco::Message::PRIO_INFORMATION;
    }
    Poco::Logger& main_logger = Poco::Logger::create("MainLogger", \
      console_channel, log_priority);
    Poco::Logger& process_logger = Poco::Logger::create("MessageProcessor", \
      console_channel, log_priority);
    Poco::Logger& controller_logger = Poco::Logger::create("Controller", \
      console_channel, log_priority);
    Poco::Logger& data_logger = Poco::Logger::create("Data", \
      console_channel, log_priority);
    Poco::Logger& auth_logger = Poco::Logger::create("Auth", \
      console_channel, log_priority);
    Poco::Logger& event_logger = Poco::Logger::create("Event", \
      console_channel, log_priority);
    Poco::Logger& database_manager_logger = Poco::Logger::create("DatabaseManager", \
      console_channel, log_priority);
    data_logger.information("Data Logger Initialized");
    controller_logger.information("Controller Logger Initialized");
    process_logger.information("Processor Logger Initialized");
    auth_logger.information("Authorization Logger Initialized");
    event_logger.information("Event Stream Logger Initialized");
    database_manager_logger.information("Database Manager Logger Initialized");
    main_logger.information("Logging Configuration complete");

    main_logger.information("Profile Configuration Log");
    std::vector<std::string> profile_config_log = config.get_config_record();
    for (auto& log_line: profile_config_log) {
      main_logger.information(log_line);
    }

    // Register the service with Consul
    AOSSL::StringBuffer http_host;
    config.get_opt(std::string("http.host"), http_host);
    AOSSL::StringBuffer http_port;
    config.get_opt(std::string("http.port"), http_port);
    AOSSL::StringBuffer udp_port;
    config.get_opt(std::string("udp.port"), udp_port);
    if (config.get_consul()) {
      main_logger.information("Registering with Consul");
      std::vector<std::string> tags;
      tags.push_back(std::string("cluster=") + config.get_cluster_name());
      AOSSL::ConsulComponentFactory consul_factory;
      // Create the actual Service to register
      my_app = \
        consul_factory.get_service_interface(std::string("Clyman"), \
        std::string("Clyman"), http_host.val, http_port.val, tags);
      my_app_udp = \
        consul_factory.get_service_interface(std::string("Clyman_Udp"), \
        std::string("Clyman_Udp"), http_host.val, udp_port.val, tags);
      if (!config.get_consul()->register_service(*my_app)) {
        main_logger.error("Consul Registration Failed");
      }
      if (!config.get_consul()->register_service(*my_app_udp)) {
        main_logger.error("Consul UDP Registration Failed");
      }
    }

    // Get the Mongo Connection information
    AOSSL::StringBuffer initial_db_conn;
    config.get_opt(std::string("mongo"), initial_db_conn);
    AOSSL::StringBuffer database_name;
    config.get_opt(std::string("mongo.db"), database_name);
    AOSSL::StringBuffer db_obj_collection;
    config.get_opt(std::string("mongo.obj.collection"), db_obj_collection);
    AOSSL::StringBuffer db_prop_collection;
    config.get_opt(std::string("mongo.prop.collection"), db_prop_collection);
    AOSSL::StringBuffer db_ssl_active;
    config.get_opt(std::string("mongo.ssl.active"), db_ssl_active);
    AOSSL::StringBuffer db_ssl_valserver_cert;
    config.get_opt(std::string("mongo.ssl.validate_server_cert"), db_ssl_valserver_cert);
    AOSSL::StringBuffer db_ssl_pem_file;
    config.get_opt(std::string("mongo.ssl.pem.file"), db_ssl_pem_file);
    AOSSL::StringBuffer db_ssl_pem_password;
    config.get_opt(std::string("mongo.ssl.pem.password"), db_ssl_pem_password);
    AOSSL::StringBuffer db_ssl_crl_file;
    config.get_opt(std::string("mongo.ssl.crl.file"), db_ssl_crl_file);
    AOSSL::StringBuffer db_ssl_ca_file;
    config.get_opt(std::string("mongo.ssl.ca.file"), db_ssl_ca_file);
    AOSSL::StringBuffer db_ssl_ca_dir;
    config.get_opt(std::string("mongo.ssl.ca.dir"), db_ssl_ca_dir);

    bool m_ssl_active = false;
    bool m_validate_server_certificates = false;
    if (db_ssl_active.val == "true") m_ssl_active = true;
    if (db_ssl_valserver_cert.val == "true") m_validate_server_certificates = true;
    // Start the DB Manager
    DatabaseManager db_manager(&config, initial_db_conn.val, database_name.val, \
        db_obj_collection.val, db_prop_collection.val, m_ssl_active, m_validate_server_certificates, \
        db_ssl_pem_file.val, db_ssl_pem_password.val, db_ssl_ca_file.val, \
        db_ssl_ca_dir.val, db_ssl_crl_file.val);

    // Start the User Account Manager
    AOSSL::StringBuffer auth_type_buffer;
    AOSSL::StringBuffer auth_un_buffer;
    AOSSL::StringBuffer auth_pw_buffer;
    AOSSL::StringBuffer hash_pw_buffer;
    config.get_opt(std::string("transaction.security.auth.type"), auth_type_buffer);
    if (auth_type_buffer.val == "single") {
      config.get_opt(config.get_cluster_name() + \
          std::string(".transaction.security.auth.user"), auth_un_buffer);
      config.get_opt(config.get_cluster_name() + \
          std::string(".transaction.security.auth.password"), auth_pw_buffer);
      config.get_opt(config.get_cluster_name() + \
          std::string(".transaction.security.hash.password"), hash_pw_buffer);
      acct_manager = new SingleAccountManager(auth_un_buffer.val, auth_pw_buffer.val, hash_pw_buffer.val);
    } else {
      acct_manager = NULL;
    }

    // Start the Publisher to send events
    AOSSL::StringBuffer aes_active_buffer;
    AOSSL::StringBuffer aesout_key_buffer;
    AOSSL::StringBuffer aesout_salt_buffer;
    config.get_opt(config.get_cluster_name() + \
        std::string(".event.security.aes.enabled"), aes_active_buffer);
    config.get_opt(config.get_cluster_name() + \
        std::string(".event.security.out.aes.key"), aesout_key_buffer);
    config.get_opt(config.get_cluster_name() + \
        std::string(".event.security.out.aes.salt"), aesout_salt_buffer);
    if ((aes_active_buffer.val == "true") && !(aesout_key_buffer.val.empty()) \
        && !(aesout_salt_buffer.val.empty())) {
      publisher = new EventStreamPublisher(aesout_key_buffer.val, aesout_salt_buffer.val);
    } else {
      publisher = new EventStreamPublisher;
    }

    // Get the Event Destination Config
    AOSSL::StringBuffer ed_name_buf;
    AOSSL::StringBuffer ed_host_buf;
    AOSSL::StringBuffer ed_port_buf;
    config.get_opt(std::string("event.destination.name"), ed_name_buf);
    config.get_opt(std::string("event.destination.host"), ed_host_buf);
    config.get_opt(std::string("event.destination.port"), ed_port_buf);
    // Start the Cluster Manager
    if (ed_host_buf.val.empty() && ed_port_buf.val.empty()) {
      cluster = new ClusterManager(&config, ed_name_buf.val);
    } else {
      cluster = new ClusterManager(ed_host_buf.val, ed_port_buf.val);
    }
    cluster->update_cluster_info();

    // Start the background thread error handler
    ClymanErrorHandler eh;
    Poco::ErrorHandler* pOldEH = Poco::ErrorHandler::set(&eh);

    // Kick off the Cluster Update background thread
    std::thread cluster_thread(update_cluster, &config, cluster, 30000000);
    cluster_thread.detach();

    // Kick off the Event Stream background thread
    std::thread es_thread(event_stream, &config, &db_manager, publisher, cluster);
    es_thread.detach();

    AOSSL::StringBuffer ssl_enabled_buf;
    config.get_opt(std::string("transaction.security.ssl.enabled"), ssl_enabled_buf);

    // Look to see if we should be generating our SSL Certs from Vault
    AOSSL::StringBuffer use_vault_ca_buf;
    AOSSL::StringBuffer vault_role_buf;
    AOSSL::StringBuffer vault_common_name_buf;
    config.get_opt(std::string("transaction.security.ssl.ca.vault.active"), use_vault_ca_buf);
    config.get_opt(std::string("transaction.security.ssl.ca.vault.role_name"), vault_role_buf);
    config.get_opt(std::string("transaction.security.ssl.ca.vault.common_name"), vault_common_name_buf);
    if (use_vault_ca_buf.val == "true") {
      // Generate a new SSL Cert from Vault
      AOSSL::SslCertificateBuffer ssl_cert_buf;
      config.get_vault()->gen_ssl_cert(vault_role_buf.val, vault_common_name_buf.val, ssl_cert_buf);
      if (ssl_cert_buf.success) {
        // We need to write the certificate, private key, and CA to files
        std::ofstream out_key("private.key");
        out_key << ssl_cert_buf.private_key;
        out_key.close();
        std::ofstream out_cert("cert.pem");
        out_cert << ssl_cert_buf.certificate;
        out_cert.close();
        std::ofstream out_ca("rootcert.pem");
        out_ca << ssl_cert_buf.issuing_ca;
        out_ca.close();
        bool use_chain_certs = false;
        std::string chain_cert_path = "cachain.pem";
        if (ssl_cert_buf.ca_chain.empty()) {
          main_logger.debug("Empty CA Chain detected, ignoring");
        } else {
          use_chain_certs = true;
        }
        std::ofstream out_chain(chain_cert_path);
        out_chain << ssl_cert_buf.ca_chain;
        out_chain.close();
        // Initialize the SSL Manager with those files
        Poco::SharedPtr<Poco::Net::PrivateKeyPassphraseHandler> pConsoleHandler = new Poco::Net::KeyConsoleHandler(true);
        Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> pInvalidCertHandler = new Poco::Net::ConsoleCertificateHandler(true);
        Poco::Net::Context::Ptr pContext = new Poco::Net::Context(Poco::Net::Context::SERVER_USE, "private.key", "cert.pem", "rootcert.pem", Poco::Net::Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
        if (use_chain_certs) {
          Poco::Net::X509Certificate chain_cert(chain_cert_path);
          main_logger.debug("Adding Chain Certificate to SSL Context");
          pContext->addChainCertificate(chain_cert);
        }
        Poco::Net::SSLManager::instance().initializeServer(pConsoleHandler, pInvalidCertHandler, pContext);
      }
    } else {
      if (ssl_enabled_buf.val == "true") {
        // If we aren't generating ssl certs from Consul and we still need
        // to start an https endpoint, then look for an ssl.properties file
        loadConfiguration("ssl.properties");
      }
    }

    // Main Application Loop (Serving HTTP API)
    std::string http_address = http_host.val + std::string(":") + http_port.val;
    Poco::Net::SocketAddress saddr(http_address);
    main_logger.debug("HTTP Address: %s, Security Enabled: %s", http_address, ssl_enabled_buf.val);
    if (ssl_enabled_buf.val == "true") {
      main_logger.information("Opening Secure HTTP Socket");
      Poco::Net::SecureServerSocket svs(saddr, 64);
      Poco::Net::HTTPServer srv(new ObjectHandlerFactory(&config, acct_manager, &db_manager, publisher, cluster), svs, \
        new Poco::Net::HTTPServerParams);
      srv.start();
      waitForTerminationRequest();
      srv.stop();
    } else {
      main_logger.information("Opening HTTP Socket");
      Poco::Net::ServerSocket svs(saddr);
      Poco::Net::HTTPServer srv(new ObjectHandlerFactory(&config, acct_manager, &db_manager, publisher, cluster), svs, \
        new Poco::Net::HTTPServerParams);
      srv.start();
      waitForTerminationRequest();
      srv.stop();
    }
    // Handle graceful shutdown of background threads
    main_logger.information("Shutting down application");
    is_app_running = false;
    if (my_app) {
      config.get_consul()->deregister_service(*my_app);
      delete my_app;
    }
    if (my_app_udp) {
      config.get_consul()->deregister_service(*my_app_udp);
      delete my_app_udp;
    }
    while(is_sender_running.load()) {usleep(1000000);}
    Poco::ErrorHandler::set(pOldEH);
    return Poco::Util::Application::EXIT_OK;
  }
};

#endif  // SRC_APP_INCLUDE_IVAN_APPLICATION_H_
