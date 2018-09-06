#include <stdlib.h>
#include <unistd.h>
#include <string>

#include "cluster_manager.h"
#include "aossl/profile/include/tiered_app_profile.h"

#ifndef SRC_APP_INCLUDE_CLUSTER_LOADER_PROCESS_H_
#define SRC_APP_INCLUDE_CLUSTER_LOADER_PROCESS_H_

void update_cluster(AOSSL::TieredApplicationProfile *config, ClusterManager *cluster, long interval) {
  while (true) {
    // Pull latest cluster information down from Consul
    cluster->update_cluster_info();
    // Pull latest config information down from Consul
    config->load_config();
    // Go to sleep until we need to get updates again
    usleep(interval);
  }
}

#endif  // SRC_APP_INCLUDE_CLUSTER_LOADER_PROCESS_H_
