#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * Illegal send buffer (NULL pointer). line 17
 *
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int *local_sum = NULL;
  int global_sum[2] = {0};

  MPI_Init(&argc, &argv);

  int root = 0;

  MPI_Gather(local_sum, 1, MPI_INT, global_sum, 1, MPI_INT, root, MPI_COMM_WORLD);

  MPI_Finalize();

  return 0;
}
