#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * negative count in type construction. (line 16)
 */
int main(int argc, char *argv[]) {
  int myRank;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  MPI_Datatype type;
  int count = 1;

  if (myRank == 0) {
    if (argc == 1) {
      count = -1;
    } else {
      count = 1;
    }
    MPI_Type_contiguous(count, MPI_INT, &type);
    MPI_Type_commit(&type);
    MPI_Type_free(&type);
  }

  MPI_Finalize();

  return 0;
}
