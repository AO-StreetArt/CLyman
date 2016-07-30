//tests for the configuration manager
//src/test/test.properties

#include "../configuration_manager.h"
#include <assert.h>

int main( int argc, char** argv )
{

  std::string initFileName = "src/test/log4cpp_test.properties";
  try {
  	log4cpp::PropertyConfigurator::configure(initFileName);
  }
  catch ( log4cpp::ConfigureFailure &e ) {
  	printf("[log4cpp::ConfigureFailure] caught while reading Logging Configuration File");
  	printf(e.what());
  	exit(1);
  }

  log4cpp::Category& root = log4cpp::Category::getRoot();

  log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("sub1"));

  log4cpp::Category& log = log4cpp::Category::getInstance(std::string("sub1.log"));

  logging = &log;

  ConfigurationManager cm;

  //Set up the UUID Generator
  uuidAdmin ua;

  //Set up our command line interpreter
  CommandLineInterpreter cli ( argc, argv );

  cm.configure( cli, ua );

  //Basic Tests

  assert ( cm.get_dbconnstr() == "couchbase://localhost/default" );
  assert ( cm.get_dbauthactive() == false );
  assert ( cm.get_dbpswd() == "default" );
  assert ( cm.get_obconnstr() == "tcp://localhost:5556" );
  assert ( cm.get_ibconnstr() == "tcp://*:5555" );
  assert ( cm.get_smartupdatesactive() == true );
  assert ( cm.get_mfjson() == false );
  assert ( cm.get_mfprotobuf() == true );
  assert ( cm.get_rfjson() == true );
  assert ( cm.get_rfprotobuf() == false );
  assert ( cm.get_subduration() == 240 );

  //Redis Connection List Tests
  std::vector<RedisConnChain> RedisConnectionList = cm.get_redisconnlist();
  int conn_list_size = RedisConnectionList.size();

  RedisConnChain redis_chain = RedisConnectionList[0];
  assert( redis_chain.ip == "127.0.0.1" );
  assert( redis_chain.port == 6379 );
  assert( redis_chain.elt4 == "test" );
  assert( redis_chain.elt5 == 2);
  assert( redis_chain.elt6 == 5);
  assert( redis_chain.elt7 == 0);

  RedisConnChain redis_chain2 = RedisConnectionList[1];
  assert( redis_chain2.ip == "127.0.0.1" );
  assert( redis_chain2.port == 6380 );
  assert( redis_chain2.elt4 == "test2" );
  assert( redis_chain2.elt5 == 2);
  assert( redis_chain2.elt6 == 5);
  assert( redis_chain2.elt7 == 0);


  return 0;
}
