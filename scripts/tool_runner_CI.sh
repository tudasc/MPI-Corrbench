#!/bin/bash

TOOL=$1
CATEGORY=$2
BENCH_BASE_DIR=$MPI_CORRECTNESS_BM_DIR
OUT_DIR=$MPI_CORRECTNESS_BM_EXPERIMENT_DIR

if [ ! -d "$BENCH_BASE_DIR" ]; then
echo "Error: please provide MPI_CORRECTNESS_BM_DIR as environment Variable"
exit
fi

if [ ! -d "$BENCH_BASE_DIR" ]; then
echo "Error: please provide MPI_CORRECTNESS_BM_EXPERIMENT_DIR as environment Variable"
exit
fi

SCRIPTS_DIR=$BENCH_BASE_DIR/scripts
SRC_DIR=$BENCH_BASE_DIR/micro-benches

# setup for the Hybrid testcases
export OMP_NUM_THREADS=2
export CORRBENCH_CFLAGS="-DNUM_THREADS=2 -DBUFFER_LENGTH_INT=10"


if [ -z "$TOOL" ]  ||  [ !  -d "$SCRIPTS_DIR/$TOOL" ]; then

echo "Error: No known Tool given"
exit
fi

CATEGORY_DIR=${CATEGORY/-/\/}

if [ -z "$CATEGORY" ]  ||  [ !  -d "$SRC_DIR/0-level/$CATEGORY_DIR" ]; then

echo "Error: No known error category given"
exit
fi

# make output dir if it not exists
mkdir -p $OUT_DIR
mkdir -p $OUT_DIR/$TOOL

# only the src files, exclude dirs
CASE_LIST=$(find "$SRC_DIR/0-level/$CATEGORY_DIR" -type f -name "*.c" )
# convert list into array

NUMCASES=$(wc -w <<< $CASE_LIST)

MAX_INDEX=$(( NUMCASES - 1))
echo "Number Of Testcases: $MAX_INDEX"

# setup environment for executing the tools
export CPATH=$CPATH:$SRC_DIR/0-level/correct/include:$SRC_DIR/0-level/openmp
cd $OUT_DIR/$TOOL

i=0
for CASE in $CASE_LIST; do

# setup workdir for the case
mkdir $i

cp $CASE $i/testcase.c
# keep the case name for later evaluation
echo "$CASE" > $i/case_name
# execute the tool
(cd $i && bash $SCRIPTS_DIR/$TOOL/execute_tool_in_CI.sh)

((i++))

done

echo "executed Tool on all Testcases"



python3 $SCRIPTS_DIR/Parser.py . $TOOL --outfile results_$CATEGORY_$TOOL.json

echo "Gathered all results"
