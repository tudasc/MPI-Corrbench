#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * MPI_Reduce has an invalid operation. (line 23)
 *
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  float local_f = 3.41f;
  float global_sum;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  MPI_Op op = MPI_SUM;
  if (argc == 1) {
    op = MPI_LXOR;
  } else {
    op = MPI_SUM;
  }
  MPI_Reduce(&local_f, &global_sum, 1, MPI_FLOAT, op, 0, MPI_COMM_WORLD);

  if (myRank == 0) {
    printf("Result: %f", global_sum);
  }

  MPI_Finalize();

  return 0;
}
