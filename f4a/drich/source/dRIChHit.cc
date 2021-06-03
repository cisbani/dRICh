#include "dRIChHit.h"

#include <phool/phool.h>


using std::cout;
using std::cerr;
using std::endl;

dRIChHit::dRIChHit(const PHG4Hit *g4hit) { CopyFrom(g4hit); };

void dRIChHit::Reset() { // TODO: make sure this is filled out
  hitid = ULONG_LONG_MAX;
  trackid = INT_MIN;
  showerid = INT_MIN;
  edep = NAN;
  for(int i=0; i<2; i++) {
    set_position(i,G4ThreeVector(NAN,NAN,NAN));
    set_t(i,NAN);
  };
};

int dRIChHit::get_detid() const {
  // a compile time check if the hit_idbits are within range (1-32)
  static_assert (PHG4HitDefs::hit_idbits <= sizeof(unsigned int)*8,"hit_idbits < 32, fix in PHG4HitDefs.h");
  int detid = (hitid>>PHG4HitDefs::hit_idbits);
  return detid;
};

void dRIChHit::print() const {
  cout<<"New dRIChHit  " << hitid 
    << "  on track "<<trackid<<" EDep "<<edep<<endl;
  cout<<"Location: X "<<x[0]<<"/"<<x[1]<<"  Y "<<y[0]<<"/"<<y[1]<<"  Z "<<z[0]<<"/"<<z[1]<<endl;
  cout<<"Time        "<<t[0]<<"/"<<t[1]<<endl;
}

