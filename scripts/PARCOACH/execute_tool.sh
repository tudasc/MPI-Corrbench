module purge
module load gcc openmpi llvm
module use /home/groups/da_sc/modules/modulefiles/.gcc/8.3/.openmpi/4.0
module load parcoach

TIMEOUT_CMD="/usr/bin/timeout -k 120 120"

# without tool

/usr/bin/time --format "%e,%M" -o time_compile_baseline mpicc -g -fopenmp testcase.c -lm

/usr/bin/time --format "%e,%M" -o time_run_baseline $TIMEOUT_CMD mpirun -n 2 ./a.out 1> /dev/null 2>&1

# with tool

/usr/bin/time --format "%e,%M" -o time_compile clang -DEXCLUDE_PARCOACH_UNSUPPORTED_FUNCTIONS -c -g -fopenmp -emit-llvm testcase.c -o testcase.bc

/usr/bin/time --format "%e,%M" -o time_run $TIMEOUT_CMD opt -load $PARCOACH_ROOT/lib/aSSA.* -parcoach -check-mpi < testcase.bc > /dev/null 2> output.txt

echo "$(tail -n 1 time_compile_baseline),$( tail -n 1 time_compile)" >> compile_overhead.csv
echo "$(tail -n 1 time_run_baseline),$( tail -n 1 time_run)" >> run_overhead.csv

# clean up
rm time_compile_baseline time_compile time_run_baseline time_run
rm testcase.bc

