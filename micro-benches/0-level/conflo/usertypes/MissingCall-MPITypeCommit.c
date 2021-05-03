#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * Error occurs if the commit call is missing, line 21
 */
int main(int argc, char *argv[]) {
  int myRank;
  char buffer[4] = {'0', '1', '2', '3'};
  int COUNT = 100;

  MPI_Datatype type;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  MPI_Type_contiguous(COUNT, MPI_CHAR, &type);

  if (myRank == 0) {
    MPI_Send(buffer, 1, type, 1, 123, MPI_COMM_WORLD);
  } else if (myRank == 1) {
    MPI_Recv(buffer, 1, type, 0, 123, MPI_COMM_WORLD, &status);
    printf("%d", buffer[0]);
  }

  MPI_Type_free(&type);

  MPI_Finalize();

  return 0;
}
