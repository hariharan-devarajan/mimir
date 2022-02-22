//
// Created by hariharan on 2/21/22.
//

#include <athena/api/mpi.h>
#include <athena/client/athena_client.h>
#include <athena/server/athena_server.h>
#include <mpi.h>

#include <cmath>
#include <iostream>

/**
 * MPI
 */
int ATHENA_DECL(MPI_Init)(int *argc, char ***argv) {
  MAP_OR_FAIL(MPI_Init);
  int status = real_MPI_Init_(argc, argv);
  if (status == 0) {
    mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO,
                                           "Intercepting MPI_Init");
    auto server = athena::Server::Instance(true);
    MPI_Barrier(MPI_COMM_WORLD);
    auto client = athena::Client::Instance(true);
  }
  return status;
}

int ATHENA_DECL(MPI_Finalize)(void) {
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