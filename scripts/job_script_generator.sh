#!/bin/bash

# ./job_script_generator.sh TOOL
# We will generate 2 jobscripts:
# one job array for all the testcases
# and one for the evaluation script
# the Tool is given as the input Parameter,
# a Directory with the name of the Tool containing execute_tool.sh and parse_output.py is expected for the generated jobscipts to work properly
# the Slurm job Parameters are read from the SLURM-header.in file

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

TOOL=$1

if [ -z "$TOOL" ]  ||  [ !  -d "$SCRIPTS_DIR/$TOOL" ]; then

echo "Error: No known Tool given"
exit
fi

# may be omited
FLAGS_TO_USE=$2

OUTFILE=$3

if [ -z "$OUTFILE" ]; then

OUTFILE=2Ranks.sh
fi


# make output dir if it not exists
mkdir -p $OUT_DIR
mkdir -p $OUT_DIR/$TOOL

# only the src files, exclude dirs
#CASE_LIST=$(find "$SRC_DIR/0-level" -type f -name "*.c" )#
CASE_LIST=$(find "$SRC_DIR/0-level/openmp" -type f -name "*.c" )
# convert list into array

NUMCASES=$(wc -w <<< $CASE_LIST)

#overwrite old file
cp $SCRIPTS_DIR/SLURM-header.in $SCRIPTS_DIR/$TOOL/$OUTFILE

MAX_INDEX=$(( NUMCASES - 1))
#echo "Number Of Testcases: $MAX_INDEX"

# MUST need 3 ranks
if  [ $TOOL = "MUST" ]; then
echo "#SBATCH -n 3" >> $SCRIPTS_DIR/$TOOL/$OUTFILE
elif  [ $TOOL = "MPI-Checker" ] || [ $TOOL = "PARCOACH" ]; then
# mpi checker and PARCOACH only 1 Rank (static analysis only)
echo "#SBATCH -n 1" >> $SCRIPTS_DIR/$TOOL/$OUTFILE
else
echo "#SBATCH -n 2" >> $SCRIPTS_DIR/$TOOL/$OUTFILE
fi
echo "#SBATCH -a 0-$MAX_INDEX" >> $SCRIPTS_DIR/$TOOL/$OUTFILE
echo "#SBATCH -o $OUT_DIR/$TOOL/job%A_%a.out" >> $SCRIPTS_DIR/$TOOL/$OUTFILE
echo " " >> $SCRIPTS_DIR/$TOOL/$OUTFILE

# setup the testcase

echo "TESTCASES=( $CASE_LIST )" >> $SCRIPTS_DIR/$TOOL/$OUTFILE

echo "THIS_CASE=\${TESTCASES[\$SLURM_ARRAY_TASK_ID]}" >> $SCRIPTS_DIR/$TOOL/$OUTFILE

echo 'export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK' >> $SCRIPTS_DIR/$TOOL/$OUTFILE

echo "export CORRBENCH_CFLAGS=$FLAGS_TO_USE" >> $SCRIPTS_DIR/$TOOL/$OUTFILE

# setup workdir for the case
echo "mkdir -p $OUT_DIR/$TOOL/\$SLURM_ARRAY_JOB_ID/\$SLURM_ARRAY_TASK_ID" >> $SCRIPTS_DIR/$TOOL/$OUTFILE
echo "mkdir -p $OUT_DIR/$TOOL/\$SLURM_ARRAY_JOB_ID/\$SLURM_ARRAY_TASK_ID" >> $SCRIPTS_DIR/$TOOL/$OUTFILE
echo "cd $OUT_DIR/$TOOL/\$SLURM_ARRAY_JOB_ID/\$SLURM_ARRAY_TASK_ID" >> $SCRIPTS_DIR/$TOOL/$OUTFILE
echo "cp \$THIS_CASE $OUT_DIR/$TOOL/\$SLURM_ARRAY_JOB_ID/\$SLURM_ARRAY_TASK_ID/testcase.c" >> $SCRIPTS_DIR/$TOOL/$OUTFILE
echo "export CPATH=\$CPATH:$SRC_DIR/0-level/correct/include:$SRC_DIR/0-level/openmp" >> $SCRIPTS_DIR/$TOOL/$OUTFILE
echo 'if [ ! -f compile_overhead.csv ]; then echo "baseline_time,baseline_mem,time,mem" >> compile_overhead.csv ; fi' >> $SCRIPTS_DIR/$TOOL/$OUTFILE
echo 'if [ ! -f run_overhead.csv ]; then echo "baseline_time,baseline_mem,time,mem" >> run_overhead.csv ; fi' >> $SCRIPTS_DIR/$TOOL/$OUTFILE
# for later evaluation, there is no need to keep the src-file around, for convenience, we keep the filename
echo 'echo "$THIS_CASE" > case_name' >> $SCRIPTS_DIR/$TOOL/$OUTFILE

echo " " >> $SCRIPTS_DIR/$TOOL/$OUTFILE
cat $SCRIPTS_DIR/$TOOL/execute_tool.sh >> $SCRIPTS_DIR/$TOOL/$OUTFILE

#clean up
echo "#clean up" >> $SCRIPTS_DIR/$TOOL/$OUTFILE
echo "rm testcase.c" >> $SCRIPTS_DIR/$TOOL/$OUTFILE


# script for parsing all the output 
cp $SCRIPTS_DIR/SLURM-header.in $SCRIPTS_DIR/$TOOL/parsing.sh
echo "#SBATCH -n 1" >> $SCRIPTS_DIR/$TOOL/parsing.sh
echo "#SBATCH -o $OUT_DIR/$TOOL/parsing%j.out" >> $SCRIPTS_DIR/$TOOL/parsing.sh
# setup env
echo "BENCH_BASE_DIR=$BENCH_BASE_DIR" >> $SCRIPTS_DIR/$TOOL/parsing.sh
echo "OUT_DIR=$OUT_DIR" >> $SCRIPTS_DIR/$TOOL/parsing.sh
echo "TOOL=$TOOL" >> $SCRIPTS_DIR/$TOOL/parsing.sh

cat $SCRIPTS_DIR/parsing_job.sh >> $SCRIPTS_DIR/$TOOL/parsing.sh

echo "generated jobscript for $TOOL"
