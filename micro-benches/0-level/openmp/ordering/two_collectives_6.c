#include "nondeterminism.h"
#include <mpi.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * the user had to make shure only one collective initialization is active at a time per process and communicator
 * according to the standard (see p303)
 */

#define BUFFER_LENGTH_INT 100
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))
int main(int argc, char *argv[]) {
  int myRank, size;
  int provided;
  const int requested = MPI_THREAD_MULTIPLE;

  MPI_Init_thread(&argc, &argv, requested, &provided);
  if (provided < requested) {
    has_error_manifested(false);
    exit(EXIT_FAILURE);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const int other_rank = size - myRank - 1;

#pragma omp parallel num_threads(2)
  {
#pragma omp task
    {
      int *buffer = malloc(BUFFER_LENGTH_BYTE);
      MPI_Bcast(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD);
      free(buffer);
    }
#pragma omp task
    {
      int *buffer = malloc(BUFFER_LENGTH_BYTE);
      MPI_Bcast(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD);
      free(buffer);
    }
  }  // end parallel

  MPI_Finalize();

  return 0;
}
