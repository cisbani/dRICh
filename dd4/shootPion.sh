#!/bin/bash

# set compact file (full ATHENA or standalone dRICh)
if [ $# -lt 1 ]; then
  echo ""
  echo "USAGE: $0 [a=athena,d=drichOnly]"
  echo ""
  exit 1
fi
detector=$1
if [ "$detector" = "a" ]; then compactFile="athena.xml";
elif [ "$detector" = "d" ]; then compactFile="athena_drich.xml"; fi
echo "compactFile = $compactFile"

# source environment variables
source environ.sh

# check if you have a custom npdet install (see updateNPdet.sh)
extraArgs=""
if [ -d "$DRICH_DD4_INSTALL/../NPDet_install/bin" ]; then
  exe=$DRICH_DD4_INSTALL/../NPDet_install/bin/npsim
  extraArgs="--enableQtUI"
else
  exe=$(which npsim)
fi
echo "executable: $exe"

# run the simulation
wd=$(pwd)
pushd $DRICH_DD4_ATHENA
$exe \
  --runType vis \
  --compactFile ${compactFile} \
  --random.seed 1 \
  --macro ${wd}/macro/pion.mac \
  --outputFile ${wd}/out/sim.root \
  --enableG4GPS \
  ${extraArgs}
popd
