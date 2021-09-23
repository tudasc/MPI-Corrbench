#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

// Using conflicting MPI calls on MPI_Info:
// Concurrently setting MPI_Info (marker "A") is a data race.

#define NUM_THREADS 8

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

  MPI_Info info_obj;
  MPI_Info_create(&info_obj);

  const char *values[2] = {"Thread 1", "Thread 2"};

#pragma omp parallel num_threads(NUM_THREADS)
  { MPI_Info_set(info_obj, "Hello", values[omp_get_thread_num()]); /* A */ }

  MPI_Info_free(&info_obj);
  MPI_Finalize();

  has_error_manifested(true);

  return 0;
}
