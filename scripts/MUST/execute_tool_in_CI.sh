#!/bin/bash

TIMEOUT_CMD="/usr/bin/timeout -k 120 120"


export MPICC=mpicc
export I_MPI_CC=clang
export I_MPI_CXX=clang++

# in $(pwd) will be the file testcase.c , that should be evaluated

$MPICC $CORRBENCH_CFLAGS -g -fopenmp testcase.c -fsanitize=thread -lm

$TIMEOUT_CMD mustrun -n 2 --must:distributed --must:hybrid --oversubscribe ./a.out
