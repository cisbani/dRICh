#!/bin/bash
# install / update NPdet
# clones latest version of NPDet, and builds it; all will appear in 
# $DRICH_DD4_INSTALL/../NPDet*

source environ.sh
pushd $DRICH_DD4_INSTALL/..
mkdir -p NPDet NPDet_build NPDet_install
rm -rf NPDet NPDet_build NPDet_install
git clone https://eicweb.phy.anl.gov/EIC/NPDet.git
mkdir -p NPDet_build NPDet_install
cd NPDet_build
cmake ../NPDet/. -DCMAKE_INSTALL_PREFIX=../NPDet_install
make -j$(grep -c processor /proc/cpuinfo)
make install
popd


