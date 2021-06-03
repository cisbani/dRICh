#ifndef DRICHSTEPPINGACTION_H
#define DRICHSTEPPINGACTION_H

#include <g4main/PHG4SteppingAction.h>

class dRIChDetector;

class G4Step;
class G4VPhysicalVolume;
class PHCompositeNode;
class dRIChHit;
class PHG4HitContainer;
class PHParameters;

class dRIChSteppingAction : public PHG4SteppingAction
{
  public:
    //! constructor
    dRIChSteppingAction(dRIChDetector*, const PHParameters* parameters);

    //! destructor
    virtual ~dRIChSteppingAction();

    //! stepping action
    virtual bool UserSteppingAction(const G4Step*, bool);

    //! reimplemented from base class
    virtual void SetInterfacePointers(PHCompositeNode*);

  private:

    //! pointer to the detector
    dRIChDetector * m_Detector;
    const PHParameters * m_Params;
    //! pointer to hit container
    PHG4HitContainer * m_HitContainer;
    dRIChHit * m_Hit;
    PHG4HitContainer * m_SaveHitContainer;
    G4VPhysicalVolume * m_SaveVolPre;
    G4VPhysicalVolume * m_SaveVolPost;

    int m_SaveTrackId;
    int m_SavePreStepStatus;
    int m_SavePostStepStatus;
    int m_ActiveFlag;
    double m_EdepSum;
    double m_EionSum;
    bool verbose;
};

#endif // DRICHSTEPPINGACTION_H
