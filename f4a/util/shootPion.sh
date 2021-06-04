#!/bin/bash

if [ -z "$DRICH_F4A" ]; then
  source $(dirname $(realpath $0))/../env.sh
fi

# call macros
macroDir=$DRICH_F4A/drich/macros
pushd $macroDir
root -l shootPion.C'(10000,0,0)' # nEvents, enableGUI, verbose
root -b -q hitHistos.C
popd

# move output file(s) to out/
mkdir -p out
mv -v $macroDir/*.root out/
