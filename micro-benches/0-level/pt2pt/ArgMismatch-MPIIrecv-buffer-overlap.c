#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * recv buffers are overlappng ==> but write acces to recv buffer is forbidden until irevćv is finished. (line 28)
 */

#define MSG_TAG_A 124523
#define N 1000

int main(int argc, char *argv[]) {
  int rank;
  int buffer[N];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Request mpi_request1;
  MPI_Request mpi_request2;

  MPI_Status mpi_status;

  if (rank == 0) {
    MPI_Send(buffer, N, MPI_INT, 1, MSG_TAG_A, MPI_COMM_WORLD);
    MPI_Send(buffer, N / 2, MPI_INT, 1, MSG_TAG_A, MPI_COMM_WORLD);

  } else if (rank == 1) {
    MPI_Irecv(buffer, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, &mpi_request1);
    MPI_Irecv(&buffer[N / 2], N / 2, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, &mpi_request2);

    MPI_Wait(&mpi_request1, MPI_STATUS_IGNORE);
    MPI_Wait(&mpi_request2, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();

  return 0;
}
