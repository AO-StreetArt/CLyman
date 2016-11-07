#include <zmq.hpp>
#include <sstream>
#include <string>
#include <cstdlib>
#include <stdlib.h>
#include <exception>

#include "aossl/factory/redis_interface.h"

#include "obj3.h"
#include "configuration_manager.h"
#include "lyman_log.h"

#ifndef REDIS_LOCKING
#define REDIS_LOCKING

//Establish a Redis Mutex Lock on a given key
inline void get_lock(std::string key, std::string val) {

  //Determine if another instance of CLyman has a lock on the Redis Mutex
  std::string current_mutex_key;
  bool lock_established = false;

  while (!lock_established) {

    redis_logging->error("Redis Mutex Lock Routine Started");

    if ( xRedis->exists( key ) ) {
      try {
        current_mutex_key = xRedis->load( key );
      }
      catch (std::exception& e) {
        redis_logging->error("Exception encountered during Redis Request");
        redis_logging->error(e.what());
      }
    }

    if ( ((current_mutex_key != "") && (current_mutex_key != val)) || val.empty() )  {
      //Another instance of Clyman has a lock on the redis mutex
      //Block until the lock is cleared
      redis_logging->error("Existing Redis Mutex Lock Detected, waiting for lock to be released");
      while (xRedis->exists( key )) {}
    }

    //Try to establish a lock on the Redis Mutex
    redis_logging->error("Attempting to obtain Redis Mutex Lock");
    if ( !(xRedis->exists( key )) ) {
      try {
        lock_established = xRedis->setnx( key, val);
      }
      catch (std::exception& e) {
        redis_logging->error("Exception encountered during Redis Request");
        redis_logging->error(e.what());
      }
    }
  }
}

void get_lock(std::string key) {get_lock(key, "");}

bool release_lock(std::string key, std::string val) {
  std::string current_mutex_key = "";
  if ( xRedis->exists( key ) ) {
    current_mutex_key = xRedis->load( key );
  }
  if ( val.empty() || (val == current_mutex_key) ) {
    return xRedis->del(key);
  }
  return false;
}

#endif
