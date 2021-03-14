#ifndef G4E_CI_DRICH_MODEL_HH
#define G4E_CI_DRICH_MODEL_HH


#include <G4PVDivision.hh>
#include "G4RotationMatrix.hh"
#include "G4Material.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"
#include "G4MaterialPropertiesTable.hh"
 
//#include "ci_DRICH_Config.hh"

/*
 * Service Classes
 */

// Aerogel
class ciDRICHar {

public:

  double *scaledE;
  double *scaledN;
  double *scaledA;
  double *scaledS;
  
  ciDRICHar() {
    scaledE=NULL;
    scaledN=NULL;
    scaledA=NULL;
    scaledS=NULL;
    
  };
  
  ~ciDRICHar() {
    if (scaledE != NULL) {
      delete[] scaledE;
      delete[] scaledN;
      delete[] scaledA;
      delete[] scaledS;
    }
  };
  
  //
  // Compute the refractive index, absorption length, scattering length for different energies points
  // 
  // different methods are available for the refractive index:
  //   0 - Vorobiev
  //   1 - Sellmeier - CLAS12
  //   2 - Sellmeier - LHCB
  //   3 - CLAS12 experimental points rescaled by Alessio/GEMC (the same used for the scattering and absorption length)
  //
  // data are scaled according to the input density of the aerogel
  //
  //
  int setOpticalParams(G4MaterialPropertiesTable* mpt, double density, int mode) {
    
    const double aeroE[]= // energy : wavelenth 660 nm -> 200 nm
      { 1.87855*eV,1.96673*eV,2.05490*eV,2.14308*eV,2.23126*eV, 2.31943*eV,2.40761*eV,2.49579*eV,2.58396*eV,2.67214*eV,
	2.76032*eV,2.84849*eV,2.93667*eV,3.02485*eV,3.11302*eV, 3.20120*eV,3.28938*eV,3.37755*eV,3.46573*eV,3.55391*eV,
	3.64208*eV,3.73026*eV,3.81844*eV,3.90661*eV,3.99479*eV, 4.08297*eV,4.17114*eV,4.25932*eV,4.34750*eV,4.43567*eV,
	4.52385*eV,4.61203*eV,4.70020*eV,4.78838*eV,4.87656*eV, 4.96473*eV,5.05291*eV,5.14109*eV,5.22927*eV,5.31744*eV,
	5.40562*eV,5.49380*eV,5.58197*eV,5.67015*eV,5.75833*eV, 5.84650*eV,5.93468*eV,6.02286*eV,6.11103*eV,6.19921*eV };
    
    const double aeroN[]= // refractive index - scaled from CLAS12 RICH to n(300) = 1.02, rho ~ 0.088, n(400)~1.019
      { 1.01825,1.01829,1.01834,1.01839,1.01844, 1.01849,1.01854,1.01860,1.01866,1.01872,
	1.01878,1.01885,1.01892,1.01899,1.01906, 1.01914,1.01921,1.01929,1.01938,1.01946,
	1.01955,1.01964,1.01974,1.01983,1.01993, 1.02003,1.02014,1.02025,1.02036,1.02047,
	1.02059,1.02071,1.02084,1.02096,1.02109, 1.02123,1.02137,1.02151,1.02166,1.02181,
	1.02196,1.02212,1.02228,1.02244,1.02261, 1.02279,1.02297,1.02315,1.02334,1.02354 };
    
    const double aeroA[]= // absorption length - scaled from CLAS12 RICH to n(300) = 1.02, rho ~ 0.088
      { 17.5000*cm,17.7466*cm,17.9720*cm,18.1789*cm,18.3694*cm, 18.5455*cm,18.7086*cm,18.8602*cm,19.0015*cm,19.1334*cm,
	19.2569*cm,19.3728*cm,19.4817*cm,19.5843*cm,19.6810*cm, 19.7725*cm,19.8590*cm,19.9410*cm,20.0188*cm,20.0928*cm,
	18.4895*cm,16.0174*cm,13.9223*cm,12.1401*cm,10.6185*cm, 9.3147*cm,8.1940*cm,7.2274*cm,6.3913*cm,5.6659*cm,
	5.0347*cm,4.4841*cm,4.0024*cm,3.5801*cm,3.2088*cm, 2.8817*cm,2.5928*cm,2.3372*cm,2.1105*cm,1.9090*cm,
	1.7296*cm,1.5696*cm,1.4266*cm,1.2986*cm,1.1837*cm, 1.0806*cm,0.9877*cm,0.9041*cm,0.8286*cm,0.7603*cm };
    
    const double aeroS[] = // rayleigh - scaled from CLAS12 RICH to n(300) = 1.02, rho ~ 0.088
      { 35.1384*cm, 29.24805*cm, 24.5418*cm, 20.7453*cm, 17.6553*cm, 15.1197*cm, 13.02345*cm, 11.2782*cm, 9.81585*cm, 8.58285*cm,
	7.53765*cm, 6.6468*cm, 5.88375*cm, 5.22705*cm, 4.6596*cm, 4.167*cm, 3.73785*cm, 3.36255*cm, 3.03315*cm, 2.7432*cm, 
	2.48700*cm, 2.26005*cm, 2.05845*cm, 1.87875*cm, 1.71825*cm, 1.57455*cm, 1.44555*cm, 1.3296*cm, 1.2249*cm, 1.1304*cm,
	1.04475*cm, 0.9672*cm, 0.89655*cm, 0.83235*cm, 0.77385*cm, 0.7203*cm, 0.67125*cm, 0.6264*cm, 0.58515*cm, 0.54735*cm,
	0.51255*cm, 0.48045*cm, 0.45075*cm, 0.4233*cm, 0.39795*cm, 0.37455*cm, 0.3528*cm, 0.33255*cm, 0.3138*cm, 0.29625*cm };

    const int nEntries = sizeof(aeroE)/sizeof(double);

    double refn = density2refIndex(density); // use a n vs rho formula with provide n at 400 nm
    double refwl = 400*nm;
    
    double refee = refwl/nm / 1239.84193 * eV; // reference energy
    double an0 = linint(refee, nEntries, aeroE, aeroN);
    //    double aa0 = linint(refee, nEntries, aeroE, aeroA);
    //    double as0 = linint(refee, nEntries, aeroE, aeroS);

    double aa;
    double nn;
    double ri, rho, a0, wl0, rnscale;
    
    if (scaledE==NULL) {
      scaledE = new double[nEntries];
      scaledN = new double[nEntries];
      scaledS = new double[nEntries];
      scaledA = new double[nEntries];
    }
    
    for (int i=0;i<nEntries;i++) {
      double ee = aeroE[i]; 
      double wl = 1239.84193 / (ee/eV) * nm; // from eV to nm

      switch (mode) {
      case 0:     // --- Vorobiev model
	aa = airFraction(refn, refwl);
	nn = aa * riAir(wl) + (1. - aa)*riQuartz(wl);
	break;
      case 1:     // --- Sellmeier, 1 pole from  (CLAS12/RICH EPJ A (2016) 52: 23)
	ri = 1.0494; // 400 nm
	rho = 0.230; // g/cm3
	a0 = 0.09683;
	wl0 = 84.13;
	rnscale =  sqrt(1.+ (a0*refwl*refwl)/(refwl*refwl-a0*a0));
	nn = sqrt(1.+ (a0*wl*wl)/(wl*wl-a0*a0))* refn / rnscale;
	break;
      case 2:    // --- Sellmeier, 1 pole from T. Bellunato et al. Eur. Phys. J. C52, 759-764 (2007)
	ri = 1.03; // 400 nm
	rho = 0.149; // g/cm3
	a0 = 0.05639;
	wl0 = 84.22;
	rnscale =  sqrt(1.+ (a0*refwl*refwl)/(refwl*refwl-a0*a0));
	nn = sqrt(1.+ (a0*wl*wl)/(wl*wl-a0*a0)) * refn / rnscale;
	break;
      case 3:    // --- experimental points 
	rho = 0.088; // g/cm3
	nn = aeroN[i] * refn / an0; // scale refractive index
	break;
      default:
	nn = refn;
	break;
      }

      scaledE[i] = ee;
      scaledN[i] = nn;
      scaledA[i] = aeroA[i] * rho/density; // approx. larger the density, smaller the abs. length
      scaledS[i] = aeroS[i] * rho/density; // approx. larger the density, smaller the abs. length

    }

    printf("Aerogel Refractive Index, Absorption and Scattering Lengths rescaled to density %f g/cm3, method: %d\n", density/g*cm3, mode);

    mpt->AddProperty("RINDEX",     scaledE, scaledN , nEntries)->SetSpline(true);
    mpt->AddProperty("ABSLENGTH",  scaledE, scaledA, nEntries)->SetSpline(true);
    mpt->AddProperty("RAYLEIGH",   scaledE, scaledS, nEntries)->SetSpline(true);
    mpt->AddConstProperty("SCINTILLATIONYIELD", 0. / MeV); // TBC
    mpt->AddConstProperty("RESOLUTIONSCALE", 1.0);
    
    return nEntries;
    
  }
  
private:

  // Linear Interpolation
  double linint(double val, int n, const double* x, const double* y) {

    if (val<=x[0]) return y[0];
    if (val>=x[n-1]) return y[n-1];
    
    for (int i=0;i<(n-1);i++) {
      if ((val>=x[i]) && (val<x[i+1])) {
	return (y[i+1]-y[i])/(x[i+1]-x[i])*(val-x[i])+y[i];
      }
    }
    
    return 0.;
    
  }
  
  
  // Quartz (SiO2) Refractive index: https://refractiveindex.info/?shelf=main&book=SiO2&page=Malitson
  double riQuartz(double wl) { // wavelength   
    double x = wl / um;
    double nn = sqrt(1+0.6961663*x*x/(x*x-pow(0.0684043,2))+0.4079426*x*x/(x*x-pow(0.1162414,2))+0.8974794*x*x/(x*x-pow(9.896161,2)));
    if (nn<1.) {
      printf("WARNING: estimated quartz refractive index is %f at wavelenght %f nm -> set to 1\n",nn,x);
      nn = 1.;
    }
    return nn;
  }

  // Air Refractive index: https://refractiveindex.info/?shelf=other&book=air&page=Ciddor
  double riAir(double wl) { // wavelength   
    double x = wl / um;
    double nn = 1.0+(0.05792105/(238.0185-1.0/x/x)+0.00167917/(57.362-1.0/x/x));
    if (nn<1.) {
      printf("WARNING: estimated air refractive index is %f at wavelenght %f nm -> set to 1\n",nn,x);
      nn = 1.;
    }
    return nn;
  }

  /*
   * n_aer = A * n_air + (1-A) * n_quartz  : compute air weight-fraction given a reference n_air,lambda
   *  Vorobiev Model
   *    rn     : reference refractive index of the aerogel
   *    rlambda: wavelength of the reference refractive index
   */
  double airFraction(double rn, double rlambda) {
    double rnq = riQuartz(rlambda);
    double rna = riAir(rlambda);
    double a = (rnq - rn)/(rnq - rna);
    return a;
  }
  
  // density of aerogel from air and quartz densities
  // rho = (n-1)/0.21 g/cm3 (Bellunato) -> rho proportional to (n-1)
  // rho_air    = 0.0011939 g/cm3   T=25 deg
  // rho_quartz = 2.196 g/cm3  (amorphous ?)
  double Density(double rn, double rlambda) {
    double aa = airFraction(rn, rlambda);
    return (aa * 1.1939 * mg/cm3 + (1.- aa ) * 2.32 * g/cm3);
  }

  // at 400 nm - (CLAS12/RICH EPJ A (2016) 52: 23)
  double density2refIndex(double rho) {
    double nn2 = 1.+0.438*rho/g*cm3;
    return sqrt(nn2);
  }
  
};
  
#endif //G4E_CI_DRICH_MODEL_HH

