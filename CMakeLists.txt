#
# cd /annie/app/users/rhatcher/tank_flux/
# mkdir G4ANNIE-cmakebuild
# cd G4ANNIE-cmakebuild
#
#
# WHICH_G4=`ls -d ${GEANT4_FQ_DIR}/lib64/Geant4-*`
# WHICH_CLHEP=`ls -d ${CLHEP_LIB_DIR}/CLHEP-*`
#
# cmake -DGeant4_DIR=${WHICH_G4} -DCLHEP_DIR=${WHICH_CLHEP}
#    [ -DWITH_G4NU=ON ] ../G4ANNIE
#
#
# Required environment variables:
#    ROOTSYS    e.g. /nusoft/app/externals/root/v5_34_01/Linux64bit+2.6-2.5-nu-e2-debug
#    XERCESROOT e.g. /nusoft/app/externals/xerces_c/v3_1_1/Linux64bit+2.6-2.5-e2-debug
#
# if WITH_G4NU G4NuPhysicsLists, then ${G4NU} must exist
#
#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(G4ANNIE)

option(WITH_G4NU "build against G4NuPhysicsLists" OFF)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#find_package(CLHEP REQUIRED)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories($ENV{XERCESCROOT}/include)
include_directories($ENV{ROOTSYS}/include)
include_directories($ENV{GEANT4_FQ_DIR}/include)

include_directories(${PROJECT_SOURCE_DIR}/include)

include_directories(${G4NUPL}/include)

set(_root_internal_libraries
    Core
    RIO
    Net
    Hist
    Graf
    Graf3d
    Gpad
    Tree
    Rint
    Postscript
    Matrix
    Physics
    MathCore
    Thread
    EG
    # for GENIE
    Geom
    EGPythia6
)
set(Root_LIBRARIES ${_root_internal_libraries})

link_directories($ENV{ROOTSYS}/lib)

#----------------------------------------------------------------------------
# GENIE stuff
#

include_directories($ENV{GENIE_INSTALL}/include/GENIE)
#link_directories($ENV{GENIE_INSTALL}/lib)
#include_directories("/usr/lib64")

#set(_genie_internal_libraries
#    GFwAlg-3.0.6.so
#    GFwEG-3.0.6.so
#    GFwGHEP-3.0.6.so
#    GFwInt-3.0.6.so
#    GFwMsg-3.0.6.so
#    GFwNtp-3.0.6.so
#    #GFwNum-3.0.6.so
#    GFwParDat-3.0.6.so
#    GFwReg-3.0.6.so
#    GFwUtl-3.0.6.so
#    GPhAMNGEG-3.0.6.so
#    GPhAMNGXS-3.0.6.so
#    GPhChmXS-3.0.6.so
#    GPhCmn-3.0.6.so
#    GPhCohEG-3.0.6.so
#    GPhCohXS-3.0.6.so
#    GPhDISEG-3.0.6.so
#    GPhDISXS-3.0.6.so
#    GPhDcy-3.0.6.so
#    GPhDeEx-3.0.6.so
#    GPhDfrcEG-3.0.6.so
#    GPhDfrcXS-3.0.6.so
#    GPhGlwResEG-3.0.6.so
#    GPhGlwResXS-3.0.6.so
#    GPhHadTransp-3.0.6.so
#    GPhHadnz-3.0.6.so
#    GPhIBDEG-3.0.6.so
#    GPhIBDXS-3.0.6.so
#    GPhMEL-3.0.6.so
#    GPhMNucEG-3.0.6.so
#    GPhMNucXS-3.0.6.so
#    GPhNuElEG-3.0.6.so
#    GPhNuElXS-3.0.6.so
#    GPhNuclSt-3.0.6.so
#    GPhPDF-3.0.6.so
#    GPhQELEG-3.0.6.so
#    GPhQELXS-3.0.6.so
#    GPhResEG-3.0.6.so
#    GPhResXS-3.0.6.so
#    GPhStrEG-3.0.6.so
#    GPhStrXS-3.0.6.so
#    GPhXSIg-3.0.6.so
#    GTlFlx-3.0.6.so
#    GTlGeo-3.0.6.so
#)

include_directories($ENV{LOG4CPP_FQ_DIR}/include)
link_directories($ENV{LOG4CPP_FQ_DIR}/lib)

set(_log4cpp_internal_libraries log4cpp)

link_directories($ENV{XERCESCROOT}/lib)
link_directories($ENV{LIBXML2ROOT}/lib)

set(_xerces_internal_libraries xerces-c xml2)

set(Genie_LIBRARIES ${_genie_internal_libraries} ${_log4cpp_internal_libraries} ${_xerces_internal_libraries} ${_root_internal_libraries})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

if(WITH_G4NU)
   set(G4NU $ENV{G4NU})
   include_directories(${G4NU}/include)
   link_directories(${G4NU}/lib)
   set(G4NU_LIBRARIES g4nupl)
   list(REMOVE_ITEM sources ${PROJECT_SOURCE_DIR}/src/AltG4PhysListFactory.cc)
endif()


#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(g4annie_dirt_flux g4annie_dirt_flux.cc ${sources} ${headers})
target_link_libraries(g4annie_dirt_flux ${Geant4_LIBRARIES} ${Root_LIBRARIES} ${G4NU_LIBRARIES})#${Genie_LIBRARIES}

if(WITH_G4NU)
  SET_TARGET_PROPERTIES(g4annie_dirt_flux PROPERTIES COMPILE_FLAGS -DWITH_G4NU )
endif()

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B1. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
#set(EXAMPLEG4SA_SCRIPTS
#  g4sa.mac
#  newgeom.mac
#  rndmSeed.mac
#  run_e.mac
#  )
#
#foreach(_script ${EXAMPLEG4SA_SCRIPTS})
#  configure_file(
#    ${PROJECT_SOURCE_DIR}/${_script}
#    ${PROJECT_BINARY_DIR}/${_script}
#    COPYONLY
#    )
#endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(G4SA DEPENDS g4annie_dirt_flux)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS g4annie_dirt_flux DESTINATION bin)
