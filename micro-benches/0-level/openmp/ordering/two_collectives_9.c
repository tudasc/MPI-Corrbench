#include "nondeterminism.h"
#include <mpi.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * the user had to make shure only one collective initialization is active at a time per process and communicator
 * according to the standard (see p303), this can be done with openmp critical
 * this still can result in deadlock as the oerdering is not enforced
 */

#define BUFFER_LENGTH_FLOAT BUFFER_LENGTH_INT
_Static_assert(sizeof(int) == sizeof(float),
               L"Asserts that float and int have the same lenght, so that a missmatch may accidentely occur!");

int main(int argc, char *argv[]) {
  int myRank, size;
  int provided;
  const int requested = MPI_THREAD_MULTIPLE;

  MPI_Init_thread(&argc, &argv, requested, &provided);
  if (provided < requested) {
    has_error_manifested(false);
    exit(EXIT_FAILURE);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const int other_rank = size - myRank - 1;

  int *buffer_int = malloc(BUFFER_LENGTH_BYTE);
  int *buffer_float = malloc(BUFFER_LENGTH_BYTE);

  if (myRank == 0) {
    fill_message_buffer(buffer_int, BUFFER_LENGTH_BYTE, 0);
    fill_message_buffer(buffer_int, BUFFER_LENGTH_BYTE, 2);
  }

#pragma omp parallel num_threads(2)
  {
#pragma omp critical
    MPI_Bcast(buffer_int, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_COMM_WORLD);
#pragma omp critical
    MPI_Bcast(buffer_float, BUFFER_LENGTH_FLOAT, MPI_FLOAT, 0, MPI_COMM_WORLD);
  }  // end parallel

  if (myRank == 1) {
    has_error_manifested(!(has_buffer_expected_content(buffer_int, BUFFER_LENGTH_BYTE, 0) &&
                           has_buffer_expected_content(buffer_int, BUFFER_LENGTH_BYTE, 2)));
  }

  free(buffer_int);
  free(buffer_float);

  MPI_Finalize();

  return 0;
}
