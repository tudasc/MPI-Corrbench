# installs parcoach
mkdir MUST_install

#PARCOACH_ROOT = $(pwd)parcoach/build

git clone https://github.com/parcoach/parcoach.git
mkdir parcoach/build
cd parcoach/build
cmake ..
make 

cd ../..
