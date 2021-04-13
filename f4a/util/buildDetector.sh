#!/bin/bash

if [ $# -ge 1 ]; then clean=1; else clean=0; fi
if [ -z "$DRICH_F4A" ]; then
  source $(dirname $(realpath $0))/../env.sh
fi

dirDet=$DRICH_F4A/drich
dirSource=$dirDet/source
dirBuild=$dirDet/build
dirInstall=$DRICH_F4A/install
echo "source dir  = $dirSource"
echo "build dir   = $dirBuild"
echo "install dir = $dirInstall"

mkdir -p $dirBuild
if [ $clean -eq 1 ]; then
  echo "clean build dir... done"
  rm -r $dirBuild
  mkdir -p $dirBuild
fi

pushd $dirBuild
$dirSource/autogen.sh --prefix=$dirInstall && make install
popd

