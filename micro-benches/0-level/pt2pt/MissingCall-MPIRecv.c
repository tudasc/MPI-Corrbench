#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * Deadlock occurs if P0 sends a message and there is no
 * corresponding receive from P1(line 18)
 */
int main(int argc, char *argv[]) {
  int myRank;
  int buffer[4] = {0, 2, 3, 4};

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (myRank == 0) {
    MPI_Send(buffer, 3, MPI_INT, 1, 123, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}
