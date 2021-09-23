#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

// This test is loosely based on a unit test of the MUST correctness checker.
// See https://itc.rwth-aachen.de/must/
// Data race in parallel region on buffer: Concurrently, (1) buffer is written to
// (marker "A"), and (b) without any thread-synchronization ("omp barrier") passed to a
// send operation (marker "B").
// Note: The MPI_Barrier is not sufficient to protect against the race (marker "C").

#define BUFFER_LENGTH_INT 100
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))

#define NUM_THREADS 3

// Data Race may occur between computing the msg buffer (location A) and the send operation (location B)

bool has_error(const int *buffer) {
  for (int i = 0; i < NUM_THREADS; ++i) {
    if (buffer[i] != -1) {
      return true;
    }
  }
  return false;
}

// This test is loosely based on a unit test of the MUST correctness checker.
// See https://itc.rwth-aachen.de/must/
int main(int argc, char *argv[]) {
  int provided;
  const int requested = MPI_THREAD_SERIALIZED;

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
  MPI_Request req;

  fill_message_buffer(recv_data, BUFFER_LENGTH_BYTE, 6);
  fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 1);

  MPI_Irecv(recv_data, BUFFER_LENGTH_INT, MPI_INT, size - rank - 1, 1, MPI_COMM_WORLD, &req);

#pragma omp parallel num_threads(NUM_THREADS)  // likely race with >= 3 threads
  {
    send_data[omp_get_thread_num()] = -1; /* A */

#pragma omp single nowait
    { MPI_Barrier(MPI_COMM_WORLD); /* C */ }  // nowait allows other thread to reach MPI_Send, while Barrier is executed
#pragma omp single
    { MPI_Send(send_data, BUFFER_LENGTH_INT, MPI_INT, size - rank - 1, 1, MPI_COMM_WORLD); /* B */ }
  }

  MPI_Wait(&req, MPI_STATUS_IGNORE);

  const bool error = has_error(recv_data);
  has_error_manifested(error);

  MPI_Finalize();

  return 0;
}
