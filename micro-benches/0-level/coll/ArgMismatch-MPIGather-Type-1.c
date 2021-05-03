#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * Host language buffer types and MPI types don't agree. line 17
 *
 */
int main(int argc, char *argv[]) {
  int rank, numProcs;

  int local_sum = 4;
  int global_sum[2] = {0};

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int root = 0;

  if (rank % 2 == 0) {
    MPI_Gather(&local_sum, 1, MPI_INT, global_sum, 1, MPI_INT, root, MPI_COMM_WORLD);
  } else {
    MPI_Gather(&local_sum, 1, MPI_CHAR, global_sum, 1, MPI_CHAR, root, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}
