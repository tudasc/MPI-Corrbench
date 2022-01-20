#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

// Using conflicting MPI calls on MPI_Info:
// Concurrently setting MPI_Info (marker "A") is a data race.

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

  DEF_ORDER_CAPTURING_VARIABLES

  MPI_Info info_obj;
  MPI_Info_create(&info_obj);

#pragma omp parallel num_threads(NUM_THREADS) reduction(+ : overlap_count)
  {
    size_t length = snprintf(NULL, 0, "Thread %d", omp_get_thread_num());
    char *s = malloc(length + 1);
    snprintf(s, length + 1, "Thread %d", omp_get_thread_num());

    CHECK_OVERLAP_BEGIN
    MPI_Info_set(info_obj, "Hello", s); /* A */
    CHECK_OVERLAP_END

    free(s);
  }

  MPI_Info_free(&info_obj);
  MPI_Finalize();

  has_error_manifested(overlap_count != 0);

  return 0;
}
