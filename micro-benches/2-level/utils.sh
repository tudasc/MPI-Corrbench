

do_download="no"
do_build="no"
do_run="no"
do_clean="no"


# Compilers
mpi_cc=${CORRBENCH_mpicc:-${MPICC}}
mpi_cxx=${CORRBENCH_mpicxx:-${MPICXX}}
cc=${CORREBENCH_cc:-${CC}}
cxx=${CORRBENCH_cxx:-${CXX}}

# #executors
mpi_run="${CORRBENCH_mpirun:-mpirun}"

# compile jobs
n_procs=${CORRBENCH_cpus:-$(cat /proc/cpuinfo | grep MHz | wc -l)}
# This number is chosen arbitrarily (but is the default for our tests with LULESH, AMG and Kripke
mpi_procs=${CORRBENCH_procs:-8}

# For special purpose Make targets
make_target=${CORRBENCH_make_target}

# Output
outputTo=/dev/null

function print_usage() {
	echo "Script to download, build, run, and clean MPI CorrBench Mini-Apps"
	echo "Modify the default behavior of the script using the environment variables CORRBENCH_<mpicc, mpicxx, cc, cxx, mpirun, cpus, procs, make_target>"
	echo -e "mpicc = MPI C compiler\nmpicxx = MPI C++ Compiler\ncc = C compiler\ncxx = C++ compiler\nmpirun = MPI exec command (includes potential special flags, e.g., --oversubsribe)\ncpus = number of parallel Make processes\nprocs = number of MPI ranks\nmake_target = custom make targets, e.g., to run clang-tidy [tidy-target,parcoach-target]"
	echo -e "To apply mutations to the target benchmark set the CORRBENCH_mutate_file environment variable to the mutate patch, e.g., \"coll.type-mismatch.01/amg.sstruct_grid.mpi_int.patch\""
}

function print_info() {
	benchmark=$1
	echo -e "Running ***${benchmark}*** with\nCC: ${cc}\nCXX: ${cxx}\nMPICC: ${mpi_cc}\nMPICXX: ${mpi_cxx}\nMPIRUN: ${mpi_run}\n"
	echo -e "Performing\nDownload: ${do_download}\nBuild: ${do_build} (${n_procs} cpus)\nRun: ${do_run}\nClean: ${do_clean}\n\n"
}

function parse_args() {
  while (("$#")); do

		if [[ $1 == "download" ]]; then
		  do_download="yes"
		fi

		if [[ $1 == "build" ]]; then
		  do_build="yes"
		fi

		if [[ $1 == "run" ]]; then
		  do_run="yes"
		fi

		if [[ $1 == "clean" ]]; then
		  do_clean="yes"
		fi

		shift

	done
}

function err_report() {
  echo "Error on line $1"
	exit -1
}

trap 'err_report $LINENO' ERR

if [[ "$#" -eq 0 ]]; then
	print_usage
	exit 1
fi

