#!/bin/bash

TIMEOUT_CMD="/usr/bin/timeout -k 120 120"

mpicc --showme:compile

clang -DEXCLUDE_PARCOACH_UNSUPPORTED_FUNCTIONS $(mpicc --showme:compile) -c -g -emit-llvm testcase.c -o testcase.bc

$TIMEOUT_CMD opt-9 -load $PARCOACH_ROOT/build/src/aSSA/aSSA.* -parcoach -check-mpi < testcase.bc > /dev/null 2> output.txt

cat output.txt
