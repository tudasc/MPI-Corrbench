# installs must in $(pwd)/MUST_install
mkdir MUST_install

INSTALL_DIR=$(pwd)/MUST_install

wget https://hpc.rwth-aachen.de/must/files/MUST-v1.6.tar.gz
tar -xzf MUST-v1.6.tar.gz

cd MUST-v1.6
mkdir build && cd build

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR ..
make -j install

cd ../..
