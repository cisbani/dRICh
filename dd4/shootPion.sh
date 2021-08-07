#!/bin/bash

# arguments
if [ $# -lt 3 ]; then
  echo ""
  echo "USAGE: $0 [a=athena,d=drichOnly] [r=run,v=vis] [runTest]"
  echo "  run tests:"
  echo "    1: aim pions at center of aerogel sector"
  echo "    2: inner edge test"
  echo "    3: outer edge test"
  echo "    4: radial scan test"
  echo "    5: azimuthal+radial scan test"
  echo "   10: spray test"
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
function pyc { python -c "import math; print($1)"; }
sep


# source environment variables
source environ.sh


#######################################################
# build macro file ####################################
wd=$(pwd)
macroCommon="${wd}/macro/${runType}_common.mac"
macroFile="${wd}/macro/tmp.mac"

## common settings ######################
cat $macroCommon > $macroFile

## particle type and energy ######################
if [ $runTest -eq 10 ]; then
  particle="gamma"
  energy="10.0 GeV"
else
  particle="pi+"
  energy="8.0 GeV" # test gas ring
  #energy="1.0 GeV" # test aerogel ring
fi
cat << EOF >> $macroFile
/gps/verbose 2
/gps/particle $particle
/gps/number 1
#/gps/ene/type Gauss
/gps/ene/mono  $energy
#/gps/ene/sigma 3.0 GeV
EOF

## source settings ######################
cat << EOF >> $macroFile
#/gps/pos/type Volume
#/gps/pos/shape Cylinder
#/gps/pos/centre 0.0 0.0 0.0 cm
#/gps/pos/radius 0.01 cm
#/gps/pos/halfz  10 cm
/gps/position 0 0 0 cm
EOF

## directions and run ######################

### dimensions of envelope
z0=155.0 # z-position of entrance window (snout frontplane)
sl=50.0 # snout length
z1=$(pyc "$z0+$sl") # z of cylinder frontplane
r0=10.0 # bore radius at entrance window
r1=110.0 # snout frontplane radius
r2=125.0 # snout backplane radius (cylinder entrance window)

### acceptance limits
# note: rmin and rmax are radial limits, at the snout entrance
# - rmin limited by bore radius at snout entrance; a buffer
#   is used to account for thicknesses etc.
rmin=$r0
rmin=$(pyc "$rmin + 1.0") # buffer
# - rmax limited by snout backplane radius at snout backplane,
#   a value smaller than snout frontplane radius; cherenkov
#   photons from this angle will reflect off the mirror and
#   hit the outer edge of the vessel, so we use a buffer
#   to further decrease rmax to a reasonable range so the 
#   opticalphotons will reach the sensors
#     -  5 cm buffer -> start to see aerogel rings
#     - 20 cm buffer -> start to see gas rings
rmax=$(pyc "$r2 * $z0 / $z1")
rmax=$(pyc "$rmax - 20.0") # buffer

### set directions and runs
if [ $runTest -eq 1 ]; then
cat << EOF >> $macroFile
# aim at +x dRICh sector
/gps/direction 0.3 0.0 0.8
/run/beamOn $numEvents
EOF
elif [ $runTest -eq 2 ]; then
cat << EOF >> $macroFile
# inner edge of acceptance
/gps/direction $rmin 0 $z0
/run/beamOn $numEvents
EOF
elif [ $runTest -eq 3 ]; then
cat << EOF >> $macroFile
# outer edge of acceptance
/gps/direction $rmax 0 $z0
/run/beamOn $numEvents
EOF
elif [ $runTest -eq 4 ]; then
  numSteps=4 # works best if even
  step=$(pyc "($rmax-$rmin)/($numSteps-1)")
  numEvents=100 #override
  for x in `seq $rmin $step $rmax`; do
    cat << EOF >> $macroFile
# scan test
/gps/direction $x 0 $z0
/run/beamOn $numEvents
EOF
  done
elif [ $runTest -eq 5 ]; then
  # TODO: should have written this whole script in python....
  numSteps=4 # works best if even
  step=$(pyc "($rmax-$rmin)/($numSteps-1)")
  nPhi=42 # works best if this is an odd multiple of 6 (e.g., 18,30,42,54)
  numEvents=50 #override
  pi=3.141592
  for x in `seq $rmin $step $rmax`; do
    for p in `seq 1 1 $nPhi`; do
      phi=$(pyc "2*($p-1)*$pi/$nPhi")
      xrot=$(pyc "$x*math.cos($phi)")
      yrot=$(pyc "$x*math.sin($phi)")
      cat << EOF >> $macroFile
# scan test
/gps/direction $xrot $yrot $z0
/run/beamOn $numEvents
EOF
    done
  done
elif [ $runTest -eq 10 ]; then
cat << EOF >> $macroFile
# spray test
/gps/ang/type iso
/gps/ang/mintheta 5 degree
/gps/ang/maxtheta 30 degree
/run/beamOn $numEvents
EOF
fi

# END build macro file ####################################
###########################################################



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
