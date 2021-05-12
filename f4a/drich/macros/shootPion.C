#ifndef MACRO_SHOOTPION_C
#define MACRO_SHOOTPION_C

// in fun4all singularity container, these include directives
// are with respect to $ROOT_INCLUDE_PATH

// - $DRICH_HOME/f4a/install/include
#include <drich/dRichSubsystem.h>

// - $OFFLINE_MAIN/include
#include <g4detectors/PHG4DetectorSubsystem.h>
#include <g4histos/G4HitNtuple.h>
#include <g4main/PHG4ParticleGenerator.h>
#include <g4main/PHG4ParticleGeneratorBase.h>
#include <g4main/PHG4ParticleGun.h>
#include <g4main/PHG4Reco.h>
#include <g4main/PHG4SimpleEventGenerator.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllDummyInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>
#include <phool/recoConsts.h>

// - $G4_MAIN/include
#include <Geant4/G4SystemOfUnits.hh>

// load libs
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4detectors.so)
R__LOAD_LIBRARY(libdRich.so)
R__LOAD_LIBRARY(libg4histos.so)

void shootPion(int nEvents = -1, Bool_t enableGUI=false) {

  // start server
  Fun4AllServer * f4a = Fun4AllServer::instance();
  recoConsts * rc = recoConsts::instance();
  //rc->set_IntFlag("RANDOMSEED",12345); // fix seed to reproduce results

  /* particle names:
   * pi+,pi-,pi0,
   * e-,e+,
   * kaon+,kaon-,kaon0,kaon0L,kaon0S,
   * proton,anti_proton,
   * mu+,mu-,
   * gamma,
   */

  /*// particle generator: spray particles within specified bounds
  PHG4ParticleGenerator * gen = new PHG4ParticleGenerator();
  gen->set_name("pi+");
  gen->set_mom_range(1.0, 1.0);
  gen->set_z_range(0., 0.);
  gen->set_vtx(0, 0, -5*m);
  //gen->set_phi_range(-3.141592,3.141592); // default 2pi
  gen->set_eta_range(1, 3);
  f4a->registerSubsystem(gen);
  */

  // particle gun: shoot particle in specified direction
  PHG4ParticleGun *gun = new PHG4ParticleGun();
  gun->set_name("pi+");
  gun->set_vtx(0, 0, -1*m);
  gun->set_mom(0.07*GeV, 0.07*GeV, 1*GeV);
  f4a->registerSubsystem(gun);

  // start geant4 server, and add detector modules 
  // - main geant4 module
  PHG4Reco *g4 = new PHG4Reco();
  g4->set_field(0);
  g4->save_DST_geometry(false); // disable this, if not tracking
  // - dRICh module
  dRichSubsystem *drichSubsys = new dRichSubsystem("dRich");
  drichSubsys->SetActive();
  g4->registerSubsystem(drichSubsys);
  // - register geant4 module
  f4a->registerSubsystem(g4);

  // hits module
  G4HitNtuple *hits = new G4HitNtuple("Hits");
  hits->AddNode("dRich_0", 0); /* NOTE:
      * node name must match the
      * subsystem name, appended with "_detectorID"; the
      * second argument of AddNode is the detector ID
      */
  f4a->registerSubsystem(hits);

  // dummy input manager, to drive event loop
  Fun4AllInputManager *dum = new Fun4AllDummyInputManager("Dummy");
  f4a->registerInputManager(dum);
  
  // eventloop
  if(nEvents>0) {
    f4a->run(nEvents);
    cout << "Print out Detector parameters" << endl;
    drichSubsys->Print();
  };

  // open Qt GUI, to visualize detector,
  // with or without a single event
  if(nEvents<=1 && enableGUI) {
    if(nEvents<=0) g4->InitRun(f4a->topNode());
    //g4->ApplyDisplayAction();
    //g4->ApplyCommand("/control/execute init_gui_vis.mac");
    //g4->ApplyCommand("/control/execute vis.mac");
    g4->StartGui(); // start Qt
  };


  // cleanup and exit
  f4a->End();
  if(g4) delete g4;
  if(f4a) delete f4a;
  gSystem->Exit(0);

};

#endif
