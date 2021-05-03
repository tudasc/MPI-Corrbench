#!/bin/bash

# ./job_script_generator_mini_app.sh TOOL APP
# Generates a Job array for a given Tool to execute the given mini app with all mutations
# the Slurm job Parameters are read from the SLURM-header.in file



#Number of tasks to use for benchmakr run:
TASKS=8



APP=$2

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

if [ -z "$APP" ] ; then

echo "Error: No APP given"
exit
fi

OUTFILE=$SCRIPTS_DIR/$TOOL/$APP.sh


# make output dir if it not exists
mkdir -p $OUT_DIR
mkdir -p $OUT_DIR/$TOOL
mkdir -p $OUT_DIR/$TOOL/$APP


# only the src files, exclude dirs
CASE_LIST=$(find "$SRC_DIR/2-level/patches/$APP/mutations/" -type f -name "*.patch" )
# convert list into array

NUMCASES=$(wc -w <<< $CASE_LIST)

#overwrite old file
cp $SCRIPTS_DIR/SLURM-header.in $OUTFILE

#include no mutation
MAX_INDEX=$NUMCASES
echo "Number Of Testcases: $MAX_INDEX"

# MUST need 3 ranks
if  [ $TOOL = "MUST" ]; then
TASKS=$((TASKS+1))
#reserve one proc for must
elif  [ $TOOL = "MPI-Checker" ] || [ $TOOL = "PARCOACH" ]; then
# mpi checker and PARCOACH only 1 Rank (static analysis only)
TASKS=1
fi

echo "#SBATCH -n $TASKS" >> $OUTFILE
echo "#SBATCH -a 0-$MAX_INDEX" >> $OUTFILE
#echo "#SBATCH -a 0-0" >> $OUTFILE
echo "#SBATCH -o $OUT_DIR/$TOOL/$APP/job%A_%a.out" >> $OUTFILE
echo " " >> $OUTFILE

# setup the testcase

echo "TESTCASES=( $CASE_LIST )" >> $OUTFILE


echo 'if [ $SLURM_ARRAY_TASK_ID -eq $SLURM_ARRAY_TASK_MAX ]; then THIS_CASE="" ; else' >> $OUTFILE
echo "THIS_CASE=\${TESTCASES[\$SLURM_ARRAY_TASK_ID]};fi" >> $OUTFILE
# empty patch file
echo 'export CORRBENCH_mutate_file=$THIS_CASE' >> $OUTFILE
echo 'if [ $SLURM_ARRAY_TASK_ID -eq $SLURM_ARRAY_TASK_MAX ]; then THIS_CASE="NONE" ; fi' >> $OUTFILE
echo 'echo "$THIS_CASE" > case_name' >> $OUTFILE
echo "MPI_CORRECTNESS_BM_DIR=$MPI_CORRECTNESS_BM_DIR" >> $OUTFILE
echo "APP=$APP" >> $OUTFILE

if [ $APP == "kripke" ]; then
echo "APP_DIR=./kripke/kripke-v1.2.4-d85c6bc/build/bin" >> $OUTFILE
fi

if [ $APP == "amg2013" ]; then
echo "APP_DIR=./amg2013/AMG2013/test" >> $OUTFILE
fi

if [ $APP == "lulesh" ]; then
echo "APP_DIR=./lulesh/LULESH-2.0.3" >> $OUTFILE
fi

# setup workdir for the case
echo "mkdir -p $OUT_DIR/$TOOL/$APP/\$SLURM_ARRAY_JOB_ID/\$SLURM_ARRAY_TASK_ID" >> $OUTFILE
echo "mkdir -p $OUT_DIR/$TOOL/$APP/\$SLURM_ARRAY_JOB_ID/\$SLURM_ARRAY_TASK_ID" >> $OUTFILE
echo "cd $OUT_DIR/$TOOL/$APP/\$SLURM_ARRAY_JOB_ID/\$SLURM_ARRAY_TASK_ID" >> $OUTFILE
echo "cp -r \$BENCH_BASE_DIR/micro-benches/2-level ." >> $OUTFILE
echo 'if [ ! -f compile_overhead.csv ]; then echo "baseline_time,baseline_mem,time,mem" >> compile_overhead.csv ; fi' >> $OUTFILE
echo 'if [ ! -f run_overhead.csv ]; then echo "baseline_time,baseline_mem,time,mem" >> run_overhead.csv ; fi' >> $OUTFILE
echo "cd 2-level" >> $OUTFILE

echo " " >> $OUTFILE
cat $SCRIPTS_DIR/$TOOL/execute_mini-app.sh >> $OUTFILE

#clean up
echo "#clean up" >> $OUTFILE
#echo "rm testcase.c" >> $OUTFILE


echo "generated jobscript for $TOOL"
