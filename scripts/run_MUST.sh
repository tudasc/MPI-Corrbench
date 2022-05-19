#!/bin/bash
# SLURM Settings used for all jobs

##SBATCH --mail-type=END
#SBATCH -J MPI-Correctness-Bench

#SBATCH --mem-per-cpu=3000
#SBATCH -t 12:00:00

# Maximum number of threads required for multi-threaded testcases
#SBATCH -c 8

#SBATCH -n 3
#SBATCH -a 0-58
#SBATCH -o /work/scratch/tj75qeje/mpi-correctness-benchmarks/MUST/job%A_%a.out


#Settings
NUM_RUNS=5
 
TESTCASES=( /home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/memory/private_bcast.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/memory/private_after_send.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/memory/private_ibcast.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/memory/private_isend.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/memory/private_send.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/wrong_threading_level_5.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/finalize_missuse_2.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/finalize_missuse.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/finalize_missuse_3.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/wrong_threading_level_3.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/missing_threading_level_check.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/finalize_missuse_4.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/missing_init_thread_4.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/wrong_threading_level_4.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/missing_init_thread.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/wrong_threading_level_6.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/missing_init_thread_3.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/wrong_threading_level_2.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/wrong_threading_level.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/missing_init_thread_2.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/threading/finalize_missuse_5.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_task_bcast.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_reduce.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/info_set_2.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_task_send.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_task_ibcast.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/info_set.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_send.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_isend.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_task_isend.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_isend_2.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_ibcast.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_bcast.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_send_2.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_isend_3.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/data_race/data_race_send_3.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_8.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/request_reuse.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_3.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_7.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/send_recv_same_buffer.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_5.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_4.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/deadlock_2.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/deadlock_probe.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/info_free.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/task_comm_free.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/comm_free.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/derived_datatype.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/probe.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/deadlock_probe_2.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/deadlock_task_send_recv.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/dependant/nondeterministic_deadlock.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/deadlock_send_recv.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_2.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_9.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/two_collectives_6.c
/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp/ordering/deadlock.c )

CFLAGS_LIST=( "-DUSE_DISTURBED_THREAD_ORDER -DNUM_THREADS=1 -DBUFFER_LENGTH_INT=10"
"-DUSE_DISTURBED_THREAD_ORDER -DNUM_THREADS=2 -DBUFFER_LENGTH_INT=10"
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
export CPATH=$CPATH:/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/correct/include:/home/tj75qeje/MPI-Corrbench/micro-benches/0-level/openmp

EXPREIMENT_ID=$SLURM_ARRAY_TASK_ID

module use /home/tj75qeje/moudles/modulefiles
module purge
module load cmake gcc python openmpi/4.0.3 llvm/10 librhash libxml2 MUST/1.8
#module load cmake gcc python openmpi llvm/10 librhash libxml2 MUST/1.7.1
#module load cmake gcc llvm/10 intelmpi/test python llvm/10 librhash libxml2 MUST/1.8_intel
#module load cmake gcc llvm/10 python librhash libxml2 MUST/1.8_intel

# Openmpi
export OMPI_CC=clang
export OMPI_CXX=clang++

#source ~/modules/software/intelmpi/setvars.sh

#intelmpi /mpich
#export MPICC="mpigcc -cc=clang"
#export CC="mpigcc -cc=clang"
#export MPICXX="mpigxx -cxx=clang++"
#export CXX="mpigxx -cxx=clang++"
#export FC="mpifc"
#export MPIFC="mpifc"

#export MPICH_CC=clang
#export MPICC=mpigcc
#export MPICH_CXX=clang++
#export MPICXX=mpigxx

TIMEOUT_CMD="/usr/bin/timeout -k 90 90"
TIMEOUT_CMD_NO_TOOL="/usr/bin/timeout -k 30 30"


# for each cflag-setting
for f in "${!CFLAGS_LIST[@]}" ; do
export CORRBENCH_CFLAGS=${CFLAGS_LIST[$f]}


#i=0
# for each testcase
#for EXPREIMENT_ID in $(seq 0 102) ; do

THIS_CASE=${TESTCASES[$EXPREIMENT_ID]}

echo $THIS_CASE

#EXPREIMENT_ID=$i

for MEASUREMENT_ID in $(seq 1 $NUM_RUNS); do 

mkdir -p /work/scratch/tj75qeje/mpi-correctness-benchmarks/MUST/${MEASUREMENT_ID}_${f}
mkdir -p /work/scratch/tj75qeje/mpi-correctness-benchmarks/MUST/${MEASUREMENT_ID}_${f}/$EXPREIMENT_ID
cd /work/scratch/tj75qeje/mpi-correctness-benchmarks/MUST/${MEASUREMENT_ID}_${f}/$EXPREIMENT_ID
cp $THIS_CASE /work/scratch/tj75qeje/mpi-correctness-benchmarks/MUST/${MEASUREMENT_ID}_${f}/$EXPREIMENT_ID/testcase.c

if [ ! -f compile_overhead.csv ]; then echo "baseline_time,baseline_mem,time,mem" >> compile_overhead.csv ; fi
if [ ! -f run_overhead.csv ]; then echo "baseline_time,baseline_mem,time,mem" >> run_overhead.csv ; fi
echo "$THIS_CASE" > case_name
echo "$CORRBENCH_CFLAGS" > cflags_used
 
# in $(pwd) is a file testcase.c , that should be analyzed

# without tool
mkdir without_tool
cd without_tool
/usr/bin/time --format "%e,%M" -o time_compile_baseline $MPICC $CORRBENCH_CFLAGS -g -fopenmp ../testcase.c -lm

/usr/bin/time --format "%e,%M" -o time_run_baseline $TIMEOUT_CMD_NO_TOOL mpirun -n 2 ./a.out
echo $? > exit_code_no_tool

rm a.out
cd ..

# with tool
/usr/bin/time --format "%e,%M" -o time_compile $MPICC $CORRBENCH_CFLAGS -g -fopenmp -fsanitize=thread testcase.c -lm
echo "$MPICC $CORRBENCH_CFLAGS -g -fopenmp -fsanitize=thread testcase.c -lm"

/usr/bin/time --format "%e,%M" -o time_run $TIMEOUT_CMD mustrun --must:distributed --must:hybrid -n 2 ./a.out

#use standard must mode for crash handeling?
#/usr/bin/time --format "%e,%M" -o time_run $TIMEOUT_CMD mustrun --must:hybrid -n 2 ./a.out

echo "$(tail -n 1 without_tool/time_compile_baseline),$( tail -n 1 time_compile)" >> compile_overhead.csv
echo "$(tail -n 1 without_tool/time_run_baseline),$( tail -n 1 time_run)" >> run_overhead.csv

#clean up
rm without_tool/time_compile_baseline time_compile without_tool/time_run_baseline time_run
rm a.out
#rm -R must_temp
#clean up
rm testcase.c

## END must execution




done
# end for each measurement
#i=$((i+1))
#done
#end for each testcase

done
# end for each cflag


echo "DONE!!!"


