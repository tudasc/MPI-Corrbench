#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define MSG_TAG_A 124523
#define N 1000

/*
 * Invalid rank (size of communicator) in MPI_Irecv . line 24
 */

int main(int argc, char *argv[]) {
  int rank, comm_size;
  int buffer[N] = {0};

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  if (rank == 0) {
    MPI_Send(buffer, N, MPI_INT, 1, MSG_TAG_A, MPI_COMM_WORLD);
  } else if (rank == 1) {
    MPI_Request req;
    MPI_Status stat;
    MPI_Irecv(buffer, N, MPI_INT, comm_size, MSG_TAG_A, MPI_COMM_WORLD, &req);
    MPI_Wait(&req, &stat);
  }

  MPI_Finalize();

  return 0;
}
