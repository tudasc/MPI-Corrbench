#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * Error occurs if the wait call is missing for both processes,
 * line 28 and 32
 */
int main(int argc, char *argv[]) {
  int myRank;
  int buffer[10], buffer2[10];
  int left, right;

  MPI_Request request;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (myRank == 0) {
    MPI_Isend(buffer2, 10, MPI_INT, 1, 123, MPI_COMM_WORLD, &request);
    // Missing Wait Call
  } else if (myRank == 1) {
    MPI_Irecv(buffer, 10, MPI_INT, 0, 123, MPI_COMM_WORLD, &request);
    // Missing Wait Call
  }

  MPI_Request_free(&request);

  MPI_Finalize();

  return 0;
}
