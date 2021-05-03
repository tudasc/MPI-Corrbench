#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
/*
 * RDMA access outside the target process' window. line 25
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
    MPI_Win_fence(0, win);
    MPI_Aint acc_start = 0;
    int local_buf[N] = {0};
    if (argc == 1) {
      acc_start = 5;
    } else {
      acc_start = 0;
    }
    MPI_Get(&local_buf, N, MPI_INT, 1, acc_start, N, MPI_INT, win);
    // acces start at position 5 so it will be out of the tgt win

    MPI_Win_fence(MPI_MODE_NOPRECEDE, win);
  } else {
    MPI_Win_fence(0, win);
    MPI_Win_fence(0, win);
  }

  MPI_Win_free(&win);

  free(buffer);

  MPI_Finalize();

  return 0;
}
