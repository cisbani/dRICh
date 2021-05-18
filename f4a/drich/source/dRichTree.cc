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
    hitType = (Int_t) hit->get_hit_type();
    petal = (Int_t) hit->get_petal();
    psst = (Int_t) hit->get_psst();
    pdg = (Int_t) hit->get_pdg();
    strcpy(particleName,hit->get_particle_name().c_str());
    strcpy(process,hit->get_process().c_str());
    parentID = (Int_t) hit->get_parent_id();
    vectorToArray( hit->get_position(1), photHitPos );
    vectorToArray( hit->get_momentum(), photP );
    vectorToArray( hit->get_momentum_dir(), photPdir );
    vectorToArray( hit->get_vertex_position(), photVtxPos );
    vectorToArray( hit->get_vertex_momentum_dir(), photVtxPdir );
    photDeltaT = (Double_t) hit->get_delta_t();
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
  m_tree->Branch("hitType",&hitType,"hitType/I");
  m_tree->Branch("petal",&petal,"petal/I");
  m_tree->Branch("psst",&psst,"psst/I");
  m_tree->Branch("pdg",&pdg,"pdg/I");
  m_tree->Branch("particleName",particleName,"particleName/C");
  m_tree->Branch("process",process,"process/C");
  m_tree->Branch("parentID",&parentID,"parentID/I");
  m_tree->Branch("photHitPos",photHitPos,"photHitPos[3]/D");
  m_tree->Branch("photP",photP,"photP[3]/D");
  m_tree->Branch("photPdir",photPdir,"photPdir[3]/D");
  m_tree->Branch("photVtxPos",photVtxPos,"photVtxPos[3]/D");
  m_tree->Branch("photVtxPdir",photVtxPdir,"photVtxPdir[3]/D");
  m_tree->Branch("photDeltaT",&photDeltaT,"photDeltaT/D");
};


//----------------------------------------
/*
// TODO: may not need this...
void dRichTree::resetVars() {
  evnum = 0;
  trackID = -999;
  for(int c=0; c<3; c++) photHitPos[c]=-999;
  photDeltaT=-999;
};
*/
