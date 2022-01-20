#include "nondeterminism.h"

#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>

// This test is loosely based on test DRB013 of the data race bench.
// See https://github.com/LLNL/dataracebench
//
// Data race in parallel region due to buffer writes in parallel for loop with nowait clause (marker "A").
// Hence, the reduce at marker "B" is executed by some thread while other threads may still write to the buffer.

bool has_error(const int *buffer, int expected) {
  for (int i = 0; i < BUFFER_LENGTH_INT; ++i) {
    if (buffer[i] != expected) {
      return true;
    }
  }
  return false;
}

//
int main(int argc, char *argv[]) {
  int provided;
  const int requested = MPI_THREAD_SERIALIZED;

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
  fill_message_buffer(reduced_data, BUFFER_LENGTH_BYTE, 0);
  fill_message_buffer(local_data, BUFFER_LENGTH_BYTE, 6);

  const int value = 5;

#pragma omp parallel num_threads(NUM_THREADS)
  {
    DISTURB_THREAD_ORDER
#pragma omp for nowait
    for (int i = 0; i < BUFFER_LENGTH_INT; i++) {
      local_data[i] = local_data[i] + value; /* A */
    }
#pragma omp barrier  // fixes the data race
#pragma omp single
    { MPI_Reduce(local_data, reduced_data, BUFFER_LENGTH_INT, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); /* B */ }
  }

  if (rank == 0) {
    const bool error = has_error(reduced_data, size * value);
    has_error_manifested(error);
  }

  MPI_Finalize();

  return 0;
}
