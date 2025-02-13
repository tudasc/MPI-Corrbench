#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * Reduce call data types do not match. (line 20)
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int local_sum[5] = {4, 4, 4, 4, 4};
  int global_sum[5] = {0};

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (myRank == 0) {
    MPI_Reduce(local_sum, global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  } else {
    MPI_Reduce(local_sum, global_sum, 5, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  }

  if (myRank == 0) {
    printf("Result: %d", global_sum[0]);
  }

  MPI_Finalize();

  return 0;
}
