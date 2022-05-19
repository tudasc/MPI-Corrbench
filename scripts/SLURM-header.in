#!/bin/bash
# SLURM Settings used for all jobs

##SBATCH --mail-type=END
#SBATCH -J MPI-Correctness-Bench

#SBATCH --mem-per-cpu=3000
#SBATCH -t 00:10:00

# Maximum number of threads required for multi-threaded testcases
#SBATCH -c 8

# --ntasks and --array
# Will be specified by the Job Generator Script according to the requirements of each test-case
#-o will also set by the job-script generator
