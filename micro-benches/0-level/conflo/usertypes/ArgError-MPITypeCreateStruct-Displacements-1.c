#include <assert.h>
#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MY_TAG 0

/*
 * The displacements are not calculated with the respective MPI functions.
 * (Potentially) Leading to errors in the type layout.
 * line 31
 */

struct S {
  int i;
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

  int count = 3;
  int blocklengths[3] = {1, 1, 1};
  /* Use wrong displacement */
  MPI_Aint displacements[3];
  if (argc == 1) {
    displacements[0] = 0;
    displacements[0] = sizeof(int);
    displacements[0] = sizeof(char) + sizeof(int);
  } else {
    assert(0 == 1 && "This benchmark has no \"good\" case.");
  }
  MPI_Datatype old_types[3] = {MPI_INT, MPI_CHAR, MPI_DOUBLE};

  MPI_Datatype new_t;
  MPI_Type_create_struct(count, blocklengths, displacements, old_types, &new_t);
  MPI_Type_commit(&new_t);

  S *buf = (S *)malloc(sizeof(S));

  if (my_rank == 0) {
    buf->i = 42;
    buf->c = 'a';
    buf->d = 21 * 2.0;

    MPI_Send(buf, 1, new_t, 1, MY_TAG, MPI_COMM_WORLD);
  } else if (my_rank == 1) {
    MPI_Recv(buf, 1, new_t, 0, MY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Received: %d, %c, %f\n", buf->i, buf->c, buf->d);
  }

  MPI_Type_free(&new_t);
  MPI_Finalize();

  free(buf);

  return 0;
}
