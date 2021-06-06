#include "dRIChTree.h"

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
#include "dRIChHit.h"

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
dRIChTree::dRIChTree(const std::string &name,
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
dRIChTree::~dRIChTree() {
  delete m_tree;
  //delete m_hm; //---
};


//-------------------------------------
int dRIChTree::Init(PHCompositeNode *topNode) {
  if(Verbosity()>5) cout << endl << "CALL dRIChTree::Init" << endl;

  m_outfile = new TFile(m_outfileN.c_str(),"RECREATE");

  return 0;
};


//-------------------------------------
int dRIChTree::process_event(PHCompositeNode *topNode) {
  if(Verbosity()>5)
    cout << endl << "CALL dRIChTree::process_event"
         << " ===================="
         << endl;

  getHits(topNode);

  return Fun4AllReturnCodes::EVENT_OK;
};


//-------------------------------------
int dRIChTree::End(PHCompositeNode *topNode) {
  if(Verbosity()>1) cout << endl << "CALL dRIChTree::End" << endl;

  m_outfile->cd();
  m_tree->Write();
  m_outfile->Write();
  m_outfile->Close();

  delete m_outfile;
  if(Verbosity()>1) cout << "DONE dRIChTree::End" << endl;
  return 0;
};



//----------------------------------------------
void dRIChTree::getHits(PHCompositeNode *topNode) {

  // get hits container
  PHG4HitContainer *hitCont =
    findNode::getClass<PHG4HitContainer>(
      topNode, "G4HIT_dRICh_0"
    ); // TODO: do not hard code name

  if(!hitCont) {
    cerr << "ERROR: hitCont not found" << endl;
    return;
  };
  
  evnum++;
  if(evnum%100==0) cout << ">" << evnum << " events processed" << endl;

  // loop over hits, filling tree
  auto hitRange = hitCont->getHits();
  for(auto hitIter = hitRange.first;
      hitIter != hitRange.second;
      hitIter++) {
    dRIChHit *hit = dynamic_cast<dRIChHit*>(hitIter->second);

    if(Verbosity()>5) {
      cout << "----- HIT PRINTOUT:" << endl;
      hit->print();
    };

    trackID = (Int_t) hit->get_trkid();
    strcpy(hitType,hit->get_hit_type_name());
    strcpy(hitSubtype,hit->get_hit_subtype_name());
    petal = (Int_t) hit->get_petal();
    psst = (Int_t) hit->get_psst();
    pdg = (Int_t) hit->get_pdg();
    strcpy(particleName,hit->get_particle_name().c_str());
    strcpy(process,hit->get_process().c_str());
    parentID = (Int_t) hit->get_parent_id();
    vectorToArray( hit->get_position(1), hitPos );
    vectorToArray( hit->get_momentum(), hitP );
    vectorToArray( hit->get_momentum_dir(), hitPdir );
    vectorToArray( hit->get_vertex_position(), hitVtxPos );
    vectorToArray( hit->get_vertex_momentum_dir(), hitVtxPdir );
    deltaT = (Double_t) hit->get_delta_t();
    m_tree->Fill();
  };


};


//---------------------------------------------
void dRIChTree::vectorToArray(
  G4ThreeVector vec, Double_t *arr) {
  arr[0] = (Double_t) vec.x();
  arr[1] = (Double_t) vec.y();
  arr[2] = (Double_t) vec.z();
};

//---------------------------------------------
void dRIChTree::initTrees() {
  m_tree = new TTree("tree","tree");
  m_tree->Branch("evnum",&evnum,"evnum/I");
  m_tree->Branch("trackID",&trackID,"trackID/I");
  m_tree->Branch("hitType",hitType,"hitType/C");
  m_tree->Branch("hitSubtype",hitSubtype,"hitSubtype/C");
  m_tree->Branch("petal",&petal,"petal/I");
  m_tree->Branch("psst",&psst,"psst/I");
  m_tree->Branch("pdg",&pdg,"pdg/I");
  m_tree->Branch("particleName",particleName,"particleName/C");
  m_tree->Branch("process",process,"process/C");
  m_tree->Branch("parentID",&parentID,"parentID/I");
  m_tree->Branch("hitPos",hitPos,"hitPos[3]/D");
  m_tree->Branch("hitP",hitP,"hitP[3]/D");
  m_tree->Branch("hitPdir",hitPdir,"hitPdir[3]/D");
  m_tree->Branch("hitVtxPos",hitVtxPos,"hitVtxPos[3]/D");
  m_tree->Branch("hitVtxPdir",hitVtxPdir,"hitVtxPdir[3]/D");
  m_tree->Branch("deltaT",&deltaT,"deltaT/D");
};


//----------------------------------------
/*
// TODO: may not need this...
void dRIChTree::resetVars() {
  evnum = 0;
  trackID = -999;
  for(int c=0; c<3; c++) hitPos[c]=-999;
  deltaT=-999;
};
*/
