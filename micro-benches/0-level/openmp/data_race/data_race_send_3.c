#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUFFER_LENGTH_INT 100
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))

#define NUM_THREADS 3

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

  //  for (int i = 0; i < NUM_THREADS; ++i) {
  //    printf("%i, ", recv_data[i]);
  //  }
  //  printf("\n");

#pragma omp parallel num_threads(NUM_THREADS)  // likely race with >= 3 threads
  {
    send_data[omp_get_thread_num()] = -1;

#pragma omp single nowait
    { MPI_Barrier(MPI_COMM_WORLD); }  // nowait allows other thread to reach MPI_Send, while Barrier is executed
#pragma omp single
    { MPI_Send(send_data, BUFFER_LENGTH_INT, MPI_INT, size - rank - 1, 1, MPI_COMM_WORLD); }
  }

  MPI_Wait(&req, MPI_STATUS_IGNORE);

  const bool error = has_error(recv_data);
  has_error_manifested(error);
  //  if (error) {
  //    printf("Has the error.\n");
  //    for (int i = 0; i < NUM_THREADS; ++i) {
  //      printf("%i, ", recv_data[i]);
  //    }
  //    printf("\n");
  //  }

  MPI_Finalize();

  return 0;
}
