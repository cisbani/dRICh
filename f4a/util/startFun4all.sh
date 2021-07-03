#!/bin/bash

### start local singularity container

# set this to the path to the fun4all singularity repo
# [ https://github.com/eic/Singularity.git ]
fundir=${HOME}/d/fun4all/Singularity
if [ $# -ge 1 ]; then fundir=$1; fi

# start singularity container, and source EIC env vars
# - standard container, suitable for batch jobs
singularity shell \
  -B $fundir/cvmfs:/cvmfs \
  $fundir/cvmfs/eic.opensciencegrid.org/singularity/rhic_sl7_ext.simg
