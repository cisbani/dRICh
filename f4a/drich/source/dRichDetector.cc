//____________________________________________________________________________..
//
// This is a working template for the G4 Construct() method which needs to be implemented
// We wedge a method between the G4 Construct() to enable volume hierarchies on the macro
// so here it is called ConstructMe() but there is no functional difference
// Currently this installs a simple G4Box solid, creates a logical volume from it
// and places it. Put your own detector in place (just make sure all active volumes
// get inserted into the m_PhysicalVolumesSet)
// 
// Rather than using hardcoded values you should consider using the parameter class
// Parameter names and defaults are set in dRichSubsystem::SetDefaultParameters()
// Only parameters defined there can be used (also to override in the macro)
// to avoids typos.
// IMPORTANT: parameters have no inherent units, there is a convention (cm/deg)
// but in any case you need to multiply them here with the correct CLHEP/G4 unit 
// 
// The place where you put your own detector is marked with
// //begin implement your own here://
// //end implement your own here://
// Do not forget to include the G4 includes for your volumes
//____________________________________________________________________________..

#include "dRichDetector.h"
#include <g4dRIChOptics.hh>
#include <ci_DRICH_Config.hh>

#include <phparameter/PHParameters.h>

#include <g4main/PHG4Detector.h>

#include <G4Box.hh>
#include <G4Color.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4VisAttributes.hh>
#include <G4tgbVolumeMgr.hh>

#include <cmath>
#include <iostream>

class G4VSolid;
class PHCompositeNode;

using std::string;
using std::cout;
using std::cerr;
using std::endl;

// ---------------------------------------------------
dRichDetector::dRichDetector(
  PHG4Subsystem *subsys,
  PHCompositeNode *Node,
  PHParameters *parameters,
  const string &dnam
) : PHG4Detector(subsys, Node, dnam), m_Params(parameters)
{}

// ---------------------------------------------------
int dRichDetector::IsInDetector(
  G4VPhysicalVolume *volume
) const {
  std::set<G4VPhysicalVolume *>::const_iterator iter =
    m_PhysicalVolumesSet.find(volume);
  if (iter != m_PhysicalVolumesSet.end()) {
    return 1;
  }
  return 0;
}

// ---------------------------------------------------
void dRichDetector::ConstructMe(G4LogicalVolume *logicWorld) {

  // get env vars
  char * drichHome = std::getenv("DRICH_HOME");
  if(drichHome==NULL) {
    cerr << "[+] ERROR: DRICH_HOME env var not set" << endl;
    return;
  };

  // load model text file and configuration
  ci_DRICH_Config cfg;
  cfg.model_file = G4String(drichHome) +
                   "/share/config/drich-g4model.txt";
  cout << "[+] MODEL TEXT FILE: " << cfg.model_file << endl;
  // - check existence
  std::ifstream mf(cfg.model_file.data());
  if(!mf.is_open()) {
    cerr << "[+] ERROR: cannot find MODEL TEXT FILE" << endl;
    return;
  };
  mf.close();


  // graphics
  G4VisAttributes * vis = new G4VisAttributes(G4Color(0., 0., 0.9, 0.5));
  vis->SetForceSolid(true);
  vis->SetVisibility(true);
  vis->SetLineWidth(1);
  vis->SetForceSolid(true);
  logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
  //logicWorld->SetVisAttributes(vis);


  // build detector by text file
  cout << "[+] build detector from model text file" << endl;
  G4tgbVolumeMgr * volmgr = G4tgbVolumeMgr::GetInstance();
  volmgr->AddTextFile(cfg.model_file);
  G4VPhysicalVolume * rVessel = 
    volmgr->ReadAndConstructDetector();
  cout << "[+] detector summary" << endl;
  volmgr->DumpSummary();
  cout << "[+] detector G4Solid list" << endl;
  volmgr->DumpG4SolidList();

  // add to logical world
  logicWorld->AddDaughter(rVessel);
  
  // add to volume list, for IsInDetector method
  m_PhysicalVolumesSet.insert(rVessel);


  /// BEGIN TEMPLATE CODE ///////////////////////

  /*
  // Do not forget to multiply the parameters with their respective CLHEP/G4 unit !
  double xdim = m_Params->get_double_param("size_x") * cm;
  double ydim = m_Params->get_double_param("size_y") * cm;
  double zdim = m_Params->get_double_param("size_z") * cm;
  G4VSolid *solidbox = new G4Box("dRichSolid", xdim / 2., ydim / 2., zdim / 2.);
  G4LogicalVolume *logical = new G4LogicalVolume(solidbox, G4Material::GetMaterial(m_Params->get_string_param("material")), "dRichLogical");

  G4VisAttributes *vis = new G4VisAttributes(G4Color(G4Colour::Grey()));  // grey is good to see the tracks in the display
  vis->SetForceSolid(true);
  logical->SetVisAttributes(vis);
  G4RotationMatrix *rotm = new G4RotationMatrix();
  rotm->rotateX(m_Params->get_double_param("rot_x") * deg);
  rotm->rotateY(m_Params->get_double_param("rot_y") * deg);
  rotm->rotateZ(m_Params->get_double_param("rot_z") * deg);

  G4VPhysicalVolume *phy = new G4PVPlacement(
      rotm,
      G4ThreeVector(m_Params->get_double_param("place_x") * cm,
        m_Params->get_double_param("place_y") * cm,
        m_Params->get_double_param("place_z") * cm),
      logical, "dRich", logicWorld, 0, false, OverlapCheck());
  // add it to the list of placed volumes so the IsInDetector method
  // picks them up
  m_PhysicalVolumesSet.insert(phy);
  //end implement your own here://
  */

  return;
}

// ---------------------------------------------------
void dRichDetector::Print(const string &what) const
{
  cout << "dRich Detector:" << endl;
  if (what == "ALL" || what == "VOLUME")
  {
    cout << "Version 0.1" << endl;
    cout << "Parameters:" << endl;
    m_Params->Print();
  }
  return;
}
