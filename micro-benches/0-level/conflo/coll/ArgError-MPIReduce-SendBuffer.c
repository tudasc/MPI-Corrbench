#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * MPI_Reduce has a null pointer argument, line 19
 *
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int local_sum = 4;
  int global_sum;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  int *send_b = &local_sum;
  if (argc == 1) {
    send_b = NULL;
  } else {
    send_b = &local_sum;
  }
  MPI_Reduce(send_b, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (myRank == 0) {
    printf("Result: %d", global_sum);
  }

  MPI_Finalize();

  return 0;
}
