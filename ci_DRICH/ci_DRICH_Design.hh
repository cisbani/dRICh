#ifndef G4E_CI_DRICH_DESIGN_HH
#define G4E_CI_DRICH_DESIGN_HH


#include <G4PVDivision.hh>
#include "G4RotationMatrix.hh"
#include "G4Material.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"
#include "TMath.h"
#include "TLine.h"

#include "TGeoManager.h"

#include "Geant4GM/volumes/Factory.h"
#include "RootGM/volumes/Factory.h"
#include "ci_DRICH_Config.hh"


class ci_DRICH_Design {
public:
  void Construct(ci_DRICH_Config cfg, G4Material* worldMaterial,  G4VPhysicalVolume *motherVolume);

  void ConstructDetectors();

  G4Tubs *Solid;      //pointer to the solid
  G4LogicalVolume *Logic;    //pointer to the logical
  G4VPhysicalVolume *Phys;  //pointer to the physical
  G4VPhysicalVolume *RichWorld; // Physical volume of internals of rich detector

  /// Parameters that was used in the moment of construction
  ci_DRICH_Config  ConstructionConfig;

  std::vector<G4VPhysicalVolume*> PhotoDets;

  void PrintConfig();
  
private:

    // define here local variables and parameter of detectors

    G4VPhysicalVolume *RichVessel;

};


#endif //G4E_CI_DRICH_DESIGN_HH
