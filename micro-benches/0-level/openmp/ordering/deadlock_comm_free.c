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

  fill_message_buffer(recv_data, BUFFER_LENGTH_BYTE, 1);
  fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 6);

  MPI_Comm other_comm_world;
  MPI_Comm_split(MPI_COMM_WORLD, 0, rank, &other_comm_world);

  int split_rank;
  int split_size;
  MPI_Comm_size(MPI_COMM_WORLD, &split_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &split_rank);

  const int from = split_size - split_rank - 1;
  const int to_rank = (from == 0) ? 1 : 0;

#pragma omp parallel num_threads(NUM_THREADS)
  {
#pragma omp single nowait
    {
      MPI_Sendrecv(send_data, BUFFER_LENGTH_INT, MPI_INT, to_rank, 1, recv_data, BUFFER_LENGTH_INT, MPI_INT, to_rank, 1,
                   other_comm_world, MPI_STATUS_IGNORE);
    }
#pragma omp single
    { MPI_Comm_free(&other_comm_world); }
  }

  MPI_Finalize();

  has_error(false);

  return 0;
}
