#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

// firstprivate is initialized

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

  int private_data = rank;

#pragma omp parallel num_threads(NUM_THREADS) default(shared) firstprivate(private_data) /* A */
  {
#pragma omp master
    { MPI_Bcast(&private_data, 1, MPI_INT, 0, MPI_COMM_WORLD); /* B */ }
  }

  if (rank == 1) {
    has_error_manifested(private_data != 0);
  }

  has_error_manifested(true);

  MPI_Finalize();

  return 0;
}
