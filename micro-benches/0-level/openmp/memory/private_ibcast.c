#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

// A private variable (uninitialized in parallel region, marker "A") may cause undefined behavior when broadcasted
// (marker "B") (and used in a different process).

#define NUM_THREADS 2

int main(int argc, char *argv[]) {
  int provided;
  const int requested = MPI_THREAD_FUNNELED;

  MPI_Init_thread(&argc, &argv, requested, &provided);
  if (provided < requested) {
    has_error_manifested(false);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  int size;
  int rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Request req;
  int private_data = rank;

#pragma omp parallel num_threads(NUM_THREADS) default(shared) private(private_data) /* A */
  {
#pragma omp master
    {
      MPI_Ibcast(&private_data, 1, MPI_INT, 0, MPI_COMM_WORLD, &req); /* B */
      MPI_Wait(&req, MPI_STATUS_IGNORE);
    }
  }

  if (rank == 1) {
    has_error_manifested(private_data != 0);
  }

  has_error_manifested(true);

  MPI_Finalize();

  return 0;
}
