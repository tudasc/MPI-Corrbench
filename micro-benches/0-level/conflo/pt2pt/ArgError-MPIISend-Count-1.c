#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * Negative number of elements specified in send. (line 22)
 *
 */

#define MSG_TAG_A 124523
#define N 1000

int main(int argc, char *argv[]) {
  int rank;
  int buffer[N] = {0};

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Request mpi_request;
  MPI_Status mpi_status;

  if (rank == 0) {
    int buffer_size = N;
    if (argc == 1) {
      buffer_size = -1;
    } else {
      buffer_size = N;
    }
    MPI_Isend(buffer, buffer_size, MPI_INT, 1, MSG_TAG_A, MPI_COMM_WORLD, &mpi_request);
    MPI_Wait(&mpi_request, &mpi_status);
  } else if (rank == 1) {
    MPI_Recv(buffer, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();

  return 0;
}
