source /home/app/cern/geant4/10.07/bin/geant4.sh
source /home/app/cern/geant4/10.07/share/Geant4-10.7.0/geant4make/geant4make.sh

source /home/app/cern/root/6.22/bin/thisroot.sh

export G4E_HOME=/space/cisbani/escalate/g4e/g4e-master/

libhep=/space/cisbani/escalate/hepmc/hepmc-HEPMC_02_06_09/lib/

libvgm=/home/app/cern/vgm/lib

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$libhep:$libvgm

