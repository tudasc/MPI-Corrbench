#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdlib.h>

#define BUFFER_LENGTH_INT 1
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))

#define NUM_THREADS 2

/*
 * Scenario adapted from listing 1.3/1.4:
 * Hoefler T., Bronevetsky G., Barrett B., de Supinski B.R., Lumsdaine A. (2010) Efficient MPI Support for Advanced
 * Hybrid Programming Models. In: Keller R., Gabriel E., Resch M., Dongarra J. (eds) Recent Advances in the Message
 * Passing Interface. EuroMPI 2010. Lecture Notes in Computer Science, vol 6305. Springer, Berlin, Heidelberg.
 * https://doi.org/10.1007/978-3-642-15646-5_6.
 */

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
  fill_message_buffer(recv_data, BUFFER_LENGTH_BYTE, 6);
  fill_message_buffer(send_data, BUFFER_LENGTH_BYTE, 1);
  fill_message_buffer(send_data_2, BUFFER_LENGTH_BYTE, 2);

  if (rank == 0) {
    MPI_Send(send_data, BUFFER_LENGTH_INT, MPI_INT, 1, 1, MPI_COMM_WORLD);
    MPI_Recv(recv_data, BUFFER_LENGTH_INT, MPI_INT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(send_data_2, BUFFER_LENGTH_INT, MPI_INT, 1, 2, MPI_COMM_WORLD);
  } else if (rank == 1) {
    omp_lock_t lock;
    omp_init_lock(&lock);
#pragma omp parallel num_threads(NUM_THREADS)
    {
#pragma omp sections
      {
#pragma omp section
        {
          // A:
          // usleep(50) fixes ordering
          omp_set_lock(&lock);
          MPI_Status status;
          MPI_Probe(0, 2, MPI_COMM_WORLD, &status);

          int local_size = 0;
          MPI_Get_count(&status, MPI_INT, &local_size);
          int *local_buffer = malloc(sizeof(int) * local_size);

          MPI_Recv(local_buffer, BUFFER_LENGTH_INT, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

          free(local_buffer);
          omp_unset_lock(&lock);
        }
#pragma omp section
        {
          // B:
          omp_set_lock(&lock);
          MPI_Status status;
          MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

          int local_size = 0;
          MPI_Get_count(&status, MPI_INT, &local_size);
          int *local_buffer = malloc(sizeof(int) * local_size);

          MPI_Recv(local_buffer, BUFFER_LENGTH_INT, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

          MPI_Send(send_data, BUFFER_LENGTH_INT, MPI_INT, 0, 1, MPI_COMM_WORLD);

          free(local_buffer);
          omp_unset_lock(&lock);
        }
      }
    }
  }
  MPI_Finalize();

  has_error_manifested(false);

  return 0;
}
