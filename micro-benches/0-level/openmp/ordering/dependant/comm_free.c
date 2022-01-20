#include "nondeterminism.h"

#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>

// Using a freed communicator is not allowed:
// Before a thread begins the sendrecv (marker "A"), due to the nowait, another thread may execute comm_free
// on the communicator (marker "B").
// Free'ing the communicator before the sendrecv started is erroneous.

#define NUM_THREADS 8

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

  printf("BEFORE: %i,%i,%i,%i\n", before_a, after_a, before_b, after_b);

  int recv_data[BUFFER_LENGTH_INT];
  int send_data[BUFFER_LENGTH_INT];

  fill_message_buffer(recv_data, BUFFER_LENGTH_BYTE, 1);
  fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 6);

  MPI_Comm other_comm_world;
  MPI_Comm_dup(MPI_COMM_WORLD, &other_comm_world);

  int split_rank;
  int split_size;
  MPI_Comm_size(other_comm_world, &split_size);
  MPI_Comm_rank(other_comm_world, &split_rank);

  const int to_rank = (split_rank == 0) ? 1 : 0;

#pragma omp parallel num_threads(NUM_THREADS)
  {
#pragma omp single nowait
    {
#ifdef USE_DISTURBED_THREAD_ORDER
      us_sleep(20);
#endif
      ENTER_CALL_A
      MPI_Sendrecv(send_data, BUFFER_LENGTH_INT, MPI_INT, to_rank, 1, recv_data, BUFFER_LENGTH_INT, MPI_INT, to_rank, 1,
                   other_comm_world, MPI_STATUS_IGNORE); /* A */
      EXIT_CALL_A
    }

#pragma omp single
    {
#ifndef USE_DISTURBED_THREAD_ORDER
      us_sleep(20);
#endif
      ENTER_CALL_B
      MPI_Comm_free(&other_comm_world); /* B */
      EXIT_CALL_B
    }
  }

  has_error_manifested(!CHECK_FOR_EXPECTED_ORDER);

  MPI_Finalize();
  return 0;
}
