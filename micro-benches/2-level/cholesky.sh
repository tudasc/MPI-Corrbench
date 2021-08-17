#! /usr/bin/env bash

source ./utils.sh

parse_args "$@"

# Benchmark name 
target_name="cholesky_omptasks"

print_info ${target_name}

cholesky_dir=${target_name}/detach-deps
cholesky_patch_file=${CORRBENCH_mutate_file}

if [[ ${do_download} == "yes" ]]; then
  echo "Download ${target_name}"
  git clone -b mpi-detach https://github.com/RWTH-HPC/${target_name}.git
fi

if [[ ${do_build} == "yes" ]]; then
  echo "Building application"
  cd ${cholesky_dir}
  cp ../../patches/Makefile.cholesky ./Makefile

  # apply patches and build the mutations
  if [[ -n "${cholesky_patch_file}" ]]; then
    # Init git repo, add everything apply patch
    echo -e "Applying the patch file: " ${cholesky_patch_file}
    git apply ${cholesky_patch_file}
  else
    echo "Applying no mutation"
  fi

  make MPICXX="${mpi_cxx}" ${make_target} -j ${n_procs}
  cd ../../..
fi

if [[ "${do_run}" == "yes" ]]; then
  echo "Running application"
  cd ${cholesky_dir}
  export MPIX_DETACH=progress
  time ${mpi_run} -np ${mpi_procs} ./cholesky_clang_fine 4096 128 1
  cd ../..
fi

if [[ "${do_clean}" == "yes" ]]; then
  echo "Removing directory ${target_name}"
  rm -rf ${target_name}
fi

