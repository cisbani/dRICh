#ifndef DRICHDETECTOR_H
#define DRICHDETECTOR_H

#include <g4main/PHG4Detector.h>

#include <set>
#include <map>
#include <string>  // for string
#include <fstream>

class G4LogicalVolume;
class G4VPhysicalVolume;
class PHCompositeNode;
class PHG4Subsystem;
class PHParameters;

class dRichDetector : public PHG4Detector
{
  public:
    //! constructor
    dRichDetector(PHG4Subsystem *subsys, PHCompositeNode *Node, PHParameters *parameters, const std::string &dnam);

    //! destructor
    virtual ~dRichDetector() {}

    //! construct
    void ConstructMe(G4LogicalVolume *world) override;

    void Print(const std::string &what = "ALL") const override;

    //!@name volume accessors
    //@{
    int IsInDetector(G4VPhysicalVolume *) const;
    //@}

    // recursively add detectors to active volume list
    void ActivateVolumeTree(G4VPhysicalVolume *volu, G4int petal=0);

    // access detector numbers, for the given volume
    int GetPetal(G4VPhysicalVolume *volu);
    int GetPSST(G4VPhysicalVolume *volu);

    void SuperDetector(const std::string &name) { m_SuperDetector = name; }
    const std::string SuperDetector() const { return m_SuperDetector; }


  private:
    PHParameters *m_Params;

    // active volumes
    std::set<G4VPhysicalVolume *> m_PhysicalVolumesSet;
    std::map<G4VPhysicalVolume*,G4int> m_PetalMap;

    std::string m_SuperDetector;
};

#endif // DRICHDETECTOR_H
