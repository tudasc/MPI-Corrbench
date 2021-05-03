#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * invalid count value in MPI_Send, line 38
 */

#define MSG_TAG_A 124523
#define N 1000

/*
invalid argument on line 40
*/
int main(int argc, char *argv[]) {
  int rank;
  int buffer[N];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int target_rank = 1;
  if (argc == 1) {
    target_rank = 10;
  } else {
    target_rank = 1;
  }

  if (rank == 0) {
    MPI_Send(buffer, N, MPI_INT, target_rank, MSG_TAG_A, MPI_COMM_WORLD);
  } else if (rank == 1) {
    MPI_Recv(buffer, N, MPI_INT, 0, MSG_TAG_A, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();

  return 0;
}
