#include "nondeterminism.h"
#include <mpi.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * may lead to a corrupted send operation
 */

#define BUFFER_LENGTH_INT 100
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))
int main(int argc, char *argv[]) {
  int myRank;
  int provided;
  const int requested = MPI_THREAD_MULTIPLE;

  MPI_Init_thread(&argc, &argv, requested, &provided);
  if (provided < requested) {
    has_error_manifested(false);
    exit(EXIT_FAILURE);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  int *buffer = malloc(BUFFER_LENGTH_BYTE);

  if (myRank == 1) {
    fill_message_buffer(buffer, BUFFER_LENGTH_BYTE, 0);

    //printf("Sending Buffer: %X\n", buffer[75]);//for reference

#pragma omp parallel
    {
#pragma omp sections
      {
#pragma omp section
        { MPI_Send(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, 123, MPI_COMM_WORLD); }
#pragma omp section
        {
          // may conflict with the send in line 39
          fill_message_buffer(buffer, BUFFER_LENGTH_BYTE, 3);
        }
      }
    }  // end parallel

  } else {
    fill_message_buffer(buffer, BUFFER_LENGTH_BYTE, 6);
    MPI_Recv(buffer, BUFFER_LENGTH_INT, MPI_INT, 1, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    has_error_manifested(!has_buffer_expected_content(buffer, BUFFER_LENGTH_BYTE, 0));
    //printf("Receive Buffer %X\n", buffer[75]);//for reference
  }

  free(buffer);
  MPI_Finalize();

  return 0;
}
