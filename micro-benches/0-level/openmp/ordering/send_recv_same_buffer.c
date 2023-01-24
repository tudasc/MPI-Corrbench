#include "nondeterminism.h"

#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>

// Data race on a buffer: A data-race can occur, as the same buffer is used in sending ( marker "A") and receive (marker
// "B"). This may not strictly be an error according the MPI standard.

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
  int send_data_2[BUFFER_LENGTH_INT];

  fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 1);
  fill_message_buffer(send_data_2, BUFFER_LENGTH_BYTE, 2);

  fill_message_buffer(recv_data, BUFFER_LENGTH_BYTE, 6);

  int has_race = 0;

#pragma omp parallel num_threads(NUM_THREADS)
  {
    if (rank == 0) {
#pragma omp sections
      {
#pragma omp section
        { MPI_Send(send_data, BUFFER_LENGTH_INT, MPI_INT, 1, 0, MPI_COMM_WORLD); /* A */ }
#pragma omp section
        { MPI_Recv(send_data, BUFFER_LENGTH_INT, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /* B */ }
      }

    } else if (rank == 1) {
#pragma omp sections
      {
#pragma omp section
        { MPI_Send(send_data_2, BUFFER_LENGTH_INT, MPI_INT, 0, 0, MPI_COMM_WORLD); }
#pragma omp section
        { MPI_Recv(recv_data, BUFFER_LENGTH_INT, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); }
      }
    }
  }

  if (rank == 1) {
    has_race = !has_buffer_expected_content(recv_data, BUFFER_LENGTH_BYTE, 1) &&
               !has_buffer_expected_content(recv_data, BUFFER_LENGTH_BYTE, 2);
  } else if (rank == 0) {
    has_race = !has_buffer_expected_content(send_data, BUFFER_LENGTH_BYTE, 2);
  }

  int has_error_m = 0;
  MPI_Reduce(&has_race, &has_error_m, 1, MPI_INT, MPI_LOR, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    has_error_manifested(has_error_m);
  }

  MPI_Finalize();
  return 0;
}
