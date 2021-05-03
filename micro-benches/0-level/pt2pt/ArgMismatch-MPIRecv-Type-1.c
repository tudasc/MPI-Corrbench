#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define MSG_TAG_A 124523
#define N 1000

/*
 * Type mismatch in Recv for buffer and MPI data type.
 * line 23
 */
int main(int argc, char *argv[]) {
  int rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  char recvbuffer[1];

  if (rank == 0) {
    double test = 42.0;
    MPI_Send(&test, 1, MPI_DOUBLE, 1, MSG_TAG_A, MPI_COMM_WORLD);
  } else if (rank == 1) {
    MPI_Recv(&recvbuffer, 1, MPI_DOUBLE, 0, MSG_TAG_A, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("%d", recvbuffer[0]);
  }

  MPI_Finalize();

  return 0;
}
