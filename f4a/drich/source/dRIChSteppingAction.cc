#include "dRIChSteppingAction.h"
#include "dRIChDetector.h"
#include "dRIChHit.h"

#include <phparameter/PHParameters.h>

#include <g4detectors/PHG4StepStatusDecode.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Hitv1.h>
#include <g4main/PHG4Shower.h>
#include <g4main/PHG4SteppingAction.h>
#include <g4main/PHG4TrackUserInfoV1.h>

#include <phool/getClass.h>

#include <TSystem.h>

#include <G4ParticleDefinition.hh> 
#include <G4ReferenceCountedHandle.hh>
#include <G4Step.hh>
#include <G4StepPoint.hh> 
#include <G4StepStatus.hh>
#include <G4String.hh> 
#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>
#include <G4TouchableHandle.hh>
#include <G4Track.hh>
#include <G4VProcess.hh>
#include <G4TrackStatus.hh>
#include <G4Types.hh>
#include <G4VPhysicalVolume.hh>
#include <G4VTouchable.hh>
#include <G4VUserTrackInformation.hh>

#include <cmath>
#include <iostream>
#include <string>

class PHCompositeNode;

using std::string;
using std::cout;
using std::cerr;
using std::endl;

//____________________________________________________________________________..
dRIChSteppingAction::dRIChSteppingAction(dRIChDetector *detector, const PHParameters *parameters)
  : PHG4SteppingAction(detector->GetName())
  , m_Detector(detector)
  , m_Params(parameters)
  , m_HitContainer(nullptr)
  , m_Hit(nullptr)
  , m_SaveHitContainer(nullptr)
  , m_SaveVolPre(nullptr)
  , m_SaveVolPost(nullptr)
  , m_SaveTrackId(-1)
  , m_SavePreStepStatus(-1)
  , m_SavePostStepStatus(-1)
  , m_ActiveFlag(m_Params->get_int_param("active"))
  , m_EdepSum(0)
  , m_EionSum(0)
  , verbose(m_Params->get_int_param("verbosity")>0)
{
}

//____________________________________________________________________________..
dRIChSteppingAction::~dRIChSteppingAction()
{
  // if the last hit was a zero energy deposit hit, it is
  // just reset and the memory is still allocated, so we
  // need to delete it here if the last hit was saved, hit
  // is a nullptr pointer which are legal to delete (it
  // results in a no operation)
  delete m_Hit;
}

//____________________________________________________________________________..
// This is the implementation of the G4 UserSteppingAction
bool dRIChSteppingAction::UserSteppingAction(const G4Step *aStep, bool was_used)
{

  if(verbose)
     cout << "[>>>>>] call dRIChSteppingAction::UserSteppingAction" << endl;

  // get touchables, points, volumes
  G4TouchableHandle preTouch = aStep->GetPreStepPoint()->GetTouchableHandle();
  G4TouchableHandle postTouch = aStep->GetPostStepPoint()->GetTouchableHandle();
  G4VPhysicalVolume *preVol = preTouch->GetVolume();
  G4VPhysicalVolume *postVol = postTouch->GetVolume();
  G4StepPoint *prePoint = aStep->GetPreStepPoint();
  G4StepPoint *postPoint = aStep->GetPostStepPoint();

  // skip this step, if leaving the world
  if(postPoint->GetStepStatus() == fWorldBoundary) {
    if(verbose) cout << "... skip this step (leaving world)" << endl;
    if(m_Hit) m_Hit->Reset();
    return false;
  };

  // get track
  const G4Track *aTrack = aStep->GetTrack();
  if(verbose)
    cout << "[-] track ID=" << aTrack->GetTrackID()
         << ", particle=" << aTrack->GetParticleDefinition()->GetParticleName()
         << endl;

  // IsInDetector(preVol) returns
  //  == 0 outside of detector
  //  > 0 for hits in active volume
  //  < 0 for hits in passive material
  int whichactive = m_Detector->IsInDetector(preVol);
  if(verbose)
    cout << "[_] step preVol=" << preVol->GetName()
         << ", postVol=" << postVol->GetName()
         << ", whichactive=" << whichactive
         << endl;

  // check if this step is an entrance or an exit
  bool isEntrance = preVol->GetName().contains("World")
                 && postVol->GetName().contains("DRICHvessel");
  bool isExit =  preVol->GetName().contains("DRICHvessel")
                 && postVol->GetName().contains("World"); // TODO
  if(verbose && isEntrance)
    cout << "[__] step is ENTERING vessel" << endl;

  // classifiers for entrance hits
  enum entranceClass {
    entPrimary,   /* primary, thrown from generator */
    entSecondary, /* secondary, byproduct of thrown particle */
    entPostStep   /* incident particle from PostStepDoItVector */
  };
  int entClass = entPrimary;

  // skip this step, if it's outside the detector, and not an entrance
  // or exit of the vessel
  if(!whichactive && !isEntrance) {
    if(verbose) cout << "... skip this step" << endl;
    return false;
  };

  // get step energy // TODO: do we need `eion`?
  G4double edep = 0;
  G4double eion = 0;
  if(!isEntrance) {
    edep = aStep->GetTotalEnergyDeposit() / GeV;
    eion = (aStep->GetTotalEnergyDeposit() - aStep->GetNonIonizingEnergyDeposit()) / GeV;
  };
  if(verbose)
    cout << "[_] step edep=" << edep << ", eion=" << eion << endl;


  // Here we have to decide if we need to create a new hit.  Normally this should 
  // only be neccessary if a G4 Track enters a new volume or is freshly created
  // For this we look at the step status of the prePoint (beginning of the G4 Step).
  // This should be either fGeomBoundary (G4 Track crosses into volume) or 
  // fUndefined (G4 Track newly created)
  // Sadly over the years with different G4 versions we have observed cases where
  // G4 produces "impossible hits" which we try to catch here
  // These errors were always rare and it is not clear if they still exist but we
  // still check for them for safety. We can reproduce G4 runs identically (if given
  // the sequence of random number seeds you find in the log), the printouts help
  // us giving the G4 support information about those failures
  switch(prePoint->GetStepStatus()) {

    // --- abnormal cases
    case fPostStepDoItProc: // step defined by PostStepDoItVector
      if(m_SavePostStepStatus != fGeomBoundary) {
        // this is the okay case, fPostStepDoItProc called in a volume, not first thing inside
        // a new volume, just proceed here
        if(verbose) cout << "[__] first step in a new volume" << endl;
      } else {
        if(verbose) cout << "[ + ] step was defined by PostStepDoItVector" << endl;
        if(!isEntrance) {
          // this is an impossible G4 Step print out diagnostic to help debug, not sure if
          // this is still with us
          cerr << "ERROR: impossible G4 Step" << endl;
          cout << GetName() << ": New Hit for  " << endl;
          cout << "prestep status: "
            << PHG4StepStatusDecode::GetStepStatus(prePoint->GetStepStatus())
            << ", poststep status: "
            << PHG4StepStatusDecode::GetStepStatus(postPoint->GetStepStatus())
            << ", last pre step status: "
            << PHG4StepStatusDecode::GetStepStatus(m_SavePreStepStatus)
            << ", last post step status: "
            << PHG4StepStatusDecode::GetStepStatus(m_SavePostStepStatus) << endl;
          cout << "last track: " << m_SaveTrackId
            << ", current trackid: " << aTrack->GetTrackID() << endl;
          cout << "phys pre vol: " << preVol->GetName()
            << " post vol : " << postTouch->GetVolume()->GetName() << endl;
          cout << " previous phys pre vol: " << m_SaveVolPre->GetName()
            << " previous phys post vol: " << m_SaveVolPost->GetName() << endl;
        } else {
          entClass = entPostStep;
        }
      }

      // if this step is incident on the vessel, and we have not yet created a
      // hit, create one
      if(isEntrance) {
        m_Hit = nullptr; // kill any leftover hit
        if(verbose) cout << "[++++] NEW hit (entrance)" << endl;
        m_Hit = new dRIChHit();
        // set some entrance attributes, and track ID
        m_Hit->set_position(0, prePoint->GetPosition() / cm);
        m_Hit->set_t(0, prePoint->GetGlobalTime() / nanosecond);
        m_Hit->set_trkid(aTrack->GetTrackID());
        m_SaveTrackId = aTrack->GetTrackID();
        m_SaveHitContainer = m_HitContainer;
      }
      break;

    // --- normal cases
    case fGeomBoundary:
    case fUndefined:
    default:

      // do nothing if not geometry boundary, not undefined, and not entrance
      if(  ! prePoint->GetStepStatus()==fGeomBoundary
        && ! prePoint->GetStepStatus()==fUndefined
        && ! isEntrance
      ) {
        if(verbose) cout << "[+] prepoint status ignored" << endl;
        break;
      };

      // create new track
      if(!m_Hit) {
        if(verbose) cout << "[++++] NEW hit" << endl;
        m_Hit = new dRIChHit();
      }

      // print info
      if(verbose) {
        cout << "[+] prepoint status=";
        if(prePoint->GetStepStatus()==fGeomBoundary) cout << "fGeomBoundary";
        else if(prePoint->GetStepStatus()==fUndefined) cout << "fUndefined";
        else cout << "UNKNOWN!";
        cout << endl;
        if(aTrack->GetTrackID()>1) {
          cout << "[-] secondary track, creator process="
               << aTrack->GetCreatorProcess()->GetProcessName();
        } else {
          cout << "[-] primary track, particle="
               << aTrack->GetParticleDefinition()->GetParticleName();
        };
        cout << endl;
      };
      
      // check if entrance particle is secondary
      if(aTrack->GetTrackID()>1 && isEntrance) entClass = entSecondary;

      // set some entrance attributes, and track ID
      m_Hit->set_position(0, prePoint->GetPosition() / cm);
      m_Hit->set_t(0, prePoint->GetGlobalTime() / nanosecond);
      m_Hit->set_trkid(aTrack->GetTrackID());
      m_SaveTrackId = aTrack->GetTrackID();
      m_SaveHitContainer = m_HitContainer;

      // initializate accumulators (e.g., for total energy deposition)
      m_EdepSum = 0;
      if(whichactive > 0) {
        m_EionSum = 0;
        m_Hit->set_eion(0);
      }

      // tracking of the truth info // TODO: not used yet?
      if(G4VUserTrackInformation *p = aTrack->GetUserInformation()) {
        if(PHG4TrackUserInfoV1 *pp = dynamic_cast<PHG4TrackUserInfoV1 *>(p)) {
          m_Hit->set_trkid(pp->GetUserTrackId());
          pp->GetShower()->add_g4hit_id(m_SaveHitContainer->GetID(), m_Hit->get_hit_id());
        }
      }

      break;
  }


  // This section is called for every step -------------------------------------
  // - some sanity checks for inconsistencies (aka bugs) we have seen over the years
  // - check if this hit was created, if not print out last post step status
  if(!m_Hit || !std::isfinite(m_Hit->get_x(0))) {
    cout << GetName() << ": hit was not created" << endl;
    cout << "prestep status: "
      << PHG4StepStatusDecode::GetStepStatus(prePoint->GetStepStatus())
      << ", poststep status: "
      << PHG4StepStatusDecode::GetStepStatus(postPoint->GetStepStatus())
      << ", last pre step status: "
      << PHG4StepStatusDecode::GetStepStatus(m_SavePreStepStatus)
      << ", last post step status: "
      << PHG4StepStatusDecode::GetStepStatus(m_SavePostStepStatus) << endl;
    cout << "last track: " << m_SaveTrackId
      << ", current trackid: " << aTrack->GetTrackID() << endl;
    cout << "phys pre vol: " << preVol->GetName()
      << " post vol : " << postTouch->GetVolume()->GetName() << endl;
    cout << " previous phys pre vol: " << m_SaveVolPre->GetName()
      << " previous phys post vol: " << m_SaveVolPost->GetName() << endl;
    // This is fatal - a hit from nowhere. This needs to be looked at and fixed
    gSystem->Exit(1);
  }
  // check if track id matches the initial one when the hit was created
  if(aTrack->GetTrackID() != m_SaveTrackId) {
    cout << GetName() << ": hits do not belong to the same track" << endl;
    cout << "saved track: " << m_SaveTrackId
      << ", current trackid: " << aTrack->GetTrackID()
      << ", prestep status: " << prePoint->GetStepStatus()
      << ", previous post step status: " << m_SavePostStepStatus << endl;
    // This is fatal - a hit from nowhere. This needs to be looked at and fixed
    gSystem->Exit(1);
  }
  // We need to cache a few things from one step to the next
  // to identify impossible hits and subsequent debugging printout
  m_SavePreStepStatus = prePoint->GetStepStatus();
  m_SavePostStepStatus = postPoint->GetStepStatus();
  m_SaveVolPre = preVol;
  m_SaveVolPost = postVol;


  // update accumulators
  m_EdepSum += edep;
  if(whichactive > 0) {
    m_EionSum += eion;
  }


  // save the hit ---------------------------------------------
  
  // hit classifiers
  enum hitTypes {
    hEntrance,         /* vessel entrance, primary */
    hEntranceAtypical, /* vessel entrance, not primary */
    hPSST,             /* photosensor hit */
    hIgnore            /* ignored hit */
  };
  int hitType;
  G4String hitTypeStr;

  // if any of these conditions is true, this is the last step in
  // this volume and we consider saving the hit
  int petal = isEntrance ? 0 : m_Detector->GetPetal(preVol);
  if(postPoint->GetStepStatus() == fGeomBoundary /*left volume*/
  || postPoint->GetStepStatus() == fWorldBoundary /*left world*/
  || postPoint->GetStepStatus() == fAtRestDoItProc /*track stops*/
  || aTrack->GetTrackStatus() == fStopAndKill /*track ends*/
  ) {

    // get prePoint and postPoint volume names
    G4String preName = prePoint->GetPhysicalVolume()->GetName();
    G4String postName = postPoint->GetPhysicalVolume()->GetName();
    if(verbose) 
      cout << "[---+] last step in the volume (pre=" << preName
           << ", post=" << postName << ")" << endl;

    // classify hit type
    if(  preName.contains("DRICHpetal")
      && postName.contains("DRICHpsst")) hitType = hPSST;
    else if(isEntrance)
      hitType = entClass==entPrimary ? hEntrance : hEntranceAtypical;
    else hitType = hIgnore;
    // set string
    switch(hitType) {
      case hPSST:             hitTypeStr = "psst"; break;
      case hEntrance:         hitTypeStr = "entrance"; break;
      case hEntranceAtypical: hitTypeStr = "entranceAtypical"; break;
      default: hitTypeStr = "UNKNOWN";
    }


    // hits to keep +++++++++++++++++++++++
    if(hitType != hIgnore) {

      if(verbose)
        cout << "[-+] " << hitTypeStr << " hit, KEEP!" << endl;

      // set hit vars
      m_Hit->set_hit_type_name(hitTypeStr);
      m_Hit->set_petal(petal);
      m_Hit->set_psst(m_Detector->GetPSST(postVol));
      m_Hit->set_pdg(aTrack->GetParticleDefinition()->GetPDGEncoding());
      m_Hit->set_particle_name(aTrack->GetParticleDefinition()->GetParticleName());
      switch(hitType) {
        case hPSST:
        case hEntranceAtypical:
          if(entClass==entPostStep) m_Hit->set_process("postStepDoItVector");
          else m_Hit->set_process(aTrack->GetCreatorProcess()->GetProcessName());
          break;
        case hEntrance:
          m_Hit->set_process("primary");
          break;
      };
      m_Hit->set_parent_id(aTrack->GetParentID());
      m_Hit->set_position(1, postPoint->GetPosition() / cm);
      m_Hit->set_momentum(aTrack->GetMomentum() / eV);
      m_Hit->set_momentum_dir(aTrack->GetMomentumDirection() /*unitless*/);
      m_Hit->set_vertex_position(aTrack->GetVertexPosition() / cm);
      m_Hit->set_vertex_momentum_dir(aTrack->GetVertexMomentumDirection() /*unitless*/);
      m_Hit->set_t(1, postPoint->GetGlobalTime() / nanosecond);


      // tracking of the truth info // TODO: not used yet?
      if(G4VUserTrackInformation *p = aTrack->GetUserInformation()) {
        if(PHG4TrackUserInfoV1 *pp = dynamic_cast<PHG4TrackUserInfoV1 *>(p)) {
          pp->SetKeep(1);  // we want to keep the track
        }
      }


      // total accumulators
      m_Hit->set_edep(m_EdepSum);
      if(whichactive > 0) {
        m_Hit->set_eion(m_EionSum);
      }
      if(verbose)
        cout << "[-+] m_EdepSum=" << m_EdepSum
             << ", m_EionSum=" << m_EionSum
             << endl;
           

      // transfer ownership to container
      m_SaveHitContainer->AddHit(petal, m_Hit);
      m_Hit = nullptr; // so that next track will create new hit
    }

    else {
      // do not save this hit ++++++++++++++++++++++++++++
      // - reset hit object for reuse
      // - if last hit overall, memory still allocated
      if(verbose) cout << "[-+] not keeping this hit" << endl;
      m_Hit->Reset();
    };
  };

  // return true to indicate the hit was used
  return true;
}

//____________________________________________________________________________..
void dRIChSteppingAction::SetInterfacePointers(PHCompositeNode *topNode)
{
  string hitnodename = "G4HIT_" + m_Detector->GetName();
  // now look for the map and grab a pointer to it.
  m_HitContainer = findNode::getClass<PHG4HitContainer>(topNode, hitnodename);
  // if we do not find the node we need to make it.
  if (!m_HitContainer)
  {
    cout << "dRIChSteppingAction::SetTopNode - unable to find "
      << hitnodename << endl;
  }
}
