//tests for the configuration manager

#include "configuration_manager.h"

int main()
{
  ConfigurationManager cm;

  cm.configure("test.properties");

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
