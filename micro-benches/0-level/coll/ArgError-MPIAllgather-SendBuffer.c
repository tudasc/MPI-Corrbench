#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
/*
 * Null pointer as send buffer in MPI_Allgather. line 18
 *
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int *local_sum = NULL;
  int global_sum[2] = {0};

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  MPI_Allgather(local_sum, 1, MPI_INT, global_sum, 1, MPI_INT, MPI_COMM_WORLD);

  if (myRank == 0) {
    printf("Result: %d", global_sum);
  }

  MPI_Finalize();

  return 0;
}
