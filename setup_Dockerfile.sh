#! /usr/bin/env bash

# misc
export NCPU=1

# log4cpp
export LOG4CPP_FQ_DIR="/log4cpp_install"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/log4cpp_install/lib"

# fsplit
export PATH="${PATH}:/fsplit"

# Pythia6
export PYTHIA6_DIR="/Pythia6/v6_424/"
export PYTHIA6_INCLUDE_DIR="/Pythia6/v6_424/inc/"
export PYTHIA6_LIBRARY="/Pythia6/v6_424/lib/"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/Pythia6/v6_424/lib"

# Xerces-C
export XERCESCROOT="/Xerces-C_install"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/Xerces-C_install/lib64"

# ROOT
export ROOTSYS="/ROOT_install"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/ROOT_install/lib"
export PATH="${PATH}:/ROOT_install/bin"

# Geant4
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/Geant4_install/lib64"

# GENIE
export GENIE="/GENIE"
export GENIE_INSTALL="/GENIE"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/GENIE/lib"