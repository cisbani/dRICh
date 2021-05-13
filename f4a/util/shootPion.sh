#!/bin/bash

if [ -z "$DRICH_F4A" ]; then
  source $(dirname $(realpath $0))/../env.sh
fi

# call macro
macroDir=$DRICH_F4A/drich/macros
pushd $macroDir
root -l shootPion.C'(10,0)'
popd

# move output file(s) to out/
mkdir -p out
mv -v $(ls -t $macroDir/*.root | head -n1) out/
