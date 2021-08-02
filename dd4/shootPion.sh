#!/bin/bash

source environ.sh

npsim \
  --runType vis \
  --outputFile out/sim.root \
  --macroFile macro/pion.mac \
  --enableG4GPS \
  --compactFile $DRICH_DD4_ATHENA/athena.xml
  #--enableQtUI \

# TODO make standalone dRICh_athena.xml file, for convenience
