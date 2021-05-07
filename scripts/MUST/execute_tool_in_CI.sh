#!/bin/bash

TIMEOUT_CMD="/usr/bin/timeout -k 120 120"

# in $(pwd) ist eine datei testcase.c , die ausgewertet werden soll

mpicc -g testcase.c -lm

$TIMEOUT_CMD mustrun -n 2 ./a.out
