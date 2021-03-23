# dRICH

Code for the EIC dual radiator RICH detector
- Model for escalate/g4e simulation
- Analysis syftware
- Reconstruction

ci_DRICH: the escalate/g4e DRICH subdetector source coude

config: the g4e DRICH config/macro files
  drich.mac : macro file
  drich-model.txt : the geometry and main material model text file

utils: utility files, for current development; probably removed in the future, stable release

# Approach

The geometrical model is described in a GEANT4-text file (https://geant4.web.cern.ch/sites/geant4.web.cern.ch/files/geant4/collaboration/working_groups/geometry/docs/textgeom/textgeom.pdf)

# Geant4 Parameters

dRICH specific Geant4 macro commands are defined in ci_DRICH_Config
They include definition of model parameters that cannot be specified in the geometry file.

All geometrical related parameters are defined in the Geant4 geometry-text file

# Geant4 Naming Convention:

A component name is prefixed by "ciDRICH" and then the specific name of the component; e.g.: ciDRICHaerogel.
This name is used, if there are no ambiguities for the Geant4 name of the related component classes:
 - logical volume
 - one or more physical (placed) volumes
 - material for high energy particles and optical simulation
 - one or more surfaces for optical simulation

E.g. "ciDRICHaerogel" is the name of the logical and physical volumes, the material of the radiator and the aerogel-gas surface (if defined); a postfix is admitted for clarity; e.g. ciDRICHaerogelMat

Whenever applicable programmatic of C++ global variable names will have the same convention of the above name; or a truncated component name (at least 3 letters) and a postfix attribute; e.g.:

 ciDRICHaerogel
 ciDRICHaerLVol
 ciDRICHaerPVol
 ciDIRCHaerMat
 ...

if the variable is not global, the "ciDRICH" prefix is omitted.


Geometry file parameter starts with the component or quantity name (small letters) followed by attributes in camel case way :

 vesselInRadius
 petalPhiStart