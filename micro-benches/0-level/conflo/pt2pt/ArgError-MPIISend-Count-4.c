#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define MSG_TAG_A 123
#define N 1000

int compute_buffer_index() { return N - (N + 1); }

/*
 * Negative number of elements specified in send. (line 22)
 */
int main(int argc, char **argv) {
  int buffer[N] = {0};

  MPI_Init(NULL, NULL);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int buffer_size = N;

  if (argc == 1) {
    buffer_size = N - (N + 1);
  } else {
    buffer_size = N;
  }

  if (rank == 0) {
    MPI_Send(&buffer, buffer_size, MPI_INT, 1, MSG_TAG_A, MPI_COMM_WORLD);
  } else if (rank == 1) {
    MPI_Recv(&buffer, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();
  return 0;
}
