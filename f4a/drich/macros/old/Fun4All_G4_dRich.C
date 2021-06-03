#ifndef MACRO_FUN4ALLG4DRICH_C
#define MACRO_FUN4ALLG4DRICH_C

#include <drich/dRichSubsystem.h>

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

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4detectors.so)
R__LOAD_LIBRARY(libdRich.so)
R__LOAD_LIBRARY(libg4histos.so)

// needs 10000 geantinos to make a decent scan plot
void Fun4All_G4_dRich(int nEvents = 10000)
{
  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();
  // if you want to fix the random seed to reproduce results
  // set this flag
  // nail this down so I know what the first event looks like...
  //  rc->set_IntFlag("RANDOMSEED",12345);

  //
  // Particle Generator
  //

  // the PHG4ParticleGenerator makes cones using phi and eta
  PHG4ParticleGenerator *gen = new PHG4ParticleGenerator();
  gen->set_name("geantino");
  gen->set_mom_range(1.0, 1.0);
  gen->set_z_range(0., 0.);
  // experimentally found ranges, they cover the original block
  gen->set_vtx(-50, 0, 0);
  gen->set_phi_range(-14.5 / 180 * TMath::Pi(), 14.5 / 180 * TMath::Pi());
  gen->set_eta_range(-0.26, 0.26);
  se->registerSubsystem(gen);

  // ParticleGun shoots right into the original dRich volume
  PHG4ParticleGun *gun = new PHG4ParticleGun();
  //   gun->set_name("pi-");
  gun->set_name("geantino");
  gun->set_vtx(0, 0, -20);
  gun->set_mom(0, 0, 1);
  //   se->registerSubsystem(gun);

  //
  // Geant4 setup
  //
  PHG4Reco *g4Reco = new PHG4Reco();
  // setup of G4:
  //   no field
  //   no saving of geometry: it takes time and we do not do tracking
  //   so we do not need the geometry
  g4Reco->set_field(0);
  g4Reco->save_DST_geometry(false);

  dRichSubsystem *drichSS = new dRichSubsystem("dRich1");
  drichSS->SetActive();
  g4Reco->registerSubsystem(drichSS);

  // drichSS = new dRichSubsystem("dRich2");
  // drichSS->set_double_param("place_x",40.);
  // drichSS->set_double_param("place_y",40.);
  // drichSS->set_double_param("rot_x",45.);
  // drichSS->set_string_param("material","G4_Fe");
  // drichSS->SetActive();
  // g4Reco->registerSubsystem(drichSS);

  se->registerSubsystem(g4Reco);

  ///////////////////////////////////////////
  // Fun4All modules
  ///////////////////////////////////////////

  G4HitNtuple *hits = new G4HitNtuple("Hits");
  hits->AddNode("dRich1_0", 0);
  se->registerSubsystem(hits);

  ///////////////////////////////////////////
  // IOManagers...
  ///////////////////////////////////////////

  // Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT","G4Example02.root");
  // out->Verbosity(10);
  // se->registerOutputManager(out);

  // this (dummy) input manager just drives the event loop
  Fun4AllInputManager *in = new Fun4AllDummyInputManager("Dummy");
  se->registerInputManager(in);
  // events = 0 => run forever
  if (nEvents <= 0)
  {
    return 0;
  }
  se->run(nEvents);
  cout << "Print out Detector parameters" << endl;
  drichSS->Print();
  se->End();
  delete se;
  cout << endl
       << endl;
  cout << "Now open the ntuple file with " << endl;
  cout << "  root.exe G4HitNtuple.root" << endl;
  cout << "and draw the 3d hit distribution," << endl;
  cout << "exit hits in black, entry hits in red:" << endl;
  cout << "  hitntup->Draw(\"x1:y1:z1\")" << endl;
  cout << "  hitntup->SetMarkerColor(2)" << endl;
  cout << "  hitntup->Draw(\"x0:y0:z0\",\"\",\"same\")" << endl;
  cout << endl;
  gSystem->Exit(0);
}

#endif
