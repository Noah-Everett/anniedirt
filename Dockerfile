###########################################################
##################### Dockerfile Info #####################
###########################################################
###                                                     ###
### ANNIEDirt (other names include g4dirt and g4annie)  ###
###                                                     ###
### Created by Noah Everett                             ###
###     noah.everett@mines.sdsmt.edu                    ###
###                                                     ###
### Dockerfile is based on GENIE Dockerfile created     ###
### by James Minock which was based on ToolAnalysis     ###
### Dockerfile created by Dr. Benjamin Richards.        ###
###                                                     ###
###########################################################
###########################################################



##############################
########## Set FROM ##########
##############################
FROM quay.io/centos/centos:stream8



##############################
########## Set User ##########
##############################
USER root



#######################################
########## Build Environment ##########
#######################################
RUN dnf config-manager --set-enabled extras,powertools
RUN yum install -y gcc
RUN yum install -y gcc-c++
RUN yum install -y gcc-gfortran
RUN yum install -y cmake
RUN yum install -y make
RUN yum install -y git
WORKDIR /
ENV NCPU="10"



#############################
########## log4cpp ##########
#############################
RUN git clone --depth 1 --branch v2.9.1 https://github.com/orocos-toolchain/log4cpp log4cpp_src
RUN mkdir log4cpp_build
RUN mkdir log4cpp_install
WORKDIR log4cpp_build
RUN cmake ../log4cpp_src -DCMAKE_INSTALL_PREFIX=../log4cpp_install
RUN make -j${NCPU}
RUN make install
RUN cp /log4cpp_install/lib/liborocos-log4cpp.so /log4cpp_install/lib/liblog4cpp.so
WORKDIR /
RUN rm -rf log4cpp_src
RUN rm -rf log4cpp_build
ENV LOG4CPP_FQ_DIR="/log4cpp_install"
ENV LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/log4cpp_install/lib"



############################
########## fsplit ##########
############################
RUN yum install -y wget
RUN mkdir fsplit
WORKDIR fsplit
RUN wget https://gist.githubusercontent.com/marc1uk/c0e32d955dd1c06ef69d80ce643018ad/raw/10e592d42737ecc7dca677e774ae66dcb5a3859d/fsplit.c
RUN gcc fsplit.c -o fsplit
WORKDIR /
ENV PATH="/fsplit/:${PATH}"	



#############################
########## Pythia6 ##########
#############################
RUN git clone --depth 1 https://github.com/GENIE-MC-Community/Pythia6Support.git Pythia6
WORKDIR Pythia6
RUN ./build_pythia6.sh --dummies=keep
WORKDIR /
ENV PYTHIA6_DIR="/Pythia6/v6_424/"
ENV PYTHIA6_INCLUDE_DIR="/Pythia6/v6_424/inc/"
ENV PYTHIA6_LIBRARY="/Pythia6/v6_424/lib/"
ENV LD_LIBRARY_PATH="/Pythia6/v6_424/lib:${LD_LIBRARY_PATH}"



###########################
########## Expat ##########
###########################
RUN git clone --depth 1 --branch R_2_5_0 https://github.com/libexpat/libexpat Expat_src
RUN mkdir Expat_build
RUN mkdir Expat_install
WORKDIR Expat_build
RUN cmake ../../Expat_src/expat -DCMAKE_INSTALL_PREFIX=../Expat_install
RUN make -j${NCPU}
RUN make install
WORKDIR /
RUN rm -rf Expat_src
RUN rm -rf Expat_build



##############################
########## Xerces-C ##########
##############################
RUN git clone --depth 1 --branch v3.2.4 https://github.com/apache/xerces-c Xerces-C_src
RUN mkdir Xerces-C_build
RUN mkdir Xerces-C_install
WORKDIR Xerces-C_build
RUN cmake ../Xerces-C_src -DCMAKE_INSTALL_PREFIX=../Xerces-C_install
RUN make -j${NCPU}
RUN make install
WORKDIR /
RUN rm -rf Xerces-C_src
RUN rm -rf Xerces-C_build
ENV XERCESCROOT="/Xerces-C_install"
ENV LD_LIBRARY_PATH="/Xerces-C_install/lib64:${LD_LIBRARY_PATH}"



##########################
########## ROOT ##########
##########################
RUN yum install -y python27
RUN yum install -y libxml2-devel
RUN yum install -y gsl-devel
RUN git clone --depth 1 --branch v6-26-10 https://github.com/root-project/root ROOT_src
RUN mkdir ROOT_build
RUN mkdir ROOT_install
WORKDIR ROOT_build
RUN cmake ../ROOT_src -DCMAKE_INSTALL_PREFIX=../ROOT_install \
                      -Dx11=OFF \
                      -Dminimal=ON \
                      -Dgdml=ON \
                      -Dmathmore=ON \
                      -Dpythia6=ON \
                      -Dxml=ON
RUN make -j${NCPU}
RUN make install
WORKDIR /
RUN rm -rf ROOT_src
RUN rm -rf ROOT_build
ENV ROOTSYS="/ROOT_install"
ENV LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/ROOT_install/lib"
ENV PATH="${PATH}:/ROOT_install/bin"	



############################
########## Geant4 ##########
############################
RUN git clone --depth 1 --branch v10.7.2 https://github.com/Geant4/geant4 Geant4_src
RUN mkdir Geant4_build
RUN mkdir Geant4_install
WORKDIR Geant4_build
RUN cd /Geant4_build
RUN cmake ../Geant4_src -DCMAKE_INSTALL_PREFIX=../Geant4_install  \
                        -DCMAKE_PREFIX_PATH=/Xerces-C_install \
                        -DCMAKE_CXX_STANDARD=14 \
                        -DGEANT4_USE_GDML=ON \
                        -DGEANT4_USE_SYSTEM_EXPAT=OFF \
                        -DGEANT4_INSTALL_DATA=ON
RUN make -j${NCPU}
RUN make install
RUN source /Geant4_install/bin/geant4.sh
WORKDIR /
RUN rm -rf Geant4_src
RUN rm -rf Geant4_build
ENV LD_LIBRARY_PATH="/Geant4_install/lib64:${LD_LIBRARY_PATH}"



###########################
########## GENIE ##########
###########################
RUN yum install -y quota
RUN yum install -y patch
RUN yum install -y libnsl2-devel
RUN git clone --depth 1 --branch Develop/MacARM https://github.com/Noah-Everett/GENIE-Generator GENIE_src
RUN mkdir /GENIE_install
WORKDIR GENIE_src
ENV GENIE="/GENIE_src"
RUN ./configure --prefix=/GENIE_install \
                --with-libxml2-inc=/usr/include/libxml2 \
                --with-libxml2-lib=/usr/lib64 \
                --with-pythia6-lib=/Pythia6/v6_424/lib \
                --with-log4cpp-inc=/log4cpp_install/include/oroco \
                --with-log4cpp-lib=/log4cpp_install/lib \
                --enable-fnal \
                --disable-lhapdf5
                #--disable-dylibversion \
RUN make -j${NCPU}
RUN make install
WORKDIR /
RUN mv /GENIE_src/VERSION /GENIE_install
RUN mv /GENIE_src/config /GENIE_install
RUN rm -rf GENIE_src
ENV GENIE_INSTALL="/GENIE_install"
#ENV GENIE_INSTALL="/GENIE_src"
ENV GENIE="/GENIE_install"
ENV LD_LIBRARY_PATH="/GENIE_install/lib:${LD_LIBRARY_PATH}"



###############################
########## ANNIEDirt ##########
###############################
RUN echo "yesssssss"
RUN git clone --depth 1 --branch Docker https://github.com/Noah-Everett/anniedirt ANNIEDirt_src
RUN mkdir ANNIEDirt_build
RUN mkdir ANNIEDirt_install
WORKDIR ANNIEDirt_build
RUN cmake ../ANNIEDirt_src -DCMAKE_INSTALL_PREFIX=/ANNIEDirt_install \
                           -DCMAKE_CXX_STANDARD=14
RUN make -j${NCPU}
RUN make install
#RUN cp /ANNIEDirt_src/run_g4dirt.sh /ANNIEDirt_install
RUN cp /ANNIEDirt_src/run_g4dirt.sh /home
#RUN cp /ANNIEDirt_src/annie_v04.gdml /ANNIEDirt_install
RUN cp /ANNIEDirt_src/annie_v04.gdml /home
RUN cp /ANNIEDirt_install/bin/g4annie_dirt_flux /home
WORKDIR /
RUN rm -rf ANNIEDirt_src
RUN rm -rf ANNIEDirt_build



##############################
########## Clean Up ##########
##############################
RUN yum clean all
RUN rm -rf /vr/cache/yum
