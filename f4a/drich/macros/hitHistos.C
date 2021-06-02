// draw some histograms for testing dRichTree output

// global pointers
TCanvas *canv;
TFile *infile,*outfile;
TTree *tr;

// subroutine to draw a histogram to a canvas, with
// controllable settings; if you want to specify binning,
// use `>>` syntax in `vars` argument
void drawHisto( TString canvN
              , TString vars
              , TString cuts
              , TString style
              , Int_t logx=0
              , Int_t logy=0
              , Int_t logz=0
) {
  canv = new TCanvas(canvN,canvN);
  canv->SetLogx(logx); canv->SetLogy(logy); canv->SetLogz(logz);
  tr->Draw(vars,cuts,style);
  canv->Write();
};

// main function -----------------------------------
void hitHistos(TString infileN="dRichTree.root") {

  // read tree
  infile = new TFile(infileN,"READ");
  tr = (TTree*) infile->Get("tree");

  // define output file
  outfile = new TFile("dRichHists.root","RECREATE");

  // HISTOGRAMS ////////////////////////////
  drawHisto(
      "hitType_vs_particleName",
      "hitType:particleName",
      "",
      "colz",
      0,0,1
      );
  drawHisto(
      "hitType_vs_process",
      "hitType:process",
      "",
      "colz",
      0,0,1
      );
  // photosensor hits
  drawHisto(
      "psst_hitPos",
      "photHitPos[1]:photHitPos[0]>>psst_hitPos(500,-200,200,500,-200,200)",
      "hitType==\"psst\" && process==\"Cerenkov\"",
      "colz",
      0,0,0
      );
  // time series
  drawHisto(
      "timeSeries_particleName",
      "particleName:evnum",
      "",
      "*"
      );
  drawHisto(
      "timeSeries_process",
      "process:evnum",
      "",
      "*"
      );
  //////////////////////////////////////////

  // cleanup
  infile->Close();
  outfile->Close();
};

