module purge
module load gcc openmpi llvm


# clang threat time run as time compile as it is static analysis only

/usr/bin/time --format "%e,%M" -o time_run_baseline mpicc -g testcase.c -lm

/usr/bin/time --format "%e,%M" -o time_run clang-tidy testcase.c -checks='*mpi*' -- $(mpicc --showme:compile) > output.txt


#cat time_compile_baseline time_compile | tr '\n' ',' | head -c-1 > compile_overhead.csv

cat time_run_baseline time_run | tr '\n' ',' | head -c-1 > run_overhead.csv

#clean up
rm time_run_baseline time_run

