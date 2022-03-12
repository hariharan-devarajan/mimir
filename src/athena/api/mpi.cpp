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

int ATHENA_DECL(MPI_Init)(int *argc, char ***argv) {
  MAP_OR_FAIL(MPI_Init);
  int status = real_MPI_Init_(argc, argv);
  if (status == 0) {
    init_mimir();
    set_mpi();
    mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO,
                                           "Intercepting MPI_Init");
    athena::Server::Instance();
    MPI_Barrier(MPI_COMM_WORLD);
  }
  return status;
}

int ATHENA_DECL(MPI_Finalize)(void) {
    MAP_OR_FAIL(MPI_Finalize);
    int status = real_MPI_Finalize_();
  finalize_mimir();
  mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO,
                                         "Intercepting MPI_Finalize");

  return status;
}