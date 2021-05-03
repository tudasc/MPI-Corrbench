#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * tags do not agree. (line 19/23)
 */
int main(int argc, char *argv[]) {
  int myRank, numProcs;
  int buffer[4] = {0, 2, 3, 4};

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  MPI_Request mpi_request;
  MPI_Status mpi_status;

  if (myRank == 0) {
    MPI_Isend(buffer, 4, MPI_INT, 1, 0, MPI_COMM_WORLD, &mpi_request);
    MPI_Wait(&mpi_request, &mpi_status);
    printf("Operation Complete");
  } else if (myRank == 1) {
    MPI_Recv(buffer, 4, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Operation Complete");
  }

  MPI_Finalize();

  return 0;
}
