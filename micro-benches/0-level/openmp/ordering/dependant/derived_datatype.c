#include "nondeterminism.h"

#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>

// Using conflicting MPI calls for derived datatype:
// MPI_Send (marker "A") with a derived datatype may happen after MPI_Type_free on the same datatype (marker "B"), which
// is errorneous.

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

  DEF_ORDER_CAPTURING_VARIABLES

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
#ifdef USE_DISTURBED_THREAD_ORDER
      us_sleep(10);
#endif
      ENTER_CALL_A
      MPI_Send(send_data, 1, mult_ints_dtype, size - rank - 1, 1, MPI_COMM_WORLD); /* A */
      EXIT_CALL_A
    }
#pragma omp single
    {
#ifndef USE_DISTURBED_THREAD_ORDER
      us_sleep(10);
#endif
      ENTER_CALL_B
      MPI_Type_free(&mult_ints_dtype); /* B */
      EXIT_CALL_B
    }
  }

  MPI_Wait(&req, MPI_STATUS_IGNORE);

  has_error_manifested(!CHECK_FOR_EXPECTED_ORDER);

  MPI_Finalize();

  return 0;
}
