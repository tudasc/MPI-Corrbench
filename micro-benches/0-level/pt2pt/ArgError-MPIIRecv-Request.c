#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define MSG_TAG_A 124523
#define N 1000

/*
 * NULL pointer for MPI_request in MPI_Irecv. line 23
 */

int main(int argc, char *argv[]) {
  int rank;
  int buffer[N] = {0};

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    MPI_Send(buffer, N, MPI_INT, 1, MSG_TAG_A, MPI_COMM_WORLD);
  } else if (rank == 1) {
    MPI_Request *null_req = NULL;
    MPI_Status stat;
    MPI_Irecv(buffer, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, null_req);
    MPI_Wait(&null_req, &stat);
  }

  MPI_Finalize();

  return 0;
}
