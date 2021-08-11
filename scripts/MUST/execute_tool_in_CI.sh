#!/bin/bash

TIMEOUT_CMD="/usr/bin/timeout -k 120 120"

# in $(pwd) will be the file testcase.c , that should be evaluated

mpicc -g -fopenmp testcase.c -lm

$TIMEOUT_CMD mustrun -n 2 --oversubscribe ./a.out
