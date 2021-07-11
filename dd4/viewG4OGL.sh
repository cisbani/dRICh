#!/bin/bash

source environ.sh

npsim \
  --runType vis \
  --compactFile $DRICH_DD4_ATHENA/athena.xml \
  --random.seed 1 \
  --enableGun \
  --gun.energy 2*GeV \
  --gun.thetaMin 90*deg \
  --gun.thetaMax 90*deg \
  --gun.distribution uniform \
  --macro macros/vis.mac \
  --outputFile out/sim.root
