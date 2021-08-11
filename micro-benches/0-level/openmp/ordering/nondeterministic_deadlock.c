#include "nondeterminism.h"
#include <mpi.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * openmp syncronization is done in a way that it leads to a deadlock
 */

#define BUFFER_LENGTH_INT 1000
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))
int main(int argc, char *argv[]) {
  int myRank;
  int provided;
  const int requested = MPI_THREAD_MULTIPLE;

  MPI_Init_thread(&argc, &argv, requested, &provided);
  if (provided < requested) {
    exit(EXIT_FAILURE);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

#pragma omp parallel
  {
    int tid = omp_get_thread_num();
    if (myRank == 0) {
#pragma omp for
      for (int i = 0; i < 10000; ++i) {
// ordering of tasks is undefined in OpenMP
#pragma omp task
        {
          int *buffer = malloc(BUFFER_LENGTH_BYTE);
          MPI_Recv(buffer, BUFFER_LENGTH_INT, MPI_INT, 1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          free(buffer);
        }
      }
    } else {  // other MPI rank
#pragma omp for
      for (int i = 10000; i > 0; --i) {
// different loop ordering increases the chance that deadlock actually manifests but as
// ordering of tasks is undefined in OpenMP
// this does not make a difference in the programs semantic
#pragma omp task
        {
          int *buffer = malloc(BUFFER_LENGTH_BYTE);
          MPI_Send(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, i, MPI_COMM_WORLD);
          free(buffer);
        }
      }
    }
  }  // end parallel

  // if reaches:
  has_error_manifested(false);
  MPI_Finalize();

  return 0;
}
