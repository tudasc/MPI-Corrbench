#include <mpi.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * openmp syncronization is done in a way that it leads to a deadlock
 */

#define BUFFER_LENGTH_INT 100
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
    int *buffer = malloc(BUFFER_LENGTH_BYTE);
    int tid = omp_get_thread_num();
    if (myRank == 0) {
      if (tid == 0) {
        MPI_Recv(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, 123, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);  // this recv is not matched before omp barrier
#pragma omp barrier
        MPI_Send(buffer, BUFFER_LENGTH_INT, MPI_INT, 1, 123, MPI_COMM_WORLD);
      } else {
        MPI_Send(buffer, BUFFER_LENGTH_INT, MPI_INT, 1, 123, MPI_COMM_WORLD);
#pragma omp barrier
        MPI_Recv(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }

    } else {  // other MPI rank
      MPI_Recv(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
#pragma omp barrier
      MPI_Send(buffer, BUFFER_LENGTH_INT, MPI_INT, 1, 123, MPI_COMM_WORLD);
    }
    free(buffer);
  }  // end parallel

  MPI_Finalize();

  return 0;
}
