#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
/*
 * MPI_Irecv fails in 10th. iteration due to mismatching tags, line 48
 *
 */

#define MSG_TAG_A 124523
#define N 10
#define EVEN 0

int determineReceiverTag(int index, int tag, int *countEvenNumbers) {
  if (index % 2 == EVEN) {
    (*countEvenNumbers)++;
  }

  if ((*countEvenNumbers) == (N / 2)) {
    return tag + 1;
  }

  return tag;
}

int main(int argc, char *argv[]) {
  int rank, numProcs;
  int buffer[N];
  int i;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Request request;
  MPI_Status status;

  int countEvenNumbers = 0;

  for (i = 0; i < 10; i++) {
    if (rank == 0) {
      int tag_sender = i * N;
      MPI_Isend(buffer, 1, MPI_INT, 1, tag_sender, MPI_COMM_WORLD, &request);
      MPI_Wait(&request, &status);
    }

    else if (rank == 1) {
      int tag_receiver = determineReceiverTag(i, i * N, &countEvenNumbers);
      printf("Count Even Numbers: %d \n", countEvenNumbers);
      MPI_Irecv(buffer, 1, MPI_INT, 0, tag_receiver, MPI_COMM_WORLD, &request);
      MPI_Wait(&request, &status);
    }
  }

  MPI_Finalize();

  return 0;
}
