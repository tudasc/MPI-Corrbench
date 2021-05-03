#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * Too many elements in Reduce call. (line 20)
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int local_sum = 4;
  int offset = 0xff;
  int global_sum = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  int num_elems = 1;
  if (argc == 1) {
    num_elems = 2;
  } else {
    num_elems = 1;
  }

  if (myRank == 0) {
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  } else {
    MPI_Reduce(&local_sum, &global_sum, num_elems, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  }

  if (myRank == 0) {
    printf("Result: %d", global_sum);
  }

  MPI_Finalize();

  return 0;
}
