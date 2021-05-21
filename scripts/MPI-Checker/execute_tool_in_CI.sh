#!/bin/bash

clang-tidy testcase.c -checks='*mpi*' -- $(mpicc --showme:compile) > output.txt
