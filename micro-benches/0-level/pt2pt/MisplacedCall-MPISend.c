#include <mpi.h>
#include <stddef.h>

/*
 * illegal call to send before init. (line 9)
 */
int main(int argc, char *argv[]) {
  int data = 0;

  MPI_Send(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

  MPI_Init(&argc, &argv);

  MPI_Recv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);

  MPI_Finalize();

  return 0;
}
