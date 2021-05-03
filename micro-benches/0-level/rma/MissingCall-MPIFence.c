#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
/*
 * no syncronization of RDMA Operation. (line 24)
 */

#define N 10

int main(int argc, char *argv[]) {
  int rank, numProcs;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int *buffer = malloc(N * sizeof(int));

  MPI_Win win;
  MPI_Win_create(&buffer, N * sizeof(int), 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);

  if (rank == 0) {
    int local_buf[N] = {0};
    MPI_Put(&local_buf, N, MPI_INT, 1, 0, N, MPI_INT, win);

  } else {
  }

  MPI_Win_free(&win);

  free(buffer);

  MPI_Finalize();

  return 0;
}
