//____________________________________________________________________________..
//
// This is a working template for the Stepping Action which needs to be implemented
// for active detectors. Most of the code is error handling and access to the G4 objects
// and our data structures. It does not need any adjustment. The only thing you need to
// do is to add the properties of the G4Hits you want to save for later analysis
// This needs to be done in 2 places, G4Hits are generated when a G4 track enters a new
// volume (or is created). Here you give it an initial value. When the G4 track leaves
// the volume the final value needs to be set.
// The places to do this is marked by //implement your own here//
//
// As guidance you can look at the total (integrated over all steps in a volume) energy
// deposit which should always be saved.
// Additionally the total ionization energy is saved - this can be removed if you are not
// interested in this. Naturally you may want remove these comments in your version
//
//____________________________________________________________________________..


/*

incident particle:
entry window 4-momentum
exit window 4-momentum

pmt: getCopyNumber

photon track:
getPosition
momentumDirection
momentum
vertexPosition (of cherenkov photon, emitted from incident particle)
vertexMomentumDirection
PDGEncoding
parentID

step:
getTotalEnergyDeposit # probably useful for non-optical particle

time:
getGlobalTime

*/

#include "dRichSteppingAction.h"

#include "dRichDetector.h"

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
dRichSteppingAction::dRichSteppingAction(dRichDetector *detector, const PHParameters *parameters)
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
{
}

//____________________________________________________________________________..
dRichSteppingAction::~dRichSteppingAction()
{
  // if the last hit was a zero energie deposit hit, it is just reset
  // and the memory is still allocated, so we need to delete it here
  // if the last hit was saved, hit is a nullptr pointer which are
  // legal to delete (it results in a no operation)
  delete m_Hit;
}

//____________________________________________________________________________..
// This is the implementation of the G4 UserSteppingAction
bool dRichSteppingAction::UserSteppingAction(const G4Step *aStep, bool was_used)
{
  //cout << "[+++++] call dRichSteppingAction::UserSteppingAction" << endl;
  G4TouchableHandle preTouch = aStep->GetPreStepPoint()->GetTouchableHandle();
  G4TouchableHandle postTouch = aStep->GetPostStepPoint()->GetTouchableHandle();
  // get volume of the current step
  G4VPhysicalVolume *volume = preTouch->GetVolume();
  // IsInDetector(volume) returns
  //  == 0 outside of detector
  //  > 0 for hits in active volume
  //  < 0 for hits in passive material
  int whichactive = m_Detector->IsInDetector(volume);
  if(!whichactive) return false;

  cout << "[_____] in " << volume->GetName() << ", whichactive=" << whichactive << endl;

  // collect energy and track length step by step
  G4double edep = aStep->GetTotalEnergyDeposit() / GeV;
  G4double eion = (aStep->GetTotalEnergyDeposit() - aStep->GetNonIonizingEnergyDeposit()) / GeV;
  const G4Track *aTrack = aStep->GetTrack();
  cout << "[_]  track " << aTrack->GetTrackID()
       << " is " << aTrack->GetParticleDefinition()->GetParticleName()
       << endl;
  printf("  track info @ 0x%p\n",(void*)aTrack->GetUserInformation());
  G4StepPoint *prePoint = aStep->GetPreStepPoint();
  G4StepPoint *postPoint = aStep->GetPostStepPoint();

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
  // 
  switch(prePoint->GetStepStatus())
  {
    // -- abnormal cases
    case fPostStepDoItProc: // step defined by PostStepDoItVector
      if (m_SavePostStepStatus != fGeomBoundary)
      {
        // this is the okay case, fPostStepDoItProc called in a volume, not first thing inside
        // a new volume, just proceed here
        break;
      }
      else
      {
        // this is an impossible G4 Step print out diagnostic to help debug, not sure if
        // this is still with us
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
        cout << "phys pre vol: " << volume->GetName()
          << " post vol : " << postTouch->GetVolume()->GetName() << endl;
        cout << " previous phys pre vol: " << m_SaveVolPre->GetName()
          << " previous phys post vol: " << m_SaveVolPost->GetName() << endl;
      }
    // -- normal cases
    case fGeomBoundary:
    case fUndefined:
      if(!m_Hit) {
        cout << "[+] make new hit" << endl;
        m_Hit = new PHG4Hitv1();
      }


      ///*// diagnostic printout
      if(prePoint->GetStepStatus()==fGeomBoundary) {
        cout << "[+] prepoint at geometry boundary" << endl;
        //cout << "[+]   preTouch volume  = " << volume->GetName() << endl;
        cout << "[+]   prePoint  volume = " << prePoint->GetPhysicalVolume()->GetName() << endl;
        cout << "[+]   postPoint volume = " << postPoint->GetPhysicalVolume()->GetName() << endl;
      } else {
        cout << "[+] prepoint has unknown status" << endl;
        //cout << "[+]   preTouch volume  = " << volume->GetName() << endl;
        cout << "[+]   prePoint  volume = " << prePoint->GetPhysicalVolume()->GetName() << endl;
        cout << "[+]   postPoint volume = " << postPoint->GetPhysicalVolume()->GetName() << endl;
      };

      cout << "[+] track ID = " << aTrack->GetTrackID() << endl;
      if(aTrack->GetTrackID()>1) {
        cout << "[+] track creator process = " << aTrack->GetCreatorProcess()->GetProcessName() << endl;
      };
      cout << "[+] particle name = " << aTrack->GetParticleDefinition()->GetParticleName() << endl;
      //*/

        /* pre/postPoint
         * pre/postTouch
         * volume (=preTouch->volume)
         * aStep
         * aTrack
         */

      /*// diagnostic printout
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
      cout << "phys pre vol: " << volume->GetName()
        << " post vol : " << postTouch->GetVolume()->GetName() << endl;
      cout << " previous phys pre vol: " << m_SaveVolPre->GetName()
        << " previous phys post vol: " << m_SaveVolPost->GetName() << endl;
      */
        
      // set petal number
      m_Hit->set_layer(m_Detector->GetPetal(volume));
      // here we set the entrance values in cm
      m_Hit->set_x(0, prePoint->GetPosition().x() / cm);
      m_Hit->set_y(0, prePoint->GetPosition().y() / cm);
      m_Hit->set_z(0, prePoint->GetPosition().z() / cm);
      // time in ns
      m_Hit->set_t(0, prePoint->GetGlobalTime() / nanosecond);
      // set the track ID
      m_Hit->set_trkid(aTrack->GetTrackID());
      m_SaveTrackId = aTrack->GetTrackID();
      // set the initial energy deposit
      m_EdepSum = 0; // TODO: maybe we don't need this
      // implement your own here://
      // add the properties you are interested in via set_XXX methods
      // you can find existing set methods in $OFFLINE_MAIN/include/g4main/PHG4Hit.h
      // this is initialization of your value. This is not needed you can just set the final
      // value at the last step in this volume later one
      if(whichactive > 0)
      {
        m_EionSum = 0;  // assuming the ionization energy is only needed for active
                        // volumes (scintillators)
        m_Hit->set_eion(0);
        m_SaveHitContainer = m_HitContainer;
      }
      else
      {
        cout << "implement stuff for whichactive < 0 (inactive volumes)" << endl;
        gSystem->Exit(1);
      }
      // this is for the tracking of the truth info
      if (G4VUserTrackInformation *p = aTrack->GetUserInformation())
      {
        if (PHG4TrackUserInfoV1 *pp = dynamic_cast<PHG4TrackUserInfoV1 *>(p))
        {
          m_Hit->set_trkid(pp->GetUserTrackId());
          pp->GetShower()->add_g4hit_id(m_SaveHitContainer->GetID(), m_Hit->get_hit_id());
        }
      }
      break;
    default:
      break;
  }

  // This section is called for every step
  // some sanity checks for inconsistencies (aka bugs) we have seen over the years
  // check if this hit was created, if not print out last post step status
  if (!m_Hit || !std::isfinite(m_Hit->get_x(0)))
  {
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
    cout << "phys pre vol: " << volume->GetName()
      << " post vol : " << postTouch->GetVolume()->GetName() << endl;
    cout << " previous phys pre vol: " << m_SaveVolPre->GetName()
      << " previous phys post vol: " << m_SaveVolPost->GetName() << endl;
    // This is fatal - a hit from nowhere. This needs to be looked at and fixed
    gSystem->Exit(1);
  }
  // check if track id matches the initial one when the hit was created
  if (aTrack->GetTrackID() != m_SaveTrackId)
  {
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
  m_SaveVolPre = volume;
  m_SaveVolPost = postTouch->GetVolume();

  // here we just update the exit values, it will be overwritten
  // for every step until we leave the volume or the particle
  // ceases to exist
  // sum up the energy to get total deposited
  m_EdepSum += edep;
  if (whichactive > 0)
  {
    m_EionSum += eion;
  }


  // if any of these conditions is true this is the last step in
  // this volume and we need to save the hit
  // postPoint->GetStepStatus() == fGeomBoundary: track leaves this volume
  // postPoint->GetStepStatus() == fWorldBoundary: track leaves this world
  // (happens when your detector goes outside world volume)
  // postPoint->GetStepStatus() == fAtRestDoItProc: track stops (typically
  // aTrack->GetTrackStatus() == fStopAndKill is also set)
  // aTrack->GetTrackStatus() == fStopAndKill: track ends
  if (postPoint->GetStepStatus() == fGeomBoundary ||
      postPoint->GetStepStatus() == fWorldBoundary ||
      postPoint->GetStepStatus() == fAtRestDoItProc ||
      aTrack->GetTrackStatus() == fStopAndKill)
  {
    // save only hits on the photo sensors
    if(prePoint->GetPhysicalVolume()->GetName().contains("DRICHpetal") &&
       postPoint->GetPhysicalVolume()->GetName().contains("DRICHpsst")) {

      cout << "[+++] hit on PSST, keep this hit!" << endl
           << "      (x,y,z) = (" << postPoint->GetPosition().x() / cm
                           << "," << postPoint->GetPosition().y() / cm
                           << "," << postPoint->GetPosition().z() / cm
                           << ")" << endl;

      // update values at exit coordinates and set keep flag of track to keep
      m_Hit->set_x(1, postPoint->GetPosition().x() / cm);
      m_Hit->set_y(1, postPoint->GetPosition().y() / cm);
      m_Hit->set_z(1, postPoint->GetPosition().z() / cm);
      m_Hit->set_t(1, postPoint->GetGlobalTime() / nanosecond);
      printf("track info ptr @ 0x%p\n",(void*)aTrack->GetUserInformation());
      if(G4VUserTrackInformation *p = aTrack->GetUserInformation()) {
        cout << "[+] here" << endl;
        if(PHG4TrackUserInfoV1 *pp = dynamic_cast<PHG4TrackUserInfoV1 *>(p)) {
          cout << "[+] set keep" << endl;
          pp->SetKeep(1);  // we want to keep the track
        }
      }

      cout << "  m_EdepSum=" << m_EdepSum
           << "  m_EionSum=" << m_EionSum
           << endl;
           

      m_Hit->set_edep(m_EdepSum);
      if(whichactive > 0) {
        m_Hit->set_eion(m_EionSum);
      }

      // transfer ownership to container
      m_SaveHitContainer->AddHit(m_Detector->GetPetal(volume), m_Hit);
      // then set to null so we will create a new hit for the next track
      m_Hit = nullptr;
    }
    else {
      // otherwise reset track for reuse; this means we have to delete it in
      // the dtor. If this was the last hit we processed the memory is still
      // allocated
      cout << "[+++] reset hit for reuse" << endl;
      m_Hit->Reset();
    };
  };
  // return true to indicate the hit was used
  return true;
}

//____________________________________________________________________________..
void dRichSteppingAction::SetInterfacePointers(PHCompositeNode *topNode)
{
  string hitnodename = "G4HIT_" + m_Detector->GetName();
  // now look for the map and grab a pointer to it.
  m_HitContainer = findNode::getClass<PHG4HitContainer>(topNode, hitnodename);
  // if we do not find the node we need to make it.
  if (!m_HitContainer)
  {
    cout << "dRichSteppingAction::SetTopNode - unable to find "
      << hitnodename << endl;
  }
}
