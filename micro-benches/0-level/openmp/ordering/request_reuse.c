#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

// Multiple Threads use the same Request Object for non-blocking communication

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

  int recv_data[BUFFER_LENGTH_INT];
  int send_data[BUFFER_LENGTH_INT];
  MPI_Request req;
  MPI_Request req_send;

  fill_message_buffer(recv_data, BUFFER_LENGTH_BYTE, 6);
  fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 1);

#pragma omp parallel num_threads(NUM_THREADS)
  {
    const int index = omp_get_thread_num();
    MPI_Irecv(&recv_data[index], 1, MPI_INT, size - rank - 1, index, MPI_COMM_WORLD, &req);

    send_data[index] = -1;

#pragma omp barrier
    MPI_Isend(&send_data[index], 1, MPI_INT, size - rank - 1, index, MPI_COMM_WORLD, &req_send);

    MPI_Wait(&req_send, MPI_STATUS_IGNORE);
    MPI_Wait(&req, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();

  has_error_manifested(false);

  return 0;
}
