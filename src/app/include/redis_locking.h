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
#include <cstdlib>
#include <exception>
#include <sstream>
#include <string>

#include "aossl/redis/include/redis_interface.h"
#include "app_log.h"

#ifndef SRC_APP_INCLUDE_REDIS_LOCKING_H_
#define SRC_APP_INCLUDE_REDIS_LOCKING_H_

const int MAX_LOCK_TRIALS = 10000;

class RedisLocker {
RedisInterface *redis = NULL;

 public:
  RedisLocker(RedisInterface *r) {redis = r;}
  ~RedisLocker() {}

// Establish a Redis Mutex Lock on a given key
inline bool get_lock(std::string key, std::string val, bool exit_on_existing_lock) {
  // Determine if another instance of CLyman has a lock on the Redis Mutex
  std::string current_mutex_key;
  bool lock_established = false;
  bool key_exists = false;
  int num_trials = 0;
  redis_logging->debug("Redis Mutex Lock Routine Starting");

  // Try to obtain the lock until we actually do
  while (!lock_established) {
    // Exit if we go over the maximum number of trials for establishing a lock
    // Safeguard to prevent infinite looping
    if (num_trials > MAX_LOCK_TRIALS) return false;

    // Check for an existing lock, pulling the value if we need to
    if (redis->exists(key)) {
      key_exists = true;
      // We only need to pull the value associated to the redis key if we have
      // something to compare it to
      if (!exit_on_existing_lock){
        try {
          current_mutex_key = redis->load(key);
        }
        catch (std::exception& e) {
          redis_logging->error("Exception encountered during Redis Request");
          redis_logging->error(e.what());
        }
      }
    }

    // We have an existing lock that isn't ours,
    // and we want to exit
    if (key_exists && (current_mutex_key != val)) {
      redis_logging->error("Existing Redis Mutex Lock Detected, Exiting");
      return false;
    }
    // We have an existing lock that isn't ours,
    // and we want to keep trying to obtain it until we do.
    else if (key_exists && (!exit_on_existing_lock)) {
      redis_logging->error("Existing Redis Mutex Lock Detected, Waiting");
      while (redis->exists(key)) {}
    }

    // Try to establish a lock on the Redis Mutex
    redis_logging->error("Attempting to obtain Redis Mutex Lock");
    try {
      return redis->setnx(key, val);
    }
    catch (std::exception& e) {
      redis_logging->error("Exception encountered during Redis Request");
      redis_logging->error(e.what());
      return false;
    }
    num_trials++;
  }
  return false;
}

bool get_lock(std::string key, std::string val) \
  {return get_lock(key, val, true);}

bool get_lock(std::string key) {return get_lock(key, "", false);}

bool release_lock(std::string key, std::string val) {
  std::string current_mutex_key = "";
  if (redis->exists(key)) {
    current_mutex_key = redis->load(key);
  }
  if (val.empty() || (val == current_mutex_key)) {
    return redis->del(key);
  }
  return false;
}

bool release_lock(std::string key) {
  if (redis->exists(key)) {
    return redis->del(key);
  }
  return false;
}
};

#endif  // SRC_APP_INCLUDE_REDIS_LOCKING_H_
