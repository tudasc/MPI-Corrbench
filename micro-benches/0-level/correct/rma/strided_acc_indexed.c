/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

/* One-Sided MPI 2-D Strided Accumulate Test
 *
 * Author: James Dinan <dinan@mcs.anl.gov>
 * Date  : December, 2010
 *
 * This code performs N accumulates into a 2d patch of a shared array.  The
 * array has dimensions [X, Y] and the subarray has dimensions [SUB_X, SUB_Y]
 * and begins at index [0, 0].  The input and output buffers are specified
 * using an MPI indexed type.
 */

#include "mpitest.h"
#include "squelch.h"
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define XDIM 16
#define YDIM 16
#define SUB_XDIM 8
#define SUB_YDIM 8
#define ITERATIONS 1

int main(int argc, char **argv) {
  int itr, i, j, rank, nranks, peer, bufsize, errors;
  double *win_buf, *src_buf;
  MPI_Win buf_win;

  MTest_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nranks);

  bufsize = XDIM * YDIM * sizeof(double);
  MPI_Alloc_mem(bufsize, MPI_INFO_NULL, &win_buf);
  MPI_Alloc_mem(bufsize, MPI_INFO_NULL, &src_buf);

  for (i = 0; i < XDIM * YDIM; i++) {
    *(win_buf + i) = -1.0;
    *(src_buf + i) = 1.0 + rank;
  }

  MPI_Win_create(win_buf, bufsize, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &buf_win);

  peer = (rank + 1) % nranks;

  /* Perform ITERATIONS strided accumulate operations */

  for (itr = 0; itr < ITERATIONS; itr++) {
    MPI_Aint idx_loc[SUB_YDIM];
    int idx_rem[SUB_YDIM];
    int blk_len[SUB_YDIM];
    MPI_Datatype src_type, dst_type;

    for (i = 0; i < SUB_YDIM; i++) {
      MPI_Get_address(&src_buf[i * XDIM], &idx_loc[i]);
      idx_rem[i] = i * XDIM;
      blk_len[i] = SUB_XDIM;
    }

#ifdef ABSOLUTE
    MPI_Type_hindexed(SUB_YDIM, blk_len, idx_loc, MPI_DOUBLE, &src_type);
#else
    MPI_Type_indexed(SUB_YDIM, blk_len, idx_rem, MPI_DOUBLE, &src_type);
#endif
    MPI_Type_indexed(SUB_YDIM, blk_len, idx_rem, MPI_DOUBLE, &dst_type);

    MPI_Type_commit(&src_type);
    MPI_Type_commit(&dst_type);

    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, peer, 0, buf_win);

#ifdef ABSOLUTE
    MPI_Accumulate(MPI_BOTTOM, 1, src_type, peer, 0, 1, dst_type, MPI_SUM, buf_win);
#else
    MPI_Accumulate(src_buf, 1, src_type, peer, 0, 1, dst_type, MPI_SUM, buf_win);
#endif

    MPI_Win_unlock(peer, buf_win);

    MPI_Type_free(&src_type);
    MPI_Type_free(&dst_type);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  /* Verify that the results are correct */

  MPI_Win_lock(MPI_LOCK_EXCLUSIVE, rank, 0, buf_win);
  errors = 0;
  for (i = 0; i < SUB_XDIM; i++) {
    for (j = 0; j < SUB_YDIM; j++) {
      const double actual = *(win_buf + i + j * XDIM);
      const double expected = -1.0 + (1.0 + ((rank + nranks - 1) % nranks)) * (ITERATIONS);
      if (fabs(actual - expected) > 1.0e-10) {
        SQUELCH(
            printf("%d: Data validation failed at [%d, %d] expected=%f actual=%f\n", rank, j, i, expected, actual););
        errors++;
        fflush(stdout);
      }
    }
  }
  for (i = SUB_XDIM; i < XDIM; i++) {
    for (j = 0; j < SUB_YDIM; j++) {
      const double actual = *(win_buf + i + j * XDIM);
      const double expected = -1.0;
      if (fabs(actual - expected) > 1.0e-10) {
        SQUELCH(
            printf("%d: Data validation failed at [%d, %d] expected=%f actual=%f\n", rank, j, i, expected, actual););
        errors++;
        fflush(stdout);
      }
    }
  }
  for (i = 0; i < XDIM; i++) {
    for (j = SUB_YDIM; j < YDIM; j++) {
      const double actual = *(win_buf + i + j * XDIM);
      const double expected = -1.0;
      if (fabs(actual - expected) > 1.0e-10) {
        SQUELCH(
            printf("%d: Data validation failed at [%d, %d] expected=%f actual=%f\n", rank, j, i, expected, actual););
        errors++;
        fflush(stdout);
      }
    }
  }
  MPI_Win_unlock(rank, buf_win);

  MPI_Win_free(&buf_win);
  MPI_Free_mem(win_buf);
  MPI_Free_mem(src_buf);

  MTest_Finalize(errors);
  return MTestReturnValue(errors);
}
