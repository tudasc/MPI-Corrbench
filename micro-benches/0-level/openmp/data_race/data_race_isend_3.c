#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

// This test is loosely based on a unit test of the MUST correctness checker.
// See https://itc.rwth-aachen.de/must/
//
// Data race in parallel region on MPI_Request object:
// Concurrently, the same MPI_Request (marker "A") is shared between
// different threads posting an Isend (marker "B"), and waiting on it at marker "C".

#define NUM_THREADS 8

bool has_error(const int *buffer) {
  for (int i = 0; i < NUM_THREADS; ++i) {
    if (buffer[i] != -1) {
      return true;
    }
  }
  return false;
}

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
  MPI_Request req_send; /* A */

  fill_message_buffer(recv_data, BUFFER_LENGTH_BYTE, 6);
  fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 1);

#pragma omp parallel num_threads(NUM_THREADS)
  {
    DISTURB_THREAD_ORDER
    const int index = omp_get_thread_num();
    MPI_Request req;
    MPI_Irecv(&recv_data[index], 1, MPI_INT, size - rank - 1, index, MPI_COMM_WORLD, &req);

    send_data[index] = -1;

#pragma omp barrier
    // req_send is overwritten by multiple threads:
    MPI_Isend(&send_data[index], 1, MPI_INT, size - rank - 1, index, MPI_COMM_WORLD, &req_send); /* B */

    MPI_Wait(&req_send, MPI_STATUS_IGNORE); /* C */
    MPI_Wait(&req, MPI_STATUS_IGNORE);
  }

  const bool error = has_error(recv_data);
  has_error_manifested(error);

  MPI_Finalize();

  return 0;
}
