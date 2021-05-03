#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * Deadlock occurs if MPIReduce is not executed by all
 * processes(line 21)
 */
int main(int argc, char *argv[]) {
  int myRank;

  int local_sum = 4;
  int global_sum;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (myRank != 0) {
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}
