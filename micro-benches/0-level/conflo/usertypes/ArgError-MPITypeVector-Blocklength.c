#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * negative block length for type creation. (line 17)
 */
int main(int argc, char *argv[]) {
  int myRank;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  MPI_Datatype type;

  if (myRank == 0) {
    int neg_block_length = 1;
    if (argc == 1) {
      neg_block_length = -1;
    } else {
      neg_block_length = 1;
    }
    MPI_Type_vector(2, neg_block_length, 4, MPI_INT, &type);
    MPI_Type_commit(&type);
    MPI_Type_free(&type);
  }

  MPI_Finalize();

  return 0;
}
