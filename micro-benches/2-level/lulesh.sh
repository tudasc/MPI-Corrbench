#! /usr/bin/env bash

source ./utils.sh

parse_args "$@"

# Benchmark name 
target_name="LULESH"
target_name_tgz="${target_name}.tar.gz"

print_info ${target_name}

# Target applicationi
lulesh_dir=lulesh

lulesh_patch_file=${CORRBENCH_mutate_file}

if [[ ${do_download} == "yes" ]]; then
	echo "Download ${target_name}"
  wget -O ${target_name_tgz} https://github.com/LLNL/LULESH/archive/2.0.3.tar.gz
	echo "Creating directories and unpacking"

  mkdir -p ${lulesh_dir} && cd ${lulesh_dir}
  tar xzf ../${target_name_tgz}
  cd ..
fi

# "Creating directories, unpacking, configuring, building"
if [[ ${do_build} == "yes" ]]; then
	echo "Building application"
	cd ${lulesh_dir}/LULESH-2.0.3
	cp ../../patches/Makefile.lulesh ./Makefile

	# apply patches and build the mutations
  if [[ -n "${lulesh_patch_file}" ]]; then
    # Init git repo, add everything apply patch
    echo -e "Applying the patch file: " ${lulesh_patch_file}
    git init >${outputTo} 2>&1
    git add * >${outputTo} 2>&1
    git commit -m "Basic patched LULESH" >${outputTo} 2>&1
    git apply ${lulesh_patch_file}
  else
    echo "Applying no mutation"
  fi

  make MPICXX="${mpi_cxx} -DUSE_MPI=1" ${make_target} -j ${n_procs}
	cd ../../..
fi

# "Running the target application"

if [[ "${do_run}" == "yes" ]]; then

	echo "Running application"
	cd ${lulesh_dir}/LULESH-2.0.3
  time ${mpi_run} -np ${mpi_procs} ./lulesh2.0 -s 40
	cd ../..
fi

if [[ "${do_clean}" == "yes" ]]; then
	echo "Removing ${target_name_tgz} and directorie ${lulesh_dir}"
	rm -rf ${target_name_tgz} ${lulesh_dir} 

fi

