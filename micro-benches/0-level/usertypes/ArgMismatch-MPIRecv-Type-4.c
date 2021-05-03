#include <mpi.h>
#include <stddef.h>
#include <stdlib.h>

#define MY_TAG 0

/*
 * Type mismatch between MPI_Send and MPI_Recv
 * line 31
 */

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  int *buf = (int *)malloc(4 * sizeof(int));
  buf[0] = buf[1] = buf[2] = buf[3] = 0;
  double *buf_d = (double *)malloc(2 * sizeof(double));
  buf_d[0] = buf_d[1] = 1.0;

  MPI_Datatype two_ints;
  MPI_Type_contiguous(2, MPI_INT, &two_ints);
  MPI_Type_commit(&two_ints);

  if (my_rank == 0) {
    MPI_Send(buf, 1, two_ints, 1, MY_TAG, MPI_COMM_WORLD);
  } else if (my_rank == 1) {
    MPI_Recv(buf_d, 2, MPI_DOUBLE, 0, MY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Type_free(&two_ints);
  MPI_Finalize();

  free(buf);
  free(buf_d);

  return 0;
}
