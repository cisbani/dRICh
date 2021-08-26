// draw hits, and make some other related plots
// (cf. drawSegmentation.cpp for readout)
#include <cstdlib>
#include <iostream>
#include <functional>

// ROOT
#include "TSystem.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TBox.h"
#include "ROOT/RDataFrame.hxx"

// DD4hep
#include "dd4pod/Geant4ParticleCollection.h"
#include "dd4pod/TrackerHitCollection.h"
#include "dd4pod/PhotoMultiplierHitCollection.h"

using std::cout;
using std::cerr;
using std::endl;

using namespace ROOT;

int main(int argc, char** argv) {

  // setup
  TString infileN="out/sim_run.root";
  if(argc>1) infileN = TString(argv[1]);
  TApplication mainApp("mainApp",&argc,argv);
  EnableImplicitMT();
  RDataFrame dfIn("events",infileN.Data());

  // lambdas
  auto numHits = [](VecOps::RVec<dd4pod::PhotoMultiplierHitData> hits) { return hits.size(); };
  /*
  // TODO: would be nice if this worked...
  auto forAll = []<typename T, typename U>(VecOps::RVec<T> vec, std::function<U(T)> action) {
    VecOps::RVec<T> result;
    for(auto elem : vec) result.emplace_back(action(elem));
    return result;
  };
  auto momentum = [&forAll](VecOps::RVec<dd4pod::Geant4ParticleData> parts) { 
    return forAll(parts,[](dd4pod::Geant4ParticleData p){return p.ps.mag();});
  };
  */
  auto momentum = [](VecOps::RVec<dd4pod::Geant4ParticleData> parts) {
    VecOps::RVec<double> result;
    for(auto p : parts) result.emplace_back(p.ps.mag());
    return result;
  };
  auto hitPos = [](VecOps::RVec<dd4pod::PhotoMultiplierHitData> hits) {
    VecOps::RVec<dd4pod::VectorXYZ> result;
    for(auto h : hits) result.emplace_back(h.position);
    return result;
  };
  auto hitPosX = [](VecOps::RVec<dd4pod::VectorXYZ> positions) {
    VecOps::RVec<double> result;
    for(auto p : positions) result.emplace_back(p.x);
    return result;
  };
  auto hitPosY = [](VecOps::RVec<dd4pod::VectorXYZ> positions) {
    VecOps::RVec<double> result;
    for(auto p : positions) result.emplace_back(p.y);
    return result;
  };

  // transformations
  auto df1 = dfIn
    .Define("thrownP",momentum,{"mcparticles"})
    .Define("numHits",numHits,{"DRICHHits"})
    .Define("hitPos",hitPos,{"DRICHHits"})
    .Define("hitX",hitPosX,{"hitPos"})
    .Define("hitY",hitPosY,{"hitPos"})
    ;
  auto dfFinal = df1;

  // actions
  auto thrownPHist = dfFinal.Histo1D("thrownP");
  auto hitPositionHist = dfFinal.Histo2D(
      { "hitPositions","dRICh hit positions",
      1000,-2000,2000, 1000,-2000,2000 },
      "hitX","hitY"
      );
  auto numHitsVsThrownP = dfFinal.Histo2D(
      { "numHitsVsThrownP","number of dRICh hits vs. thrown P", 
      10,0,10, 100,0,100 },
      "thrownP","numHits"
      );

  // execution
  TCanvas *canv = new TCanvas();
  thrownPHist->Draw();
  canv = new TCanvas();
  hitPositionHist->Draw("colz");
  canv = new TCanvas();
  numHitsVsThrownP->Draw("box");

  /*
  //gStyle->SetOptStat(0);
  TFile *infile = new TFile(infileN,"READ");
  TTree *tr = (TTree*)infile->Get("events");
  TH2D *hitsHist = new TH2D(
      "hitsHist",
      "dRICh hits;x [cm];y [cm]",
      1000, -200, 200,
      1000, -200, 200
      );
  tr->Project(
      "hitsHist",
      "DRICHHits.position.y/10:DRICHHits.position.x/10"
      );
  TCanvas *canv = new TCanvas();
  canv->SetLogz();
  canv->SetGrid(1,1);
  hitsHist->Draw("colz");
  hitsHist->GetXaxis()->SetRangeUser(100,200);
  hitsHist->GetYaxis()->SetRangeUser(-40,40);
  TString outImage = infileN;
  outImage(TRegexp("\\.root$"))=".png";
  canv->Print(outImage);
  */

  cout << "\n\npress ^C to exit.\n\n";
  mainApp.Run();
  return 0;
};
