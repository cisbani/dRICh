#include "dRichHit.h"

#include <phool/phool.h>

#include <climits>
#include <cmath>
#include <cstdlib>
#include <string>
#include <utility>

using std::cout;
using std::cerr;
using std::endl;

dRichHit::dRichHit(const PHG4Hit *g4hit) { CopyFrom(g4hit); };

void dRichHit::Reset() {
  hitid = ULONG_LONG_MAX;
  trackid = INT_MIN;
  showerid = INT_MIN;
  edep = NAN;
  for(int i=0; i<2; i++) {
    set_x(i,NAN);
    set_y(i,NAN);
    set_z(i,NAN);
    set_t(i,NAN);
  };
};

int dRichHit::get_detid() const {
  // a compile time check if the hit_idbits are within range (1-32)
  static_assert (PHG4HitDefs::hit_idbits <= sizeof(unsigned int)*8,"hit_idbits < 32, fix in PHG4HitDefs.h");
  int detid = (hitid>>PHG4HitDefs::hit_idbits);
  return detid;
};

void dRichHit::print() const {
  cout<<"New dRichHit  " << hitid 
    << "  on track "<<trackid<<" EDep "<<edep<<endl;
  cout<<"Location: X "<<x[0]<<"/"<<x[1]<<"  Y "<<y[0]<<"/"<<y[1]<<"  Z "<<z[0]<<"/"<<z[1]<<endl;
  cout<<"Time        "<<t[0]<<"/"<<t[1]<<endl;
}

