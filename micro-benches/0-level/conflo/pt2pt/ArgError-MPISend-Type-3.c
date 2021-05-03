#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MSG_TAG_A 124523
#define N 1000

/*
 * wrong data type to send. (line 21)
 */
int main(int argc, char *argv[]) {
  int rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  char *recvbuffer = malloc(sizeof(char));
  char test = 'a';
  MPI_Datatype ty = MPI_CHAR;
  if (argc == 1) {
    ty = MPI_DOUBLE;
  } else {
    ty = MPI_CHAR;
  }

  if (rank == 0) {
    MPI_Send(&test, 1, ty, 1, MSG_TAG_A, MPI_COMM_WORLD);
  } else if (rank == 1) {
    MPI_Recv(&recvbuffer, 1, ty, 0, MSG_TAG_A, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("%d", recvbuffer[0]);
  }

  MPI_Finalize();

  return 0;
}
