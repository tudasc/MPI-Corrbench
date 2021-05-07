# installs parcoach

#PARCOACH_ROOT = $(pwd)/parcoach

git clone https://github.com/parcoach/parcoach.git
mkdir parcoach/build
cd parcoach/build
cmake ..
make 

cd ../..
