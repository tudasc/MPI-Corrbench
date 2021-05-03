#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * Negative number of elements to reduce. (line 16)
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int local_sum = 4;
  int global_sum = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  int buff_size = 1;
  if (argc == 1) {
    buff_size = -1;
  } else {
    buff_size = 1;
  }

  MPI_Reduce(&local_sum, &global_sum, buff_size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (myRank == 0) {
    printf("Result: %d", global_sum);
  }

  MPI_Finalize();

  return 0;
}
