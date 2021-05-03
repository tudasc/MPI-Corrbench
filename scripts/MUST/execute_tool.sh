
module use /home/tj75qeje/moudles/modulefiles
module purge
module load cmake gcc python openmpi llvm librhash libxml2 MUST

TIMEOUT_CMD="/usr/bin/timeout -k 120 120"

# in $(pwd) ist eine datei testcase.c , die ausgewertet weeden soll

# without tool
/usr/bin/time --format "%e,%M" -o time_compile_baseline mpicc -g testcase.c -lm

/usr/bin/time --format "%e,%M" -o time_run_baseline $TIMEOUT_CMD mpirun -n 2 ./a.out

# with tool
/usr/bin/time --format "%e,%M" -o time_compile mpicc -g testcase.c -lm

/usr/bin/time --format "%e,%M" -o time_run $TIMEOUT_CMD mustrun -n 2 ./a.out

echo "$(tail -n 1 time_compile_baseline),$( tail -n 1 time_compile)" >> compile_overhead.csv
echo "$(tail -n 1 time_run_baseline),$( tail -n 1 time_run)" >> run_overhead.csv

#clean up
rm time_compile_baseline time_compile time_run_baseline time_run
rm a.out
rm -R must_temp
