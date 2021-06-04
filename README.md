# dRICh

Code for the EIC dual radiator RICH detector
1. Geant4 model (geometrical and optical)
1. Analysis software
1. Reconstruction
and porting to escalate/g4e and fun4all

## Repository structure

* `ci_DRICH`: the escalate/g4e dRICh subdetector source code

* `config`: the g4e dRICh config/macro files
  * drich.mac : macro file
  * drich-model.txt : the geometry and main material model text file

* `utils`: utility files, for current development; probably removed in the future, stable release

* other to come

## Approach

### Geometrical model

The dRICh is currently made of:
1. Vessel: is a `G4Cons`, covering 2pi azimuth and about 5 to 25 deg polar; 
   it contains the gas radiator and all other components; as a first guess its walls are made of aluminum material, 
   entrance and exit windows are thinner aluminum like foils; all other components are replicated into 6 virtual petals
1. Aerogel: the first radiator just after the entrance window
1. Acrylic Filter: after the aerogel
1. Mirror: a tilted spherical petal, near the exit window
1. Photosensor: it is made of a matrix of small tiles sensors; it is out of the dRICh charged particle acceptance; 
   it sits very approximately on the line which is symmetric to the (aerogel center - mirror middle point) line respect to the mirror radius ending in the mirror middle point, at a distance of 1/2 radius from the mirror middle point.

This geometrical model is described in the drich-model.txt according to the GEANT4-text file format [g4textgeom](https://geant4.web.cern.ch/sites/geant4.web.cern.ch/files/geant4/collaboration/working_groups/geometry/docs/textgeom/textgeom.pdf)

All parameters directly related to the geometrical model (and simple material) are defined in the drich-model.txt; e.g. detector position and size, material composition and densities ...

### Optical description

The optical description of the dRICh components (aerogel, acrylic filter, gas, mirror and sensors) are embedded into C++ classes derived from a generic "ciDRICHOptics" class. Each component has its optical specifications in terms of e.g. dispersion relation, absorption length ...

All parameters related to the optical description, and not connected to the geometrical model are defined as geant4 macro parameters (e.g. the filter cut-off frequency)

## escalate/g4e porting

### Geant4 Parameters

dRICh specific Geant4 macro commands are defined in ci_DRICH_Config
They include definition of model parameters that cannot be specified in the geometry file.

All geometrical related parameters are defined in the Geant4 geometry-text file

### Geant4 Naming Convention:

A component name is prefixed by "ciDRICH" and then the specific name of the component; e.g.: ciDRICHaerogel.
This name is used, if there are no ambiguities for the Geant4 name of the related component classes:
* logical volume
* one or more physical (placed) volumes
* material for high energy particles and optical simulation
* one or more surfaces for optical simulation

E.g. "ciDRICHaerogel" is the name of the logical and physical volumes, the material of the radiator and the aerogel-gas surface (if defined); a postfix is admitted for clarity; e.g. ciDRICHaerogelMat

Whenever applicable programmatic of C++ global variable names will have the same convention of the above name; or a truncated component name (at least 3 letters) and a postfix attribute; e.g.:

* ciDRICHaerogel
* ciDRICHaerLVol
* ciDRICHaerPVol
* ciDIRCHaerMat
* ...

if the variable is not global, the "ciDRICH" prefix is omitted.

Geometry file parameter starts with the component or quantity name (small letters) followed by attributes in camel case way :

* vesselInRadius
* petalPhiStart
