#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * scatter has wrong number of elements for recv buffer. (line 16)
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;

  int local_sum = 4;
  int global_sum = 0;

  MPI_Init(&argc, &argv);

  int root = 0;

  int num_elems = 1;
  if (argc == 1) {
    num_elems = 3;
  } else {
    num_elems = 1;
  }

  MPI_Scatter(&local_sum, 1, MPI_INT, &global_sum, num_elems, MPI_INT, root, MPI_COMM_WORLD);

  MPI_Finalize();

  return 0;
}
