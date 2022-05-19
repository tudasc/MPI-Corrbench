#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

// implicit barrier enforces correct ordering

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

  MPI_Info info_obj;
  MPI_Info_create(&info_obj);

#pragma omp parallel num_threads(NUM_THREADS)
  {
#pragma omp single
    {
#ifdef USE_DISTURBED_THREAD_ORDER
      us_sleep(10);
#endif
      MPI_Info_set(info_obj, "Hello", "World"); /* A */
    }
#pragma omp single
    {
#ifndef USE_DISTURBED_THREAD_ORDER
      us_sleep(10);
#endif

      MPI_Info_free(&info_obj); /* B */
    }
  }

#ifdef USE_DISTURBED_THREAD_ORDER
  has_error_manifested(true);
#else
  has_error_manifested(false);
#endif

  MPI_Finalize();

  return 0;
}
