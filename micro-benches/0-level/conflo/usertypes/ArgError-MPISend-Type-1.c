#include <mpi.h>
#include <stddef.h>
#include <stdlib.h>

#define MY_TAG 0

/*
 * Type mismatch between send buffer and declared data type
 * line 29/31
 */

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  long int *buf = (long int *)malloc(2 * sizeof(long int));
  buf[0] = buf[1] = 0;

  MPI_Datatype two_ints;
  MPI_Datatype ty = MPI_LONG_INT;
  if (argc == 1) {
    ty = MPI_INT;
  } else {
    ty = MPI_LONG_INT;
  }
  MPI_Type_contiguous(2, ty, &two_ints);
  MPI_Type_commit(&two_ints);

  if (my_rank == 0) {
    MPI_Send(buf, 1, two_ints, 1, MY_TAG, MPI_COMM_WORLD);
  } else if (my_rank == 1) {
    MPI_Recv(buf, 1, two_ints, 0, MY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Type_free(&two_ints);
  MPI_Finalize();

  free(buf);

  return 0;
}
