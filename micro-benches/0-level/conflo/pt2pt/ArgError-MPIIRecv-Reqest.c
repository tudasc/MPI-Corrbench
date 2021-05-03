#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define MSG_TAG_A 124523
#define N 1000

/*
 * NULL Pointer in MPI_Recv, line 22
 */

int main(int argc, char *argv[]) {
  int rank;
  int buffer[N];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    MPI_Send(buffer, N, MPI_INT, 1, MSG_TAG_A, MPI_COMM_WORLD);
  } else if (rank == 1) {
    MPI_Request *null_req = NULL;
    MPI_Request real_req;
    if (argc == 1) {
      null_req = NULL;
    } else {
      null_req = &real_req;
    }
    MPI_Irecv(buffer, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, null_req);
  }

  MPI_Finalize();

  return 0;
}
