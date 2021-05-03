#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define N 1000

/*
 * The used MPI Tag in MPI_Send/MPI_Recv overflows the MPI_TAG_UB. (line 19/21)
 */
int main(int argc, char *argv[]) {
  int rank;
  int buffer[N] = {0};
  long long too_large_tag = MPI_TAG_UB;
  too_large_tag += 1;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    MPI_Send(buffer, N, MPI_INT, 1, too_large_tag, MPI_COMM_WORLD);
  } else if (rank == 1) {
    MPI_Recv(buffer, N, MPI_INT, 0, too_large_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();

  return 0;
}
