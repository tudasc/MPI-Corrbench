#include "nondeterminism.h"

#include <mpi.h>
#include <stdlib.h>

// Deadlock: If two processes with one thread each execute this code, it may deadlock.
// P_0 and P_1 both post either the recv (marker "A") or the send (marker "B"), the deadlock occurs.

#define NUM_THREADS 1

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

  int recv_data[BUFFER_LENGTH_INT];
  int send_data[BUFFER_LENGTH_INT];

  fill_message_buffer(recv_data, BUFFER_LENGTH_BYTE, 3);
  fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 6);

#pragma omp parallel num_threads(NUM_THREADS)  // (may) deadlock with one thread
  {
#pragma omp single
    {
#pragma omp task
      {
        MPI_Recv(recv_data, BUFFER_LENGTH_INT, MPI_INT, size - rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /* A */
      }
#pragma omp task
      { MPI_Send(send_data, BUFFER_LENGTH_INT, MPI_INT, size - rank - 1, 1, MPI_COMM_WORLD); /* B */ }
    }
  }

  MPI_Finalize();

  has_error_manifested(false);

  return 0;
}
