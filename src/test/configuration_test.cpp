//tests for the configuration manager
//src/test/test.properties

#include "../configuration_manager.h"
#include <assert.h>
#include "aossl/factory/logging_interface.h"
#include "aossl/factory/commandline_interface.h"
#include "aossl/factory/uuid_interface.h"
#include "aossl/factory_cli.h"
#include "aossl/factory_logging.h"
#include "aossl/factory_uuid.h"

int main( int argc, char** argv )
{

  LoggingComponentFactory *logging_factory = new LoggingComponentFactory;
  CommandLineInterpreterFactory *cli_factory = new CommandLineInterpreterFactory;
  uuidComponentFactory *id_factory = new uuidComponentFactory;

  //-------------------------------Logging--------------------------------------//
  //----------------------------------------------------------------------------//

  std::string initFileName = "log4cpp.properties";
  logging = logging_factory->get_logging_interface(initFileName);

  logging->debug("PreTest Setup");

  //Set up the UUID Generator
  uuidInterface *ua = id_factory->get_uuid_interface();

  //Set up our command line interpreter
  CommandLineInterface *cli = cli_factory->get_command_line_interface( argc, argv );

  ConfigurationManager cm( cli, ua, factory );

  logging->debug("Configure the app");

  cm.configure();

  logging->debug("Checking Variable Retrieval");

  //Basic Tests

  assert ( cm.get_dbconnstr() == "couchbase://localhost/default" );
  assert ( cm.get_dbauthactive() == false );
  assert ( cm.get_dbpswd() == "default" );
  assert ( cm.get_obconnstr() == "tcp://localhost:5556" );
  assert ( cm.get_ibconnstr() == "tcp://*:5555" );
  assert ( cm.get_smartupdatesactive() == true );
  assert ( cm.get_mfjson() == false );
  assert ( cm.get_mfprotobuf() == true );
  assert ( cm.get_rfjson() == false );
  assert ( cm.get_rfprotobuf() == true );
  assert ( cm.get_subduration() == 240 );
  logging->debug("Values checked");

  //Redis Connection List Tests
  std::vector<RedisConnChain> RedisConnectionList = cm.get_redisconnlist();
  int conn_list_size = RedisConnectionList.size();

  if (conn_list_size > 0) {
    RedisConnChain redis_chain = RedisConnectionList[0];
    assert( redis_chain.ip == "127.0.0.1" );
    assert( redis_chain.port == 6379 );
    assert( redis_chain.password == "test" );
    assert( redis_chain.pool_size == 2);
    assert( redis_chain.timeout == 5);
    assert( redis_chain.role == 0);
    logging->debug("Redis connection list 1 checked");
  }

  if (conn_list_size > 1) {
    RedisConnChain redis_chain2 = RedisConnectionList[1];
    assert( redis_chain2.ip == "127.0.0.1" );
    assert( redis_chain2.port == 6380 );
    assert( redis_chain2.password == "test2" );
    assert( redis_chain2.pool_size == 2);
    assert( redis_chain2.timeout == 5);
    assert( redis_chain2.role == 0);
    logging->debug("Redis connection list 2 checked");
  }

  delete cli;
  delete ua;
  delete logging;
  return 0;
}
