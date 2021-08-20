#!/bin/bash
source environ.sh
mkdir -p doc
docfile=$(pwd)/doc/detector.md
pushd $DRICH_DD4_ATHENA
bin/build_documentation | tee $docfile
#bin/build_documentation
popd
