#include "nondeterminism.h"

#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUFFER_LENGTH_INT 100
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))

#define NUM_THREADS 2

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
        {
          //          us_sleep(3);
          MPI_Send(send_data, BUFFER_LENGTH_INT, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
#pragma omp section
        { MPI_Recv(send_data, BUFFER_LENGTH_INT, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); }
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
    has_race = !has_buffer_expected_content(recv_data, BUFFER_LENGTH_BYTE, 1) ||
               !has_buffer_expected_content(recv_data, BUFFER_LENGTH_BYTE, 2);
    if (has_race) {
      printf("Error p1\n");
    }
  } else if (rank == 0) {
    has_race = !has_buffer_expected_content(send_data, BUFFER_LENGTH_BYTE, 2);
    if (has_race) {
      printf("Error p0\n");
    }
  }

  int has_error_m = 0;
  MPI_Reduce(&has_race, &has_error_m, 1, MPI_INT, MPI_LOR, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    if (has_error_m) {
      printf("Error all\n");
    }
    has_error_manifested(has_error_m);
  }

  MPI_Finalize();
  return 0;
}
