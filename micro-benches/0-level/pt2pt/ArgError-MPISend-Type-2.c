#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define MSG_TAG_A 124523
#define N 1000

/*
 * null pointer type in send. (line 19)
 */
int main(int argc, char *argv[]) {
  int rank;
  int buffer[N] = {0};

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    MPI_Datatype null_type = NULL;
    MPI_Send(buffer, N, null_type, 1, MSG_TAG_A, MPI_COMM_WORLD);
  } else if (rank == 1) {
    MPI_Recv(buffer, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();

  return 0;
}
