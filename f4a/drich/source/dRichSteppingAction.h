#ifndef DRICHSTEPPINGACTION_H
#define DRICHSTEPPINGACTION_H

#include <g4main/PHG4SteppingAction.h>

class dRichDetector;

class G4Step;
class G4VPhysicalVolume;
class PHCompositeNode;
class PHG4Hit;
class PHG4HitContainer;
class PHParameters;

class dRichSteppingAction : public PHG4SteppingAction
{
  public:
    //! constructor
    dRichSteppingAction(dRichDetector*, const PHParameters* parameters);

    //! destructor
    virtual ~dRichSteppingAction();

    //! stepping action
    virtual bool UserSteppingAction(const G4Step*, bool);

    //! reimplemented from base class
    virtual void SetInterfacePointers(PHCompositeNode*);

  private:
    //! pointer to the detector
    dRichDetector * m_Detector;
    const PHParameters * m_Params;
    //! pointer to hit container
    PHG4HitContainer * m_HitContainer;
    PHG4Hit * m_Hit; // (actually type PHG4Hitv1*)
    PHG4HitContainer * m_SaveHitContainer;
    G4VPhysicalVolume * m_SaveVolPre;
    G4VPhysicalVolume * m_SaveVolPost;

    int m_SaveTrackId;
    int m_SavePreStepStatus;
    int m_SavePostStepStatus;
    int m_ActiveFlag;
    double m_EdepSum;
    double m_EionSum;
};

#endif // DRICHSTEPPINGACTION_H
