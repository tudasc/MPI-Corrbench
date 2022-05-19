#!/bin/bash

#Settings
NUM_RUNS=5
 
TESTCASES=( /home/tim/MPI-Corrbench/micro-benches/0-level/openmp/memory/private_bcast.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/memory/private_after_send.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/memory/private_ibcast.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/memory/private_isend.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/memory/private_send.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/wrong_threading_level_5.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/finalize_missuse_2.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/finalize_missuse.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/finalize_missuse_3.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/wrong_threading_level_3.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/missing_threading_level_check.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/finalize_missuse_4.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/missing_init_thread_4.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/wrong_threading_level_4.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/missing_init_thread.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/wrong_threading_level_6.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/missing_init_thread_3.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/wrong_threading_level_2.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/wrong_threading_level.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/missing_init_thread_2.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/threading/finalize_missuse_5.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_task_bcast.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_reduce.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/info_set_2.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_task_send.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_task_ibcast.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/info_set.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_send.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_isend.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_task_isend.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_isend_2.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_ibcast.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_bcast.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_send_2.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_isend_3.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_send_3.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_8.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/request_reuse.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_3.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_7.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/send_recv_same_buffer.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_5.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_4.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/deadlock_2.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/deadlock_probe.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/info_free.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/task_comm_free.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/comm_free.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/derived_datatype.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/probe.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/deadlock_probe_2.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/deadlock_task_send_recv.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/nondeterministic_deadlock.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/deadlock_send_recv.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_2.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_9.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_6.c
/home/tim/MPI-Corrbench/micro-benches/0-level/openmp/ordering/deadlock.c )

CFLAGS_LIST=( "-DUSE_DISTURBED_THREAD_ORDER -DNUM_THREADS=2 -DBUFFER_LENGTH_INT=10"
"-DUSE_DISTURBED_THREAD_ORDER -DNUM_THREADS=4 -DBUFFER_LENGTH_INT=10"
"-DUSE_DISTURBED_THREAD_ORDER -DNUM_THREADS=8 -DBUFFER_LENGTH_INT=10"
"-DUSE_DISTURBED_THREAD_ORDER -DNUM_THREADS=1 -DBUFFER_LENGTH_INT=10000"
"-DUSE_DISTURBED_THREAD_ORDER -DNUM_THREADS=2 -DBUFFER_LENGTH_INT=10000"
"-DUSE_DISTURBED_THREAD_ORDER -DNUM_THREADS=4 -DBUFFER_LENGTH_INT=10000"
"-DUSE_DISTURBED_THREAD_ORDER -DNUM_THREADS=8 -DBUFFER_LENGTH_INT=10000"
"-DUSE_DISTURBED_THREAD_ORDER -DNUM_THREADS=1 -DBUFFER_LENGTH_INT=1000000"
"-DUSE_DISTURBED_THREAD_ORDER -DNUM_THREADS=2 -DBUFFER_LENGTH_INT=1000000"
"-DUSE_DISTURBED_THREAD_ORDER -DNUM_THREADS=4 -DBUFFER_LENGTH_INT=1000000"
"-DUSE_DISTURBED_THREAD_ORDER -DNUM_THREADS=8 -DBUFFER_LENGTH_INT=1000000"
"-DNUM_THREADS=1 -DBUFFER_LENGTH_INT=10"
"-DNUM_THREADS=2 -DBUFFER_LENGTH_INT=10"
"-DNUM_THREADS=4 -DBUFFER_LENGTH_INT=10"
"-DNUM_THREADS=8 -DBUFFER_LENGTH_INT=10"
"-DNUM_THREADS=1 -DBUFFER_LENGTH_INT=10000"
"-DNUM_THREADS=2 -DBUFFER_LENGTH_INT=10000"
"-DNUM_THREADS=4 -DBUFFER_LENGTH_INT=10000"
"-DNUM_THREADS=8 -DBUFFER_LENGTH_INT=10000"
"-DNUM_THREADS=1 -DBUFFER_LENGTH_INT=1000000"
"-DNUM_THREADS=2 -DBUFFER_LENGTH_INT=1000000"
"-DNUM_THREADS=4 -DBUFFER_LENGTH_INT=1000000"
"-DNUM_THREADS=8 -DBUFFER_LENGTH_INT=1000000" )

export OMP_NUM_THREADS=8
export CPATH=$CPATH:/home/tim/MPI-Corrbench/micro-benches/0-level/correct/include:/home/tim/MPI-Corrbench/micro-benches/0-level/openmp

source /home/tim/ITAC/setvars.sh

#use clang, so we will use the same compiler/ same openmp implementation for all tests
export MPICC="mpigcc -cc=clang-10"
export CC="mpigcc -cc=clang-10"
export MPICXX="mpigxx -cxx=clang++-10"
export CXX="mpigxx -cxx=clang++"
export FC="mpifc"
export MPIFC="mpifc"

TIMEOUT_CMD="/usr/bin/timeout -k 90 90"


# for each cflag-setting
for f in "${!CFLAGS_LIST[@]}" ; do
export CORRBENCH_CFLAGS=${CFLAGS_LIST[$f]}


#i=0
# for each testcase
for EXPREIMENT_ID in "${!TESTCASES[@]}" ; do

THIS_CASE=${TESTCASES[$EXPREIMENT_ID]}

echo "cflag number $f case number $EXPREIMENT_ID"

#EXPREIMENT_ID=$i

for MEASUREMENT_ID in $(seq 1 $NUM_RUNS); do 

mkdir -p /home/tim/mpi-correctness-benchmarks/ITAC/${MEASUREMENT_ID}_${f}
mkdir -p /home/tim/mpi-correctness-benchmarks/ITAC/${MEASUREMENT_ID}_${f}/$EXPREIMENT_ID
cd /home/tim/mpi-correctness-benchmarks/ITAC/${MEASUREMENT_ID}_${f}/$EXPREIMENT_ID
cp $THIS_CASE /home/tim/mpi-correctness-benchmarks/ITAC/${MEASUREMENT_ID}_${f}/$EXPREIMENT_ID/testcase.c

if [ ! -f compile_overhead.csv ]; then echo "baseline_time,baseline_mem,time,mem" >> compile_overhead.csv ; fi
if [ ! -f run_overhead.csv ]; then echo "baseline_time,baseline_mem,time,mem" >> run_overhead.csv ; fi
echo "$THIS_CASE" > case_name
echo "$CORRBENCH_CFLAGS" > cflags_used
 
# in $(pwd) is a file testcase.c , that should be analyzed

# without tool
mkdir without_tool
cd without_tool

/usr/bin/time --format "%e,%M" -o time_compile_baseline $MPICC $CORRBENCH_CFLAGS -g -fopenmp ../testcase.c -lm

/usr/bin/time --format "%e,%M" -o time_run_baseline $TIMEOUT_CMD mpirun -n 2 ./a.out 1> /dev/null 2>&1
echo $? > exit_code_no_tool

rm a.out
cd ..

# with tool
/usr/bin/time --format "%e,%M" -o time_compile $MPICC $CORRBENCH_CFLAGS -g -fopenmp testcase.c -lm

/usr/bin/time --format "%e,%M" -o time_run $TIMEOUT_CMD mpirun -n 2 -check_mpi -genv VT_CHECK_TRACING on ./a.out 1> output.txt 2>&1

echo "$(tail -n 1 without_tool/time_compile_baseline),$( tail -n 1 time_compile)" >> compile_overhead.csv
echo "$(tail -n 1 without_tool/time_run_baseline),$( tail -n 1 time_run)" >> run_overhead.csv

#clean up
rm without_tool/time_compile_baseline time_compile without_tool/time_run_baseline time_run
rm a.out*
#rm -R must_temp
#clean up
rm testcase.c

## END must execution




done
# end for each measurement
#i=$((i+1))
done
#end for each testcase

done 
# end for each cflag

echo "DONE!!!"

