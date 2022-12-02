# installs parcoach

git clone https://github.com/parcoach/parcoach.git
mkdir parcoach/build
cd parcoach
git checkout tags/v1.2

cd build
cmake ..
make 

cd ../..
