#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define MSG_TAG_A 123
#define N 1000
#define N2 1204

/*
 * NULL Pointer in MPI_Test, line 30
 */
int main() {
  int buffer[N2] = {0};
  int buffer2[N2] = {0};

  MPI_Init(NULL, NULL);
  int rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Request request;
  MPI_Status mpi_status;

  void *ptr = NULL;
  int flag = 0;

  if (rank == 0) {
    MPI_Send(buffer2, N, MPI_INT, 1, MSG_TAG_A, MPI_COMM_WORLD);
  } else if (rank == 1) {
    MPI_Irecv(buffer, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, &request);
    MPI_Test(&request, &flag, ptr);

    while (flag == 0) {
      MPI_Test(&request, &flag, &mpi_status);
    }
  }

  MPI_Finalize();
}
