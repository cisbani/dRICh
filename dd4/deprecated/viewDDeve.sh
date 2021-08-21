#!/bin/bash

source environ.sh

pushd $DRICH_DD4_ATHENA
ddeve -config compact/subsystem_views/drich_only.xml
popd
