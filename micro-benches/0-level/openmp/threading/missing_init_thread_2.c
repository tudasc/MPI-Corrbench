#include <mpi.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>

/*
 * A Threaded Program need to use MPI_Init Thread, even if one only requests the  MPI_THREAD_FUNNELED level
 */
int main(int argc, char *argv[]) {
  int myRank;
  int buffer_out[10], buffer_in[10];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

#pragma omp parallel default(none) shared(myRank, buffer_in, buffer_out)
  {
#pragma omp for
    for (int i = 0; i < 10; i++) {
      buffer_out[i] = i * 10;
    }
// implicit OpenMP barrier
#pragma omp master
    {
      if (myRank == 0) {
        MPI_Send(buffer_out, 10, MPI_INT, 1, 123, MPI_COMM_WORLD);
      } else if (myRank == 1) {
        MPI_Recv(buffer_in, 10, MPI_INT, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }
  }

  if (myRank == 0) {
    MPI_Send(buffer_out, 10, MPI_INT, 1, 123, MPI_COMM_WORLD);
  } else if (myRank == 1) {
    MPI_Recv(buffer_in, 10, MPI_INT, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();

  return 0;
}
