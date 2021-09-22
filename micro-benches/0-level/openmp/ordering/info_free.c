#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

// Using conflicting MPI calls on MPI_Info:
// Setting MPI_Info (marker "A") may happen concurrently with MPI_Info_free (marker "B").
// Idea taken from: "Thread-safety in an MPI implementation: Requirements and analysis" by
// W. Gropp and R. Thakur. DOI: 10.1016/j.parco.2007.07.002

#define BUFFER_LENGTH_INT 10000
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

  MPI_Info info_obj;
  MPI_Info_create(&info_obj);

#pragma omp parallel num_threads(NUM_THREADS)
  {
#pragma omp single nowait
    { MPI_Info_set(info_obj, "Hello", "World"); /* A */ }
#pragma omp single
    { MPI_Info_free(&info_obj); /* B */ }
  }

  MPI_Finalize();

  has_error_manifested(true);

  return 0;
}
