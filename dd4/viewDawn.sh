#!/bin/bash

source environ.sh

pushd $DRICH_DD4_ATHENA

export DETECTOR_PATH=$PWD
./bin/make_dawn_views -t view01 -d scripts/view1 -D
#./bin/make_dawn_views -t view01_ev001 -d scripts/view1 -s 1

popd
