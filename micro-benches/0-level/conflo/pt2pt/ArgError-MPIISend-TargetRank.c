#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * Illegal rank (too large) in send. (line 23)
 */

#define MSG_TAG_A 124523
#define N 1000

int main(int argc, char *argv[]) {
  int rank;
  int size;
  int buffer[N];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Request mpi_request;
  MPI_Status mpi_status;

  if (rank == 0) {
    int unavailable_dest = 1;
    if (argc == 1) {
      unavailable_dest = size;
    } else {
      unavailable_dest = 1;
    }
    MPI_Isend(buffer, N, MPI_INT, unavailable_dest, MSG_TAG_A, MPI_COMM_WORLD, &mpi_request);
    MPI_Wait(&mpi_request, &mpi_status);
  } else if (rank == 1) {
    MPI_Recv(buffer, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();

  return 0;
}
