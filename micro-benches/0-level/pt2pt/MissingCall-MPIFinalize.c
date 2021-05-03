#include <mpi.h>
#include <stddef.h>

#include <stdio.h>

/*
 * missing call to finalize. (line 11)
 */
int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  printf("argc: %d\n", argc);

  return 0;
}
