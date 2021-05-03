#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define MSG_TAG_A 123
#define N 1000

/*
 * Too many elements to send. (line 18)
 */
int main(int argc, char *argv[]) {
  int buffer[N] = {0};

  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    MPI_Send(buffer, 5 * N, MPI_INT, 1, MSG_TAG_A, MPI_COMM_WORLD);
  } else if (rank == 1) {
    MPI_Recv(&buffer, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();
}
