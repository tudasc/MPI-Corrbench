#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * Deadlock occurs if the message can not be bufferde (line 17 - 25 see the msg tags)
 */
int main(int argc, char *argv[]) {
  int myRank;
  int buffer[4] = {0, 2, 3, 4};

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (myRank == 0) {
    MPI_Send(buffer, 4, MPI_INT, 1, 0, MPI_COMM_WORLD);
    MPI_Send(buffer, 4, MPI_INT, 1, 1, MPI_COMM_WORLD);
    printf("Operation Complete");
  } else if (myRank == 1) {
    MPI_Recv(buffer, 4, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
    MPI_Recv(buffer, 4, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
    printf("Operation Complete");
  }

  MPI_Finalize();

  return 0;
}
