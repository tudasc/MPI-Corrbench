#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
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

  if (argc == 1) {
    local_sum = NULL;
  } else {
    local_sum = (int *)malloc(sizeof(int));
  }
  MPI_Gather(local_sum, 1, MPI_INT, &global_sum, 1, MPI_INT, root, MPI_COMM_WORLD);
  free(local_sum);
  MPI_Finalize();

  return 0;
}
