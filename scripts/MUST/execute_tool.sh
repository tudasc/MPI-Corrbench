
module use /home/tj75qeje/moudles/modulefiles
module purge
module load llvm/intel MUST/1.8

# Openmpi
export OMPI_CC=clang
export OMPI_CXX=clang++

source ~/modules/software/intelmpi/setvars.sh

export MPICC=mpicc
export I_MPI_CC=clang
export I_MPI_CXX=clang++


#intelmpi /mpich
#export MPICC="mpigcc -cc=clang"
#export CC="mpigcc -cc=clang"
#export MPICXX="mpigxx -cxx=clang++"
#export CXX="mpigxx -cxx=clang++"
#export FC="mpifc"
#export MPIFC="mpifc"

TIMEOUT_CMD="/usr/bin/timeout -k 120 120"

# in $(pwd) is a file testcase.c , that should be analyzed

# without tool
mkdir without_tool
cd without_tool
/usr/bin/time --format "%e,%M" -o time_compile_baseline $MPICC $CORRBENCH_CFLAGS -g -fopenmp ../testcase.c -lm

/usr/bin/time --format "%e,%M" -o time_run_baseline $TIMEOUT_CMD mpirun -n 2 ./a.out
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
