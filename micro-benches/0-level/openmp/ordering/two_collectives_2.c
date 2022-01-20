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

  if (myRank == 0) {
#pragma omp parallelnum_threads(NUM_THREADS)
    {
      CHECK_OVERLAP_BEGIN
      MPI_Barrier(MPI_COMM_WORLD);
      CHECK_OVERLAP_END
    }  // end parallel
  }

  else {        // other MPI rank
    usleep(5);  // "force" other Process to call the barrier two times at the same time
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);  // prevent deadlock
  }

  has_error_manifested(overlap_count != 0);
  MPI_Finalize();

  return 0;
}
