#!/bin/bash
# wrapper for `eic-shell`, which starts the singularity container

# set env vars
source environ.sh

# if you have a local `root` install sourced, you may have `$LIBPATH`
# set, which will cause problems starting `eic-shell`; here we temporarily
# unset it, so we start "clean"
export LIBPATH_OLD=$LIBPATH
unset LIBPATH

# start singularity shell
#export LD_LIBRARY_PATH=/opt/detector/lib:$LD_LIBRARY_PATH
$DRICH_DD4_INSTALL/eic-shell

# revert environment
export LIBPATH=$LIBPATH_OLD
unset LIBPATH_OLD
