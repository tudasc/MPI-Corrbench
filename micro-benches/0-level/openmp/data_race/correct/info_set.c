#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

// one MPI info obj per thread is fine
#undef NUM_THREADS
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

  const char *values[2] = {"Thread 1", "Thread 2"};

#pragma omp parallel num_threads(NUM_THREADS) private(info_obj)
  {
    MPI_Info_create(&info_obj);
    MPI_Info_set(info_obj, "Hello", values[omp_get_thread_num()]);
    MPI_Info_free(&info_obj);); /* A */
  }

  MPI_Finalize();

  has_error_manifested(true);

  return 0;
}
