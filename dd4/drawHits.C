void drawHits(TString infileN="out/sim_run.root") {
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
};
