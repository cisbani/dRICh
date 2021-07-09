#!/bin/bash
# alias for checking for overlaps

if [ $# -ne 1 ]; then
  echo """
  USAGE: $0 [check]
    check:
      1 = TGeo
      2 = Geant4
    ideally you want to run both the checks
  """
  exit 1
fi

xmlfile=${DRICH_DD4_ATHENA}/athena.xml

case $1 in
  1)
    checkOverlaps -c $xmlfile
    ;;
  2)
    python ${DRICH_DD4_ATHENA}/scripts/checkOverlaps.py -c $xmlfile
    ;;
  *)
    echo "ERROR: unknown check"
    exit 1
esac
