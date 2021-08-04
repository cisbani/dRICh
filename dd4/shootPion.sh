#!/bin/bash

# arguments
if [ $# -lt 3 ]; then
  echo ""
  echo "USAGE: $0 [a=athena,d=drichOnly] [r=run,v=vis] [runTest]"
  echo "  run tests:"
  echo "    1: aim pions at center of aerogel sector"
  echo "    2: edge tests"
  echo "    3: spray test"
  echo "    4: scan test"
  echo ""
  exit 1
fi
detector=$1
runcmd=$2
runTest=$3

# set compact file (full ATHENA or standalone dRICh)
if [ "$detector" = "a" ]; then compactFile="athena.xml";
elif [ "$detector" = "d" ]; then compactFile="athena_drich.xml";
else echo "ERROR: unknown detector"; exit 1; fi
echo "compactFile = $compactFile"


# set run type, and associated settings
if [ "$runcmd" = "r" ]; then
  runType="run"
  numEvents=100
elif [ "$runcmd" = "v" ]; then
  runType="vis"
  numEvents=1
else echo "ERROR: unknown run type"; exit 1; fi
echo "runType = $runType"

function sep { printf '%50s\n' | tr ' ' -; }
sep


# source environment variables
source environ.sh


# build macro file
wd=$(pwd)
macroCommon="${wd}/macro/${runType}_common.mac"
macroFile="${wd}/macro/tmp.mac"

## common settings
cat $macroCommon > $macroFile

## particle type and energy
if [ $runTest -eq 3 ]; then
  particle="gamma"
  energy="10.0 GeV"
else
  particle="pi+"
  energy="10.0 GeV"
fi
cat << EOF >> $macroFile
/gps/verbose 2
/gps/particle $particle
/gps/number 1
#/gps/ene/type Gauss
/gps/ene/mono  $energy
#/gps/ene/sigma 3.0 GeV
EOF

## source settings
cat << EOF >> $macroFile
#/gps/pos/type Volume
#/gps/pos/shape Cylinder
#/gps/pos/centre 0.0 0.0 0.0 cm
#/gps/pos/radius 0.01 cm
#/gps/pos/halfz  10 cm
/gps/position 0 0 0 cm
EOF

## directions and run
if [ $runTest -eq 1 ]; then
cat << EOF >> $macroFile
# aim at +x dRICh sector
/gps/direction 0.3 0.0 0.8
/run/beamOn $numEvents
EOF
elif [ $runTest -eq 2 ]; then
cat << EOF >> $macroFile
# edge tests
#/gps/direction 100 0 177 # outer edge of aerogel
#/run/beamOn $numEvents
/gps/direction 15 0 177 # inner edge of aerogel
/run/beamOn $numEvents
EOF
elif [ $runTest -eq 3 ]; then
cat << EOF >> $macroFile
# spray test
/gps/ang/type iso
/gps/ang/mintheta 5 degree
/gps/ang/maxtheta 30 degree
/run/beamOn $numEvents
EOF
elif [ $runTest -eq 4 ]; then
cat << EOF >> $macroFile
# scan test
/gps/direction 100 0 177
/run/beamOn $numEvents
/gps/direction 80 0 177 
/run/beamOn $numEvents
/gps/direction 60 0 177 
/run/beamOn $numEvents
/gps/direction 40 0 177 
/run/beamOn $numEvents
/gps/direction 20 0 177
/run/beamOn $numEvents
EOF
fi


## print full macro
echo "MACRO FILE"; sep; cat $macroFile; sep


# set output file
outputFile=${wd}/out/sim_${runType}.root


# check if you have a custom npdet install
# (see updateNPdet.sh)
extraArgs=""
if [ -d "$DRICH_DD4_INSTALL/../NPDet_install/bin" ]; then
  exe=$DRICH_DD4_INSTALL/../NPDet_install/bin/npsim
  extraArgs="--enableQtUI"
else
  exe=$(which npsim)
fi
echo "executable: $exe"
sep


# run npsim
pushd $DRICH_DD4_ATHENA
$exe \
  --runType $runType \
  --compactFile $compactFile \
  --random.seed 1 \
  --macro $macroFile \
  --outputFile $outputFile \
  --enableG4GPS \
  ${extraArgs}
popd
