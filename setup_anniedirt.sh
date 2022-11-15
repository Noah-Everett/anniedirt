###############################
### TEMPORARY PRODUCT SETUP ###
###############################
###
### NOTE: This setup code was taken direcrly from run_grid_wcsim.sh. 
###       Most of these products are probably not nessecary, just a quick 
###       solution for setting up ANNIEDirt to be built.
###
###############################

export CODE_BASE=/cvmfs/larsoft.opensciencegrid.org/products
source ${CODE_BASE}/setup
export PRODUCTS=${PRODUCTS}:${CODE_BASE}

setup ifdhc        v2_6_3       -q c7:debug:p392
setup geant4       v4_10_3_p03c -q debug:e15
setup genie        v2_12_10b    -q debug:e15  
setup genie_phyopt v2_12_10     -q dkcharmtau
setup genie_xsec   v2_12_10     -q DefaultPlusMECWithNC
setup xerces_c     v3_2_0a      -q debug:e15
setup root         v6_12_06a    -q debug:e15
setup cmake        v3_10_1

source ${CODE_BASE}/larsoft/root/v6_06_08/Linux64bit+2.6-2.12-e10-nu-debug/bin/thisroot.sh
#source /cvmfs/larsoft.opensciencegrid.org/products/root/v6_12_06a/Linux64bit+2.6-2.12-e15-debug/bin/thisroot.sh
export CXX=$(which g++)
export CC=$(which gcc)
export XERCESROOT=${CODE_BASE}/larsoft/xerces_c/v3_1_3/Linux64bit+2.6-2.12-e10-debug
export G4SYSTEM=Linux-g++
export ROOT_PATH=${CODE_BASE}/larsoft/root/v6_06_08/Linux64bit+2.6-2.12-e10-nu-debug/cmake
#export ROOT_PATH=/cvmfs/larsoft.opensciencegrid.org/products/root/v6_12_06a/Linux64bit+2.6-2.12-e15-debug/cmake
export GEANT4_PATH=${GEANT4_FQ_DIR}/lib64/Geant4-10.1.3
export ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${GENIE}/../include/GENIE
export ROOT_LIBRARY_PATH=${ROOT_LIBRARY_PATH}:${GENIE}/../lib
