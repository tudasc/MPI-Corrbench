#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * NULL pointer as mpi request in isend. (line 26)
 */

#define MSG_TAG_A 124523
#define N 1000

/*
invalid argument on line 46
*/
int main(int argc, char *argv[]) {
  int rank;
  int buffer[N];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Request mpi_request;
  MPI_Status mpi_status;

  if (rank == 0) {
    MPI_Request *ptr = NULL;

    MPI_Isend(buffer, N, MPI_INT, 1, MSG_TAG_A, MPI_COMM_WORLD, ptr);
    MPI_Wait(&mpi_request, &mpi_status);
  } else if (rank == 1) {
    MPI_Recv(buffer, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();

  return 0;
}
