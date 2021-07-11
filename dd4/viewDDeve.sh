#!/bin/bash

source environ.sh

pushd $DRICH_DD4_ATHENA
ddeve -config athena.xml
popd
