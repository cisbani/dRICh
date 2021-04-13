#include <g4main/PHG4Reco.h>
void gui() {
  gROOT->ProcessLine(".x Fun4All_G4_Example01.C(-1)");
  gROOT->LoadMacro("DisplayOn.C");
  printf("PHG4Reco * g4 = QTGui()\n");
};
