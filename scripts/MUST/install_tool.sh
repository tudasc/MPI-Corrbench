# installs must in $(pwd)/MUST_install
mkdir MUST_install

INSTALL_DIR=$(pwd)/MUST_install

wget https://hpc.rwth-aachen.de/must/files/MUST-v1.6.tar.gz
tar -xzf MUST-v1.6.tar.gz

cd MUST-v1.6
mkdir build && cd build

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR ..
make install

cd ../..

# check if installation was succesful
export PATH=$PATH/$INSTALL_DIR/bin
export LIBRARY_PATH=$LIBRARY_PATH/$INSTALL_DIR/lib
export LD_LIBRARY_PATH=$LIBRARY_PATH/$INSTALL_DIR/lib
export CPATH=$CPATH/$INSTALL_DIR/include
mustrun --help
