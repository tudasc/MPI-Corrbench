#include "nondeterminism.h"

#include <mpi.h>
#include <omp.h>
#include <stdlib.h>

/*
 * Scenario adapted from listing 1.3/1.4:
 * Hoefler T., Bronevetsky G., Barrett B., de Supinski B.R., Lumsdaine A. (2010) Efficient MPI Support for Advanced
 * Hybrid Programming Models. In: Keller R., Gabriel E., Resch M., Dongarra J. (eds) Recent Advances in the Message
 * Passing Interface. EuroMPI 2010. Lecture Notes in Computer Science, vol 6305. Springer, Berlin, Heidelberg.
 * https://doi.org/10.1007/978-3-642-15646-5_6.
 *
 * Matrix here: (0, 1) (0, 2)
 * (source, tag) -> aquire/release
 * (0, 2) -> (lock) tag entry 2
 * (0, *) -> (lock all [tag] entries of 0 - 1 + 2)
 *
 */
//--> if message with tag 2 will get caught in section B, the other probe will never match

// Fix: use Mprobe

#define NUM_THREADS 2

int main(int argc, char *argv[]) {
  int provided;
  const int requested = MPI_THREAD_MULTIPLE;

  MPI_Init_thread(&argc, &argv, requested, &provided);
  if (provided < requested) {
    has_error_manifested(false);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  printf("Use T lvl: %d MULTIPLE:%d\n", provided, requested);

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

#ifndef USE_DISTURBED_THREAD_ORDER
          us_sleep(50);
#endif
          omp_set_lock(&lock);
          MPI_Status status;
          MPI_Message message;
          MPI_Mprobe(0, 2, MPI_COMM_WORLD, &message, &status);

          int local_size = 0;
          MPI_Get_count(&status, MPI_INT, &local_size);
          int *local_buffer = malloc(sizeof(int) * local_size);

          MPI_Mrecv(local_buffer, BUFFER_LENGTH_INT, MPI_INT, &message, MPI_STATUS_IGNORE);
          omp_unset_lock(&lock);

          free(local_buffer);
        }
#pragma omp section
        {
#ifdef USE_DISTURBED_THREAD_ORDER
          us_sleep(50);
#endif
          // B:
          omp_set_lock(&lock);
          MPI_Status status;
          MPI_Message message;
          MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD,'&message, &status);

          int local_size = 0;
          MPI_Get_count(&status, MPI_INT, &local_size);
          int *local_buffer = malloc(sizeof(int) * local_size);

          MPI_Mrecv(local_buffer, BUFFER_LENGTH_INT, MPI_INT, &message, MPI_STATUS_IGNORE);
          omp_unset_lock(&lock);

          MPI_Send(send_data, BUFFER_LENGTH_INT, MPI_INT, 0, 1, MPI_COMM_WORLD);

          free(local_buffer);
        }
      }
    }
  }

#ifdef USE_DISTURBED_THREAD_ORDER
  has_error_manifested(true);  // true is default, so if it deadlocks, it is ok
#else
  has_error_manifested(false);
#endif

  MPI_Finalize();

  return 0;
}
