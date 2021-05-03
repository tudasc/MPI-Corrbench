
# this file is used by the job_script_generator to build the jobscript for gathering the tools result
# setup python env
module purge
module load gcc python


SCRIPTS_DIR=$BENCH_BASE_DIR/scripts

# get the job we should postprocess
ARRAY_JOB=$(echo $SLURM_JOB_DEPENDENCY | cut -d':' -f 2 )

python3 $SCRIPTS_DIR/Parser.py $OUT_DIR/$TOOL/$ARRAY_JOB $TOOL
