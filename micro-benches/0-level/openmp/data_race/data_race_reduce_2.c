#include "nondeterminism.h"

#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUFFER_LENGTH_INT 100
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))

#define NUM_THREADS 2

bool has_error(const int *buffer, int expected) {
  for (int i = 0; i < BUFFER_LENGTH_INT; ++i) {
    if (buffer[i] != expected) {
      return true;
    }
  }
  return false;
}

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

  int reduced_data[BUFFER_LENGTH_INT];
  int local_data[BUFFER_LENGTH_INT];
  int local_data2[BUFFER_LENGTH_INT];
  fill_message_buffer(reduced_data, BUFFER_LENGTH_BYTE, 0);
  fill_message_buffer(local_data, BUFFER_LENGTH_BYTE, 6);
  fill_message_buffer(local_data2, BUFFER_LENGTH_BYTE, 5);

#pragma omp parallel sections
  {
#pragma omp section
    { MPI_Reduce(local_data2, reduced_data, BUFFER_LENGTH_INT, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD); }
#pragma omp section
    { MPI_Reduce(local_data, reduced_data, BUFFER_LENGTH_INT, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD); }
  }

  MPI_Finalize();

  return 0;
}
