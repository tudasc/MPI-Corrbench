#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * number elements in Reduce call does not match. (line 18/20)
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int local_sum[2] = {4, 2};
  int global_sum[2];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (myRank == 0) {
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  } else {
    MPI_Reduce(&local_sum, &global_sum, 2, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  }

  if (myRank == 0) {
    printf("Result: %d", global_sum);
  }

  MPI_Finalize();

  return 0;
}
