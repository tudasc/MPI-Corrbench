#!/bin/bash

TIMEOUT_CMD="/usr/bin/timeout -k 120 120"

# in $(pwd) will be the file testcase.c , that should be evaluated

mpiicc -cc=gcc -g -fopenmp testcase.c -lm

mpirun -n 2 -check_mpi -genv VT_CHECK_TRACING on ./a.out 1> output.txt 2>&1

