//
// Created by hariharan on 2/21/22.
//

#include <athena/api/mpi.h>
#include <athena/client/athena_client.h>
#include <athena/server/athena_server.h>
#include <mpi.h>

#include <cmath>
#include <iostream>
#include "mimir/api/job_configuration.h"
/**
 * MPI
 */

void job_details() {
  using namespace mimir;
  auto SHM = std::getenv("SHM_PATH");
  auto PFS = std::getenv("SHM_PATH");
  MimirHandler job_configuration_handler;
  JobConfigurationAdvice job_conf_advice;
  job_conf_advice._job_id = 0;
  job_conf_advice._devices.emplace_back(SHM, 16);
  job_conf_advice._devices.emplace_back(PFS, 128);
  job_conf_advice._job_time_minutes = 30;
  job_conf_advice._num_cores_per_node = 2;
  job_conf_advice._num_gpus_per_node = 0;
  job_conf_advice._num_nodes = 2;
  job_conf_advice._node_names = {"localhost", "localhost"};
  job_conf_advice._rpc_port = 8888;
  job_conf_advice._rpc_threads = 1;
  job_conf_advice._priority = 100;
  job_configuration_advice_begin(job_conf_advice, job_configuration_handler);
}

int ATHENA_DECL(MPI_Init)(int *argc, char ***argv) {
  MAP_OR_FAIL(MPI_Init);
  int status = real_MPI_Init_(argc, argv);
  if (status == 0) {
    set_mpi();
    job_details();
    mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO,
                                           "Intercepting MPI_Init");
    athena::Server::Instance(true);
    MPI_Barrier(MPI_COMM_WORLD);
    athena::Client::Instance(true);
    MPI_Barrier(MPI_COMM_WORLD);
  }
  return status;
}

int ATHENA_DECL(MPI_Finalize)(void) {
  MPI_Barrier(MPI_COMM_WORLD);
  OnExit();
  mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO,
                                         "Intercepting MPI_Finalize");
  auto client = athena::Client::Instance();
  if (client != nullptr) {
    client->finalize();
    athena::Server::Instance()->finalize();
  } else {
    mimir::Logger::Instance("ATHENA")->log(mimir::LOG_WARN,
                                           "Client not initialized");
  }
  MAP_OR_FAIL(MPI_Finalize);
  int status = real_MPI_Finalize_();
  return status;
}