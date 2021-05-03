#! /usr/bin/env bash

source ./utils.sh

parse_args "$@"

# Benchmark name 
target_name="Kripke"
target_name_tgz="${target_name}.tar.gz"

print_info ${target_name}

# Target application
kripke_dir=kripke

kripke_patch_file=${CORRBENCH_mutate_file}

if [[ ${do_download} == "yes" ]]; then
	echo "Download ${target_name}"
  wget -O ${target_name_tgz} https://github.com/LLNL/Kripke/releases/download/v1.2.4/kripke-v1.2.4-d85c6bc.tar.gz
	echo "Creating directories and unpacking"
  mkdir -p ${kripke_dir} && cd ${kripke_dir}
  tar xzf ../${target_name_tgz}
  cd ..
fi

# "Creating directories, unpacking, configuring, building"
if [[ ${do_build} == "yes" ]]; then
	echo "Building application"
	cd ${kripke_dir}
  cd kripke-v1.2.4-d85c6bc
	
	if [[ -n "${kripke_patch_file}" ]]; then
	# Init git repo, add everything apply patch
    echo -e "Applying the patch file: " ${kripke_patch_file}
	  git init >${outputTo} 2>&1
	  git add * >${outputTo} 2>&1
	  git commit -m "Basic patched Kripke" >${outputTo} 2>&1
	  git apply ${kripke_patch_file}
  else 
	  echo "Applying no mutation"
	fi

  mkdir build && cd build
  cmake -DMPI_CXX_COMPILER=${CORRBENCH_mpicxx} -DCMAKE_CXX_COMPILER=${CORRBENCH_cxx} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DENABLE_MPI=ON -DENABLE_CPPCHECK=OFF ..
	if [[ ! -z ${make_target} ]]; then
		if [[ ${make_target} != "parcoach-target" ]]; then
			echo "Warning uncommon target"
		else
			cd ..
			cp ../../patches/Makefile.kripke Makefile
			make ${make_target} -j ${n_procs}
			cd ../../
		fi
	else
		echo ${PWD}
    make ${make_target} -j ${n_procs}
	  cd ../../..
	fi
fi

# "Running the target application"

if [[ ${do_run} == "yes" ]]; then
	echo "Running application"
	cd ${kripke_dir}/kripke-v1.2.4-d85c6bc/build/bin
  ${mpi_run} -np ${mpi_procs} ./kripke.exe --procs 2,2,2 --zones 70,70,70
	cd ../../../
fi

if [[ ${do_clean} == "yes" ]]; then
	echo "Removing ${target_name_tgz} and directorie ${kripke_dir} "
	rm -rf ${kripke_dir} ${target_name_tgz}
fi

