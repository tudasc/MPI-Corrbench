#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define MSG_TAG_A 123
#define N 1000
#define N2 1204

/*
 * NULL Pointer in MPI_Test, line 30
 */
int main(int argc, char **argv) {
  int buffer[N2] = {0};
  int buffer2[N2] = {0};

  MPI_Init(NULL, NULL);
  int rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Request request;
  MPI_Status mpi_status;
  int flag = 0;

  MPI_Status *null_stat = &mpi_status;
  if (argc == 1) {
    null_stat = NULL;
  } else {
    null_stat = &mpi_status;
  }

  if (rank == 0) {
    MPI_Send(buffer2, N, MPI_INT, 1, MSG_TAG_A, MPI_COMM_WORLD);
  } else if (rank == 1) {
    MPI_Irecv(buffer, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, &request);
    MPI_Test(&request, &flag, null_stat);

    while (flag == 0) {
      MPI_Test(&request, &flag, &mpi_status);
    }
  }

  MPI_Finalize();
  return 0;
}
