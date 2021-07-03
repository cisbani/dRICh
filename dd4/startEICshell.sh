#!/bin/bash
# wrapper for `eic-shell`, which starts the singularity container

# set env vars
source environ.sh

# start singularity shell
$DRICH_DD4_INSTALL/eic-shell

