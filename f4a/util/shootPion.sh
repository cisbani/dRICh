#!/bin/bash

if [ -z "$DRICH_F4A" ]; then
  source $(dirname $(realpath $0))/../env.sh
fi

pushd $DRICH_F4A/drich/macros
root -l shootPion.C'(1)'
popd

