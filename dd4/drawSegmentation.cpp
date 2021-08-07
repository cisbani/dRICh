// test readout segmentation
#include <cstdlib>
#include <iostream>

// ROOT
#include "TSystem.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "ROOT/RDataFrame.hxx"

using namespace ROOT;
int main(int argc, char** argv) {
  TApplication mainApp("mainApp",&argc,argv);

  RDataFrame dfIn("events","out/sim_run.root");

  auto decodeID = [](VecOps::RVec<Long64_t> id, std::pair<int,int> b) {
    int bitshift = b.first;
    int length = b.second;
    Long64_t div = (Long64_t) pow(2,bitshift);
    Long64_t mask = (Long64_t) pow(2,length) - 1;
    return (id/div) & mask;
  };

  auto dfBits = dfIn
      .Define("detB", "std::pair<int,int> p{0,8}; return p")
      .Define("secB", "std::pair<int,int> p{8,8}; return p")
      .Define("modB", "std::pair<int,int> p{16,16}; return p")
      .Define("xB",   "std::pair<int,int> p{32,16}; return p")
      .Define("yB",   "std::pair<int,int> p{48,16}; return p")
      ;

  auto dfDecoded = dfBits
      .Define("detDR", decodeID, {"DRICHHits.cellID","detB"})
      .Define("secDR", decodeID, {"DRICHHits.cellID","secB"})
      .Define("modDR", decodeID, {"DRICHHits.cellID","modB"})
      .Define("xDR",   decodeID, {"DRICHHits.cellID","xB"})
      .Define("yDR",   decodeID, {"DRICHHits.cellID","yB"})
      ;

  TCanvas * c = new TCanvas();
  c->Divide(3,2);
  c->cd(1); dfDecoded.Histo1D("detDR")->DrawClone();
  c->cd(2); dfDecoded.Histo1D("secDR")->DrawClone();
  c->cd(3); dfDecoded.Histo1D("modDR")->DrawClone();
  c->cd(4); dfDecoded.Histo1D("xDR")->DrawClone();
  c->cd(5); dfDecoded.Histo1D("yDR")->DrawClone();

  std::cout << "\n\npress ^C to exit.\n\n";
  mainApp.Run();
  return 0;
};
