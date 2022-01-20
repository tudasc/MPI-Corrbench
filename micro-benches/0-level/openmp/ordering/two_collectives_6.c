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

  DEF_ORDER_CAPTURING_VARIABLES

  const int other_rank = size - myRank - 1;

#pragma omp parallel num_threads(2) reduction(+ : overlap_count)
  {
#pragma omp task
    {
      int *buffer = malloc(BUFFER_LENGTH_BYTE);
      CHECK_OVERLAP_BEGIN
      MPI_Bcast(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD);
      CHECK_OVERLAP_END
      free(buffer);
    }
#pragma omp task
    {
      int *buffer = malloc(BUFFER_LENGTH_BYTE);
      CHECK_OVERLAP_BEGIN
      MPI_Bcast(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD);
      CHECK_OVERLAP_END
      free(buffer);
    }
  }  // end parallel

  has_error_manifested(overlap_count != 0);
  MPI_Finalize();

  return 0;
}
