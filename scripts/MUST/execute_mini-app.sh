
module use /home/tj75qeje/moudles/modulefiles
module purge
module load cmake git gcc python openmpi llvm librhash libxml2 MUST

cp -r $MPI_CORRECTNESS_BM_DIR/micro-benches/2-level .

cd 2-level

TIMEOUT_CMD="/usr/bin/timeout -k 3600 3600"

# one slurm task should be reserved for MUST
TASKS=$((SLURM_NTASKS-1))

export CORRBENCH_procs=$TASKS
./$APP.sh download

/usr/bin/time --format "%e,%M" -o time_compile_baseline ./$APP.sh build

/usr/bin/time --format "%e,%M" -o time_run_baseline $TIMEOUT_CMD ./$APP.sh run

./$APP.sh clean

./$APP.sh download

export CORRBENCH_mpirun=mustrun

/usr/bin/time --format "%e,%M" -o time_compile ./$APP.sh build

/usr/bin/time --format "%e,%M" -o time_run $TIMEOUT_CMD ./$APP.sh run

cp $APP_DIR/MUST_Output.html ../MUST_Output.html

echo "$(tail -n 1 time_compile_baseline),$( tail -n 1 time_compile)" >> ../compile_overhead.csv
echo "$(tail -n 1 time_run_baseline),$( tail -n 1 time_run)" >> ../run_overhead.csv

./$APP.sh clean

#clean up
rm time_compile_baseline time_compile time_run_baseline time_run
