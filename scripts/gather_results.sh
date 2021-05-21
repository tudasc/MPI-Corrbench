#!/bin/bash

# This script does not need Parameters
# If you dont want to submit the postprocessing into the slurm queue one can also use this script, which will gahter all results from all tools

BENCH_BASE_DIR=$MPI_CORRECTNESS_BM_DIR
IN_DIR=$MPI_CORRECTNESS_BM_EXPERIMENT_DIR

if [ ! -d "$BENCH_BASE_DIR" ]; then
echo "Error: please provide MPI_CORRECTNESS_BM_DIR as environment Variable"
exit
fi

if [ ! -d "$IN_DIR" ]; then
echo "Error: please provide MPI_CORRECTNESS_BM_EXPERIMENT_DIR as environment Variable"
exit
fi


OUT_DIR=$BENCH_BASE_DIR/output
SCRIPTS_DIR=$BENCH_BASE_DIR/scripts
SRC_DIR=$BENCH_BASE_DIR/micro-benches

TOOLS="MUST ITAC MPI-Checker PARCOACH"

# make output dir if it not exists
mkdir -p $OUT_DIR


for TOOL in $TOOLS ; do
	if [ -z "$TOOL" ]  ||  [ !  -d "$SCRIPTS_DIR/$TOOL" ]; then
	echo "Error: No scripts for $TOOL found"
	exit
	fi
	
	for dir in $IN_DIR/$TOOL/* ; do
	    if [ -d "$dir" ]; then
		python3 $SCRIPTS_DIR/Parser.py $dir $TOOL &
    	    fi
	done
	
	# wait for all parser to be complete
	wait < <(jobs -p)

	python3 $SCRIPTS_DIR/gather_results.py $TOOL &
	python3 $SCRIPTS_DIR/gather_performance.py $TOOL &
	wait < <(jobs -p)
done

echo "gathered all data"
