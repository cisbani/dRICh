// draw some histograms for testing dRIChTree output

// global pointers
TCanvas *canv;
TFile *infile,*outfile;
TTree *tr;

// geometry constants (for guidelines)
const Double_t vesselRadius = 210.5; // cm
const Double_t pipeRadius = 9.5; // cm
const Double_t vesselFront = -161.0 / 2; // cm
const Double_t vesselBack = 161.0 / 2; // cm

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

  // draw histogram
  canv = new TCanvas(canvN,canvN);
  canv->SetLogx(logx); canv->SetLogy(logy); canv->SetLogz(logz);
  canv->SetGrid(1,1);
  tr->Draw(vars,cuts,style);

  // draw vessel guidelines (for hit positions, etc.)
  Double_t xv = vesselRadius * TMath::Sin(TMath::Pi()/3);
  Double_t yv = vesselRadius * TMath::Cos(TMath::Pi()/3);
  Double_t xp = pipeRadius * TMath::Sin(TMath::Pi()/3);
  Double_t yp = pipeRadius * TMath::Cos(TMath::Pi()/3);
  if(canvN.Contains("Pos2D")) {
    TEllipse * circ[2];
    circ[0] = new TEllipse(0,0,vesselRadius,vesselRadius);
    circ[1] = new TEllipse(0,0,pipeRadius,pipeRadius);
    for(int c=0; c<2; c++) {
      circ[c]->SetFillStyle(0);
      circ[c]->SetLineColor(kBlack);
      circ[c]->SetLineWidth(3);
      circ[c]->Draw("SAME");
    };
    TLine * line[6];
    line[0] = new TLine(0, vesselRadius,0, pipeRadius);
    line[1] = new TLine(0,-vesselRadius,0,-pipeRadius);
    line[2] = new TLine(-xv, yv,-xp, yp);
    line[3] = new TLine( xv,-yv, xp,-yp);
    line[4] = new TLine(-xv,-yv,-xp,-yp);
    line[5] = new TLine( xv, yv, xp, yp);
    for(int l=0; l<6; l++) {
      line[l]->SetLineColor(kBlack);
      line[l]->SetLineWidth(3);
      line[l]->Draw("SAME");
    };
  }
  else if(canvN.Contains("Pos3D")) {
    TPolyLine3D * vcirc[2];
    int ndiv=100;
    Double_t xx,yy;
    for(int c=0; c<2; c++) vcirc[c] = new TPolyLine3D(ndiv+1);
    for(int k=0; k<=ndiv; k++) {
      Double_t xc = vesselRadius * TMath::Cos(2*k*TMath::Pi()/ndiv);
      Double_t yc = vesselRadius * TMath::Sin(2*k*TMath::Pi()/ndiv);
      vcirc[0]->SetPoint(k,xc,vesselFront,yc);
      vcirc[1]->SetPoint(k,xc,vesselBack,yc);
    };
    for(int c=0; c<2; c++) {
      vcirc[c]->SetLineWidth(3);
      vcirc[c]->SetLineColor(kBlack);
      vcirc[c]->Draw("SAME");
    };
    TPolyLine3D * vline[6];
    for(int l=0; l<6; l++) vline[l] = new TPolyLine3D(2);
    vline[0]->SetPoint(0,0,vesselFront,vesselRadius);
    vline[0]->SetPoint(1,0,vesselBack ,vesselRadius);
    vline[1]->SetPoint(0,0,vesselFront,-vesselRadius);
    vline[1]->SetPoint(1,0,vesselBack ,-vesselRadius);
    vline[2]->SetPoint(0,-xv,vesselFront,yv);
    vline[2]->SetPoint(1,-xv,vesselBack,yv);
    vline[3]->SetPoint(0,xv,vesselFront,-yv);
    vline[3]->SetPoint(1,xv,vesselBack,-yv);
    vline[4]->SetPoint(0,-xv,vesselFront,-yv);
    vline[4]->SetPoint(1,-xv,vesselBack,-yv);
    vline[5]->SetPoint(0,xv,vesselFront,yv);
    vline[5]->SetPoint(1,xv,vesselBack,yv);
    for(int l=0; l<6; l++) {
      vline[l]->SetLineWidth(3);
      vline[l]->SetLineColor(kBlack);
      vline[l]->Draw("SAME");
    };
  };
  canv->Write();
};

// other subroutine defs
void BinLog(TAxis * axis);

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
  TString binning2D = Form("(500,%f,%f,500,%f,%f)",
      -vesselRadius,vesselRadius,-vesselRadius,vesselRadius);
  TString binning3D = Form("(50,%f,%f,50,%f,%f,50,%f,%f)",
      -vesselRadius,vesselRadius,vesselFront,vesselBack,-vesselRadius,vesselRadius);
  drawHisto(
      "optical_hitPos2D",
      "hitPos[1]:hitPos[0]>>optical_hitPos2D"+binning2D,
      "hitType==\"psst\" && hitSubtype==\"optical\"",
      "colz",
      0,0,1
      );
  drawHisto(
      "optical_hitPos3D",
      "hitPos[0]:hitPos[2]:hitPos[1]>>optical_hitPos3D"+binning3D,
      "hitType==\"psst\" && hitSubtype==\"optical\"",
      "box",
      0,0,0
      );
  // - hit postions for each hitType, for validation of hitType classification
  for(int h=0; h<nHitTypes; h++) { // loop over hitTypes
    drawHisto(
        hitTypeList[h]+"_hitPos2D",
        "hitPos[1]:hitPos[0]>>"+hitTypeList[h]+"_hitPos2D"+binning2D,
        "hitType==\""+hitTypeList[h]+"\"",
        "colz",
        0,0,1
        );
    drawHisto(
        hitTypeList[h]+"_hitPos3D",
        "hitPos[0]:hitPos[2]:hitPos[1]>>"+hitTypeList[h]+"_hitPos3D"+binning3D,
        "hitType==\""+hitTypeList[h]+"\"",
        "box",
        0,0,0
        );
  };
  // - vertex postions for each hitType, for validation of hitType classification
  for(int h=0; h<nHitTypes; h++) { // loop over hitTypes
    drawHisto(
        hitTypeList[h]+"_hitVtxPos2D",
        "hitVtxPos[1]:hitVtxPos[0]>>"+hitTypeList[h]+"_hitVtxPos2D"+binning2D,
        "hitType==\""+hitTypeList[h]+"\"",
        "colz",
        0,0,1
        );
    drawHisto(
        hitTypeList[h]+"_hitVtxPos3D",
        "hitVtxPos[0]:hitVtxPos[2]:hitVtxPos[1]>>"+hitTypeList[h]+"_hitVtxPos3D"+binning3D,
        "hitType==\""+hitTypeList[h]+"\"",
        "box",
        0,0,0
        );
  };

  // - hit momentum
  drawHisto(
      "optical_hitMomentum2D",
      "hitP[1]:hitP[0]",
      "hitType==\"psst\" && hitSubtype==\"optical\"",
      "colz",
      0,0,1
      );
  drawHisto(
      "optical_hitMomentum3D",
      "hitP[0]:hitP[2]:hitP[1]",
      "hitType==\"psst\" && hitSubtype==\"optical\"",
      "box",
      0,0,0
      );
  for(int h=0; h<nHitTypes; h++) { // loop over hitTypes
    drawHisto(
        hitTypeList[h]+"_hitMomentum2D",
        "hitP[1]:hitP[0]",
        "hitType==\""+hitTypeList[h]+"\"",
        "colz",
        0,0,1
        );
    drawHisto(
        hitTypeList[h]+"_hitMomentum3D",
        "hitP[0]:hitP[2]:hitP[1]",
        "hitType==\""+hitTypeList[h]+"\"",
        "box",
        0,0,0
        );
  };

  // - hit momentum direction
  drawHisto(
      "optical_hitMomentumDir2D",
      "hitPdir[1]:hitPdir[0]",
      "hitType==\"psst\" && hitSubtype==\"optical\"",
      "colz",
      0,0,1
      );
  drawHisto(
      "optical_hitMomentumDir3D",
      "hitPdir[0]:hitPdir[2]:hitPdir[1]",
      "hitType==\"psst\" && hitSubtype==\"optical\"",
      "box",
      0,0,0
      );
  for(int h=0; h<nHitTypes; h++) { // loop over hitTypes
    drawHisto(
        hitTypeList[h]+"_hitMomentumDir2D",
        "hitPdir[1]:hitPdir[0]",
        "hitType==\""+hitTypeList[h]+"\"",
        "colz",
        0,0,1
        );
    drawHisto(
        hitTypeList[h]+"_hitMomentumDir3D",
        "hitPdir[0]:hitPdir[2]:hitPdir[1]",
        "hitType==\""+hitTypeList[h]+"\"",
        "box",
        0,0,0
        );
  };

  // delta T
  for(int h=0; h<nHitTypes; h++) { // loop over hitTypes
    drawHisto(
        hitTypeList[h]+"_deltaT",
        "deltaT>>"+hitTypeList[h]+"_deltaT"+"(5000,0,100)",
        "hitType==\""+hitTypeList[h]+"\"",
        "",
        0,1,0
        );
  };

  // energy deposition
  // - edep spectrum for primary exits
  drawHisto(
      "edepSpectrum_primary_exit",
      "edep",
      "hitType==\"exit\" && hitSubtype==\"primary\"",
      "",
      0,1,0
      );

  // - edep spectrum for optical photons (NB: units converted to eV)
  TH1D * edepSpectrum_optical_psst = new TH1D(
    "edepSpectrum_optical_psst_hist","Edep for optical PSST hits (units=eV)",
    1000,1,1e4);
  BinLog(edepSpectrum_optical_psst->GetXaxis());
  tr->Project(
      "edepSpectrum_optical_psst_hist",
      "edep*1e9", /* convert GeV -> eV */
      "hitType==\"psst\" && hitSubtype==\"optical\""
      );
  canv = new TCanvas("edepSpectrum_optical_psst","edepSpectrum_optical_psst");
  canv->SetLogx();
  canv->SetGrid(1,1);
  edepSpectrum_optical_psst->Draw();
  canv->Write();

  // edep vs. subtype,particle,process,momentum for each hitType
  for(int h=0; h<nHitTypes; h++) { // loop over hitTypes
    drawHisto(
        hitTypeList[h]+"_edep_vs_hitSubtype",
        "edep:hitSubtype",
        "hitType==\""+hitTypeList[h]+"\"",
        "colz",
        0,0,1
        );
    drawHisto(
        hitTypeList[h]+"_edep_vs_particleName",
        "edep:particleName",
        "hitType==\""+hitTypeList[h]+"\"",
        "colz",
        0,0,1
        );
    drawHisto(
        hitTypeList[h]+"_edep_vs_process",
        "edep:process",
        "hitType==\""+hitTypeList[h]+"\"",
        "colz",
        0,0,1
        );
  };

  // - edep vs momentum for primary exits
  drawHisto(
      "edepVsP_primary_exit",
      "edep:TMath::Sqrt(hitP[0]*hitP[0]+hitP[1]*hitP[1]+hitP[2]*hitP[2])",
      "hitType==\"exit\" && hitSubtype==\"primary\"",
      "colz",
      0,0,1
      );

  // - edep vs momentum for optical photons (NB: units converted to eV)
  TH2D * edepVsP_optical_psst = new TH2D(
    "edepVsP_optical_psst_hist","Edep vs. P for optical PSST hits (units=eV)",
    100,1,1e2,
    100,1,1e4);
  BinLog(edepVsP_optical_psst->GetXaxis());
  BinLog(edepVsP_optical_psst->GetYaxis());
  tr->Project(
      "edepVsP_optical_psst_hist",
      "edep*1e9:TMath::Sqrt(hitP[0]*hitP[0]+hitP[1]*hitP[1]+hitP[2]*hitP[2])*1e9", /* convert GeV -> eV */
      "hitType==\"psst\" && hitSubtype==\"optical\""
      );
  canv = new TCanvas("edepVsP_optical_psst","edepVsP_optical_psst");
  canv->SetLogx();
  canv->SetLogy();
  canv->SetGrid(1,1);
  edepVsP_optical_psst->Draw("colz");
  canv->Write();


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



// create equal width binning for a logarithmic scale axis
void BinLog(TAxis * axis) {
  Float_t lb = axis->GetXmin();
  Float_t ub = axis->GetXmax();
  if(lb<=0||ub<=0||lb>=ub) {
    fprintf(stderr,"ERROR: bad axis range for Tools::BinLog\n");
    return;
  };
  lb = TMath::Log10(lb);
  ub = TMath::Log10(ub);

  Int_t nbins = axis->GetNbins();
  Float_t width = (ub-lb)/nbins;
  Float_t * newBins = new Float_t[nbins+1];
  for (int b=0; b<=nbins; b++) newBins[b] = TMath::Power(10,lb+b*width);
  axis->Set(nbins,newBins);
  delete[] newBins;
} 
