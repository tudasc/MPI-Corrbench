#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define MSG_TAG_A 124
#define MSG_TAG_B 1234
#define N 1000
#define N2 1204

/*
 * Invalid communicator in send. line 24
 */
int main(int argc, char **argv) {
  int buffer[N] = {0};

  MPI_Init(&argc, &argv);
  int rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Request request;
  MPI_Status status;

  if (rank == 0) {
    MPI_Isend(buffer, N, MPI_INT, 1, MSG_TAG_A, MPI_COMM_NULL, &request);
    MPI_Wait(&request, &status);
  } else if (rank == 1) {
    MPI_Recv(buffer, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();
  return 0;
}
