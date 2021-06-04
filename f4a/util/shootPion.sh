#!/bin/bash

# source environment
if [ -z "$DRICH_F4A" ]; then
  source $(dirname $(realpath $0))/../env.sh
fi

# arguments
if [ $# -eq 0 ]; then 
  echo ""
  echo "USAGE: $0 [numEvents] [enableGUI(1/0,optional)] [verbose(1/0,optional)]"
  exit 1
fi
nEvents=$1
enableGUI=0
verbose=0
if [ $# -ge 2 ]; then enableGUI=$2; fi
if [ $# -ge 3 ]; then verbose=$3; fi


# call macros
macroDir=$DRICH_F4A/drich/macros
pushd $macroDir
root -l shootPion.C'('$nEvents','$enableGUI','$verbose')'
root -b -q hitHistos.C
popd

# move output file(s) to out/
mkdir -p out
mv -v $macroDir/*.root out/
