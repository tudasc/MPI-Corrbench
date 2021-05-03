#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * message tags don't agree. (line 16/19)
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;
  int buffer[4] = {0, 2, 3, 4};

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (myRank == 0) {
    MPI_Send(buffer, 4, MPI_INT, 1, 0, MPI_COMM_WORLD);
    printf("Operation Complete");
  } else if (myRank == 1) {
    MPI_Recv(buffer, 4, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
    printf("Operation Complete");
  }

  MPI_Finalize();

  return 0;
}
