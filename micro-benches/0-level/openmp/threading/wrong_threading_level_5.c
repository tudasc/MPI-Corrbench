#include "nondeterminism.h"
#include <mpi.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * This program needs at least MPI_THREAD_SERIALIZED
 */
int main(int argc, char *argv[]) {
  int myRank;
  int buffer_out[10], buffer_in[10];
  int provided;
  const int requested = MPI_THREAD_FUNNELED;

  MPI_Init_thread(&argc, &argv, requested, &provided);
  if (provided < requested) {
    has_error_manifested(false);
    exit(EXIT_FAILURE);  // no MPI error if program exit here
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

#pragma omp parallel num_threads(NUM_THREADS)
  {
#pragma omp for
    for (int i = 0; i < 10; i++) {
      buffer_out[i] = i * 10;
    }
// implicit OpenMP barrier
#pragma omp single
    {
      if (myRank == 0) {
        MPI_Send(buffer_out, 10, MPI_INT, 1, 123, MPI_COMM_WORLD);
      } else if (myRank == 1) {
        MPI_Recv(buffer_in, 10, MPI_INT, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }
  }

  if (myRank == 0) {
    MPI_Send(buffer_out, 10, MPI_INT, 1, 123, MPI_COMM_WORLD);
  } else if (myRank == 1) {
    MPI_Recv(buffer_in, 10, MPI_INT, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  has_error_manifested(NUM_THREADS > 1);
  MPI_Finalize();

  return 0;
}
