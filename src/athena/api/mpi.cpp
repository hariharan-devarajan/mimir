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
    athena::Server::Instance();
    athena::Client::Instance();
  }
  return status;
}

int ATHENA_DECL(MPI_Finalize)(void) {
  athena::Client::Instance()->finalize();
  athena::Server::Instance()->finalize();
  MAP_OR_FAIL(MPI_Finalize);
  int status = real_MPI_Finalize_();
  return status;
}