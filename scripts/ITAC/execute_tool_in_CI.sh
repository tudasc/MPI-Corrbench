#!/bin/bash

TIMEOUT_CMD="/usr/bin/timeout -k 120 120"

# in $(pwd) will be the file testcase.c , that should be evaluated

export MPICC=mpicc
export I_MPI_CC=clang
export I_MPI_CXX=clang++

mpiicc -cc=gcc $CORRBENCH_CFLAGS -g -fopenmp testcase.c -lm

$TIMEOUT_CMD mpirun -n 2 -check_mpi -genv VT_CHECK_TRACING on ./a.out 1> output.txt 2>&1

