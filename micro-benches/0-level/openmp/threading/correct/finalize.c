#include "nondeterminism.h"
#include <mpi.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * master Thread may call finalize if all comm completed, even if other Threads are still around
 * TODO: current ambiguity: what if MPi does not respect the "magic thread barrier"?
 */
int main(int argc, char *argv[]) {
  int myRank;
  int buffer_out[10], buffer_in[10];
  int provided;
  const int requested = MPI_THREAD_MULTIPLE;

  MPI_Init_thread(&argc, &argv, requested, &provided);
  if (provided < requested) {
    has_error_manifested(false);
    exit(EXIT_FAILURE);
  }

  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

#pragma omp parallel
  {
#pragma omp for
    for (int i = 0; i < 10; i++) {
      buffer_out[i] = i * 10;
    }
// implicit OpenMP barrier
#pragma omp sections
    {
#pragma omp section
      {
        if (myRank == 0) {
          MPI_Send(buffer_out, 10, MPI_INT, 1, 123, MPI_COMM_WORLD);
        } else if (myRank == 1) {
          MPI_Recv(buffer_in, 10, MPI_INT, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
      }
#pragma omp section
      if (myRank == 1) {
        MPI_Send(buffer_out, 10, MPI_INT, 0, 123, MPI_COMM_WORLD);
      } else if (myRank == 0) {
        MPI_Recv(buffer_in, 10, MPI_INT, 1, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }
    // implicit OpenMP Barrier
#pragma omp master
    { MPI_Finalize(); }
  }
  // end of omp parallel

  return 0;
}
