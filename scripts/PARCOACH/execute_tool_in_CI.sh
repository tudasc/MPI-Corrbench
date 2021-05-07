#!/bin/bash

TIMEOUT_CMD="/usr/bin/timeout -k 120 120"

clang -DEXCLUDE_PARCOACH_UNSUPPORTED_FUNCTIONS -c -g $(mpicc --showme:compile) -emit-llvm testcase.c -o testcase.bc

$TIMEOUT_CMD opt -load $PARCOACH_ROOT/build/src/aSSA/aSSA.* -parcoach -check-mpi < testcase.bc > /dev/null 2> output.txt
