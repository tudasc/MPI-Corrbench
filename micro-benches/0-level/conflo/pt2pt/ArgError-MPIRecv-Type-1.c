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

  void *target_buf = NULL;
  char c_recvbuffer[1];
  double d_recvbuffer[1];
  if (argc == 1) {
    target_buf = c_recvbuffer;
  } else {
    target_buf = d_recvbuffer;
  }

  if (rank == 0) {
    double test = 42.0;
    MPI_Send(&test, 1, MPI_DOUBLE, 1, MSG_TAG_A, MPI_COMM_WORLD);
  } else if (rank == 1) {
    MPI_Recv(target_buf, 1, MPI_DOUBLE, 0, MSG_TAG_A, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();

  return 0;
}
