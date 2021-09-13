#include "nondeterminism.h"

#include <mpi.h>
#include <stdlib.h>

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
          printf("Go1 %i\n", value[0]);
          MPI_Send(&value[0], 2, MPI_INT, 1, 0, MPI_COMM_WORLD);
          printf("Go1 %i\n", value[0]);
        }
#pragma omp section
        {
          int value = -2;
          printf("Go2 %i\n", value);
          MPI_Send(&value, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
          printf("Go2 %i\n", value);
        }
      }

    } else if (rank == 1) {
      // Deadlocks often (not always)
      MPI_Status status;
      MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      printf("Probe %i\n", status.MPI_TAG);

      int count;
      MPI_Get_count(&status, MPI_INT, &count);

      int *value = (int *)malloc(sizeof(int) * count);
      MPI_Recv(value, count, MPI_INT, 0, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      const bool thread_race = (count == 1 && value[0] != -2) || (count == 2 && value[0] != -1);
      printf("Race %i\n", thread_race);

      free(value);
    }
  }

  MPI_Finalize();

  has_error_manifested(false);

  return 0;
}
