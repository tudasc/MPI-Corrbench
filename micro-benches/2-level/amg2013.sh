#! /usr/bin/env bash

source ./utils.sh

parse_args "$@"

# Benchmark name 
target_name="AMG2013"
target_name_tgz="${target_name}.tar.gz"

#print_info ${target_name} ${mpi_cc} ${mpi_cxx} ${cc} ${cxx} ${mpi_run}
print_info ${target_name}

# Target applicationi
amg_dir=amg2013

amg_patch_file=${CORRBENCH_mutate_file}

if [[ ${do_download} == "yes" ]]; then
	echo "Download ${target_name}"
  wget -O ${target_name_tgz} https://computing.llnl.gov/projects/co-design/download/amg2013.tgz
	echo "Creating directories and unpacking"
  mkdir -p ${amg_dir} && cd ${amg_dir}
  tar xzf ../${target_name_tgz}
  cd ..
fi

# "Creating directories, unpacking, configuring, building"
if [[ ${do_build} == "yes" ]]; then
  # TODO We can go all crazy here in how to build these mutations.: For now, I'll simply use git?!
  # Second part is to mutate and build mutated version of the target
	echo "Building mutated version"
	# TODO mutate AMG and write this part
	cd ${amg_dir}
  cd AMG2013
	# We do not want openmp support as of now
	sed -i "s/-fopenmp//" Makefile.include
	sed -i "s/-DHYPRE_USING_OPENMP//" Makefile.include

	# Make AMG buildable in parallel
	cp ../../patches/amg2013/Makefile.amg2013 ./Makefile

	# Patch MPI 3.0 conforming names to type creation
	patch -p1 utilities/utilities.h ../../patches/amg2013/amg.utilities.h.patch
	patch -p1 utilities/thread_mpistubs.h ../../patches/amg2013/amg.thread_mpistubs.h.patch
	patch -p1 utilities/thread_mpistubs.c ../../patches/amg2013/amg.thread_mpistubs.c.patch
	patch -p1 utilities/mpistubs.h ../../patches/amg2013/amg.mpistubs.h.patch
	patch -p1 parcsr_mv/par_csr_communication.c ../../patches/amg2013/amg.par_csr_communication.c.patch
	
	# Patch the Makefiles for a parcoach target
	# amg_folder=(IJ_mv krylov parcsr_ls parcsr_mv seq_mv sstruct_mv struct_mv test utilities)
	for folder in ${amg_folder[@]}
	do
		patch -p1 ./${folder}/Makefile ../../patches/amg2013/amg.Makefile.${folder}
	done

	# Add the clang-tidy targets to the Makefiles
	find . -type f -name "Makefile" -exec sh -c "cat ../../patches/amg2013/amg.Makefile.app >> {}" \;

	if [[ -n "${amg_patch_file}" ]]; then
	# Init git repo, add everything apply patch
    echo -e "Applying the patch file: " ${amg_patch_file}
	  git init >${outputTo} 2>&1
	  git add * >${outputTo} 2>&1
	  git commit -m "Basic patched AMG" >${outputTo} 2>&1
	  git apply ${amg_patch_file}
  else 
	  echo "Applying no mutation"
	fi

  make CC=${cc} ${make_target} -j ${n_procs}
	cd ../..
fi

# "Running the target application"

if [[ ${do_run} == "yes" ]]; then
	echo "Running application"
	# Run mutated version
	cd ${amg_dir}/AMG2013/test
  ${mpi_run} -np ${mpi_procs} ./amg2013 -pooldist 1 -n 256 256 256 -r 80 80 80 -P 1 1 1 -printstats
	cd ../../..
fi

if [[ ${do_clean} == "yes" ]]; then
	echo "Removing ${target_name_tgz} and directorie ${amg_dir} "
	rm -rf ${target_name_tgz} ${amg_dir}
fi

