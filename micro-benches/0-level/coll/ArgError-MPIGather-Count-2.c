#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * Wrong number of elements to send. line 17
 *
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int local_sum = 4;
  int global_sum[2] = {0};

  MPI_Init(&argc, &argv);

  int root = 0;

  MPI_Gather(&local_sum, 2, MPI_INT, global_sum, 1, MPI_INT, root, MPI_COMM_WORLD);

  MPI_Finalize();

  return 0;
}
