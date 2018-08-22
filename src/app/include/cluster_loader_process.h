#include <stdlib.h>
#include <unistd.h>
#include <string>

#include "cluster_manager.h"

#ifndef SRC_APP_INCLUDE_CLUSTER_LOADER_PROCESS_H_
#define SRC_APP_INCLUDE_CLUSTER_LOADER_PROCESS_H_

void update_cluster(ClusterManager *cluster, long interval) {
  while (true) {
    cluster->update_cluster_info();
    usleep(interval);
  }
}

#endif  // SRC_APP_INCLUDE_CLUSTER_LOADER_PROCESS_H_
