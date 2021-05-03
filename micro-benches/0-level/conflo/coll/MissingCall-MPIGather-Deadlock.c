#include <mpi.h>
#include <stddef.h>
#include <stdio.h>

/*
 * Deadlock occurs if MPI_GATHER is not executed by all
 * Processes(line 40)
 */
int main(int argc, char *argv[]) {
  int myRank;

  int rand_nums[4];
  int num_elements_per_proc = 4;

  int ROOT_PROCESS = 0;
  int sub_adds[2];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  // Scatter the random numbers from the root process to all processes in
  // the MPI world

  if (myRank == 0) {
    rand_nums[0] = 1;
    rand_nums[1] = 2;
    rand_nums[2] = 3;
    rand_nums[3] = 4;
  }

  MPI_Bcast(&rand_nums, num_elements_per_proc, MPI_INT, ROOT_PROCESS, MPI_COMM_WORLD);

  int sub_add = rand_nums[0] + rand_nums[1] + rand_nums[2] + rand_nums[3];

  if (myRank == 0) {
    printf("Root Process");
    MPI_Gather(&sub_add, 1, MPI_FLOAT, sub_adds, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    int sub_adds_int = sub_adds[0] + sub_adds[1];

    printf("%d", sub_adds_int);
  }

  MPI_Finalize();

  return 0;
}
