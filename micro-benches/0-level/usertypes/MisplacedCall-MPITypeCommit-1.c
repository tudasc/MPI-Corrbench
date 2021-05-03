#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * type commit after send. (line 28)
 */
int main(int argc, char *argv[]) {
  int myRank;
  int buffer[24];

  MPI_Status status;
  MPI_Datatype type, type2;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  MPI_Type_contiguous(3, MPI_INT, &type2);

  MPI_Type_vector(3, 2, 6, type2, &type);

  if (myRank == 0) {
    int i;

    for (i = 0; i < 24; i++) {
      buffer[i] = i;
    }
    MPI_Send(buffer, 1, type, 1, 123, MPI_COMM_WORLD);
    MPI_Type_commit(&type2);
    MPI_Type_commit(&type);
  } else if (myRank == 1) {
    int i;

    for (i = 0; i < 24; i++) {
      buffer[i] = -1;
    }

    MPI_Recv(buffer, 1, type, 0, 123, MPI_COMM_WORLD, &status);

    for (i = 0; i < 24; i++) {
      printf("buffer[%d] = %d\n", i, buffer[i]);
    }

    fflush(stdout);
  }

  MPI_Type_free(&type);
  MPI_Type_free(&type2);

  MPI_Finalize();

  return 0;
}
