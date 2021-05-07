#!/bin/bash

TOOL=$1
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


if [ -z "$TOOL" ]  ||  [ !  -d "$SCRIPTS_DIR/$TOOL" ]; then

echo "Error: No known Tool given"
exit
fi

# make output dir if it not exists
mkdir -p $OUT_DIR
mkdir -p $OUT_DIR/$TOOL

# only the src files, exclude dirs
CASE_LIST=$(find "$SRC_DIR/0-level" -type f -name "*.c" )
# convert list into array

NUMCASES=$(wc -w <<< $CASE_LIST)

MAX_INDEX=$(( NUMCASES - 1))
echo "Number Of Testcases: $MAX_INDEX"

cd $OUT_DIR

i=0
for CASE in $CASE_LIST; do

# setup workdir for the case
mkdir $i

cp CASE $i/testcase.c

# execute the tool
(cd $i && bash $SCRIPTS_DIR/$TOOL/execute_tool_in_CI.sh)

((i++))

done

echo "executed Tool on all Testcases"

python3 $SCRIPTS_DIR/Parser.py . $TOOL --outfile results_$TOOL.json

echo "Gathered all results"

