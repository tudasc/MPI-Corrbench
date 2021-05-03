#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * Wrong number of elements to receive in MPI_Allgather (num of elem send and recved do not match ). line 17
 *
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int local_sum = 4;
  int global_sum[2] = {0, 0};

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  MPI_Allgather(&local_sum, 1, MPI_INT, global_sum, 2, MPI_INT, MPI_COMM_WORLD);

  if (myRank == 0) {
    printf("Result: %d", global_sum);
  }

  MPI_Finalize();

  return 0;
}
