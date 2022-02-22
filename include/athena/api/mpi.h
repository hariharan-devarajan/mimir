//
// Created by hariharan on 2/21/22.
//

#ifndef ATHENA_MPI_H
#define ATHENA_MPI_H

#include "interceptor.h"

ATHENA_FORWARD_DECL(MPI_Init, int, (int *argc, char ***argv));
ATHENA_FORWARD_DECL(MPI_Finalize, int, (void));

#endif  // ATHENA_MPI_H
