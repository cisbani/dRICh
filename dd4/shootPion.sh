#!/bin/bash

source environ.sh

npsim \
  --runType run \
  --outputFile out/sim.root \
  --macroFile macros/pion.mac \
  --enableG4GPS \
  --compactFile $DRICH_DD4_ATHENA/athena.xml
