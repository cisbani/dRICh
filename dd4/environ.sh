##########################
# set paths for your setup:
##########################

# path to `eic_container` installation
export DRICH_DD4_INSTALL=${HOME}/d/athena/install

# path to `EIC/detectors/athena` repository
export DRICH_DD4_ATHENA=${HOME}/d/athena/eic/detectors/athena


##########################
# paths for singularity container (do not edit)
##########################
if [ -f "/opt/detector/setup.sh" ]; then
  source /opt/detector/setup.sh
fi
