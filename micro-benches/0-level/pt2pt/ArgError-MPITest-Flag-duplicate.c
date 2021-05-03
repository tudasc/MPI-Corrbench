#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define MSG_TAG_A 124
#define MSG_TAG_B 1234
#define N 1000
#define N2 1204

/*
 * null pointer for flag to test. (line 29)
 */
int main(int argc, char *argv[]) {
  int buffer[N2] = {0};
  int buffer2[N2] = {0};

  MPI_Init(&argc, &argv);
  int rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Request request;
  MPI_Status mpi_status;

  int *flag = NULL;

  if (rank == 1) {
    MPI_Irecv(buffer, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, &request);

    MPI_Test(&request, flag, &mpi_status);

    while (*flag == 0) {
      MPI_Test(&request, flag, &mpi_status);
    }
  } else if (rank == 0) {
    MPI_Send(buffer2, N, MPI_INT, 1, MSG_TAG_A, MPI_COMM_WORLD);
  }

  MPI_Finalize();
}
