#include "nondeterminism.h"

#include <mpi.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_LENGTH_INT 10
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

  int send_data[BUFFER_LENGTH_INT];

  fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 2);

  if (rank == 0) {
#pragma omp parallel num_threads(NUM_THREADS)
    {
#pragma omp single
      {
#pragma omp task  // fix for data race: depend(out : send_data)
        {
          us_sleep(10);  // Data race is very rare otherwise
          fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 6);
        }
#pragma omp task  // fix for data race: depend(in : send_data)
        { MPI_Bcast(send_data, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD); }
      }
    }
  } else {
    MPI_Bcast(send_data, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD);
  }

  const bool error = !has_buffer_expected_content(send_data, BUFFER_LENGTH_BYTE, 6);
  has_error_manifested(error);
  if (error) {
    printf("Has error\n");
  }

  MPI_Finalize();

  return 0;
}
