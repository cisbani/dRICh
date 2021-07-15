void drawHits(TString infileN="out/sim.root") {
  TFile *infile = new TFile(infileN,"READ");
  TTree *tr = (TTree*)infile->Get("events");
  TH2D *hitsHist = new TH2D(
      "hitsHist",
      "dRICh hits;x [cm];y [cm]",
      500, -150, 150,
      500, -150, 150
      );
  tr->Project(
      "hitsHist",
      "DRICHHits.position.y/10:DRICHHits.position.x/10"
      );
  TCanvas *canv = new TCanvas();
  canv->SetLogz();
  canv->SetGrid(1,1);
  hitsHist->Draw("colz");
};
