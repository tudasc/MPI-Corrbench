#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * Deadlock occurs if there is no corresponding send call for
 * the receive routine(line 17)
 */
int main(int argc, char *argv[]) {
  int myRank;
  int buffer[4] = {0, 2, 3, 4};

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (myRank == 1) {
    MPI_Recv(buffer, 3, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
  }

  MPI_Finalize();

  return 0;
}
