#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MY_TAG 0

/*
 * Wrong type for second struct member in user-defined datatype. line 47
 * line 44
 */

struct S {
  int i;
  unsigned int ui;
  char c;
  double d;
};

typedef struct S S;

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  S *buf = (S *)malloc(sizeof(S));

  const int count = 4;
  int blocklengths[4] = {1, 1, 1, 1};

  MPI_Aint displacements[4];
  MPI_Get_address(buf, displacements);
  MPI_Get_address(&buf->ui, displacements + 1);
  MPI_Get_address(&buf->c, displacements + 2);
  MPI_Get_address(&buf->d, displacements + 3);

  MPI_Aint base;
  base = displacements[0];
  displacements[0] = MPI_Aint_diff(displacements[0], base);
  displacements[1] = MPI_Aint_diff(displacements[1], base);
  displacements[2] = MPI_Aint_diff(displacements[2], base);
  displacements[3] = MPI_Aint_diff(displacements[3], base);

  MPI_Datatype old_types[4] = {MPI_INT, MPI_INT, MPI_CHAR, MPI_DOUBLE};

  MPI_Datatype new_t;
  MPI_Type_create_struct(count, blocklengths, displacements, old_types, &new_t);
  MPI_Type_commit(&new_t);

  if (my_rank == 0) {
    buf->i = -42;
    buf->ui = 42;
    buf->c = 'a';
    buf->d = 21 * 2.0;

    MPI_Send(buf, 1, new_t, 1, MY_TAG, MPI_COMM_WORLD);
  } else if (my_rank == 1) {
    MPI_Recv(buf, 1, new_t, 0, MY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Received: %d, %u, %c, %f\n", buf->i, buf->ui, buf->c, buf->d);
  }

  MPI_Type_free(&new_t);
  MPI_Finalize();

  free(buf);

  return 0;
}
