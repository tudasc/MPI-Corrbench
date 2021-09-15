#include "nondeterminism.h"

#include <mpi.h>
#include <stdlib.h>

#define BUFFER_LENGTH_INT 10
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))

#define NUM_THREADS 2

// Data Race may occur between computing the msg buffer (location A) and the bcast operation (location B)

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

  int send_data[BUFFER_LENGTH_INT];

  fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 2);
  MPI_Request req_bcast;

  if (rank == 0) {
#pragma omp parallel num_threads(NUM_THREADS)
    {
#pragma omp single
      {
#pragma omp task  // fix for data race: depend(out : send_data)
        {
          us_sleep(10);                                           // Data race is very rare otherwise
          fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 6);  // A
        }
#pragma omp task  // fix for data race: depend(in : send_data)
        { MPI_Ibcast(send_data, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD, &req_bcast); }  // B
      }
    }
  } else {
    MPI_Ibcast(send_data, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD, &req_bcast);
  }

  MPI_Wait(&req_bcast, MPI_STATUS_IGNORE);

  const bool error = !has_buffer_expected_content(send_data, BUFFER_LENGTH_BYTE, 6);
  has_error_manifested(error);

  MPI_Finalize();

  return 0;
}
