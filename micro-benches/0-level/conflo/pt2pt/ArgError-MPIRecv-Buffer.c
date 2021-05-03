#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * null pointer as recv buffer. (line 25)
 */

#define MSG_TAG_A 124523
#define N 1000

/*
invalid buffer on line 45
*/
int main(int argc, char *argv[]) {
  int rank;
  int buffer[N] = {0};

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    MPI_Send(buffer, N, MPI_INT, 1, MSG_TAG_A, MPI_COMM_WORLD);
  } else if (rank == 1) {
    int *ptr = &buffer[0];
    if (argc == 1) {
      ptr = NULL;
    } else {
      ptr = &buffer[0];
    }

    MPI_Recv(ptr, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();

  return 0;
}
