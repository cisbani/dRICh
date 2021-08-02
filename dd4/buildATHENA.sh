#!/bin/bash

if [ $# -lt 1 ]; then
  echo ""
  echo "USAGE: $0 [a=athena,d=drichOnly] [c=clean(default off)]"
  echo ""
  echo "EXAMPLES: $0 a c = full athena, clean build"
  echo "          $0 d   = dRICh only, not clean build"
  echo ""
  exit 1
fi
detector=$1
if [ $# -ge 2 ]; then clean=1; else clean=0; fi

source environ.sh

wdir=$(pwd)
outdir=${wdir}/out
mkdir -p $outdir

pushd $DRICH_DD4_ATHENA

if [ "$detector" = "a" ]; then
  compactFile="athena.xml"
elif [ "$detector" = "d" ]; then
  compactFile="athena_drich.xml"
  ln -svf ${wdir}/compact/${compactFile} ./
fi
echo "compactFile = $compactFile"

if [ $clean -eq 1 ]; then
  echo "clean build dir... done"
  mkdir -p build
  rm -rv build
fi

cmake -B build -S . \
  -DCMAKE_INSTALL_PREFIX=$ATHENA_PREFIX \
  -DCMAKE_CXX_STANDARD=17 \
  &&\
cmake --build build -j$(grep -c processor /proc/cpuinfo) -- install &&\
dd_web_display --export $compactFile &&\
mv detector_geometry.root $outdir/ &&\
popd &&\
exit 0

popd
exit 1
