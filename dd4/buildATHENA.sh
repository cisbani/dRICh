#!/bin/bash

outdir=${PWD}/out
mkdir -p $outdir

pushd $DRICH_DD4_ATHENA
cmake -B build -S . \
  -DCMAKE_INSTALL_PREFIX=$ATHENA_PREFIX \
  -DCMAKE_CXX_STANDARD=17 \
  &&\
cmake --build build -j$(grep -c processor /proc/cpuinfo) -- install &&\
dd_web_display --export athena.xml &&\
mv detector_geometry.root $outdir/

popd
