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


  // useful variables
  const Int_t nHitTypes = 3;
  TString hitTypeList[nHitTypes] = {"entrance","psst","exit"};


  // HISTOGRAMS ////////////////////////////

  // process vs particle
  drawHisto(
      "process_vs_particleName",
      "process:particleName",
      "",
      "colztext",
      0,0,1
      );

  // hit types
  drawHisto(
      "hitType_vs_particleName",
      "hitType:particleName",
      "",
      "colztext",
      0,0,1
      );
  drawHisto(
      "hitType_vs_process",
      "hitType:process",
      "",
      "colztext",
      0,0,1
      );
  drawHisto(
      "hitType_vs_hitSubtype",
      "hitType:hitSubtype",
      "",
      "colztext",
      0,0,1
      );

  // hit subtypes
  for(int h=0; h<nHitTypes; h++) { // loop over hitTypes
    drawHisto(
        hitTypeList[h]+"Subtype_vs_particleName",
        "hitSubtype:particleName",
        "hitType==\""+hitTypeList[h]+"\"",
        "colztext",
        0,0,1
        );
    drawHisto(
        hitTypeList[h]+"Subtype_vs_process",
        "hitSubtype:process",
        "hitType==\""+hitTypeList[h]+"\"",
        "colztext",
        0,0,1
        );
  };

  // hit positions
  // - cherenkov hits, what we will see in the DAQ (but with binning finer than
  //   the photosensors)
  drawHisto(
      "optical_hitPos2D",
      "hitPos[1]:hitPos[0]>>optical_hitPos2D(500,-200,200,500,-200,200)",
      "hitType==\"psst\" && hitSubtype==\"optical\"",
      "colz",
      0,0,1
      );
  drawHisto(
      "optical_hitPos3D",
      "hitPos[0]:hitPos[2]:hitPos[1]",
      "hitType==\"psst\" && hitSubtype==\"optical\"",
      "",
      0,0,0
      );
  // - hit postions for each hitType, for validation of hitType classification
  for(int h=0; h<nHitTypes; h++) { // loop over hitTypes
    drawHisto(
        hitTypeList[h]+"_hitPos2D",
        "hitPos[1]:hitPos[0]>>"+hitTypeList[h]+"_hitPos2D(500,-200,200,500,-200,200)",
        "hitType==\""+hitTypeList[h]+"\"",
        "colz",
        0,0,1
        );
    drawHisto(
        hitTypeList[h]+"_hitPos3D",
        "hitPos[0]:hitPos[2]:hitPos[1]",
        "hitType==\""+hitTypeList[h]+"\"",
        "",
        0,0,0
        );
  };
  // - vertex postions for each hitType, for validation of hitType classification
  for(int h=0; h<nHitTypes; h++) { // loop over hitTypes
    drawHisto(
        hitTypeList[h]+"_hitVtxPos2D",
        "hitVtxPos[1]:hitVtxPos[0]>>"+hitTypeList[h]+"_hitVtxPos2D(500,-200,200,500,-200,200)",
        "hitType==\""+hitTypeList[h]+"\"",
        "colz",
        0,0,1
        );
    drawHisto(
        hitTypeList[h]+"_hitVtxPos3D",
        "hitVtxPos[0]:hitVtxPos[2]:hitVtxPos[1]",
        "hitType==\""+hitTypeList[h]+"\"",
        "",
        0,0,0
        );
  };

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

