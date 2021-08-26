// draw hits, and make some other related plots
// (cf. drawSegmentation.cpp for readout)
#include <cstdlib>
#include <iostream>

// ROOT
#include "TSystem.h"
#include "TRegexp.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TBox.h"
#include "ROOT/RDataFrame.hxx"

// NPdet
#include "dd4pod/Geant4ParticleCollection.h"
#include "dd4pod/TrackerHitCollection.h"
#include "dd4pod/PhotoMultiplierHitCollection.h"

using std::cout;
using std::cerr;
using std::endl;

using namespace ROOT;
using namespace ROOT::VecOps;
using namespace dd4pod;

int main(int argc, char** argv) {

  // setup
  TString infileN="out/sim_run.root";
  if(argc>1) infileN = TString(argv[1]);
  TApplication mainApp("mainApp",&argc,argv);
  EnableImplicitMT();
  RDataFrame dfIn("events",infileN.Data());
  TString outfileN = infileN;
  outfileN(TRegexp("\\.root$"))=".";


  /* lambdas
   * - most of these transform an `RVec<T1>` to an `RVec<T2>` using `VecOps::Map` or `VecOps::Filter`
   * - see NPdet/src/dd4pod/dd4hep.yaml for POD syntax
   */
  // calculate number of hits
  auto numHits = [](RVec<PhotoMultiplierHitData> hits) { return hits.size(); };
  // calculate momentum magnitude for each particle (units=GeV)
  auto momentum = [](RVec<Geant4ParticleData> parts){ return Map(parts,[](auto p){ return p.ps.mag(); }); };
  // filter for thrown particles
  auto isThrown = [](RVec<Geant4ParticleData> parts){ return Filter(parts,[](auto p){ return p.ID==0; }); };
  // get positions for each hit (units=cm)
  auto hitPos = [](RVec<PhotoMultiplierHitData> hits){ return Map(hits,[](auto h){ return h.position; }); };
  auto hitPosX = [](RVec<VectorXYZ> v){ return Map(v,[](auto p){ return p.x/10; }); };
  auto hitPosY = [](RVec<VectorXYZ> v){ return Map(v,[](auto p){ return p.y/10; }); };
  auto hitPosZ = [](RVec<VectorXYZ> v){ return Map(v,[](auto p){ return p.z/10; }); };


  // transformations
  auto df1 = dfIn
    .Define("thrownParticles",isThrown,{"mcparticles"})
    .Define("thrownP",momentum,{"thrownParticles"})
    .Define("numHits",numHits,{"DRICHHits"})
    .Define("hitPos",hitPos,{"DRICHHits"})
    .Define("hitX",hitPosX,{"hitPos"})
    .Define("hitY",hitPosY,{"hitPos"})
    ;
  auto dfFinal = df1;


  // actions
  auto hitPositionHist = dfFinal.Histo2D(
      { "hitPositions","dRICh hit positions (units=mm)",
      1000,-200,200, 1000,-200,200 },
      "hitX","hitY"
      );
  auto numHitsVsThrownP = dfFinal.Histo2D(
      { "numHitsVsThrownP","number of dRICh hits vs. thrown momentum", 
      35,0,35, 100,0,100 },
      "thrownP","numHits"
      );


  // execution
  TCanvas *canv;
  canv = new TCanvas();
  hitPositionHist->Draw("colz");
  hitPositionHist->GetXaxis()->SetRangeUser(100,200);
  hitPositionHist->GetYaxis()->SetRangeUser(-40,40);
  canv->SetLogz();
  canv->Print(outfileN+"hits.png");

  canv = new TCanvas();
  numHitsVsThrownP->Draw("box");
  canv->Print(outfileN+"nums.png");

  cout << "\n\npress ^C to exit.\n\n";
  mainApp.Run();
  return 0;
};
