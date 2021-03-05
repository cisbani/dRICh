/*
 * Simple dRICH geometry, based on A. Del Dotto - GEMC MonteCarlo model
 * (https://github.com/EIC-eRD11/dualRICH_inMEIC/tree/master/gemc_drich)
 *
 * Date: May 2020
 *
 * Author: E. Cisbani
 *
 * Version: 0.2
 *
 * units are: cm, deg
 *
 */

#include "TGeoManager.h"
//#include "TGeoOpticalSurface.h"
#include "TMath.h"
#include "TLine.h"


/*
 * r_entr, r_exit : entrance and exit vessel radius
 *
 * unit lenght is cm
 *
 */
TGeoManager *BuildDRichModel(Double_t r_entr = 210., Double_t r_exit = 210.) {
    Double_t PIG = 3.1415;
    Double_t DEG = 180. / PIG;

    Double_t dx_world = 500;
    Double_t dy_world = 500;
    Double_t dz_world = 500;

    // dRICH
    Double_t z0 = 0.;

    // dRICH vessel
    Double_t dz_vessel = 100;
    Double_t rin_vessel = 10.;
    //  Double_t rout0_vessel= 300.;
    //  Double_t rout1_vessel= 350.;
    Double_t rout0_vessel = r_entr;
    Double_t rout1_vessel = r_exit;
    Double_t phi0_vessel = -30.;
    Double_t phi1_vessel = 30.;

    // aerogel (relative to vessel)
    Double_t zc_aerogel = -80.5;
    Double_t dz_aerogel = 2;
    Double_t r0_aerogel = 10.;
    Double_t r1_aerogel = 120.;

    // acrylic filter (relative to vessel)
    Double_t zc_acrylic = -78.34;
    Double_t dz_acrylic = 0.16;

    // mirror (relative to vessel)
    Double_t xc_mirror = 145.; // center of the sphere
    Double_t yc_mirror = 0.;
    Double_t zc_mirror = -210.;
    Double_t r0_mirror = 289.9; // radius
    Double_t r1_mirror = 290.0;
    Double_t the0_mirror = TMath::ASin(xc_mirror / r0_mirror) * DEG;
    Double_t t0_mirror =
            the0_mirror - TMath::ASin((xc_mirror - rin_vessel) / r0_mirror) * DEG + 0.2; // something sligtly more ..
    Double_t t1_mirror = 40.; // max theta

    printf("Aerogel:\n");
    printf("  Surface (all sectors): %f cm2\n", PIG * (r1_aerogel * r1_aerogel - r0_aerogel * r0_aerogel));

    printf("Mirror (spherical cap)\n");
    printf("   center: transverse (from beam axis): %f cm\n", xc_mirror);
    printf("           longitudinal (from vertex): %f cm\n", zc_mirror + z0 + dz_vessel);
    printf("   radius: %f\n", r1_mirror);
    printf("   Rotation along y axis %f deg, theta_min/max %f %f deg\n", the0_mirror, t0_mirror, t1_mirror);
    Double_t cord0 = r1_mirror * TMath::Sin(t0_mirror / DEG);
    Double_t cord1 = r1_mirror * TMath::Sin(t1_mirror / DEG);
    Double_t hh0 = r1_mirror * (1. - TMath::Cos(t0_mirror / DEG));
    Double_t hh1 = r1_mirror * (1. - TMath::Cos(t1_mirror / DEG));
    printf("   Spherical Small and Large Sectors (r and h): %f %f - %f %f cm\n", cord0, hh0, cord1, hh1);
    printf("   Surface (all sectors): %f cm2\n", PIG * r1_mirror * (2. * hh1 - 2. * hh0)); // calotta sferica
    // Phton Detector Geometry
    Double_t dx_phdet = 2.5;
    Double_t dy_phdet = 2.5;
    Double_t dz_phdet = 0.5;

#define NPHDET 320
    Double_t xc_phdet[NPHDET] = {144.5, 144.5, 144.5, 144.5, 144.5, 144.5, 144.5, 144.5, 144.5, 144.5, 144.5, 144.5,
                                 149.5, 149.5, 149.5, 149.5, 149.5, 149.5, 149.5, 149.5, 149.5, 149.5, 149.5, 149.5,
                                 154.5, 154.5, 154.5, 154.5, 154.5, 154.5, 154.5, 154.5, 154.5, 154.5, 154.5, 154.5,
                                 159.5, 159.5, 159.5, 159.5, 159.5, 159.5, 159.5, 159.5, 159.5, 159.5, 159.5, 159.5,
                                 164.5, 164.5, 164.5, 164.5, 164.5, 164.5, 164.5, 164.5, 164.5, 164.5, 164.5, 164.5,
                                 164.5, 164.5,
                                 169.5, 169.5, 169.5, 169.5, 169.5, 169.5, 169.5, 169.5, 169.5, 169.5, 169.5, 169.5,
                                 169.5, 169.5,
                                 174.5, 174.5, 174.5, 174.5, 174.5, 174.5, 174.5, 174.5, 174.5, 174.5, 174.5, 174.5,
                                 174.5, 174.5,
                                 179.5, 179.5, 179.5, 179.5, 179.5, 179.5, 179.5, 179.5, 179.5, 179.5, 179.5, 179.5,
                                 179.5, 179.5,
                                 184.5, 184.5, 184.5, 184.5, 184.5, 184.5, 184.5, 184.5, 184.5, 184.5, 184.5, 184.5,
                                 184.5, 184.5, 184.5, 184.5,
                                 189.5, 189.5, 189.5, 189.5, 189.5, 189.5, 189.5, 189.5, 189.5, 189.5, 189.5, 189.5,
                                 189.5, 189.5, 189.5, 189.5,
                                 194.5, 194.5, 194.5, 194.5, 194.5, 194.5, 194.5, 194.5, 194.5, 194.5, 194.5, 194.5,
                                 194.5, 194.5, 194.5, 194.5,
                                 199.5, 199.5, 199.5, 199.5, 199.5, 199.5, 199.5, 199.5, 199.5, 199.5, 199.5, 199.5,
                                 199.5, 199.5, 199.5, 199.5,
                                 204.5, 204.5, 204.5, 204.5, 204.5, 204.5, 204.5, 204.5, 204.5, 204.5, 204.5, 204.5,
                                 204.5, 204.5, 204.5, 204.5, 204.5, 204.5,
                                 209.5, 209.5, 209.5, 209.5, 209.5, 209.5, 209.5, 209.5, 209.5, 209.5, 209.5, 209.5,
                                 209.5, 209.5, 209.5, 209.5, 209.5, 209.5,
                                 214.5, 214.5, 214.5, 214.5, 214.5, 214.5, 214.5, 214.5, 214.5, 214.5, 214.5, 214.5,
                                 214.5, 214.5, 214.5, 214.5, 214.5, 214.5,
                                 219.5, 219.5, 219.5, 219.5, 219.5, 219.5, 219.5, 219.5, 219.5, 219.5, 219.5, 219.5,
                                 219.5, 219.5, 219.5, 219.5, 219.5, 219.5,
                                 224.5, 224.5, 224.5, 224.5, 224.5, 224.5, 224.5, 224.5, 224.5, 224.5, 224.5, 224.5,
                                 224.5, 224.5, 224.5, 224.5, 224.5, 224.5, 224.5, 224.5,
                                 229.5, 229.5, 229.5, 229.5, 229.5, 229.5, 229.5, 229.5, 229.5, 229.5, 229.5, 229.5,
                                 229.5, 229.5, 229.5, 229.5, 229.5, 229.5, 229.5, 229.5,
                                 234.5, 234.5, 234.5, 234.5, 234.5, 234.5, 234.5, 234.5, 234.5, 234.5, 234.5, 234.5,
                                 234.5, 234.5, 234.5, 234.5, 234.5, 234.5, 234.5, 234.5,
                                 239.5, 239.5, 239.5, 239.5, 239.5, 239.5, 239.5, 239.5, 239.5, 239.5, 239.5, 239.5,
                                 239.5, 239.5, 239.5, 239.5, 239.5, 239.5, 239.5, 239.5};

    Double_t yc_phdet[NPHDET] = {-27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5, 22.5, 27.5,
                                 -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5, 22.5, 27.5,
                                 -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5, 22.5, 27.5,
                                 -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5, 22.5, 27.5,
                                 -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5, 22.5, 27.5, 32.5,
                                 -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5, 22.5, 27.5, 32.5,
                                 -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5, 22.5, 27.5, 32.5,
                                 -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5, 22.5, 27.5, 32.5,
                                 -37.5, -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5, 22.5, 27.5,
                                 32.5, 37.5,
                                 -37.5, -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5, 22.5, 27.5,
                                 32.5, 37.5,
                                 -37.5, -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5, 22.5, 27.5,
                                 32.5, 37.5,
                                 -37.5, -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5, 22.5, 27.5,
                                 32.5, 37.5,
                                 -42.5, -37.5, -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5,
                                 22.5, 27.5, 32.5, 37.5, 42.5,
                                 -42.5, -37.5, -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5,
                                 22.5, 27.5, 32.5, 37.5, 42.5,
                                 -42.5, -37.5, -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5,
                                 22.5, 27.5, 32.5, 37.5, 42.5,
                                 -42.5, -37.5, -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5, 17.5,
                                 22.5, 27.5, 32.5, 37.5, 42.5,
                                 -47.5, -42.5, -37.5, -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5,
                                 17.5, 22.5, 27.5, 32.5, 37.5, 42.5, 47.5,
                                 -47.5, -42.5, -37.5, -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5,
                                 17.5, 22.5, 27.5, 32.5, 37.5, 42.5, 47.5,
                                 -47.5, -42.5, -37.5, -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5,
                                 17.5, 22.5, 27.5, 32.5, 37.5, 42.5, 47.5,
                                 -47.5, -42.5, -37.5, -32.5, -27.5, -22.5, -17.5, -12.5, -7.5, -2.5, 2.5, 7.5, 12.5,
                                 17.5, 22.5, 27.5, 32.5, 37.5, 42.5, 47.5};

    Double_t zc_phdet[NPHDET] = {-39.8816, -39.1429, -38.5000, -37.9533, -37.5000, -37.5000, -37.5000, -37.5000,
                                 -37.9878, -38.5000, -39.1774, -39.8288,
                                 -39.8974, -39.2128, -38.5000, -38.3182, -37.5000, -37.5000, -37.5000, -37.5000,
                                 -38.1933, -38.5000, -39.3169, -39.8889,
                                 -40.2363, -39.4394, -38.5854, -38.4868, -37.9079, -37.5143, -37.5116, -37.8902,
                                 -38.4833, -38.6447, -39.4744, -40.1957,
                                 -40.5440, -39.6892, -39.2500, -38.5133, -38.5000, -38.4014, -38.4605, -38.5000,
                                 -38.5256, -39.2971, -39.6176, -40.5244,
                                 -42.0248, -41.0732, -40.3526, -39.6047, -39.3261, -38.8418, -38.5652, -38.5976,
                                 -38.7278, -39.3095, -39.6728, -40.3875, -41.0682, -42.0672,
                                 -42.8929, -41.8614, -40.9891, -40.3873, -39.8034, -39.5244, -39.5000, -39.5000,
                                 -39.5147, -39.8816, -40.3831, -40.9250, -41.7706, -42.7603,
                                 -43.6143, -42.7135, -41.8521, -41.2931, -40.6972, -40.4268, -40.2746, -40.2753,
                                 -40.4589, -40.7078, -41.2179, -41.9219, -42.7051, -43.7055,
                                 -44.7877, -43.7000, -42.9865, -42.1977, -41.7208, -41.5000, -41.3395, -41.2500,
                                 -41.4277, -41.8924, -42.2439, -42.9177, -43.7317, -44.6935,
                                 -47.1707, -45.9098, -44.9598, -44.1290, -43.4425, -42.9632, -42.7024, -42.5370,
                                 -42.5290, -42.7079, -42.9878, -43.4315, -44.0000, -44.9535, -45.9400, -47.0851,
                                 -48.4783, -47.3364, -46.3452, -45.5000, -44.9111, -44.2708, -44.0802, -43.9400,
                                 -43.8951, -44.0195, -44.2778, -44.9941, -45.4518, -46.3132, -47.3400, -48.4577,
                                 -50.0741, -48.8415, -47.9337, -47.1579, -46.5000, -45.9545, -45.5141, -45.3837,
                                 -45.3286, -45.5759, -45.9706, -46.3718, -47.0385, -47.8297, -48.9684, -49.9810,
                                 -51.8222, -50.6481, -49.7245, -48.6932, -47.9651, -47.6398, -47.3116, -47.0238,
                                 -47.0000, -47.2439, -47.6026, -48.1707, -48.7421, -49.6136, -50.5816, -51.7603,
                                 -55.1582, -53.7414, -52.5909, -51.5326, -50.5250, -50.1667, -49.5723, -49.2901,
                                 -49.0046, -49.0143, -49.2204, -49.4643, -50.0747, -50.7062, -51.7093, -52.6707,
                                 -53.8085, -55.2407,
                                 -57.2619, -55.9118, -54.6648, -53.5816, -52.8034, -52.1790, -51.6978, -51.3382,
                                 -51.1420, -51.0595, -51.3182, -51.7551, -52.0402, -52.7955, -53.7208, -54.6358,
                                 -55.9651, -57.3785,
                                 -59.8333, -58.3283, -57.0309, -55.9773, -55.3462, -54.5241, -54.0000, -53.6585,
                                 -53.3585, -53.3250, -53.5714, -53.8295, -54.5095, -55.1344, -56.1275, -57.2053,
                                 -58.3901, -59.8194,
                                 -62.4579, -61.1463, -59.7530, -58.5351, -57.7073, -57.0060, -56.5000, -56.1436,
                                 -55.8133, -56.0000, -56.1923, -56.5435, -56.9706, -57.6354, -58.5392, -59.5659,
                                 -61.0269, -62.2766,
                                 -66.8412, -65.4545, -63.8737, -62.5202, -61.2241, -60.5825, -59.7526, -59.2069,
                                 -58.8176, -58.6882, -58.6667, -59.0507, -59.2531, -59.8505, -60.5000, -61.5111,
                                 -62.3471, -63.7475, -65.5000, -67.0567,
                                 -70.3222, -68.5103, -66.9419, -65.5562, -64.3901, -63.5543, -62.7184, -62.0476,
                                 -61.9023, -61.7021, -61.5971, -61.8269, -62.1622, -62.9605, -63.6319, -64.5619,
                                 -65.6222, -67.0833, -68.4770, -70.3659,
                                 -73.5594, -71.9483, -70.5353, -68.7299, -67.7292, -66.8232, -66.1098, -65.5000,
                                 -65.2791, -65.1304, -64.9130, -65.0714, -65.5455, -66.0600, -66.7696, -67.5721,
                                 -68.8654, -70.3839, -71.8271, -73.6304,
                                 -77.3154, -75.6429, -74.1549, -72.8298, -71.5101, -70.1800, -69.5316, -68.9486,
                                 -68.5690, -68.2791, -68.3804, -68.6220, -69.0566, -69.4756, -70.4450, -71.2412,
                                 -72.4905, -74.0161, -75.8071, -77.4307};


    // ************* GEOMETRY CONSTRUCTION

    TGeoManager *geom = new TGeoManager("geom", "dRICH Geometry / Single Sector");

    // Materials are dummy in the model
    TGeoMaterial *Vacuum = new TGeoMaterial("vacuum", 0, 0, 0);
    TGeoMaterial *Fe = new TGeoMaterial("Fe", 55.845, 26, 7.87);

    // Media are dummy in the model
    TGeoMedium *Air = new TGeoMedium("Air", 0, Vacuum);
    TGeoMedium *Iron = new TGeoMedium("Iron", 0, Fe);

    // ----- Volumes

    // WORLD
    TGeoVolume *world = //geom->MakeBox("world", Air, dx_world, dy_world, dz_world);
            geom->MakeTube("ci_DRICH", Air, 0, dx_world, dz_world);
    geom->SetTopVolume(world);
    geom->SetTopVisible(kTRUE); // 0 make it invisible

    // BEAM PIPE
//    TGeoVolume *pipe = geom->MakeTube("pipe", Iron, rin_vessel * 0.8, rin_vessel * 0.95, z0);
//    pipe->SetVisibility(kTRUE);
//    pipe->SetLineColor(kViolet);
//    world->AddNode(pipe, 1, (TGeoMatrix *) new TGeoTranslation(0, 0, z0));




    // VESSEL
    TGeoVolume *vessel = geom->MakeCons("ci_DRICH_Vessel", Air, dz_vessel, rin_vessel, rout0_vessel, rin_vessel, rout1_vessel,
                                        phi0_vessel, phi1_vessel);
    vessel->SetVisibility(kTRUE);
    vessel->SetLineColor(kGray);


    TGeoVolume *vcenter = geom->MakeSphere("ci_DRICH_vcenter", Iron, 0, 5, 0, 180, 0, 360);
    vcenter->SetVisibility(kTRUE);
    vcenter->SetLineColor(kOrange);
    vessel->AddNode(vcenter, 1, new TGeoTranslation(0, 0, 0));


    // AEROGEL
    TGeoVolume *aerogel = geom->MakeCons("ci_DRICH_aerogel", Air, dz_aerogel, r0_aerogel, r1_aerogel, r0_aerogel, r1_aerogel,
                                         phi0_vessel, phi1_vessel);
    aerogel->SetVisibility(kTRUE);
    aerogel->SetLineColor(kYellow);

    vessel->AddNode(aerogel, 1, new TGeoTranslation(0, 0, zc_aerogel));

    // ACRYLIC
    TGeoVolume *acrylic = geom->MakeCons("ci_DRICH_acrylic", Air, dz_acrylic, r0_aerogel, r1_aerogel, r0_aerogel, r1_aerogel,
                                         phi0_vessel, phi1_vessel);
    acrylic->SetVisibility(kTRUE);
    acrylic->SetLineColor(kGreen);

    vessel->AddNode(acrylic, 1, new TGeoTranslation(0, 0, zc_acrylic));

    // MIRROR
    TGeoVolume *mirror = geom->MakeSphere("ci_DRICH_mirror", Iron, r0_mirror, r1_mirror, t0_mirror, t1_mirror, phi0_vessel,
                                          phi1_vessel);
    mirror->SetVisibility(kTRUE);
    mirror->SetLineColor(kBlue);
    //  mirror->SetColor(kBlue);

    // vessel->AddNode(mirror,1,new TGeoTranslation(xc_mirror,yc_mirror,zc_mirror));

    TGeoRotation *rot_mirror = new TGeoRotation("ci_DRICH_rotm", 90. - the0_mirror, 0., 90., 90., -the0_mirror, 0.);
    TGeoCombiTrans *com_mirror = new TGeoCombiTrans(xc_mirror, yc_mirror, zc_mirror, rot_mirror);
    vessel->AddNode(mirror, 1, com_mirror);

    //  vessel->AddNode(mirror, 0, tra_mirror);

    // PHOTON DETECTOR
    TGeoVolume *phdet = geom->MakeBox("ci_DRICH_phdet", Iron, dx_phdet, dy_phdet, dz_phdet);
    phdet->SetVisibility(kTRUE);
    phdet->SetLineColor(kBlack);
    for (Int_t i = 0; i < NPHDET; i++) {
        if ((xc_phdet[i] > 140.) && (xc_phdet[i] < 200.)) {
            vessel->AddNode(phdet, i, new TGeoTranslation(xc_phdet[i], yc_phdet[i], zc_phdet[i]));
        }
    }

    world->AddNode(vessel, 1, new TGeoTranslation(0, 0, z0 + dz_vessel));

    //  TGeoOpticalSurface *surf_mirror = new TGeoOpticalSurface("surfMirror", kMglisur, kFpolished, kTdielectric_metal);

    geom->CloseGeometry();
    geom->SetVisLevel(4);
    geom->SetVisOption(0);

    return geom;


    //  vessel->Raytrace();

}


void ci_DRICH_Model(Double_t r_entr = 210., Double_t r_exit = 210.) {

    auto geom = BuildDRichModel(r_entr, r_exit);
    auto vessel = geom->FindVolumeFast("ci_DRICH_vessel");
    auto pipe = geom->FindVolumeFast("ci_DRICH_pipe");
    //world->Draw("ogle");
    vessel->Draw();
    pipe->Draw("same");

}
