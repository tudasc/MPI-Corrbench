#include "nondeterminism.h"

#include <mpi.h>
#include <stdlib.h>

// Deadlock occurs, if the probe (marker "A") matches to the same message on both threads meaning that one thread will
// wait indefinitely for the message to arrive (at marker "B")

#define BUFFER_LENGTH_INT 2
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))

#define NUM_THREADS 2

int main(int argc, char *argv[]) {
  int provided;
  const int requested = MPI_THREAD_MULTIPLE;

  MPI_Init_thread(&argc, &argv, requested, &provided);
  if (provided < requested) {
    has_error_manifested(false);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  int size;
  int rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

#pragma omp parallel num_threads(NUM_THREADS)
  {
    if (rank == 0) {
#pragma omp sections
      {
#pragma omp section
        {
          int value[2] = {-1, -1};
          MPI_Send(&value[0], 2, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
#pragma omp section
        {
          int value = -2;
          MPI_Send(&value, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
        }
      }

    } else if (rank == 1) {
      // Deadlocks often (not always)
      MPI_Status status;
      MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); /* A */

      int count;
      MPI_Get_count(&status, MPI_INT, &count);

      int *value = (int *)malloc(sizeof(int) * count);
      MPI_Recv(value, count, MPI_INT, 0, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /* B */

      free(value);
    }
  }

  MPI_Finalize();

  has_error_manifested(false);

  return 0;
}
