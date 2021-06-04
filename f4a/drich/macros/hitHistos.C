// draw some histograms for testing dRIChTree output

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
  canv->SetGrid(1,1);
  tr->Draw(vars,cuts,style);
  canv->Write();
};

// main function -----------------------------------
void hitHistos(TString infileN="dRIChTree.root") {

  // read tree
  infile = new TFile(infileN,"READ");
  tr = (TTree*) infile->Get("tree");

  // define output file
  outfile = new TFile("dRIChHists.root","RECREATE");

  // HISTOGRAMS ////////////////////////////
  // process vs particle
  drawHisto(
      "process_vs_particleName",
      "process:particleName",
      "",
      "colz",
      0,0,1
      );
  // general hits
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
  drawHisto(
      "hitType_vs_hitSubtype", /*sanity check*/
      "hitType:hitSubtype",
      "",
      "colz",
      0,0,1
      );
  // entrance hit subtypes
  drawHisto(
      "entranceSubtype_vs_particleName",
      "hitSubtype:particleName",
      "hitType==\"entrance\"",
      "colz",
      0,0,1
      );
  drawHisto(
      "entranceSubtype_vs_process",
      "hitSubtype:process",
      "hitType==\"entrance\"",
      "colz",
      0,0,1
      );
  // photosensor hit subtypes
  drawHisto(
      "psstSubtype_vs_particleName",
      "hitSubtype:particleName",
      "hitType==\"psst\"",
      "colz",
      0,0,1
      );
  drawHisto(
      "psstSubtype_vs_process",
      "hitSubtype:process",
      "hitType==\"psst\"",
      "colz",
      0,0,1
      );
  // photosensor hits for DAQ
  drawHisto(
      "psst_hitPos",
      "photHitPos[1]:photHitPos[0]>>psst_hitPos(500,-200,200,500,-200,200)",
      "hitType==\"psst\" && process==\"Cerenkov\"",
      "colz",
      0,0,1
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

