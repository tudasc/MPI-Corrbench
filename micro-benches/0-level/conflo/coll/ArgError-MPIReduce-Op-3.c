#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * MPI_Reduce has an invalid operation / data type combination. (line 23)
 *
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int b_false = 0;
  int b_true = 1;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  MPI_Op op = MPI_LOR;
  if (argc == 1) {
    op = MPI_PROD;
  } else {
    op = MPI_LOR;
  }
  MPI_Reduce(&b_true, &b_false, 1, MPI_C_BOOL, op, 0, MPI_COMM_WORLD);

  if (myRank == 0) {
    printf("Result: %d", b_false);
  }

  MPI_Finalize();

  return 0;
}
