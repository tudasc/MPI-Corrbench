#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MY_TAG 0

/*
 * The stride in bytes given to type create is wrong, and ends up pointing into a basic data type.
 * line 33
 */

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  /* buf is matrix with x*y elements [x][y] */
  const int x = 16;
  const int y = 16;
  const int m_size = x * y;
  float *buf = (float *)malloc(m_size * sizeof(float));

  for (int i = 0; i < m_size; ++i) {
    buf[i] = i;
  }

  MPI_Datatype col_t;
  /* One column vector */
  int strideInBytes = 4;
  if (argc == 1) {
    strideInBytes = 3;
  } else {
    strideInBytes = 4;
  }
  MPI_Type_create_hvector(16, 1, strideInBytes, MPI_FLOAT, &col_t);
  MPI_Type_commit(&col_t);

  if (my_rank == 0) {
    MPI_Send(buf, 1, col_t, 1, MY_TAG, MPI_COMM_WORLD);
  } else if (my_rank == 1) {
    MPI_Recv(buf, 1, col_t, 0, MY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Type_free(&col_t);
  MPI_Finalize();

  free(buf);

  return 0;
}
