#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
/*
 * invalid buffer (buffer must be allocated)
 *
 */

#define N 10

int main(int argc, char *argv[]) {
  int rank, numProcs;
  int *buffer;
  // missing malloc of buffer

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Win win;

  if (argc == 1) {
    /* Missing malloc */
  } else {
    buffer = (int *)malloc(2 * N * sizeof(int));
  }

  MPI_Win_create(buffer, N * 2 * sizeof(int), 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);

  MPI_Win_free(&win);

  free(buffer);

  MPI_Finalize();

  return 0;
}
