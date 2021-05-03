#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * MPI_Reduce has a null pointer argument, line 18
 *
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int local_sum = 4;
  int global_sum;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  int *recv_buf = NULL;
  MPI_Reduce(&local_sum, recv_buf, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  MPI_Finalize();

  return 0;
}
