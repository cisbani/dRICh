#include "dRichTree.h"

// TODO: may not need things commented with `---`

// tracking
//#include <g4vertex/GlobalVertex.h> //---
//#include <g4vertex/GlobalVertexMap.h> //---

// fun4all
//#include <fun4all/Fun4AllHistoManager.h> //---
#include <fun4all/Fun4AllReturnCodes.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Hitv1.h>
#include <g4main/PHG4HitContainer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
//#include <g4main/PHG4Particle.h> //---
//#include <g4main/PHG4TruthInfoContainer.h> //---

// drich
#include "dRichHit.h"

// root
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>

// geant
#include <G4SystemOfUnits.hh>

// C++ includes
#include <sstream>
#include <string>

using std::string;
using std::cout;
using std::cerr;
using std::endl;


//-------------------------------------
dRichTree::dRichTree(const std::string &name,
                     const std::string &filename)
  : SubsysReco(name)
  , m_outfileN(filename)
  //, m_hm(nullptr) //---
{
  //resetVars(); //---
  evnum = 0;
  initTrees();
};


//-------------------------------------
dRichTree::~dRichTree() {
  delete m_tree;
  //delete m_hm; //---
};


//-------------------------------------
int dRichTree::Init(PHCompositeNode *topNode) {
  if(Verbosity()>5) cout << "CALL dRichTree::Init" << endl;

  m_outfile = new TFile(m_outfileN.c_str(),"RECREATE");

  return 0;
};


//-------------------------------------
int dRichTree::process_event(PHCompositeNode *topNode) {
  if(Verbosity()>5)
    cout << "CALL dRichTree::process_event" << endl;

  getHits(topNode);

  return Fun4AllReturnCodes::EVENT_OK;
};


//-------------------------------------
int dRichTree::End(PHCompositeNode *topNode) {
  if(Verbosity()>1) cout << "CALL dRichTree::End" << endl;

  m_outfile->cd();
  m_tree->Write();
  m_outfile->Write();
  m_outfile->Close();

  delete m_outfile;
  if(Verbosity()>1) cout << "DONE dRichTree::End" << endl;
  return 0;
};



//----------------------------------------------
void dRichTree::getHits(PHCompositeNode *topNode) {

  // get hits container
  PHG4HitContainer *hitCont =
    findNode::getClass<PHG4HitContainer>(
      topNode, "G4HIT_dRich_0"
    ); // TODO: do not hard code name

  if(!hitCont) {
    cerr << "ERROR: hitCont not found" << endl;
    return;
  };
  
  evnum++;

  // loop over hits, filling tree
  auto hitRange = hitCont->getHits();
  for(auto hitIter = hitRange.first;
      hitIter != hitRange.second;
      hitIter++) {
    dRichHit *hit = dynamic_cast<dRichHit*>(hitIter->second);

    if(Verbosity()>5) {
      cout << "----- HIT PRINTOUT:" << endl;
      hit->print();
    };

    trackID = (Int_t) hit->get_trkid();
    petal = (Int_t) hit->get_petal();
    psst = (Int_t) hit->get_psst();
    vectorToArray( hit->get_position(1), photonHit );
    vectorToArray( hit->get_momentum(), photonP );
    photonDeltaT = (Double_t) hit->get_delta_t();
    m_tree->Fill();
  };


};


//---------------------------------------------
void dRichTree::vectorToArray(
  G4ThreeVector vec, Double_t *arr) {
  arr[0] = (Double_t) vec.x();
  arr[1] = (Double_t) vec.y();
  arr[2] = (Double_t) vec.z();
};

//---------------------------------------------
void dRichTree::initTrees() {
  m_tree = new TTree("tree","tree");
  m_tree->Branch("evnum",&evnum,"evnum/I");
  m_tree->Branch("trackID",&trackID,"trackID/I");
  m_tree->Branch("petal",&petal,"petal/I");
  m_tree->Branch("psst",&psst,"psst/I");
  m_tree->Branch("photonHit",photonHit,"photonHit[3]/D");
  m_tree->Branch("photonP",photonP,"photonP[3]/D");
  m_tree->Branch("photonDeltaT",&photonDeltaT,"photonDeltaT/D");
};


//----------------------------------------
/*
// TODO: may not need this...
void dRichTree::resetVars() {
  evnum = 0;
  trackID = -999;
  for(int c=0; c<3; c++) photonHit[c]=-999;
  photonDeltaT=-999;
};
*/
