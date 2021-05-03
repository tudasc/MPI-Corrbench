#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define MSG_TAG_A 124
#define MSG_TAG_B 1234
#define N 1000
#define N2 1204

/*
 * null pointer to test. (line 30)
 */
int main(int argc, char **argv) {
  int buffer[N2], buffer2[N2];

  MPI_Init(&argc, &argv);
  int rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Request request;
  MPI_Status mpi_status;

  int flag = 0;
  int *fl_ptr = &flag;
  if (argc == 1) {
    fl_ptr = NULL;
  } else {
    fl_ptr = &flag;
  }

  if (rank == 0) {
    MPI_Send(buffer2, N, MPI_INT, 1, 123, MPI_COMM_WORLD);
  } else if (rank == 1) {
    MPI_Irecv(buffer, N, MPI_INT, 0, 123, MPI_COMM_WORLD, &request);
    MPI_Test(&request, fl_ptr, &mpi_status);

    while (flag == 0) {
      MPI_Test(&request, &flag, &mpi_status);
    }
  }

  MPI_Finalize();
  return 0;
}
