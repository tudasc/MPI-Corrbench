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

  MPI_Op op = MPI_SUM;
  if (argc == 1) {
    op = NULL;
  } else {
    op = MPI_SUM;
  }
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, op, 0, MPI_COMM_WORLD);

  if (myRank == 0) {
    printf("Result: %d", global_sum);
  }

  MPI_Finalize();

  return 0;
}
