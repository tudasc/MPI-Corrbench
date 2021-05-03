#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * Deadlock occurs if two processes are waiting for each other(line 17 - 25)
 */
int main(int argc, char *argv[]) {
  int myRank;
  int buffer[4] = {0, 2, 3, 4};

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (myRank == 0) {
    if (argc == 1) {
      MPI_Recv(buffer, 4, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
      MPI_Send(buffer, 4, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } else {
      MPI_Send(buffer, 4, MPI_INT, 1, 0, MPI_COMM_WORLD);
      MPI_Recv(buffer, 4, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
    }
    printf("Operation Complete");
  } else if (myRank == 1) {
    MPI_Recv(buffer, 4, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
    MPI_Send(buffer, 4, MPI_INT, 0, 0, MPI_COMM_WORLD);
    printf("Operation Complete");
  }

  MPI_Finalize();

  return 0;
}
