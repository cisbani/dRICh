#!/bin/bash

### start local singularity container

# set this to the path to the fun4all singularity repo
# [ https://github.com/eic/Singularity.git ]
fundir=${HOME}/d/fun4all/Singularity
if [ $# -ge 1 ]; then fundir=$1; fi

# start singularity container, and source EIC env vars
singularity shell \
  -B $fundir/cvmfs:/cvmfs \
  $fundir/cvmfs/eic.opensciencegrid.org/singularity/rhic_sl7_ext.simg \
  -c "source /cvmfs/eic.opensciencegrid.org/x8664_sl7/opt/fun4all/core/bin/eic_setup.sh -n && exec bash"
