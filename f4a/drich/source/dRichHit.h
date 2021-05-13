#ifndef DRICHHIT_H
#define DRICHHIT_H

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Hitv1.h>
#include <g4main/PHG4HitDefs.h>

#include <climits>  // for INT_MIN, ULONG_LONG_MAX
#include <cmath>
#include <cstdint>
#include <iostream>
#include <map>

class dRichHit : public PHG4Hit
{
  public:
    dRichHit() = default;
    explicit dRichHit(const PHG4Hit* g4hit);
    virtual ~dRichHit() = default;
    void Reset();

    // accessors
    int get_trkid() const { return trackid; }
    float get_x(const int i) const { return x[i]; } // i:
    float get_y(const int i) const { return y[i]; } //  0=entry
    float get_z(const int i) const { return z[i]; } //  1=exit
    float get_t(const int i) const { return t[i]; }
    float get_delta_t() const { return t[1]-t[0]; }
    int get_petal() const { return petal; }
    int get_psst() const { return psst; }

    float get_edep() const { return edep; }
    PHG4HitDefs::keytype get_hit_id() const { return hitid; }
    int get_detid() const;
    int get_shower_id() const { return showerid; }

    // modifiers
    void set_x(const int i, const float f) { x[i] = f; }
    void set_y(const int i, const float f) { y[i] = f; }
    void set_z(const int i, const float f) { z[i] = f; }
    void set_t(const int i, const float f) { t[i] = f; }
    void set_petal(const int i) { petal = i; }
    void set_psst(const int i) { psst = i; }

    void set_edep(const float f) { edep = f; }
    void set_hit_id(const PHG4HitDefs::keytype i) { hitid = i; }
    void set_shower_id(const int i) { showerid = i; }
    void set_trkid(const int i) { trackid = i; }


    // diagnostics
    virtual void print() const;



  protected:
    float x[2] = {NAN, NAN};
    float y[2] = {NAN, NAN};
    float z[2] = {NAN, NAN};
    float t[2] = {NAN, NAN};
    int petal = INT_MIN;
    int psst = INT_MIN;
    PHG4HitDefs::keytype hitid = ULONG_LONG_MAX;
    int trackid = INT_MIN;
    int showerid = INT_MIN;
    float edep = NAN;

};

#endif
