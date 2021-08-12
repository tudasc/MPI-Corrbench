#include "nondeterminism.h"

#include <mpi.h>
#include <stdlib.h>

#define BUFFER_LENGTH_INT 8
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

  fill_message_buffer(recv_data, BUFFER_LENGTH_BYTE, 3);
  fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 6);

#pragma omp parallel num_threads(NUM_THREADS)
  {
#pragma omp single
    {
#pragma omp task
      {
        //        printf("T_B_[%i] ", rank);
        MPI_Send(send_data, BUFFER_LENGTH_INT, MPI_INT, size - rank - 1, 1, MPI_COMM_WORLD);
      }
    }
#pragma omp task
    {
      //      printf("T_A_[%i] ", rank);
      MPI_Recv(recv_data, BUFFER_LENGTH_INT, MPI_INT, size - rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }

  MPI_Finalize();

  return 0;
}
