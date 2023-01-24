#!/bin/bash

# ./run_measurement.sh N
# N = Number of trials to run for each testcase 

# This script generates all jobscriptsfor the tools "MUST ITAC MPI-Checker PARCOACH"
# and submits all testcases to the queue

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

num_re='^[0-9]+$'
if ! [[ $1 =~ $num_re ]] ; then
echo "Error: please provide the number of Jobs to run as a command line argument"
exit
fi

NUM_RUNS=$1

OUT_DIR=$BENCH_BASE_DIR/output
SCRIPTS_DIR=$BENCH_BASE_DIR/scripts
SRC_DIR=$BENCH_BASE_DIR/micro-benches

TOOLS="MUST ITAC"
#TOOLS="MUST ITAC MPI-Checker PARCOACH"


for TOOL in $TOOLS ; do
	if [ -z "$TOOL" ]  ||  [ !  -d "$SCRIPTS_DIR/$TOOL" ]; then
	echo "Error: No scripts for $TOOL found"
	exit
	fi

	# make output dir if it not exists
	mkdir -p $OUT_DIR
	mkdir -p $OUT_DIR/$TOOL

f=0
while IFS= read -r line; do
    echo "cflags $line"

	bash $SCRIPTS_DIR/job_script_generator.sh $TOOL "$line" 2Ranks$f.sh

	for i in $(seq 1 $NUM_RUNS); do
		JID=$(sbatch $SCRIPTS_DIR/$TOOL/2Ranks$f.sh | grep "Submitted batch job" | cut -d' ' -f 4)
		if ! [[ $JID =~ $num_re ]] ; then
			echo "Error during job submission: ABORT"
			exit
		fi
        	JID_PARSER=$(sbatch --dependency afterok:$JID $SCRIPTS_DIR/$TOOL/parsing.sh | grep "Submitted batch job" | cut -d' ' -f 4)
		if ! [[ $JID =~ $num_re ]] ; then
			echo "Error during job submission: ABORT"
			exit
		fi
	done

f=$((f+1))
done < cflags_list
# for each set of cflags
	echo "submitted jobs for $TOOL"
done

echo "submitted all batch-jobs"
