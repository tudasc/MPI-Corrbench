#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * null pointer for old type. (line 17)
 */
int main(int argc, char *argv[]) {
  int myRank;

  MPI_Datatype type;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (myRank == 0) {
    void *ptr = NULL;
    MPI_Type_vector(2, 3, 4, ptr, &type);
    MPI_Type_commit(&type);
    MPI_Type_free(&type);
  }

  MPI_Finalize();

  return 0;
}
