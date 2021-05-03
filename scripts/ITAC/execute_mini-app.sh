
module use /home/tj75qeje/moudles/modulefiles
module purge
module load cmake git intel python intelmpi

cp -r $MPI_CORRECTNESS_BM_DIR/micro-benches/2-level .

cd 2-level

TIMEOUT_CMD="/usr/bin/timeout -k 3600 3600"

TASKS=$SLURM_NTASKS

export CORRBENCH_procs=$TASKS
./$APP.sh download

/usr/bin/time --format "%e,%M" -o time_compile_baseline ./$APP.sh build

/usr/bin/time --format "%e,%M" -o time_run_baseline $TIMEOUT_CMD ./$APP.sh run

./$APP.sh clean

./$APP.sh download

export CORRBENCH_mpirun="mpirun -check_mpi -genv VT_CHECK_TRACING on"

/usr/bin/time --format "%e,%M" -o time_compile ./$APP.sh build

/usr/bin/time --format "%e,%M" -o time_run $TIMEOUT_CMD ./$APP.sh run 1> ../output.txt 2>&1

echo "$(tail -n 1 time_compile_baseline),$( tail -n 1 time_compile)" >> ../compile_overhead.csv
echo "$(tail -n 1 time_run_baseline),$( tail -n 1 time_run)" >> ../run_overhead.csv

./$APP.sh clean

#clean up
rm time_compile_baseline time_compile time_run_baseline time_run
