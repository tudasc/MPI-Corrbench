#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUFFER_LENGTH_INT 300
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))

#define NUM_THREADS 3  // with less than 3, error occurs rarely

bool has_error(const int *buffer) {
  for (int i = 0; i < NUM_THREADS; ++i) {
    if (buffer[i] == -1) {
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
  MPI_Request req;
  MPI_Request req_master;

  fill_message_buffer(recv_data, BUFFER_LENGTH_BYTE, 6);
  fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 1);

  MPI_Irecv(recv_data, BUFFER_LENGTH_INT, MPI_INT, size - rank - 1, 1, MPI_COMM_WORLD, &req);

#pragma omp parallel num_threads(NUM_THREADS)
  {
#pragma omp master
    { MPI_Isend(send_data, BUFFER_LENGTH_INT, MPI_INT, size - rank - 1, 1, MPI_COMM_WORLD, &req_master); }
    // data race fixed by adding in above master section an MPI_WAIT
    send_data[omp_get_thread_num()] = -1;
  }

  MPI_Wait(&req_master, MPI_STATUS_IGNORE);  // misplaced, causes data race
  MPI_Wait(&req, MPI_STATUS_IGNORE);

  const bool error = has_error(recv_data);
  has_error_manifested(error);
  //  if (error) {
  //    printf("Has the error.\n");
  //    for (int i = 0; i < NUM_THREADS; ++i) {
  //      printf("%i, ", recv_data[i]);
  //    }
  //    printf("\n");
  //  }

  MPI_Finalize();

  return 0;
}
