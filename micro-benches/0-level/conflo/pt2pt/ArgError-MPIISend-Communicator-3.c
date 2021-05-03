#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define MSG_TAG_A 124
#define MSG_TAG_B 1234
#define N 1000
#define N2 1204

/*
 * Illegal rank. Processes are in different communicators. line 29/32
 */
int main(int argc, char **argv) {
  int buffer[N];
  int buffer2[N2];

  MPI_Init(&argc, &argv);
  int rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int color = 1;
  if (argc == 1) {
    color = rank / 1;
  } else {
    color = 1;
  }

  MPI_Comm row_comm;
  MPI_Comm_split(MPI_COMM_WORLD, color, rank, &row_comm);
  MPI_Request request;
  MPI_Status status;

  if (rank == 0) {
    MPI_Isend(buffer, 1, MPI_INT, 1, MSG_TAG_A, row_comm, &request);
    MPI_Wait(&request, &status);
  } else if (rank == 1) {
    MPI_Irecv(buffer2, N2, MPI_INT, 0, MSG_TAG_A, row_comm, &request);
    MPI_Wait(&request, &status);
  }

  MPI_Finalize();
  return 0;
}
