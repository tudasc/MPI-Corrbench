#include "nondeterminism.h"

#include <mpi.h>
#include <stdlib.h>

// Data race in parallel region for Process 1 using omp sections:
// Concurrently, (1) buffer is written to (marker "A"), and (2) passed to a Isend (marker "B")
// leading to data race on the buffer.

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

    // printf("Sending Buffer: %X\n", buffer[75]);//for reference

#pragma omp parallel
    {
#pragma omp sections
      {
#pragma omp section
        { MPI_Send(buffer, BUFFER_LENGTH_INT, MPI_INT, 0, 123, MPI_COMM_WORLD); /* B */ }
#pragma omp section
        {
          us_sleep(10);                                       // make data race more likely
          fill_message_buffer(buffer, BUFFER_LENGTH_BYTE, 3); /* A */
        }
      }
    }  // end parallel

  } else {
    fill_message_buffer(buffer, BUFFER_LENGTH_BYTE, 6);
    MPI_Recv(buffer, BUFFER_LENGTH_INT, MPI_INT, 1, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // no ordering of modification either buffer is fine, as long as it is not corrupted
    has_error_manifested(!(has_buffer_expected_content(buffer, BUFFER_LENGTH_BYTE, 0) ||
                           !has_buffer_expected_content(buffer, BUFFER_LENGTH_BYTE, 3)));
    // printf("Receive Buffer %X\n", buffer[75]);//for reference
    // printf("Has Buffer content of thread 0 (before modification)? %d\n",has_buffer_expected_content(buffer,
    // BUFFER_LENGTH_BYTE, 0)); printf("Has Buffer content of thread 1 (after modification)?
    // %d\n",has_buffer_expected_content(buffer, BUFFER_LENGTH_BYTE, 3));
  }

  free(buffer);
  MPI_Finalize();

  return 0;
}
