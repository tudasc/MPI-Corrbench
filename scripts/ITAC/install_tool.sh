# installs must in $(pwd)/ITAC_install
mkdir ITAC_install

INSTALL_DIR=$(pwd)/ITAC_install

# intel MPI
wget https://registrationcenter-download.intel.com/akdlm/irc_nas/17729/l_mpi_oneapi_p_2021.2.0.215.sh

#ITAC
wget https://registrationcenter-download.intel.com/akdlm/irc_nas/17686/l_itac_oneapi_p_2021.2.0.152.sh

# install 
bash l_mpi_oneapi_p_2021.2.0.215.sh -s -a --silent --eula accept --install-dir $INSTALL_DIR
bash l_itac_oneapi_p_2021.2.0.152.sh -s -a --silent --eula accept --install-dir $INSTALL_DIR
