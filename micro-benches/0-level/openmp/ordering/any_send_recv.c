#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUFFER_LENGTH_INT 100
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))

#define NUM_THREADS 2

bool has_error(int *buffer) {
  for (int i = 0; i < NUM_THREADS; ++i) {
    if (buffer[i] != i) {
      return true;
    }
  }
  return false;
}

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
  int send_data[BUFFER_LENGTH_INT];

  fill_message_buffer(recv_data, BUFFER_LENGTH_BYTE, 6);
  fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 1);

#pragma omp parallel num_threads(NUM_THREADS)
  {
    const int thread_num = omp_get_thread_num();
    send_data[thread_num] = thread_num;

#pragma omp barrier
    if (rank == 0) {
      MPI_Send(&send_data[thread_num], 1, MPI_INT, size - rank - 1, thread_num, MPI_COMM_WORLD);
    } else if (rank == 1) {
      // Any tag leads to "random" ordering
      MPI_Recv(&recv_data[thread_num], 1, MPI_INT, size - rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }

  const bool error = has_error(recv_data);
  has_error_manifested(error);

  MPI_Finalize();

  return 0;
}
