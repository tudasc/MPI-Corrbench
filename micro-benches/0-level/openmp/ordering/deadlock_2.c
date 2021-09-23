#include "nondeterminism.h"
#include <mpi.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * the rank with parallel openmp sends to many messages
 */

#define BUFFER_LENGTH_INT 100
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))
int main(int argc, char *argv[]) {
  int myRank;
  int provided;
  const int requested = MPI_THREAD_MULTIPLE;

  MPI_Init_thread(&argc, &argv, requested, &provided);
  if (provided < requested) {
    has_error_manifested(false);
    exit(EXIT_FAILURE);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (myRank == 0) {
#pragma omp parallel
    {
      int *buffer = malloc(BUFFER_LENGTH_BYTE);
      MPI_Send(buffer, BUFFER_LENGTH_INT, MPI_INT, 1, 123, MPI_COMM_WORLD);
      MPI_Recv(buffer, BUFFER_LENGTH_INT, MPI_INT, 1, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      free(buffer);
    }  // end parallel
  }

  else {  // other MPI rank
    int *buffer = malloc(BUFFER_LENGTH_BYTE);
    MPI_Recv(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, 123, MPI_COMM_WORLD);
    free(buffer);
  }

  MPI_Finalize();

  return 0;
}
