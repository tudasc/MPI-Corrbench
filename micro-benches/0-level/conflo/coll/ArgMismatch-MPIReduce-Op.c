#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * Operations of Reduce Calls do not match. (line 19/21)
 *
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int local_sum = 16;
  int global_sum = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  MPI_Op r_op = MPI_SUM;
  if (argc == 1) {
    r_op = MPI_MAX;
  } else {
    r_op = MPI_SUM;
  }

  if (myRank == 0) {
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  } else {
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, r_op, 0, MPI_COMM_WORLD);
  }

  if (myRank == 0) {
    printf("Result: %d", global_sum);
  }

  MPI_Finalize();

  return 0;
}
