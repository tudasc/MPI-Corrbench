/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *
 *  (C) 2003 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
#include "mpi.h"
#include "mpitest.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef MAX_INT
#define MAX_INT 0x7fffffff
#endif

/*
static char MTEST_Descrip[] = "Test MPI_Accumulate with fence";
*/

/* same as accfence2.c, but uses alloc_mem */

int main(int argc, char *argv[]) {
  int errs = 0;
  int rank, size, source;
  int minsize = 2, count, i;
  MPI_Comm comm;
  MPI_Win win;
  int *winbuf, *sbuf;

  MTest_Init(&argc, &argv);

  /* The following illustrates the use of the routines to
   * run through a selection of communicators and datatypes.
   * Use subsets of these for tests that do not involve combinations
   * of communicators, datatypes, and counts of datatypes */
  while (MTestGetIntracommGeneral(&comm, minsize, 1)) {
    if (comm == MPI_COMM_NULL)
      continue;
    /* Determine the sender and receiver */
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    source = 0;

    for (count = 32768; count < 65000; count = count * 2) {
      /* We compare with an integer value that can be as large as
       * size * (count * count + (1/2)*(size-1))
       * For large machines (size large), this can exceed the
       * maximum integer for some large values of count.  We check
       * that in advance and break this loop if the above value
       * would exceed MAX_INT.  Specifically,
       *
       * size*count*count + (1/2)*size*(size-1) > MAX_INT
       * count*count > (MAX_INT/size - (1/2)*(size-1))
       */
      if (count * count > (MAX_INT / size - (size - 1) / 2))
        break;

      MPI_Alloc_mem(count * sizeof(int), MPI_INFO_NULL, &winbuf);
      MPI_Alloc_mem(count * sizeof(int), MPI_INFO_NULL, &sbuf);

      for (i = 0; i < count; i++)
        winbuf[i] = 0;
      for (i = 0; i < count; i++)
        sbuf[i] = rank + i * count;
      MPI_Win_create(winbuf, count * sizeof(int), sizeof(int), MPI_INFO_NULL, comm, &win);
      MPI_Win_fence(0, win);
      MPI_Accumulate(sbuf, count, MPI_INT, source, 0, count, MPI_INT, MPI_SUM, win);
      MPI_Win_fence(0, win);
      if (rank == source) {
        /* Check the results */
        for (i = 0; i < count; i++) {
          int result = i * count * size + (size * (size - 1)) / 2;
          if (winbuf[i] != result) {
            if (errs < 10) {
              fprintf(stderr, "Winbuf[%d] = %d, expected %d (count = %d, size = %d)\n", i, winbuf[i], result, count,
                      size);
            }
            errs++;
          }
        }
      }

      MPI_Win_free(&win);

      MPI_Free_mem(winbuf);
      MPI_Free_mem(sbuf);
    }
    MTestFreeComm(&comm);
  }

  MTest_Finalize(errs);
  return MTestReturnValue(errs);
}
