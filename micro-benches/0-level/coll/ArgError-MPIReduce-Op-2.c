#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * MPI_REPLACE is not a valid Reduce operation. line 17
 *
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int local_sum = 4;
  int global_sum = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_REPLACE, 0, MPI_COMM_WORLD);

  if (myRank == 0) {
    printf("Result: %d", global_sum);
  }

  MPI_Finalize();

  return 0;
}
