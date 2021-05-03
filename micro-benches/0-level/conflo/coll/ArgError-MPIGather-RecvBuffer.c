#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
/*
 * Illegal receive buffer (NULL pointer). line 17
 *
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int local_sum = 4;
  int *global_sum = NULL;

  MPI_Init(&argc, &argv);

  int root = 0;

  if (argc == 1) {
    global_sum = NULL;
  } else {
    global_sum = (int *)malloc(2 * sizeof(int));
  }
  MPI_Gather(&local_sum, 1, MPI_INT, global_sum, 1, MPI_INT, root, MPI_COMM_WORLD);

  free(global_sum);

  MPI_Finalize();

  return 0;
}
