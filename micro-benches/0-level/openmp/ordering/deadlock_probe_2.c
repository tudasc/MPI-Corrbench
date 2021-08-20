#include "nondeterminism.h"

#include <mpi.h>
#include <stdlib.h>

#define BUFFER_LENGTH_INT 2
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))

#define NUM_THREADS 2

// From: Fixing Probe for Multi-Threaded MPI Applications Fixing Probe for Multi-Threaded MPI Applications
// (Revision 4): http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.568.6739
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

  if (rank == 0) {
    int value = -1;
    MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
  } else if (rank == 1) {
#pragma omp parallel num_threads(NUM_THREADS)
    {
      MPI_Status status;
      int value;
      MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&value, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }

  MPI_Finalize();

  has_error_manifested(false);

  return 0;
}
