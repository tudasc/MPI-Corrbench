
module purge
#module load intel intelmpi

module load gcc llvm/10 intelmpi/test

source ~/modules/software/intelmpi/setvars.sh

#intelmpi /mpich
export MPICC="mpigcc -cc=clang"
export CC="mpigcc -cc=clang"
export MPICXX="mpigxx -cxx=clang++"
export CXX="mpigxx -cxx=clang++"
export FC="mpifc"
export MPIFC="mpifc"

TIMEOUT_CMD="/usr/bin/timeout -k 120 120"

# in $(pwd) ist eine datei testcase.c , die ausgewertet weeden soll

# without tool
mkdir without_tool
cd without_tool

/usr/bin/time --format "%e,%M" -o time_compile_baseline $MPICC -g -fopenmp ../testcase.c -lm

/usr/bin/time --format "%e,%M" -o time_run_baseline $TIMEOUT_CMD mpirun -n 2 ./a.out 1> /dev/null 2>&1

rm a.out
cd ..

# with tool
/usr/bin/time --format "%e,%M" -o time_compile $MPICC -g -fopenmp testcase.c -lm

/usr/bin/time --format "%e,%M" -o time_run $TIMEOUT_CMD mpirun -n 2 -check_mpi -genv VT_CHECK_TRACING on ./a.out 1> output.txt 2>&1

echo "$(tail -n 1 without_tool/time_compile_baseline),$( tail -n 1 time_compile)" >> compile_overhead.csv
echo "$(tail -n 1 without_tool/time_run_baseline),$( tail -n 1 time_run)" >> run_overhead.csv

#clean up
rm without_tool/time_compile_baseline time_compile without_tool/time_run_baseline time_run
rm a.out*

