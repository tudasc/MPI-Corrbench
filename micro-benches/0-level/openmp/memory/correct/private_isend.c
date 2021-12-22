#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

// firstprivate is initialized

#define BUFFER_LENGTH_INT 1
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))

#define NUM_THREADS 2

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
  fill_message_buffer(recv_data, BUFFER_LENGTH_BYTE, 1);

  if (rank == 0) {
    MPI_Request send_req;
    int private_data = rank;

#pragma omp parallel num_threads(NUM_THREADS) default(shared) firstprivate(private_data) /* A */
    {
#pragma omp master
      {
        MPI_Isend(&private_data, BUFFER_LENGTH_INT, MPI_INT, size - rank - 1, 1, MPI_COMM_WORLD, &send_req); /* B */
        MPI_Wait(&send_req, MPI_STATUS_IGNORE);
      }
    }

  } else if (rank == 1) {
    MPI_Recv(recv_data, BUFFER_LENGTH_INT, MPI_INT, size - rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    has_error_manifested(recv_data[0] != 0);
  }

  MPI_Finalize();

  return 0;
}
