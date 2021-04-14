#ifndef DRICHDETECTOR_H
#define DRICHDETECTOR_H

#include <g4main/PHG4Detector.h>

#include <set>
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

    void SuperDetector(const std::string &name) { m_SuperDetector = name; }
    const std::string SuperDetector() const { return m_SuperDetector; }


  private:
    PHParameters *m_Params;

    // active volumes
    std::set<G4VPhysicalVolume *> m_PhysicalVolumesSet;

    std::string m_SuperDetector;
};

#endif // DRICHDETECTOR_H
