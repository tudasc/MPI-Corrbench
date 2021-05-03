#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * A missed MPI_Wait between lines 13 and 14: loss of request information in
 * MPI communication.
 */
int main(int argc, char *argv[]) {
  int myRank;

  int data = 4;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  MPI_Request req;
  MPI_Status stat;

  MPI_Ibcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD, &req);
  MPI_Ibcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD, &req);
  MPI_Wait(&req, &stat);

  MPI_Finalize();

  return 0;
}
