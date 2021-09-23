#include "nondeterminism.h"

#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>

// Using conflicting MPI calls for derived datatype:
// MPI_Send (marker "A") with a derived datatype may happen after MPI_Type_free on the same datatype (marker "B"), which
// is errorneous.

#define BUFFER_LENGTH_INT 2
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

  int recv_data[BUFFER_LENGTH_INT];
  int send_data[BUFFER_LENGTH_INT];
  MPI_Request req;

  MPI_Datatype mult_ints_dtype;
  MPI_Type_contiguous(BUFFER_LENGTH_INT, MPI_INT, &mult_ints_dtype);
  MPI_Type_commit(&mult_ints_dtype);

  fill_message_buffer(recv_data, BUFFER_LENGTH_BYTE, 6);
  fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 1);

  MPI_Irecv(recv_data, 1, mult_ints_dtype, size - rank - 1, 1, MPI_COMM_WORLD, &req);

#pragma omp parallel num_threads(NUM_THREADS)
  {
#pragma omp single nowait
    {
      us_sleep(1);
      MPI_Send(send_data, 1, mult_ints_dtype, size - rank - 1, 1, MPI_COMM_WORLD); /* A */
    }
#pragma omp single
    { MPI_Type_free(&mult_ints_dtype); /* B */ }
  }

  MPI_Wait(&req, MPI_STATUS_IGNORE);

  MPI_Finalize();

  has_error_manifested(true);

  return 0;
}
