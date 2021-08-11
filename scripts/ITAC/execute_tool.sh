
module purge
module load intel intelmpi


TIMEOUT_CMD="/usr/bin/timeout -k 120 120"

# in $(pwd) ist eine datei testcase.c , die ausgewertet weeden soll

# without tool
/usr/bin/time --format "%e,%M" -o time_compile_baseline mpiicc -g -fopenmp testcase.c -lm

/usr/bin/time --format "%e,%M" -o time_run_baseline $TIMEOUT_CMD mpirun -n 2 ./a.out 1> /dev/null 2>&1

# with tool
/usr/bin/time --format "%e,%M" -o time_compile mpiicc -g -fopenmp testcase.c -lm

/usr/bin/time --format "%e,%M" -o time_run $TIMEOUT_CMD mpirun -n 2 -check_mpi -genv VT_CHECK_TRACING on ./a.out 1> output.txt 2>&1

echo "$(tail -n 1 time_compile_baseline),$( tail -n 1 time_compile)" >> compile_overhead.csv
echo "$(tail -n 1 time_run_baseline),$( tail -n 1 time_run)" >> run_overhead.csv

#clean up
rm time_compile_baseline time_compile time_run_baseline time_run
rm a.out*
