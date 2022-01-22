#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

// A private variable (uninitialized in parallel region, marker "A") may cause undefined behavior when send (marker "B")
// (and used in a different process).

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

  int recv_data[BUFFER_LENGTH_INT];
  fill_message_buffer(recv_data, BUFFER_LENGTH_BYTE, 1);

  if (rank == 0) {
    int private_data = rank;

#pragma omp parallel num_threads(NUM_THREADS) default(shared) private(private_data) /* A */
    {
#pragma omp master
      { MPI_Send(&private_data, 1, MPI_INT, size - rank - 1, 1, MPI_COMM_WORLD); /* B */ }
    }
  } else if (rank == 1) {
    MPI_Recv(recv_data, 1, MPI_INT, size - rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    has_error_manifested(recv_data[0] != 0);
  }

  MPI_Finalize();

  return 0;
}
