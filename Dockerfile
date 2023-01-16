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
RUN dnf config-manager --set-enabled extras,powertools \
 && yum install -y gcc \
                   gcc-c++ \
                   gcc-gfortran \
                   cmake \
                   make \
                   git \
                   wget \
                   python27 \
                   libxml2-devel \
                   gsl-devel \
                   quota \
                   patch \
                   libnsl2-devel \
 && yum clean all \
 && rm -rf /vr/cache/yum \
 && export NCPU="10" \
 && wget https://raw.githubusercontent.com/Noah-Everett/anniedirt/Docker/setup_Dockerfile.sh



#############################
########## log4cpp ##########
#############################
RUN source /setup_Dockerfile.sh \
 && git clone --depth 1 --branch v2.9.1 https://github.com/orocos-toolchain/log4cpp /log4cpp_src \
 && mkdir /log4cpp_build \
 && mkdir /log4cpp_install \
 && cd /log4cpp_build \
 && cmake /log4cpp_src -DCMAKE_INSTALL_PREFIX=/log4cpp_install \
 && make -j${NCPU} \
 && make install \
 && cp /log4cpp_install/lib/liborocos-log4cpp.so /log4cpp_install/lib/liblog4cpp.so \
 && cd / \
 && rm -rf /log4cpp_src \
 && rm -rf /log4cpp_build



############################
########## fsplit ##########
############################
RUN source /setup_Dockerfile.sh \
 && mkdir /fsplit \
 && cd /fsplit \
 && wget https://gist.githubusercontent.com/marc1uk/c0e32d955dd1c06ef69d80ce643018ad/raw/10e592d42737ecc7dca677e774ae66dcb5a3859d/fsplit.c \
 && gcc fsplit.c -o fsplit \
 && cd /



#############################
########## Pythia6 ##########
#############################
RUN source /setup_Dockerfile.sh \
 && git clone --depth 1 https://github.com/GENIE-MC-Community/Pythia6Support.git /Pythia6 \
 && cd /Pythia6 \
 && ./build_pythia6.sh --dummies=keep \
 && cd /



###########################
########## Expat ##########
###########################
RUN source /setup_Dockerfile.sh \
 && git clone --depth 1 --branch R_2_5_0 https://github.com/libexpat/libexpat /Expat_src \
 && mkdir /Expat_build \
 && mkdir /Expat_install \
 && cd /Expat_build \
 && cmake /Expat_src/expat -DCMAKE_INSTALL_PREFIX=/Expat_install \
 && make -j${NCPU} \
 && make install \
 && cd / \
 && rm -rf /Expat_src \
 && rm -rf /Expat_build



##############################
########## Xerces-C ##########
##############################
RUN source /setup_Dockerfile.sh \
 && git clone --depth 1 --branch v3.2.4 https://github.com/apache/xerces-c /Xerces-C_src \
 && mkdir /Xerces-C_build \
 && mkdir /Xerces-C_install \
 && cd /Xerces-C_build \
 && cmake /Xerces-C_src -DCMAKE_INSTALL_PREFIX=/Xerces-C_install \
 && make -j${NCPU} \
 && make install \
 && cd / \
 && rm -rf /Xerces-C_src \
 && rm -rf /Xerces-C_build



##########################
########## ROOT ##########
##########################
RUN source /setup_Dockerfile.sh \
 && git clone --depth 1 --branch v6-26-10 https://github.com/root-project/root /ROOT_src \
 && mkdir /ROOT_build \
 && mkdir /ROOT_install \
 && cd /ROOT_build \
 && cmake /ROOT_src -DCMAKE_INSTALL_PREFIX=/ROOT_install \
                    -Dx11=OFF \
                    -Dminimal=ON \
                    -Dgdml=ON \
                    -Dmathmore=ON \
                    -Dpythia6=ON \
                    -Dxml=ON \
 && make -j${NCPU} \
 && make install \
 && cd / \
 && rm -rf /ROOT_src \
 && rm -rf /ROOT_build



############################
########## Geant4 ##########
############################
RUN source /setup_Dockerfile.sh \
 && git clone --depth 1 --branch v10.7.2 https://github.com/Geant4/geant4 /Geant4_src \
 && mkdir /Geant4_build \
 && mkdir /Geant4_install \
 && cd /Geant4_build \
 && cmake ../Geant4_src -DCMAKE_INSTALL_PREFIX=../Geant4_install  \
                        -DCMAKE_PREFIX_PATH=/Xerces-C_install \
                        -DCMAKE_CXX_STANDARD=14 \
                        -DGEANT4_USE_GDML=ON \
                        -DGEANT4_USE_SYSTEM_EXPAT=OFF \
                        -DGEANT4_INSTALL_DATA=ON \
 && make -j${NCPU} \
 && make install \
 && source /Geant4_install/bin/geant4.sh \
 && cd / \
 && rm -rf Geant4_src \
 && rm -rf Geant4_build \



###########################
########## GENIE ##########
###########################
RUN source /setup_Dockerfile.sh \
 && git clone --depth 1 --branch Develop/MacARM https://github.com/Noah-Everett/GENIE-Generator /GENIE \
 && cd GENIE \
 && ./configure --prefix=/GENIE_install \
                --with-libxml2-inc=/usr/include/libxml2 \
                --with-libxml2-lib=/usr/lib64 \
                --with-pythia6-lib=/Pythia6/v6_424/lib \
                --with-log4cpp-inc=/log4cpp_install/include/oroco \
                --with-log4cpp-lib=/log4cpp_install/lib \
                --enable-fnal \
                --disable-lhapdf5 \
 && make -j${NCPU} \
 && cd /



###############################
########## ANNIEDirt ##########
###############################
RUN source /setup_Dockerfile.sh \
 && git clone --depth 1 --branch Docker https://github.com/Noah-Everett/anniedirt /ANNIEDirt_src \
 && mkdir ANNIEDirt_build \
 && mkdir ANNIEDirt_install \
 && cd /ANNIEDirt_build \
 && cmake /ANNIEDirt_src -DCMAKE_INSTALL_PREFIX=/ANNIEDirt_install \
                         -DCMAKE_CXX_STANDARD=14 \
 && make -j${NCPU} \
 && make install \
 && cp /ANNIEDirt_src/run_g4dirt.sh /home \
 && cp /ANNIEDirt_src/annie_v04.gdml /home \
 && cp /ANNIEDirt_install/bin/g4annie_dirt_flux /home \
 && cd / \
 && rm -rf /ANNIEDirt_src \
 && rm -rf /ANNIEDirt_build