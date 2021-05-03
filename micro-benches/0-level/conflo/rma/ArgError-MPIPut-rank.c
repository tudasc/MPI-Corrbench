#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
/*
 * invalid target rank in put call. (line 25)
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
    int local_buf[N] = {0};
    int target_rank = 1;
    if (argc == 1) {
      target_rank = -1;
    } else {
      target_rank = 0;
    }
    MPI_Put(&local_buf, N, MPI_INT, target_rank, 0, N, MPI_INT, win);

    MPI_Win_fence(0, win);
  } else {
    MPI_Win_fence(0, win);
    MPI_Win_fence(0, win);
  }

  MPI_Win_free(&win);

  free(buffer);

  MPI_Finalize();

  return 0;
}
