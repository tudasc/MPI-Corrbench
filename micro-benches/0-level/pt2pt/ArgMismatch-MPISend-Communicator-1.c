#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

#define MSG_TAG_A 124
#define MSG_TAG_B 1234
#define N 1000
#define N2 1204

/*
 * Illegal rank in mpi send, different communicators. (line 27)
 */
int main(int argc, char **argv) {
  int buffer[N];
  int buffer2[N2];

  MPI_Init(&argc, &argv);
  int rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int color = rank / 1;  // Determine color based on row

  MPI_Comm row_comm;
  MPI_Comm_split(MPI_COMM_WORLD, color, rank, &row_comm);

  if (rank == 0) {
    MPI_Send(buffer, 1, MPI_INT, 1, MSG_TAG_A, row_comm);
  } else if (rank == 1) {
    MPI_Recv(buffer2, N2, MPI_INT, 0, MSG_TAG_A, row_comm, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();
  return 0;
}
