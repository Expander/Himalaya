// ====================================================================
// This file is part of Himalaya.
//
// Himalaya is licenced under the GNU General Public License (GNU GPL)
// version 3.
// ====================================================================

#include "H6.hpp"
#include "HierarchyCalculator.hpp"
#include "Hierarchies.hpp"
#include "Constants.hpp"
#include "Utils.hpp"
#include <cmath>
#include <type_traits>

/**
 * 	Constructor
 * 	@param flagMap the flagMap for the truncation of expansion variables
 * 	@param Al4p a double alpha_s/4/Pi
 * 	@param beta a double which is the mixing angle beta
 * 	@param Dmglst2 a double Mgl - Mst2
 * 	@param lmMt a double log((<renormalization scale> / Mt)^2)
 * 	@param lmMst1 a double log((<renormalization scale> / Mst1)^2)
 * 	@param lmMst2 a double log((<renormalization scale> / Mst2)^2)
 * 	@param lmMsq a double log((<renormalization scale> / Msq)^2)
 * 	@param Mt a double top/bottom quark mass
 * 	@param Mst1 a double stop 1 mass
 * 	@param Mst2 a double stop 2 mass
 * 	@param Msq a double average squark mass w/o the stop quark
 * 	@param MuSUSY a double mu parameter
 * 	@param s2t a double 2 times the sine of the stop/sbottom quark mixing angle
 * 	@param mdrFlag an int 0 for DR and 1 for MDR scheme
 * 	@param oneLoopFlag an int flag to consider the one-loop expansion terms
 * 	@param twoLoopFlag an int flag to consider the two-loop expansion terms
 * 	@param threeLoopFlag an int flag to consider the three-loop expansion terms
 */
himalaya::H6::H6(const std::map<unsigned int, unsigned int>& flagMap, double Al4p, double beta, double Dmglst2,
		 double lmMt, double lmMst1, double lmMst2, double lmMsq,
		 double Mt, double Mst1, double Mst2, double Msq, double MuSUSY,
		 double s2t,
		 int mdrFlag, int oneLoopFlag, int twoLoopFlag, int threeLoopFlag){
   // abbrev for tan(beta) and sin(beta)
   Tbeta = tan(beta);
   Sbeta = sin(beta);
   this -> Dmglst2 = Dmglst2;
   this -> lmMt = lmMt;
   this -> lmMst1 = lmMst1;
   this -> lmMst2 = lmMst2;
   this -> lmMsq = lmMsq;
   this -> Mt = Mt;
   this -> Mst1 = Mst1;
   this -> Mst2 = Mst2;
   this -> Msq = Msq;
   this -> MuSUSY = MuSUSY;
   this -> s2t = s2t;
   this -> oneLoopFlag = oneLoopFlag;
   this -> twoLoopFlag = twoLoopFlag;
   this -> threeLoopFlag = threeLoopFlag;
   this -> Al4p = Al4p;
   // mdr flags, indicates if one wants to shift the dr stop mass to the mdr stop mass
   shiftst1 = mdrFlag;
   shiftst2 = mdrFlag;
   shiftst3 = mdrFlag;
   xDR2DRMOD = mdrFlag;
   // expansion flags
   xDmglst2 = flagMap.at(ExpansionDepth::xxDmglst2);
   xMsq = flagMap.at(ExpansionDepth::xxMsq);
   xMst = flagMap.at(ExpansionDepth::xxMst);
}

/**
 * 	@return The diagonal (1, 1) matrix element of the Higgs mass matrix as a double for the hierarchy 'H6'
 */
double himalaya::H6::getS1() const {
   return -(pow2(Mt)*(-(pow2(MuSUSY)*(3645*oneLoopFlag*pow2(s2t)*(2 - lmMst1 +
        lmMst2 - (2*(lmMst1 - lmMst2)*pow2(Mst1)*(pow2(Mst1) + pow2(Mst2)))/
        pow4(Mst2)) + (19440*Al4p*pow2(s2t)*((2 - lmMst2)*twoLoopFlag*xDmglst2*
        xDR2DRMOD*pow2(Dmglst2)*((pow2(Mst1) + pow2(Mst2))*pow4(Mst2) - 2*(
        lmMst1 - lmMst2)*pow2(Mst1)*(pow2(Mst1)*pow2(Mst2) + pow4(Mst1) + pow4(
        Mst2))) + 5*Al4p*(1 - 2*lmMsq)*threeLoopFlag*xMsq*pow2(Msq)*pow2(Mst2)*
        ((1 - 2*lmMst2*(-1 + shiftst1))*pow2(Mst1)*pow2(Mst2) + 2*lmMst1*(-1 +
        shiftst1)*pow2(Mst1)*pow2(Mst2) + shiftst2*pow2(Mst1)*(2*pow2(Mst1) +
        pow2(Mst2)) - 2*lmMst1*(1 - 2*shiftst1 + shiftst2)*pow4(Mst1) + 2*
        lmMst2*(1 - 2*shiftst1 + shiftst2)*pow4(Mst1) + pow4(Mst2) - shiftst1*(
        2*pow2(Mst1)*pow2(Mst2) + 2*pow4(Mst1) + pow4(Mst2)))))/(pow2(Mst1)*
        pow6(Mst2)) + 2*threeLoopFlag*pow2(Al4p)*(-(Mt*s2t*(90*Mst2*(48*((5*(71
        + 12*lmMsq*(-2 + lmMst1 - lmMst2) + 40*lmMst2 - 4*lmMst1*(4 + 3*lmMst2)
        + 12*pow2(lmMst2)))/pow2(Msq) - (48*pow2(1 + lmMst2))/pow2(Mst1)) + (5*
        (Mst2*(Mst2*(911 + 12*lmMsq*(-37 + 18*lmMst1 - 18*lmMst2) + 606*lmMst2
        - 54*lmMst1*(3 + 4*lmMst2) + 216*pow2(lmMst2)) + Dmglst2*(5591 + 12*
        lmMsq*(-181 + 90*lmMst1 - 90*lmMst2) + 2550*lmMst2 - 54*lmMst1*(7 + 20*
        lmMst2) + 1080*pow2(lmMst2))) + (1153 + 12*lmMsq*(-35 + 54*lmMst1 - 54*
        lmMst2) + 906*lmMst2 - 162*lmMst1*(3 + 4*lmMst2) + 648*pow2(lmMst2))*
        pow2(Mst1)))/pow4(Msq)) - (60*(2*pow2(Msq)*pow2(Mst1)*(125789 + 13716*
        B4 - 54*DN - 29160*lmMsq + 3240*pow2(lmMsq) - 108*lmMst1*(422 - 45*
        lmMsq + 45*pow2(lmMsq)) - 2376*pow2(lmMst1) + 108*lmMst2*(1096 - 551*
        lmMst1 + 15*lmMsq*(-7 + 6*lmMst1) + 45*pow2(lmMsq) + 8*pow2(lmMst1)) -
        108*(-717 + 90*lmMsq + 248*lmMst1)*pow2(lmMst2) + 25920*pow3(lmMst2)) -
        720*(55 + 6*lmMsq*(-2 + 5*lmMst1 - 5*lmMst2) + 52*lmMst2 - 10*lmMst1*(4
        + 3*lmMst2) + 30*pow2(lmMst2))*pow4(Mst1)))/(pow2(Msq)*pow3(Mst2)) - (
        135*(-640*(23 + lmMsq*(-6 + 9*lmMst1 - 9*lmMst2) + 18*lmMst2 - 3*
        lmMst1*(4 + 3*lmMst2) + 9*pow2(lmMst2))*pow2(Msq)*pow2(Mst1) + 3*(18201
        + 1760*B4 - 16*DN - 5760*lmMsq + 960*pow2(lmMsq) - 16*lmMst1*(199 - 30*
        lmMsq + 30*pow2(lmMsq)) + 16*lmMst2*(1291 - 322*lmMst1 + 30*lmMsq*(-5 +
        2*lmMst1) + 30*pow2(lmMsq) - 16*pow2(lmMst1)) - 256*pow2(lmMst1) - 32*(
        -313 + 30*lmMsq + 72*lmMst1)*pow2(lmMst2) + 2560*pow3(lmMst2))*pow4(
        Msq) - 20*(233 + 36*lmMsq*(-2 + 5*lmMst1 - 5*lmMst2) + 207*lmMst2 - 45*
        lmMst1*(3 + 4*lmMst2) + 180*pow2(lmMst2))*pow4(Mst1)))/(Mst2*pow4(Msq))
        + 14580*Dmglst2*((40*(95 + 12*lmMsq*(-2 + lmMst1 - lmMst2) + 32*lmMst2
        - 4*lmMst1*(2 + 3*lmMst2) + 12*pow2(lmMst2)))/(9.*pow2(Msq)) - (128*(-1
        + 2*lmMst2 + 3*pow2(lmMst2)))/(9.*pow2(Mst1)) - (481.4302469135802 + (
        1048*B4)/9. - (20*DN)/9. - 400*lmMsq + (80*pow2(lmMsq))/3. - (4*lmMst1*
        (-173 + 90*lmMsq + 30*pow2(lmMsq)))/9. + (4*lmMst2*(1623 - 130*lmMst1 +
        30*lmMsq*(-1 + 2*lmMst1) + 30*pow2(lmMsq) - 16*pow2(lmMst1)))/9. + (64*
        pow2(lmMst1))/9. - (8*(-265 + 30*lmMsq + 136*lmMst1)*pow2(lmMst2))/9. -
        (80*(77 + 6*lmMsq*(-2 + 3*lmMst1 - 3*lmMst2) + 24*lmMst2 - 6*lmMst1*(2
        + 3*lmMst2) + 18*pow2(lmMst2))*pow2(Mst1))/(9.*pow2(Msq)) + 128*pow3(
        lmMst2) - (5*(1961 + 180*lmMsq*(-2 + 5*lmMst1 - 5*lmMst2) + 675*lmMst2
        - 45*lmMst1*(7 + 20*lmMst2) + 900*pow2(lmMst2))*pow4(Mst1))/(27.*pow4(
        Msq)))/pow2(Mst2) - pow2(Mst1)*((5*lmMst1*(7 + 20*lmMst2) + lmMsq*(
        66.29629629629629 - 100*lmMst1 + 100*lmMst2) - (5*(8713 + 3282*lmMst2 +
        3240*pow2(lmMst2)))/162.)/pow4(Msq) + (881.6139917695473 + 248*B4 - 4*
        DN - (2560*lmMsq)/3. + lmMst1*(130.96296296296296 - 120*lmMsq - 40*
        pow2(lmMsq)) + (80*pow2(lmMsq))/3. + (176*pow2(lmMst1))/9. + (8*lmMst2*
        (4364 - 573*lmMst1 + 45*lmMsq*(5 + 6*lmMst1) + 135*pow2(lmMsq) + 24*
        pow2(lmMst1)))/27. - (8*(-377 + 90*lmMsq + 376*lmMst1)*pow2(lmMst2))/9.
         - (80*(107 + 6*lmMsq*(-2 + 5*lmMst1 - 5*lmMst2) + 32*lmMst2 - 10*
        lmMst1*(2 + 3*lmMst2) + 30*pow2(lmMst2))*pow2(Mst1))/(9.*pow2(Msq)) + (
        2944*pow3(lmMst2))/9.)/pow4(Mst2))) + (2*(-5*(2552929 + 257904*B4 -
        648*DN - 456840*lmMsq + 38880*pow2(lmMsq) - 216*lmMst1*(4591 - 360*
        lmMsq + 450*pow2(lmMsq)) + 41904*pow2(lmMst1) + 216*lmMst2*(9211 -
        6466*lmMst1 + 180*lmMsq*(-4 + 5*lmMst1) + 450*pow2(lmMsq) + 576*pow2(
        lmMst1)) - 864*(-1784 + 225*lmMsq + 840*lmMst1)*pow2(lmMst2) - 3456*
        pow3(lmMst1) + 604800*pow3(lmMst2))*pow4(Mst1) + 80*OepS2*(12*Mst2*(-
        253*Dmglst2 + 68*Mst2)*pow2(Mst1) + 63*(-5*Dmglst2 + 3*Mst2)*pow3(Mst2)
        + 1882*pow4(Mst1)) - 54*S2*(-12*Mst2*(Dmglst2*(4978 + 7590*lmMst1 -
        7590*lmMst2) - 15*(169 + 136*lmMst1 - 136*lmMst2)*Mst2)*pow2(Mst1) +
        27*(Dmglst2*(453 - 350*lmMst1 + 350*lmMst2) + 15*(-1 + 14*lmMst1 - 14*
        lmMst2)*Mst2)*pow3(Mst2) + 10*(9185 + 5646*lmMst1 - 5646*lmMst2)*pow4(
        Mst1))))/pow5(Mst2))) + (10*(3*xDmglst2*pow2(Dmglst2)*(3*Mst2*(144*(-
        608*z3 + 9*z4)*pow2(Mt) + pow2(Mst2)*pow2(s2t)*(129397*z3 + 6344*z4 +
        1254*pow2(z2)) - 8*Mst2*Mt*s2t*(-36013*z3 - 218*z4 + 3804*pow2(z2))) -
        4*s2t*pow2(Mst1)*(Mst2*s2t*(-243722*z3 - 7489*z4 + 795*pow2(z2)) + Mt*(
        -343276*z3 - 8090*z4 + 37437*pow2(z2)))) + 18*Mst2*pow2(Mst1)*(4*Mst2*
        Mt*s2t*(272*T1ep + 11396*z3 + 1135*z4 - 2388*pow2(z2)) - pow2(Mst2)*
        pow2(s2t)*(1948*T1ep - 56978*z3 - 216*lmMst1*z3 + 216*lmMst2*z3 + 1136*
        z4 + 813*pow2(z2)) - 3*pow2(Mt)*(-10667 + 96*B4 - 96*D3 + 48*DN + 3072*
        lmMst1 - 9600*lmMst2 + 3456*lmMst1*lmMst2 - 384*lmMt + 384*lmMst1*lmMt
        - 384*lmMst2*lmMt + 224*OepS2 + 13932*S2 - 4536*lmMst1*S2 + 4536*
        lmMst2*S2 - 336*T1ep - 2616*z3 - 600*z4 - 4992*pow2(lmMst2) + 1536*
        lmMst1*pow2(lmMst2) - 252*pow2(z2) - 1536*pow3(lmMst2))) + 54*(Mst2*Mt*
        s2t*(84*T1ep + 10490*z3 + 510*z4 - 1665*pow2(z2)) + 3*pow2(Mst2)*pow2(
        s2t)*(-32*T1ep + 1913*z3 + 12*lmMst1*z3 - 12*lmMst2*z3 - 10*z4 + 48*
        pow2(z2)) - 8*pow2(Mt)*(-436 + 6*B4 - 6*D3 + 3*DN + 48*lmMst1 - 408*
        lmMst2 + 96*lmMst1*lmMst2 - 24*lmMt + 972*S2 - 230*z3 - 27*z4 - 192*
        pow2(lmMst2) + 48*lmMst1*pow2(lmMst2) - 48*pow3(lmMst2)))*pow3(Mst2) -
        3*s2t*(-4*Mt*(3764*T1ep + 67232*z3 + 11764*z4 - 20505*pow2(z2)) + Mst2*
        s2t*(29428*T1ep - 614504*z3 - 1296*lmMst1*z3 + 1296*lmMst2*z3 + 15686*
        z4 + 18183*pow2(z2)))*pow4(Mst1) + 2*Dmglst2*(-18*pow2(Mst1)*(-6*pow2(
        Mst2)*pow2(s2t)*(172*T1ep + 9207*z3 + 185*z4 + 237*pow2(z2)) + 2*Mst2*
        Mt*s2t*(1012*T1ep - 59336*z3 - 709*z4 + 8535*pow2(z2)) + pow2(Mt)*(-
        28405 + 288*B4 - 288*D3 + 144*DN - 5472*lmMst1 - 25632*lmMst2 + 576*
        lmMst1*lmMst2 + 1152*lmMt - 576*lmMst1*lmMt + 576*lmMst2*lmMt - 224*
        OepS2 + 21060*S2 + 4536*lmMst1*S2 - 4536*lmMst2*S2 + 336*T1ep + 3000*z3
        - 1128*z4 - 5184*pow2(lmMst2) + 4608*lmMst1*pow2(lmMst2) + 252*pow2(z2)
        - 4608*pow3(lmMst2))) + 27*pow2(Mst2)*(Mst2*Mt*s2t*(-140*T1ep + 37474*
        z3 + 542*z4 - 5289*pow2(z2)) + pow2(Mst2)*pow2(s2t)*(56*T1ep + 16175*z3
        + 424*z4 + 474*pow2(z2)) - 16*pow2(Mt)*(-540 + 6*B4 - 6*D3 + 3*DN - 48*
        lmMst1 - 432*lmMst2 + 648*S2 + 83*z3 - 27*z4 - 96*pow2(lmMst2) + 48*
        lmMst1*pow2(lmMst2) - 48*pow3(lmMst2))) + 4*pow2(s2t)*(17308*T1ep +
        338536*z3 + 11327*z4 + 15897*pow2(z2))*pow4(Mst1))))/pow5(Mst2) +
        14580*pow2(s2t)*(97.70987654320987 - (40*B4)/9. + (4*D3)/9. - (2*DN)/9.
         - (185*lmMsq)/9. + (25*pow2(lmMsq))/3. - (lmMst1*(5279 - 1950*lmMsq +
        630*pow2(lmMsq)))/54. - (53*pow2(lmMst1))/9. + (lmMst2*(10421 - 6558*
        lmMst1 + 30*lmMsq*(-95 + 42*lmMst1) + 630*pow2(lmMsq) + 234*pow2(
        lmMst1)))/54. - ((-1460 + 210*lmMsq + 399*lmMst1)*pow2(lmMst2))/9. - (
        16*pow3(lmMst1))/9. + pow2(Mst1)*((111 - 3028*lmMst2 + 90*lmMsq*(4 -
        13*lmMst1 + 13*lmMst2) + 2*lmMst1*(1334 + 675*lmMst2) - 90*pow2(lmMst1)
        - 1260*pow2(lmMst2))/(135.*pow2(Msq)) - (4962073 + 205200*B4 - 5400*DN
        - 405000*lmMsq + 120*lmMst1*(57971 - 14625*lmMsq + 2700*pow2(lmMsq)) +
        900*(-1331 + 180*lmMsq)*pow2(lmMst1) - 120*lmMst2*(52436 - 70455*lmMst1
        + 225*lmMsq*(-65 + 36*lmMst1) + 2700*pow2(lmMsq) + 8280*pow2(lmMst1)) +
        900*(-8063 + 900*lmMsq + 3792*lmMst1)*pow2(lmMst2) + (90*(-1725 +
        lmMst1*(-7006 + 2640*lmMsq - 4800*lmMst2) + (7006 - 2640*lmMsq)*lmMst2
        + 1080*pow2(lmMst1) + 3720*pow2(lmMst2))*pow2(Mst1))/pow2(Msq) - 55800*
        pow3(lmMst1) - 2363400*pow3(lmMst2))/(24300.*pow2(Mst2))) + (376*pow3(
        lmMst2))/9. + pow2(Mst2)*(((2*(836 - 2235*lmMst2 + 75*lmMst1*(27 + 16*
        lmMst2) + 30*lmMsq*(7 - 40*lmMst1 + 40*lmMst2) - 1200*pow2(lmMst2)))/
        pow2(Msq) + (15*(341 + 642*lmMst2 + 64*lmMst1*(1 + lmMst2) - 30*lmMsq*(
        5 + 6*lmMst2) + 90*pow2(lmMsq) + 272*pow2(lmMst2)))/pow2(Mst1))/270. +
        (257250*Dmglst2*Mst2*(557 - 224*lmMst2 + 12*lmMsq*(1 - 32*lmMst1 + 32*
        lmMst2) + 4*lmMst1*(53 + 96*lmMst2) - 384*pow2(lmMst2))*pow2(Mst1) -
        128625*(18*Dmglst2*(13 - 28*lmMsq + 28*lmMst2) + (67 - 84*lmMsq + 84*
        lmMst2)*Mst2)*pow3(Mst2) + (4167613 - 19932360*lmMst2 + 20580*lmMst1*(
        701 + 540*lmMst2) + 420*lmMsq*(13109 - 26460*lmMst1 + 25620*lmMst2) +
        176400*pow2(lmMsq) - 10936800*pow2(lmMst2))*pow4(Mst1))/(5.5566e6*pow2(
        Mst1)*pow4(Msq))) + Dmglst2*((pow2(Mst1)*(109.11799176954733 - (8*B4)/
        3. + (32*D3)/9. - (20*DN)/9. + 80*lmMsq - lmMst1*(78.19061728395062 +
        20*pow2(lmMsq)) - (2888*pow2(lmMst1))/135. + lmMst2*(40*lmMsq*lmMst1 +
        20*pow2(lmMsq) + (4*(-21616 - 64515*lmMst1 + 31275*pow2(lmMst1)))/2025.
        ) - (4*(-5023 + 1350*lmMsq + 6285*lmMst1)*pow2(lmMst2))/135. + (5*(291
        + 2*(-103 + 84*lmMsq)*lmMst2 + lmMst1*(206 - 168*lmMsq + 168*lmMst2) -
        168*pow2(lmMst2))*pow2(Mst1))/(27.*pow2(Msq)) + (20*pow3(lmMst1))/27. +
        (3340*pow3(lmMst2))/27.))/pow3(Mst2) - (8*(24*lmMst2*(1 + lmMst2) + (5*
        (14 - 15*lmMsq + 15*lmMst2)*pow2(Mst1))/pow2(Msq))*pow3(Mst2))/(27.*
        pow4(Mst1)) + (Mst2*(-20*(-118 - 109*lmMst1 + 6*lmMsq*(-4 + 17*lmMst1 -
        17*lmMst2) + 133*lmMst2 - 102*lmMst1*lmMst2 + 102*pow2(lmMst2))*pow2(
        Msq)*pow2(Mst1) + 6*(-115 + 366*lmMst2 + 64*lmMst1*lmMst2 - 30*lmMsq*(-
        1 + 6*lmMst2) + 90*pow2(lmMsq) + 336*pow2(lmMst2))*pow4(Msq) + 5*(219 -
        95*lmMst2 + 132*lmMsq*lmMst2 + lmMst1*(95 - 132*lmMsq + 132*lmMst2) -
        132*pow2(lmMst2))*pow4(Mst1)))/(54.*pow2(Mst1)*pow4(Msq)) + (3197 -
        432*B4 + 576*D3 - 360*DN + 9180*lmMsq + 216*lmMst1*(7 - 15*pow2(lmMsq))
        + 1620*pow2(lmMsq) - 1404*pow2(lmMst1) + 36*lmMst2*(-291 - 464*lmMst1 +
        90*lmMsq*(-1 + 2*lmMst1) + 90*pow2(lmMsq) + 32*pow2(lmMst1)) - 36*(-607
        + 180*lmMsq + 336*lmMst1)*pow2(lmMst2) + (120*(69 + (-53 + 42*lmMsq)*
        lmMst2 + lmMst1*(53 - 42*lmMsq + 42*lmMst2) - 42*pow2(lmMst2))*pow2(
        Mst1))/pow2(Msq) + 10944*pow3(lmMst2) + (15*(216 + (-95 + 132*lmMsq)*
        lmMst2 + lmMst1*(95 - 132*lmMsq + 132*lmMst2) - 132*pow2(lmMst2))*pow4(
        Mst1))/pow4(Msq))/(162.*Mst2)) + ((5*(-43 + 30*lmMsq - 30*lmMst2))/(27.
        *pow2(Msq)*pow2(Mst1)) - (-41220947 + 420*lmMsq*(12479 + 69090*lmMst1 -
        69930*lmMst2) + 21234990*lmMst2 - 10290*lmMst1*(2573 + 2820*lmMst2) +
        176400*pow2(lmMsq) + 29194200*pow2(lmMst2))/(1.11132e7*pow4(Msq)) - (
        16*pow2(1 + lmMst2))/(9.*pow4(Mst1)))*pow4(Mst2) + pow4(Mst1)*((
        1.0702990137854083 - (4519*lmMst2)/980. + lmMsq*(0.36171579743008314 -
        (169*lmMst1)/63. + (19*lmMst2)/7.) + lmMst1*(4.249508692365835 + (31*
        lmMst2)/9.) - pow2(lmMsq)/63. - (8*pow2(lmMst1))/21. - (194*pow2(
        lmMst2))/63.)/pow4(Msq) - (363.3804294212688 + (76*B4)/9. - (2*DN)/9. -
        (35*lmMsq)/2. + lmMst1*(211.3489518770471 - (695*lmMsq)/9. + (40*pow2(
        lmMsq))/3.) - (214.87936507936507 - 20*lmMsq)*pow2(lmMst1) - lmMst2*(
        190.46006298815823 - (71398*lmMst1)/105. + (5*lmMsq*(-139 + 120*lmMst1)
        )/9. + (40*pow2(lmMsq))/3. + (334*pow2(lmMst1))/3.) + ((-146507 +
        14700*lmMsq + 91070*lmMst1)*pow2(lmMst2))/315. - (44*pow3(lmMst1))/9. -
        (1556*pow3(lmMst2))/9.)/pow4(Mst2) + (Dmglst2*(536.1152102791342 - (8*
        B4)/3. + (32*D3)/9. - (20*DN)/9. + 90*lmMsq - (123.11224321827497 + 20*
        lmMsq*(1 + lmMsq))*lmMst1 - lmMst2*(17.33220122616948 - 20*lmMsq*(1 +
        lmMsq) + (133.04550264550264 - 40*lmMsq)*lmMst1 - (1180*pow2(lmMst1))/
        9.) - (15886*pow2(lmMst1))/945. + (149.85608465608465 - 40*lmMsq - (
        2812*lmMst1)/9.)*pow2(lmMst2) - (92*pow3(lmMst1))/27. + (4988*pow3(
        lmMst2))/27.))/pow5(Mst2)) - (108*Dmglst2*(344*OepS2 + 9*(15643 - 774*
        lmMst1 + 774*lmMst2)*S2)*pow2(Mst1)*pow2(Mst2) - 9*(3896*OepS2 - 81*(
        9473 + 974*lmMst1 - 974*lmMst2)*S2)*pow2(Mst1)*pow3(Mst2) + 3*Mst2*(-
        29428*OepS2 + 27*(160997 + 22071*lmMst1 - 22071*lmMst2)*S2)*pow4(Mst1)
        + 8*Dmglst2*(17308*OepS2 + 27*(93919 - 12981*lmMst1 + 12981*lmMst2)*S2)
        *pow4(Mst1) + 54*Dmglst2*(56*OepS2 - 81*(-1677 + 14*lmMst1 - 14*lmMst2)
        *S2)*pow4(Mst2) + 1296*(-4*OepS2 + 81*(22 + lmMst1 - lmMst2)*S2)*pow5(
        Mst2))/(2187.*pow5(Mst2)) + ((-1 + 2*lmMst2)*shiftst3*(2*(1 - 2*lmMst1
        + 2*lmMst2)*pow2(Mst2)*pow4(Mst1) + 2*(1 - lmMst1 + lmMst2)*pow2(Mst1)*
        pow4(Mst2) + (2 - 6*lmMst1 + 6*lmMst2)*pow6(Mst1) + pow6(Mst2)))/(3.*
        pow2(Mst1)*pow4(Mst2)))) + 4*Al4p*xDmglst2*pow2(Dmglst2)*(Al4p*
        threeLoopFlag*((144*pow2(Mt)*(33934 - 90*B4 + 90*D3 - 45*DN + 120*(163
        + 24*lmMst1)*lmMst2 - 120*lmMt - 10206*S2 - 720*(2 + lmMst1)*pow2(
        lmMst2) + 720*(lmMst1 + pow3(lmMst2))))/pow4(Mst2) + 7290*pow2(s2t)*((
        5*(448 - 273*lmMst2 + 18*lmMsq*(8 - 19*lmMst1 + 19*lmMst2) + 3*lmMst1*(
        43 + 114*lmMst2) - 342*pow2(lmMst2)))/(27.*pow2(Msq)) + (
        92.43189300411522 - (76*B4)/9. + (80*D3)/9. - (14*DN)/3. + (545*lmMsq)/
        9. + 5*pow2(lmMsq) - (2*lmMst1*(-502 + 210*lmMsq + 45*pow2(lmMsq)))/9.
         - (25*pow2(lmMst1))/
        9. + (lmMst2*(-6175 + 1308*lmMst1 + 90*lmMsq*(11 + 6*lmMst1) + 270*
        pow2(lmMsq) + 96*pow2(lmMst1)))/27. - ((371 + 180*lmMsq + 336*lmMst1)*
        pow2(lmMst2))/9. + ((98.88888888888889 + (30 - 60*lmMsq)*lmMst1 - 30*
        lmMst2 + 60*(lmMsq + lmMst1)*lmMst2 - 60*pow2(lmMst2))*pow2(Mst1))/
        pow2(Msq) + (304*pow3(lmMst2))/9.)/pow2(Mst2) + pow2(Mst2)*((-5*(-3125
        + 540*lmMsq*(-1 + 4*lmMst1 - 4*lmMst2) + 876*lmMst2 - 48*lmMst1*(7 +
        45*lmMst2) + 2160*pow2(lmMst2)))/(216.*pow4(Msq)) + (32*(2 + lmMst2 -
        3*pow2(lmMst2)))/(9.*pow4(Mst1))) + pow2(Mst1)*((5*(493 + 5*(-17 + 60*
        lmMsq)*lmMst2 + lmMst1*(85 - 300*lmMsq + 300*lmMst2) - 300*pow2(lmMst2)
        ))/(36.*pow4(Msq)) - (182.90272153635118 + (140*B4)/9. - 16*D3 + (74*
        DN)/9. - (20*lmMsq)/3. + lmMst1*(-174.74765432098766 + 40*lmMsq + 10*
        pow2(lmMsq)) + lmMst2*(252.96987654320986 - (6002*lmMst1)/45. - 20*
        lmMsq*(2 + lmMst1) - 10*pow2(lmMsq) - (106*pow2(lmMst1))/3.) + (1556*
        pow2(lmMst1))/45. + (98.8 + 20*lmMsq + (878*lmMst1)/9.)*pow2(lmMst2) +
        (10*pow3(lmMst1))/9. - (190*pow3(lmMst2))/3.)/pow4(Mst2)) + ((47120*
        OepS2 - 54*(620417 + 17670*lmMst1 - 17670*lmMst2)*S2)*pow2(Mst1) + 3*(
        280*OepS2 - 81*(65819 + 70*lmMst1 - 70*lmMst2)*S2)*pow2(Mst2))/(3645.*
        pow4(Mst2)) + (160*(-4 + 15*lmMsq - 15*lmMst2)*pow2(Msq)*pow2(Mst2) +
        4*(-919 + 64*lmMst1*(-2 + lmMst2) - 466*lmMst2 - 30*lmMsq*(-13 + 6*
        lmMst2) + 90*pow2(lmMsq) + 592*pow2(lmMst2))*pow4(Msq) + 15*(-9 + 140*
        lmMsq - 140*lmMst2)*pow4(Mst2))/(72.*pow2(Mst1)*pow4(Msq))) + (s2t*(
        120*T1ep*(2*(3073*Mt - 589*Mst2*s2t)*pow2(Mst1) - 21*(-16*Mt + Mst2*
        s2t)*pow2(Mst2)) - (Mt*(pow4(Msq)*(12*pow2(Mst1)*pow2(Mst2)*(238288 -
        61560*B4 + 1620*DN + 288900*lmMsq + 1620*(-31 + 10*lmMsq)*lmMst1 - 180*
        (1817 + 90*lmMsq + 528*lmMst1)*lmMst2 + 2240*OepS2 - 81*(-1373 + 560*
        lmMst1 - 560*lmMst2)*S2 - 4320*pow2(lmMst1) + 4320*(11 + 12*lmMst1)*
        pow2(lmMst2) - 51840*pow3(lmMst2)) - 5*(769589 + 295488*B4 - 7776*DN -
        1432080*lmMsq - 144*(2161 + 810*lmMsq)*lmMst1 + 144*(17377 + 810*lmMsq
        + 4428*lmMst1)*lmMst2 - 98336*OepS2 + 756*(259 + 2634*lmMst1 - 2634*
        lmMst2)*S2 - 49248*pow2(lmMst1) - 2592*(179 + 96*lmMst1)*pow2(lmMst2) +
        248832*pow3(lmMst2))*pow4(Mst1) + 103680*(3 + 4*lmMst2 - 3*pow2(lmMst2)
        )*pow4(Mst2)) + 10800*pow2(Msq)*((983 + 36*lmMst1 + 36*lmMsq*(-2 + 3*
        lmMst1 - 3*lmMst2) + 36*lmMst2 - 108*lmMst1*lmMst2 + 108*pow2(lmMst2))*
        pow2(Mst2)*pow4(Mst1) + (509 + 12*lmMst1 + 36*lmMsq*(-2 + lmMst1 -
        lmMst2) + 60*lmMst2 - 36*lmMst1*lmMst2 + 36*pow2(lmMst2))*pow2(Mst1)*
        pow4(Mst2)) + 2700*((2375 + 81*lmMst1 + 180*lmMsq*(-2 + 3*lmMst1 - 3*
        lmMst2) + 279*lmMst2 - 540*lmMst1*lmMst2 + 540*pow2(lmMst2))*pow4(Mst1)
        *pow4(Mst2) + (1307 + 27*lmMst1 + 180*lmMsq*(-2 + lmMst1 - lmMst2) +
        333*lmMst2 - 180*lmMst1*lmMst2 + 180*pow2(lmMst2))*pow2(Mst1)*pow6(
        Mst2))))/(pow2(Mst1)*pow4(Msq))))/pow5(Mst2)) + (2430*s2t*twoLoopFlag*(
        -2*(-8*(10 + lmMst1 - lmMst2)*Mt + Mst2*s2t*(8 - 9*lmMst1 + 9*lmMst2 +
        2*lmMst1*lmMst2 - 2*pow2(lmMst2)))*pow2(Mst2)*pow4(Mst1) - 2*(-4*(8 +
        lmMst1 - lmMst2)*Mt + Mst2*s2t*(8 - 5*lmMst1 + 4*lmMst2 + 2*lmMst1*
        lmMst2 - 2*pow2(lmMst2)))*pow2(Mst1)*pow4(Mst2) + (268*Mt + Mst2*s2t*(-
        17 - 4*lmMst1*(-7 + lmMst2) - 28*lmMst2 + 4*pow2(lmMst2)))*pow6(Mst1) +
        2*(-2 + lmMst2)*s2t*pow7(Mst2)))/(pow2(Mst1)*pow7(Mst2))) + (135*Al4p*
        s2t*(36*twoLoopFlag*pow2(Mst1)*(4*(4*Mt*(5 + 6*lmMst2 - lmMst1*(4 + 3*
        lmMst2) + 3*pow2(lmMst2)) + Mst2*s2t*(-1 + 13*lmMst2 - lmMst1*(13 + 8*
        lmMst2) + pow2(lmMst1) + 7*pow2(lmMst2)))*pow3(Mst2)*pow4(Mst1) + 2*(-(
        Mst2*s2t*(-14 - 20*lmMst2 + 2*lmMst1*(5 + 3*lmMst2) + pow2(lmMst1) - 7*
        pow2(lmMst2))) + 8*Mt*(4 + 3*lmMst2 - lmMst1*(1 + lmMst2) + pow2(
        lmMst2)))*pow2(Mst1)*pow5(Mst2) + 4*Dmglst2*(Mst2*s2t*(-5 + 8*lmMst2 -
        4*lmMst1*(2 + lmMst2) + 4*pow2(lmMst2)) + Mt*(65 + lmMst1*(34 - 20*
        lmMst2) - 26*lmMst2 + 20*pow2(lmMst2)))*pow6(Mst1) + Mst2*(Mst2*s2t*(-1
        + 50*lmMst2 - 2*lmMst1*(25 + 32*lmMst2) + 20*pow2(lmMst1) + 44*pow2(
        lmMst2)) + Mt*(84 + 152*lmMst2 - 40*lmMst1*(3 + 2*lmMst2) + 80*pow2(
        lmMst2)))*pow6(Mst1) + 8*Dmglst2*((Mst2*s2t*(-2 + 3*lmMst2 - lmMst1*(3
        + 2*lmMst2) + 2*pow2(lmMst2)) + Mt*(22 + lmMst1*(8 - 6*lmMst2) - 4*
        lmMst2 + 6*pow2(lmMst2)))*pow2(Mst2)*pow4(Mst1) + (-(Mst2*s2t*(1 +
        lmMst1 - 2*lmMst2 + 2*lmMst1*lmMst2 - 2*pow2(lmMst2))) + 2*Mt*(6 +
        lmMst1 + lmMst2 - lmMst1*lmMst2 + pow2(lmMst2)))*pow2(Mst1)*pow4(Mst2)
        + lmMst2*s2t*pow7(Mst2)) + 4*(1 + lmMst2)*s2t*pow8(Mst2)) + Mst2*
        xDR2DRMOD*(-144*(2*Dmglst2*lmMst2 + Mst2 + lmMst2*Mst2)*s2t*
        twoLoopFlag*pow2(Mst1)*((pow2(Mst1) + pow2(Mst2))*pow4(Mst2) - 2*(
        lmMst1 - lmMst2)*pow2(Mst1)*(pow2(Mst1)*pow2(Mst2) + pow4(Mst1) + pow4(
        Mst2))) + Al4p*threeLoopFlag*(1536*Mt*pow2(Mst1)*(2*(pow2(Mst1)*((1 +
        lmMst2)*Mst2*(-1 - 5*lmMst2 + lmMst1*(3 + 2*lmMst2) - 2*pow2(lmMst2)) +
        Dmglst2*(8 + 7*lmMst2 - 11*pow2(lmMst2) + lmMst1*(-3 + 5*lmMst2 + 6*
        pow2(lmMst2)) - 6*pow3(lmMst2)))*pow3(Mst2) + Mst2*((1 + lmMst2)*Mst2*(
        1 - 10*lmMst2 + 4*lmMst1*(2 + lmMst2) - 4*pow2(lmMst2)) + 2*Dmglst2*(8
        + 13*lmMst2 - 8*pow2(lmMst2) + lmMst1*(-5 + 5*lmMst2 + 6*pow2(lmMst2))
        - 6*pow3(lmMst2)))*pow4(Mst1)) + (1 + lmMst2)*(-2*(-Dmglst2 + 3*
        Dmglst2*lmMst2 + Mst2 + lmMst2*Mst2)*pow5(Mst2) + (7 - 32*lmMst2 + 4*
        lmMst1*(7 + 3*lmMst2) - 12*pow2(lmMst2))*pow6(Mst1))) - (xDmglst2*pow2(
        Dmglst2)*(s2t*pow2(Mst1)*(2*(lmMst1 - lmMst2)*pow2(Mst1) - pow2(Mst2))*
        (pow2(Mst1) + pow2(Mst2))*(480*(4 - 15*lmMsq + 15*lmMst2)*pow2(Msq)*
        pow3(Mst2) + 45*(9 - 140*lmMsq + 140*lmMst2)*pow5(Mst2)) + 4*pow4(Msq)*
        (-(pow2(Mst2)*(Mst2*s2t*(3305 + 6632*lmMst2 - 90*lmMsq*(13 + 3*lmMsq -
        6*lmMst2)*(1 - 2*lmMst1 + 2*lmMst2) - 192*(-2 + lmMst2)*pow2(lmMst1) +
        1788*pow2(lmMst2) + 6*lmMst1*(-995 - 402*lmMst2 + 336*pow2(lmMst2)) -
        1824*pow3(lmMst2)) + 128*Mt*(5 + 149*lmMst2 + 12*pow2(lmMst2) + 6*
        lmMst1*(-7 - 8*lmMst2 + 6*pow2(lmMst2)) - 36*pow3(lmMst2)))*pow4(Mst1))
        - 3*(Mst2*s2t*(835 - 64*lmMst1*(-2 + lmMst2) + 466*lmMst2 + 30*lmMsq*(-
        13 + 6*lmMst2) - 90*pow2(lmMsq) - 592*pow2(lmMst2)) + 256*Mt*(3 + 4*
        lmMst2 - 3*pow2(lmMst2)))*pow2(Mst1)*pow4(Mst2) + 2*(-3*Mst2*s2t*(672 +
        lmMst2*(1507 - 390*lmMsq - 90*pow2(lmMsq)) - 288*(-2 + lmMst2)*pow2(
        lmMst1) + 18*(49 + 10*lmMsq)*pow2(lmMst2) + lmMst1*(-1411 - 1458*lmMst2
        - 30*lmMsq*(-13 + 6*lmMst2) + 90*pow2(lmMsq) + 848*pow2(lmMst2)) - 560*
        pow3(lmMst2)) + 64*Mt*(85 - 215*lmMst2 + lmMst1*(66 + 90*lmMst2 - 72*
        pow2(lmMst2)) - 54*pow2(lmMst2) + 72*pow3(lmMst2)))*pow6(Mst1) + 192*
        s2t*(2 + lmMst2 - 3*pow2(lmMst2))*pow7(Mst2))) + s2t*(pow2(Mst1)*((
        pow2(Mst1) + pow2(Mst2))*pow4(Mst2) - 2*(lmMst1 - lmMst2)*pow2(Mst1)*(
        pow2(Mst1)*pow2(Mst2) + pow4(Mst1) + pow4(Mst2)))*(40*(-43 + 30*lmMsq -
        30*lmMst2)*pow2(Msq)*pow3(Mst2) + 5*(-67 + 84*lmMsq - 84*lmMst2)*pow5(
        Mst2)) + 12*Mst2*pow4(Msq)*((303 + 1452*lmMst2 + 30*lmMsq*(-1 + 2*
        lmMst1 - 2*lmMst2)*(5 - 3*lmMsq + 6*lmMst2) + 64*(1 + lmMst2)*pow2(
        lmMst1) + 1804*pow2(lmMst2) - 2*lmMst1*(431 + 814*lmMst2 + 336*pow2(
        lmMst2)) + 608*pow3(lmMst2))*pow4(Mst1)*pow4(Mst2) + (335 + 654*lmMst2
        + 64*lmMst1*(1 + lmMst2) - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) +
        272*pow2(lmMst2))*pow2(Mst1)*pow6(Mst2) + 2*(pow2(Mst2)*(64 + 15*
        lmMst2*(33 - 10*lmMsq + 6*pow2(lmMsq)) + 288*(1 + lmMst2)*pow2(lmMst1)
        + 6*(173 - 30*lmMsq)*pow2(lmMst2) - lmMst1*(431 + 1326*lmMst2 - 30*
        lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) + 848*pow2(lmMst2)) + 560*pow3(
        lmMst2))*pow6(Mst1) + (80 + lmMst2*(479 - 150*lmMsq + 90*pow2(lmMsq)) +
        544*(1 + lmMst2)*pow2(lmMst1) + 2*(631 - 90*lmMsq)*pow2(lmMst2) -
        lmMst1*(399 + 1806*lmMst2 - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) +
        1360*pow2(lmMst2)) + 816*pow3(lmMst2))*pow8(Mst1)) - 32*pow2(1 +
        lmMst2)*pow8(Mst2)) + 2*Dmglst2*(pow2(Mst1)*(160*(-14 + 15*lmMsq - 15*
        lmMst2)*pow2(Msq)*pow2(Mst2) + 45*(-13 + 28*lmMsq - 28*lmMst2)*pow4(
        Mst2))*((pow2(Mst1) + pow2(Mst2))*pow4(Mst2) - 2*(lmMst1 - lmMst2)*
        pow2(Mst1)*(pow2(Mst1)*pow2(Mst2) + pow4(Mst1) + pow4(Mst2))) + 12*
        pow4(Msq)*((45 + 30*lmMsq*(1 + 3*lmMsq - 6*lmMst2)*(1 - 2*lmMst1 + 2*
        lmMst2) + 8*lmMst2*(29 + 8*pow2(lmMst1)) + 1068*pow2(lmMst2) - 2*
        lmMst1*(-83 + 430*lmMst2 + 336*pow2(lmMst2)) + 608*pow3(lmMst2))*pow4(
        Mst1)*pow4(Mst2) + (-115 + (366 + 64*lmMst1)*lmMst2 - 30*lmMsq*(-1 + 6*
        lmMst2) + 90*pow2(lmMsq) + 336*pow2(lmMst2))*pow2(Mst1)*pow6(Mst2) + 2*
        (pow2(Mst2)*(96 + lmMst2*(173 + 30*lmMsq + 90*pow2(lmMsq) + 288*pow2(
        lmMst1)) + (526 - 180*lmMsq)*pow2(lmMst2) - lmMst1*(13 + 30*lmMsq*(1 -
        6*lmMst2) + 526*lmMst2 + 90*pow2(lmMsq) + 848*pow2(lmMst2)) + 560*pow3(
        lmMst2))*pow6(Mst1) + (96 + lmMst2*(285 + 30*lmMsq + 90*pow2(lmMsq) +
        544*pow2(lmMst1)) + (590 - 180*lmMsq)*pow2(lmMst2) - lmMst1*(109 + 30*
        lmMsq*(1 - 6*lmMst2) + 590*lmMst2 + 90*pow2(lmMsq) + 1360*pow2(lmMst2))
        + 816*pow3(lmMst2))*pow8(Mst1)) - 64*lmMst2*(1 + lmMst2)*pow8(Mst2)))))
        /pow4(Msq)))))/(pow4(Mst1)*pow6(Mst2)) + (2*Al4p*z2*(6*xDmglst2*pow2(
        Dmglst2)*pow2(Mst2)*(3240*s2t*twoLoopFlag*((-24*Mt + Mst2*s2t)*pow2(
        Mst1)*pow2(Mst2) + (-36*Mt + Mst2*s2t)*pow4(Mst1) + (-12*Mt + Mst2*s2t)
        *pow4(Mst2)) + Al4p*threeLoopFlag*pow2(Mst2)*(-3*Mst2*(8*(63187 -
        32400*lmMsq - 810*lmMst1 + 52650*lmMst2)*Mst2*Mt*s2t + 192*(17 + 1320*
        lmMst2)*pow2(Mt) + (8599 + 16200*lmMsq + 12240*lmMst1 - 43920*lmMst2)*
        pow2(Mst2)*pow2(s2t)) + s2t*(-2*(5*(9883 - 155520*lmMsq - 41688*lmMst1
        + 290520*lmMst2)*Mt + (249097 + 24300*lmMsq + 30510*lmMst1 - 78030*
        lmMst2)*Mst2*s2t)*pow2(Mst1) + (8100*pow2(Mst2)*(2*(136*Mt - 9*Mst2*
        s2t)*pow2(Mst1) + (144*Mt - 19*Mst2*s2t)*pow2(Mst2)))/pow2(Msq) + (
        4050*((328*Mt - 25*Mst2*s2t)*pow2(Mst1) + 2*(92*Mt - 15*Mst2*s2t)*pow2(
        Mst2))*pow4(Mst2))/pow4(Msq)) + (28080*pow2(s2t)*pow5(Mst2))/pow2(Mst1)
        )) - 9720*s2t*twoLoopFlag*(4*Mt*pow2(Mst1)*(pow2(Mst1) + pow2(Mst2))*
        pow4(Mst2) + xMst*(2*(48*Mt - Mst2*s2t)*xDmglst2*pow2(Dmglst2) + 4*Mt*
        pow2(Mst2))*pow6(Mst1) - 4*Dmglst2*((-13*Mt + Mst2*s2t)*pow3(Mst2)*
        pow4(Mst1) + (-9*Mt + Mst2*s2t)*pow2(Mst1)*pow5(Mst2) + Mst2*(-17*Mt +
        Mst2*s2t)*xMst*pow6(Mst1) + (-5*Mt + Mst2*s2t)*pow7(Mst2)) + (4*Mt -
        Mst2*s2t)*pow8(Mst2)) - Al4p*threeLoopFlag*pow4(Mst2)*(1080*pow2(Mt)*((
        Dmglst2*(7286 - 240*lmMst1 + 6384*lmMst2) + (-353 + 72*lmMst1 + 696*
        lmMst2)*Mst2)*pow2(Mst1) + 384*Dmglst2*(5 + 6*lmMst2)*pow2(Mst2) + 16*(
        53 + 24*lmMst2)*pow3(Mst2)) + s2t*((6*Mt*(pow4(Msq)*(12*Mst2*(Dmglst2*(
        534391 - 113400*lmMsq - 23760*lmMst1 + 196560*lmMst2) + 10*(5827 -
        2700*lmMsq + 2988*lmMst1 + 468*lmMst2)*Mst2)*pow2(Mst1) + 9*(Dmglst2*(
        278347 - 79200*lmMsq + 3720*lmMst1 + 119160*lmMst2) + 15*(3469 - 1440*
        lmMsq + 664*lmMst1 + 1384*lmMst2)*Mst2)*pow3(Mst2) + (825326 - 453600*
        lmMsq + 792720*lmMst1 - 239760*lmMst2)*pow4(Mst1)) - 43200*Mst2*pow2(
        Msq)*(3*pow2(Mst1)*pow3(Mst2) + 4*Mst2*pow4(Mst1) + 3*Dmglst2*(7*pow2(
        Mst1)*pow2(Mst2) + 10*pow4(Mst1) + 4*pow4(Mst2)) + 2*pow5(Mst2)) -
        5400*pow3(Mst2)*(7*pow2(Mst1)*pow3(Mst2) + 9*Mst2*pow4(Mst1) + Dmglst2*
        (67*pow2(Mst1)*pow2(Mst2) + 93*pow4(Mst1) + 41*pow4(Mst2)) + 5*pow5(
        Mst2))))/pow4(Msq) + (12960*xDR2DRMOD*(pow2(Mst1)*pow2(Mst2)*(Dmglst2*
        Mst2*(32*(1 + 3*lmMst2)*Mt + (23 - 46*lmMst1 + 30*lmMst2)*Mst2*s2t) +
        2*(16*(-2 + 3*lmMst2)*Mt + (13 - 10*lmMst1 + 6*lmMst2)*Mst2*s2t)*
        xDmglst2*pow2(Dmglst2) + (32*(1 + lmMst2)*Mt + (5 - 26*lmMst1 + 18*
        lmMst2)*Mst2*s2t)*pow2(Mst2)) + 2*(Dmglst2*Mst2*(32*(1 + 3*lmMst2)*Mt +
        (-23*lmMst1 + 15*lmMst2)*Mst2*s2t) + 2*(16*(-2 + 3*lmMst2)*Mt + (4 - 5*
        lmMst1 + 3*lmMst2)*Mst2*s2t)*xDmglst2*pow2(Dmglst2) + (32*(1 + lmMst2)*
        Mt + (-4 - 13*lmMst1 + 9*lmMst2)*Mst2*s2t)*pow2(Mst2))*pow4(Mst1) +
        s2t*(23*Dmglst2*Mst2 + 10*xDmglst2*pow2(Dmglst2) + 13*pow2(Mst2))*pow5(
        Mst2) + 2*(48*(1 + lmMst2)*Mt + Dmglst2*(-23*lmMst1 + 15*lmMst2)*s2t +
        (-4 - 13*lmMst1 + 9*lmMst2)*Mst2*s2t)*pow6(Mst1)))/pow2(Mst1)) + (pow2(
        s2t)*(3*Mst2*(32400*xMsq*(pow2(Mst1)*pow2(Mst2) - 2*lmMst2*(-1 +
        shiftst1)*pow2(Mst1)*pow2(Mst2) - 2*shiftst1*pow2(Mst1)*pow2(Mst2) +
        shiftst2*pow2(Mst1)*pow2(Mst2) - 2*shiftst1*pow4(Mst1) + 2*shiftst2*
        pow4(Mst1) + 2*lmMst2*(1 - 2*shiftst1 + shiftst2)*pow4(Mst1) + 2*
        lmMst1*((-1 + shiftst1)*pow2(Mst1)*pow2(Mst2) + (-1 + 2*shiftst1 -
        shiftst2)*pow4(Mst1)) + pow4(Mst2) - shiftst1*pow4(Mst2))*pow6(Msq) +
        pow4(Msq)*(6*(240379 + 10800*lmMsq - 21420*lmMst2 - 1080*shiftst3 -
        2160*lmMst2*shiftst3 + 180*lmMst1*(55 + 12*shiftst3))*pow2(Mst2)*pow4(
        Mst1) + 180*(-59 + 450*lmMsq - 1362*lmMst2 - 36*shiftst3 - 36*lmMst2*
        shiftst3 + 6*lmMst1*(107 + 6*shiftst3))*pow2(Mst1)*pow4(Mst2) + (
        3647353 + 64800*lmMsq - 52380*lmMst2 - 6480*shiftst3 - 19440*lmMst2*
        shiftst3 + 540*lmMst1*(-31 + 36*shiftst3))*pow6(Mst1) - 1080*(49 + 3*
        shiftst3)*pow6(Mst2)) + 5400*pow2(Msq)*(4*pow4(Mst1)*pow4(Mst2) + 4*
        pow2(Mst2)*pow6(Mst1) + 7*pow2(Mst1)*pow6(Mst2)) + 1350*(4*pow4(Mst2)*
        pow6(Mst1) + 4*pow4(Mst1)*pow6(Mst2) + 9*pow2(Mst1)*pow8(Mst2))) + 4*
        Dmglst2*(pow4(Msq)*(162*(-9053 + 900*lmMsq + 1810*lmMst1 - 3570*lmMst2)
        *pow2(Mst2)*pow4(Mst1) + 135*(-4151 + 1080*lmMsq + 1368*lmMst1 - 3480*
        lmMst2)*pow2(Mst1)*pow4(Mst2) + 2*(-2286439 + 72900*lmMsq + 307800*
        lmMst1 - 450360*lmMst2)*pow6(Mst1) - 74520*pow6(Mst2)) + 8100*pow2(Msq)
        *(14*pow4(Mst1)*pow4(Mst2) + 14*pow2(Mst2)*pow6(Mst1) + 17*pow2(Mst1)*
        pow6(Mst2)) + 4050*(11*pow4(Mst2)*pow6(Mst1) + 11*pow4(Mst1)*pow6(Mst2)
        + 16*pow2(Mst1)*pow8(Mst2)))))/(pow2(Mst1)*pow4(Msq)))))/pow9(Mst2))) +
        (270*s2t*xMst*(27*(lmMst1 - lmMst2)*oneLoopFlag*s2t*pow2(MuSUSY)*pow3(
        Mst2) + Al4p*twoLoopFlag*(-8*Mst2*(-18*(-2 + lmMst2)*(-lmMst1 + lmMst2)
        *s2t*xDmglst2*xDR2DRMOD*pow2(Dmglst2) + Dmglst2*Mt*(785 + 6*lmMst1*(85
        - 42*lmMst2) - 438*lmMst2 + 252*pow2(lmMst2)) + Mst2*Mt*(193 + 474*
        lmMst2 - 6*lmMst1*(67 + 42*lmMst2) + 252*pow2(lmMst2)) - Dmglst2*Mst2*
        s2t*(49 - 84*lmMst2 + lmMst1*(84 - 36*lmMst2*(-1 + xDR2DRMOD)) + 36*(-1
        + xDR2DRMOD)*pow2(lmMst2)) - s2t*(1 + 3*lmMst2*(-37 + 6*xDR2DRMOD) - 3*
        lmMst1*(-37 + 6*lmMst2*(-12 + xDR2DRMOD) + 6*xDR2DRMOD) - 81*pow2(
        lmMst1) + 9*(-15 + 2*xDR2DRMOD)*pow2(lmMst2))*pow2(Mst2))*pow2(MuSUSY)
        + (2*xDmglst2*pow2(Dmglst2)*(48*(-143 + 18*lmMst1 - 18*lmMst2)*Mt*
        Tbeta*pow2(MuSUSY) + 2*Mst2*MuSUSY*(MuSUSY*s2t*Tbeta*(157 - 348*lmMst1
        + 348*lmMst2 + 36*lmMst1*lmMst2 - 36*pow2(lmMst2)) + 30*(-43 + 60*
        lmMst1 - 60*lmMst2)*Mt*pow2(Sbeta)) + 15*(-43 + 60*lmMst1 - 60*lmMst2)*
        s2t*Tbeta*(-1 + pow2(Sbeta))*pow2(Sbeta)*pow3(Mst2)))/Tbeta))*pow6(
        Mst1))/pow9(Mst2)))/29160.;
}

/**
 * 	@return The diagonal (2, 2) matrix element of the Higgs mass matrix as a double for the hierarchy 'H6'
 */
double himalaya::H6::getS2() const {
   return -(oneLoopFlag*((4*Mt*MuSUSY*s2t*(4*(lmMst1 - lmMst2)*pow2(Mt) + ((-2 -
        lmMst1 + lmMst2)*pow2(Mst1) + (2 - lmMst1 + lmMst2)*pow2(Mst2))*pow2(
        s2t)))/Tbeta + 4*pow2(Mt)*pow2(s2t)*(2*(lmMst1 - lmMst2)*(pow2(Mst1) -
        pow2(Mst2)) + pow2(MuSUSY)*(2 - lmMst1 + lmMst2 - (2*(lmMst1 - lmMst2)*
        pow2(Mst1)*(pow2(Mst1) + pow2(Mst2)))/pow4(Mst2))) - (4*pow2(Mt)*pow2(
        MuSUSY)*pow2(s2t)*(2 - lmMst1 + lmMst2 - (2*(lmMst1 - lmMst2)*pow2(
        Mst1)*(pow2(Mst1) + pow2(Mst2)))/pow4(Mst2)))/pow2(Sbeta) + 16*(lmMst1
        + lmMst2 - 2*lmMt)*pow4(Mt) - (-4*pow2(Mst1)*pow2(Mst2) + (2 + lmMst1 -
        lmMst2)*pow4(Mst1) + (2 - lmMst1 + lmMst2)*pow4(Mst2))*pow4(s2t)))/32.
         + (threeLoopFlag*pow2(Al4p)*(-(pow4(Mst1)*(4*pow2(Mt)*pow2(s2t)*(-6*
        xDmglst2*xDR2DRMOD*pow2(Dmglst2)*pow2(Sbeta)*(MuSUSY*(408118*z3 -
        83590*z4 - 51027*pow2(z2)) + 2*Mst2*Tbeta*(105742*z3 + 4838*z4 + 7257*
        pow2(z2))) + 3*Mst2*(Tbeta*pow2(MuSUSY)*(1 - pow2(Sbeta))*(-8*(76813 +
        162*lmMst1 - 162*lmMst2)*z3 + 15686*z4 + 18183*pow2(z2)) - Mst2*pow2(
        Sbeta)*(6*MuSUSY*(1144*z3 - 4954*z4 + 6177*pow2(z2)) + Mst2*Tbeta*(-
        66166*z3 + 13834*z4 + 20751*pow2(z2)))) + 2*Dmglst2*(4*Tbeta*pow2(
        MuSUSY)*(-1 + pow2(Sbeta))*(338536*z3 + 11327*z4 + 15897*pow2(z2)) +
        pow2(Sbeta)*(36*Tbeta*pow2(Mst2)*(-734*z3 + 1538*z4 + 2307*pow2(z2)) -
        3*Mst2*MuSUSY*(-844756*z3 + 10126*z4 + 105585*pow2(z2))))) + 16*s2t*(
        16*Tbeta*xDmglst2*xDR2DRMOD*pow2(Dmglst2)*pow2(Sbeta)*(-360619*z3 +
        20332*z4 + 30498*pow2(z2)) + 3*(4*Tbeta*pow2(Mst2)*pow2(Sbeta)*(-57758*
        z3 + 1370*z4 + 2055*pow2(z2)) + Mst2*MuSUSY*pow2(Sbeta)*(-22466*z3 +
        11402*z4 + 17103*pow2(z2)) - Tbeta*pow2(MuSUSY)*(-1 + pow2(Sbeta))*(-
        67232*z3 - 11764*z4 + 20505*pow2(z2))) - 4*Dmglst2*pow2(Sbeta)*(9*
        MuSUSY*(2806*z3 + 1514*z4 + 2757*pow2(z2)) + Mst2*Tbeta*(-614702*z3 +
        25730*z4 + 38595*pow2(z2))))*pow3(Mt) - 4*Mst2*Mt*pow2(Sbeta)*(3*pow2(
        Mst2)*(Mst2*Tbeta*(-44630*z3 + 878*z4 + 1317*pow2(z2)) + MuSUSY*(-
        272636*z3 + 8870*z4 + 13305*pow2(z2))) - Dmglst2*Mst2*(8*MuSUSY*(89947*
        z3 + 6332*z4 + 9498*pow2(z2)) + Mst2*Tbeta*(-565214*z3 + 31142*z4 +
        46713*pow2(z2))) + 4*xDmglst2*xDR2DRMOD*pow2(Dmglst2)*(MuSUSY*(-614777*
        z3 + 7184*z4 + 28272*pow2(z2)) + Mst2*Tbeta*(-394856*z3 + 31622*z4 +
        47433*pow2(z2))))*pow3(s2t) + pow2(Sbeta)*(-64*(MuSUSY*(-321216*z3 +
        9978*z4 + 9135*pow2(z2)) - 2*Tbeta*(6*Mst2*(24241*z3 + 146*z4 + 219*
        pow2(z2)) + Dmglst2*(-562226*z3 + 5582*z4 + 8373*pow2(z2))))*pow4(Mt) +
        Tbeta*(3*pow2(Mst2)*((-34070 + 648*lmMst1 - 648*lmMst2)*z3 + 2594*z4 +
        5835*pow2(z2)) - 2*Dmglst2*Mst2*(-197843*z3 + 16796*z4 + 25194*pow2(z2)
        ) + xDmglst2*xDR2DRMOD*pow2(Dmglst2)*(-699017*z3 + 61508*z4 + 92262*
        pow2(z2)))*pow3(Mst2)*pow4(s2t)))) + 6*pow2(Mst1)*(-(xDmglst2*pow2(
        Dmglst2)*(2*Mst2*pow2(Mt)*pow2(s2t)*(6*Mst2*MuSUSY*pow2(Sbeta)*(127198*
        z3 + 6782*z4 - 14613*pow2(z2)) - 4*Tbeta*pow2(MuSUSY)*(-1 + pow2(Sbeta)
        )*(-243722*z3 - 7489*z4 + 795*pow2(z2)) + 9*Tbeta*pow2(Mst2)*pow2(
        Sbeta)*(-15053*z3 + 792*z4 + 1188*pow2(z2))) + 8*s2t*(-(Tbeta*pow2(
        MuSUSY)*(-1 + pow2(Sbeta))*(-343276*z3 - 8090*z4 + 37437*pow2(z2))) +
        8*Mst2*pow2(Sbeta)*(-9*MuSUSY*(2534*z3 + 33*z4 + 90*pow2(z2)) + Mst2*
        Tbeta*(-32168*z3 + 2114*z4 + 3171*pow2(z2))))*pow3(Mt) - 2*Mt*pow2(
        Sbeta)*(MuSUSY*(-586697*z3 - 10924*z4 + 6942*pow2(z2)) + 2*Mst2*Tbeta*(
        -88880*z3 + 5474*z4 + 8211*pow2(z2)))*pow3(Mst2)*pow3(s2t) - 16*pow2(
        Sbeta)*(MuSUSY*(-186761*z3 + 8792*z4 + 13188*pow2(z2)) + Mst2*Tbeta*(-
        463990*z3 + 11338*z4 + 17007*pow2(z2)))*pow4(Mt) + Tbeta*pow2(Sbeta)*(-
        99253*z3 + 4054*z4 + 5352*pow2(z2))*pow4(s2t)*pow5(Mst2))) + 3*Mst2*(
        pow2(Mt)*(-4*pow2(Mst2)*(2*Tbeta*pow2(Mt)*pow2(Sbeta)*(69050*z3 + 418*
        z4 + 627*pow2(z2)) - Tbeta*pow2(MuSUSY)*pow2(s2t)*(-1 + pow2(Sbeta))*((
        -56978 - 216*lmMst1 + 216*lmMst2)*z3 + 1136*z4 + 813*pow2(z2)) + Mt*
        MuSUSY*s2t*pow2(Sbeta)*(7078*z3 + 2990*z4 + 4485*pow2(z2))) - 2*s2t*
        pow2(Sbeta)*(3*MuSUSY*s2t*(14114*z3 + 3010*z4 - 4557*pow2(z2)) + 4*Mt*
        Tbeta*(-48454*z3 + 754*z4 + 1131*pow2(z2)))*pow3(Mst2)) - 16*Mst2*
        MuSUSY*(-4*Mt*pow2(Sbeta)*(-8219*z3 + 326*z4 + 165*pow2(z2)) - MuSUSY*
        s2t*Tbeta*(-1 + pow2(Sbeta))*(-11396*z3 - 1135*z4 + 2388*pow2(z2)))*
        pow3(Mt) + 4*Mt*pow2(s2t)*pow2(Sbeta)*(4*Mt*Tbeta*((-940 + 54*lmMst1 -
        54*lmMst2)*z3 + 214*z4 + 483*pow2(z2)) + MuSUSY*s2t*((-39761 - 108*
        lmMst1 + 108*lmMst2)*z3 + 1046*z4 + 1245*pow2(z2)))*pow4(Mst2) + Tbeta*
        (-144*pow2(MuSUSY)*(-1 + pow2(Sbeta))*(218*z3 + 50*z4 + 21*pow2(z2))*
        pow4(Mt) + pow2(Sbeta)*(Mst2*s2t*(22544*z3 - 956*z4 - 1677*pow2(z2)) +
        4*Mt*(-8678*z3 + 740*z4 + 219*pow2(z2)))*pow3(s2t)*pow5(Mst2))) - 6*
        Dmglst2*(pow2(Mst2)*pow2(Mt)*(16*Tbeta*pow2(Mt)*pow2(Sbeta)*(-24526*z3
        + 322*z4 + 483*pow2(z2)) + 3*MuSUSY*pow2(s2t)*(Mst2*pow2(Sbeta)*(
        124922*z3 + 1210*z4 - 18273*pow2(z2)) + 8*MuSUSY*Tbeta*(-1 + pow2(
        Sbeta))*(9207*z3 + 185*z4 + 237*pow2(z2))) - 12*Mt*s2t*pow2(Sbeta)*(8*
        MuSUSY*(590*z3 - 4*z4 + 75*pow2(z2)) + Mst2*Tbeta*(-21022*z3 + 922*z4 +
        1383*pow2(z2)))) + 8*Mst2*MuSUSY*(24*Mt*pow2(Sbeta)*(-955*z3 + 26*z4 +
        93*pow2(z2)) - MuSUSY*s2t*Tbeta*(-1 + pow2(Sbeta))*(-59336*z3 - 709*z4
        + 8535*pow2(z2)))*pow3(Mt) + 6*Mt*pow2(s2t)*pow2(Sbeta)*(4*Mt*Tbeta*(
        741*z3 + 100*z4 + 150*pow2(z2)) + MuSUSY*s2t*(20653*z3 + 316*z4 + 474*
        pow2(z2)))*pow4(Mst2) + Tbeta*(-48*pow2(MuSUSY)*(-1 + pow2(Sbeta))*(
        250*z3 - 94*z4 + 21*pow2(z2))*pow4(Mt) + pow2(Sbeta)*(3*Mst2*s2t*(-
        2239*z3 + 54*z4) + 2*Mt*(-6250*z3 + 208*z4 + 1203*pow2(z2)))*pow3(s2t)*
        pow5(Mst2)))) + 9*Mst2*(-(xDmglst2*pow2(Dmglst2)*(-4*pow2(Mst2)*pow2(
        Mt)*(-(Tbeta*pow2(MuSUSY)*pow2(s2t)*(-1 + pow2(Sbeta))*(129397*z3 +
        6344*z4 + 1254*pow2(z2))) + 2*Mt*pow2(Sbeta)*(3*MuSUSY*s2t*(55597*z3 -
        264*z4 + 252*pow2(z2)) + Mt*Tbeta*(-154541*z3 + 1064*z4 + 1596*pow2(z2)
        ))) + 4*Mt*s2t*pow2(Sbeta)*(4*Mt*(3*MuSUSY*s2t*(36013*z3 + 218*z4 -
        3804*pow2(z2)) + 2*Mt*Tbeta*(-19363*z3 + 112*z4 + 168*pow2(z2))) +
        Mst2*s2t*(3*Mt*Tbeta*(55597*z3 - 264*z4 + 252*pow2(z2)) + MuSUSY*s2t*(
        129397*z3 + 6344*z4 + 1254*pow2(z2))))*pow3(Mst2) + 32*Mst2*MuSUSY*(Mt*
        pow2(Sbeta)*(19363*z3 - 112*z4 - 168*pow2(z2)) - MuSUSY*s2t*Tbeta*(-1 +
        pow2(Sbeta))*(-36013*z3 - 218*z4 + 3804*pow2(z2)))*pow3(Mt) + Tbeta*(-
        576*(608*z3 - 9*z4)*pow2(MuSUSY)*(-1 + pow2(Sbeta))*pow4(Mt) + pow2(
        Sbeta)*(-(Mst2*s2t*(129397*z3 + 6344*z4 + 1254*pow2(z2))) + 16*Mt*(-
        36013*z3 - 218*z4 + 3804*pow2(z2)))*pow3(s2t)*pow5(Mst2)))) - 6*
        Dmglst2*Mst2*(4*Mt*pow2(Mst2)*(pow2(Mst2)*pow2(s2t)*pow2(Sbeta)*(4*Mt*
        Tbeta*(439*z3 - 108*z4) + MuSUSY*s2t*(16175*z3 + 424*z4 + 474*pow2(z2))
        ) + Mt*(Tbeta*pow2(MuSUSY)*pow2(s2t)*(-1 + pow2(Sbeta))*(16175*z3 +
        424*z4 + 474*pow2(z2)) + 2*Mt*pow2(Sbeta)*(4*MuSUSY*s2t*(-439*z3 + 108*
        z4) + Mt*Tbeta*(11897*z3 + 56*z4 + 84*pow2(z2))))) - 2*s2t*pow2(Mt)*
        pow2(Sbeta)*(4*Mt*Tbeta*(5742*z3 - 506*z4 + 105*pow2(z2)) + 3*MuSUSY*
        s2t*(-37474*z3 - 542*z4 + 5289*pow2(z2)))*pow3(Mst2) + 4*Mst2*MuSUSY*(
        2*Mt*pow2(Sbeta)*(5742*z3 - 506*z4 + 105*pow2(z2)) - MuSUSY*s2t*Tbeta*(
        -1 + pow2(Sbeta))*(-37474*z3 - 542*z4 + 5289*pow2(z2)))*pow3(Mt) +
        Tbeta*(-64*(83*z3 - 27*z4)*pow2(MuSUSY)*(-1 + pow2(Sbeta))*pow4(Mt) +
        pow2(Sbeta)*(-(Mst2*s2t*(16175*z3 + 424*z4 + 474*pow2(z2))) + 2*Mt*(-
        37474*z3 - 542*z4 + 5289*pow2(z2)))*pow3(s2t)*pow5(Mst2))) + 6*pow2(
        Mst2)*(12*pow2(Mst2)*pow2(Mt)*(-(Mt*MuSUSY*s2t*pow2(Sbeta)*((1622 - 48*
        lmMst1 + 48*lmMst2)*z3 + 142*z4 + 69*pow2(z2))) + Tbeta*(-2*pow2(Mt)*
        pow2(Sbeta)*((3902 - 24*(lmMst1 + lmMst2) + 48*lmMt)*z3 + 14*z4 + 21*
        pow2(z2)) + pow2(MuSUSY)*pow2(s2t)*(1 - pow2(Sbeta))*((1913 + 12*lmMst1
        - 12*lmMst2)*z3 - 10*z4 + 48*pow2(z2)))) + 6*s2t*pow2(Mt)*pow2(Sbeta)*(
        -4*Mt*Tbeta*(-1922*z3 + 206*z4 + 21*pow2(z2)) + 5*MuSUSY*s2t*(-2098*z3
        - 102*z4 + 333*pow2(z2)))*pow3(Mst2) - 4*Mst2*MuSUSY*(-6*Mt*pow2(Sbeta)
        *(-1922*z3 + 206*z4 + 21*pow2(z2)) - 5*MuSUSY*s2t*Tbeta*(-1 + pow2(
        Sbeta))*(-2098*z3 - 102*z4 + 333*pow2(z2)))*pow3(Mt) - 6*Mt*pow2(s2t)*
        pow2(Sbeta)*(2*MuSUSY*s2t*((1913 + 12*lmMst1 - 12*lmMst2)*z3 - 10*z4 +
        48*pow2(z2)) - Mt*Tbeta*((1622 - 48*lmMst1 + 48*lmMst2)*z3 + 142*z4 +
        69*pow2(z2)))*pow4(Mst2) + Tbeta*(-32*(230*z3 + 27*z4)*pow2(MuSUSY)*(-1
        + pow2(Sbeta))*pow4(Mt) + 10*Mt*pow2(Sbeta)*(2098*z3 + 102*z4 - 333*
        pow2(z2))*pow3(s2t)*pow5(Mst2) + 3*pow2(Sbeta)*((1913 + 12*lmMst1 - 12*
        lmMst2)*z3 - 10*z4 + 48*pow2(z2))*pow4(s2t)*pow6(Mst2))))))/(5832.*
        Tbeta*pow2(Sbeta)*pow5(Mst2)) + pow2(Al4p)*((5*(1 - 2*lmMsq)*
        threeLoopFlag*xMsq*pow2(Msq)*(4*Mt*MuSUSY*s2t*(Mt*MuSUSY*s2t*Tbeta*
        pow2(Mst1)*(pow2(Mst2) - 2*(lmMst1 - lmMst2)*(pow2(Mst1) + pow2(Mst2)))
        + pow2(Mst2)*pow2(Sbeta)*((-1 + shiftst1)*pow2(Mst2)*(-4*pow2(Mt) +
        pow2(Mst2)*pow2(s2t)) - pow2(Mst1)*(-4*(-1 + shiftst2)*pow2(Mt) + (
        shiftst1 - shiftst2 + lmMst1*(-2 + shiftst1 + shiftst2) - lmMst2*(-2 +
        shiftst1 + shiftst2))*pow2(Mst2)*pow2(s2t)) - (-1 + shiftst2)*pow2(s2t)
        *pow4(Mst1))) + Tbeta*(-8*pow2(Mst1)*pow2(Mst2)*pow2(Mt)*((1 - lmMst1 +
        lmMst2)*shiftst1*pow2(MuSUSY)*pow2(s2t) + 2*shiftst2*pow2(Mt)*pow2(
        Sbeta)) + 4*pow2(Mt)*pow2(MuSUSY)*pow2(s2t)*pow4(Mst2) + pow2(Sbeta)*(-
        4*pow2(Mt)*pow2(s2t)*(2*(pow2(Mst2) + (-lmMst1 + lmMst2)*pow2(MuSUSY))*
        pow4(Mst1) + pow2(Mst1)*((1 - 2*lmMst1 + 2*lmMst2)*pow2(Mst2)*pow2(
        MuSUSY) - 4*pow4(Mst2)) + (2*pow2(Mst2) + pow2(MuSUSY))*pow4(Mst2)) +
        16*pow2(Mst2)*(pow2(Mst1) + pow2(Mst2))*pow4(Mt) + pow2(Mst1)*pow2(
        Mst2)*((-1 + 2*lmMst1 - 2*lmMst2)*pow2(Mst1)*pow2(Mst2) + pow4(Mst1) +
        (-1 - 2*lmMst1 + 2*lmMst2)*pow4(Mst2))*pow4(s2t)) - shiftst1*(4*pow2(
        Mst1)*pow2(Mst2)*pow2(s2t)*pow2(Sbeta)*(2*(1 - lmMst1 + lmMst2)*pow2(
        Mt)*(pow2(Mst2) - pow2(MuSUSY)) - pow2(s2t)*pow4(Mst2)) + pow4(Mst1)*(
        8*(-1 + 2*lmMst1 - 2*lmMst2)*pow2(Mt)*pow2(MuSUSY)*pow2(s2t)*(-1 +
        pow2(Sbeta)) + (3 + 2*lmMst1 - 2*lmMst2)*pow2(Sbeta)*pow4(Mst2)*pow4(
        s2t)) + pow4(Mst2)*(-4*pow2(Mt)*pow2(s2t)*(pow2(MuSUSY)*(-1 + pow2(
        Sbeta)) + 2*pow2(Mst2)*pow2(Sbeta)) + pow2(Sbeta)*(16*pow4(Mt) + pow4(
        Mst2)*pow4(s2t)))) - shiftst2*pow2(Mst1)*pow2(s2t)*(4*pow2(Mst2)*pow2(
        Mt)*(pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 2*(1 + lmMst1 - lmMst2)*pow2(
        Mst2)*pow2(Sbeta)) - 4*pow2(Mst1)*(2*pow2(Mt)*((-1 + lmMst1 - lmMst2)*
        pow2(MuSUSY)*(-1 + pow2(Sbeta)) + pow2(Mst2)*pow2(Sbeta)) + pow2(s2t)*
        pow2(Sbeta)*pow4(Mst2)) + pow2(s2t)*pow2(Sbeta)*(pow2(Mst2)*pow4(Mst1)
        + (3 - 2*lmMst1 + 2*lmMst2)*pow6(Mst2))) + pow2(Sbeta)*pow4(s2t)*pow8(
        Mst2))))/(6.*Tbeta*pow2(Mst1)*pow2(Sbeta)*pow4(Mst2)) + threeLoopFlag*(
        (s2t*pow3(Mt)*((-18*pow2(Mst1)*(1058993 - 330480*lmMst2 + 64800*lmMsq*(
        5 - 2*lmMst1 + 4*lmMst2 - 2*lmMt) + 8640*(-13 + 4*lmMst2 + 4*lmMt)*
        pow2(lmMst1) + 622080*pow2(lmMst2) - 4320*lmMt*(-31 - 6*lmMst2 + 8*
        pow2(lmMst2)) - 2160*lmMst1*(325 + 300*lmMst2 - 52*lmMt + 112*pow2(
        lmMst2) - 48*pow2(lmMt)) - 103680*lmMst2*pow2(lmMt) + (21600*(7 - 2*
        lmMst1 + 4*lmMst2 - 2*lmMt)*pow2(Mst1))/pow2(Msq) + 207360*pow3(lmMst2)
        ))/Mst2 + 1080*pow3(Mst2)*(-8*((5*(119 - 63*lmMst2 + 12*lmMst1*(4 + 3*
        lmMst2) - 6*(1 + 3*lmMst2)*lmMt + 3*lmMsq*(7 - 12*lmMst1 + 18*lmMst2 +
        6*lmMt) - 18*pow2(lmMsq) - 36*pow2(lmMst2)))/pow2(Msq) + (144*pow2(1 +
        lmMst2))/pow2(Mst1)) + (15*Mst2*(Mst2*(-206 - 6*lmMst2 - 9*lmMst1*(3 +
        4*lmMst2) + 12*lmMsq*(2 + 3*lmMst1 - 6*lmMst2 - 3*lmMt) + 9*lmMt + 36*
        lmMst2*lmMt + 36*pow2(lmMsq) + 36*pow2(lmMst2)) + Dmglst2*(-754 + 50*
        lmMst2 - 9*lmMst1*(7 + 20*lmMst2) + 4*lmMsq*(10 + 45*lmMst1 - 66*lmMst2
        - 21*lmMt) - 27*lmMt + 84*lmMst2*lmMt + 84*pow2(lmMsq) + 180*pow2(
        lmMst2))) - 10*(5 - 54*lmMst2 + 3*lmMst1*(7 + 12*lmMst2) + 21*lmMt +
        36*lmMst2*lmMt - 12*lmMsq*(-1 + 3*lmMst1 + 3*lmMt) + 36*pow2(lmMsq) -
        36*pow2(lmMst2))*pow2(Mst1))/pow4(Msq)) - (24*(1907557 - 326160*lmMst2
        + 1080*(-158 + 6*lmMst2 + 39*lmMt)*pow2(lmMst1) + 75600*pow2(lmMst2) +
        48600*lmMsq*(2 + 11*lmMst2 - lmMst1*(9 + 2*lmMst2 - 2*lmMt) - 2*(1 +
        lmMst2)*lmMt + 2*pow2(lmMst2)) + 1620*lmMt*(83 + 104*lmMst2 + 58*pow2(
        lmMst2)) - 540*lmMst1*(797 + 120*lmMt + 4*lmMst2*(4 + 63*lmMt) + 78*
        pow2(lmMst2) - 144*pow2(lmMt)) - 77760*lmMst2*pow2(lmMt) + 1080*pow3(
        lmMst1) + 34560*pow3(lmMst2))*pow4(Mst1))/pow3(Mst2) + (54*Dmglst2*(
        pow2(Msq)*pow2(Mst1)*(678821 + 737120*lmMst2 + 21600*lmMsq*(13 - 2*
        lmMst1 + 4*lmMst2 - 2*lmMt) - 2880*(21 + 12*lmMst2 - 4*lmMt)*pow2(
        lmMst1) + 440160*pow2(lmMst2) - 480*lmMt*(-279 - 13*lmMst2 + 24*pow2(
        lmMst2)) - 34560*(2 + lmMst2)*pow2(lmMt) + 160*lmMst1*(-4724 - 1941*
        lmMst2 - 39*lmMt + 72*pow2(lmMst2) + 216*pow2(lmMt)) + 23040*pow3(
        lmMst2)) + 7200*(15 - 2*lmMst1 + 4*lmMst2 - 2*lmMt)*pow4(Mst1)))/(pow2(
        Msq)*pow2(Mst2)) + (54*Mst2*(-400*(95 - 18*lmMst2 + 6*lmMst1*(5 + 6*
        lmMst2) + 6*(5 + 6*lmMst2)*lmMt - 6*lmMsq*(7 + 6*(lmMst1 + lmMt)) + 36*
        pow2(lmMsq) - 36*pow2(lmMst2))*pow2(Msq)*pow2(Mst1) - 9*(5253 - 4640*
        lmMst2 + 1200*lmMsq*(13 - 4*lmMst2*(1 + lmMst2) + lmMst1*(2 + 4*lmMst2)
        + 2*lmMt) - 2400*(lmMst1 - lmMst2)*pow2(lmMsq) - 1280*(1 + lmMst2)*
        pow2(lmMst1) + 29600*pow2(lmMst2) - 80*lmMt*(127 + 96*lmMst2 + 32*pow2(
        lmMst2)) - 80*lmMst1*(183 + 274*lmMst2 - 32*(1 + lmMst2)*lmMt + 112*
        pow2(lmMst2)) - 3840*pow2(lmMt) + 10240*pow3(lmMst2))*pow4(Msq) - 100*(
        47 - 54*lmMst2 + (3 + 36*lmMst2)*(lmMst1 + lmMt) + 12*lmMsq*(4 - 3*(
        lmMst1 + lmMt)) + 36*pow2(lmMsq) - 36*pow2(lmMst2))*pow4(Mst1) + (120*
        Dmglst2*Mst2*(-20*(125 - 37*lmMst2 + 12*lmMst1*(2 + 3*lmMst2) + 6*lmMt
        - 6*lmMst2*lmMt + lmMsq*(7 - 36*lmMst1 + 42*lmMst2 + 6*lmMt) - 6*pow2(
        lmMsq) - 36*pow2(lmMst2))*pow2(Msq)*pow2(Mst1) - 192*(-1 + 2*lmMst2 +
        3*pow2(lmMst2))*pow4(Msq) - 5*(41 - 6*lmMst2 + 3*lmMst1*(-1 + 12*
        lmMst2) + (-3 + 36*lmMst2)*lmMt + 12*lmMsq*(1 - 3*(lmMst1 + lmMt)) +
        36*pow2(lmMsq) - 36*pow2(lmMst2))*pow4(Mst1)))/pow2(Mst1)))/pow4(Msq) +
        2*Dmglst2*((-10800*(131 + 126*lmMst2 + 6*lmMst1*(-7 + 6*lmMst2) + 6*(-7
        + 6*lmMst2)*lmMt - 6*lmMsq*(7 + 6*(lmMst1 + lmMt)) + 36*pow2(lmMsq) -
        36*pow2(lmMst2))*pow2(Mst1))/pow2(Msq) + 135*(1983 + 182144*lmMst2 +
        4320*(lmMst1 - lmMst2)*pow2(lmMsq) + 2304*(-1 + lmMst2)*pow2(lmMst1) +
        33504*pow2(lmMst2) + 240*lmMsq*(-161 + lmMst1*(54 - 36*lmMst2) - 60*
        lmMst2 + 6*lmMt + 36*pow2(lmMst2)) + 48*lmMt*(31 - 128*lmMst2 + 96*
        pow2(lmMst2)) + 144*lmMst1*(-285 + 32*lmMt - 2*lmMst2*(55 + 16*lmMt) +
        112*pow2(lmMst2)) - 18432*pow3(lmMst2)) + pow4(Mst1)*((-2700*(155 + 90*
        lmMst2 + 3*lmMst1*(-23 + 12*lmMst2) + (-69 + 36*lmMst2)*lmMt + 12*
        lmMsq*(4 - 3*(lmMst1 + lmMt)) + 36*pow2(lmMsq) - 36*pow2(lmMst2)))/
        pow4(Msq) + (4*(24208447 + 1263240*lmMst2 - 9720*(266 + 74*lmMst2 - 55*
        lmMt)*pow2(lmMst1) - 1140480*pow2(lmMst2) + 145800*lmMsq*(28 + lmMst2*(
        41 - 6*lmMt) - 10*lmMt + lmMst1*(-31 - 6*lmMst2 + 6*lmMt) + 6*pow2(
        lmMst2)) + 3240*lmMt*(846 + 1042*lmMst2 + 309*pow2(lmMst2)) - 233280*(2
        + 3*lmMst2)*pow2(lmMt) + 360*lmMst1*(-21491 + lmMst2*(8190 - 4266*lmMt)
        - 5922*lmMt + 3267*pow2(lmMst2) + 1944*pow2(lmMt)) + 9720*pow3(lmMst1)
        - 466560*pow3(lmMst2)))/pow4(Mst2))) + 15*pow2(MuSUSY)*(18*Dmglst2*((
        720*(95 + 12*lmMsq*(-2 + lmMst1 - lmMst2) + 32*lmMst2 - 4*lmMst1*(2 +
        3*lmMst2) + 12*pow2(lmMst2)))/pow2(Msq) - (2304*(-1 + 2*lmMst2 + 3*
        pow2(lmMst2)))/pow2(Mst1) + (5*(8713 + 12*lmMsq*(-179 + 270*lmMst1 -
        270*lmMst2) + 3282*lmMst2 - 162*lmMst1*(7 + 20*lmMst2) + 3240*pow2(
        lmMst2))*pow2(Mst1))/pow4(Msq)) + Mst2*(864*((5*(71 + 12*lmMsq*(-2 +
        lmMst1 - lmMst2) + 40*lmMst2 - 4*lmMst1*(4 + 3*lmMst2) + 12*pow2(
        lmMst2)))/pow2(Msq) - (48*pow2(1 + lmMst2))/pow2(Mst1)) + (90*(Mst2*(
        Mst2*(911 + 12*lmMsq*(-37 + 18*lmMst1 - 18*lmMst2) + 606*lmMst2 - 54*
        lmMst1*(3 + 4*lmMst2) + 216*pow2(lmMst2)) + Dmglst2*(5591 + 12*lmMsq*(-
        181 + 90*lmMst1 - 90*lmMst2) + 2550*lmMst2 - 54*lmMst1*(7 + 20*lmMst2)
        + 1080*pow2(lmMst2))) + (1153 + 12*lmMsq*(-35 + 54*lmMst1 - 54*lmMst2)
        + 906*lmMst2 - 162*lmMst1*(3 + 4*lmMst2) + 648*pow2(lmMst2))*pow2(Mst1)
        ))/pow4(Msq)) - (24*(pow2(Msq)*pow2(Mst1)*(125789 + 13716*B4 - 54*DN -
        29160*lmMsq + 3240*pow2(lmMsq) - 108*lmMst1*(422 - 45*lmMsq + 45*pow2(
        lmMsq)) - 2376*pow2(lmMst1) + 108*lmMst2*(1096 - 551*lmMst1 + 15*lmMsq*
        (-7 + 6*lmMst1) + 45*pow2(lmMsq) + 8*pow2(lmMst1)) - 108*(-717 + 90*
        lmMsq + 248*lmMst1)*pow2(lmMst2) + 25920*pow3(lmMst2)) - 360*(55 + 6*
        lmMsq*(-2 + 5*lmMst1 - 5*lmMst2) + 52*lmMst2 - 10*lmMst1*(4 + 3*lmMst2)
        + 30*pow2(lmMst2))*pow4(Mst1)))/(pow2(Msq)*pow3(Mst2)) - (27*(-640*(23
        + lmMsq*(-6 + 9*lmMst1 - 9*lmMst2) + 18*lmMst2 - 3*lmMst1*(4 + 3*
        lmMst2) + 9*pow2(lmMst2))*pow2(Msq)*pow2(Mst1) + 3*(18201 + 1760*B4 -
        16*DN - 5760*lmMsq + 960*pow2(lmMsq) - 16*lmMst1*(199 - 30*lmMsq + 30*
        pow2(lmMsq)) + 16*lmMst2*(1291 - 322*lmMst1 + 30*lmMsq*(-5 + 2*lmMst1)
        + 30*pow2(lmMsq) - 16*pow2(lmMst1)) - 256*pow2(lmMst1) - 32*(-313 + 30*
        lmMsq + 72*lmMst1)*pow2(lmMst2) + 2560*pow3(lmMst2))*pow4(Msq) - 20*(
        233 + 36*lmMsq*(-2 + 5*lmMst1 - 5*lmMst2) + 207*lmMst2 - 45*lmMst1*(3 +
        4*lmMst2) + 180*pow2(lmMst2))*pow4(Mst1)))/(Mst2*pow4(Msq)) - (2916*
        Dmglst2*(481.4302469135802 + (1048*B4)/9. - (20*DN)/9. - 400*lmMsq + (
        80*pow2(lmMsq))/3. - (4*lmMst1*(-173 + 90*lmMsq + 30*pow2(lmMsq)))/9. +
        (4*lmMst2*(1623 - 130*lmMst1 + 30*lmMsq*(-1 + 2*lmMst1) + 30*pow2(
        lmMsq) - 16*pow2(lmMst1)))/9. + (64*pow2(lmMst1))/9. - (8*(-265 + 30*
        lmMsq + 136*lmMst1)*pow2(lmMst2))/9. - (80*(77 + 6*lmMsq*(-2 + 3*lmMst1
        - 3*lmMst2) + 24*lmMst2 - 6*lmMst1*(2 + 3*lmMst2) + 18*pow2(lmMst2))*
        pow2(Mst1))/(9.*pow2(Msq)) + 128*pow3(lmMst2) - (5*(1961 + 180*lmMsq*(-
        2 + 5*lmMst1 - 5*lmMst2) + 675*lmMst2 - 45*lmMst1*(7 + 20*lmMst2) +
        900*pow2(lmMst2))*pow4(Mst1))/(27.*pow4(Msq))))/pow2(Mst2) - (2916*
        Dmglst2*pow2(Mst1)*(881.6139917695473 + 248*B4 - 4*DN - (2560*lmMsq)/3.
         + lmMst1*(130.96296296296296 - 120*lmMsq - 40*pow2(lmMsq)) + (80*pow2(
        lmMsq))/3. + (176*pow2(lmMst1))/9. + (8*lmMst2*(4364 - 573*lmMst1 + 45*
        lmMsq*(5 + 6*lmMst1) + 135*pow2(lmMsq) + 24*pow2(lmMst1)))/27. - (8*(-
        377 + 90*lmMsq + 376*lmMst1)*pow2(lmMst2))/9. - (80*(107 + 6*lmMsq*(-2
        + 5*lmMst1 - 5*lmMst2) + 32*lmMst2 - 10*lmMst1*(2 + 3*lmMst2) + 30*
        pow2(lmMst2))*pow2(Mst1))/(9.*pow2(Msq)) + (2944*pow3(lmMst2))/9.))/
        pow4(Mst2) - (2*(2552929 + 257904*B4 - 648*DN - 456840*lmMsq + 38880*
        pow2(lmMsq) - 216*lmMst1*(4591 - 360*lmMsq + 450*pow2(lmMsq)) + 41904*
        pow2(lmMst1) + 216*lmMst2*(9211 - 6466*lmMst1 + 180*lmMsq*(-4 + 5*
        lmMst1) + 450*pow2(lmMsq) + 576*pow2(lmMst1)) - 864*(-1784 + 225*lmMsq
        + 840*lmMst1)*pow2(lmMst2) - 3456*pow3(lmMst1) + 604800*pow3(lmMst2))*
        pow4(Mst1))/pow5(Mst2)) + (-108*S2*(54*pow2(Mst1)*pow2(Mst2)*((4367 +
        3770*lmMst1 - 3770*lmMst2)*pow2(Mst2) + 10*(169 + 136*lmMst1 - 136*
        lmMst2)*pow2(MuSUSY)) + 6*(8*(16297 + 10275*lmMst1 - 10275*lmMst2)*
        pow2(Mst2) + 5*(9185 + 5646*lmMst1 - 5646*lmMst2)*pow2(MuSUSY))*pow4(
        Mst1) - Dmglst2*Mst2*(18*pow2(Mst1)*((15691 + 41490*lmMst1 - 41490*
        lmMst2)*pow2(Mst2) + 4*(2489 + 3795*lmMst1 - 3795*lmMst2)*pow2(MuSUSY))
        - 81*pow2(Mst2)*(50*(27 - 14*lmMst1 + 14*lmMst2)*pow2(Mst2) + (453 -
        350*lmMst1 + 350*lmMst2)*pow2(MuSUSY)) + 16*(220117 + 192975*lmMst1 -
        192975*lmMst2)*pow4(Mst1)) - 243*(2*(79 - 70*lmMst1 + 70*lmMst2)*pow2(
        Mst2) + 5*(1 - 14*lmMst1 + 14*lmMst2)*pow2(MuSUSY))*pow4(Mst2)) - 160*
        OepS2*(Dmglst2*Mst2*(945*pow2(Mst2)*(2*pow2(Mst2) + pow2(MuSUSY)) + 18*
        pow2(Mst1)*(1383*pow2(Mst2) + 506*pow2(MuSUSY)) + 102920*pow4(Mst1)) -
        3*(2*(2740*pow2(Mst2) + 941*pow2(MuSUSY))*pow4(Mst1) + 189*(2*pow2(
        Mst2) + pow2(MuSUSY))*pow4(Mst2) + 6*pow2(Mst1)*(136*pow2(Mst2)*pow2(
        MuSUSY) + 377*pow4(Mst2)))))/pow5(Mst2)))/43740. - Mt*pow3(s2t)*(
        Dmglst2*pow2(Mst1)*(47.73436213991769 - (68*B4)/9. - (2*DN)/9. + (80*
        lmMsq)/3. + (40*pow2(lmMsq))/3. + (4*lmMst1*(77 + 135*lmMsq + 45*pow2(
        lmMsq)))/27. + (4*lmMst2*(409 + 183*lmMst1 - 45*lmMsq*(7 + 2*lmMst1) -
        45*pow2(lmMsq) - 72*pow2(lmMst1)))/27. - (8*pow2(lmMst1))/3. + (4*(105
        + 30*lmMsq + 104*lmMst1)*pow2(lmMst2))/9. + pow2(Mst1)*(20/(9.*pow2(
        Msq)) + (406.2899291266575 - (24322*lmMst2)/243. + lmMsq*(
        23.333333333333332 - 20*lmMst1 + 20*lmMst2) + (4*(37 - 40*lmMst2)*pow2(
        lmMst1))/9. + (148*pow2(lmMst2))/9. + (2*lmMst1*(8705 - 3996*lmMst2 +
        5616*pow2(lmMst2)))/243. - (32*pow3(lmMst1))/9. - (224*pow3(lmMst2))/9.
        )/pow2(Mst2)) - (320*pow3(lmMst2))/9.) - ((542267 - 29376*lmMst2 +
        19440*lmMsq*(1 - 2*lmMst1 + 2*lmMst2) + 864*(205 + 216*lmMst2)*pow2(
        lmMst1) + 177120*pow2(lmMst2) - 1728*lmMst1*(31 + 205*lmMst2 + 204*
        pow2(lmMst2)) - 6912*pow3(lmMst1) + 172800*pow3(lmMst2))*pow4(Mst1))/(
        5832.*Mst2) - Mst2*(pow2(Mst1)*(19.17798353909465 + (68*B4)/9. + (2*DN)
        /9. + 40*lmMsq - (40*pow2(lmMsq))/3. - (4*lmMst1*(223 - 15*lmMsq + 15*
        pow2(lmMsq)))/9. - (8*pow2(lmMst1))/3. + (4*lmMst2*(-163 - 229*lmMst1 +
        15*lmMsq*(3 + 2*lmMst1) + 15*pow2(lmMsq) + 24*pow2(lmMst1)))/9. - (4*(-
        107 + 30*lmMsq + 104*lmMst1)*pow2(lmMst2))/9. + (320*pow3(lmMst2))/9.)
        + (20*pow4(Mst1))/(9.*pow2(Msq))) - Dmglst2*pow2(Mst2)*(
        254.93734567901234 + (524*B4)/9. - (10*DN)/9. - 200*lmMsq + (40*pow2(
        lmMsq))/3. - (2*lmMst1*(-173 + 90*lmMsq + 30*pow2(lmMsq)))/9. + (2*
        lmMst2*(1495 - 130*lmMst1 + 30*lmMsq*(-1 + 2*lmMst1) + 30*pow2(lmMsq) -
        16*pow2(lmMst1)))/9. + (32*pow2(lmMst1))/9. - (4*(-169 + 30*lmMsq +
        136*lmMst1)*pow2(lmMst2))/9. + (80*(9 - 3*lmMsq*(2 + lmMst1 - lmMst2) +
        4*lmMst2 + lmMst1*(2 + 3*lmMst2) - 3*pow2(lmMst2))*pow2(Mst1))/(9.*
        pow2(Msq)) + 64*pow3(lmMst2) + (5*(23 + 12*lmMsq - 12*lmMst2)*pow4(
        Mst1))/(108.*pow4(Msq))) + (pow3(Mst2)*(-320*(25 - 6*lmMsq*(2 + lmMst1
        - lmMst2) + 4*lmMst2 + lmMst1*(8 + 6*lmMst2) - 6*pow2(lmMst2))*pow2(
        Msq)*pow2(Mst1) - 3*(17177 + 1760*B4 - 16*DN - 5760*lmMsq + 960*pow2(
        lmMsq) - 16*lmMst1*(199 - 30*lmMsq + 30*pow2(lmMsq)) + 16*lmMst2*(1163
        - 322*lmMst1 + 30*lmMsq*(-5 + 2*lmMst1) + 30*pow2(lmMsq) - 16*pow2(
        lmMst1)) - 256*pow2(lmMst1) - 32*(-281 + 30*lmMsq + 72*lmMst1)*pow2(
        lmMst2) + 2560*pow3(lmMst2))*pow4(Msq) + 10*(1 - 12*lmMsq + 12*lmMst2)*
        pow4(Mst1) + (2*Dmglst2*Mst2*(240*(95 + 12*lmMsq*(-2 + lmMst1 - lmMst2)
        + 32*lmMst2 - 4*lmMst1*(2 + 3*lmMst2) + 12*pow2(lmMst2))*pow2(Msq)*
        pow2(Mst1) - 768*(-1 + 2*lmMst2 + 3*pow2(lmMst2))*pow4(Msq) - 5*(823 -
        12*lmMsq*(61 + 30*lmMst1 - 30*lmMst2) + 606*lmMst2 + 18*lmMst1*(7 + 20*
        lmMst2) - 360*pow2(lmMst2))*pow4(Mst1)))/pow2(Mst1)))/(216.*pow4(Msq))
        + ((4*((5*(71 + 12*lmMsq*(-2 + lmMst1 - lmMst2) + 40*lmMst2 - 4*lmMst1*
        (4 + 3*lmMst2) + 12*pow2(lmMst2)))/pow2(Msq) - (48*pow2(1 + lmMst2))/
        pow2(Mst1)))/27. + (5*(Dmglst2*Mst2*(5591 + 12*lmMsq*(-181 + 90*lmMst1
        - 90*lmMst2) + 2550*lmMst2 - 54*lmMst1*(7 + 20*lmMst2) + 1080*pow2(
        lmMst2)) + 3*(-223 + 12*lmMsq*(13 + 6*lmMst1 - 6*lmMst2) - 102*lmMst2 -
        18*lmMst1*(3 + 4*lmMst2) + 72*pow2(lmMst2))*pow2(Mst1)))/(324.*pow4(
        Msq)))*pow5(Mst2) + ((4*OepS2*(3*Mst2*(438*pow2(Mst1)*pow2(Mst2) + 439*
        pow4(Mst1) + 189*pow4(Mst2)) - Dmglst2*(7218*pow2(Mst1)*pow2(Mst2) +
        15571*pow4(Mst1) + 945*pow4(Mst2))))/2187. + (S2*(Dmglst2*(18*(14033 +
        12030*lmMst1 - 12030*lmMst2)*pow2(Mst1)*pow2(Mst2) + (836021 + 467130*
        lmMst1 - 467130*lmMst2)*pow4(Mst1) + 81*(-453 + 350*lmMst1 - 350*
        lmMst2)*pow4(Mst2)) - 15*(18*(347 + 146*lmMst1 - 146*lmMst2)*pow2(Mst1)
        *pow3(Mst2) + (6121 + 2634*lmMst1 - 2634*lmMst2)*Mst2*pow4(Mst1) + 81*(
        -1 + 14*lmMst1 - 14*lmMst2)*pow5(Mst2))))/810.)/pow2(Mst2)) + (pow2(Mt)
        *pow2(MuSUSY)*(-(Mt*s2t*((Dmglst2*((720*(95 + 12*lmMsq*(-2 + lmMst1 -
        lmMst2) + 32*lmMst2 - 4*lmMst1*(2 + 3*lmMst2) + 12*pow2(lmMst2)))/pow2(
        Msq) - (2304*(-1 + 2*lmMst2 + 3*pow2(lmMst2)))/pow2(Mst1) + (5*(8713 +
        12*lmMsq*(-179 + 270*lmMst1 - 270*lmMst2) + 3282*lmMst2 - 162*lmMst1*(7
        + 20*lmMst2) + 3240*pow2(lmMst2))*pow2(Mst1))/pow4(Msq)))/162. + Mst2*(
        (8*((5*(71 + 12*lmMsq*(-2 + lmMst1 - lmMst2) + 40*lmMst2 - 4*lmMst1*(4
        + 3*lmMst2) + 12*pow2(lmMst2)))/pow2(Msq) - (48*pow2(1 + lmMst2))/pow2(
        Mst1)))/27. + (5*(Mst2*(Mst2*(911 + 12*lmMsq*(-37 + 18*lmMst1 - 18*
        lmMst2) + 606*lmMst2 - 54*lmMst1*(3 + 4*lmMst2) + 216*pow2(lmMst2)) +
        Dmglst2*(5591 + 12*lmMsq*(-181 + 90*lmMst1 - 90*lmMst2) + 2550*lmMst2 -
        54*lmMst1*(7 + 20*lmMst2) + 1080*pow2(lmMst2))) + (1153 + 12*lmMsq*(-35
        + 54*lmMst1 - 54*lmMst2) + 906*lmMst2 - 162*lmMst1*(3 + 4*lmMst2) +
        648*pow2(lmMst2))*pow2(Mst1)))/(162.*pow4(Msq))) - (2*pow2(Msq)*pow2(
        Mst1)*(125789 + 13716*B4 - 54*DN - 29160*lmMsq + 3240*pow2(lmMsq) -
        108*lmMst1*(422 - 45*lmMsq + 45*pow2(lmMsq)) - 2376*pow2(lmMst1) + 108*
        lmMst2*(1096 - 551*lmMst1 + 15*lmMsq*(-7 + 6*lmMst1) + 45*pow2(lmMsq) +
        8*pow2(lmMst1)) - 108*(-717 + 90*lmMsq + 248*lmMst1)*pow2(lmMst2) +
        25920*pow3(lmMst2)) - 720*(55 + 6*lmMsq*(-2 + 5*lmMst1 - 5*lmMst2) +
        52*lmMst2 - 10*lmMst1*(4 + 3*lmMst2) + 30*pow2(lmMst2))*pow4(Mst1))/(
        243.*pow2(Msq)*pow3(Mst2)) - (-640*(23 + lmMsq*(-6 + 9*lmMst1 - 9*
        lmMst2) + 18*lmMst2 - 3*lmMst1*(4 + 3*lmMst2) + 9*pow2(lmMst2))*pow2(
        Msq)*pow2(Mst1) + 3*(18201 + 1760*B4 - 16*DN - 5760*lmMsq + 960*pow2(
        lmMsq) - 16*lmMst1*(199 - 30*lmMsq + 30*pow2(lmMsq)) + 16*lmMst2*(1291
        - 322*lmMst1 + 30*lmMsq*(-5 + 2*lmMst1) + 30*pow2(lmMsq) - 16*pow2(
        lmMst1)) - 256*pow2(lmMst1) - 32*(-313 + 30*lmMsq + 72*lmMst1)*pow2(
        lmMst2) + 2560*pow3(lmMst2))*pow4(Msq) - 20*(233 + 36*lmMsq*(-2 + 5*
        lmMst1 - 5*lmMst2) + 207*lmMst2 - 45*lmMst1*(3 + 4*lmMst2) + 180*pow2(
        lmMst2))*pow4(Mst1))/(108.*Mst2*pow4(Msq)) - (Dmglst2*(
        481.4302469135802 + (1048*B4)/9. - (20*DN)/9. - 400*lmMsq + (80*pow2(
        lmMsq))/3. - (4*lmMst1*(-173 + 90*lmMsq + 30*pow2(lmMsq)))/9. + (4*
        lmMst2*(1623 - 130*lmMst1 + 30*lmMsq*(-1 + 2*lmMst1) + 30*pow2(lmMsq) -
        16*pow2(lmMst1)))/9. + (64*pow2(lmMst1))/9. - (8*(-265 + 30*lmMsq +
        136*lmMst1)*pow2(lmMst2))/9. - (80*(77 + 6*lmMsq*(-2 + 3*lmMst1 - 3*
        lmMst2) + 24*lmMst2 - 6*lmMst1*(2 + 3*lmMst2) + 18*pow2(lmMst2))*pow2(
        Mst1))/(9.*pow2(Msq)) + 128*pow3(lmMst2) - (5*(1961 + 180*lmMsq*(-2 +
        5*lmMst1 - 5*lmMst2) + 675*lmMst2 - 45*lmMst1*(7 + 20*lmMst2) + 900*
        pow2(lmMst2))*pow4(Mst1))/(27.*pow4(Msq))))/pow2(Mst2) - (Dmglst2*pow2(
        Mst1)*(881.6139917695473 + 248*B4 - 4*DN - (2560*lmMsq)/3. + lmMst1*(
        130.96296296296296 - 120*lmMsq - 40*pow2(lmMsq)) + (80*pow2(lmMsq))/3.
         + (176*pow2(lmMst1))/
        9. + (8*lmMst2*(4364 - 573*lmMst1 + 45*lmMsq*(5 + 6*lmMst1) + 135*pow2(
        lmMsq) + 24*pow2(lmMst1)))/27. - (8*(-377 + 90*lmMsq + 376*lmMst1)*
        pow2(lmMst2))/9. - (80*(107 + 6*lmMsq*(-2 + 5*lmMst1 - 5*lmMst2) + 32*
        lmMst2 - 10*lmMst1*(2 + 3*lmMst2) + 30*pow2(lmMst2))*pow2(Mst1))/(9.*
        pow2(Msq)) + (2944*pow3(lmMst2))/9.))/pow4(Mst2) - ((2552929 + 257904*
        B4 - 648*DN - 456840*lmMsq + 38880*pow2(lmMsq) - 216*lmMst1*(4591 -
        360*lmMsq + 450*pow2(lmMsq)) + 41904*pow2(lmMst1) + 216*lmMst2*(9211 -
        6466*lmMst1 + 180*lmMsq*(-4 + 5*lmMst1) + 450*pow2(lmMsq) + 576*pow2(
        lmMst1)) - 864*(-1784 + 225*lmMsq + 840*lmMst1)*pow2(lmMst2) - 3456*
        pow3(lmMst1) + 604800*pow3(lmMst2))*pow4(Mst1))/(1458.*pow5(Mst2)) + (-
        12*Mst2*(10120*Dmglst2*OepS2 - 2720*Mst2*OepS2 - 54*Dmglst2*(2489 +
        3795*lmMst1 - 3795*lmMst2)*S2 + 405*(169 + 136*lmMst1 - 136*lmMst2)*
        Mst2*S2)*pow2(Mst1) + 9*(-1400*Dmglst2*OepS2 + 81*Dmglst2*(-453 + 350*
        lmMst1 - 350*lmMst2)*S2 + 15*Mst2*(56*OepS2 + 81*(1 - 14*lmMst1 + 14*
        lmMst2)*S2))*pow3(Mst2) + 10*(7528*OepS2 - 27*(9185 + 5646*lmMst1 -
        5646*lmMst2)*S2)*pow4(Mst1))/(3645.*pow5(Mst2)))) + (pow2(Mt)*(144*
        Dmglst2*pow2(Mst2)*(180 - 2*B4 + 2*D3 - DN + 16*lmMst1 + 144*lmMst2 -
        216*S2 - 16*(-2 + lmMst1)*pow2(lmMst2) + 16*pow3(lmMst2)) + pow2(Mst1)*
        (3*Mst2*(10667 - 96*B4 + 96*D3 - 48*DN - 3072*lmMst1 - 384*(-25 + 9*
        lmMst1)*lmMst2 - 384*(-1 + lmMst1 - lmMst2)*lmMt - 224*OepS2 + 324*(-43
        + 14*lmMst1 - 14*lmMst2)*S2 - 384*(-13 + 4*lmMst1)*pow2(lmMst2) + 1536*
        pow3(lmMst2)) + 2*Dmglst2*(28405 - 288*B4 + 288*D3 - 144*DN + 5472*
        lmMst1 - 288*(-89 + 2*lmMst1)*lmMst2 + 576*(-2 + lmMst1 - lmMst2)*lmMt
        + 224*OepS2 - 324*(65 + 14*lmMst1 - 14*lmMst2)*S2 - 576*(-9 + 8*lmMst1)
        *pow2(lmMst2) + 4608*pow3(lmMst2))) + 24*(436 - 6*B4 + 6*D3 - 3*DN -
        48*lmMst1 + (408 - 96*lmMst1)*lmMst2 + 24*lmMt - 972*S2 - 48*(-4 +
        lmMst1)*pow2(lmMst2) + 48*pow3(lmMst2))*pow3(Mst2)))/(81.*pow5(Mst2)) +
        pow2(s2t)*(97.70987654320987 - (40*B4)/9. + (4*D3)/9. - (2*DN)/9. - (
        185*lmMsq)/9. + (25*pow2(lmMsq))/3. - (lmMst1*(5279 - 1950*lmMsq + 630*
        pow2(lmMsq)))/54. - (53*pow2(lmMst1))/9. + (lmMst2*(10421 - 6558*lmMst1
        + 30*lmMsq*(-95 + 42*lmMst1) + 630*pow2(lmMsq) + 234*pow2(lmMst1)))/54.
         - ((-1460 + 210*lmMsq + 399*lmMst1)*pow2(lmMst2))/9. + ((111 - 3028*
        lmMst2 + 90*lmMsq*(4 - 13*lmMst1 + 13*lmMst2) + 2*lmMst1*(1334 + 675*
        lmMst2) - 90*pow2(lmMst1) - 1260*pow2(lmMst2))*pow2(Mst1))/(135.*pow2(
        Msq)) + (((1672 - 4470*lmMst2 + 150*lmMst1*(27 + 16*lmMst2) + 60*lmMsq*
        (7 - 40*lmMst1 + 40*lmMst2) - 2400*pow2(lmMst2))/pow2(Msq) + (15*(341 +
        642*lmMst2 + 64*lmMst1*(1 + lmMst2) - 30*lmMsq*(5 + 6*lmMst2) + 90*
        pow2(lmMsq) + 272*pow2(lmMst2)))/pow2(Mst1))*pow2(Mst2))/270. - (16*
        pow3(lmMst1))/9. + (376*pow3(lmMst2))/9. + pow2(Mst1)*(-(4962073 +
        205200*B4 - 5400*DN - 405000*lmMsq + 120*lmMst1*(57971 - 14625*lmMsq +
        2700*pow2(lmMsq)) + 900*(-1331 + 180*lmMsq)*pow2(lmMst1) - 120*lmMst2*(
        52436 - 70455*lmMst1 + 225*lmMsq*(-65 + 36*lmMst1) + 2700*pow2(lmMsq) +
        8280*pow2(lmMst1)) + 900*(-8063 + 900*lmMsq + 3792*lmMst1)*pow2(lmMst2)
        + (90*(-1725 + lmMst1*(-7006 + 2640*lmMsq - 4800*lmMst2) + (7006 -
        2640*lmMsq)*lmMst2 + 1080*pow2(lmMst1) + 3720*pow2(lmMst2))*pow2(Mst1))
        /pow2(Msq) - 55800*pow3(lmMst1) - 2363400*pow3(lmMst2))/(24300.*pow2(
        Mst2)) + (Dmglst2*(109.11799176954733 - (8*B4)/3. + (32*D3)/9. - (20*
        DN)/9. + 80*lmMsq - lmMst1*(78.19061728395062 + 20*pow2(lmMsq)) - (
        2888*pow2(lmMst1))/135. + lmMst2*(40*lmMsq*lmMst1 + 20*pow2(lmMsq) + (
        4*(-21616 - 64515*lmMst1 + 31275*pow2(lmMst1)))/2025.) - (4*(-5023 +
        1350*lmMsq + 6285*lmMst1)*pow2(lmMst2))/135. + (5*(291 + 2*(-103 + 84*
        lmMsq)*lmMst2 + lmMst1*(206 - 168*lmMsq + 168*lmMst2) - 168*pow2(
        lmMst2))*pow2(Mst1))/(27.*pow2(Msq)) + (20*pow3(lmMst1))/27. + (3340*
        pow3(lmMst2))/27.))/pow3(Mst2)) - (Dmglst2*(192*lmMst2*(1 + lmMst2) + (
        (560 - 600*lmMsq + 600*lmMst2)*pow2(Mst1))/pow2(Msq))*pow3(Mst2))/(27.*
        pow4(Mst1)) + ((5*(-43 + 30*lmMsq - 30*lmMst2))/(27.*pow2(Msq)*pow2(
        Mst1)) - (-41220947 + 420*lmMsq*(12479 + 69090*lmMst1 - 69930*lmMst2) +
        21234990*lmMst2 - 10290*lmMst1*(2573 + 2820*lmMst2) + 176400*pow2(
        lmMsq) + 29194200*pow2(lmMst2))/(1.11132e7*pow4(Msq)) - (16*pow2(1 +
        lmMst2))/(9.*pow4(Mst1)))*pow4(Mst2) + pow4(Mst1)*((1.0702990137854083
         - (4519*lmMst2)/
        980. + lmMsq*(0.36171579743008314 - (169*lmMst1)/63. + (19*lmMst2)/7.)
        + lmMst1*(4.249508692365835 + (31*lmMst2)/9.) - pow2(lmMsq)/63. - (8*
        pow2(lmMst1))/21. - (194*pow2(lmMst2))/63.)/pow4(Msq) - (
        363.3804294212688 + (76*B4)/9. - (2*DN)/9. - (35*lmMsq)/2. + lmMst1*(
        211.3489518770471 - (695*lmMsq)/9. + (40*pow2(lmMsq))/3.) - (
        214.87936507936507 - 20*lmMsq)*pow2(lmMst1) - lmMst2*(
        190.46006298815823 - (71398*lmMst1)/105. + (5*lmMsq*(-139 + 120*lmMst1)
        )/9. + (40*pow2(lmMsq))/3. + (334*pow2(lmMst1))/3.) + ((-146507 +
        14700*lmMsq + 91070*lmMst1)*pow2(lmMst2))/315. - (44*pow3(lmMst1))/9. -
        (1556*pow3(lmMst2))/9.)/pow4(Mst2) + (Dmglst2*(536.1152102791342 - (8*
        B4)/3. + (32*D3)/9. - (20*DN)/9. + 90*lmMsq - (123.11224321827497 + 20*
        lmMsq*(1 + lmMsq))*lmMst1 - lmMst2*(17.33220122616948 - 20*lmMsq*(1 +
        lmMsq) + (133.04550264550264 - 40*lmMsq)*lmMst1 - (1180*pow2(lmMst1))/
        9.) - (15886*pow2(lmMst1))/945. + (149.85608465608465 - 40*lmMsq - (
        2812*lmMst1)/9.)*pow2(lmMst2) - (92*pow3(lmMst1))/27. + (4988*pow3(
        lmMst2))/27.))/pow5(Mst2)) - (108*Dmglst2*(344*OepS2 + 9*(15643 - 774*
        lmMst1 + 774*lmMst2)*S2)*pow2(Mst1)*pow2(Mst2) - 9*(3896*OepS2 - 81*(
        9473 + 974*lmMst1 - 974*lmMst2)*S2)*pow2(Mst1)*pow3(Mst2) + 3*Mst2*(-
        29428*OepS2 + 27*(160997 + 22071*lmMst1 - 22071*lmMst2)*S2)*pow4(Mst1)
        + 8*Dmglst2*(17308*OepS2 + 27*(93919 - 12981*lmMst1 + 12981*lmMst2)*S2)
        *pow4(Mst1) + 54*Dmglst2*(56*OepS2 - 81*(-1677 + 14*lmMst1 - 14*lmMst2)
        *S2)*pow4(Mst2) + 1296*(-4*OepS2 + 81*(22 + lmMst1 - lmMst2)*S2)*pow5(
        Mst2))/(2187.*pow5(Mst2)) + (((257250*Dmglst2*Mst2*(557 - 224*lmMst2 +
        12*lmMsq*(1 - 32*lmMst1 + 32*lmMst2) + 4*lmMst1*(53 + 96*lmMst2) - 384*
        pow2(lmMst2)) + (4167613 - 19932360*lmMst2 + 20580*lmMst1*(701 + 540*
        lmMst2) + 420*lmMsq*(13109 - 26460*lmMst1 + 25620*lmMst2) + 176400*
        pow2(lmMsq) - 10936800*pow2(lmMst2))*pow2(Mst1))*pow2(Mst2))/5.5566e6 -
        (5*(18*Dmglst2*(13 - 28*lmMsq + 28*lmMst2) + (67 - 84*lmMsq + 84*
        lmMst2)*Mst2)*pow5(Mst2))/(216.*pow2(Mst1)) + (Dmglst2*((18*(-115 + (
        366 + 64*lmMst1)*lmMst2 - 30*lmMsq*(-1 + 6*lmMst2) + 90*pow2(lmMsq) +
        336*pow2(lmMst2))*pow2(Mst2) + pow2(Mst1)*(3197 - 432*B4 + 576*D3 -
        360*DN + 9180*lmMsq + 1620*pow2(lmMsq) - 216*lmMst1*(-7 + 15*pow2(
        lmMsq)) - 1404*pow2(lmMst1) + 36*lmMst2*(-291 - 464*lmMst1 + 90*lmMsq*(
        -1 + 2*lmMst1) + 90*pow2(lmMsq) + 32*pow2(lmMst1)) - 36*(-607 + 180*
        lmMsq + 336*lmMst1)*pow2(lmMst2) + 10944*pow3(lmMst2)))*pow4(Msq) + 60*
        pow2(Msq)*((118 + 109*lmMst1 + (-133 + 102*lmMst1)*lmMst2 + 6*lmMsq*(4
        - 17*lmMst1 + 17*lmMst2) - 102*pow2(lmMst2))*pow2(Mst1)*pow2(Mst2) + 2*
        (69 + (-53 + 42*lmMsq)*lmMst2 + lmMst1*(53 - 42*lmMsq + 42*lmMst2) -
        42*pow2(lmMst2))*pow4(Mst1)) + 15*((219 - 95*lmMst2 + 132*lmMsq*lmMst2
        + lmMst1*(95 - 132*lmMsq + 132*lmMst2) - 132*pow2(lmMst2))*pow2(Mst2)*
        pow4(Mst1) + (216 - 95*lmMst2 + 132*lmMsq*lmMst2 + lmMst1*(95 - 132*
        lmMsq + 132*lmMst2) - 132*pow2(lmMst2))*pow6(Mst1))))/(162.*Mst2*pow2(
        Mst1)))/pow4(Msq))))/pow2(Sbeta) + pow4(s2t)*(Mst2*pow2(Mst1)*(-(Mst2*(
        95.16896090534979 - B4/9. + (2*D3)/9. - DN/6. - (445*lmMsq)/36. +
        lmMst1*(21.80061728395062 - (5*pow2(lmMsq))/2.) + (35*pow2(lmMsq))/12.
         + (lmMst2*(18499 + 20550*lmMst1 + 675*lmMsq*(-7 + 2*lmMst1) + 2025*
        pow2(lmMsq) - 6525*pow2(lmMst1)))/810. + ((-1649 + 180*lmMsq)*pow2(
        lmMst1))/108. + ((289 - 360*lmMsq + 1398*lmMst1)*pow2(lmMst2))/108. + (
        (-2953 + 60*lmMsq*(17 + 6*lmMst1 - 6*lmMst2) - 636*lmMst2 - 24*lmMst1*(
        16 + 75*lmMst2) + 720*pow2(lmMst1) + 1080*pow2(lmMst2))*pow2(Mst1))/(
        1080.*pow2(Msq)) - (79*pow3(lmMst1))/54. - (185*pow3(lmMst2))/54.)) +
        Dmglst2*(14.217769547325103 + (2*B4)/3. - (8*D3)/9. + (5*DN)/9. - (15*
        lmMsq)/2. - (5*pow2(lmMsq))/2. + lmMst1*(-24.21432098765432 + 5*pow2(
        lmMsq)) - lmMst2*(5*lmMsq*(-1 + 2*lmMst1) + 5*pow2(lmMsq) + (-128893 -
        86970*lmMst1 - 48150*pow2(lmMst1))/4050.) - (137*pow2(lmMst1))/135. + (
        10*lmMsq - (83*(34 + 15*lmMst1))/135.)*pow2(lmMst2) - (5*(25 + 12*
        lmMsq*(-4 + 3*lmMst1 - 3*lmMst2) + (48 - 36*lmMst1)*lmMst2 + 36*pow2(
        lmMst2))*pow2(Mst1))/(108.*pow2(Msq)) + (5*pow3(lmMst1))/27. - (77*
        pow3(lmMst2))/27.)) + (35.682629270197204 + B4 + D3/9. - DN/9. - (655*
        lmMsq)/72. + lmMst1*(65.86121882086168 - (70*lmMsq)/9. + (5*pow2(lmMsq)
        )/12.) + (25*pow2(lmMsq))/12. - lmMst2*(33.61121882086168 + (99767*
        lmMst1)/3780. - (5*lmMsq*(13 + 9*lmMst1))/18. + (5*pow2(lmMsq))/12. - (
        305*pow2(lmMst1))/36.) + (27.599470899470898 - (5*lmMsq)/3.)*pow2(
        lmMst1) + (7.516137566137566 - (5*lmMsq)/6. - (473*lmMst1)/36.)*pow2(
        lmMst2) - (10*pow3(lmMst1))/27. + (136*pow3(lmMst2))/27. + (Dmglst2*(
        84.40344866031853 - (5*lmMsq*(2 + 3*lmMst1))/3. + (1011403*lmMst2)/
        1.1907e6 + (5*pow2(lmMsq))/2. + (4.326984126984127 + (11*lmMst2)/3.)*
        pow2(lmMst1) + lmMst1*(10.650581170739901 + (1474*lmMst2)/315. - (11*
        pow2(lmMst2))/3.) - (113*pow2(lmMst2))/35. - (11*pow3(lmMst1))/9. + (
        11*pow3(lmMst2))/9.))/Mst2)*pow4(Mst1) + (pow3(Mst2)*(Mst2*(-82320*(
        1318 - 15*lmMsq*(23 + 41*lmMst1 - 41*lmMst2) - 346*lmMst2 + lmMst1*(691
        + 525*lmMst2) + 45*pow2(lmMst1) - 570*pow2(lmMst2))*pow2(Msq)*pow2(
        Mst1) + 34300*(9403 - 720*B4 + 72*D3 - 36*DN - 630*lmMsq - 270*pow2(
        lmMsq) - 3*lmMst1*(5663 - 1950*lmMsq + 630*pow2(lmMsq)) - 954*pow2(
        lmMst1) + 3*lmMst2*(6377 - 6942*lmMst1 + 30*lmMsq*(-59 + 42*lmMst1) +
        630*pow2(lmMsq) + 234*pow2(lmMst1)) - 18*(-1172 + 210*lmMsq + 399*
        lmMst1)*pow2(lmMst2) - 288*pow3(lmMst1) + 6768*pow3(lmMst2))*pow4(Msq)
        + 3*(6074157 - 420*lmMsq*(9224 + 5705*lmMst1 - 6545*lmMst2) + 1208165*
        lmMst2 + 35*lmMst1*(76169 + 108780*lmMst2) - 176400*pow2(lmMsq) -
        705600*pow2(lmMst1) - 3278100*pow2(lmMst2))*pow4(Mst1)) + 17150*
        Dmglst2*(240*(-77 + lmMsq*(6 + 60*lmMst1 - 60*lmMst2) + 50*lmMst2 - 4*
        lmMst1*(14 + 15*lmMst2) + 60*pow2(lmMst2))*pow2(Msq)*pow2(Mst1) + 2*(
        7337 - 432*B4 + 576*D3 - 360*DN + 8100*lmMsq - 1620*pow2(lmMsq) - 216*
        lmMst1*(-7 + 15*pow2(lmMsq)) - 1404*pow2(lmMst1) + 36*lmMst2*(-689 -
        528*lmMst1 + 90*lmMsq*(1 + 2*lmMst1) + 90*pow2(lmMsq) + 32*pow2(lmMst1)
        ) - 36*(-239 + 180*lmMsq + 336*lmMst1)*pow2(lmMst2) + 10944*pow3(
        lmMst2))*pow4(Msq) + 15*(113 - 34*lmMst2 + 12*lmMsq*(1 - 10*lmMst1 +
        10*lmMst2) + 2*lmMst1*(11 + 60*lmMst2) - 120*pow2(lmMst2))*pow4(Mst1))
        + (51450*Dmglst2*pow2(Mst2)*(-40*(-230 + 6*lmMsq*(16 + 17*lmMst1 - 17*
        lmMst2) + 13*lmMst2 - lmMst1*(109 + 102*lmMst2) + 102*pow2(lmMst2))*
        pow2(Msq)*pow2(Mst1) + 12*(-115 + (494 + 64*lmMst1)*lmMst2 - 30*lmMsq*(
        -1 + 6*lmMst2) + 90*pow2(lmMsq) + 464*pow2(lmMst2))*pow4(Msq) + 5*(-793
        + 12*lmMsq*(19 + 42*lmMst1 - 42*lmMst2) + 6*lmMst2 - 18*lmMst1*(13 +
        28*lmMst2) + 504*pow2(lmMst2))*pow4(Mst1)))/pow2(Mst1)))/(2.22264e7*
        pow4(Msq)) + (9*(2744*OepS2 - 81*(3137 + 686*lmMst1 - 686*lmMst2)*S2)*
        pow2(Mst1)*pow3(Mst2) + 15*Mst2*(1556*OepS2 - 27*(3799 + 1167*lmMst1 -
        1167*lmMst2)*S2)*pow4(Mst1) - 2*Dmglst2*(972*(16*OepS2 + (275 - 324*
        lmMst1 + 324*lmMst2)*S2)*pow2(Mst1)*pow2(Mst2) + (33592*OepS2 - 27*(
        51635 + 25194*lmMst1 - 25194*lmMst2)*S2)*pow4(Mst1) + 27*(56*OepS2 -
        81*(-1677 + 14*lmMst1 - 14*lmMst2)*S2)*pow4(Mst2)) - 1296*(-4*OepS2 +
        81*(22 + lmMst1 - lmMst2)*S2)*pow5(Mst2))/(8748.*Mst2) + (((41160*(2986
        - 30*lmMsq*(43 + 40*lmMst1 - 40*lmMst2) - 735*lmMst2 + 75*lmMst1*(27 +
        16*lmMst2) - 1200*pow2(lmMst2)))/pow2(Msq) + (308700*(405 + 770*lmMst2
        + 64*lmMst1*(1 + lmMst2) - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) +
        336*pow2(lmMst2)))/pow2(Mst1) + ((-45671209 + 420*lmMsq*(51313 + 42630*
        lmMst1 - 44310*lmMst2) - 9501870*lmMst2 - 10290*lmMst1*(1171 + 1740*
        lmMst2) + 352800*pow2(lmMsq) + 18257400*pow2(lmMst2))*pow2(Mst1))/pow4(
        Msq))*pow6(Mst2))/2.22264e7) + pow4(Mt)*(402.71234567901234 + (520*
        lmMsq)/27. + (220*pow2(lmMsq))/9. - (10*lmMst1*(85 - 10*lmMsq + 6*pow2(
        lmMsq)))/9. - (392*pow2(lmMst1))/9. + (4*lmMst2*(722 - 1449*lmMst1 +
        270*lmMsq*lmMst1 - 135*pow2(lmMsq) + 63*pow2(lmMst1)))/81. + (2*lmMt*(
        1417 - 688*lmMst2 + 90*lmMsq*(-9 + 4*lmMst2) + 12*lmMst1*(27 + 32*
        lmMst2) + 18*pow2(lmMst1) - 246*pow2(lmMst2)))/27. - (4*(-803 + 90*
        lmMsq + 189*lmMst1)*pow2(lmMst2))/27. - (4*(-12 + 10*lmMsq + lmMst1 +
        45*lmMst2)*pow2(lmMt))/3. + (4*(22483 + 2250*lmMst2 + 30*lmMst1*(-91 +
        150*lmMst2) - 750*lmMt + 4500*lmMst2*lmMt - 30*lmMsq*(-41 + 60*lmMst1 +
        150*lmMt) + 3150*pow2(lmMsq) - 1350*pow2(lmMst1) - 4500*pow2(lmMst2))*
        pow2(Mst1))/(2025.*pow2(Msq)) + (2*((2*(-7839 - 8660*lmMst2 + 375*
        lmMst1*(7 + 6*lmMst2) + 4875*lmMt + 6000*lmMst2*lmMt - 10*lmMsq*(-116 +
        225*lmMst1 + 285*lmMst2 + 600*lmMt) + 5550*pow2(lmMsq) - 2700*pow2(
        lmMst2)))/pow2(Msq) + (75*(277 + 514*lmMst2 + 64*lmMst1*(1 + lmMst2) -
        30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) + 208*pow2(lmMst2)))/pow2(
        Mst1))*pow2(Mst2))/675. + (40*pow3(lmMsq))/9. - (68*pow3(lmMst1))/9. +
        (pow2(Mst1)*(6906919 - 4511760*lmMst2 - 1800*(373 + 114*lmMst2 - 90*
        lmMt)*pow2(lmMst1) - 352800*pow2(lmMst2) + 162000*lmMsq*(12 + lmMst2*(
        15 - 2*lmMt) - 2*lmMst1*(5 + lmMst2 - lmMt) - 5*lmMt + 2*pow2(lmMst2))
        + 16200*lmMt*(194 + 71*lmMst2 + 42*pow2(lmMst2)) - 259200*(1 + 2*
        lmMst2)*pow2(lmMt) + 120*lmMst1*(3938 + 1215*lmMt - 15*lmMst2*(7 + 468*
        lmMt) + 5940*pow2(lmMst2) + 4320*pow2(lmMt)) + (27000*(39 - 6*lmMst2*(-
        8 + lmMt) - 14*lmMt + lmMst1*(-34 - 6*lmMst2 + 6*lmMt) + 6*pow2(lmMst2)
        )*pow2(Mst1))/pow2(Msq) + 14400*pow3(lmMst1) - 522000*pow3(lmMst2)))/(
        12150.*pow2(Mst2)) + (148*pow3(lmMst2))/9. + (184*pow3(lmMt))/3. + (2*
        Dmglst2*pow2(Mst1)*(23048051 + 10812060*lmMst2 + 12600*(1213 + 285*
        lmMst2 - 225*lmMt)*pow2(lmMst1) - 58646700*pow2(lmMst2) - 2835000*
        lmMsq*(27 + lmMst2*(19 - 2*lmMt) - 2*lmMst1*(6 + lmMst2 - lmMt) - 7*
        lmMt + 2*pow2(lmMst2)) + 31500*lmMt*(-622 - 389*lmMst2 + 6*pow2(lmMst2)
        ) + 1260*lmMst1*(58569 - 1075*lmMt + 5*lmMst2*(6883 + 420*lmMt) + 9300*
        pow2(lmMst2) - 7200*pow2(lmMt)) + 4536000*(3 + 2*lmMst2)*pow2(lmMt) + (
        472500*(-87 + lmMst1*(40 + 6*lmMst2 - 6*lmMt) + 6*lmMst2*(-10 + lmMt) +
        20*lmMt - 6*pow2(lmMst2))*pow2(Mst1))/pow2(Msq) - 252000*pow3(lmMst1) -
        15057000*pow3(lmMst2)))/(212625.*pow3(Mst2)) - (32*Dmglst2*(24*lmMst2*(
        1 + lmMst2) + (5*(14 - 15*lmMsq + 15*lmMst2)*pow2(Mst1))/pow2(Msq))*
        pow3(Mst2))/(27.*pow4(Mst1)) - ((860 - 600*lmMsq + 600*lmMst2)/(27.*
        pow2(Msq)*pow2(Mst1)) + (51.22583486304575 - (220709*lmMsq)/6615. + (5*
        (-41 + 60*lmMsq)*lmMst1)/54. + (44.290854119425546 + (788*lmMsq)/63. -
        (50*lmMst1)/9.)*lmMst2 + (5*(-77 + 228*lmMsq - 228*lmMst2)*lmMt)/54. -
        (1234*pow2(lmMsq))/63. + (446*pow2(lmMst2))/63.)/pow4(Msq) + (64*pow2(1
        + lmMst2))/(9.*pow4(Mst1)))*pow4(Mst2) + pow4(Mst1)*((16.98404168016413
         + (16403*lmMsq)/2205. - (4.383446712018141 + (2*lmMsq)/7.)*lmMst1 + (
        10*lmMst1*lmMst2)/3. - (5*(11 + 12*lmMsq - 12*lmMst2)*lmMt)/18. + (38*
        pow2(lmMsq))/21. - (32*pow2(lmMst1))/21. - (10*pow2(lmMst2))/3.)/pow4(
        Msq) + (1405.4626938375586 + 140*lmMsq + (514.9222171831696 - 520*
        lmMsq)*lmMst1 - (137992*pow2(lmMst1))/945. - lmMst2*(963.0333282942806
         - (550874*lmMst1)/
        945. + 40*lmMsq*(-16 + 3*lmMst1) + (376*pow2(lmMst1))/9.) - (
        579.1343915343915 - 120*lmMsq - (656*lmMst1)/3.)*pow2(lmMst2) + (lmMt*(
        3425 + 1080*lmMsq*(-1 + lmMst1 - lmMst2) + 2618*lmMst2 - 2*lmMst1*(573
        + 1112*lmMst2) + 312*pow2(lmMst1) + 1912*pow2(lmMst2)))/9. + (64*(-1 +
        3*lmMst1 - 3*lmMst2)*pow2(lmMt))/3. + (64*pow3(lmMst1))/27. - (4840*
        pow3(lmMst2))/27.)/pow4(Mst2) - (Dmglst2*(3879.568149934479 + 1840*
        lmMsq + (581.2930880994373 - 2320*lmMsq)*lmMst1 - (126272*pow2(lmMst1))
        /189. - (4*lmMst2*(57911521 - 22348620*lmMst1 + 2381400*lmMsq*(-19 + 3*
        lmMst1) + 2487240*pow2(lmMst1)))/59535. + (16*(-15893 + 5670*lmMsq +
        4284*lmMst1)*pow2(lmMst2))/189. + (16*lmMt*(2105 + 405*lmMsq*(-3 + 2*
        lmMst1 - 2*lmMst2) + 2247*lmMst2 - 3*lmMst1*(341 + 364*lmMst2) + 234*
        pow2(lmMst1) + 858*pow2(lmMst2)))/27. + (128*(-5 + 6*lmMst1 - 6*lmMst2)
        *pow2(lmMt))/3. + (256*pow3(lmMst1))/27. - (5536*pow3(lmMst2))/27.))/
        pow5(Mst2)) + (-945*pow2(MuSUSY)*(144*Dmglst2*pow2(Mst2)*(180 - 2*B4 +
        2*D3 - DN + 16*lmMst1 + 144*lmMst2 - 16*(-2 + lmMst1)*pow2(lmMst2) +
        16*pow3(lmMst2)) + pow2(Mst1)*(3*Mst2*(10667 - 96*B4 + 96*D3 - 48*DN -
        3072*lmMst1 - 384*(-25 + 9*lmMst1)*lmMst2 - 384*(-1 + lmMst1 - lmMst2)*
        lmMt - 384*(-13 + 4*lmMst1)*pow2(lmMst2) + 1536*pow3(lmMst2)) + 2*
        Dmglst2*(28405 - 288*B4 + 288*D3 - 144*DN + 5472*lmMst1 - 288*(-89 + 2*
        lmMst1)*lmMst2 + 576*(-2 + lmMst1 - lmMst2)*lmMt - 576*(-9 + 8*lmMst1)*
        pow2(lmMst2) + 4608*pow3(lmMst2))) + 24*(436 - 6*B4 + 6*D3 - 3*DN - 48*
        lmMst1 + (408 - 96*lmMst1)*lmMst2 + 24*lmMt - 48*(-4 + lmMst1)*pow2(
        lmMst2) + 48*pow3(lmMst2))*pow3(Mst2)) + 560*OepS2*(3*Mst2*(pow2(Mst1)*
        (627*pow2(Mst2) + 378*pow2(MuSUSY)) + 1168*pow4(Mst1) + 189*pow4(Mst2))
        + 4*Dmglst2*(63*pow2(Mst1)*(23*pow2(Mst2) - 3*pow2(MuSUSY)) + 5582*
        pow4(Mst1) + 189*pow4(Mst2))) - 27*S2*(42*Mst2*(9*pow2(Mst1)*((4163 +
        2090*lmMst1 - 2090*lmMst2)*pow2(Mst2) + 90*(-43 + 14*lmMst1 - 14*
        lmMst2)*pow2(MuSUSY)) + 8*(8653 + 4380*lmMst1 - 4380*lmMst2)*pow4(Mst1)
        + 81*(-240*pow2(Mst2)*pow2(MuSUSY) + (109 + 70*lmMst1 - 70*lmMst2)*
        pow4(Mst2))) + 4*Dmglst2*(45*pow2(Mst1)*(23*(115 + 588*lmMst1 - 588*
        lmMst2)*pow2(Mst2) - 126*(65 + 14*lmMst1 - 14*lmMst2)*pow2(MuSUSY)) + (
        2001242 + 2344440*lmMst1 - 2344440*lmMst2)*pow4(Mst1) - 81*(3360*pow2(
        Mst2)*pow2(MuSUSY) + (1593 - 980*lmMst1 + 980*lmMst2)*pow4(Mst2)))))/(
        76545.*pow5(Mst2)) + (350*pow2(Mst1)*(-(Dmglst2*Mst2*(1267 + 1632*
        lmMst2 - 6*lmMst1*(11 + 60*lmMst2) + 78*lmMt - 936*lmMst2*lmMt + 12*
        lmMsq*(-137 + 30*lmMst1 + 48*lmMst2 + 78*lmMt) - 936*pow2(lmMsq) + 360*
        pow2(lmMst2))) + 3*(5 + 15*lmMst1 + 6*(-13 + 6*lmMst1)*lmMst2 + 3*(5 +
        12*lmMst2)*lmMt + 12*lmMsq*(4 - 3*(lmMst1 + lmMt)) + 36*pow2(lmMsq) -
        36*pow2(lmMst2))*pow2(Mst1))*pow3(Mst2) + 4*Dmglst2*(2*(315*(-243 + 2*(
        55 + 32*lmMst1)*lmMst2 - 30*lmMsq*(-1 + 6*lmMst2) + 90*pow2(lmMsq) +
        208*pow2(lmMst2))*pow2(Mst2) + pow2(Mst1)*(496858 - 397915*lmMst2 +
        525*(419 + 208*lmMst2)*lmMt - 1050*lmMsq*(23 + 18*lmMst1 + 6*lmMst2 +
        30*lmMt) + 28350*pow2(lmMsq) - 4410*pow2(lmMst1) - 221970*pow2(lmMst2)
        + 630*lmMst1*(269 + 254*lmMst2 - 64*lmMt + 64*pow2(lmMst2)) + 63000*
        pow2(lmMt) - 40320*pow3(lmMst2)))*pow4(Msq) - 525*(18*lmMt*pow2(Mst1) +
        (-83 + 21*lmMt + 12*lmMsq*(-4 + 3*lmMt) - 36*pow2(lmMsq) + 36*pow2(
        lmMst2))*pow2(Mst2) + 3*lmMst1*(6*pow2(Mst1) + (7 + 12*lmMsq - 12*
        lmMst2)*pow2(Mst2)) + 6*lmMst2*(-6*pow2(Mst1) + (1 - 6*lmMt)*pow2(Mst2)
        ))*pow4(Mst1) - 6300*pow2(Msq)*((5 + 12*lmMst2 - lmMst1*(1 + 6*lmMst2)
        + lmMt - 12*lmMst2*lmMt + 6*lmMsq*(-2 + lmMst1 + lmMst2 + 2*lmMt) - 12*
        pow2(lmMsq) + 6*pow2(lmMst2))*pow2(Mst1)*pow2(Mst2) + (2 + 4*lmMst1 -
        8*lmMst2 + 4*lmMt)*pow4(Mst1))) - 525*(18*Dmglst2*(13 - 28*lmMsq + 28*
        lmMst2) + (67 - 84*lmMsq + 84*lmMst2)*Mst2)*pow6(Mst2))/(5670.*Mst2*
        pow2(Mst1)*pow4(Msq))) - (T1ep*(4*Dmglst2*pow4(Mst1)*(8*pow2(Mt)*pow2(
        s2t)*(4327*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 6921*pow2(Mst2)*pow2(
        Sbeta)) - 205840*Mst2*s2t*pow2(Sbeta)*pow3(Mt) + 15571*Mt*pow2(Sbeta)*
        pow3(Mst2)*pow3(s2t) + 89312*pow2(Sbeta)*pow4(Mt) - 4199*pow2(Sbeta)*
        pow4(Mst2)*pow4(s2t)) - 756*Dmglst2*pow3(Mst2)*(-4*Mst2*pow2(Mt)*pow2(
        MuSUSY)*pow2(s2t)*(-1 + pow2(Sbeta)) + 10*s2t*(pow2(MuSUSY)*(-1 + pow2(
        Sbeta)) + 2*pow2(Mst2)*pow2(Sbeta))*pow3(Mt) - 5*Mt*pow2(Sbeta)*pow3(
        s2t)*pow4(Mst2) - 16*Mst2*pow2(Sbeta)*pow4(Mt) + pow2(Sbeta)*pow4(s2t)*
        pow5(Mst2)) + 324*pow4(Mst2)*(Mst2*pow2(Mt)*pow2(s2t)*(-16*pow2(MuSUSY)
        *(-1 + pow2(Sbeta)) - 23*pow2(Mst2)*pow2(Sbeta)) + 14*s2t*(pow2(MuSUSY)
        *(-1 + pow2(Sbeta)) + 2*pow2(Mst2)*pow2(Sbeta))*pow3(Mt) - 7*Mt*pow2(
        Sbeta)*pow3(s2t)*pow4(Mst2) + 28*Mst2*pow2(Sbeta)*pow4(Mt) + 4*pow2(
        Sbeta)*pow4(s2t)*pow5(Mst2)) + 3*pow4(Mst1)*(-4*Mst2*pow2(Mt)*pow2(s2t)
        *(7357*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 6917*pow2(Mst2)*pow2(Sbeta)) +
        16*s2t*(941*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 2740*pow2(Mst2)*pow2(
        Sbeta))*pow3(Mt) - 1756*Mt*pow2(Sbeta)*pow3(s2t)*pow4(Mst2) + 18688*
        Mst2*pow2(Sbeta)*pow4(Mt) + 1945*pow2(Sbeta)*pow4(s2t)*pow5(Mst2)) -
        72*Dmglst2*pow2(Mst1)*(-12*pow2(Mst2)*pow2(Mt)*pow2(s2t)*(43*pow2(
        MuSUSY)*(-1 + pow2(Sbeta)) + 50*pow2(Mst2)*pow2(Sbeta)) + 2*Mst2*s2t*(
        506*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 1383*pow2(Mst2)*pow2(Sbeta))*
        pow3(Mt) + 56*(3*pow2(MuSUSY)*(-1 + pow2(Sbeta)) - 23*pow2(Mst2)*pow2(
        Sbeta))*pow4(Mt) - 401*Mt*pow2(Sbeta)*pow3(s2t)*pow5(Mst2) + 108*pow2(
        Sbeta)*pow4(s2t)*pow6(Mst2)) + 18*Mst2*pow2(Mst1)*(-4*pow2(Mst2)*pow2(
        Mt)*pow2(s2t)*(487*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 644*pow2(Mst2)*
        pow2(Sbeta)) + 8*Mst2*s2t*(136*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 377*
        pow2(Mst2)*pow2(Sbeta))*pow3(Mt) + 8*(126*pow2(MuSUSY)*(-1 + pow2(
        Sbeta)) + 209*pow2(Mst2)*pow2(Sbeta))*pow4(Mt) - 292*Mt*pow2(Sbeta)*
        pow3(s2t)*pow5(Mst2) + 343*pow2(Sbeta)*pow4(s2t)*pow6(Mst2))))/(1458.*
        pow2(Sbeta)*pow5(Mst2)) + pow2(Mt)*pow2(s2t)*((2*pow2(Mst1)*(517702 -
        2700*D3 + 2700*DN + 10125*lmMsq - 64800*(lmMst1 - lmMst2)*lmMt - 30375*
        pow2(lmMsq) - 15*lmMst1*(43538 - 5850*lmMsq + 675*pow2(lmMsq)) + 15*
        lmMst2*(47993 - 10020*lmMst1 - 450*lmMsq*(4 + 3*lmMst1) + 675*pow2(
        lmMsq) - 1035*pow2(lmMst1)) + 225*(-887 + 90*lmMsq)*pow2(lmMst1) + 225*
        (1195 - 339*lmMst1)*pow2(lmMst2) - 29250*pow3(lmMst1) + 121050*pow3(
        lmMst2)))/6075. + ((2*(5108 + 15*lmMst1*(718 - 75*lmMst2) - 15*lmMsq*(
        218 + 195*lmMst1 - 75*lmMst2) - 7500*lmMst2 + 900*pow2(lmMsq) + 2025*
        pow2(lmMst1)))/(2025.*pow2(Msq)) + (196.55862427463637 + (-
        293.24826908541195 + (40*lmMsq)/3.)*lmMst1 + lmMst2*(305.24826908541195
         - (40*lmMsq)/3. - (32894*lmMst1)/945. - (286*pow2(lmMst1))/9.) - (
        50753*pow2(lmMst1))/945. + (32*lmMt*(-2*lmMst1*(1 + lmMst2) + lmMst2*(2
        + lmMst2) + pow2(lmMst1)))/3. + (88.51534391534392 - (2*lmMst1)/9.)*
        pow2(lmMst2) + (190*pow3(lmMst1))/27. + (674*pow3(lmMst2))/27.)/pow2(
        Mst2) - (Dmglst2*(190.3527287429963 - (160*lmMsq)/3. + (-
        535.9278609221467 + (200*lmMsq)/3.)*lmMst1 + lmMst2*(315.4834164777022
         - (200*lmMsq)/
        3. + (84592*lmMst1)/315. - (208*pow2(lmMst1))/9.) - (35576*pow2(lmMst1)
        )/315. + (8*(-6127 + 5110*lmMst1)*pow2(lmMst2))/315. + (64*lmMt*(2 + 5*
        lmMst2 - lmMst1*(5 + 2*lmMst2) + pow2(lmMst1) + pow2(lmMst2)))/3. + (
        16*pow3(lmMst1))/27. - (2896*pow3(lmMst2))/27.))/pow3(Mst2) + (5*
        Dmglst2*(1 + 4*lmMsq - 4*lmMst2)*Mst2)/(6.*pow4(Msq)))*pow4(Mst1) - (
        Dmglst2*pow3(Mst2)*(40*(107 - 120*lmMsq + 120*lmMst2)*pow2(Msq)*pow2(
        Mst1) + 12*(-179 + 238*lmMst2 + 64*lmMst1*lmMst2 - 30*lmMsq*(-1 + 6*
        lmMst2) + 90*pow2(lmMsq) + 272*pow2(lmMst2))*pow4(Msq) + 5*(-65 + 324*
        lmMsq - 324*lmMst2)*pow4(Mst1)))/(54.*pow2(Mst1)*pow4(Msq)) + pow2(
        Mst2)*(252.5348765432099 + (8*D3)/9. - (8*DN)/9. - (100*lmMsq)/3. + (
        32*lmMt)/3. + 20*pow2(lmMsq) + (2*lmMst1*(763 - 300*lmMsq + 45*pow2(
        lmMsq)))/27. + (124*pow2(lmMst1))/9. - (2*lmMst2*(30*lmMsq*(8 + 3*
        lmMst1) + 45*pow2(lmMsq) + 13*(-209 - 30*lmMst1 + 9*pow2(lmMst1))))/27.
         + (2*(256 + 30*lmMsq + 31*lmMst1)*pow2(lmMst2))/9. - (2*(3091 - 10230*
        lmMst2 + 60*lmMst1*(92 + 75*lmMst2) + 30*lmMsq*(157 - 60*(lmMst1 +
        lmMst2)) + 1800*pow2(lmMsq) - 1350*pow2(lmMst1) - 1350*pow2(lmMst2))*
        pow2(Mst1))/(2025.*pow2(Msq)) + (32*pow3(lmMst1))/9. - (16*pow3(lmMst2)
        )/9. - ((2.3647986178598424 - (991*lmMsq)/441. + ((-194 + 385*lmMsq)*
        lmMst1)/735. + (2.511111111111111 - lmMsq + lmMst1)*lmMst2 + (5*pow2(
        lmMsq))/21. - (16*pow2(lmMst1))/21.)*pow4(Mst1))/pow4(Msq)) + (Dmglst2*
        ((2*(225*pow2(Mst2)*(28373 - 240*B4 + 240*D3 - 120*DN + 750*lmMsq +
        1380*lmMst1 + 2700*pow2(lmMsq) + 690*pow2(lmMst1) - 20*lmMst2*(-1676 +
        270*lmMsq - 27*lmMst1 + 48*pow2(lmMst1)) - 30*(-391 + 32*lmMst1)*pow2(
        lmMst2) + 1920*pow3(lmMst2)) + pow2(Mst1)*(2416741 + 1789710*lmMst2 +
        67500*lmMsq*(8 - 3*lmMst1 + 12*lmMst2) - 324000*(2 + lmMst2)*lmMt -
        303750*pow2(lmMsq) + 450*(-341 + 30*lmMst2)*pow2(lmMst1) + 60*lmMst1*(
        34484 - 12885*lmMst2 + 5400*lmMt - 21825*pow2(lmMst2)) + 224550*pow2(
        lmMst2) - 4500*pow3(lmMst1) + 1300500*pow3(lmMst2))))/30375. + (20*(5*(
        3 - 4*lmMsq + 4*lmMst2)*pow2(Mst1)*pow2(Mst2) + 2*(4 - lmMst1 + lmMst2)
        *pow4(Mst1)))/(9.*pow2(Msq))))/Mst2 - (((2*(22642 - 15*lmMsq*(1282 +
        75*lmMst1 - 195*lmMst2) + 16230*lmMst2 + 375*lmMst1*(8 + 3*lmMst2) -
        900*pow2(lmMsq) - 2025*pow2(lmMst2)))/pow2(Msq) + (225*(309 + 578*
        lmMst2 + 64*lmMst1*(1 + lmMst2) - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(
        lmMsq) + 240*pow2(lmMst2)))/pow2(Mst1))*pow4(Mst2))/2025. - ((Dmglst2*(
        18.796296296296298 - 50*lmMsq + 50*lmMst2)*Mst2 + (lmMsq*(
        6.64172335600907 - lmMst1 + (11*lmMst2)/21.) + lmMst1*(1.4 + lmMst2) +
        (5*pow2(lmMsq))/21. - (2*(65299 + 186186*lmMst2 + 17640*pow2(lmMst2)))/
        46305.)*pow2(Mst1))*pow4(Mst2))/pow4(Msq) + (16*Dmglst2*(24*lmMst2*(1 +
        lmMst2) + (5*(14 - 15*lmMsq + 15*lmMst2)*pow2(Mst1))/pow2(Msq))*pow5(
        Mst2))/(27.*pow4(Mst1)) - pow2(MuSUSY)*(97.70987654320987 - (40*B4)/9.
         + (4*D3)/
        9. - (2*DN)/9. - (185*lmMsq)/9. + (25*pow2(lmMsq))/3. - (lmMst1*(5279 -
        1950*lmMsq + 630*pow2(lmMsq)))/54. - (53*pow2(lmMst1))/9. + (lmMst2*(
        10421 - 6558*lmMst1 + 30*lmMsq*(-95 + 42*lmMst1) + 630*pow2(lmMsq) +
        234*pow2(lmMst1)))/54. - ((-1460 + 210*lmMsq + 399*lmMst1)*pow2(lmMst2)
        )/9. + ((111 - 3028*lmMst2 + 90*lmMsq*(4 - 13*lmMst1 + 13*lmMst2) + 2*
        lmMst1*(1334 + 675*lmMst2) - 90*pow2(lmMst1) - 1260*pow2(lmMst2))*pow2(
        Mst1))/(135.*pow2(Msq)) + (((2*(836 - 2235*lmMst2 + 75*lmMst1*(27 + 16*
        lmMst2) + 30*lmMsq*(7 - 40*lmMst1 + 40*lmMst2) - 1200*pow2(lmMst2)))/
        pow2(Msq) + (15*(341 + 642*lmMst2 + 64*lmMst1*(1 + lmMst2) - 30*lmMsq*(
        5 + 6*lmMst2) + 90*pow2(lmMsq) + 272*pow2(lmMst2)))/pow2(Mst1))*pow2(
        Mst2))/270. - (16*pow3(lmMst1))/9. + (376*pow3(lmMst2))/9. + (pow2(
        Mst1)*(-(Mst2*(4962073 + 205200*B4 - 5400*DN - 405000*lmMsq + 120*
        lmMst1*(57971 - 14625*lmMsq + 2700*pow2(lmMsq)) + 900*(-1331 + 180*
        lmMsq)*pow2(lmMst1) - 120*lmMst2*(52436 - 70455*lmMst1 + 225*lmMsq*(-65
        + 36*lmMst1) + 2700*pow2(lmMsq) + 8280*pow2(lmMst1)) + 900*(-8063 +
        900*lmMsq + 3792*lmMst1)*pow2(lmMst2) + (90*(-1725 + lmMst1*(-7006 +
        2640*lmMsq - 4800*lmMst2) + (7006 - 2640*lmMsq)*lmMst2 + 1080*pow2(
        lmMst1) + 3720*pow2(lmMst2))*pow2(Mst1))/pow2(Msq) - 55800*pow3(lmMst1)
        - 2363400*pow3(lmMst2)))/24300. + Dmglst2*(109.11799176954733 - (8*B4)/
        3. + (32*D3)/9. - (20*DN)/9. + 80*lmMsq - lmMst1*(78.19061728395062 +
        20*pow2(lmMsq)) - (2888*pow2(lmMst1))/135. + lmMst2*(40*lmMsq*lmMst1 +
        20*pow2(lmMsq) + (4*(-21616 - 64515*lmMst1 + 31275*pow2(lmMst1)))/2025.
        ) - (4*(-5023 + 1350*lmMsq + 6285*lmMst1)*pow2(lmMst2))/135. + (5*(291
        + 2*(-103 + 84*lmMsq)*lmMst2 + lmMst1*(206 - 168*lmMsq + 168*lmMst2) -
        168*pow2(lmMst2))*pow2(Mst1))/(27.*pow2(Msq)) + (20*pow3(lmMst1))/27. +
        (3340*pow3(lmMst2))/27.)))/pow3(Mst2) - (8*Dmglst2*(24*lmMst2*(1 +
        lmMst2) + (5*(14 - 15*lmMsq + 15*lmMst2)*pow2(Mst1))/pow2(Msq))*pow3(
        Mst2))/(27.*pow4(Mst1)) + ((5*(-43 + 30*lmMsq - 30*lmMst2))/(27.*pow2(
        Msq)*pow2(Mst1)) - (-41220947 + 420*lmMsq*(12479 + 69090*lmMst1 -
        69930*lmMst2) + 21234990*lmMst2 - 10290*lmMst1*(2573 + 2820*lmMst2) +
        176400*pow2(lmMsq) + 29194200*pow2(lmMst2))/(1.11132e7*pow4(Msq)) - (
        16*pow2(1 + lmMst2))/(9.*pow4(Mst1)))*pow4(Mst2) + pow4(Mst1)*((
        1.0702990137854083 - (4519*lmMst2)/980. + lmMsq*(0.36171579743008314 -
        (169*lmMst1)/63. + (19*lmMst2)/7.) + lmMst1*(4.249508692365835 + (31*
        lmMst2)/9.) - pow2(lmMsq)/63. - (8*pow2(lmMst1))/21. - (194*pow2(
        lmMst2))/63.)/pow4(Msq) - (363.3804294212688 + (76*B4)/9. - (2*DN)/9. -
        (35*lmMsq)/2. + lmMst1*(211.3489518770471 - (695*lmMsq)/9. + (40*pow2(
        lmMsq))/3.) - (214.87936507936507 - 20*lmMsq)*pow2(lmMst1) - lmMst2*(
        190.46006298815823 - (71398*lmMst1)/105. + (5*lmMsq*(-139 + 120*lmMst1)
        )/9. + (40*pow2(lmMsq))/3. + (334*pow2(lmMst1))/3.) + ((-146507 +
        14700*lmMsq + 91070*lmMst1)*pow2(lmMst2))/315. - (44*pow3(lmMst1))/9. -
        (1556*pow3(lmMst2))/9.)/pow4(Mst2) + (Dmglst2*(536.1152102791342 - (8*
        B4)/3. + (32*D3)/9. - (20*DN)/9. + 90*lmMsq - (123.11224321827497 + 20*
        lmMsq*(1 + lmMsq))*lmMst1 - lmMst2*(17.33220122616948 - 20*lmMsq*(1 +
        lmMsq) + (133.04550264550264 - 40*lmMsq)*lmMst1 - (1180*pow2(lmMst1))/
        9.) - (15886*pow2(lmMst1))/945. + (149.85608465608465 - 40*lmMsq - (
        2812*lmMst1)/9.)*pow2(lmMst2) - (92*pow3(lmMst1))/27. + (4988*pow3(
        lmMst2))/27.))/pow5(Mst2)) + (((257250*Dmglst2*Mst2*(557 - 224*lmMst2 +
        12*lmMsq*(1 - 32*lmMst1 + 32*lmMst2) + 4*lmMst1*(53 + 96*lmMst2) - 384*
        pow2(lmMst2)) + (4167613 - 19932360*lmMst2 + 20580*lmMst1*(701 + 540*
        lmMst2) + 420*lmMsq*(13109 - 26460*lmMst1 + 25620*lmMst2) + 176400*
        pow2(lmMsq) - 10936800*pow2(lmMst2))*pow2(Mst1))*pow2(Mst2))/5.5566e6 -
        (5*(18*Dmglst2*(13 - 28*lmMsq + 28*lmMst2) + (67 - 84*lmMsq + 84*
        lmMst2)*Mst2)*pow5(Mst2))/(216.*pow2(Mst1)) + (Dmglst2*((18*(-115 + (
        366 + 64*lmMst1)*lmMst2 - 30*lmMsq*(-1 + 6*lmMst2) + 90*pow2(lmMsq) +
        336*pow2(lmMst2))*pow2(Mst2) + pow2(Mst1)*(3197 - 432*B4 + 576*D3 -
        360*DN + 9180*lmMsq + 1620*pow2(lmMsq) - 216*lmMst1*(-7 + 15*pow2(
        lmMsq)) - 1404*pow2(lmMst1) + 36*lmMst2*(-291 - 464*lmMst1 + 90*lmMsq*(
        -1 + 2*lmMst1) + 90*pow2(lmMsq) + 32*pow2(lmMst1)) - 36*(-607 + 180*
        lmMsq + 336*lmMst1)*pow2(lmMst2) + 10944*pow3(lmMst2)))*pow4(Msq) + 60*
        pow2(Msq)*((118 + 109*lmMst1 + (-133 + 102*lmMst1)*lmMst2 + 6*lmMsq*(4
        - 17*lmMst1 + 17*lmMst2) - 102*pow2(lmMst2))*pow2(Mst1)*pow2(Mst2) + 2*
        (69 + (-53 + 42*lmMsq)*lmMst2 + lmMst1*(53 - 42*lmMsq + 42*lmMst2) -
        42*pow2(lmMst2))*pow4(Mst1)) + 15*((219 - 95*lmMst2 + 132*lmMsq*lmMst2
        + lmMst1*(95 - 132*lmMsq + 132*lmMst2) - 132*pow2(lmMst2))*pow2(Mst2)*
        pow4(Mst1) + (216 - 95*lmMst2 + 132*lmMsq*lmMst2 + lmMst1*(95 - 132*
        lmMsq + 132*lmMst2) - 132*pow2(lmMst2))*pow6(Mst1))))/(162.*Mst2*pow2(
        Mst1)))/pow4(Msq)) - (((17150*(-43 + 30*lmMsq - 30*lmMst2))/(pow2(Msq)*
        pow2(Mst1)) + (242073 - 105*lmMsq*(3909 + 49*lmMst1 - 259*lmMst2) +
        399812*lmMst2 + 343*lmMst1*(31 + 15*lmMst2) - 11025*pow2(lmMsq) -
        16170*pow2(lmMst2))/pow4(Msq) - (164640*pow2(1 + lmMst2))/pow4(Mst1))*
        pow6(Mst2))/46305. + (-3*Mst2*(6*pow2(Mst1)*pow2(Mst2)*((25760*OepS2 -
        162*(4783 + 3220*lmMst1 - 3220*lmMst2)*S2)*pow2(Mst2) + 5*(3896*OepS2 -
        81*(9473 + 974*lmMst1 - 974*lmMst2)*S2)*pow2(MuSUSY)) + ((276680*OepS2
        - 27*(406627 + 207510*lmMst1 - 207510*lmMst2)*S2)*pow2(Mst2) + 10*(
        29428*OepS2 - 27*(160997 + 22071*lmMst1 - 22071*lmMst2)*S2)*pow2(
        MuSUSY))*pow4(Mst1) + 27*((920*OepS2 - 81*(-891 + 230*lmMst1 - 230*
        lmMst2)*S2)*pow2(Mst2) + 160*(4*OepS2 - 81*(22 + lmMst1 - lmMst2)*S2)*
        pow2(MuSUSY))*pow4(Mst2)) + 4*Dmglst2*(54*pow2(Mst1)*pow2(Mst2)*((2000*
        OepS2 - 162*(31 + 250*lmMst1 - 250*lmMst2)*S2)*pow2(Mst2) + 5*(344*
        OepS2 + 9*(15643 - 774*lmMst1 + 774*lmMst2)*S2)*pow2(MuSUSY)) + 2*(9*(
        30760*OepS2 - 27*(28283 + 23070*lmMst1 - 23070*lmMst2)*S2)*pow2(Mst2) +
        10*(17308*OepS2 + 27*(93919 - 12981*lmMst1 + 12981*lmMst2)*S2)*pow2(
        MuSUSY))*pow4(Mst1) + 135*(56*OepS2 - 81*(-1677 + 14*lmMst1 - 14*
        lmMst2)*S2)*pow2(MuSUSY)*pow4(Mst2) - 2768742*S2*pow6(Mst2)))/(21870.*
        pow5(Mst2))) + (MuSUSY*((pow4(Mt)*(24*Mst2*pow2(Mst1)*(-3*Dmglst2*(
        93487 + 196530*lmMst2 - 9920*OepS2 - 72*(-601 + 2790*lmMst2)*S2 + 2700*
        (lmMst1 - lmMst2)*pow2(lmMsq) - 360*(23 + 8*lmMst2 - 4*lmMt)*pow2(
        lmMst1) + 68040*pow2(lmMst2) + 300*lmMsq*(29 + lmMst1*(9 - 18*lmMst2) +
        6*lmMst2 - 15*lmMt + 18*pow2(lmMst2)) + 30*lmMt*(301 - 246*lmMst2 + 48*
        pow2(lmMst2)) - 4320*(2 + lmMst2)*pow2(lmMt) + 20*lmMst1*(-5846 + 321*
        lmMt - 12*lmMst2*(209 + 12*lmMt) + 10044*S2 + 576*pow2(lmMst2) + 216*
        pow2(lmMt)) - 8640*pow3(lmMst2)) + 2*Mst2*(71744 - 28755*lmMst2 - 4400*
        OepS2 - 162*(-457 + 550*lmMst2)*S2 - 4050*(lmMst1 - lmMst2)*pow2(lmMsq)
        + 540*(-15 + 4*lmMt)*pow2(lmMst1) + 4050*lmMsq*(11 - lmMst1 + 2*(1 +
        lmMst1)*lmMst2 - lmMt - 2*pow2(lmMst2)) + 89910*pow2(lmMst2) - 135*
        lmMt*(113 + 132*lmMst2 + 48*pow2(lmMst2)) - 270*lmMst1*(221 + lmMst2*(
        279 - 16*lmMt) - 66*lmMt - 330*S2 + 112*pow2(lmMst2) - 24*pow2(lmMt)) -
        6480*(1 + lmMst2)*pow2(lmMt) + 30240*pow3(lmMst2))) + 9*(-5*Dmglst2*(
        7335 + 169856*lmMst2 - 1120*OepS2 - 1620*(27 + 14*lmMst2)*S2 + 4320*(
        lmMst1 - lmMst2)*pow2(lmMsq) + 2304*(-1 + lmMst2)*pow2(lmMst1) + 21984*
        pow2(lmMst2) + 480*lmMsq*(-79 - 30*lmMst2 - 9*lmMst1*(-3 + 2*lmMst2) +
        3*lmMt + 18*pow2(lmMst2)) + 48*lmMt*(31 - 128*lmMst2 + 96*pow2(lmMst2))
        + 72*lmMst1*(-538 + 64*lmMt - 4*lmMst2*(63 + 16*lmMt) + 315*S2 + 224*
        pow2(lmMst2)) - 18432*pow3(lmMst2)) + 3*Mst2*(2439 - 10080*lmMst2 -
        1120*OepS2 - 324*(79 + 70*lmMst2)*S2 - 7200*(lmMst1 - lmMst2)*pow2(
        lmMsq) - 3840*(1 + lmMst2)*pow2(lmMst1) + 7200*lmMsq*(7 + lmMst1 - 2*
        lmMst2 + 2*lmMst1*lmMst2 + lmMt - 2*pow2(lmMst2)) + 92640*pow2(lmMst2)
        - 240*lmMt*(175 + 96*lmMst2 + 32*pow2(lmMst2)) - 120*lmMst1*(334 + 516*
        lmMst2 - 64*(1 + lmMst2)*lmMt - 189*S2 + 224*pow2(lmMst2)) - 11520*
        pow2(lmMt) + 30720*pow3(lmMst2)))*pow3(Mst2) + 20*(567143 - 125172*
        lmMst2 + 648*lmMst2*(-38 + 5*lmMst2)*lmMt - 9720*(lmMsq*(lmMst1*(9 - 2*
        lmMt) + 2*lmMst2*(-6 + lmMt) + 3*(-4 + lmMt)) + lmMt) - 32480*OepS2 -
        216*(-4082 + 3045*lmMst2)*S2 - 9720*(lmMst1 - lmMst2)*pow2(lmMsq) +
        108*(-457 + 12*lmMst2 + 126*lmMt)*pow2(lmMst1) + 240300*pow2(lmMst2) -
        108*lmMst1*(2111 - 420*lmMt + 4*lmMst2*(454 + 39*lmMt) - 6090*S2 + 750*
        pow2(lmMst2) - 288*pow2(lmMt)) - 15552*(1 + 2*lmMst2)*pow2(lmMt) + 216*
        pow3(lmMst1) + 79488*pow3(lmMst2))*pow4(Mst1) + (207360*(1 + lmMst2)*(-
        Dmglst2 + 3*Dmglst2*lmMst2 + Mst2 + lmMst2*Mst2)*pow5(Mst2))/pow2(Mst1)
        + (7200*Mst2*(9*(19 + (-8 + 6*lmMsq)*lmMst2 + lmMst1*(7 - 6*lmMsq + 6*
        lmMst2) + lmMt - 6*pow2(lmMst2))*pow2(Mst1)*pow3(Mst2) - 9*Mst2*(-25 +
        lmMst1*(-5 + 6*lmMsq - 6*lmMst2) + (4 - 6*lmMsq)*lmMst2 + lmMt + 6*
        pow2(lmMst2))*pow4(Mst1) + Dmglst2*((439 + 6*(-8 + 21*lmMsq)*lmMst2 +
        3*lmMst1*(17 - 42*lmMsq + 42*lmMst2) - 3*lmMt - 126*pow2(lmMst2))*pow2(
        Mst1)*pow2(Mst2) + (313 + 42*(-2 + 3*lmMsq)*lmMst2 + 3*lmMst1*(23 - 42*
        lmMsq + 42*lmMst2) + 15*lmMt - 126*pow2(lmMst2))*pow4(Mst1) + 6*(64 -
        20*lmMst2 + 6*lmMst1*(2 + 3*lmMst2) + 3*lmMt - 3*lmMst2*lmMt + lmMsq*(5
        - 18*lmMst1 + 21*lmMst2 + 3*lmMt) - 3*pow2(lmMsq) - 18*pow2(lmMst2))*
        pow4(Mst2)) + 2*(58 - 36*lmMst2 + 6*lmMst1*(4 + 3*lmMst2) - 3*lmMt - 9*
        lmMst2*lmMt + 3*lmMsq*(5 - 6*lmMst1 + 9*lmMst2 + 3*lmMt) - 9*pow2(
        lmMsq) - 18*pow2(lmMst2))*pow5(Mst2)))/pow2(Msq) - (450*pow3(Mst2)*((-
        1271 + 216*lmMst2 - 6*lmMst1*(41 + 60*lmMst2) + 12*lmMsq*(5 + 30*lmMst1
        - 36*lmMst2 - 6*lmMt) - 30*lmMt + 72*lmMst2*lmMt + 72*pow2(lmMsq) +
        360*pow2(lmMst2))*pow2(Mst1)*pow3(Mst2) + 36*Mst2*(-38 + lmMst1*(-7 +
        12*lmMsq - 12*lmMst2) + (8 - 12*lmMsq)*lmMst2 - lmMt + 12*pow2(lmMst2))
        *pow4(Mst1) + Dmglst2*(3*(-1693 + 136*lmMst2 - 6*lmMst1*(19 + 84*
        lmMst2) + 4*lmMsq*(5 + 126*lmMst1 - 132*lmMst2 - 6*lmMt) - 42*lmMt +
        24*lmMst2*lmMt + 24*pow2(lmMsq) + 504*pow2(lmMst2))*pow2(Mst1)*pow2(
        Mst2) + 4*(-1330 + lmMst1*(-51 + 396*lmMsq - 396*lmMst2) + (48 - 396*
        lmMsq)*lmMst2 + 3*lmMt + 396*pow2(lmMst2))*pow4(Mst1) + 3*(-1535 + 184*
        lmMst2 - 18*lmMst1*(7 + 20*lmMst2) + 4*lmMsq*(-1 + 90*lmMst1 - 132*
        lmMst2 - 42*lmMt) - 54*lmMt + 168*lmMst2*lmMt + 168*pow2(lmMsq) + 360*
        pow2(lmMst2))*pow4(Mst2)) + 3*(-403 + 24*lmMst2 - 18*lmMst1*(3 + 4*
        lmMst2) + 12*lmMsq*(1 + 6*lmMst1 - 12*lmMst2 - 6*lmMt) + 18*lmMt + 72*
        lmMst2*lmMt + 72*pow2(lmMsq) + 72*pow2(lmMst2))*pow5(Mst2)))/pow4(Msq))
        )/(7290.*pow5(Mst2)) - pow2(Mt)*pow2(s2t)*((pow2(Mst1)*(Mst2*(
        794.5756172839506 + 96*B4 - 120*lmMsq - 10*lmMst1*(43 - 4*lmMsq + 4*
        pow2(lmMsq)) - (56*pow2(lmMst1))/3. + (2*lmMst2*(901 - 780*lmMst1 + 60*
        lmMsq*(-1 + 2*lmMst1) + 60*pow2(lmMsq) + 32*pow2(lmMst1)))/3. - (16*(-
        101 + 15*lmMsq + 44*lmMst1)*pow2(lmMst2))/3. - (40*(9 + 4*lmMst1*(-4 +
        3*lmMsq - 3*lmMst2) + 4*(4 - 3*lmMsq)*lmMst2 + 12*pow2(lmMst2))*pow2(
        Mst1))/(9.*pow2(Msq)) + (640*pow3(lmMst2))/3.) + Dmglst2*(
        600.2756172839506 + (592*B4)/3. - (8*DN)/3. - 680*lmMsq + lmMst1*(
        81.11111111111111 - 120*lmMsq - 40*pow2(lmMsq)) + (56*pow2(lmMst1))/3.
         + lmMst2*(40*lmMsq*(3 + lmMsq + 2*lmMst1) + (2*(3859 - 756*lmMst1 +
        96*pow2(lmMst1)))/9.) - (16*(-28 + 15*lmMsq + 60*lmMst1)*pow2(lmMst2))/
        3. - (80*(15 + lmMst1*(-4 + 6*lmMsq - 6*lmMst2) + (4 - 6*lmMsq)*lmMst2
        + 6*pow2(lmMst2))*pow2(Mst1))/(3.*pow2(Msq)) + (896*pow3(lmMst2))/3.)))
        /pow2(Mst2) + pow3(Mst2)*((-4*((5*(71 + 12*lmMsq*(-2 + lmMst1 - lmMst2)
        + 40*lmMst2 - 4*lmMst1*(4 + 3*lmMst2) + 12*pow2(lmMst2)))/pow2(Msq) - (
        48*pow2(1 + lmMst2))/pow2(Mst1)))/9. - (5*(Mst2*(Mst2*(911 + 12*lmMsq*(
        -37 + 18*lmMst1 - 18*lmMst2) + 606*lmMst2 - 54*lmMst1*(3 + 4*lmMst2) +
        216*pow2(lmMst2)) + Dmglst2*(5591 + 12*lmMsq*(-181 + 90*lmMst1 - 90*
        lmMst2) + 2550*lmMst2 - 54*lmMst1*(7 + 20*lmMst2) + 1080*pow2(lmMst2)))
        + (242 + 24*lmMsq*(1 + 18*lmMst1 - 18*lmMst2) + 300*lmMst2 - 108*
        lmMst1*(3 + 4*lmMst2) + 432*pow2(lmMst2))*pow2(Mst1)))/(108.*pow4(Msq))
        ) + ((1073.5195473251028 + 96*B4 - 110*lmMsq - lmMst1*(
        457.55555555555554 - 20*lmMsq + 40*pow2(lmMsq)) + (652*pow2(lmMst1))/9.
         + (2*lmMst2*(2635 - 3160*lmMst1 + 90*lmMsq*(-1 + 4*lmMst1) + 180*pow2(
        lmMsq) + 528*pow2(lmMst1)))/9. + (629.7777777777778 - 80*lmMsq - 416*
        lmMst1)*pow2(lmMst2) - (32*pow3(lmMst1))/9. + (2720*pow3(lmMst2))/9.)*
        pow4(Mst1))/pow3(Mst2) - (Dmglst2*pow2(Mst2)*(360*(95 + 12*lmMsq*(-2 +
        lmMst1 - lmMst2) + 32*lmMst2 - 4*lmMst1*(2 + 3*lmMst2) + 12*pow2(
        lmMst2))*pow2(Msq)*pow2(Mst1) - 1152*(-1 + 2*lmMst2 + 3*pow2(lmMst2))*
        pow4(Msq) + 5*(1561 + 12*lmMsq*(1 + 90*lmMst1 - 90*lmMst2) + 366*lmMst2
        - 54*lmMst1*(7 + 20*lmMst2) + 1080*pow2(lmMst2))*pow4(Mst1)))/(54.*
        pow2(Mst1)*pow4(Msq)) + Mst2*(737.0416666666666 + (220*B4)/3. - (2*DN)/
        3. - 240*lmMsq + 40*pow2(lmMsq) - (2*lmMst1*(199 - 30*lmMsq + 30*pow2(
        lmMsq)))/3. + (2*lmMst2*(1227 - 322*lmMst1 + 30*lmMsq*(-5 + 2*lmMst1) +
        30*pow2(lmMsq) - 16*pow2(lmMst1)))/3. - (32*pow2(lmMst1))/3. - 4*(-99 +
        10*lmMsq + 24*lmMst1)*pow2(lmMst2) - (20*(21 + 8*lmMst1*(-4 + 3*lmMsq -
        3*lmMst2) + 8*(4 - 3*lmMsq)*lmMst2 + 24*pow2(lmMst2))*pow2(Mst1))/(9.*
        pow2(Msq)) + (320*pow3(lmMst2))/3. + ((5*lmMst1*(3 + 4*lmMst2) + lmMsq*
        (0.5555555555555556 - 20*lmMst1 + 20*lmMst2) - (5*(245 + 336*lmMst2 +
        432*pow2(lmMst2)))/108.)*pow4(Mst1))/pow4(Msq)) + Dmglst2*(
        743.4787037037037 + (524*B4)/3. - (10*DN)/3. - 600*lmMsq + lmMst1*(
        115.33333333333333 - 60*lmMsq - 20*pow2(lmMsq)) + 40*pow2(lmMsq) + (2*
        lmMst2*(1559 - 130*lmMst1 + 30*lmMsq*(-1 + 2*lmMst1) + 30*pow2(lmMsq) -
        16*pow2(lmMst1)))/3. + (32*pow2(lmMst1))/3. - (4*(-217 + 30*lmMsq +
        136*lmMst1)*pow2(lmMst2))/3. - (20*(59 + 8*lmMst1*(-2 + 3*lmMsq - 3*
        lmMst2) + 8*(2 - 3*lmMsq)*lmMst2 + 24*pow2(lmMst2))*pow2(Mst1))/(3.*
        pow2(Msq)) + 192*pow3(lmMst2) - pow4(Mst1)*((141.34259259259258 - (5*
        lmMsq)/9. + 5*(-7 + 20*lmMsq)*lmMst1 + (35.55555555555556 - 100*(lmMsq
        + lmMst1))*lmMst2 + 100*pow2(lmMst2))/pow4(Msq) + (618.5941700960219 -
        (592*B4)/3. + (8*DN)/3. + 750*lmMsq + (20*lmMst1*(542 + 243*lmMsq +
        162*pow2(lmMsq)))/81. + (92*pow2(lmMst1))/3. - lmMst2*(20*lmMsq*(3 + 4*
        lmMst1) + 40*pow2(lmMsq) + (8*(11723 - 702*lmMst1 + 756*pow2(lmMst1)))/
        81.) + (4*(-75 + 60*lmMsq + 344*lmMst1)*pow2(lmMst2))/3. - (32*pow3(
        lmMst1))/3. - (1120*pow3(lmMst2))/3.)/pow4(Mst2))) + S2*((9*(-1 + 14*
        lmMst1 - 14*lmMst2)*Mst2)/2. - ((Dmglst2*(799.6333333333333 + 907*
        lmMst1 - 907*lmMst2) - ((685 + 418*lmMst1 - 418*lmMst2)*Mst2)/2.)*pow2(
        Mst1))/pow2(Mst2) + ((6143 + 3198*lmMst1 - 3198*lmMst2)*pow4(Mst1))/(9.
        *pow3(Mst2)) + Dmglst2*(135.9 - 105*lmMst1 + 105*lmMst2 - ((525961 +
        356010*lmMst1 - 356010*lmMst2)*pow4(Mst1))/(135.*pow4(Mst2)))) + (4*
        OepS2*(Dmglst2*(8163*pow2(Mst1)*pow2(Mst2) + 23734*pow4(Mst1) + 945*
        pow4(Mst2)) - 3*(627*pow2(Mst1)*pow3(Mst2) + 1066*Mst2*pow4(Mst1) +
        189*pow5(Mst2))))/(729.*pow4(Mst2))) - s2t*pow3(Mt)*(454.18086419753087
         + (16*D3)/9. - (16*DN)/9. - (100*lmMsq)/3. + (64*lmMt)/3. + 20*pow2(
        lmMsq) + (4*lmMst1*(667 - 300*lmMsq + 45*pow2(lmMsq)))/27. + (4*lmMst2*
        (1946 + lmMsq*(30 - 90*lmMst1) + 294*lmMst1 - 45*pow2(lmMsq) - 117*
        pow2(lmMst1)))/27. + (248*pow2(lmMst1))/9. + (4*(152 + 30*lmMsq + 31*
        lmMst1)*pow2(lmMst2))/9. - (4*(9608 - 15*lmMsq*(218 + 195*lmMst1 - 75*
        lmMst2) - 5250*lmMst2 + 15*lmMst1*(568 + 375*lmMst2) + 900*pow2(lmMsq)
        - 1350*pow2(lmMst1) - 3375*pow2(lmMst2))*pow2(Mst1))/(2025.*pow2(Msq))
        - (2*((2*(6517 - 15*lmMsq*(532 + 75*lmMst1 - 195*lmMst2) + 4980*lmMst2
        + 375*lmMst1*(8 + 3*lmMst2) - 900*pow2(lmMsq) - 2025*pow2(lmMst2)))/
        pow2(Msq) + (225*(277 + 514*lmMst2 + 64*lmMst1*(1 + lmMst2) - 30*lmMsq*
        (5 + 6*lmMst2) + 90*pow2(lmMsq) + 208*pow2(lmMst2)))/pow2(Mst1))*pow2(
        Mst2))/2025. + (64*pow3(lmMst1))/9. - (32*pow3(lmMst2))/9. + pow2(Mst1)
        *((19319827 + 19042320*lmMst2 + 518400*lmMt + 1036800*lmMst2*lmMt -
        3600*(701 + 186*lmMst2)*pow2(lmMst1) + 5943600*pow2(lmMst2) + 324000*
        lmMsq*(-2 + lmMst1 - lmMst2 - 2*lmMst1*lmMst2 + pow2(lmMst1) + pow2(
        lmMst2)) - 240*lmMst1*(35693 + 5610*lmMst2 + 4320*lmMt + 3690*pow2(
        lmMst2)) + (54000*(-4 + lmMst1*(2 - 6*lmMst2) - 2*lmMst2 + 3*pow2(
        lmMst1) + 3*pow2(lmMst2))*pow2(Mst1))/pow2(Msq) - 295200*pow3(lmMst1) +
        1850400*pow3(lmMst2))/(24300.*pow2(Mst2)) + (Dmglst2*(1195.824329218107
         - (64*B4)/9. + (64*D3)/9. - (32*DN)/9. + 80*lmMsq + (334.6883950617284
         - (80*lmMsq)/3.)*lmMst1 + (128*(-2 + lmMst1 - lmMst2)*lmMt)/3. + (16*
        lmMst2*(143041 + 3375*lmMsq - 14460*lmMst1 - 3375*pow2(lmMst1)))/2025.
         + (32*pow2(lmMst1))/
        135. + (16*(2402 - 1695*lmMst1)*pow2(lmMst2))/135. + (80*(3 - lmMst1 +
        lmMst2)*pow2(Mst1))/(9.*pow2(Msq)) - (16*pow3(lmMst1))/27. + (6160*
        pow3(lmMst2))/27.))/pow3(Mst2)) + (32*Dmglst2*(24*lmMst2*(1 + lmMst2) +
        (5*(14 - 15*lmMsq + 15*lmMst2)*pow2(Mst1))/pow2(Msq))*pow3(Mst2))/(27.*
        pow4(Mst1)) - (((68600*(-43 + 30*lmMsq - 30*lmMst2))/(pow2(Msq)*pow2(
        Mst1)) + (393767 - 420*lmMsq*(2194 + 49*lmMst1 - 259*lmMst2) + 878948*
        lmMst2 + 1372*lmMst1*(31 + 15*lmMst2) - 44100*pow2(lmMsq) - 64680*pow2(
        lmMst2))/pow4(Msq) - (658560*pow2(1 + lmMst2))/pow4(Mst1))*pow4(Mst2))/
        92610. - pow4(Mst1)*((309383 - 420*lmMsq*(256 + 259*lmMst1 - 49*lmMst2)
        - 145432*lmMst2 + 28*lmMst1*(9034 + 13965*lmMst2) + 44100*pow2(lmMsq) -
        141120*pow2(lmMst1) - 205800*pow2(lmMst2))/(92610.*pow4(Msq)) - (
        1188.1718576027706 - (80*lmMsq)/3. + (-960.3533282942807 + 40*lmMsq)*
        lmMst1 + lmMst2*(1415.4644394053919 - (138308*lmMst1)/945. - (40*lmMsq*
        (3 + 2*lmMst1))/3. - (820*pow2(lmMst1))/9.) + (2*(-94783 + 6300*lmMsq)*
        pow2(lmMst1))/945. + (432.2899470899471 + (40*lmMsq)/3. - (332*lmMst1)/
        9.)*pow2(lmMst2) + (64*lmMt*(1 + 4*lmMst2 - 2*lmMst1*(2 + lmMst2) +
        pow2(lmMst1) + pow2(lmMst2)))/3. + (52*pow3(lmMst1))/27. + (3404*pow3(
        lmMst2))/27.)/pow4(Mst2) - (Dmglst2*(772.4522050654477 - (64*B4)/9. + (
        64*D3)/9. - (32*DN)/9. + (560*lmMsq)/3. + (1513.2107835726883 - 160*
        lmMsq)*lmMst1 + (38704*pow2(lmMst1))/189. + lmMst2*(160*lmMsq + (4*(
        1947623 - 3019800*lmMst1 + 97020*pow2(lmMst1)))/19845.) + (16*(6787 -
        5439*lmMst1)*pow2(lmMst2))/189. - (128*lmMt*(4 + 6*lmMst2 - 2*lmMst1*(3
        + lmMst2) + pow2(lmMst1) + pow2(lmMst2)))/3. - (16*pow3(lmMst1))/9. + (
        1328*pow3(lmMst2))/3.))/pow5(Mst2)) + ((-8*OepS2*(4485*pow2(Mst1)*pow3(
        Mst2) + 11402*Mst2*pow4(Mst1) - 24*Dmglst2*(150*pow2(Mst1)*pow2(Mst2) +
        919*pow4(Mst1)) + 621*pow5(Mst2)))/729. + (S2*(9*(11113 + 14950*lmMst1
        - 14950*lmMst2)*pow2(Mst1)*pow3(Mst2) + 4*(126661 + 85515*lmMst1 -
        85515*lmMst2)*Mst2*pow4(Mst1) - 24*Dmglst2*(45*(139 + 100*lmMst1 - 100*
        lmMst2)*pow2(Mst1)*pow2(Mst2) + (34538 + 27570*lmMst1 - 27570*lmMst2)*
        pow4(Mst1) + 5697*pow4(Mst2)) + 81*(-891 + 230*lmMst1 - 230*lmMst2)*
        pow5(Mst2)))/135.)/pow5(Mst2) + (5*pow2(Mst1)*(20*Dmglst2*(-43 + 144*
        lmMsq - 144*lmMst2)*Mst2 + (75 + 60*lmMsq*(-3 + 2*lmMst1 - 2*lmMst2) +
        356*lmMst2 - 8*lmMst1*(22 + 15*lmMst2) + 120*pow2(lmMst2))*pow2(Mst1))*
        pow3(Mst2) + 2*Dmglst2*(4*(-15*(-179 + 2*(87 + 32*lmMst1)*lmMst2 - 30*
        lmMsq*(-1 + 6*lmMst2) + 90*pow2(lmMsq) + 208*pow2(lmMst2))*pow2(Mst2) +
        2*pow2(Mst1)*(15529 - 120*B4 + 120*D3 - 60*DN + 150*lmMsq + 690*lmMst1
        + 675*pow2(lmMsq) + 345*pow2(lmMst1) - 5*lmMst2*(-3091 + 270*lmMsq +
        42*lmMst1 + 96*pow2(lmMst1)) + (4305 - 480*lmMst1)*pow2(lmMst2) + 960*
        pow3(lmMst2)))*pow4(Msq) - 600*pow2(Msq)*((17 - 20*lmMsq + 20*lmMst2)*
        pow2(Mst1)*pow2(Mst2) + 2*pow4(Mst1)) - 75*((7 + 12*lmMsq - 12*lmMst2)*
        pow2(Mst2)*pow4(Mst1) + 4*pow6(Mst1))) + 25*(18*Dmglst2*(13 - 28*lmMsq
        + 28*lmMst2) + (67 - 84*lmMsq + 84*lmMst2)*Mst2)*pow6(Mst2))/(270.*
        Mst2*pow2(Mst1)*pow4(Msq))) + Mt*(-((-1 + 2*lmMst2)*s2t*shiftst3*pow2(
        Mst2)*(-4*pow2(Mt) + ((-1 - lmMst1 + lmMst2)*pow2(Mst1) + pow2(Mst2))*
        pow2(s2t)))/(3.*pow2(Mst1)) + (2*T1ep*(-27*pow3(Mst2)*(3*Mst2*(46*Mst2*
        s2t*pow2(Mt) + 21*Mt*pow2(Mst2)*pow2(s2t) - 28*pow3(Mt) - 16*pow3(Mst2)
        *pow3(s2t)) + 7*Dmglst2*(-15*Mt*pow2(Mst2)*pow2(s2t) + 20*pow3(Mt) + 4*
        pow3(Mst2)*pow3(s2t))) - 9*Mst2*pow2(Mst1)*(Mst2*(2990*Mst2*s2t*pow2(
        Mt) + 627*Mt*pow2(Mst2)*pow2(s2t) - 1760*pow3(Mt) - 830*pow3(Mst2)*
        pow3(s2t)) + 3*Dmglst2*(-800*Mst2*s2t*pow2(Mt) - 907*Mt*pow2(Mst2)*
        pow2(s2t) + 1984*pow3(Mt) + 316*pow3(Mst2)*pow3(s2t))) + (12*(11028*
        Dmglst2 - 5701*Mst2)*s2t*pow2(Mt) + 6*(11867*Dmglst2 - 1599*Mst2)*Mst2*
        Mt*pow2(s2t) + 48720*pow3(Mt) + (-25328*Dmglst2 + 13305*Mst2)*pow2(
        Mst2)*pow3(s2t))*pow4(Mst1)))/(729.*pow5(Mst2)) + pow3(s2t)*(pow2(Mst1)
        *(301.91041152263375 + 4*B4 + (4*D3)/9. - (4*DN)/9. - (335*lmMsq)/9. +
        (25*pow2(lmMsq))/3. + lmMst1*(188.5172839506173 - (325*lmMsq)/9. + (5*
        pow2(lmMsq))/3.) + (10*(-149 + 18*lmMsq)*pow2(lmMst1))/27. - (lmMst2*(
        53429 - 183450*lmMst1 + 2250*lmMsq*(-7 + 6*lmMst1) + 1350*pow2(lmMsq) +
        29610*pow2(lmMst1)))/810. + ((-3683 + 270*lmMsq + 2595*lmMst1)*pow2(
        lmMst2))/27. - (110*pow3(lmMst1))/27. - (1498*pow3(lmMst2))/27.) - (-(-
        1503 + 60*lmMsq*(12 + 5*lmMst1 - 5*lmMst2) + 950*lmMst2 - 10*lmMst1*(
        167 + 210*lmMst2) + 900*pow2(lmMst1) + 1200*pow2(lmMst2))/(270.*pow2(
        Msq)) - (159.17989444184494 + (27180961*lmMst2)/396900. - (2*(78238 +
        33285*lmMst2)*pow2(lmMst1))/945. - (157316*pow2(lmMst2))/945. + (5*
        lmMsq*(-1 + 6*lmMst2 - 2*lmMst1*(3 + 16*lmMst2) + 16*pow2(lmMst1) + 16*
        pow2(lmMst2)))/6. + lmMst1*(-74.92759133282942 + (313792*lmMst2)/945. +
        (446*pow2(lmMst2))/3.) - (70*pow3(lmMst1))/27. - (2042*pow3(lmMst2))/
        27.)/pow2(Mst2) + (Dmglst2*(426.99721850958684 + (1510169*lmMst2)/
        59535. + lmMsq*(10 - 20*lmMst1 + 20*lmMst2) + (2*(433 + 6552*lmMst2)*
        pow2(lmMst1))/189. + (194*pow2(lmMst2))/189. - (lmMst1*(2674409 +
        333900*lmMst2 + 7514640*pow2(lmMst2)))/59535. - (112*pow3(lmMst1))/27.
         + (1648*pow3(lmMst2))/
        27.))/pow3(Mst2))*pow4(Mst1) + (pow2(Mst2)*(82320*(725 - 30*lmMsq*(5 +
        lmMst1 - lmMst2) + 793*lmMst2 - lmMst1*(643 + 150*lmMst2) + 90*pow2(
        lmMst1) + 60*pow2(lmMst2))*pow2(Msq)*pow2(Mst1) - 68600*(12760 - 720*B4
        + 72*D3 - 36*DN - 1980*lmMsq + 540*pow2(lmMsq) - 3*lmMst1*(5471 - 1950*
        lmMsq + 630*pow2(lmMsq)) - 954*pow2(lmMst1) + 3*lmMst2*(8495 - 6750*
        lmMst1 + 210*lmMsq*(-11 + 6*lmMst1) + 630*pow2(lmMsq) + 234*pow2(
        lmMst1)) - 18*(-1324 + 210*lmMsq + 399*lmMst1)*pow2(lmMst2) - 288*pow3(
        lmMst1) + 6768*pow3(lmMst2))*pow4(Msq) - 3*(1186407 - 420*lmMsq*(5549 +
        6020*lmMst1 - 6860*lmMst2) - 3793580*lmMst2 + 560*lmMst1*(10936 + 9555*
        lmMst2) - 176400*pow2(lmMsq) - 1411200*pow2(lmMst1) - 4116000*pow2(
        lmMst2))*pow4(Mst1) - (102900*Dmglst2*Mst2*(-40*(-174 + 6*lmMsq*(6 +
        17*lmMst1 - 17*lmMst2) + 73*lmMst2 - lmMst1*(109 + 102*lmMst2) + 102*
        pow2(lmMst2))*pow2(Msq)*pow2(Mst1) + 12*(-115 + (430 + 64*lmMst1)*
        lmMst2 - 30*lmMsq*(-1 + 6*lmMst2) + 90*pow2(lmMsq) + 400*pow2(lmMst2))*
        pow4(Msq) + 5*(-119 + 12*lmMsq*(-1 + 10*lmMst1 - 10*lmMst2) + 34*lmMst2
        - 2*lmMst1*(11 + 60*lmMst2) + 120*pow2(lmMst2))*pow4(Mst1)))/pow2(Mst1)
        ))/(1.11132e7*pow4(Msq)) - Dmglst2*((pow2(Mst1)*(5430043 + 1334580*
        lmMst2 + 202500*lmMsq*(7 + 6*lmMst2) - 607500*pow2(lmMsq) + 900*(-859 +
        3690*lmMst2)*pow2(lmMst1) + 846900*pow2(lmMst2) - 120*lmMst1*(44309 +
        12315*lmMst2 + 56475*pow2(lmMst2)) + (33750*(5 - 2*lmMst1 + 2*lmMst2)*
        pow2(Mst1))/pow2(Msq) + 45000*pow3(lmMst1) + 3411000*pow3(lmMst2)))/(
        60750.*Mst2) + (Mst2*(5267 - 432*B4 + 576*D3 - 360*DN + 8640*lmMsq +
        216*lmMst1*(7 - 15*pow2(lmMsq)) - 1404*pow2(lmMst1) + 72*lmMst2*(-237 -
        248*lmMst1 + 90*lmMsq*lmMst1 + 45*pow2(lmMsq) + 16*pow2(lmMst1)) - 36*(
        -439 + 180*lmMsq + 336*lmMst1)*pow2(lmMst2) + (60*(20 + 6*lmMsq*(-4 +
        3*lmMst1 - 3*lmMst2) + 27*lmMst2 - 3*lmMst1*(1 + 6*lmMst2) + 18*pow2(
        lmMst2))*pow2(Mst1))/pow2(Msq) + 10944*pow3(lmMst2) - (45*pow4(Mst1))/
        pow4(Msq)))/162.) - (((2*(637 - 20*lmMsq*(9 + 20*lmMst1 - 20*lmMst2) -
        495*lmMst2 + 25*lmMst1*(27 + 16*lmMst2) - 400*pow2(lmMst2)))/pow2(Msq)
        + (5*(373 + 706*lmMst2 + 64*lmMst1*(1 + lmMst2) - 30*lmMsq*(5 + 6*
        lmMst2) + 90*pow2(lmMsq) + 304*pow2(lmMst2)))/pow2(Mst1))*pow4(Mst2))/
        90. + ((-343000*Dmglst2*Mst2*(337 - 24*lmMsq*(5 + 8*lmMst1 - 8*lmMst2)
        + 14*lmMst2 + 2*lmMst1*(53 + 96*lmMst2) - 192*pow2(lmMst2)) + 3*(
        3653969 - 140*lmMsq*(12899 + 5390*lmMst1 - 6230*lmMst2) + 2069970*
        lmMst2 + 37730*lmMst1*(-7 + 20*lmMst2) - 58800*pow2(lmMsq) - 813400*
        pow2(lmMst2))*pow2(Mst1))*pow4(Mst2))/(3.7044e6*pow4(Msq)) + (8*
        Dmglst2*(24*lmMst2*(1 + lmMst2) + (5*(14 - 15*lmMsq + 15*lmMst2)*pow2(
        Mst1))/pow2(Msq))*pow5(Mst2))/(27.*pow4(Mst1)) + (54*Dmglst2*(632*OepS2
        + 9*(16193 - 1422*lmMst1 + 1422*lmMst2)*S2)*pow2(Mst1)*pow2(Mst2) - 45*
        (664*OepS2 - 81*(1261 + 166*lmMst1 - 166*lmMst2)*S2)*pow2(Mst1)*pow3(
        Mst2) - 15*Mst2*(3548*OepS2 - 27*(15148 + 2661*lmMst1 - 2661*lmMst2)*
        S2)*pow4(Mst1) + 4*Dmglst2*(25328*OepS2 + 27*(47051 - 18996*lmMst1 +
        18996*lmMst2)*S2)*pow4(Mst1) + 54*Dmglst2*(56*OepS2 - 81*(-1677 + 14*
        lmMst1 - 14*lmMst2)*S2)*pow4(Mst2) + 1296*(-4*OepS2 + 81*(22 + lmMst1 -
        lmMst2)*S2)*pow5(Mst2))/(2187.*pow3(Mst2)) + ((5*(43 - 30*lmMsq + 30*
        lmMst2))/(27.*pow2(Msq)*pow2(Mst1)) + (-58456697 + 420*lmMsq*(63929 +
        69090*lmMst1 - 69930*lmMst2) - 374010*lmMst2 - 10290*lmMst1*(2573 +
        2820*lmMst2) + 176400*pow2(lmMsq) + 29194200*pow2(lmMst2))/(1.11132e7*
        pow4(Msq)) + (16*pow2(1 + lmMst2))/(9.*pow4(Mst1)))*pow6(Mst2)))))/
        Tbeta + ((1 - 2*lmMst2)*shiftst3*((8*(-1 + lmMst1 - lmMst2)*pow2(Mst1)*
        pow2(Mt)*pow2(s2t) - 16*pow4(Mt) - (-4*pow2(Mst1)*pow2(Mst2) + (3 + 2*
        lmMst1 - 2*lmMst2)*pow4(Mst1) + pow4(Mst2))*pow4(s2t))*pow6(Mst2) - (4*
        pow2(Mt)*pow2(MuSUSY)*pow2(s2t)*(2*(1 - 2*lmMst1 + 2*lmMst2)*pow2(Mst2)
        *pow4(Mst1) + 2*(1 - lmMst1 + lmMst2)*pow2(Mst1)*pow4(Mst2) + (2 - 6*
        lmMst1 + 6*lmMst2)*pow6(Mst1) + pow6(Mst2)))/pow2(Sbeta) + 4*pow2(Mt)*
        pow2(s2t)*(pow2(MuSUSY)*pow6(Mst2) + 2*(pow2(MuSUSY)*((1 - 2*lmMst1 +
        2*lmMst2)*pow2(Mst2)*pow4(Mst1) + (1 - lmMst1 + lmMst2)*pow2(Mst1)*
        pow4(Mst2) + (1 - 3*lmMst1 + 3*lmMst2)*pow6(Mst1)) + pow8(Mst2)))))/(
        12.*pow2(Mst1)*pow4(Mst2)))) - Al4p*((twoLoopFlag*((-72*Mt*pow3(s2t)*(-
        (Mst2*(4*(3 + lmMst1*(2 + lmMst2) - pow2(lmMst2))*pow2(Mst1)*pow2(Mst2)
        + (3 + 2*lmMst1 - 2*lmMst2)*pow4(Mst1) + 4*(-4 + lmMst1 - 3*lmMst2 +
        lmMst1*lmMst2 - pow2(lmMst2))*pow4(Mst2))) + Dmglst2*(-4*(1 + lmMst1*(-
        2 + lmMst2) + 4*lmMst2 - pow2(lmMst2))*pow2(Mst1)*pow2(Mst2) + (1 + 6*
        lmMst1 - 6*lmMst2)*pow4(Mst1) + 4*(6 + lmMst1 + lmMst2 - lmMst1*lmMst2
        + pow2(lmMst2))*pow4(Mst2))))/pow2(Mst2) - (144*s2t*pow3(Mt)*(4*pow2(
        Mst1)*((4 - 5*lmMst2 + lmMst1*(5 + 2*lmMst2 - 2*lmMt) + 2*lmMst2*lmMt -
        2*pow2(lmMst2))*pow2(Mst2) + (-5 - 6*lmMst2 + lmMst1*(4 + 3*lmMst2) -
        3*pow2(lmMst2))*pow2(MuSUSY))*pow3(Mst2) + Mst2*(4*(5 - 7*lmMst2 +
        lmMst1*(7 + 2*lmMst2 - 2*lmMt) + 2*lmMst2*lmMt - 2*pow2(lmMst2))*pow2(
        Mst2) + (-21 - 38*lmMst2 + 10*lmMst1*(3 + 2*lmMst2) - 20*pow2(lmMst2))*
        pow2(MuSUSY))*pow4(Mst1) + 4*(-4 + lmMst1 - 3*lmMst2 + lmMst1*lmMst2 -
        pow2(lmMst2))*pow2(MuSUSY)*pow5(Mst2) + Dmglst2*(4*pow2(Mst1)*pow2(
        Mst2)*((6 + 13*lmMst2 - 4*lmMt - 2*lmMst2*lmMt + lmMst1*(-9 - 2*lmMst2
        + 2*lmMt) + 2*pow2(lmMst2))*pow2(Mst2) + (-11 + 2*lmMst2 + lmMst1*(-4 +
        3*lmMst2) - 3*pow2(lmMst2))*pow2(MuSUSY)) - (4*(1 - 29*lmMst2 + lmMst1*
        (25 + 6*lmMst2 - 6*lmMt) + 4*lmMt + 6*lmMst2*lmMt - 6*pow2(lmMst2))*
        pow2(Mst2) + (65 + lmMst1*(34 - 20*lmMst2) - 26*lmMst2 + 20*pow2(
        lmMst2))*pow2(MuSUSY))*pow4(Mst1) - 4*(6 + lmMst1 + lmMst2 - lmMst1*
        lmMst2 + pow2(lmMst2))*pow2(MuSUSY)*pow4(Mst2) + 8*(5 + lmMst1*(-1 +
        lmMst2) + lmMst2 - pow2(lmMst2))*pow6(Mst2)) + 4*(3 - 4*lmMst2 + 2*(
        lmMst1 + lmMst1*lmMst2 + lmMt) - 2*pow2(lmMst2))*pow7(Mst2)))/pow6(
        Mst2) + (32*pow4(Mt)*(2*Dmglst2*((53 - 18*lmMst1 + 24*lmMst2 - 24*lmMt)
        *pow2(Mst1)*pow4(Mst2) + 18*((5 + lmMst2*(11 - 2*lmMt) - 2*lmMst1*(4 +
        lmMst2 - lmMt) - 3*lmMt + 2*pow2(lmMst2))*pow2(Mst2)*pow4(Mst1) + (3 -
        6*lmMst2*(-5 + lmMt) - 5*lmMt + lmMst1*(-25 - 6*lmMst2 + 6*lmMt) + 6*
        pow2(lmMst2))*pow6(Mst1)) - 18*lmMst2*pow6(Mst2)) + 9*(2*(2 + 2*lmMst1*
        (3 + lmMst2 - lmMt) + lmMt + lmMst2*(-7 + 2*lmMt) - 2*pow2(lmMst2))*
        pow3(Mst2)*pow4(Mst1) + (3 + 2*lmMst1*(3 + lmMst2) + lmMt + lmMst2*(-5
        + 4*lmMt) + pow2(lmMst1) - pow2(lmMst2) - 6*pow2(lmMt))*pow2(Mst1)*
        pow5(Mst2) + Mst2*(9 + lmMst1*(22 + 6*lmMst2 - 6*lmMt) + 6*lmMst2*(-4 +
        lmMt) + 2*lmMt - 6*pow2(lmMst2))*pow6(Mst1) - 2*(1 + lmMst2)*pow7(Mst2)
        )))/(pow2(Mst1)*pow5(Mst2)) + (36*Mt*MuSUSY*(16*pow3(Mt)*(2*(4 +
        lmMst1*(3 + 2*lmMst2 - lmMt) + lmMst2*(-4 + lmMt) + lmMt - 2*pow2(
        lmMst2))*pow2(Mst1)*pow3(Mst2) + Mst2*(13 + 2*lmMst1*(6 + 3*lmMst2 - 2*
        lmMt) + 2*lmMt + 2*lmMst2*(-7 + 2*lmMt) - 6*pow2(lmMst2))*pow4(Mst1) +
        Dmglst2*(11 - 6*lmMst1*lmMst2 - 8*lmMst2*(-5 + lmMt) + 8*lmMst1*(-4 +
        lmMt) - 8*lmMt + 6*pow2(lmMst2))*pow4(Mst1) + 2*Dmglst2*((7 + 7*lmMst2
        + lmMst1*(-5 + lmMt) - 2*lmMt - lmMst2*lmMt)*pow2(Mst1)*pow2(Mst2) + (5
        + lmMst1*(-1 + lmMst2) + lmMst2 - pow2(lmMst2))*pow4(Mst2)) + 2*(2 +
        lmMst1 + (-2 + lmMst1)*lmMst2 + lmMt - pow2(lmMst2))*pow5(Mst2)) + 6*
        Mt*pow2(Mst2)*pow2(s2t)*(4*(1 + 3*lmMst2 - lmMst1*(3 + 2*lmMst2) + 2*
        pow2(lmMst2))*pow2(Mst1)*pow3(Mst2) + Mst2*(1 + 14*lmMst2 - 2*lmMst1*(7
        + 4*lmMst2) + 8*pow2(lmMst2))*pow4(Mst1) + Dmglst2*(4*(5 + lmMst1*(3 -
        2*lmMst2) - 3*lmMst2 + 2*pow2(lmMst2))*pow2(Mst1)*pow2(Mst2) + (21 +
        lmMst1*(18 - 8*lmMst2) - 18*lmMst2 + 8*pow2(lmMst2))*pow4(Mst1) + 4*(6
        + lmMst1 + lmMst2 - lmMst1*lmMst2 + pow2(lmMst2))*pow4(Mst2)) + 4*(4 +
        3*lmMst2 - lmMst1*(1 + lmMst2) + pow2(lmMst2))*pow5(Mst2)) + (8*Mst2*
        s2t*pow2(Mt)*(Dmglst2*(4*(1 - lmMst1 + lmMst2)*pow2(Mst2)*pow4(Mst1) +
        2*(1 + 2*lmMst2)*pow2(Mst1)*pow4(Mst2) + (4 - 8*lmMst1 + 8*lmMst2)*
        pow6(Mst1) - 4*lmMst2*pow6(Mst2)) - Mst2*(-2*lmMst1*((1 + 2*lmMst2)*(
        pow2(Mst1) + pow2(Mst2))*pow4(Mst1) + (3 + lmMst2)*pow2(Mst1)*pow4(
        Mst2)) + pow2(lmMst1)*(2*pow2(Mst2)*pow4(Mst1) - pow2(Mst1)*pow4(Mst2)
        + 2*pow6(Mst1)) + pow2(lmMst2)*(2*pow2(Mst2)*pow4(Mst1) + 3*pow2(Mst1)*
        pow4(Mst2) + 2*pow6(Mst1)) + 2*pow6(Mst2) + 2*lmMst2*(pow2(Mst2)*pow4(
        Mst1) + 2*pow2(Mst1)*pow4(Mst2) + pow6(Mst1) + pow6(Mst2)))))/pow2(
        Mst1) + (pow3(Mst2)*pow3(s2t)*(2*(-16 + 6*lmMst2 - 2*lmMst1*(8 + 5*
        lmMst2) + 3*pow2(lmMst1) + 7*pow2(lmMst2))*pow3(Mst2)*pow4(Mst1) - 2*(-
        12 - 18*lmMst2 + 2*lmMst1*(5 + 3*lmMst2) + pow2(lmMst1) - 7*pow2(
        lmMst2))*pow2(Mst1)*pow5(Mst2) + Mst2*(3 + lmMst1*(2 - 32*lmMst2) - 2*
        lmMst2 + 16*pow2(lmMst1) + 16*pow2(lmMst2))*pow6(Mst1) - 4*Dmglst2*(2*(
        1 + 2*lmMst1 - lmMst2)*pow2(Mst2)*pow4(Mst1) + 2*(1 + lmMst1 - lmMst2 +
        2*lmMst1*lmMst2 - 2*pow2(lmMst2))*pow2(Mst1)*pow4(Mst2) + (1 + 2*lmMst1
        - 2*lmMst2)*pow6(Mst1) - 2*lmMst2*pow6(Mst2)) + 4*(1 + lmMst2)*pow7(
        Mst2)))/pow2(Mst1)))/(Tbeta*pow6(Mst2)) + 36*pow2(Mt)*pow2(s2t)*(4*(-2*
        lmMst1*(-2 + lmMst2) - lmMst2*(2 + lmMst2) + 3*pow2(lmMst1))*pow2(Mst1)
        - 4*(2 - 2*lmMst2 + 2*lmMst1*(3 + lmMst2) + pow2(lmMst1) - 3*pow2(
        lmMst2))*pow2(Mst2) + (8*(2*Dmglst2*lmMst2 + Mst2 + lmMst2*Mst2)*pow3(
        Mst2))/pow2(Mst1) - (8*Dmglst2*(pow2(Mst1)*pow2(Mst2) - 2*lmMst1*pow2(
        Mst1)*(pow2(Mst1) + pow2(Mst2)) + 2*lmMst2*pow4(Mst1) + pow4(Mst2) + 4*
        lmMst2*pow4(Mst2)))/pow3(Mst2) + (pow2(MuSUSY)*(4*(-1 - 13*lmMst1 + 13*
        lmMst2 - 8*lmMst1*lmMst2 + pow2(lmMst1) + 7*pow2(lmMst2))*pow3(Mst2)*
        pow4(Mst1) - 2*(-14 + 10*lmMst1 - 20*lmMst2 + 6*lmMst1*lmMst2 + pow2(
        lmMst1) - 7*pow2(lmMst2))*pow2(Mst1)*pow5(Mst2) + Mst2*(-1 - 50*lmMst1
        + 50*lmMst2 - 64*lmMst1*lmMst2 + 20*pow2(lmMst1) + 44*pow2(lmMst2))*
        pow6(Mst1) - 4*Dmglst2*(2*(2 - 3*lmMst2 + lmMst1*(3 + 2*lmMst2) - 2*
        pow2(lmMst2))*pow2(Mst2)*pow4(Mst1) + 2*(1 + lmMst1 - 2*lmMst2 + 2*
        lmMst1*lmMst2 - 2*pow2(lmMst2))*pow2(Mst1)*pow4(Mst2) + (5 - 8*lmMst2 +
        4*lmMst1*(2 + lmMst2) - 4*pow2(lmMst2))*pow6(Mst1) - 2*lmMst2*pow6(
        Mst2)) + 4*(1 + lmMst2)*pow7(Mst2)))/(pow2(Mst1)*pow5(Mst2))) + ((-9*
        pow4(s2t)*(4*(-14 - 3*lmMst1 - 6*lmMst2 - 2*lmMst1*lmMst2 + 2*pow2(
        lmMst1))*pow3(Mst2)*pow4(Mst1) - 2*(-10 + 10*lmMst1 - 16*lmMst2 + 6*
        lmMst1*lmMst2 + pow2(lmMst1) - 7*pow2(lmMst2))*pow2(Mst1)*pow5(Mst2) +
        Mst2*(35 + 34*lmMst1 - 14*lmMst2 - 12*lmMst1*lmMst2 + 10*pow2(lmMst1) +
        2*pow2(lmMst2))*pow6(Mst1) + 4*Dmglst2*(-2*(lmMst1 - 2*lmMst1*lmMst2 +
        2*pow2(lmMst2))*pow2(Mst2)*pow4(Mst1) - 2*(1 + lmMst1 + 2*lmMst1*lmMst2
        - 2*pow2(lmMst2))*pow2(Mst1)*pow4(Mst2) + (1 + 2*lmMst1)*pow6(Mst1) +
        2*lmMst2*pow6(Mst2)) + 4*(1 + lmMst2)*pow7(Mst2)))/Mst2 - (36*s2t*pow2(
        Mt)*pow2(MuSUSY)*(4*(4*Mt*(5 + 6*lmMst2 - lmMst1*(4 + 3*lmMst2) + 3*
        pow2(lmMst2)) + Mst2*s2t*(-1 + 13*lmMst2 - lmMst1*(13 + 8*lmMst2) +
        pow2(lmMst1) + 7*pow2(lmMst2)))*pow3(Mst2)*pow4(Mst1) + 2*(-(Mst2*s2t*(
        -14 - 20*lmMst2 + 2*lmMst1*(5 + 3*lmMst2) + pow2(lmMst1) - 7*pow2(
        lmMst2))) + 8*Mt*(4 + 3*lmMst2 - lmMst1*(1 + lmMst2) + pow2(lmMst2)))*
        pow2(Mst1)*pow5(Mst2) + 4*Dmglst2*(Mst2*s2t*(-5 + 8*lmMst2 - 4*lmMst1*(
        2 + lmMst2) + 4*pow2(lmMst2)) + Mt*(65 + lmMst1*(34 - 20*lmMst2) - 26*
        lmMst2 + 20*pow2(lmMst2)))*pow6(Mst1) + Mst2*(Mst2*s2t*(-1 + 50*lmMst2
        - 2*lmMst1*(25 + 32*lmMst2) + 20*pow2(lmMst1) + 44*pow2(lmMst2)) + Mt*(
        84 + 152*lmMst2 - 40*lmMst1*(3 + 2*lmMst2) + 80*pow2(lmMst2)))*pow6(
        Mst1) + 8*Dmglst2*((Mst2*s2t*(-2 + 3*lmMst2 - lmMst1*(3 + 2*lmMst2) +
        2*pow2(lmMst2)) + Mt*(22 + lmMst1*(8 - 6*lmMst2) - 4*lmMst2 + 6*pow2(
        lmMst2)))*pow2(Mst2)*pow4(Mst1) + (-(Mst2*s2t*(1 + lmMst1 - 2*lmMst2 +
        2*lmMst1*lmMst2 - 2*pow2(lmMst2))) + 2*Mt*(6 + lmMst1 + lmMst2 -
        lmMst1*lmMst2 + pow2(lmMst2)))*pow2(Mst1)*pow4(Mst2) + lmMst2*s2t*pow7(
        Mst2)) + 4*(1 + lmMst2)*s2t*pow8(Mst2)))/(pow2(Sbeta)*pow6(Mst2)))/
        pow2(Mst1)))/216. + xDmglst2*pow2(Dmglst2)*(-(twoLoopFlag*(72*Mt*pow3(
        s2t)*pow4(Mst2)*(8*pow2(Mst1)*pow2(Mst2) + (3 - 6*lmMst1 + 6*lmMst2)*
        pow4(Mst1) + 2*(8 + lmMst1 - lmMst2)*pow4(Mst2)) + (96*Mst2*pow4(Mt)*(
        3*(37 + 41*lmMst2 - 13*lmMt - 6*lmMst2*lmMt + lmMst1*(-28 - 6*lmMst2 +
        6*lmMt) + 6*pow2(lmMst2))*pow2(Mst2)*pow4(Mst1) + (17 - 3*lmMst1 + 9*
        lmMst2 - 3*lmMt)*pow2(Mst1)*pow4(Mst2) + 3*(75 + 174*lmMst2 - 37*lmMt -
        30*lmMst2*lmMt + lmMst1*(-137 - 30*lmMst2 + 30*lmMt) + 30*pow2(lmMst2))
        *pow6(Mst1) + 3*(-2 + lmMst2)*pow6(Mst2)))/pow2(Mst1) - 16*s2t*pow3(Mt)
        *(9*(2*(27 + 70*lmMst2 - 4*lmMst1*(14 + 3*lmMst2 - 3*lmMt) - 2*(7 + 6*
        lmMst2)*lmMt + 12*pow2(lmMst2))*pow2(Mst2) + 67*pow2(MuSUSY))*pow4(
        Mst1) + 18*(8 + lmMst1 - lmMst2)*pow2(MuSUSY)*pow4(Mst2) + 18*pow2(
        Mst1)*(2*(10 + lmMst1 - lmMst2)*pow2(Mst2)*pow2(MuSUSY) + (19 + 17*
        lmMst2 - 6*lmMt - 2*lmMst2*lmMt + lmMst1*(-11 - 2*lmMst2 + 2*lmMt) + 2*
        pow2(lmMst2))*pow4(Mst2)) + (19 + 36*lmMst1 - 42*lmMst2 + 6*lmMt)*pow6(
        Mst2)) - (12*Mst2*pow2(Mt)*pow2(s2t)*(-2*pow4(Mst1)*(3*(8 - 9*lmMst1 +
        9*lmMst2 + 2*lmMst1*lmMst2 - 2*pow2(lmMst2))*pow2(Mst2)*pow2(MuSUSY) +
        (-13 + 18*lmMst1 - 24*lmMst2)*pow4(Mst2)) - 3*(4*(-4 + 7*lmMst1 - 7*
        lmMst2)*pow2(Mst2) + (17 + 4*lmMst1*(-7 + lmMst2) + 28*lmMst2 - 4*pow2(
        lmMst2))*pow2(MuSUSY))*pow6(Mst1) + 6*(-2 + lmMst2)*(2*pow2(Mst2) +
        pow2(MuSUSY))*pow6(Mst2) - 2*pow2(Mst1)*(3*(8 - 5*lmMst1 + 4*lmMst2 +
        2*lmMst1*lmMst2 - 2*pow2(lmMst2))*pow2(MuSUSY)*pow4(Mst2) + (-29 + 12*
        lmMst2)*pow6(Mst2))))/pow2(Mst1) + (-9*pow4(s2t)*pow5(Mst2)*(2*(-6 +
        lmMst1 - 2*lmMst1*lmMst2 + 2*pow2(lmMst2))*pow2(Mst2)*pow4(Mst1) + 2*(4
        + 6*lmMst2 + lmMst1*(-5 + 2*lmMst2) - 2*pow2(lmMst2))*pow2(Mst1)*pow4(
        Mst2) + (1 - 2*lmMst1)*pow6(Mst1) - 2*(-2 + lmMst2)*pow6(Mst2)) + (36*
        s2t*pow2(Mt)*pow2(MuSUSY)*(-2*(-8*(10 + lmMst1 - lmMst2)*Mt + Mst2*s2t*
        (8 - 9*lmMst1 + 9*lmMst2 + 2*lmMst1*lmMst2 - 2*pow2(lmMst2)))*pow2(
        Mst2)*pow4(Mst1) - 2*(-4*(8 + lmMst1 - lmMst2)*Mt + Mst2*s2t*(8 - 5*
        lmMst1 + 4*lmMst2 + 2*lmMst1*lmMst2 - 2*pow2(lmMst2)))*pow2(Mst1)*pow4(
        Mst2) + (268*Mt + Mst2*s2t*(-17 - 4*lmMst1*(-7 + lmMst2) - 28*lmMst2 +
        4*pow2(lmMst2)))*pow6(Mst1) + 2*(-2 + lmMst2)*s2t*pow7(Mst2)))/pow2(
        Sbeta) - (4*Mt*MuSUSY*(-2*(pow2(Mst2)*(36*(5 - 3*lmMst1 + 3*lmMst2)*
        Mst2*s2t*pow2(Mt) - 54*(12 + lmMst1 - lmMst2)*Mt*pow2(Mst2)*pow2(s2t) +
        4*(155 + 3*lmMst2*(41 - 6*lmMt) - 18*lmMst1*(4 + lmMst2 - lmMt) - 51*
        lmMt + 18*pow2(lmMst2))*pow3(Mt) - 9*(4*lmMst1 - 5*lmMst2)*pow3(Mst2)*
        pow3(s2t))*pow4(Mst1) + pow2(Mst1)*(6*(17 - 6*lmMst2)*Mst2*s2t*pow2(Mt)
        - 54*(8 + lmMst1 - lmMst2)*Mt*pow2(Mst2)*pow2(s2t) + 4*(11 + 18*lmMst1
        - 21*lmMst2 + 3*lmMt)*pow3(Mt) + 9*(6 + 5*lmMst2 + lmMst1*(-5 + 2*
        lmMst2) - 2*pow2(lmMst2))*pow3(Mst2)*pow3(s2t))*pow4(Mst2)) - (72*(9 -
        10*lmMst1 + 10*lmMst2)*Mst2*s2t*pow2(Mt) + 54*(-27 + 4*lmMst1 - 4*
        lmMst2)*Mt*pow2(Mst2)*pow2(s2t) + 8*(335 + 699*lmMst2 - 18*lmMst1*(29 +
        7*lmMst2 - 7*lmMt) - 3*(59 + 42*lmMst2)*lmMt + 126*pow2(lmMst2))*pow3(
        Mt) + 9*(1 - 10*lmMst1 + 10*lmMst2)*pow3(Mst2)*pow3(s2t))*pow6(Mst1) +
        18*(-2 + lmMst2)*s2t*(-4*pow2(Mt) + pow2(Mst2)*pow2(s2t))*pow7(Mst2)))/
        Tbeta)/pow2(Mst1)))/(108.*pow7(Mst2)) + Al4p*threeLoopFlag*(((4*(1298 +
        75*lmMst1*(11 - 6*lmMst2) + 1530*lmMst2 + (1545 - 900*lmMst2)*lmMt +
        150*lmMsq*(-26 + 3*lmMst1 + 3*lmMst2 + 6*lmMt) - 900*pow2(lmMsq) + 450*
        pow2(lmMst2)))/(135.*pow2(Msq)) - (2*(-535 + 64*lmMst1*(-2 + lmMst2) -
        722*lmMst2 - 30*lmMsq*(-13 + 6*lmMst2) + 90*pow2(lmMsq) + 208*pow2(
        lmMst2)))/(9.*pow2(Mst1)) + (4256*OepS2)/(243.*pow2(Mst2)) - (126544097
        + 53351256*lmMst2 - 42840*(123 + 44*lmMst2)*lmMt + 7560*lmMsq*(151 +
        75*lmMst1 - 390*lmMst2 + 90*lmMt) + 850500*pow2(lmMsq) + 132300*pow2(
        lmMst1) + 12525660*pow2(lmMst2) - 18900*lmMst1*(1053 + 510*lmMst2 - 64*
        (lmMt + pow2(lmMst2))) - 1587600*pow2(lmMt) + (189000*(-7 + 2*lmMst1 -
        4*lmMst2 + 2*lmMt)*pow2(Mst1))/pow2(Msq) - 1209600*pow3(lmMst2))/(
        42525.*pow2(Mst2)) + (pow2(Mst2)*((8971 + 31560*lmMst2 - 150*lmMst1*(-
        29 + 60*lmMst2) + 12990*lmMt - 19800*lmMst2*lmMt + 300*lmMsq*(-163 +
        30*lmMst1 + 36*lmMst2 + 66*lmMt) - 19800*pow2(lmMsq) + 9000*pow2(
        lmMst2))/pow4(Msq) - (3840*(2 + lmMst2 - 3*pow2(lmMst2)))/pow4(Mst1)))/
        270. + pow2(Mst1)*((-5*(95 + 138*lmMst2 + 3*lmMst1*(-31 + 12*lmMst2) +
        (-93 + 36*lmMst2)*lmMt + 12*lmMsq*(4 - 3*(lmMst1 + lmMt)) + 36*pow2(
        lmMsq) - 36*pow2(lmMst2)))/(27.*pow4(Msq)) - (3444114731 + 1993815180*
        lmMst2 - 113400*(1403 + 285*lmMst2 - 225*lmMt)*pow2(lmMst1) +
        943960500*pow2(lmMst2) + 8505000*lmMsq*(119 + lmMst2*(65 - 6*lmMt) -
        25*lmMt + lmMst1*(-40 - 6*lmMst2 + 6*lmMt) + 6*pow2(lmMst2)) - 18900*
        lmMt*(-23695 + 43*lmMst2 + 90*pow2(lmMst2)) - 3780*lmMst1*(489006 -
        21415*lmMt + 35*lmMst2*(5201 + 180*lmMt) + 8700*pow2(lmMst2) - 21600*
        pow2(lmMt)) - 13608000*(13 + 6*lmMst2)*pow2(lmMt) + 2268000*pow3(
        lmMst1) + 62937000*pow3(lmMst2))/(637875.*pow4(Mst2))) + ((2*S2*((49447
        - 2380980*lmMst1 + 2380980*lmMst2)*pow2(Mst1) + 9*(36641 - 18620*lmMst1
        + 18620*lmMst2)*pow2(Mst2) - 95256*pow2(MuSUSY)))/945. + (907040*OepS2*
        pow2(Mst1) + 72*pow2(MuSUSY)*(33934 - 90*B4 + 90*D3 - 45*DN + 720*
        lmMst1 + 120*(163 + 24*lmMst1)*lmMst2 - 120*lmMt - 720*(2 + lmMst1)*
        pow2(lmMst2) + 720*pow3(lmMst2)))/3645.)/pow4(Mst2) + (5*(32*(4 - 15*
        lmMsq + 15*lmMst2)*pow2(Msq)*pow2(Mst2) + 3*(9 - 140*lmMsq + 140*
        lmMst2)*pow4(Mst2)))/(18.*pow2(Mst1)*pow4(Msq)))*pow4(Mt) - pow2(Mt)*
        pow2(s2t)*(1007.0386243386243 - (16*B4)/9. + (16*D3)/9. - (8*DN)/9. +
        90*lmMsq + (424*lmMst1)/9. - (32*lmMt)/9. + 20*pow2(lmMsq) + (4*lmMst2*
        (2635 - 270*lmMsq + 549*lmMst1 - 48*pow2(lmMst1)))/27. + (82*pow2(
        lmMst1))/9. - (2*(39 + 32*lmMst1)*pow2(lmMst2))/9. - (((30*(11 - 40*
        lmMsq + 40*lmMst2))/pow2(Msq) + (-727 + 64*lmMst1*(-2 + lmMst2) - 594*
        lmMst2 - 30*lmMsq*(-13 + 6*lmMst2) + 90*pow2(lmMsq) + 400*pow2(lmMst2))
        /pow2(Mst1))*pow2(Mst2))/9. + (128*pow3(lmMst2))/9. + pow2(Mst1)*((10*(
        19 - 60*lmMsq + 60*lmMst2))/(9.*pow2(Msq)) + (171.76748500881834 + 20*
        lmMsq*(-6 + lmMst1) + (286426*lmMst2)/675. + (32*(18 + 5*lmMst2)*lmMt)/
        9. - 10*pow2(lmMsq) + (5.822222222222222 - (4*lmMst2)/3.)*pow2(lmMst1)
        + (4*lmMst1*(47431 + 37635*lmMst2 - 3000*lmMt - 6975*pow2(lmMst2)))/
        675. - (3566*pow2(lmMst2))/15. + (4*pow3(lmMst1))/9. + (380*pow3(
        lmMst2))/9.)/pow2(Mst2)) - S2*(89.65714285714286 + 84*lmMst1 - 84*
        lmMst2 - ((65819 + 70*lmMst1 - 70*lmMst2)*pow2(MuSUSY))/(15.*pow2(Mst2)
        ) + (2*pow2(Mst1)*(9*(39779 + 20790*lmMst1 - 20790*lmMst2)*pow2(Mst2) -
        7*(620417 + 17670*lmMst1 - 17670*lmMst2)*pow2(MuSUSY)))/(945.*pow4(
        Mst2))) - (64*(2 + lmMst2 - 3*pow2(lmMst2))*pow4(Mst2))/(9.*pow4(Mst1))
        + (8*OepS2*(2*pow2(Mst1)*(891*pow2(Mst2) - 589*pow2(MuSUSY)) - 21*pow2(
        Mst2)*pow2(MuSUSY) + 378*pow4(Mst2)))/(729.*pow4(Mst2)) - pow2(MuSUSY)*
        ((5*(448 - 273*lmMst2 + 18*lmMsq*(8 - 19*lmMst1 + 19*lmMst2) + 3*
        lmMst1*(43 + 114*lmMst2) - 342*pow2(lmMst2)))/(27.*pow2(Msq)) + (-919 +
        64*lmMst1*(-2 + lmMst2) - 466*lmMst2 - 30*lmMsq*(-13 + 6*lmMst2) + 90*
        pow2(lmMsq) + 592*pow2(lmMst2))/(18.*pow2(Mst1)) + (92.43189300411522 -
        (76*B4)/9. + (80*D3)/9. - (14*DN)/3. + (545*lmMsq)/9. + 5*pow2(lmMsq) -
        (2*lmMst1*(-502 + 210*lmMsq + 45*pow2(lmMsq)))/9. - (25*pow2(lmMst1))/
        9. + (lmMst2*(-6175 + 1308*lmMst1 + 90*lmMsq*(11 + 6*lmMst1) + 270*
        pow2(lmMsq) + 96*pow2(lmMst1)))/27. - ((371 + 180*lmMsq + 336*lmMst1)*
        pow2(lmMst2))/9. + ((98.88888888888889 + (30 - 60*lmMsq)*lmMst1 - 30*
        lmMst2 + 60*(lmMsq + lmMst1)*lmMst2 - 60*pow2(lmMst2))*pow2(Mst1))/
        pow2(Msq) + (304*pow3(lmMst2))/9.)/pow2(Mst2) + pow2(Mst2)*((-5*(-3125
        + 540*lmMsq*(-1 + 4*lmMst1 - 4*lmMst2) + 876*lmMst2 - 48*lmMst1*(7 +
        45*lmMst2) + 2160*pow2(lmMst2)))/(216.*pow4(Msq)) + (32*(2 + lmMst2 -
        3*pow2(lmMst2)))/(9.*pow4(Mst1))) + pow2(Mst1)*((5*(493 + 5*(-17 + 60*
        lmMsq)*lmMst2 + lmMst1*(85 - 300*lmMsq + 300*lmMst2) - 300*pow2(lmMst2)
        ))/(36.*pow4(Msq)) - (182.90272153635118 + (140*B4)/9. - 16*D3 + (74*
        DN)/9. - (20*lmMsq)/3. + lmMst1*(-174.74765432098766 + 40*lmMsq + 10*
        pow2(lmMsq)) + lmMst2*(252.96987654320986 - (6002*lmMst1)/45. - 20*
        lmMsq*(2 + lmMst1) - 10*pow2(lmMsq) - (106*pow2(lmMst1))/3.) + (1556*
        pow2(lmMst1))/45. + (98.8 + 20*lmMsq + (878*lmMst1)/9.)*pow2(lmMst2) +
        (10*pow3(lmMst1))/9. - (190*pow3(lmMst2))/3.)/pow4(Mst2)) - (5*(32*(4 -
        15*lmMsq + 15*lmMst2)*pow2(Msq)*pow2(Mst2) + 3*(9 - 140*lmMsq + 140*
        lmMst2)*pow4(Mst2)))/(72.*pow2(Mst1)*pow4(Msq))) - (5*((33 + 444*lmMsq
        - 444*lmMst2)*pow2(Mst2)*pow4(Mst1) + 32*(-4 + 15*lmMsq - 15*lmMst2)*
        pow2(Msq)*pow4(Mst2) + (25 - 852*lmMsq + 852*lmMst2)*pow2(Mst1)*pow4(
        Mst2) + 3*(-9 + 140*lmMsq - 140*lmMst2)*pow6(Mst2)))/(36.*pow2(Mst1)*
        pow4(Msq))) + pow4(s2t)*((-2*OepS2*(1136*pow2(Mst1) + 21*pow2(Mst2)))/
        729. + (S2*(4*(14023 + 8520*lmMst1 - 8520*lmMst2)*pow2(Mst1) + 9*(65819
        + 70*lmMst1 - 70*lmMst2)*pow2(Mst2)))/540. + pow2(Mst1)*(
        104.7055157750343 - B4/3. + (4*D3)/9. - (5*DN)/18. + (835*lmMsq)/36. +
        lmMst1*(13.868641975308641 - (40*lmMsq)/3. - (5*pow2(lmMsq))/2.) + (5*
        pow2(lmMsq))/4. + (653*pow2(lmMst1))/90. - lmMst2*(44.63716049382716 -
        (65*lmMsq)/6. + (10.011111111111111 - 5*lmMsq)*lmMst1 - (5*pow2(lmMsq))
        /2. + (127*pow2(lmMst1))/18.) - ((372 + 450*lmMsq - 515*lmMst1)*pow2(
        lmMst2))/90. + (5*pow3(lmMst1))/18. + (19*pow3(lmMst2))/18.) - pow2(
        Mst2)*(50.41352880658436 - (19*B4)/9. + (20*D3)/9. - (7*DN)/6. + (155*
        lmMsq)/36. - (5*pow2(lmMsq))/4. - (lmMst1*(-566 + 210*lmMsq + 45*pow2(
        lmMsq)))/18. - (25*pow2(lmMst1))/36. + (lmMst2*(-4681 + 1116*lmMst1 +
        90*lmMsq*(17 + 6*lmMst1) + 270*pow2(lmMsq) + 96*pow2(lmMst1)))/108. - (
        (353 + 60*lmMsq + 112*lmMst1)*pow2(lmMst2))/12. - (5*(205 - 102*lmMst2
        + 18*lmMsq*(3 - 10*lmMst1 + 10*lmMst2) + 12*lmMst1*(4 + 15*lmMst2) -
        180*pow2(lmMst2))*pow2(Mst1))/(54.*pow2(Msq)) + (76*pow3(lmMst2))/9.) -
        (((10*(544 - 18*lmMsq*(12 + 19*lmMst1 - 19*lmMst2) + 87*lmMst2 + 3*
        lmMst1*(43 + 114*lmMst2) - 342*pow2(lmMst2)))/pow2(Msq) + (3*(-1175 +
        64*lmMst1*(-2 + lmMst2) - 594*lmMst2 - 30*lmMsq*(-13 + 6*lmMst2) + 90*
        pow2(lmMsq) + 976*pow2(lmMst2)))/pow2(Mst1))*pow4(Mst2))/216. - ((8*(2
        + lmMst2 - 3*pow2(lmMst2))*pow2(Msq) + 5*(-4 + 15*lmMsq - 15*lmMst2)*
        pow2(Mst1))*pow6(Mst2))/(9.*pow2(Msq)*pow4(Mst1)) + (5*((3373 - 180*
        lmMsq*(1 + 14*lmMst1 - 14*lmMst2) + 18*lmMst2 + 18*lmMst1*(9 + 140*
        lmMst2) - 2520*pow2(lmMst2))*pow2(Mst1)*pow4(Mst2) - (3287 - 180*lmMsq*
        (11 + 12*lmMst1 - 12*lmMst2) + 1644*lmMst2 + 48*lmMst1*(7 + 45*lmMst2)
        - 2160*pow2(lmMst2))*pow6(Mst2)))/(864.*pow4(Msq))) - (Mt*pow3(s2t)*(-(
        pow4(Msq)*(12*pow2(Mst1)*pow2(Mst2)*(186448 - 61560*B4 + 1620*DN +
        288900*lmMsq + 1620*(-31 + 10*lmMsq)*lmMst1 - 180*(2201 + 90*lmMsq +
        528*lmMst1)*lmMst2 + 2240*OepS2 - 81*(-1373 + 560*lmMst1 - 560*lmMst2)*
        S2 - 4320*pow2(lmMst1) + 4320*(23 + 12*lmMst1)*pow2(lmMst2) - 51840*
        pow3(lmMst2)) - (9255817 - 32400*lmMsq*(7 + 6*lmMst1 - 6*lmMst2) +
        4247280*lmMst2 - 437920*OepS2 - 1836*(-1987 + 4830*lmMst2)*S2 + 360*
        lmMst1*(-7670 + 2520*lmMst2 + 24633*S2) - 349920*pow2(lmMst1) - 868320*
        pow2(lmMst2))*pow4(Mst1) + 103680*(3 + 4*lmMst2 - 3*pow2(lmMst2))*pow4(
        Mst2))) + 10800*pow2(Msq)*((35 - 36*lmMsq*(2 + lmMst1 - lmMst2) + 84*
        lmMst2 + 12*lmMst1*(-1 + 3*lmMst2) - 36*pow2(lmMst2))*pow2(Mst2)*pow4(
        Mst1) - (509 + lmMst1*(12 - 36*lmMst2) + 36*lmMsq*(-2 + lmMst1 -
        lmMst2) + 60*lmMst2 + 36*pow2(lmMst2))*pow2(Mst1)*pow4(Mst2)) + 2700*((
        239 - 180*lmMsq*(2 + lmMst1 - lmMst2) + 387*lmMst2 + 9*lmMst1*(-3 + 20*
        lmMst2) - 180*pow2(lmMst2))*pow4(Mst1)*pow4(Mst2) - (1307 + 180*lmMsq*(
        -2 + lmMst1 - lmMst2) + 333*lmMst2 - 9*lmMst1*(-3 + 20*lmMst2) + 180*
        pow2(lmMst2))*pow2(Mst1)*pow6(Mst2))))/(14580.*Mst2*pow2(Mst1)*pow4(
        Msq)) + (s2t*pow3(Mt)*(60480*Mst2*((5*(67 - 6*lmMst1*(1 + 3*lmMsq - 3*
        lmMst2) + 3*lmMst2 + 18*lmMsq*lmMst2 + 3*lmMt - 18*pow2(lmMst2)))/pow2(
        Msq) + (24*(-3 - 4*lmMst2 + 3*pow2(lmMst2)))/pow2(Mst1)) + (24*(4617197
        - 18900*lmMsq*(5 + 6*lmMst1 - 6*lmMst2) + 699720*lmMst2 + 3780*lmMst1*(
        -75 + 96*lmMst2 - 16*lmMt) + 1260*(19 + 56*lmMst2)*lmMt + 30240*pow2(
        lmMst1) - 599760*pow2(lmMst2) + 15120*pow2(lmMt) - (18900*(7 + 2*lmMst1
        - 4*lmMst2 + 2*lmMt)*pow2(Mst1))/pow2(Msq)))/Mst2 + (8*pow2(Mst1)*(
        16684903 + 13530195*lmMst2 + 170100*lmMsq*(17 - 2*lmMst1 + 4*lmMst2 -
        2*lmMt) - 22680*(41 + 4*lmMst2 - 4*lmMt)*pow2(lmMst1) + 4023810*pow2(
        lmMst2) - 1890*lmMt*(-1210 - 19*lmMst2 + 48*pow2(lmMst2)) - 315*lmMst1*
        (32933 + 6654*lmMst2 + 690*lmMt + 864*pow2(lmMst2) - 864*pow2(lmMt)) -
        272160*(3 + lmMst2)*pow2(lmMt) + 362880*pow3(lmMst2)))/pow3(Mst2) + (-
        7840*OepS2*(24*pow2(Mst2)*(2*pow2(Mst2) + pow2(MuSUSY)) + pow2(Mst1)*(
        1208*pow2(Mst2) + 439*pow2(MuSUSY))) - 108*S2*(pow2(Mst1)*(8*(53899 -
        221970*lmMst1 + 221970*lmMst2)*pow2(Mst2) - 245*(259 + 2634*lmMst1 -
        2634*lmMst2)*pow2(MuSUSY)) + 9*(7*(1373 - 560*lmMst1 + 560*lmMst2)*
        pow2(Mst2)*pow2(MuSUSY) + 16*(1471 - 490*lmMst1 + 490*lmMst2)*pow4(
        Mst2))))/pow5(Mst2) + (37800*(Mst2*(47 - 39*lmMst1 + 6*(11 + 6*lmMst1)*
        lmMst2 + (-39 + 36*lmMst2)*lmMt + 12*lmMsq*(1 - 3*(lmMst1 + lmMt)) +
        36*pow2(lmMsq) - 36*pow2(lmMst2))*pow2(Mst1) + (673 - 6*lmMst2 + 9*
        lmMst1*(-3 + 20*lmMst2) - 12*lmMsq*(1 + 15*lmMst1 - 18*lmMst2 - 3*lmMt)
        + 45*lmMt - 36*lmMst2*lmMt - 36*pow2(lmMsq) - 180*pow2(lmMst2))*pow3(
        Mst2)) + (7*pow2(MuSUSY)*(pow4(Msq)*(-48*pow2(Mst1)*pow2(Mst2)*(59572 -
        15390*B4 + 405*DN + 72225*lmMsq + 405*(-31 + 10*lmMsq)*lmMst1 - 45*(
        1817 + 90*lmMsq + 528*lmMst1)*lmMst2 - 1080*pow2(lmMst1) + 1080*(11 +
        12*lmMst1)*pow2(lmMst2) - 12960*pow3(lmMst2)) + 5*(769589 + 295488*B4 -
        7776*DN - 1432080*lmMsq - 144*(2161 + 810*lmMsq)*lmMst1 + 144*(17377 +
        810*lmMsq + 4428*lmMst1)*lmMst2 - 49248*pow2(lmMst1) - 2592*(179 + 96*
        lmMst1)*pow2(lmMst2) + 248832*pow3(lmMst2))*pow4(Mst1) + 103680*(-3 -
        4*lmMst2 + 3*pow2(lmMst2))*pow4(Mst2)) - 10800*pow2(Msq)*((983 + 36*
        lmMst1 + 36*lmMsq*(-2 + 3*lmMst1 - 3*lmMst2) + 36*lmMst2 - 108*lmMst1*
        lmMst2 + 108*pow2(lmMst2))*pow2(Mst2)*pow4(Mst1) + (509 + 12*lmMst1 +
        36*lmMsq*(-2 + lmMst1 - lmMst2) + 60*lmMst2 - 36*lmMst1*lmMst2 + 36*
        pow2(lmMst2))*pow2(Mst1)*pow4(Mst2)) - 2700*((2375 + 81*lmMst1 + 180*
        lmMsq*(-2 + 3*lmMst1 - 3*lmMst2) + 279*lmMst2 - 540*lmMst1*lmMst2 +
        540*pow2(lmMst2))*pow4(Mst1)*pow4(Mst2) + (1307 + 27*lmMst1 + 180*
        lmMsq*(-2 + lmMst1 - lmMst2) + 333*lmMst2 - 180*lmMst1*lmMst2 + 180*
        pow2(lmMst2))*pow2(Mst1)*pow6(Mst2))))/(pow2(Mst1)*pow5(Mst2)))/pow4(
        Msq)))/51030. + ((T1ep*(-4*pow2(Mst1)*(-2*Mst2*pow2(Mt)*pow2(s2t)*(-
        589*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 891*pow2(Mst2)*pow2(Sbeta)) - 14*
        s2t*(439*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 1208*pow2(Mst2)*pow2(Sbeta))
        *pow3(Mt) + 2737*Mt*pow2(Sbeta)*pow3(s2t)*pow4(Mst2) + 22676*Mst2*pow2(
        Sbeta)*pow4(Mt) - 284*pow2(Sbeta)*pow4(s2t)*pow5(Mst2)) + 21*pow2(Mst2)
        *(4*Mst2*pow2(Mt)*pow2(s2t)*(-(pow2(MuSUSY)*(-1 + pow2(Sbeta))) + 18*
        pow2(Mst2)*pow2(Sbeta)) + 64*s2t*(pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 2*
        pow2(Mst2)*pow2(Sbeta))*pow3(Mt) - 32*Mt*pow2(Sbeta)*pow3(s2t)*pow4(
        Mst2) - 304*Mst2*pow2(Sbeta)*pow4(Mt) + pow2(Sbeta)*pow4(s2t)*pow5(
        Mst2))))/(243.*pow5(Mst2)) - pow2(Mt)*pow2(MuSUSY)*((8*pow2(Mt)*(33934
        - 90*B4 + 90*D3 - 45*DN + 720*lmMst1 + 120*(163 + 24*lmMst1)*lmMst2 -
        120*lmMt - 10206*S2 - 720*(2 + lmMst1)*pow2(lmMst2) + 720*pow3(lmMst2))
        )/(405.*pow4(Mst2)) + pow2(s2t)*((5*(448 - 273*lmMst2 + 18*lmMsq*(8 -
        19*lmMst1 + 19*lmMst2) + 3*lmMst1*(43 + 114*lmMst2) - 342*pow2(lmMst2))
        )/(27.*pow2(Msq)) + (-919 + 64*lmMst1*(-2 + lmMst2) - 466*lmMst2 - 30*
        lmMsq*(-13 + 6*lmMst2) + 90*pow2(lmMsq) + 592*pow2(lmMst2))/(18.*pow2(
        Mst1)) + (92.43189300411522 - (76*B4)/9. + (80*D3)/9. - (14*DN)/3. + (
        545*lmMsq)/9. + 5*pow2(lmMsq) - (2*lmMst1*(-502 + 210*lmMsq + 45*pow2(
        lmMsq)))/9. - (25*pow2(lmMst1))/9. + (lmMst2*(-6175 + 1308*lmMst1 + 90*
        lmMsq*(11 + 6*lmMst1) + 270*pow2(lmMsq) + 96*pow2(lmMst1)))/27. - ((371
        + 180*lmMsq + 336*lmMst1)*pow2(lmMst2))/9. + ((98.88888888888889 + 30*(
        1 - 2*lmMsq)*lmMst1 - 30*lmMst2 + 60*(lmMsq + lmMst1)*lmMst2 - 60*pow2(
        lmMst2))*pow2(Mst1))/pow2(Msq) + (304*pow3(lmMst2))/9.)/pow2(Mst2) + (
        pow2(Mst2)*((5*(3125 - 876*lmMst2 + 540*lmMsq*(1 - 4*lmMst1 + 4*lmMst2)
        + 48*lmMst1*(7 + 45*lmMst2) - 2160*pow2(lmMst2)))/pow4(Msq) + (768*(2 +
        lmMst2 - 3*pow2(lmMst2)))/pow4(Mst1)))/216. + pow2(Mst1)*((5*(493 + 5*(
        -17 + 60*lmMsq)*lmMst2 + lmMst1*(85 - 300*lmMsq + 300*lmMst2) - 300*
        pow2(lmMst2)))/(36.*pow4(Msq)) - (182.90272153635118 + (140*B4)/9. -
        16*D3 + (74*DN)/9. - (20*lmMsq)/3. + lmMst1*(-174.74765432098766 + 40*
        lmMsq + 10*pow2(lmMsq)) + lmMst2*(252.96987654320986 - (6002*lmMst1)/
        45. - 20*lmMsq*(2 + lmMst1) - 10*pow2(lmMsq) - (106*pow2(lmMst1))/3.) +
        (1556*pow2(lmMst1))/45. + (98.8 + 20*lmMsq + (878*lmMst1)/9.)*pow2(
        lmMst2) + (10*pow3(lmMst1))/9. - (190*pow3(lmMst2))/3.)/pow4(Mst2)) + (
        (47120*OepS2 - 54*(620417 + 17670*lmMst1 - 17670*lmMst2)*S2)*pow2(Mst1)
        + 3*(280*OepS2 - 81*(65819 + 70*lmMst1 - 70*lmMst2)*S2)*pow2(Mst2))/(
        3645.*pow4(Mst2)) - (5*(32*(4 - 15*lmMsq + 15*lmMst2)*pow2(Msq)*pow2(
        Mst2) + 3*(9 - 140*lmMsq + 140*lmMst2)*pow4(Mst2)))/(72.*pow2(Mst1)*
        pow4(Msq))) - (Mt*s2t*(pow4(Msq)*(12*pow2(Mst1)*pow2(Mst2)*(238288 -
        61560*B4 + 1620*DN + 288900*lmMsq + 1620*(-31 + 10*lmMsq)*lmMst1 - 180*
        (1817 + 90*lmMsq + 528*lmMst1)*lmMst2 + 2240*OepS2 - 81*(-1373 + 560*
        lmMst1 - 560*lmMst2)*S2 - 4320*pow2(lmMst1) + 4320*(11 + 12*lmMst1)*
        pow2(lmMst2) - 51840*pow3(lmMst2)) - 5*(769589 + 295488*B4 - 7776*DN -
        1432080*lmMsq - 144*(2161 + 810*lmMsq)*lmMst1 + 144*(17377 + 810*lmMsq
        + 4428*lmMst1)*lmMst2 - 98336*OepS2 + 756*(259 + 2634*lmMst1 - 2634*
        lmMst2)*S2 - 49248*pow2(lmMst1) - 2592*(179 + 96*lmMst1)*pow2(lmMst2) +
        248832*pow3(lmMst2))*pow4(Mst1) + 103680*(3 + 4*lmMst2 - 3*pow2(lmMst2)
        )*pow4(Mst2)) + 10800*pow2(Msq)*((983 + 36*lmMst1 + 36*lmMsq*(-2 + 3*
        lmMst1 - 3*lmMst2) + 36*lmMst2 - 108*lmMst1*lmMst2 + 108*pow2(lmMst2))*
        pow2(Mst2)*pow4(Mst1) + (509 + 12*lmMst1 + 36*lmMsq*(-2 + lmMst1 -
        lmMst2) + 60*lmMst2 - 36*lmMst1*lmMst2 + 36*pow2(lmMst2))*pow2(Mst1)*
        pow4(Mst2)) + 2700*((2375 + 81*lmMst1 + 180*lmMsq*(-2 + 3*lmMst1 - 3*
        lmMst2) + 279*lmMst2 - 540*lmMst1*lmMst2 + 540*pow2(lmMst2))*pow4(Mst1)
        *pow4(Mst2) + (1307 + 27*lmMst1 + 180*lmMsq*(-2 + lmMst1 - lmMst2) +
        333*lmMst2 - 180*lmMst1*lmMst2 + 180*pow2(lmMst2))*pow2(Mst1)*pow6(
        Mst2))))/(7290.*pow2(Mst1)*pow4(Msq)*pow5(Mst2))))/pow2(Sbeta) - (
        MuSUSY*(-(pow2(Mt)*pow2(s2t)*((-4*Mst2*((5*(509 + lmMst1*(12 - 36*
        lmMst2) + 36*lmMsq*(-2 + lmMst1 - lmMst2) + 60*lmMst2 + 36*pow2(lmMst2)
        ))/pow2(Msq) + (48*(3 + 4*lmMst2 - 3*pow2(lmMst2)))/pow2(Mst1)))/9. - (
        524.3654320987654 - 152*B4 + 4*DN + (2140*lmMsq)/3. + 4*(-31 + 10*
        lmMsq)*lmMst1 - (4*(2009 + 90*lmMsq + 528*lmMst1)*lmMst2)/9. - (32*
        pow2(lmMst1))/3. + (32*(17 + 12*lmMst1)*pow2(lmMst2))/3. + (40*(79 + 4*
        lmMst1*(1 + 3*lmMsq - 3*lmMst2) - 4*(1 + 3*lmMsq)*lmMst2 + 12*pow2(
        lmMst2))*pow2(Mst1))/(3.*pow2(Msq)) - 128*pow3(lmMst2))/Mst2 + ((-56*
        OepS2*(415*pow2(Mst1) + 24*pow2(Mst2)))/243. + (S2*((21422 + 87150*
        lmMst1 - 87150*lmMst2)*pow2(Mst1) + 9*(-1373 + 560*lmMst1 - 560*lmMst2)
        *pow2(Mst2)))/45.)/pow3(Mst2) + (pow2(Mst1)*(1380.1236625514402 + 152*
        B4 - 4*DN - 760*lmMsq - (8*(1499 + 270*lmMsq)*lmMst1)/27. + (8*(5963 +
        270*lmMsq + 1422*lmMst1)*lmMst2)/27. - (184*pow2(lmMst1))/3. - 8*(45 +
        16*lmMst1)*pow2(lmMst2) + 128*pow3(lmMst2)))/pow3(Mst2) - (5*(6*Mst2*(
        178 + lmMst1*(9 + 60*lmMsq - 60*lmMst2) - 9*lmMst2 - 60*lmMsq*lmMst2 +
        60*pow2(lmMst2))*pow2(Mst1) + (1307 + 27*lmMst1 + 180*lmMsq*(-2 +
        lmMst1 - lmMst2) + 333*lmMst2 - 180*lmMst1*lmMst2 + 180*pow2(lmMst2))*
        pow3(Mst2)))/(9.*pow4(Msq)))) + (4*Mt*T1ep*(21*pow2(Mst2)*(36*Mst2*s2t*
        pow2(Mt) - 24*Mt*pow2(Mst2)*pow2(s2t) + 32*pow3(Mt) + pow3(Mst2)*pow3(
        s2t)) + pow2(Mst1)*(4320*Mst2*s2t*pow2(Mt) - 8715*Mt*pow2(Mst2)*pow2(
        s2t) + 17584*pow3(Mt) + 1157*pow3(Mst2)*pow3(s2t))))/(243.*pow5(Mst2))
        - Mt*pow3(s2t)*(143.48744855967078 - (76*B4)/9. + (80*D3)/9. - (14*DN)/
        3. + (350*lmMsq)/9. - (2*lmMst1*(-178 + 70*lmMsq + 15*pow2(lmMsq)))/3.
         - (25*pow2(lmMst1))/
        9. + (2*lmMst2*(-2738 + 606*lmMst1 + 90*lmMsq*(7 + 3*lmMst1) + 135*
        pow2(lmMsq) + 48*pow2(lmMst1)))/27. - ((667 + 180*lmMsq + 336*lmMst1)*
        pow2(lmMst2))/9. + (8*OepS2*(21 + (1157*pow2(Mst1))/pow2(Mst2)))/729. +
        (((10*(496 - 18*lmMsq*(2 + 19*lmMst1 - 19*lmMst2) - 93*lmMst2 + 3*
        lmMst1*(43 + 114*lmMst2) - 342*pow2(lmMst2)))/pow2(Msq) + (3*(-1047 +
        64*lmMst1*(-2 + lmMst2) - 530*lmMst2 - 30*lmMsq*(-13 + 6*lmMst2) + 90*
        pow2(lmMsq) + 784*pow2(lmMst2)))/pow2(Mst1))*pow2(Mst2))/54. - (S2*((
        648463 + 34710*lmMst1 - 34710*lmMst2)*pow2(Mst1) + 9*(65819 + 70*lmMst1
        - 70*lmMst2)*pow2(Mst2)))/(135.*pow2(Mst2)) + pow2(Mst1)*((5*(86 +
        lmMst1*(33 - 18*lmMst2) + 18*lmMsq*(-8 + lmMst1 - lmMst2) + 111*lmMst2
        + 18*pow2(lmMst2)))/(27.*pow2(Msq)) - (275.3346145404664 + (64*B4)/9. -
        (64*D3)/9. + (32*DN)/9. + (485*lmMsq)/9. - (4*(31991 + 3375*lmMsq)*
        lmMst1)/2025. + 5*pow2(lmMsq) + (159*pow2(lmMst1))/5. - (lmMst2*(-49139
        + 6750*lmMsq + 171990*lmMst1 + 64350*pow2(lmMst1)))/2025. + ((2591 +
        2710*lmMst1)*pow2(lmMst2))/45. + (10*pow3(lmMst1))/9. - (266*pow3(
        lmMst2))/9.)/pow2(Mst2)) + (304*pow3(lmMst2))/9. + (32*(2 + lmMst2 - 3*
        pow2(lmMst2))*pow4(Mst2))/(9.*pow4(Mst1)) + (5*((-167 + 180*lmMsq*(-3 +
        2*lmMst1 - 2*lmMst2) + 366*lmMst2 - 6*lmMst1*(-29 + 60*lmMst2) + 360*
        pow2(lmMst2))*pow2(Mst2)*pow4(Mst1) + 96*(-4 + 15*lmMsq - 15*lmMst2)*
        pow2(Msq)*pow4(Mst2) + 2*(1603 - 360*lmMsq*(1 + 3*lmMst1 - 3*lmMst2) +
        192*lmMst2 + 24*lmMst1*(7 + 45*lmMst2) - 1080*pow2(lmMst2))*pow2(Mst1)*
        pow4(Mst2) + 9*(-9 + 140*lmMsq - 140*lmMst2)*pow6(Mst2)))/(216.*pow2(
        Mst1)*pow4(Msq))) + (pow3(Mt)*(100*Mt*pow2(Mst1)*(-2*pow4(Msq)*(-3*(
        4455287 - 18900*lmMsq*(5 + 6*lmMst1 - 6*lmMst2) + 493080*lmMst2 + 1260*
        (31 + 56*lmMst2)*lmMt - 15680*OepS2 - 648*(1471 + 490*lmMst2)*S2 +
        3780*lmMst1*(-83 + 96*lmMst2 - 16*lmMt + 84*S2) + 30240*pow2(lmMst1) -
        418320*pow2(lmMst2) + 15120*pow2(lmMt))*pow2(Mst1)*pow2(Mst2) - 2*(
        15392987 + 7445655*lmMst2 + 28350*lmMsq*(43 - 12*lmMst1 + 18*lmMst2 -
        6*lmMt) - 615440*OepS2 - 54*(80377 + 230790*lmMst2)*S2 - 11340*(35 + 4*
        lmMst2 - 4*lmMt)*pow2(lmMst1) + 1361745*pow2(lmMst2) - 945*lmMt*(-840 +
        13*lmMst2 + 48*pow2(lmMst2)) - 315*lmMst1*(17917 + 1599*lmMst2 + 201*
        lmMt - 39564*S2 + 432*pow2(lmMst2) - 432*pow2(lmMt)) - 22680*(17 + 6*
        lmMst2)*pow2(lmMt) + 181440*pow3(lmMst2))*pow4(Mst1) + 181440*(3 + 4*
        lmMst2 - 3*pow2(lmMst2))*pow4(Mst2)) + 37800*pow2(Msq)*((113 - 18*
        lmMst1*(1 + 2*lmMsq - 2*lmMst2) + 18*lmMst2 + 36*lmMsq*lmMst2 - 36*
        pow2(lmMst2))*pow2(Mst2)*pow4(Mst1) + (137 - 12*lmMst1*(1 + 3*lmMsq -
        3*lmMst2) + 6*lmMst2 + 36*lmMsq*lmMst2 + 6*lmMt - 36*pow2(lmMst2))*
        pow2(Mst1)*pow4(Mst2)) + 4725*(6*(241 + 4*(5 + 18*lmMsq)*lmMst2 +
        lmMst1*(-22 - 72*lmMsq + 72*lmMst2) + 2*lmMt - 72*pow2(lmMst2))*pow4(
        Mst1)*pow4(Mst2) + (1391 - 48*lmMst2 + 18*lmMst1*(-3 + 20*lmMst2) + 90*
        lmMt - 72*lmMst2*lmMt + 12*lmMsq*(1 - 30*lmMst1 + 36*lmMst2 + 6*lmMt) -
        72*pow2(lmMsq) - 360*pow2(lmMst2))*pow2(Mst1)*pow6(Mst2))) - 9*Mst2*
        s2t*(2*pow4(Msq)*(75*pow2(Mst2)*(2025733 - 3360*B4 + 3360*D3 - 1680*DN
        + 88200*lmMsq + 115920*lmMst1 - 6720*lmMt + 7840*OepS2 - 324*(523 +
        490*lmMst1 - 490*lmMst2)*S2 + 18900*pow2(lmMsq) + 17220*pow2(lmMst1) -
        140*lmMst2*(-6065 + 270*lmMsq - 1002*lmMst1 + 96*pow2(lmMst1)) - 420*(
        143 + 32*lmMst1)*pow2(lmMst2) + 26880*pow3(lmMst2))*pow4(Mst1) + 15750*
        (599 - 64*lmMst1*(-2 + lmMst2) + 530*lmMst2 + 30*lmMsq*(-13 + 6*lmMst2)
        - 90*pow2(lmMsq) - 208*pow2(lmMst2))*pow2(Mst1)*pow4(Mst2) + 8*(
        22601752 - 31500*B4 + 31500*D3 - 15750*DN - 1299375*lmMsq + 105*(56281
        + 3375*lmMsq)*lmMst1 - 63000*(-17 + 5*lmMst1 - 5*lmMst2)*lmMt + 420000*
        OepS2 - 675*(22243 + 12600*lmMst1 - 12600*lmMst2)*S2 + 264600*pow2(
        lmMst1) - 105*lmMst2*(-148919 + 3375*lmMsq - 50160*lmMst1 + 1425*pow2(
        lmMst1)) - 1575*(3032 + 545*lmMst1)*pow2(lmMst2) + 7875*pow3(lmMst1) +
        1000125*pow3(lmMst2))*pow6(Mst1) - 1008000*(2 + lmMst2 - 3*pow2(lmMst2)
        )*pow6(Mst2)) + 315000*pow2(Msq)*((-17 + 60*lmMsq - 60*lmMst2)*pow4(
        Mst1)*pow4(Mst2) + 2*pow2(Mst2)*pow6(Mst1) + 4*(4 - 15*lmMsq + 15*
        lmMst2)*pow2(Mst1)*pow6(Mst2)) - 39375*((31 + 12*lmMsq - 12*lmMst2)*
        pow4(Mst2)*pow6(Mst1) - 2*(1 + 216*lmMsq - 216*lmMst2)*pow4(Mst1)*pow6(
        Mst2) + 3*(-9 + 140*lmMsq - 140*lmMst2)*pow2(Mst1)*pow8(Mst2)))))/(
        1.27575e6*pow4(Msq)*pow4(Mst1)*pow5(Mst2))))/Tbeta))) + xDR2DRMOD*(
        Al4p*(-((2*Dmglst2*lmMst2 + Mst2 + lmMst2*Mst2)*twoLoopFlag*(-(pow2(
        Mst2)*pow2(s2t)*pow4(Mst1)*(-8*(lmMst1 - lmMst2)*Tbeta*pow2(Mt)*pow2(
        MuSUSY)*(-1 + pow2(Sbeta)) + 4*Mt*(-(MuSUSY*s2t) + 2*Mt*Tbeta)*pow2(
        Mst2)*pow2(Sbeta) + (1 - 2*lmMst1 + 2*lmMst2)*Tbeta*pow2(s2t)*pow2(
        Sbeta)*pow4(Mst2))) - pow2(Mst1)*pow4(Mst2)*(-4*Tbeta*pow2(Mt)*pow2(
        s2t)*((-1 + 2*lmMst1 - 2*lmMst2)*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 4*
        pow2(Mst2)*pow2(Sbeta)) + pow2(Sbeta)*(16*MuSUSY*s2t*pow3(Mt) + 8*(-
        lmMst1 + lmMst2)*Mt*MuSUSY*pow2(Mst2)*pow3(s2t) - 16*Tbeta*pow4(Mt) + (
        1 + 2*lmMst1 - 2*lmMst2)*Tbeta*pow4(Mst2)*pow4(s2t))) + Tbeta*pow2(s2t)
        *(8*(lmMst1 - lmMst2)*pow2(Mt)*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + pow2(
        s2t)*pow2(Sbeta)*pow4(Mst2))*pow6(Mst1) + (-4*Tbeta*pow2(Mt)*pow2(s2t)*
        (pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 2*pow2(Mst2)*pow2(Sbeta)) + pow2(
        Sbeta)*(16*MuSUSY*s2t*pow3(Mt) - 4*Mt*MuSUSY*pow2(Mst2)*pow3(s2t) + 16*
        Tbeta*pow4(Mt) + Tbeta*pow4(Mst2)*pow4(s2t)))*pow6(Mst2)))/(6.*Tbeta*
        pow2(Mst1)*pow2(Sbeta)*pow5(Mst2)) - xDmglst2*pow2(Dmglst2)*(-((2 -
        lmMst2)*twoLoopFlag*(-(pow2(Mst2)*pow2(s2t)*pow4(Mst1)*(-8*(lmMst1 -
        lmMst2)*Tbeta*pow2(Mt)*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 4*Mt*(-(
        MuSUSY*s2t) + 2*Mt*Tbeta)*pow2(Mst2)*pow2(Sbeta) + (1 - 2*lmMst1 + 2*
        lmMst2)*Tbeta*pow2(s2t)*pow2(Sbeta)*pow4(Mst2))) - pow2(Mst1)*pow4(
        Mst2)*(-4*Tbeta*pow2(Mt)*pow2(s2t)*((-1 + 2*lmMst1 - 2*lmMst2)*pow2(
        MuSUSY)*(-1 + pow2(Sbeta)) + 4*pow2(Mst2)*pow2(Sbeta)) + pow2(Sbeta)*(
        16*MuSUSY*s2t*pow3(Mt) + 8*(-lmMst1 + lmMst2)*Mt*MuSUSY*pow2(Mst2)*
        pow3(s2t) - 16*Tbeta*pow4(Mt) + (1 + 2*lmMst1 - 2*lmMst2)*Tbeta*pow4(
        Mst2)*pow4(s2t))) + Tbeta*pow2(s2t)*(8*(lmMst1 - lmMst2)*pow2(Mt)*pow2(
        MuSUSY)*(-1 + pow2(Sbeta)) + pow2(s2t)*pow2(Sbeta)*pow4(Mst2))*pow6(
        Mst1) + (-4*Tbeta*pow2(Mt)*pow2(s2t)*(pow2(MuSUSY)*(-1 + pow2(Sbeta)) +
        2*pow2(Mst2)*pow2(Sbeta)) + pow2(Sbeta)*(16*MuSUSY*s2t*pow3(Mt) - 4*Mt*
        MuSUSY*pow2(Mst2)*pow3(s2t) + 16*Tbeta*pow4(Mt) + Tbeta*pow4(Mst2)*
        pow4(s2t)))*pow6(Mst2)))/(6.*Tbeta*pow2(Mst1)*pow2(Sbeta)*pow6(Mst2)) +
        Al4p*threeLoopFlag*(pow4(s2t)*((pow2(Mst1)*(73 + 2824*lmMst2 - 90*
        lmMsq*(-1 + 2*lmMst1 - 2*lmMst2)*(-13 + 6*lmMst2) + 270*(-1 + 2*lmMst1
        - 2*lmMst2)*pow2(lmMsq) + 1344*(-2 + lmMst2)*pow2(lmMst1) + 60*pow2(
        lmMst2) - 6*lmMst1*(643 - 686*lmMst2 + 176*pow2(lmMst2)) - 288*pow3(
        lmMst2)))/216. - ((615080*OepS2 - 27*(669989 + 461310*lmMst1 - 461310*
        lmMst2)*S2)*pow4(Mst1))/(43740.*pow2(Mst2)) + ((5*(133 + lmMst1*(48 -
        36*lmMst2) + 36*lmMsq*(2 + lmMst1 - lmMst2) - 120*lmMst2 + 36*pow2(
        lmMst2)))/(216.*pow2(Msq)) + (150.8801470186938 - (87576071*lmMst2)/
        2.3814e6 + (5*lmMsq*(29 - 78*lmMst1 + 78*lmMst2))/36. + (
        1.4137566137566138 + (143*lmMst2)/18.)*pow2(lmMst1) + lmMst1*(
        27.663925002099607 + (12028*lmMst2)/945. - (143*pow2(lmMst2))/18.) - (
        13364*pow2(lmMst2))/945. - (143*pow3(lmMst1))/54. + (143*pow3(lmMst2))/
        54.)/pow2(Mst2))*pow4(Mst1) - (pow2(Mst2)*(240*(-4 + 15*lmMsq - 15*
        lmMst2)*(1 - 2*lmMst1 + 2*lmMst2)*pow2(Msq)*pow2(Mst1) + 2*(-2089 +
        3644*lmMst2 - 90*lmMsq*(1 + 2*lmMst1 - 2*lmMst2)*(-13 + 6*lmMst2) +
        270*(1 + 2*lmMst1 - 2*lmMst2)*pow2(lmMsq) - 192*(-2 + lmMst2)*pow2(
        lmMst1) + 5916*pow2(lmMst2) + 6*lmMst1*(-1123 - 338*lmMst2 + 336*pow2(
        lmMst2)) - 1824*pow3(lmMst2))*pow4(Msq) + 5*(127 - 180*lmMsq*(2 +
        lmMst1 - lmMst2) + 447*lmMst2 + 3*lmMst1*(-29 + 60*lmMst2) - 180*pow2(
        lmMst2))*pow4(Mst1)))/(432.*pow4(Msq)) + (((40*(1 + 2*lmMst1 - 2*
        lmMst2)*(4 - 15*lmMsq + 15*lmMst2))/pow2(Msq) + (-1091 + 64*lmMst1*(-2
        + lmMst2) - 594*lmMst2 - 30*lmMsq*(-13 + 6*lmMst2) + 90*pow2(lmMsq) +
        976*pow2(lmMst2))/pow2(Mst1))*pow4(Mst2))/72. + (5*(9 - 140*lmMsq +
        140*lmMst2)*((1 - 2*lmMst1 + 2*lmMst2)*pow2(Mst1) + (1 + 2*lmMst1 - 2*
        lmMst2)*pow2(Mst2))*pow4(Mst2))/(96.*pow4(Msq)) + ((8*(2 + lmMst2 - 3*
        pow2(lmMst2))*pow2(Msq) + 5*(-4 + 15*lmMsq - 15*lmMst2)*pow2(Mst1))*
        pow6(Mst2))/(9.*pow2(Msq)*pow4(Mst1))) - (Mt*pow3(s2t)*(12150*(4*pow2(
        Msq)*pow2(Mst2) + pow4(Mst2))*pow6(Mst1) - pow4(Msq)*(25920*((77 + 59*
        lmMst2 - 6*lmMst1*(3 + lmMst2) - 12*pow2(lmMst2))*pow2(Mst2)*pow4(Mst1)
        + pow2(Mst1)*(31 - 101*lmMst2 + 6*lmMst1*(7 + 8*lmMst2 - 6*pow2(lmMst2)
        ) - 48*pow2(lmMst2) + 36*pow3(lmMst2))*pow4(Mst2)) - (26480791 +
        4317750*lmMst2 + 291600*lmMsq*(5 - 3*lmMst1 + 3*lmMst2) - 2529760*OepS2
        - 162*(-411733 + 316220*lmMst2)*S2 + 19440*(-61 + 24*lmMst2)*pow2(
        lmMst1) - 2118960*pow2(lmMst2) - 90*lmMst1*(71303 - 36720*lmMst2 -
        569196*S2 + 5184*pow2(lmMst2)) - 155520*pow3(lmMst1) + 155520*pow3(
        lmMst2))*pow6(Mst1) + 155520*(-3 - 4*lmMst2 + 3*pow2(lmMst2))*pow6(
        Mst2))))/(21870.*pow2(Mst1)*pow3(Mst2)*pow4(Msq)) + (pow3(Mt)*(20*s2t*
        pow2(Mst1)*(14175*(-5*(688 + 27*lmMst1 + 36*lmMsq*(-2 + 5*lmMst1 - 5*
        lmMst2) + 45*lmMst2 - 180*lmMst1*lmMst2 + 180*pow2(lmMst2))*pow2(Mst2)*
        pow2(MuSUSY) - 8*pow2(Msq)*(3*(-19 + 2*lmMst1 - 4*lmMst2 + 2*lmMt)*
        pow2(Mst2) + (727 + 30*lmMst1 + 18*lmMsq*(-2 + 5*lmMst1 - 5*lmMst2) +
        6*lmMst2 - 90*lmMst1*lmMst2 + 90*pow2(lmMst2))*pow2(MuSUSY)) - 6*(5 +
        2*lmMst1 - 4*lmMst2 + 2*lmMt)*pow4(Mst2))*pow6(Mst1) + pow4(Msq)*(-
        30240*pow2(Mst1)*pow2(Mst2)*((553 + 194*lmMst2 + 18*lmMst1*(-1 + 4*
        lmMst2 - 2*lmMt) + 6*(-5 + 8*lmMst2)*lmMt - 192*pow2(lmMst2))*pow2(
        Mst2) - 3*pow2(MuSUSY)*(5 + 149*lmMst2 + 12*pow2(lmMst2) + 6*lmMst1*(-7
        - 8*lmMst2 + 6*pow2(lmMst2)) - 36*pow3(lmMst2))) - 90720*(6*pow2(Mst2)*
        (116 + lmMst2*(181 - 30*lmMt) - 36*lmMt - 4*(-14 + lmMt)*pow2(lmMst2) -
        2*lmMst1*(42 + lmMst2*(21 - 2*lmMt) - 8*lmMt + 2*pow2(lmMst2)) + 4*
        pow3(lmMst2)) + pow2(MuSUSY)*(85 - 215*lmMst2 + lmMst1*(66 + 90*lmMst2
        - 72*pow2(lmMst2)) - 54*pow2(lmMst2) + 72*pow3(lmMst2)))*pow4(Mst1) +
        544320*(3 + 4*lmMst2 - 3*pow2(lmMst2))*(2*pow2(Mst2) + pow2(MuSUSY))*
        pow4(Mst2) + (370047113 - 300957300*lmMst2 - 45543680*OepS2 - 4968*(-
        135731 + 185640*lmMst2)*S2 - 136080*(339 + 62*lmMst2 - 59*lmMt)*pow2(
        lmMst1) - 210470400*pow2(lmMst2) + 1020600*lmMsq*(97 + 94*lmMst2 - 4*
        lmMst1*(17 + 3*lmMst2 - 3*lmMt) - 2*(13 + 6*lmMst2)*lmMt + 12*pow2(
        lmMst2)) + 5670*lmMt*(22061 + 24472*lmMst2 + 6024*pow2(lmMst2)) -
        1632960*(7 + 6*lmMst2)*pow2(lmMt) + 630*lmMst1*(98585 + 11160*lmMst2*(
        31 - 6*lmMt) - 140760*lmMt + 1463904*S2 + 52056*pow2(lmMst2) + 15552*
        pow2(lmMt)) + 136080*pow3(lmMst1) - 24494400*pow3(lmMst2))*pow6(Mst1)))
        + 189*Mst2*Mt*(3375*pow2(Mst1)*pow2(Mst2)*((-9 + 140*lmMsq - 140*
        lmMst2)*(pow2(Mst1) + pow2(Mst2))*pow4(Mst2) - 4*(-4 + lmMst1 - 2*
        lmMst2 + lmMt)*pow6(Mst1)) + 4*pow4(Msq)*((772621 + 6750*lmMsq*(13 - 6*
        lmMst2) + 516606*lmMst2 - 28800*lmMst1*(8 + 2*lmMst2 - lmMt) - 480*(427
        + 117*lmMst2)*lmMt + 20250*pow2(lmMsq) + 102960*pow2(lmMst2))*pow2(
        Mst2)*pow4(Mst1) + 225*(-451 + 64*lmMst1*(-2 + lmMst2) - 722*lmMst2 -
        30*lmMsq*(-13 + 6*lmMst2) + 90*pow2(lmMsq) + 208*pow2(lmMst2))*pow2(
        Mst1)*pow4(Mst2) + 14400*(283 + lmMst2*(489 - 93*lmMt) - 94*lmMt + 9*(
        21 - 2*lmMt)*pow2(lmMst2) - 2*lmMst1*(115 - 9*lmMst2*(-8 + lmMt) - 24*
        lmMt + 9*pow2(lmMst2)) + 18*pow3(lmMst2))*pow6(Mst1) + 14400*(2 +
        lmMst2 - 3*pow2(lmMst2))*pow6(Mst2)) - 9000*pow2(Msq)*(4*(4 - 15*lmMsq
        + 15*lmMst2)*pow2(Mst1)*(pow2(Mst1) + pow2(Mst2))*pow4(Mst2) + 3*(127 +
        lmMst2*(68 - 6*lmMt) - 24*lmMt + lmMst1*(-44 - 6*lmMst2 + 6*lmMt) + 6*
        pow2(lmMst2))*pow8(Mst1)))))/(765450.*pow4(Msq)*pow4(Mst1)*pow5(Mst2))
        + (s2t*((T1ep*(-58056*Mst2*s2t*pow2(Mt) - 126488*Mt*pow2(Mst2)*pow2(
        s2t) + 1301248*pow3(Mt) + 15377*pow3(Mst2)*pow3(s2t))*pow4(Mst1))/729.
         + (pow2(Mt)*pow2(MuSUSY)*(4*pow4(Msq)*(-(pow2(Mst2)*(Mst2*s2t*(3305 +
        6632*lmMst2 + 90*lmMsq*(1 - 2*lmMst1 + 2*lmMst2)*(-13 + 6*lmMst2) +
        270*(-1 + 2*lmMst1 - 2*lmMst2)*pow2(lmMsq) - 192*(-2 + lmMst2)*pow2(
        lmMst1) + 1788*pow2(lmMst2) + 6*lmMst1*(-995 - 402*lmMst2 + 336*pow2(
        lmMst2)) - 1824*pow3(lmMst2)) + 128*Mt*(5 + 149*lmMst2 + 12*pow2(
        lmMst2) + 6*lmMst1*(-7 - 8*lmMst2 + 6*pow2(lmMst2)) - 36*pow3(lmMst2)))
        *pow4(Mst1)) - 3*(Mst2*s2t*(835 - 64*lmMst1*(-2 + lmMst2) + 466*lmMst2
        + 30*lmMsq*(-13 + 6*lmMst2) - 90*pow2(lmMsq) - 592*pow2(lmMst2)) + 256*
        Mt*(3 + 4*lmMst2 - 3*pow2(lmMst2)))*pow2(Mst1)*pow4(Mst2) + 2*(-3*Mst2*
        s2t*(672 + lmMst2*(1507 - 390*lmMsq - 90*pow2(lmMsq)) - 288*(-2 +
        lmMst2)*pow2(lmMst1) + 18*(49 + 10*lmMsq)*pow2(lmMst2) + lmMst1*(-1411
        - 1458*lmMst2 - 30*lmMsq*(-13 + 6*lmMst2) + 90*pow2(lmMsq) + 848*pow2(
        lmMst2)) - 560*pow3(lmMst2)) + 64*Mt*(85 - 215*lmMst2 + lmMst1*(66 +
        90*lmMst2 - 72*pow2(lmMst2)) - 54*pow2(lmMst2) + 72*pow3(lmMst2)))*
        pow6(Mst1) + 192*s2t*(2 + lmMst2 - 3*pow2(lmMst2))*pow7(Mst2)) + 20*
        pow2(Msq)*(24*(-4 + 15*lmMsq - 15*lmMst2)*s2t*pow2(Mst1)*(pow2(Mst1) +
        pow2(Mst2))*(-2*lmMst1*pow2(Mst1) + 2*lmMst2*pow2(Mst1) + pow2(Mst2))*
        pow3(Mst2) + (-3*Mst2*s2t*(337 + lmMst1*(50 + 24*lmMsq - 24*lmMst2) -
        50*lmMst2 - 24*lmMsq*lmMst2 + 24*pow2(lmMst2)) + 32*Mt*(727 + 30*lmMst1
        + 18*lmMsq*(-2 + 5*lmMst1 - 5*lmMst2) + 6*lmMst2 - 90*lmMst1*lmMst2 +
        90*pow2(lmMst2)))*pow8(Mst1)) + 5*(9*(-9 + 140*lmMsq - 140*lmMst2)*s2t*
        pow2(Mst1)*(pow2(Mst1) + pow2(Mst2))*(-2*lmMst1*pow2(Mst1) + 2*lmMst2*
        pow2(Mst1) + pow2(Mst2))*pow5(Mst2) + 4*(-3*Mst2*s2t*(247 + lmMst1*(29
        + 60*lmMsq - 60*lmMst2) - 29*lmMst2 - 60*lmMsq*lmMst2 + 60*pow2(lmMst2)
        ) + 20*Mt*(688 + 27*lmMst1 + 36*lmMsq*(-2 + 5*lmMst1 - 5*lmMst2) + 45*
        lmMst2 - 180*lmMst1*lmMst2 + 180*pow2(lmMst2)))*pow2(Mst2)*pow8(Mst1)))
        )/(216.*pow2(Sbeta)*pow4(Msq)*pow4(Mst1))))/pow5(Mst2) - (MuSUSY*((76*
        Mt*(-6369*Mt + 992*Mst2*s2t)*T1ep*pow2(s2t)*pow4(Mst1))/(729.*pow5(
        Mst2)) - (pow2(Mt)*pow2(s2t)*(-((322696*OepS2 - 81*(86631 + 80674*
        lmMst1 - 80674*lmMst2)*S2)*pow4(Mst1))/729. + (4692.605281207133 + 152*
        B4 - 4*DN - 560*lmMsq - (1409.7654320987654 + 200*lmMsq)*lmMst1 - 224*
        pow2(lmMst1) + lmMst2*(2593.7654320987654 + 200*lmMsq + (2176*lmMst1)/
        3. + 64*pow2(lmMst1)) - (32*(47 + 6*lmMst1)*pow2(lmMst2))/3. - (64*
        pow3(lmMst1))/3. + (64*pow3(lmMst2))/3.)*pow4(Mst1) - (4*pow2(Mst2)*(
        16*pow2(Msq)*pow2(Mst1)*(15 - 11*lmMst2 + lmMst1*(4 + 7*lmMst2 - 6*
        pow2(lmMst2)) - 7*pow2(lmMst2) + 6*pow3(lmMst2)) + 5*(157 + 8*lmMst1*(1
        + 3*lmMsq - 3*lmMst2) - 8*(1 + 3*lmMsq)*lmMst2 + 24*pow2(lmMst2))*pow4(
        Mst1)))/(3.*pow2(Msq)) - ((32*(13 - 125*lmMst2 + 6*lmMst1*(7 + 8*lmMst2
        - 6*pow2(lmMst2)) - 30*pow2(lmMst2) + 36*pow3(lmMst2)) + (15*(355 + 6*
        lmMst1*(3 + 20*lmMsq - 20*lmMst2) - 6*(3 + 20*lmMsq)*lmMst2 + 120*pow2(
        lmMst2))*pow4(Mst1))/pow4(Msq))*pow4(Mst2))/9. + (64*(3 + 4*lmMst2 - 3*
        pow2(lmMst2))*pow6(Mst2))/(3.*pow2(Mst1))))/pow5(Mst2) - Mt*pow3(s2t)*(
        14.814814814814815 + (lmMst1*(-353 + 130*lmMsq + 30*pow2(lmMsq)))/3. +
        (64*pow2(lmMst1))/9. - (lmMst2*(-2617 + 1110*lmMst1 + 90*lmMsq*(13 + 6*
        lmMst1) + 270*pow2(lmMsq) + 96*pow2(lmMst1)))/27. + (2*(99 + 30*lmMsq +
        56*lmMst1)*pow2(lmMst2))/3. + (20*(-4 + 15*lmMsq - 15*lmMst2)*pow2(
        Mst1))/(9.*pow2(Msq)) - ((-963 + 64*lmMst1*(-2 + lmMst2) - 530*lmMst2 -
        30*lmMsq*(-13 + 6*lmMst2) + 90*pow2(lmMsq) + 784*pow2(lmMst2))*pow2(
        Mst2))/(18.*pow2(Mst1)) + (pow2(Mst1)*(727 + 90*lmMsq*(13 - 6*lmMst2) +
        2410*lmMst2 + 270*pow2(lmMsq) - 1536*(-2 + lmMst2)*pow2(lmMst1) + 3504*
        pow2(lmMst2) + 192*lmMst1*(-13 - 33*lmMst2 + 16*pow2(lmMst2)) - 1536*
        pow3(lmMst2)))/(54.*pow2(Mst2)) - (304*pow3(lmMst2))/9. - (5*(9 - 140*
        lmMsq + 140*lmMst2)*pow2(Mst2)*(pow2(Mst1) + 2*(lmMst1 - lmMst2)*pow2(
        Mst2)))/(24.*pow4(Msq)) + pow4(Mst1)*(5/(36.*pow4(Msq)) - (
        865.3922396522787 + (64*B4)/9. - (64*D3)/9. + (32*DN)/9. + (145*lmMsq)/
        3. + (93.68582346518855 - 50*lmMsq)*lmMst1 - (3673*pow2(lmMst1))/189. +
        lmMst2*(-167.46360124296632 + 50*lmMsq + (15746*lmMst1)/189. + (256*
        pow2(lmMst1))/9.) - ((12073 + 5376*lmMst1)*pow2(lmMst2))/189. - (256*
        pow3(lmMst1))/27. + (256*pow3(lmMst2))/27.)/pow4(Mst2)) + (2*(376960*
        OepS2 - 27*(1307689 + 282720*lmMst1 - 282720*lmMst2)*S2)*pow4(Mst1))/(
        10935.*pow4(Mst2)) - (32*(2 + lmMst2 - 3*pow2(lmMst2))*pow4(Mst2))/(9.*
        pow4(Mst1)) - (5*(19 - 6*lmMst1 + 6*lmMst2)*pow4(Mst1) + 80*(lmMst1 -
        lmMst2)*(4 - 15*lmMsq + 15*lmMst2)*pow4(Mst2))/(18.*pow2(Msq)*pow2(
        Mst2)) + (5*(32*(4 - 15*lmMsq + 15*lmMst2)*pow2(Msq)*pow4(Mst2) + 3*(9
        - 140*lmMsq + 140*lmMst2)*pow6(Mst2)))/(72.*pow2(Mst1)*pow4(Msq))) + (
        pow3(Mt)*(8*Mt*pow2(Mst1)*(-32*pow4(Msq)*((215 + 37*lmMst2 + 3*(-5 + 8*
        lmMst2)*lmMt - 18*lmMst1*(1 - 2*lmMst2 + lmMt) - 42*pow2(lmMst2))*pow2(
        Mst1)*pow2(Mst2) + (1097 + lmMst2*(1531 - 246*lmMt) - 339*lmMt + (444 -
        36*lmMt)*pow2(lmMst2) - 18*lmMst1*(39 - 2*lmMst2*(-9 + lmMt) - 7*lmMt +
        2*pow2(lmMst2)) + 36*pow3(lmMst2))*pow4(Mst1) + 18*(-3 - 4*lmMst2 + 3*
        pow2(lmMst2))*pow4(Mst2)) + (60*(167 - 12*lmMst1*(2 + 3*lmMsq - 3*
        lmMst2) + 6*(5 + 6*lmMsq)*lmMst2 - 6*lmMt - 36*pow2(lmMst2))*pow2(Msq)
        + 45*(235 - 24*lmMst1*(1 + 3*lmMsq - 3*lmMst2) + 24*(1 + 3*lmMsq)*
        lmMst2 - 72*pow2(lmMst2))*pow2(Mst2))*pow6(Mst1)) - 3*Mst2*s2t*(45*(9 -
        140*lmMsq + 140*lmMst2)*pow2(Mst1)*(pow2(Mst1) - pow2(Mst2))*pow6(Mst2)
        + 4*pow4(Msq)*(pow2(Mst2)*(2153 + 1782*lmMst2 + 90*lmMsq*(-13 + 6*
        lmMst2) - 270*pow2(lmMsq) + 192*(-2 + lmMst2)*pow2(lmMst1) + 192*
        lmMst1*(1 + 3*lmMst2 - 2*pow2(lmMst2)) - 432*pow2(lmMst2) + 192*pow3(
        lmMst2))*pow4(Mst1) + 3*(-515 + 64*lmMst1*(-2 + lmMst2) - 530*lmMst2 -
        30*lmMsq*(-13 + 6*lmMst2) + 90*pow2(lmMsq) + 208*pow2(lmMst2))*pow2(
        Mst1)*pow4(Mst2) + 192*(26 + 29*lmMst2 + (-2 + lmMst2)*pow2(lmMst1) +
        4*pow2(lmMst2) - 2*lmMst1*(8 + lmMst2 + pow2(lmMst2)) + pow3(lmMst2))*
        pow6(Mst1) + 192*(2 + lmMst2 - 3*pow2(lmMst2))*pow6(Mst2)) + 60*pow2(
        Mst2)*pow8(Mst1) - 240*pow2(Msq)*((4 - 15*lmMsq + 15*lmMst2)*(-2*pow4(
        Mst1)*pow4(Mst2) + 2*pow2(Mst1)*pow6(Mst2)) + (11 - 3*lmMst1 + 3*
        lmMst2)*pow8(Mst1)))))/(162.*pow4(Msq)*pow4(Mst1)*pow5(Mst2))))/Tbeta -
        (pow2(Mt)*pow2(s2t)*(29584 - 18720*lmMsq + 4608*lmMst1 - 4320*pow2(
        lmMsq) - 3072*pow2(lmMst1) + 96*lmMst2*(281 + 90*lmMsq + 32*lmMst1 +
        16*pow2(lmMst1)) - 768*(11 + 4*lmMst1)*pow2(lmMst2) + (960*(-4 + 15*
        lmMsq - 15*lmMst2)*pow2(Mst1))/pow2(Msq) + (8*(2839 + 90*lmMsq*(13 - 6*
        lmMst2) + 3786*lmMst2 - 192*lmMst1*(17 + 5*lmMst2) + 270*pow2(lmMsq) +
        1200*pow2(lmMst2))*pow2(Mst1))/pow2(Mst2) + (24*(-643 + 64*lmMst1*(-2 +
        lmMst2) - 594*lmMst2 - 30*lmMsq*(-13 + 6*lmMst2) + 90*pow2(lmMsq) +
        400*pow2(lmMst2))*pow2(Mst2))/pow2(Mst1) + 1536*pow3(lmMst2) - (90*(9 -
        140*lmMsq + 140*lmMst2)*(pow2(Mst1) - 2*pow2(Mst2))*pow2(Mst2))/pow4(
        Msq) + 216*pow4(Mst1)*((5*(35 + 12*lmMsq - 12*lmMst2))/(36.*pow4(Msq))
        + (810.2826641631374 - 240*lmMsq + (-94.56843537414966 + 180*lmMsq)*
        lmMst1 + lmMst2*(1029.6795464852607 - 180*lmMsq + (466664*lmMst1)/945.
         - (304*pow2(lmMst1))/
        9.) - (162772*pow2(lmMst1))/945. + (4*(-75973 + 5460*lmMst1)*pow2(
        lmMst2))/945. + (32*lmMt*(54 + 65*lmMst2 - lmMst1*(65 + 18*lmMst2) + 9*
        pow2(lmMst1) + 9*pow2(lmMst2)))/9. + (16*pow3(lmMst1))/27. + (272*pow3(
        lmMst2))/27.)/pow4(Mst2)) - (32*(338660*OepS2 + 27*(182471 - 253995*
        lmMst1 + 253995*lmMst2)*S2)*pow4(Mst1))/(945.*pow4(Mst2)) + (1536*(2 +
        lmMst2 - 3*pow2(lmMst2))*pow4(Mst2))/pow4(Mst1) + (480*(3*(-4 + lmMst1
        - lmMst2)*pow4(Mst1) + 4*(4 - 15*lmMsq + 15*lmMst2)*pow4(Mst2)))/(pow2(
        Msq)*pow2(Mst2)) - ((30*(32*(4 - 15*lmMsq + 15*lmMst2)*pow2(Msq)*pow4(
        Mst2) + 3*(9 - 140*lmMsq + 140*lmMst2)*pow6(Mst2)))/pow2(Mst1) + (pow2(
        MuSUSY)*(-45*(-9 + 140*lmMsq - 140*lmMst2)*pow2(Mst1)*(pow2(Mst1) +
        pow2(Mst2))*(-2*lmMst1*pow2(Mst1) + 2*lmMst2*pow2(Mst1) + pow2(Mst2))*
        pow4(Mst2) + 4*pow4(Msq)*(pow2(Mst2)*(3305 + 6632*lmMst2 + 90*lmMsq*(1
        - 2*lmMst1 + 2*lmMst2)*(-13 + 6*lmMst2) + 270*(-1 + 2*lmMst1 - 2*
        lmMst2)*pow2(lmMsq) - 192*(-2 + lmMst2)*pow2(lmMst1) + 1788*pow2(
        lmMst2) + 6*lmMst1*(-995 - 402*lmMst2 + 336*pow2(lmMst2)) - 1824*pow3(
        lmMst2))*pow4(Mst1) + 3*(835 - 64*lmMst1*(-2 + lmMst2) + 466*lmMst2 +
        30*lmMsq*(-13 + 6*lmMst2) - 90*pow2(lmMsq) - 592*pow2(lmMst2))*pow2(
        Mst1)*pow4(Mst2) + 6*(672 + lmMst2*(1507 - 390*lmMsq - 90*pow2(lmMsq))
        - 288*(-2 + lmMst2)*pow2(lmMst1) + 18*(49 + 10*lmMsq)*pow2(lmMst2) +
        lmMst1*(-1411 - 1458*lmMst2 - 30*lmMsq*(-13 + 6*lmMst2) + 90*pow2(
        lmMsq) + 848*pow2(lmMst2)) - 560*pow3(lmMst2))*pow6(Mst1) - 192*(2 +
        lmMst2 - 3*pow2(lmMst2))*pow6(Mst2)) + 60*(247 + lmMst1*(29 + 60*lmMsq
        - 60*lmMst2) - (29 + 60*lmMsq)*lmMst2 + 60*pow2(lmMst2))*pow2(Mst2)*
        pow8(Mst1) + 60*pow2(Msq)*(-8*(-4 + 15*lmMsq - 15*lmMst2)*pow2(Mst1)*
        pow2(Mst2)*(pow2(Mst1) + pow2(Mst2))*(-2*lmMst1*pow2(Mst1) + 2*lmMst2*
        pow2(Mst1) + pow2(Mst2)) + (337 + lmMst1*(50 + 24*lmMsq - 24*lmMst2) -
        2*(25 + 12*lmMsq)*lmMst2 + 24*pow2(lmMst2))*pow8(Mst1))))/(pow4(Mst1)*
        pow4(Mst2)))/pow4(Msq)))/216.))) + (threeLoopFlag*pow2(Al4p)*((3*pow3(
        s2t)*(3072*Mt*((1 + lmMst2)*Mst2*(2*(2 + 2*lmMst1 - lmMst2)*pow2(Mst2)*
        pow4(Mst1) + 2*(1 - 3*lmMst2 + lmMst1*(3 + 2*lmMst2) - 2*pow2(lmMst2))*
        pow2(Mst1)*pow4(Mst2) + (1 + 2*lmMst1 - 2*lmMst2)*pow6(Mst1) - 2*(1 +
        lmMst2)*pow6(Mst2)) + Dmglst2*(2*(1 - 4*lmMst1 + 10*lmMst2 + 3*pow2(
        lmMst2))*pow2(Mst2)*pow4(Mst1) + 2*pow2(Mst1)*(6 + 11*lmMst2 - 5*pow2(
        lmMst2) + lmMst1*(-3 + 5*lmMst2 + 6*pow2(lmMst2)) - 6*pow3(lmMst2))*
        pow4(Mst2) + (1 + 13*lmMst2 - 2*lmMst1*(5 + 3*lmMst2) + 6*pow2(lmMst2))
        *pow6(Mst1) - 2*(-1 + 2*lmMst2 + 3*pow2(lmMst2))*pow6(Mst2))) + (Mst2*
        s2t*(4*pow2(Msq)*pow2(Mst2)*(80*Dmglst2*(14 - 15*lmMsq + 15*lmMst2)*
        pow2(Mst1) + 10*(43 - 30*lmMsq + 30*lmMst2)*Mst2*pow2(Mst1) + 3*Mst2*
        pow2(Msq)*(143 + 1260*lmMst2*(1 + lmMst2) + 30*lmMsq*(-1 + 2*lmMst1 -
        2*lmMst2)*(5 + 6*lmMst2) + 90*(1 - 2*lmMst1 + 2*lmMst2)*pow2(lmMsq) -
        448*(1 + lmMst2)*pow2(lmMst1) + 2*lmMst1*(-463 - 334*lmMst2 + 176*pow2(
        lmMst2)) + 96*pow3(lmMst2)) + 6*Dmglst2*pow2(Msq)*(13 + 30*lmMsq*(1 -
        6*lmMst2)*(1 - 2*lmMst1 + 2*lmMst2) + 90*(1 - 2*lmMst1 + 2*lmMst2)*
        pow2(lmMsq) - 8*lmMst2*(31 + 56*pow2(lmMst1)) + 748*pow2(lmMst2) +
        lmMst1*(358 - 732*lmMst2 + 352*pow2(lmMst2)) + 96*pow3(lmMst2)))*pow4(
        Mst1) + pow2(Mst1)*(320*Dmglst2*(-1 + 2*lmMst1 - 2*lmMst2)*(14 - 15*
        lmMsq + 15*lmMst2)*pow2(Msq)*pow2(Mst1) + 40*(-1 + 2*lmMst1 - 2*lmMst2)
        *(43 - 30*lmMsq + 30*lmMst2)*Mst2*pow2(Msq)*pow2(Mst1) + 12*Mst2*(399 -
        80*lmMst2 - 30*lmMsq*(1 + 2*lmMst1 - 2*lmMst2)*(5 + 6*lmMst2) + 90*(1 +
        2*lmMst1 - 2*lmMst2)*pow2(lmMsq) - 64*(1 + lmMst2)*pow2(lmMst1) - 1228*
        pow2(lmMst2) + 2*lmMst1*(495 + 878*lmMst2 + 336*pow2(lmMst2)) - 608*
        pow3(lmMst2))*pow4(Msq) - 24*Dmglst2*(275 - 564*lmMst2 + 30*lmMsq*(1 +
        2*lmMst1 - 2*lmMst2)*(-1 + 6*lmMst2) - 90*(1 + 2*lmMst1 - 2*lmMst2)*
        pow2(lmMsq) + 64*lmMst2*pow2(lmMst1) + 332*pow2(lmMst2) - 2*lmMst1*(-83
        + 494*lmMst2 + 336*pow2(lmMst2)) + 608*pow3(lmMst2))*pow4(Msq) + 90*
        Dmglst2*(13 - 28*lmMsq + 28*lmMst2)*pow4(Mst1) + 5*(67 - 84*lmMsq + 84*
        lmMst2)*Mst2*pow4(Mst1))*pow4(Mst2) + 12*(-2*Dmglst2*(-147 + 2*(55 +
        32*lmMst1)*lmMst2 - 30*lmMsq*(-1 + 6*lmMst2) + 90*pow2(lmMsq) + 144*
        pow2(lmMst2)) - Mst2*(207 + 430*lmMst2 + 64*lmMst1*(1 + lmMst2) - 30*
        lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) + 176*pow2(lmMst2)))*pow4(Msq)*
        pow6(Mst1) + 2*Dmglst2*(-160*(1 + 2*lmMst1 - 2*lmMst2)*(14 - 15*lmMsq +
        15*lmMst2)*pow2(Msq)*pow2(Mst1) - 12*(-115 + (494 + 64*lmMst1)*lmMst2 -
        30*lmMsq*(-1 + 6*lmMst2) + 90*pow2(lmMsq) + 464*pow2(lmMst2))*pow4(Msq)
        + 45*(-1 + 2*lmMst1 - 2*lmMst2)*(13 - 28*lmMsq + 28*lmMst2)*pow4(Mst1))
        *pow6(Mst2) - (40*(1 + 2*lmMst1 - 2*lmMst2)*(43 - 30*lmMsq + 30*lmMst2)
        *pow2(Msq)*pow2(Mst1) + 12*(399 + 782*lmMst2 + 64*lmMst1*(1 + lmMst2) -
        30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) + 336*pow2(lmMst2))*pow4(Msq)
        + 5*(1 - 2*lmMst1 + 2*lmMst2)*(67 - 84*lmMsq + 84*lmMst2)*pow4(Mst1))*
        pow7(Mst2)))/pow4(Msq)))/(pow2(Mst1)*pow2(Mst2)) + 12*pow2(Mt)*pow2(
        s2t)*((8*(2*Dmglst2*(-889 - 694*lmMst2 + 576*lmMst1*(1 + lmMst2) - 90*
        lmMsq*(-1 + 6*lmMst2) + 270*pow2(lmMsq) + 48*pow2(lmMst2)) + 3*Mst2*(
        207 + 494*lmMst2 - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) + 240*pow2(
        lmMst2)))*pow2(Mst1))/Mst2 + 32*Dmglst2*Mst2*(329 + 90*lmMsq*(-1 + 6*
        lmMst2) + 96*lmMst1*(1 - 2*lmMst2*(1 + lmMst2)) - 270*pow2(lmMsq) +
        lmMst2*(-586 + 96*pow2(lmMst1)) - 528*pow2(lmMst2) + (20*(-14 + 15*
        lmMsq - 15*lmMst2)*pow2(Mst1))/pow2(Msq) + 96*pow3(lmMst2)) - 16*pow2(
        Mst2)*(621 + 1242*lmMst2 - 90*lmMsq*(5 + 6*lmMst2) + 270*pow2(lmMsq) +
        384*pow2(lmMst2) + 96*lmMst1*(3 + 5*lmMst2 + 2*pow2(lmMst2)) + (5*(43 -
        30*lmMsq + 30*lmMst2)*pow2(Mst1))/pow2(Msq) - 96*((1 + lmMst2)*pow2(
        lmMst1) + pow3(lmMst2))) - (1536*((lmMst1 - lmMst2)*(1 + lmMst2)*Mst2 +
        2*Dmglst2*(2 + 7*lmMst2 - lmMst1*(5 + 4*lmMst2) + 4*pow2(lmMst2)))*
        pow4(Mst1))/pow3(Mst2) + (4*Dmglst2*pow3(Mst2)*(-320*(-14 + 15*lmMsq -
        15*lmMst2)*pow2(Msq)*pow2(Mst1) + 12*(-179 + 238*lmMst2 + 64*lmMst1*
        lmMst2 - 30*lmMsq*(-1 + 6*lmMst2) + 90*pow2(lmMsq) + 272*pow2(lmMst2))*
        pow4(Msq) + 45*(-13 + 28*lmMsq - 28*lmMst2)*pow4(Mst1)))/(pow2(Mst1)*
        pow4(Msq)) + 8*((20*(43 - 30*lmMsq + 30*lmMst2))/pow2(Msq) + (3*(303 +
        590*lmMst2 + 64*lmMst1*(1 + lmMst2) - 30*lmMsq*(5 + 6*lmMst2) + 90*
        pow2(lmMsq) + 240*pow2(lmMst2)))/pow2(Mst1))*pow4(Mst2) - (10*(36*
        Dmglst2*(-13 + 28*lmMsq - 28*lmMst2)*Mst2 + (67 - 84*lmMsq + 84*lmMst2)
        *pow2(Mst1))*pow4(Mst2))/pow4(Msq) + (4*(-768*Dmglst2*lmMst2*(1 +
        lmMst2) - 192*Mst2*pow2(1 + lmMst2) + (160*Dmglst2*(-14 + 15*lmMsq -
        15*lmMst2)*pow2(Mst1))/pow2(Msq) + (20*(-43 + 30*lmMsq - 30*lmMst2)*
        Mst2*pow2(Mst1))/pow2(Msq) + (5*(67 - 84*lmMsq + 84*lmMst2)*Mst2*pow4(
        Mst1))/pow4(Msq))*pow5(Mst2))/pow4(Mst1) + (pow2(MuSUSY)*(pow2(Mst1)*((
        pow2(Mst1) + pow2(Mst2))*pow4(Mst2) - 2*(lmMst1 - lmMst2)*pow2(Mst1)*(
        pow2(Mst1)*pow2(Mst2) + pow4(Mst1) + pow4(Mst2)))*(40*(-43 + 30*lmMsq -
        30*lmMst2)*pow2(Msq)*pow3(Mst2) + 5*(-67 + 84*lmMsq - 84*lmMst2)*pow5(
        Mst2)) + 12*Mst2*pow4(Msq)*((303 + 1452*lmMst2 + 30*lmMsq*(-1 + 2*
        lmMst1 - 2*lmMst2)*(5 + 6*lmMst2) + 90*(1 - 2*lmMst1 + 2*lmMst2)*pow2(
        lmMsq) + 64*(1 + lmMst2)*pow2(lmMst1) + 1804*pow2(lmMst2) - 2*lmMst1*(
        431 + 814*lmMst2 + 336*pow2(lmMst2)) + 608*pow3(lmMst2))*pow4(Mst1)*
        pow4(Mst2) + (335 + 654*lmMst2 + 64*lmMst1*(1 + lmMst2) - 30*lmMsq*(5 +
        6*lmMst2) + 90*pow2(lmMsq) + 272*pow2(lmMst2))*pow2(Mst1)*pow6(Mst2) +
        2*(pow2(Mst2)*(64 + 15*lmMst2*(33 - 10*lmMsq + 6*pow2(lmMsq)) + 288*(1
        + lmMst2)*pow2(lmMst1) + 6*(173 - 30*lmMsq)*pow2(lmMst2) - lmMst1*(431
        + 1326*lmMst2 - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) + 848*pow2(
        lmMst2)) + 560*pow3(lmMst2))*pow6(Mst1) + (80 + lmMst2*(479 - 150*lmMsq
        + 90*pow2(lmMsq)) + 544*(1 + lmMst2)*pow2(lmMst1) + 2*(631 - 90*lmMsq)*
        pow2(lmMst2) - lmMst1*(399 + 1806*lmMst2 - 30*lmMsq*(5 + 6*lmMst2) +
        90*pow2(lmMsq) + 1360*pow2(lmMst2)) + 816*pow3(lmMst2))*pow8(Mst1)) -
        32*pow2(1 + lmMst2)*pow8(Mst2)) + 2*Dmglst2*(pow2(Mst1)*(160*(-14 + 15*
        lmMsq - 15*lmMst2)*pow2(Msq)*pow2(Mst2) + 45*(-13 + 28*lmMsq - 28*
        lmMst2)*pow4(Mst2))*((pow2(Mst1) + pow2(Mst2))*pow4(Mst2) - 2*(lmMst1 -
        lmMst2)*pow2(Mst1)*(pow2(Mst1)*pow2(Mst2) + pow4(Mst1) + pow4(Mst2))) +
        12*pow4(Msq)*((45 + 30*lmMsq*(1 - 6*lmMst2)*(1 - 2*lmMst1 + 2*lmMst2) +
        90*(1 - 2*lmMst1 + 2*lmMst2)*pow2(lmMsq) + 8*lmMst2*(29 + 8*pow2(
        lmMst1)) + 1068*pow2(lmMst2) - 2*lmMst1*(-83 + 430*lmMst2 + 336*pow2(
        lmMst2)) + 608*pow3(lmMst2))*pow4(Mst1)*pow4(Mst2) + (-115 + (366 + 64*
        lmMst1)*lmMst2 - 30*lmMsq*(-1 + 6*lmMst2) + 90*pow2(lmMsq) + 336*pow2(
        lmMst2))*pow2(Mst1)*pow6(Mst2) + 2*(pow2(Mst2)*(96 + lmMst2*(173 + 30*
        lmMsq + 90*pow2(lmMsq) + 288*pow2(lmMst1)) + (526 - 180*lmMsq)*pow2(
        lmMst2) - lmMst1*(13 + 30*lmMsq*(1 - 6*lmMst2) + 526*lmMst2 + 90*pow2(
        lmMsq) + 848*pow2(lmMst2)) + 560*pow3(lmMst2))*pow6(Mst1) + (96 +
        lmMst2*(285 + 30*lmMsq + 90*pow2(lmMsq) + 544*pow2(lmMst1)) + (590 -
        180*lmMsq)*pow2(lmMst2) - lmMst1*(109 + 30*lmMsq*(1 - 6*lmMst2) + 590*
        lmMst2 + 90*pow2(lmMsq) + 1360*pow2(lmMst2)) + 816*pow3(lmMst2))*pow8(
        Mst1)) - 64*lmMst2*(1 + lmMst2)*pow8(Mst2)))))/(pow4(Msq)*pow4(Mst1)*
        pow5(Mst2))) + (16*pow3(Mt)*(128*s2t*pow2(Mst1)*(-(Dmglst2*Mst2*(pow2(
        Mst1)*pow2(Mst2)*(pow2(Mst2)*(253 + 5*lmMst2*(107 - 6*lmMt) - 102*lmMt
        - 18*lmMst1*(9 + lmMst2 - 2*lmMt + 2*lmMst2*lmMt - 2*pow2(lmMst2)) +
        12*(10 + 3*lmMt)*pow2(lmMst2) - 36*pow3(lmMst2)) + 18*pow2(MuSUSY)*(8 +
        7*lmMst2 - 11*pow2(lmMst2) + lmMst1*(-3 + 5*lmMst2 + 6*pow2(lmMst2)) -
        6*pow3(lmMst2))) + 18*((2*pow2(MuSUSY)*(8 + 13*lmMst2 - 8*pow2(lmMst2)
        + lmMst1*(-5 + 5*lmMst2 + 6*pow2(lmMst2)) - 6*pow3(lmMst2)) + pow2(
        Mst2)*(23 + lmMst2*(93 - 22*lmMt) - 14*lmMt - 4*(-11 + lmMt)*pow2(
        lmMst2) - 2*lmMst1*(25 + lmMst2*(17 - 2*lmMt) - 6*lmMt + 2*pow2(lmMst2)
        ) + 4*pow3(lmMst2)))*pow4(Mst1) + (1 - 2*lmMst2 - 3*pow2(lmMst2))*(2*
        pow2(Mst2) + pow2(MuSUSY))*pow4(Mst2) + (14 + lmMst2*(189 - 48*lmMt) -
        22*lmMt + lmMst1*(-137 + 30*lmMt + 9*lmMst2*(-13 + 2*lmMt) - 18*pow2(
        lmMst2)) - 9*(-15 + 2*lmMt)*pow2(lmMst2) + 18*pow3(lmMst2))*pow6(Mst1))
        )) + 9*(1 + lmMst2)*(-2*pow2(Mst2)*((3 + 2*lmMst1*(7 + 2*lmMst2 - 2*
        lmMt) + 4*lmMst2*(-4 + lmMt) + 2*lmMt - 4*pow2(lmMst2))*pow2(Mst2) + (1
        - 10*lmMst2 + 4*lmMst1*(2 + lmMst2) - 4*pow2(lmMst2))*pow2(MuSUSY))*
        pow4(Mst1) + pow2(Mst1)*((1 - 2*lmMst1*(5 + 2*lmMst2 - 2*lmMt) - 4*
        lmMst2*(-3 + lmMt) - 6*lmMt + 4*pow2(lmMst2))*pow2(Mst2) + 2*(1 + 5*
        lmMst2 - lmMst1*(3 + 2*lmMst2) + 2*pow2(lmMst2))*pow2(MuSUSY))*pow4(
        Mst2) - (2*(8 - 27*lmMst2 + lmMst1*(25 + 6*lmMst2 - 6*lmMt) + 2*lmMt +
        6*lmMst2*lmMt - 6*pow2(lmMst2))*pow2(Mst2) + (7 - 32*lmMst2 + 4*lmMst1*
        (7 + 3*lmMst2) - 12*pow2(lmMst2))*pow2(MuSUSY))*pow6(Mst1) + 2*(1 +
        lmMst2)*(2*pow2(Mst2) + pow2(MuSUSY))*pow6(Mst2))) + (Mt*(15*pow2(Mst1)
        *(pow2(Mst1) + pow2(Mst2))*(8*(43 - 30*lmMsq + 30*lmMst2)*pow2(Msq) + (
        67 - 84*lmMsq + 84*lmMst2)*pow2(Mst2))*pow7(Mst2) + 4*Mst2*pow4(Msq)*(-
        ((3559 + 10942*lmMst2 - 270*lmMsq*(5 + 6*lmMst2) - 1152*lmMst1*(1 +
        lmMst2)*(3 + lmMst2 - lmMt) + 810*pow2(lmMsq) + 8112*pow2(lmMst2) -
        192*lmMt*(7 + 13*lmMst2 + 6*pow2(lmMst2)) + 1152*pow3(lmMst2))*pow4(
        Mst1)*pow4(Mst2)) + 576*(1 + lmMst2)*((5 - 57*lmMst2 + 2*lmMst1*(25 +
        6*lmMst2 - 6*lmMt) + 7*lmMt + 12*lmMst2*lmMt - 12*pow2(lmMst2))*pow2(
        Mst1) + (-2 - 27*lmMst2 + lmMst1*(22 + 6*lmMst2 - 6*lmMt) + 5*lmMt + 6*
        lmMst2*lmMt - 6*pow2(lmMst2))*pow2(Mst2))*pow6(Mst1) - 9*(271 + 526*
        lmMst2 + 64*lmMst1*(1 + lmMst2) - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(
        lmMsq) + 208*pow2(lmMst2))*pow2(Mst1)*pow6(Mst2) + 288*pow2(1 + lmMst2)
        *pow8(Mst2)) + 2*Dmglst2*(4*pow4(Msq)*((9115 + 12498*lmMst2 + 270*
        lmMsq*(-1 + 6*lmMst2) - 1152*lmMst1*(4 + 2*lmMst2 - lmMt) - 96*(29 +
        27*lmMst2)*lmMt - 810*pow2(lmMsq) + 1872*pow2(lmMst2))*pow4(Mst1)*pow4(
        Mst2) - 9*(-243 + 2*(55 + 32*lmMst1)*lmMst2 - 30*lmMsq*(-1 + 6*lmMst2)
        + 90*pow2(lmMsq) + 208*pow2(lmMst2))*pow2(Mst1)*pow6(Mst2) + 576*(pow2(
        Mst2)*(31 + lmMst2*(95 - 23*lmMt) - 16*lmMt + (51 - 6*lmMt)*pow2(
        lmMst2) - 2*lmMst1*(25 + lmMst2*(20 - 3*lmMt) - 6*lmMt + 3*pow2(lmMst2)
        ) + 6*pow3(lmMst2))*pow6(Mst1) + (42 + lmMst2*(242 - 62*lmMt) - 33*lmMt
        + 6*(29 - 4*lmMt)*pow2(lmMst2) - 2*lmMst1*(81 + lmMst2*(74 - 12*lmMt) -
        18*lmMt + 12*pow2(lmMst2)) + 24*pow3(lmMst2))*pow8(Mst1)) + 576*lmMst2*
        (1 + lmMst2)*pow8(Mst2)) + pow2(Mst1)*(pow2(Mst1) + pow2(Mst2))*(480*(
        14 - 15*lmMsq + 15*lmMst2)*pow2(Msq)*pow6(Mst2) + 135*(13 - 28*lmMsq +
        28*lmMst2)*pow8(Mst2)))))/pow4(Msq)))/(pow4(Mst1)*pow5(Mst2)) + (4*Mt*
        MuSUSY*((3*Mt*MuSUSY*s2t*(1536*Mt*pow2(Mst1)*(2*(pow2(Mst1)*((1 +
        lmMst2)*Mst2*(-1 - 5*lmMst2 + lmMst1*(3 + 2*lmMst2) - 2*pow2(lmMst2)) +
        Dmglst2*(8 + 7*lmMst2 - 11*pow2(lmMst2) + lmMst1*(-3 + 5*lmMst2 + 6*
        pow2(lmMst2)) - 6*pow3(lmMst2)))*pow3(Mst2) + Mst2*((1 + lmMst2)*Mst2*(
        1 - 10*lmMst2 + 4*lmMst1*(2 + lmMst2) - 4*pow2(lmMst2)) + 2*Dmglst2*(8
        + 13*lmMst2 - 8*pow2(lmMst2) + lmMst1*(-5 + 5*lmMst2 + 6*pow2(lmMst2))
        - 6*pow3(lmMst2)))*pow4(Mst1)) + (1 + lmMst2)*(-2*(-Dmglst2 + 3*
        Dmglst2*lmMst2 + Mst2 + lmMst2*Mst2)*pow5(Mst2) + (7 - 32*lmMst2 + 4*
        lmMst1*(7 + 3*lmMst2) - 12*pow2(lmMst2))*pow6(Mst1))) - (s2t*(pow2(
        Mst1)*((pow2(Mst1) + pow2(Mst2))*pow4(Mst2) - 2*(lmMst1 - lmMst2)*pow2(
        Mst1)*(pow2(Mst1)*pow2(Mst2) + pow4(Mst1) + pow4(Mst2)))*(40*(-43 + 30*
        lmMsq - 30*lmMst2)*pow2(Msq)*pow3(Mst2) + 5*(-67 + 84*lmMsq - 84*
        lmMst2)*pow5(Mst2)) + 12*Mst2*pow4(Msq)*((303 + 1452*lmMst2 + 30*lmMsq*
        (-1 + 2*lmMst1 - 2*lmMst2)*(5 + 6*lmMst2) + 90*(1 - 2*lmMst1 + 2*
        lmMst2)*pow2(lmMsq) + 64*(1 + lmMst2)*pow2(lmMst1) + 1804*pow2(lmMst2)
        - 2*lmMst1*(431 + 814*lmMst2 + 336*pow2(lmMst2)) + 608*pow3(lmMst2))*
        pow4(Mst1)*pow4(Mst2) + (335 + 654*lmMst2 + 64*lmMst1*(1 + lmMst2) -
        30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) + 272*pow2(lmMst2))*pow2(Mst1)
        *pow6(Mst2) + 2*(pow2(Mst2)*(64 + 15*lmMst2*(33 - 10*lmMsq + 6*pow2(
        lmMsq)) + 288*(1 + lmMst2)*pow2(lmMst1) + 6*(173 - 30*lmMsq)*pow2(
        lmMst2) - lmMst1*(431 + 1326*lmMst2 - 30*lmMsq*(5 + 6*lmMst2) + 90*
        pow2(lmMsq) + 848*pow2(lmMst2)) + 560*pow3(lmMst2))*pow6(Mst1) + (80 +
        lmMst2*(479 - 150*lmMsq + 90*pow2(lmMsq)) + 544*(1 + lmMst2)*pow2(
        lmMst1) + 2*(631 - 90*lmMsq)*pow2(lmMst2) - lmMst1*(399 + 1806*lmMst2 -
        30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) + 1360*pow2(lmMst2)) + 816*
        pow3(lmMst2))*pow8(Mst1)) - 32*pow2(1 + lmMst2)*pow8(Mst2)) + 2*
        Dmglst2*(pow2(Mst1)*(160*(-14 + 15*lmMsq - 15*lmMst2)*pow2(Msq)*pow2(
        Mst2) + 45*(-13 + 28*lmMsq - 28*lmMst2)*pow4(Mst2))*((pow2(Mst1) +
        pow2(Mst2))*pow4(Mst2) - 2*(lmMst1 - lmMst2)*pow2(Mst1)*(pow2(Mst1)*
        pow2(Mst2) + pow4(Mst1) + pow4(Mst2))) + 12*pow4(Msq)*((45 + 30*lmMsq*(
        1 - 6*lmMst2)*(1 - 2*lmMst1 + 2*lmMst2) + 90*(1 - 2*lmMst1 + 2*lmMst2)*
        pow2(lmMsq) + 8*lmMst2*(29 + 8*pow2(lmMst1)) + 1068*pow2(lmMst2) - 2*
        lmMst1*(-83 + 430*lmMst2 + 336*pow2(lmMst2)) + 608*pow3(lmMst2))*pow4(
        Mst1)*pow4(Mst2) + (-115 + (366 + 64*lmMst1)*lmMst2 - 30*lmMsq*(-1 + 6*
        lmMst2) + 90*pow2(lmMsq) + 336*pow2(lmMst2))*pow2(Mst1)*pow6(Mst2) + 2*
        (pow2(Mst2)*(96 + lmMst2*(173 + 30*lmMsq + 90*pow2(lmMsq) + 288*pow2(
        lmMst1)) + (526 - 180*lmMsq)*pow2(lmMst2) - lmMst1*(13 + 30*lmMsq*(1 -
        6*lmMst2) + 526*lmMst2 + 90*pow2(lmMsq) + 848*pow2(lmMst2)) + 560*pow3(
        lmMst2))*pow6(Mst1) + (96 + lmMst2*(285 + 30*lmMsq + 90*pow2(lmMsq) +
        544*pow2(lmMst1)) + (590 - 180*lmMsq)*pow2(lmMst2) - lmMst1*(109 + 30*
        lmMsq*(1 - 6*lmMst2) + 590*lmMst2 + 90*pow2(lmMsq) + 1360*pow2(lmMst2))
        + 816*pow3(lmMst2))*pow8(Mst1)) - 64*lmMst2*(1 + lmMst2)*pow8(Mst2)))))
        /pow4(Msq)))/pow2(Sbeta) + (1024*pow2(Mst1)*pow3(Mt)*(pow2(Mst1)*(-9*(1
        + lmMst2)*Mst2*(1 + lmMst2*(7 - 2*lmMt) - 2*lmMst1*(2 + lmMst2 - lmMt)
        - 3*lmMt + 2*pow2(lmMst2)) + Dmglst2*(119 + lmMst2*(224 - 15*lmMt) -
        51*lmMt + 3*(5 + 6*lmMt)*pow2(lmMst2) + 18*lmMst1*(-4 + lmMt - lmMst2*(
        1 + lmMt) + pow2(lmMst2)) - 18*pow3(lmMst2)))*pow3(Mst2) + Mst2*(9*(1 +
        lmMst2)*Mst2*(3 + 2*lmMst1*(8 + 3*lmMst2 - 3*lmMt) + 5*lmMt + 3*lmMst2*
        (-7 + 2*lmMt) - 6*pow2(lmMst2)) + Dmglst2*(263 + lmMst2*(962 - 213*
        lmMt) - 177*lmMt + (393 - 18*lmMt)*pow2(lmMst2) - 18*lmMst1*(26 -
        lmMst2*(-17 + lmMt) - 7*lmMt + pow2(lmMst2)) + 18*pow3(lmMst2)))*pow4(
        Mst1) + (1 + lmMst2)*(-18*(-Dmglst2 + 3*Dmglst2*lmMst2 + Mst2 + lmMst2*
        Mst2)*pow5(Mst2) + 9*(12 - 45*lmMst2 + 2*lmMst1*(19 + 6*lmMst2 - 6*
        lmMt) + 7*lmMt + 12*lmMst2*lmMt - 12*pow2(lmMst2))*pow6(Mst1))) - 6912*
        Mst2*Mt*pow2(Mst1)*pow2(s2t)*((1 + lmMst2)*Mst2*(2*(2 - 5*lmMst2 +
        lmMst1*(5 + 2*lmMst2) - 2*pow2(lmMst2))*pow2(Mst2)*pow4(Mst1) + 2*(-2*
        lmMst2*(2 + lmMst2) + lmMst1*(3 + 2*lmMst2))*pow2(Mst1)*pow4(Mst2) + (5
        - 12*lmMst2 + 4*lmMst1*(3 + lmMst2) - 4*pow2(lmMst2))*pow6(Mst1) - 2*(1
        + lmMst2)*pow6(Mst2)) + Dmglst2*(2*pow2(Mst2)*(8 + 19*lmMst2 - 5*pow2(
        lmMst2) + lmMst1*(-7 + 5*lmMst2 + 6*pow2(lmMst2)) - 6*pow3(lmMst2))*
        pow4(Mst1) + 2*pow2(Mst1)*(7 + 9*lmMst2 - 8*pow2(lmMst2) + lmMst1*(-3 +
        5*lmMst2 + 6*pow2(lmMst2)) - 6*pow3(lmMst2))*pow4(Mst2) + (17 + 51*
        lmMst2 - 4*pow2(lmMst2) + 4*lmMst1*(-6 + lmMst2 + 3*pow2(lmMst2)) - 12*
        pow3(lmMst2))*pow6(Mst1) - 2*(-1 + 2*lmMst2 + 3*pow2(lmMst2))*pow6(
        Mst2))) + (3*pow2(Mst2)*pow3(s2t)*(40*(43 - 30*lmMsq + 30*lmMst2)*pow2(
        Msq)*pow2(Mst1)*(2*(lmMst1 - lmMst2)*pow2(Mst1)*pow2(Mst2) + pow4(Mst1)
        - pow4(Mst2))*pow5(Mst2) + 5*(67 - 84*lmMsq + 84*lmMst2)*(pow2(Mst1) +
        2*(lmMst1 - lmMst2)*pow2(Mst2))*pow4(Mst1)*pow7(Mst2) + 12*Mst2*pow4(
        Msq)*(-2*(16 - 3*lmMst2*(133 - 50*lmMsq + 30*pow2(lmMsq)) - 32*(1 +
        lmMst2)*pow2(lmMst1) + 2*(-383 + 90*lmMsq)*pow2(lmMst2) + lmMst1*(463 +
        846*lmMst2 - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) + 336*pow2(
        lmMst2)) - 304*pow3(lmMst2))*pow4(Mst1)*pow4(Mst2) - pow2(Mst2)*(175 +
        462*lmMst2 + 1024*lmMst1*lmMst2*(1 + lmMst2) - 30*lmMsq*(5 + 6*lmMst2)
        + 90*pow2(lmMsq) - 272*pow2(lmMst2) - 512*((1 + lmMst2)*pow2(lmMst1) +
        pow3(lmMst2)))*pow6(Mst1) + (367 + 718*lmMst2 + 64*lmMst1*(1 + lmMst2)
        - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) + 304*pow2(lmMst2))*pow2(
        Mst1)*pow6(Mst2) + 32*(1 + lmMst2)*(1 + lmMst1*(2 - 32*lmMst2) - 2*
        lmMst2 + 16*pow2(lmMst1) + 16*pow2(lmMst2))*pow8(Mst1) - 32*pow2(1 +
        lmMst2)*pow8(Mst2)) + 2*Dmglst2*(384*lmMst2*pow4(Msq)*pow4(Mst1)*(2*
        pow2(lmMst1)*(8*pow2(Mst1)*pow2(Mst2) + 8*pow4(Mst1) + pow4(Mst2)) +
        pow2(lmMst2)*(16*pow2(Mst1)*(pow2(Mst1) + pow2(Mst2)) + 19*pow4(Mst2)))
        + pow2(Mst1)*pow2(Mst2)*(45*(13 - 28*lmMsq)*pow4(Mst1)*pow4(Mst2) +
        160*(-14 + 15*lmMsq)*pow2(Msq)*pow2(Mst2)*(-pow4(Mst1) + pow4(Mst2)) +
        12*pow4(Msq)*(160*pow2(Mst1)*pow2(Mst2) - 3*(-49 + 10*lmMsq + 30*pow2(
        lmMsq))*pow4(Mst1) + 5*(-23 + 6*lmMsq + 18*pow2(lmMsq))*pow4(Mst2))) -
        24*pow2(lmMst2)*(200*pow2(Msq)*pow4(Mst1)*pow6(Mst2) + 105*pow4(Mst1)*
        pow8(Mst2) + pow4(Msq)*(6*(-61 + 30*lmMsq)*pow4(Mst1)*pow4(Mst2) + 8*
        pow2(Mst2)*pow6(Mst1) - 200*pow2(Mst1)*pow6(Mst2) - 64*pow8(Mst1) + 32*
        pow8(Mst2))) + 2*lmMst1*(-12*pow4(Msq)*((-83 + 462*lmMst2 - 30*lmMsq*(-
        1 + 6*lmMst2) + 90*pow2(lmMsq) + 336*pow2(lmMst2))*pow4(Mst1)*pow4(
        Mst2) + 32*(3 + 3*lmMst2 + 16*pow2(lmMst2))*pow2(Mst2)*pow6(Mst1) - 32*
        lmMst2*pow2(Mst1)*pow6(Mst2) + 32*(3 + 2*lmMst2 + 16*pow2(lmMst2))*
        pow8(Mst1)) + pow4(Mst1)*(160*(14 - 15*lmMsq + 15*lmMst2)*pow2(Msq)*
        pow6(Mst2) + 45*(13 - 28*lmMsq + 28*lmMst2)*pow8(Mst2))) + 2*lmMst2*(
        630*pow6(Mst1)*pow6(Mst2) + 12*pow4(Msq)*((-67 + 30*lmMsq + 90*pow2(
        lmMsq))*pow4(Mst1)*pow4(Mst2) + 3*(19 + 30*lmMsq)*pow2(Mst2)*pow6(Mst1)
        + 5*(43 - 18*lmMsq)*pow2(Mst1)*pow6(Mst2) + 112*pow8(Mst1) - 32*pow8(
        Mst2)) + 45*(-13 + 28*lmMsq)*pow4(Mst1)*pow8(Mst2) + 80*pow2(Msq)*(15*
        pow4(Mst2)*pow6(Mst1) + 2*(-14 + 15*lmMsq)*pow4(Mst1)*pow6(Mst2) - 15*
        pow2(Mst1)*pow8(Mst2))))))/pow4(Msq) - (12*s2t*pow2(Mt)*(12*Mst2*pow4(
        Msq)*(-((143 + 302*lmMst2 - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) +
        48*pow2(lmMst2) + 128*lmMst1*pow2(1 + lmMst2) - 64*((1 + lmMst2)*pow2(
        lmMst1) + pow3(lmMst2)))*pow4(Mst1)*pow4(Mst2)) + (271 + 526*lmMst2 +
        64*lmMst1*(1 + lmMst2) - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) +
        208*pow2(lmMst2))*pow2(Mst1)*pow6(Mst2) + 64*(1 + lmMst2)*(pow2(1 -
        lmMst1 + lmMst2)*pow2(Mst2)*pow6(Mst1) + (1 + 3*lmMst2 - lmMst1*(3 + 2*
        lmMst2) + pow2(lmMst1) + pow2(lmMst2))*pow8(Mst1)) - 32*pow2(1 +
        lmMst2)*pow8(Mst2)) + 2*Dmglst2*(-4*pow4(Msq)*(-((121 - 650*lmMst2 +
        90*lmMsq*(-1 + 6*lmMst2) - 270*pow2(lmMsq) + 192*lmMst2*pow2(lmMst1) -
        432*pow2(lmMst2) - 192*lmMst1*(-1 + lmMst2 + 2*pow2(lmMst2)) + 192*
        pow3(lmMst2))*pow4(Mst1)*pow4(Mst2)) - 192*(lmMst2*pow2(lmMst1) +
        lmMst1*(4 + 2*lmMst2 - 2*pow2(lmMst2)) + (-4 + lmMst2)*pow2(1 + lmMst2)
        )*pow2(Mst2)*pow6(Mst1) - 3*(-179 + 2*(87 + 32*lmMst1)*lmMst2 - 30*
        lmMsq*(-1 + 6*lmMst2) + 90*pow2(lmMsq) + 208*pow2(lmMst2))*pow2(Mst1)*
        pow6(Mst2) - 192*(-6 - 14*lmMst2 + lmMst2*pow2(lmMst1) + lmMst1*(9 + 6*
        lmMst2 - 2*pow2(lmMst2)) - 6*pow2(lmMst2) + pow3(lmMst2))*pow8(Mst1) +
        192*lmMst2*(1 + lmMst2)*pow8(Mst2)) + pow2(Mst1)*(pow2(Mst1) - pow2(
        Mst2))*(160*(14 - 15*lmMsq + 15*lmMst2)*pow2(Msq)*pow6(Mst2) + 45*(13 -
        28*lmMsq + 28*lmMst2)*pow8(Mst2))) + pow2(Mst1)*(pow2(Mst1) - pow2(
        Mst2))*(40*(43 - 30*lmMsq + 30*lmMst2)*pow2(Msq)*pow7(Mst2) + 5*(67 -
        84*lmMsq + 84*lmMst2)*pow9(Mst2))))/pow4(Msq))/Tbeta))/(pow4(Mst1)*
        pow5(Mst2))))/2592.) - ((xMst*pow6(Mst1)*((-27*(lmMst1 - lmMst2)*
        oneLoopFlag*pow2(Mt)*pow2(MuSUSY)*pow2(s2t)*(-1 + pow2(Sbeta))*pow3(
        Mst2))/pow2(Sbeta) - Al4p*twoLoopFlag*(8*Mst2*s2t*pow2(Mt)*(Mst2*(Mt*(-
        193 - 474*lmMst2 + 6*lmMst1*(67 + 42*lmMst2) - 252*pow2(lmMst2)) +
        Dmglst2*s2t*(49 - 84*lmMst2 + 12*lmMst1*(7 + 3*lmMst2) - 36*pow2(
        lmMst2)))*pow2(MuSUSY) + Dmglst2*Mt*(-785 + 438*lmMst2 + 6*lmMst1*(-85
        + 42*lmMst2) - 252*pow2(lmMst2))*pow2(MuSUSY) + s2t*(1 - 111*lmMst2 +
        3*lmMst1*(37 + 72*lmMst2) - 81*pow2(lmMst1) - 135*pow2(lmMst2))*pow2(
        Mst2)*pow2(MuSUSY) + 4*(9*Dmglst2*(-lmMst1 + lmMst2)*s2t + Mt*(49 - 75*
        lmMst2 + 3*lmMst1*(25 + 6*lmMst2 - 6*lmMt) + 18*lmMst2*lmMt - 18*pow2(
        lmMst2)))*pow3(Mst2)) - 32*(Dmglst2*s2t*(95 - 447*lmMst2 + lmMst1*(411
        + 90*lmMst2 - 90*lmMt) + 36*lmMt + 90*lmMst2*lmMt - 90*pow2(lmMst2)) +
        Mt*(65 - 159*lmMst2 + 6*lmMst1*(25 + 6*lmMst2 - 6*lmMt) + 9*lmMt + 36*
        lmMst2*lmMt - 36*pow2(lmMst2)))*pow3(Mst2)*pow3(Mt) + 2*Mst2*Mt*MuSUSY*
        ((4*Mt*MuSUSY*s2t*(Dmglst2*Mst2*s2t*(-49 + 84*lmMst2 - 12*lmMst1*(7 +
        3*lmMst2) + 36*pow2(lmMst2)) + Dmglst2*Mt*(785 - 438*lmMst2 - 6*lmMst1*
        (-85 + 42*lmMst2) + 252*pow2(lmMst2)) + Mst2*Mt*(193 + 474*lmMst2 - 6*
        lmMst1*(67 + 42*lmMst2) + 252*pow2(lmMst2)) + s2t*(-1 + 111*lmMst2 - 3*
        lmMst1*(37 + 72*lmMst2) + 81*pow2(lmMst1) + 135*pow2(lmMst2))*pow2(
        Mst2)))/pow2(Sbeta) + (8*Dmglst2*(36*(-1 + 3*lmMst1 - 3*lmMst2)*Mst2*
        s2t*pow2(Mt) + 3*Mt*(-50 + 51*lmMst2 + 3*lmMst1*(-17 + 6*lmMst2) - 18*
        pow2(lmMst2))*pow2(Mst2)*pow2(s2t) + 4*(7 - 381*lmMst2 + lmMst1*(327 +
        72*lmMst2 - 81*lmMt) + 54*lmMt + 81*lmMst2*lmMt - 72*pow2(lmMst2))*
        pow3(Mt) + 2*(1 + 3*lmMst1 - 3*lmMst2)*pow3(Mst2)*pow3(s2t)) - Mst2*(-
        144*Mst2*s2t*(-lmMst1 + lmMst2 - 2*lmMst1*lmMst2 + pow2(lmMst1) + pow2(
        lmMst2))*pow2(Mt) + 24*Mt*(1 + 33*lmMst2 - 3*lmMst1*(11 + 6*lmMst2) +
        18*pow2(lmMst2))*pow2(Mst2)*pow2(s2t) + 32*(83 - 96*lmMst2 + 3*lmMst1*(
        29 + 12*lmMst2 - 9*lmMt) + 9*lmMt + 27*lmMst2*lmMt - 36*pow2(lmMst2))*
        pow3(Mt) + (5 + lmMst1*(6 - 288*lmMst2) - 6*lmMst2 + 144*pow2(lmMst1) +
        144*pow2(lmMst2))*pow3(Mst2)*pow3(s2t)))/Tbeta) + 576*Dmglst2*(4 + 6*
        lmMst1*(9 + 2*lmMst2 - 2*lmMt) + 7*lmMt + lmMst2*(-61 + 12*lmMt) - 12*
        pow2(lmMst2))*pow2(Mst2)*pow4(Mt) + 4*Dmglst2*(11 + 42*lmMst1 - 42*
        lmMst2)*Mt*pow3(s2t)*pow5(Mst2) + (-144*(lmMst1 - lmMst2)*Mst2*
        xDR2DRMOD*(2*Dmglst2*lmMst2*Mst2 + (-2 + lmMst2)*xDmglst2*pow2(Dmglst2)
        + (1 + lmMst2)*pow2(Mst2))*pow2(Mt)*pow2(MuSUSY)*pow2(s2t)*(-1 + pow2(
        Sbeta)) + (xDmglst2*pow2(Dmglst2)*(-4*Mst2*pow2(Mt)*pow2(s2t)*(Tbeta*(-
        157 + lmMst1*(348 - 36*lmMst2) - 348*lmMst2 + 36*pow2(lmMst2))*pow2(
        MuSUSY)*(-1 + pow2(Sbeta)) + 18*Mst2*((85 - 36*lmMst1 + 36*lmMst2)*
        MuSUSY + 2*(4 - 11*lmMst1 + 11*lmMst2)*Mst2*Tbeta)*pow2(Sbeta)) + 96*
        s2t*((-143 + 18*lmMst1 - 18*lmMst2)*Tbeta*pow2(MuSUSY)*(-1 + pow2(
        Sbeta)) + 3*Mst2*((13 - 21*lmMst1 + 21*lmMst2)*MuSUSY + Mst2*Tbeta*(-18
        - 169*lmMst2 + 3*lmMst1*(49 + 10*lmMst2 - 10*lmMt) + 22*lmMt + 30*
        lmMst2*lmMt - 30*pow2(lmMst2)))*pow2(Sbeta))*pow3(Mt) + pow2(Sbeta)*(
        16*Mt*((1 - 24*lmMst1 + 24*lmMst2)*MuSUSY + 6*(1 - 6*lmMst1 + 6*lmMst2)
        *Mst2*Tbeta)*pow3(Mst2)*pow3(s2t) + 32*(9*Mst2*Tbeta*(94 + 475*lmMst2 -
        6*lmMst1*(67 + 14*lmMst2 - 14*lmMt) - 73*lmMt - 84*lmMst2*lmMt + 84*
        pow2(lmMst2)) + MuSUSY*(398 + 2085*lmMst2 - 18*lmMst1*(95 + 22*lmMst2 -
        22*lmMt) - 375*lmMt - 396*lmMst2*lmMt + 396*pow2(lmMst2)))*pow4(Mt) + (
        5 + 6*lmMst1 - 6*lmMst2)*Tbeta*pow4(s2t)*pow5(Mst2))))/Tbeta)/pow2(
        Sbeta) + 2*(5 + 6*lmMst1 - 6*lmMst2)*(-2*Mt + Dmglst2*s2t)*pow3(s2t)*
        pow6(Mst2) - (11 + 6*lmMst1 - 6*lmMst2)*pow4(s2t)*pow7(Mst2))))/108. +
        (Al4p*z2*(-68040*twoLoopFlag*pow2(Mst1)*pow3(Mst2)*pow4(Msq)*(pow5(
        Mst2)*(4*Mst2*MuSUSY*pow2(Mt)*pow2(s2t)*(-(MuSUSY*Tbeta*(-1 + pow2(
        Sbeta))) + 6*Mst2*pow2(Sbeta)) + 16*s2t*Tbeta*(pow2(MuSUSY)*(-1 + pow2(
        Sbeta)) - 4*pow2(Mst2)*pow2(Sbeta))*pow3(Mt) - 4*Mt*(MuSUSY + 2*Mst2*
        Tbeta)*pow2(Sbeta)*pow3(Mst2)*pow3(s2t) + 32*(2*MuSUSY + Mst2*Tbeta)*
        pow2(Sbeta)*pow4(Mt) + Tbeta*pow2(Sbeta)*pow4(s2t)*pow5(Mst2)) + Mst2*
        pow4(Mst1)*(16*s2t*Tbeta*(pow2(MuSUSY)*(-1 + pow2(Sbeta)) - 4*pow2(
        Mst2)*pow2(Sbeta))*pow3(Mt) + 96*(2*MuSUSY + Mst2*Tbeta)*pow2(Sbeta)*
        pow4(Mt) + Tbeta*pow2(Sbeta)*pow4(s2t)*pow5(Mst2)) - 2*pow2(Mst1)*pow3(
        Mst2)*(pow2(Sbeta)*(-2*Mt*(MuSUSY + 2*Mst2*Tbeta)*pow3(Mst2)*pow3(s2t)
        - 32*(2*MuSUSY + Mst2*Tbeta)*pow4(Mt)) + Tbeta*(8*s2t*(pow2(MuSUSY) + (
        4*pow2(Mst2) - pow2(MuSUSY))*pow2(Sbeta))*pow3(Mt) + pow2(Sbeta)*pow4(
        s2t)*pow5(Mst2))) + 4*Dmglst2*(4*pow2(Mt)*(-12*(MuSUSY + 2*Mst2*Tbeta)*
        pow2(Mt)*pow2(Sbeta) + Mst2*MuSUSY*pow2(s2t)*(-(MuSUSY*Tbeta*(-1 +
        pow2(Sbeta))) + 6*Mst2*pow2(Sbeta)) + Mt*s2t*Tbeta*(13*pow2(MuSUSY)*(-1
        + pow2(Sbeta)) + 12*pow2(Mst2)*pow2(Sbeta)))*pow4(Mst1) + pow2(Mst1)*
        pow2(Mst2)*(8*s2t*(3*MuSUSY*s2t + 2*Mt*Tbeta)*pow2(Mst2)*pow2(Mt)*pow2(
        Sbeta) + Tbeta*(-4*Mst2*pow2(Mt)*pow2(MuSUSY)*pow2(s2t)*(-1 + pow2(
        Sbeta)) + 36*s2t*pow2(MuSUSY)*(-1 + pow2(Sbeta))*pow3(Mt) + (2*Mt -
        Mst2*s2t)*pow2(Sbeta)*pow3(s2t)*pow4(Mst2) - 32*Mst2*pow2(Sbeta)*pow4(
        Mt))) + pow4(Mst2)*(2*Mst2*MuSUSY*pow2(Mt)*pow2(s2t)*(-2*MuSUSY*Tbeta*(
        -1 + pow2(Sbeta)) + 15*Mst2*pow2(Sbeta)) + 4*s2t*Tbeta*(5*pow2(MuSUSY)*
        (-1 + pow2(Sbeta)) - 4*pow2(Mst2)*pow2(Sbeta))*pow3(Mt) - 2*Mt*(2*
        MuSUSY + 5*Mst2*Tbeta)*pow2(Sbeta)*pow3(Mst2)*pow3(s2t) + 16*MuSUSY*
        pow2(Sbeta)*pow4(Mt) + Tbeta*pow2(Sbeta)*pow4(s2t)*pow5(Mst2)))) - 6*
        xDmglst2*pow2(Dmglst2)*pow2(Mst2)*(22680*twoLoopFlag*pow2(Mst1)*pow4(
        Msq)*(-4*pow2(Mt)*(-8*(7*MuSUSY + 15*Mst2*Tbeta)*pow2(Mt)*pow2(Sbeta) +
        Mst2*MuSUSY*pow2(s2t)*(MuSUSY*Tbeta*(-1 + pow2(Sbeta)) - 18*Mst2*pow2(
        Sbeta)) + 12*Mt*s2t*Tbeta*(-3*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 4*pow2(
        Mst2)*pow2(Sbeta)))*pow4(Mst1) + s2t*(4*(12*Mt - Mst2*s2t)*Tbeta*pow2(
        Mt)*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + s2t*pow2(Mst2)*pow2(Sbeta)*(-4*
        Mst2*Mt*s2t*(MuSUSY + 6*Mst2*Tbeta) + 72*MuSUSY*pow2(Mt) + Tbeta*pow2(
        s2t)*pow3(Mst2)))*pow4(Mst2) - pow2(Mst1)*pow2(Mst2)*(-4*Mst2*MuSUSY*
        pow2(Mt)*pow2(s2t)*(-(MuSUSY*Tbeta*(-1 + pow2(Sbeta))) + 18*Mst2*pow2(
        Sbeta)) + 32*s2t*Tbeta*(-3*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + pow2(Mst2)
        *pow2(Sbeta))*pow3(Mt) - 32*(MuSUSY + 3*Mst2*Tbeta)*pow2(Sbeta)*pow4(
        Mt) + Tbeta*pow2(Sbeta)*pow4(s2t)*pow5(Mst2))) + Al4p*threeLoopFlag*
        pow2(Mst2)*(pow4(Msq)*(196560*(-4*Tbeta*pow2(Mt)*pow2(s2t)*(pow2(
        MuSUSY)*(-1 + pow2(Sbeta)) + 2*pow2(Mst2)*pow2(Sbeta)) + pow2(Sbeta)*(
        16*MuSUSY*s2t*pow3(Mt) - 4*Mt*MuSUSY*pow2(Mst2)*pow3(s2t) + 16*Tbeta*
        pow4(Mt) + Tbeta*pow4(Mst2)*pow4(s2t)))*pow5(Mst2) - 3*Mst2*pow2(Mst1)*
        (-4*pow2(Mst2)*pow2(Mt)*(7*(8599 + 16200*lmMsq + 12240*lmMst1 - 43920*
        lmMst2)*Tbeta*pow2(MuSUSY)*pow2(s2t)*(-1 + pow2(Sbeta)) + 4*Mt*(3*(
        85649 + 2100*lmMst1 + 441420*lmMst2)*MuSUSY*s2t + (1361431 - 32760*
        lmMst1 + 5040*lmMst2 - 133560*lmMt)*Mt*Tbeta)*pow2(Sbeta)) + 4*Mt*s2t*(
        6*Mt*s2t*(14*(-63187 + 32400*lmMsq + 810*lmMst1 - 52650*lmMst2)*MuSUSY
        + (63809 + 2100*lmMst1 + 441420*lmMst2)*Mst2*Tbeta) + 16*(-12727 +
        5670*lmMst1 + 6300*lmMst2 - 11970*lmMt)*Tbeta*pow2(Mt) - 7*(17959 +
        16200*lmMsq + 12240*lmMst1 - 43920*lmMst2)*Mst2*MuSUSY*pow2(s2t))*pow2(
        Sbeta)*pow3(Mst2) + 32*Mst2*MuSUSY*(7*(-63187 + 32400*lmMsq + 810*
        lmMst1 - 52650*lmMst2)*MuSUSY*s2t*Tbeta*(-1 + pow2(Sbeta)) + 4*(9356 -
        2835*lmMst1 - 3150*lmMst2 + 5985*lmMt)*Mt*pow2(Sbeta))*pow3(Mt) +
        Tbeta*(-5376*(17 + 1320*lmMst2)*pow2(MuSUSY)*(-1 + pow2(Sbeta))*pow4(
        Mt) + 7*(16*(63187 - 32400*lmMsq - 810*lmMst1 + 52650*lmMst2)*Mt + (
        27319 + 16200*lmMsq + 12240*lmMst1 - 43920*lmMst2)*Mst2*s2t)*pow2(
        Sbeta)*pow3(s2t)*pow5(Mst2))) - 4*pow4(Mst1)*(Mst2*pow2(Mt)*pow2(s2t)*(
        -14*(249097 + 24300*lmMsq + 30510*lmMst1 - 78030*lmMst2)*Tbeta*pow2(
        MuSUSY)*(-1 + pow2(Sbeta)) + 3*Mst2*(7*(708829 + 388800*lmMsq + 198720*
        lmMst1 - 820800*lmMst2)*MuSUSY + 30*(160243 - 16632*lmMst1 + 97272*
        lmMst2)*Mst2*Tbeta)*pow2(Sbeta)) - 2*s2t*(-35*(-9883 + 155520*lmMsq +
        41688*lmMst1 - 290520*lmMst2)*Tbeta*pow2(MuSUSY)*(-1 + pow2(Sbeta)) +
        8*Mst2*(9*(221716 - 20265*lmMst1 + 231945*lmMst2)*MuSUSY + 2*(-3042896
        + 104895*lmMst1 - 281610*lmMst2 + 55755*lmMt)*Mst2*Tbeta)*pow2(Sbeta))*
        pow3(Mt) + pow2(Sbeta)*(-7*Mt*((472397 + 24300*lmMst1 - 24300*lmMst2)*
        MuSUSY + (1467073 + 189000*lmMst1 - 189000*lmMst2)*Mst2*Tbeta)*pow3(
        Mst2)*pow3(s2t) - 4*(4*(5973901 - 192780*lmMst1 + 582120*lmMst2 -
        147420*lmMt)*MuSUSY + (47240777 + 680400*lmMsq - 3152520*lmMst1 +
        6705720*lmMst2 - 1496880*lmMt)*Mst2*Tbeta)*pow4(Mt) + 35*(20926 - 2430*
        lmMsq - 621*lmMst1 + 5373*lmMst2)*Tbeta*pow4(s2t)*pow5(Mst2)))) +
        28350*pow2(Mst1)*pow2(Mst2)*(-(pow2(Mst2)*(2*pow2(Mst2)*(12*Mst2*
        MuSUSY*pow2(Mt)*pow2(s2t)*(-5*MuSUSY*Tbeta*(-1 + pow2(Sbeta)) + 46*
        Mst2*pow2(Sbeta)) + 16*s2t*Tbeta*(23*pow2(MuSUSY)*(-1 + pow2(Sbeta)) -
        24*pow2(Mst2)*pow2(Sbeta))*pow3(Mt) - 4*Mt*(15*MuSUSY + 46*Mst2*Tbeta)*
        pow2(Sbeta)*pow3(Mst2)*pow3(s2t) + 96*(4*MuSUSY - Mst2*Tbeta)*pow2(
        Sbeta)*pow4(Mt) + 15*Tbeta*pow2(Sbeta)*pow4(s2t)*pow5(Mst2)) + pow2(
        Mst1)*(4*Mst2*MuSUSY*pow2(Mt)*pow2(s2t)*(-25*MuSUSY*Tbeta*(-1 + pow2(
        Sbeta)) + 216*Mst2*pow2(Sbeta)) + 1312*s2t*Tbeta*pow2(MuSUSY)*(-1 +
        pow2(Sbeta))*pow3(Mt) + 20*Mt*(MuSUSY + 4*Mst2*Tbeta)*pow2(Sbeta)*pow3(
        Mst2)*pow3(s2t) + pow2(Sbeta)*(768*MuSUSY*pow4(Mt) - 35*Tbeta*pow4(s2t)
        *pow5(Mst2))))) - 2*pow2(Msq)*(pow2(Mst2)*(4*Mst2*MuSUSY*pow2(Mt)*pow2(
        s2t)*(-19*MuSUSY*Tbeta*(-1 + pow2(Sbeta)) + 216*Mst2*pow2(Sbeta)) + 64*
        s2t*Tbeta*(9*pow2(MuSUSY)*(-1 + pow2(Sbeta)) - 4*pow2(Mst2)*pow2(Sbeta)
        )*pow3(Mt) - 4*Mt*(19*MuSUSY + 72*Mst2*Tbeta)*pow2(Sbeta)*pow3(Mst2)*
        pow3(s2t) + 32*(8*MuSUSY - Mst2*Tbeta)*pow2(Sbeta)*pow4(Mt) + 19*Tbeta*
        pow2(Sbeta)*pow4(s2t)*pow5(Mst2)) + 4*pow2(Mst1)*(6*Mst2*MuSUSY*pow2(
        Mt)*pow2(s2t)*(-3*MuSUSY*Tbeta*(-1 + pow2(Sbeta)) + 32*Mst2*pow2(Sbeta)
        ) + 272*s2t*Tbeta*pow2(MuSUSY)*(-1 + pow2(Sbeta))*pow3(Mt) + Mt*(MuSUSY
        + 8*Mst2*Tbeta)*pow2(Sbeta)*pow3(Mst2)*pow3(s2t) + pow2(Sbeta)*(64*
        MuSUSY*pow4(Mt) - 5*Tbeta*pow4(s2t)*pow5(Mst2))))))) + 2*Al4p*
        threeLoopFlag*pow4(Mst2)*(340200*Mst2*xMsq*pow6(Msq)*(-(pow2(s2t)*pow4(
        Mst1)*(8*(-shiftst1 + lmMst1*(-1 + 2*shiftst1 - shiftst2) + shiftst2 +
        lmMst2*(1 - 2*shiftst1 + shiftst2))*Tbeta*pow2(Mt)*pow2(MuSUSY)*(-1 +
        pow2(Sbeta)) - 4*Mt*(-1 + shiftst2)*(-(MuSUSY*s2t) + 2*Mt*Tbeta)*pow2(
        Mst2)*pow2(Sbeta) + (1 + 2*(lmMst1 - lmMst2)*(-1 + shiftst1) + 3*
        shiftst1 - 4*shiftst2)*Tbeta*pow2(s2t)*pow2(Sbeta)*pow4(Mst2))) + (1 -
        shiftst1)*pow4(Mst2)*(-4*Tbeta*pow2(Mt)*pow2(s2t)*(pow2(MuSUSY)*(-1 +
        pow2(Sbeta)) + 2*pow2(Mst2)*pow2(Sbeta)) + pow2(Sbeta)*(16*MuSUSY*s2t*
        pow3(Mt) - 4*Mt*MuSUSY*pow2(Mst2)*pow3(s2t) + 16*Tbeta*pow4(Mt) +
        Tbeta*pow4(Mst2)*pow4(s2t))) - pow2(Mst1)*pow2(Mst2)*(-4*Tbeta*pow2(Mt)
        *pow2(s2t)*((1 + 2*(lmMst1 - lmMst2)*(-1 + shiftst1) - 2*shiftst1 +
        shiftst2)*pow2(MuSUSY)*(1 - pow2(Sbeta)) + 2*(2 + (-1 + lmMst1 -
        lmMst2)*shiftst1 + (-1 - lmMst1 + lmMst2)*shiftst2)*pow2(Mst2)*pow2(
        Sbeta)) + pow2(Sbeta)*(-16*MuSUSY*s2t*(-1 + shiftst2)*pow3(Mt) + 4*Mt*
        MuSUSY*(shiftst1 - shiftst2 + lmMst1*(-2 + shiftst1 + shiftst2) -
        lmMst2*(-2 + shiftst1 + shiftst2))*pow2(Mst2)*pow3(s2t) + 16*(-1 +
        shiftst2)*Tbeta*pow4(Mt) + (1 - 4*shiftst1 - 2*(lmMst1 - lmMst2)*(-1 +
        shiftst2) + 3*shiftst2)*Tbeta*pow4(Mst2)*pow4(s2t))) - (-1 + shiftst2)*
        Tbeta*pow2(Mst2)*pow2(Sbeta)*pow4(s2t)*pow6(Mst1)) + xDR2DRMOD*(-
        181440*Mt*pow2(Sbeta)*(-40*Tbeta*xDmglst2*pow2(Dmglst2)*pow3(Mt) + (23*
        Dmglst2 + 13*Mst2)*MuSUSY*pow3(Mst2)*pow3(s2t))*pow4(Msq)*pow5(Mst2) +
        xDmglst2*pow2(Dmglst2)*(90720*pow4(Msq)*(pow4(Mst1)*(8*Mst2*pow2(Mt)*
        pow2(s2t)*((-4 + 5*lmMst1 - 3*lmMst2)*Tbeta*pow2(MuSUSY)*(-1 + pow2(
        Sbeta)) + Mst2*((24 - 36*lmMst2)*MuSUSY - 5*Mst2*Tbeta)*pow2(Sbeta)) -
        128*s2t*Tbeta*((-2 + 3*lmMst2)*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 2*(4 +
        lmMst2)*pow2(Mst2)*pow2(Sbeta))*pow3(Mt) + (20*Mt*MuSUSY + (-13 + 10*
        lmMst1 - 6*lmMst2)*s2t*Tbeta*pow2(Mst2))*pow2(Sbeta)*pow3(Mst2)*pow3(
        s2t) + 128*((7 + 2*lmMst2)*MuSUSY + 3*(8 + 3*lmMst2)*Mst2*Tbeta)*pow2(
        Sbeta)*pow4(Mt)) + 5*s2t*(-4*s2t*Tbeta*pow2(Mt)*(pow2(MuSUSY)*(-1 +
        pow2(Sbeta)) + 2*pow2(Mst2)*pow2(Sbeta)) + 4*MuSUSY*pow2(Sbeta)*(-(Mt*
        pow2(Mst2)*pow2(s2t)) + 4*pow3(Mt)) + Tbeta*pow2(Sbeta)*pow3(s2t)*pow4(
        Mst2))*pow5(Mst2) + pow2(Mst1)*pow2(Mst2)*(4*Mst2*pow2(Mt)*pow2(s2t)*((
        -13 + 10*lmMst1 - 6*lmMst2)*Tbeta*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + 4*
        Mst2*(6*(2 - 3*lmMst2)*MuSUSY + 5*Mst2*Tbeta)*pow2(Sbeta)) - 16*s2t*(4*
        (-2 + 3*lmMst2)*Tbeta*pow2(MuSUSY)*(-1 + pow2(Sbeta)) + Mst2*(5*MuSUSY
        - 8*Mst2*Tbeta)*pow2(Sbeta))*pow3(Mt) + pow2(Sbeta)*(8*Mt*((-4 + 5*
        lmMst1 - 3*lmMst2)*MuSUSY + 4*(-2 + 3*lmMst2)*Mst2*Tbeta)*pow3(Mst2)*
        pow3(s2t) - 16*(8*MuSUSY - 13*Mst2*Tbeta)*pow4(Mt) + (3 - 10*lmMst1 +
        6*lmMst2)*Tbeta*pow4(s2t)*pow5(Mst2)))) + (s2t*pow2(Sbeta)*(12*s2t*(7*(
        19432961 + 1166400*lmMsq - 278640*lmMst1 - 1898640*lmMst2)*MuSUSY - 2*(
        38749 + 3738420*lmMst1 - 7367220*lmMst2)*Mst2*Tbeta)*pow2(Mt) + 56*
        Mst2*Mt*((-4649068 + 546750*lmMst1 - 546750*lmMst2)*MuSUSY + (-8549557
        + 437400*lmMst1 - 437400*lmMst2)*Mst2*Tbeta)*pow2(s2t) + 64*(88729639 +
        2041200*lmMsq - 5210730*lmMst1 + 4683420*lmMst2 - 561330*lmMt)*Tbeta*
        pow3(Mt) + 35*(1589149 - 233280*lmMst1 + 233280*lmMst2)*Tbeta*pow3(
        Mst2)*pow3(s2t))*pow4(Msq) + 170100*pow2(Msq)*(24*Mst2*MuSUSY*pow2(Mt)*
        pow2(s2t)*(-3*MuSUSY*Tbeta*(-1 + pow2(Sbeta)) + 32*Mst2*pow2(Sbeta)) +
        1600*s2t*Tbeta*pow2(MuSUSY)*(-1 + pow2(Sbeta))*pow3(Mt) + 32*(8*MuSUSY
        - 3*Mst2*Tbeta)*pow2(Sbeta)*pow4(Mt) + Tbeta*pow2(Sbeta)*pow4(s2t)*
        pow5(Mst2)) + 85050*pow2(Mst2)*(4*Mst2*MuSUSY*pow2(Mt)*pow2(s2t)*(-25*
        MuSUSY*Tbeta*(-1 + pow2(Sbeta)) + 216*Mst2*pow2(Sbeta)) + 1888*s2t*
        Tbeta*pow2(MuSUSY)*(-1 + pow2(Sbeta))*pow3(Mt) + pow2(Sbeta)*(768*
        MuSUSY*pow4(Mt) + 5*Tbeta*pow4(s2t)*pow5(Mst2))))*pow6(Mst1)) + 45360*
        pow4(Msq)*(4*Mt*MuSUSY*(Mt*MuSUSY*s2t*Tbeta*((32*(Dmglst2 + 3*Dmglst2*
        lmMst2 + Mst2 + lmMst2*Mst2)*Mt + Mst2*(Dmglst2*(23 - 46*lmMst1 + 30*
        lmMst2) + (5 - 26*lmMst1 + 18*lmMst2)*Mst2)*s2t)*pow2(Mst1)*pow3(Mst2)
        + 2*(Mst2*(32*(Dmglst2 + 3*Dmglst2*lmMst2 + Mst2 + lmMst2*Mst2)*Mt -
        Mst2*(Dmglst2*(23*lmMst1 - 15*lmMst2) + (4 + 13*lmMst1 - 9*lmMst2)*
        Mst2)*s2t)*pow4(Mst1) + (48*(1 + lmMst2)*Mt + Dmglst2*(-23*lmMst1 + 15*
        lmMst2)*s2t + (-4 - 13*lmMst1 + 9*lmMst2)*Mst2*s2t)*pow6(Mst1)) + (23*
        Dmglst2 + 13*Mst2)*s2t*pow6(Mst2)) + pow2(Sbeta)*(2*pow2(Mst1)*pow3(
        Mst2)*(Dmglst2*(-46*Mst2*s2t*pow2(Mt) - 24*(1 + 3*lmMst2)*Mt*pow2(Mst2)
        *pow2(s2t) + 32*(-1 + lmMst2)*pow3(Mt) + (23*lmMst1 - 15*lmMst2)*pow3(
        Mst2)*pow3(s2t)) + Mst2*(-26*Mst2*s2t*pow2(Mt) - 24*(1 + lmMst2)*Mt*
        pow2(Mst2)*pow2(s2t) + 32*(1 + lmMst2)*pow3(Mt) + (4 + 13*lmMst1 - 9*
        lmMst2)*pow3(Mst2)*pow3(s2t))) - Mst2*(48*(Dmglst2 + 3*Dmglst2*lmMst2 +
        Mst2 + lmMst2*Mst2)*Mt*pow2(Mst2)*pow2(s2t) + 64*(Dmglst2*(7 + lmMst2)
        - 3*(1 + lmMst2)*Mst2)*pow3(Mt) - (23*Dmglst2 + 13*Mst2)*pow3(Mst2)*
        pow3(s2t))*pow4(Mst1) + 48*(-(Mst2*(Dmglst2 + 3*Dmglst2*lmMst2 + Mst2 +
        lmMst2*Mst2)*Mt*pow2(s2t)) + 8*(1 + lmMst2)*pow3(Mt))*pow6(Mst1) + 4*(
        23*Dmglst2 + 13*Mst2)*s2t*pow2(Mt)*pow6(Mst2))) + Tbeta*pow2(Sbeta)*(-
        128*s2t*pow2(Mst1)*pow3(Mt)*((1 + lmMst2)*(2*pow2(Mst2) + pow2(MuSUSY))
        *(2*pow2(Mst1)*pow2(Mst2) + 3*pow4(Mst1) + pow4(Mst2)) - Dmglst2*Mst2*(
        -((1 + 3*lmMst2)*pow2(Mst2)*pow2(MuSUSY)) + pow2(Mst1)*(4*(3 + lmMst2)*
        pow2(Mst2) - 2*(1 + 3*lmMst2)*pow2(MuSUSY)) + 6*(5 + 3*lmMst2)*pow4(
        Mst1) - 2*(-1 + lmMst2)*pow4(Mst2))) + (pow2(Mst1) - pow2(Mst2))*pow4(
        Mst2)*pow4(s2t)*(2*(4 + 13*lmMst1 - 9*lmMst2)*pow2(Mst1)*pow3(Mst2) +
        13*Mst2*pow4(Mst1) + Dmglst2*(2*(23*lmMst1 - 15*lmMst2)*pow2(Mst1)*
        pow2(Mst2) + 23*pow4(Mst1) - 23*pow4(Mst2)) - 13*pow5(Mst2)) + 64*(
        Dmglst2 + 3*Dmglst2*lmMst2 + Mst2 + lmMst2*Mst2)*Mt*pow2(Mst1)*pow3(
        s2t)*pow7(Mst2) + 16*pow4(Mt)*(48*(1 + lmMst2)*Mst2*(2*pow2(Mst1) +
        pow2(Mst2))*pow4(Mst1) + (29 + 16*lmMst2)*pow2(Mst1)*pow5(Mst2) -
        Dmglst2*(96*(2 + lmMst2)*pow2(Mst2)*pow4(Mst1) + 9*pow2(Mst1)*pow4(
        Mst2) + 192*(3 + 2*lmMst2)*pow6(Mst1) - 23*pow6(Mst2)) + 13*pow7(Mst2))
        + 4*pow2(Mt)*pow2(s2t)*(2*(-13*pow2(Mst2) + (4 + 13*lmMst1 - 9*lmMst2)*
        pow2(MuSUSY))*pow3(Mst2)*pow4(Mst1) + pow2(Mst1)*(52*pow2(Mst2) + (-5 +
        26*lmMst1 - 18*lmMst2)*pow2(MuSUSY))*pow5(Mst2) + 2*(4 + 13*lmMst1 - 9*
        lmMst2)*Mst2*pow2(MuSUSY)*pow6(Mst1) + Dmglst2*(-2*pow2(Mst2)*(23*pow2(
        Mst2) + (-23*lmMst1 + 15*lmMst2)*pow2(MuSUSY))*pow4(Mst1) + pow2(Mst1)*
        (92*pow2(Mst2) + (-23 + 46*lmMst1 - 30*lmMst2)*pow2(MuSUSY))*pow4(Mst2)
        + 2*(23*lmMst1 - 15*lmMst2)*pow2(MuSUSY)*pow6(Mst1) - 23*(2*pow2(Mst2)
        + pow2(MuSUSY))*pow6(Mst2)) - 13*(2*pow2(Mst2) + pow2(MuSUSY))*pow7(
        Mst2)))))) + 544320*twoLoopFlag*xMst*pow2(Mt)*(xDmglst2*pow2(Dmglst2)*(
        -16*(11*MuSUSY + 21*Mst2*Tbeta)*pow2(Mt)*pow2(Sbeta) + Mst2*MuSUSY*
        pow2(s2t)*(MuSUSY*Tbeta*(-1 + pow2(Sbeta)) - 18*Mst2*pow2(Sbeta)) - 24*
        Mt*s2t*Tbeta*(2*pow2(MuSUSY)*(-1 + pow2(Sbeta)) - 5*pow2(Mst2)*pow2(
        Sbeta))) + 2*Mt*pow2(Mst2)*(-8*Mt*(2*MuSUSY + Mst2*Tbeta)*pow2(Sbeta) +
        s2t*Tbeta*(-(pow2(MuSUSY)*(-1 + pow2(Sbeta))) + 4*pow2(Mst2)*pow2(
        Sbeta))) - 2*Dmglst2*Mst2*(-16*(2*MuSUSY + 3*Mst2*Tbeta)*pow2(Mt)*pow2(
        Sbeta) + Mst2*MuSUSY*pow2(s2t)*(-(MuSUSY*Tbeta*(-1 + pow2(Sbeta))) + 6*
        Mst2*pow2(Sbeta)) + Mt*s2t*Tbeta*(17*pow2(MuSUSY)*(-1 + pow2(Sbeta)) +
        20*pow2(Mst2)*pow2(Sbeta))))*pow4(Msq)*pow8(Mst1) + Al4p*threeLoopFlag*
        pow4(Mst2)*(28*Mt*MuSUSY*pow2(Sbeta)*(-(pow4(Msq)*(-27*pow2(Mst1)*pow3(
        Mst2)*(Dmglst2*(32*(-4879 + 270*lmMst1 - 8910*lmMst2)*Mst2*s2t*pow2(Mt)
        - 3*(278347 - 79200*lmMsq + 3720*lmMst1 + 119160*lmMst2)*Mt*pow2(Mst2)*
        pow2(s2t) + 60*(1805 + 960*lmMsq + 248*lmMst1 - 2872*lmMst2 - 448*lmMt)
        *pow3(Mt) + 20*(3599 - 1080*lmMsq - 1368*lmMst1 + 3480*lmMst2)*pow3(
        Mst2)*pow3(s2t)) - Mst2*(2*(29683 - 13560*lmMst1 + 36600*lmMst2)*Mst2*
        s2t*pow2(Mt) + 45*(3469 - 1440*lmMsq + 664*lmMst1 + 1384*lmMst2)*Mt*
        pow2(Mst2)*pow2(s2t) + 4*(88373 - 14400*lmMsq - 9960*lmMst1 + 52680*
        lmMst2 + 3360*lmMt)*pow3(Mt) + 20*(235 + 450*lmMsq + 642*lmMst1 - 1362*
        lmMst2 + 18*(1 + lmMst1)*shiftst3 - 18*lmMst2*shiftst3)*pow3(Mst2)*
        pow3(s2t))) + 9*Mst2*(-3*Dmglst2*(-32*(21379 + 1770*lmMst1 + 12630*
        lmMst2)*Mst2*s2t*pow2(Mt) + (-1302523 + 216000*lmMsq + 106200*lmMst1 -
        428760*lmMst2)*Mt*pow2(Mst2)*pow2(s2t) + 32*(57943 + 1800*lmMsq - 2850*
        lmMst1 + 2190*lmMst2 - 1260*lmMt)*pow3(Mt) + 4*(33563 - 4020*lmMst1 +
        4020*lmMst2)*pow3(Mst2)*pow3(s2t)) + Mst2*(-2*(502399 + 65160*lmMst1 -
        134280*lmMst2)*Mst2*s2t*pow2(Mt) + 15*(15395 - 8640*lmMsq + 17928*
        lmMst1 - 8712*lmMst2)*Mt*pow2(Mst2)*pow2(s2t) + 64*(45986 - 2700*lmMsq
        - 7245*lmMst1 + 21015*lmMst2 + 630*lmMt)*pow3(Mt) + 2*(242149 - 2700*
        lmMsq - 9360*lmMst1 + 19440*lmMst2)*pow3(Mst2)*pow3(s2t)))*pow4(Mst1) +
        (12*(12*Dmglst2*(520939 + 44280*lmMst1 + 76680*lmMst2) + (-2414263 -
        203040*lmMst1 + 306720*lmMst2)*Mst2)*s2t*pow2(Mt) + 6*Mst2*(5*Dmglst2*(
        1991719 - 194400*lmMsq - 328536*lmMst1 + 618840*lmMst2) + 3*(63043 -
        64800*lmMsq + 217080*lmMst1 - 147960*lmMst2)*Mst2)*Mt*pow2(s2t) + 48*(
        1126591 - 184140*lmMst1 + 383940*lmMst2 + 7560*lmMt)*pow3(Mt) - (16*
        Dmglst2*(776573 - 80595*lmMst1 + 80595*lmMst2) + 3*(-2205079 + 76140*
        lmMst1 - 76140*lmMst2)*Mst2)*pow2(Mst2)*pow3(s2t))*pow6(Mst1) - 3240*
        s2t*(92*Dmglst2 + Mst2*(49 + 3*shiftst3))*(-4*pow2(Mt) + pow2(Mst2)*
        pow2(s2t))*pow6(Mst2))) + 4050*Mst2*pow2(Mst1)*(pow2(Mst2)*(24*(Mt*
        pow2(s2t)*pow3(Mst2) + 8*Mst2*pow3(Mt))*pow4(Mst1) + 192*pow3(Mt)*pow5(
        Mst2) + pow2(Mst1)*(192*pow3(Mst2)*pow3(Mt) + 24*Mt*pow2(s2t)*pow5(
        Mst2) + 5*pow3(s2t)*pow6(Mst2)) + 60*Mt*pow2(s2t)*pow7(Mst2) + 4*
        Dmglst2*(2*(39*Mt*pow2(Mst2)*pow2(s2t) + 88*pow3(Mt))*pow4(Mst1) + 176*
        pow3(Mt)*pow4(Mst2) + pow2(Mst1)*(176*pow2(Mst2)*pow3(Mt) + 78*Mt*pow2(
        s2t)*pow4(Mst2) + 5*pow3(s2t)*pow5(Mst2)) + 123*Mt*pow2(s2t)*pow6(Mst2)
        - 16*pow3(s2t)*pow7(Mst2)) - 9*pow3(s2t)*pow8(Mst2)) + 4*pow2(Msq)*(24*
        (Mt*pow2(s2t)*pow3(Mst2) + 4*Mst2*pow3(Mt))*pow4(Mst1) + 96*pow3(Mt)*
        pow5(Mst2) + 3*pow2(Mst1)*(32*pow3(Mst2)*pow3(Mt) + 8*Mt*pow2(s2t)*
        pow5(Mst2) + pow3(s2t)*pow6(Mst2)) + 48*Mt*pow2(s2t)*pow7(Mst2) +
        Dmglst2*(8*(27*Mt*pow2(Mst2)*pow2(s2t) + 28*pow3(Mt))*pow4(Mst1) + 224*
        pow3(Mt)*pow4(Mst2) + pow2(Mst1)*(224*pow2(Mst2)*pow3(Mt) + 216*Mt*
        pow2(s2t)*pow4(Mst2) + 6*pow3(s2t)*pow5(Mst2)) + 288*Mt*pow2(s2t)*pow6(
        Mst2) - 34*pow3(s2t)*pow7(Mst2)) - 7*pow3(s2t)*pow8(Mst2)))) + Tbeta*(-
        68040*Mst2*shiftst3*pow4(Msq)*(-4*pow2(Mst1)*pow2(s2t)*pow4(Mst2)*(2*(-
        1 + lmMst1 - lmMst2)*pow2(Mt)*(-(pow2(MuSUSY)*(-1 + pow2(Sbeta))) +
        pow2(Mst2)*pow2(Sbeta)) + pow2(s2t)*pow2(Sbeta)*pow4(Mst2)) + 8*(-1 +
        3*lmMst1 - 3*lmMst2)*pow2(Mt)*pow2(MuSUSY)*pow2(s2t)*(-1 + pow2(Sbeta))
        *pow6(Mst1) + (-4*pow2(Mt)*pow2(s2t)*(pow2(MuSUSY)*(-1 + pow2(Sbeta)) +
        2*pow2(Mst2)*pow2(Sbeta)) + pow2(Sbeta)*(16*pow4(Mt) + pow4(Mst2)*pow4(
        s2t)))*pow6(Mst2) + pow4(Mst1)*(8*(-1 + 2*lmMst1 - 2*lmMst2)*pow2(Mst2)
        *pow2(Mt)*pow2(MuSUSY)*pow2(s2t)*(-1 + pow2(Sbeta)) + (3 + 2*lmMst1 -
        2*lmMst2)*pow2(Sbeta)*pow4(s2t)*pow6(Mst2))) - 28*pow2(Mt)*pow2(MuSUSY)
        *(4050*s2t*pow2(Mst1)*pow2(Mst2)*((72*Mt - 4*Mst2*s2t)*pow2(Mst2)*pow4(
        Mst1) - 4*(-14*Mt + Mst2*s2t)*pow2(Mst1)*pow4(Mst2) + 4*pow2(Msq)*(-4*(
        -12*Mt + Mst2*s2t)*pow2(Mst1)*pow2(Mst2) + (64*Mt - 4*Mst2*s2t)*pow4(
        Mst1) + (32*Mt - 7*Mst2*s2t)*pow4(Mst2)) + (40*Mt - 9*Mst2*s2t)*pow6(
        Mst2)) + 16200*Dmglst2*Mst2*s2t*pow2(Mst1)*((186*Mt - 11*Mst2*s2t)*
        pow2(Mst2)*pow4(Mst1) + (134*Mt - 11*Mst2*s2t)*pow2(Mst1)*pow4(Mst2) +
        2*pow2(Msq)*(-14*(-12*Mt + Mst2*s2t)*pow2(Mst1)*pow2(Mst2) + 2*(120*Mt
        - 7*Mst2*s2t)*pow4(Mst1) + (96*Mt - 17*Mst2*s2t)*pow4(Mst2)) + 2*(41*Mt
        - 8*Mst2*s2t)*pow6(Mst2)) + 2*Dmglst2*pow4(Msq)*(-27*pow2(Mst1)*pow2(
        Mst2)*((278347 - 79200*lmMsq + 3720*lmMst1 + 119160*lmMst2)*Mst2*Mt*s2t
        + 7680*(5 + 6*lmMst2)*pow2(Mt) + 10*(-4151 + 1080*lmMsq + 1368*lmMst1 -
        3480*lmMst2)*pow2(Mst2)*pow2(s2t)) - 36*((534391 - 113400*lmMsq -
        23760*lmMst1 + 196560*lmMst2)*Mst2*Mt*s2t + 30*(3643 - 120*lmMst1 +
        3192*lmMst2)*pow2(Mt) + 9*(-9053 + 900*lmMsq + 1810*lmMst1 - 3570*
        lmMst2)*pow2(Mst2)*pow2(s2t))*pow4(Mst1) + pow2(s2t)*(4*(2286439 -
        72900*lmMsq - 307800*lmMst1 + 450360*lmMst2)*pow6(Mst1) + 149040*pow6(
        Mst2))) + 3*pow4(Msq)*(90*pow2(Mst1)*(3*(-3469 + 1440*lmMsq - 664*
        lmMst1 - 1384*lmMst2)*Mst2*Mt*s2t - 64*(53 + 24*lmMst2)*pow2(Mt) + 2*(
        59 - 450*lmMsq - 642*lmMst1 + 1362*lmMst2)*pow2(Mst2)*pow2(s2t))*pow3(
        Mst2) - 6*Mst2*(-40*(-5827 + 2700*lmMsq - 2988*lmMst1 - 468*lmMst2)*
        Mst2*Mt*s2t + 60*(-353 + 72*lmMst1 + 696*lmMst2)*pow2(Mt) + (240379 +
        10800*lmMsq + 9900*lmMst1 - 21420*lmMst2)*pow2(Mst2)*pow2(s2t))*pow4(
        Mst1) + s2t*(4*(-412663 + 226800*lmMsq - 396360*lmMst1 + 119880*lmMst2)
        *Mt + (-3647353 - 64800*lmMsq + 16740*lmMst1 + 52380*lmMst2)*Mst2*s2t)*
        pow6(Mst1) + 52920*pow2(s2t)*pow7(Mst2))) + pow2(Sbeta)*(-7*pow4(Mst2)*
        pow4(s2t)*(540*pow2(Msq)*pow2(Mst1)*((Dmglst2*(3047 - 1080*lmMsq -
        1368*lmMst1 + 3480*lmMst2) - (529 + 450*lmMsq + 642*lmMst1 - 1362*
        lmMst2)*Mst2)*pow2(Msq) - 1020*Dmglst2*pow2(Mst2) - 210*pow3(Mst2))*
        pow4(Mst2) - 18*pow2(Mst2)*pow4(Mst1)*(-9000*pow2(Msq)*pow3(Mst2) + (
        235099 - 16200*lmMsq - 28620*lmMst1 + 60300*lmMst2)*Mst2*pow4(Msq) -
        12*Dmglst2*(3000*pow2(Msq)*pow2(Mst2) + 2*(3892 + 1350*lmMsq + 705*
        lmMst1 - 3345*lmMst2)*pow4(Msq) + 1575*pow4(Mst2)) - 3150*pow5(Mst2)) +
        (Dmglst2*(-97200*pow2(Msq)*pow2(Mst2) + (8800364 - 855360*lmMst1 +
        855360*lmMst2)*pow4(Msq) - 81000*pow4(Mst2)) - 15*(3240*pow2(Msq)*pow3(
        Mst2) + (150437 + 3240*lmMsq - 3996*lmMst1 - 8100*lmMst2)*Mst2*pow4(
        Msq) + 1350*pow5(Mst2)))*pow6(Mst1) + 3240*(92*Dmglst2 + 49*Mst2)*pow4(
        Msq)*pow6(Mst2)) - 28*pow2(Mt)*pow2(s2t)*(4050*pow2(Mst1)*pow2(Mst2)*
        pow2(MuSUSY)*(4*pow3(Mst2)*pow4(Mst1) + 4*pow2(Mst1)*pow5(Mst2) + 4*
        pow2(Msq)*(4*pow2(Mst1)*pow3(Mst2) + 4*Mst2*pow4(Mst1) + 7*pow5(Mst2))
        + 4*Dmglst2*(11*pow2(Mst2)*pow4(Mst1) + 11*pow2(Mst1)*pow4(Mst2) +
        pow2(Msq)*(28*pow2(Mst1)*pow2(Mst2) + 28*pow4(Mst1) + 34*pow4(Mst2)) +
        16*pow6(Mst2)) + 9*pow7(Mst2)) + pow4(Msq)*(-4*Dmglst2*(162*pow2(Mst2)*
        (40*(275 + 34*lmMst1 + 62*lmMst2)*pow2(Mst2) + (9053 - 900*lmMsq -
        1810*lmMst1 + 3570*lmMst2)*pow2(MuSUSY))*pow4(Mst1) - 27*pow2(Mst1)*(4*
        (-3499 + 270*lmMst1 - 8910*lmMst2)*pow2(Mst2) + 5*(-4151 + 1080*lmMsq +
        1368*lmMst1 - 3480*lmMst2)*pow2(MuSUSY))*pow4(Mst2) + 2*(45*(78533 +
        6732*lmMst1 + 180*lmMst2)*pow2(Mst2) + (2286439 - 72900*lmMsq - 307800*
        lmMst1 + 450360*lmMst2)*pow2(MuSUSY))*pow6(Mst1) + 74520*(2*pow2(Mst2)
        + pow2(MuSUSY))*pow6(Mst2)) + 3*(6*(4*(73931 + 3060*lmMst1 - 3060*
        lmMst2)*pow2(Mst2) + (240379 + 10800*lmMsq + 9900*lmMst1 - 21420*
        lmMst2)*pow2(MuSUSY))*pow3(Mst2)*pow4(Mst1) + 9*pow2(Mst1)*((-17923 +
        13560*lmMst1 - 36600*lmMst2)*pow2(Mst2) + 20*(-59 + 450*lmMsq + 642*
        lmMst1 - 1362*lmMst2)*pow2(MuSUSY))*pow5(Mst2) + Mst2*((3321329 +
        210600*lmMst1 - 210600*lmMst2)*pow2(Mst2) + (3647353 + 64800*lmMsq -
        16740*lmMst1 - 52380*lmMst2)*pow2(MuSUSY))*pow6(Mst1) - 52920*(2*pow2(
        Mst2) + pow2(MuSUSY))*pow7(Mst2)))) + 28*Mt*pow2(Mst1)*pow3(Mst2)*pow3(
        s2t)*(129600*pow2(Msq)*(3*Dmglst2*(pow2(Mst1) - 4*pow2(Mst2)) + Mst2*(
        pow2(Mst1) - 2*pow2(Mst2)))*pow4(Mst2) + pow4(Msq)*(18*Dmglst2*(233741
        + 10800*lmMsq - 58680*lmMst1 + 35640*lmMst2)*pow2(Mst1)*pow2(Mst2) +
        90*(-7913 + 2160*lmMsq + 5976*lmMst1 - 10584*lmMst2)*pow2(Mst1)*pow3(
        Mst2) + Dmglst2*(8194483 - 2329560*lmMst1 + 2329560*lmMst2)*pow4(Mst1)
        + 51*(-6167 + 9720*lmMst1 - 9720*lmMst2)*Mst2*pow4(Mst1) + 27*Dmglst2*(
        278347 - 79200*lmMsq + 3720*lmMst1 + 119160*lmMst2)*pow4(Mst2) + 405*(
        3469 - 1440*lmMsq + 664*lmMst1 + 1384*lmMst2)*pow5(Mst2)) + 48600*pow2(
        Mst1)*pow7(Mst2) + 16200*Dmglst2*(15*pow2(Mst1)*pow6(Mst2) - 41*pow8(
        Mst2))) - 16*pow4(Mt)*(-4*Dmglst2*pow4(Msq)*(27*pow2(Mst1)*pow2(Mst2)*(
        (114841 - 1680*lmMst1 + 31920*lmMst2 + 23520*lmMt)*pow2(Mst2) - 6720*(5
        + 6*lmMst2)*pow2(MuSUSY)) + 9*((2655197 + 75600*lmMsq - 325080*lmMst1 +
        778680*lmMst2 - 10080*lmMt)*pow2(Mst2) + 105*(-3643 + 120*lmMst1 -
        3192*lmMst2)*pow2(MuSUSY))*pow4(Mst1) + 4*(17026487 + 1530900*lmMsq -
        3163860*lmMst1 + 4320540*lmMst2 - 340200*lmMt)*pow6(Mst1) - 521640*
        pow6(Mst2)) - 453600*Dmglst2*pow2(Mst1)*(3*pow2(Msq)*pow2(Mst2)*(pow4(
        Mst1) - pow4(Mst2)) - 4*pow8(Mst2)) + 21*Mst2*(pow4(Msq)*(9*pow2(Mst1)*
        pow2(Mst2)*((54383 + 3600*lmMsq - 12480*lmMst1 + 25020*lmMst2 - 13980*
        lmMt)*pow2(Mst2) + 160*(53 + 24*lmMst2)*pow2(MuSUSY)) + 3*((310039 +
        21600*lmMsq - 91080*lmMst1 + 123480*lmMst2 - 43920*lmMt)*pow2(Mst2) +
        30*(-353 + 72*lmMst1 + 696*lmMst2)*pow2(MuSUSY))*pow4(Mst1) + 2*(610669
        + 145800*lmMsq - 300240*lmMst1 + 249480*lmMst2 - 78840*lmMt)*pow6(Mst1)
        + 52920*pow6(Mst2)) + 10800*pow2(Msq)*(3*pow2(Mst2)*pow6(Mst1) + 5*
        pow2(Mst1)*pow6(Mst2)) + 37800*pow2(Mst1)*pow8(Mst2))) - 56*s2t*pow2(
        Mst1)*pow3(Mt)*(Dmglst2*Mst2*(pow4(Msq)*(27*pow2(Mst2)*(30*(2029 + 960*
        lmMsq + 248*lmMst1 - 2872*lmMst2 - 448*lmMt)*pow2(Mst2) + (278347 -
        79200*lmMsq + 3720*lmMst1 + 119160*lmMst2)*pow2(MuSUSY)) + 18*pow2(
        Mst1)*(3*(438149 - 26520*lmMst1 + 60600*lmMst2 - 3360*lmMt)*pow2(Mst2)
        + 2*(534391 - 113400*lmMsq - 23760*lmMst1 + 196560*lmMst2)*pow2(MuSUSY)
        ) + 8*(9227767 + 291600*lmMsq - 754920*lmMst1 + 1088640*lmMst2 - 22680*
        lmMt)*pow4(Mst1)) + 129600*pow2(Msq)*(-3*pow2(MuSUSY)*(7*pow2(Mst1)*
        pow2(Mst2) + 10*pow4(Mst1) + 4*pow4(Mst2)) + 14*pow6(Mst2)) + 16200*(-(
        pow2(MuSUSY)*(93*pow2(Mst2)*pow4(Mst1) + 67*pow2(Mst1)*pow4(Mst2) + 41*
        pow6(Mst2))) + 88*pow8(Mst2))) + 3*(-(pow4(Msq)*(6*pow2(Mst1)*pow2(
        Mst2)*((470657 - 86040*lmMst1 + 178200*lmMst2)*pow2(Mst2) - 20*(5827 -
        2700*lmMsq + 2988*lmMst1 + 468*lmMst2)*pow2(MuSUSY)) + 2*(4*(574759 +
        32400*lmMsq - 97200*lmMst1 + 131760*lmMst2)*pow2(Mst2) + (-412663 +
        226800*lmMsq - 396360*lmMst1 + 119880*lmMst2)*pow2(MuSUSY))*pow4(Mst1)
        + 9*(2*(86693 - 14400*lmMsq - 9960*lmMst1 + 52680*lmMst2 + 3360*lmMt)*
        pow2(Mst2) - 15*(3469 - 1440*lmMsq + 664*lmMst1 + 1384*lmMst2)*pow2(
        MuSUSY))*pow4(Mst2))) + 43200*pow2(Msq)*(-(pow2(MuSUSY)*(4*pow2(Mst2)*
        pow4(Mst1) + 3*pow2(Mst1)*pow4(Mst2) + 2*pow6(Mst2))) + 6*pow8(Mst2)) +
        5400*(-(pow2(MuSUSY)*(9*pow4(Mst1)*pow4(Mst2) + 7*pow2(Mst1)*pow6(Mst2)
        + 5*pow8(Mst2))) + 24*power10(Mst2)))))))))/(408240.*Tbeta*pow2(Mst1)*
        pow2(Sbeta)*pow4(Msq)))/pow9(Mst2);
}

/**
 * 	@return The off-diagonal (1, 2) = (2, 1) matrix element of the Higgs mass matrix as a double for the hierarchy 'H6'
 */
double himalaya::H6::getS12() const {
   return Al4p*z2*(Al4p*threeLoopFlag*(MuSUSY*((Mt*s2t*shiftst3*pow2(Mst2)*(-4*
        pow2(Mt) + ((-1 - lmMst1 + lmMst2)*pow2(Mst1) + pow2(Mst2))*pow2(s2t)))
        /(3.*pow2(Mst1)) - pow2(Mt)*pow2(s2t)*((-5*(Mst2*(41*Dmglst2 + 5*Mst2)
        + 16*pow2(Msq) + 2*pow2(Mst1))*pow3(Mst2))/(3.*pow4(Msq)) + ((
        38.91543209876543 - 40*lmMsq + 134*lmMst1 - (274*lmMst2)/3.)*pow4(Mst1)
        )/pow3(Mst2) + ((71.27314814814815 - 40*lmMsq + 83*lmMst1 - (121*
        lmMst2)/3.)*pow2(Mst1) - (40*pow4(Mst1))/(3.*pow2(Msq)))/Mst2 + (Mst2*(
        3469 - 1440*lmMsq + 664*lmMst1 + 1384*lmMst2 - (320*pow2(Mst1))/pow2(
        Msq) - (80*pow4(Mst1))/pow4(Msq)))/24. + Dmglst2*(773.1861111111111 -
        220*lmMsq + (31*lmMst1)/3. + 331*lmMst2 - (120*pow2(Mst1))/pow2(Msq) +
        (pow2(Mst1)*(1206.039814814815 - 200*lmMsq - (295*lmMst1)/3. + 397*
        lmMst2 - (120*pow2(Mst1))/pow2(Msq)))/pow2(Mst2) - (10*(48*pow2(Msq) +
        13*pow2(Mst1))*pow2(Mst2))/(3.*pow4(Msq)) - (130*pow4(Mst1))/(3.*pow4(
        Msq)) + ((2049.0936213991768 - 200*lmMsq - 338*lmMst1 + (1910*lmMst2)/
        3.)*pow4(Mst1))/pow4(Mst2))) - Mt*pow3(s2t)*((149.4746913580247 - (5*
        lmMsq)/3. - (52*lmMst1)/9. + 12*lmMst2)*pow2(Mst1) + ((235 + 450*lmMsq
        + 642*lmMst1 - 1362*lmMst2 - (90*pow2(Mst1))/pow2(Msq))*pow2(Mst2))/54.
         - Dmglst2*(((33563 - 4020*lmMst1 + 4020*lmMst2)*pow2(Mst1))/(270.*
        Mst2) + Mst2*(66.64814814814815 - 20*lmMsq - (76*lmMst1)/3. + (580*
        lmMst2)/9. + (10*pow2(Mst1))/(3.*pow2(Msq))) - (pow3(Mst2)*(170/pow2(
        Msq) - 92/pow2(Mst1) - (25*pow2(Mst1))/pow4(Msq)))/9.) + ((16*Dmglst2*(
        -776573 + 80595*lmMst1 - 80595*lmMst2) + 3*(2205079 - 76140*lmMst1 +
        76140*lmMst2)*Mst2)*pow4(Mst1))/(29160.*pow3(Mst2)) + ((5*Mst2*(64*
        Dmglst2 + 9*Mst2) - pow2(14*pow2(Msq) - 5*pow2(Mst1))/pow2(Mst1))*pow4(
        Mst2))/(36.*pow4(Msq))) + (pow3(Mt)*(3*(502399 + 65160*lmMst1 - 134280*
        lmMst2)*s2t*pow2(Mst1)*pow3(Mst2) + (4828526 + 406080*lmMst1 - 613440*
        lmMst2)*Mst2*s2t*pow4(Mst1) + 2*Mt*(24*Mst2*(3*Dmglst2*(57943 + 1800*
        lmMsq - 2850*lmMst1 + 2190*lmMst2 - 1260*lmMt) + 2*(-45986 + 2700*lmMsq
        + 7245*lmMst1 - 21015*lmMst2 - 630*lmMt)*Mst2)*pow2(Mst1) + 9*(15*
        Dmglst2*(1805 + 960*lmMsq + 248*lmMst1 - 2872*lmMst2 - 448*lmMt) + (-
        88373 + 14400*lmMsq + 9960*lmMst1 - 52680*lmMst2 - 3360*lmMt)*Mst2)*
        pow3(Mst2) - 4*(1126591 - 184140*lmMst1 + 383940*lmMst2 + 7560*lmMt)*
        pow4(Mst1)) + (43200*Mst2*Mt*(14*Dmglst2*pow2(Msq) + 6*Mst2*pow2(Msq) +
        11*Dmglst2*pow2(Mst2) + 3*pow3(Mst2))*(pow2(Mst1)*pow2(Mst2) + pow4(
        Mst1) + pow4(Mst2)))/pow4(Msq) + 9*(-29683 + 13560*lmMst1 - 36600*
        lmMst2)*s2t*pow5(Mst2) - (24*Dmglst2*s2t*(6*(21379 + 1770*lmMst1 +
        12630*lmMst2)*pow2(Mst2)*pow4(Mst1) + 6*(4879 - 270*lmMst1 + 8910*
        lmMst2)*pow2(Mst1)*pow4(Mst2) + (520939 + 44280*lmMst1 + 76680*lmMst2)*
        pow6(Mst1) + 8280*pow6(Mst2)))/pow2(Mst1) - (105840*s2t*pow7(Mst2))/
        pow2(Mst1)))/(4860.*pow5(Mst2))) + (Mt*MuSUSY*(48600*Mst2*s2t*xMsq*
        pow2(Msq)*(Tbeta*pow2(Mst2)*((-1 + shiftst1)*pow2(Mst2)*(-4*pow2(Mt) +
        pow2(Mst2)*pow2(s2t)) + pow2(Mst1)*(4*(-1 + shiftst2)*pow2(Mt) + (-
        shiftst1 + shiftst2)*pow2(Mst2)*pow2(s2t)) - (-1 + shiftst2)*pow2(s2t)*
        pow4(Mst1)) + s2t*(Mt*MuSUSY*(2*(1 - 2*shiftst1 + shiftst2)*pow2(Mst1)*
        pow2(Mst2) - 4*(shiftst1 - shiftst2)*pow4(Mst1) - 2*(-1 + shiftst1)*
        pow4(Mst2)) + (-lmMst1 + lmMst2)*pow2(Mst1)*(4*Mt*MuSUSY*(1 - 2*
        shiftst1 + shiftst2)*pow2(Mst1) - 4*Mt*MuSUSY*(-1 + shiftst1)*pow2(
        Mst2) + s2t*(-2 + shiftst1 + shiftst2)*Tbeta*pow4(Mst2)))) + 6480*
        xDR2DRMOD*(2*pow2(Mst1)*pow2(Mst2)*(Dmglst2*Mst2*(2*s2t*(16*(1 + 3*
        lmMst2)*MuSUSY - 23*Mst2*Tbeta)*pow2(Mt) + Mst2*Mt*((23 - 46*lmMst1 +
        30*lmMst2)*MuSUSY - 24*(1 + 3*lmMst2)*Mst2*Tbeta)*pow2(s2t) + 32*(-1 +
        lmMst2)*Tbeta*pow3(Mt) + (23*lmMst1 - 15*lmMst2)*Tbeta*pow3(Mst2)*pow3(
        s2t)) + pow2(Mst2)*(2*s2t*(16*(1 + lmMst2)*MuSUSY - 13*Mst2*Tbeta)*
        pow2(Mt) + Mst2*Mt*((5 - 26*lmMst1 + 18*lmMst2)*MuSUSY - 24*(1 +
        lmMst2)*Mst2*Tbeta)*pow2(s2t) + 32*(1 + lmMst2)*Tbeta*pow3(Mt) + (4 +
        13*lmMst1 - 9*lmMst2)*Tbeta*pow3(Mst2)*pow3(s2t)) - 2*xDmglst2*pow2(
        Dmglst2)*(2*s2t*(-8*(-2 + 3*lmMst2)*MuSUSY + 5*Mst2*Tbeta)*pow2(Mt) +
        Mst2*Mt*((-13 + 10*lmMst1 - 6*lmMst2)*MuSUSY + 12*(-2 + 3*lmMst2)*Mst2*
        Tbeta)*pow2(s2t) + 16*Tbeta*pow3(Mt) + (4 - 5*lmMst1 + 3*lmMst2)*Tbeta*
        pow3(Mst2)*pow3(s2t))) + (-2*xDmglst2*pow2(Dmglst2)*(-64*(-2 + 3*
        lmMst2)*MuSUSY*s2t*pow2(Mt) + 4*Mst2*Mt*((-4 + 5*lmMst1 - 3*lmMst2)*
        MuSUSY + 6*(-2 + 3*lmMst2)*Mst2*Tbeta)*pow2(s2t) - 32*(7 + 2*lmMst2)*
        Tbeta*pow3(Mt) - 5*Tbeta*pow3(Mst2)*pow3(s2t)) + pow2(Mst2)*(64*(1 +
        lmMst2)*(2*MuSUSY*s2t + 3*Mt*Tbeta)*pow2(Mt) - 4*Mst2*Mt*((4 + 13*
        lmMst1 - 9*lmMst2)*MuSUSY + 12*(1 + lmMst2)*Mst2*Tbeta)*pow2(s2t) + 13*
        Tbeta*pow3(Mst2)*pow3(s2t)) + Dmglst2*Mst2*(128*(1 + 3*lmMst2)*MuSUSY*
        s2t*pow2(Mt) - 4*Mst2*Mt*(23*lmMst1*MuSUSY - 15*lmMst2*MuSUSY + 12*
        Mst2*Tbeta + 36*lmMst2*Mst2*Tbeta)*pow2(s2t) - 64*(7 + lmMst2)*Tbeta*
        pow3(Mt) + 23*Tbeta*pow3(Mst2)*pow3(s2t)))*pow4(Mst1) + s2t*(23*
        Dmglst2*Mst2 + 10*xDmglst2*pow2(Dmglst2) + 13*pow2(Mst2))*(2*Mt*MuSUSY*
        s2t + 4*Tbeta*pow2(Mt) - Tbeta*pow2(Mst2)*pow2(s2t))*pow5(Mst2) + 4*Mt*
        (48*(1 + lmMst2)*Mt*(MuSUSY*s2t + 2*Mt*Tbeta) - (Dmglst2*(23*lmMst1 -
        15*lmMst2)*MuSUSY + (4 + 13*lmMst1 - 9*lmMst2)*Mst2*MuSUSY + 12*
        Dmglst2*(1 + 3*lmMst2)*Mst2*Tbeta + 12*(1 + lmMst2)*Tbeta*pow2(Mst2))*
        pow2(s2t))*pow6(Mst1)) - (Mt*MuSUSY*(4050*s2t*pow2(Mst1)*pow2(Mst2)*((
        72*Mt - 4*Mst2*s2t)*pow2(Mst2)*pow4(Mst1) - 4*(-14*Mt + Mst2*s2t)*pow2(
        Mst1)*pow4(Mst2) + 4*pow2(Msq)*(-4*(-12*Mt + Mst2*s2t)*pow2(Mst1)*pow2(
        Mst2) + (64*Mt - 4*Mst2*s2t)*pow4(Mst1) + (32*Mt - 7*Mst2*s2t)*pow4(
        Mst2)) + (40*Mt - 9*Mst2*s2t)*pow6(Mst2)) + 16200*Dmglst2*Mst2*s2t*
        pow2(Mst1)*((186*Mt - 11*Mst2*s2t)*pow2(Mst2)*pow4(Mst1) + (134*Mt -
        11*Mst2*s2t)*pow2(Mst1)*pow4(Mst2) + 2*pow2(Msq)*(-14*(-12*Mt + Mst2*
        s2t)*pow2(Mst1)*pow2(Mst2) + 2*(120*Mt - 7*Mst2*s2t)*pow4(Mst1) + (96*
        Mt - 17*Mst2*s2t)*pow4(Mst2)) + 2*(41*Mt - 8*Mst2*s2t)*pow6(Mst2)) + 2*
        Dmglst2*pow4(Msq)*(-27*pow2(Mst1)*pow2(Mst2)*((278347 - 79200*lmMsq +
        3720*lmMst1 + 119160*lmMst2)*Mst2*Mt*s2t + 7680*(5 + 6*lmMst2)*pow2(Mt)
        + 10*(-4151 + 1080*lmMsq + 1368*lmMst1 - 3480*lmMst2)*pow2(Mst2)*pow2(
        s2t)) - 36*((534391 - 113400*lmMsq - 23760*lmMst1 + 196560*lmMst2)*
        Mst2*Mt*s2t + 30*(3643 - 120*lmMst1 + 3192*lmMst2)*pow2(Mt) + 9*(-9053
        + 900*lmMsq + 1810*lmMst1 - 3570*lmMst2)*pow2(Mst2)*pow2(s2t))*pow4(
        Mst1) + pow2(s2t)*(4*(2286439 - 72900*lmMsq - 307800*lmMst1 + 450360*
        lmMst2)*pow6(Mst1) + 149040*pow6(Mst2))) + 3*pow4(Msq)*(-90*pow2(Mst1)*
        (3*(3469 - 1440*lmMsq + 664*lmMst1 + 1384*lmMst2)*Mst2*Mt*s2t + 64*(53
        + 24*lmMst2)*pow2(Mt) - 2*(59 - 450*lmMsq + 1362*lmMst2 + 36*(1 +
        lmMst2)*shiftst3 - 6*lmMst1*(107 + 6*shiftst3))*pow2(Mst2)*pow2(s2t))*
        pow3(Mst2) + 6*Mst2*(-40*(5827 - 2700*lmMsq + 2988*lmMst1 + 468*lmMst2)
        *Mst2*Mt*s2t - 60*(-353 + 72*lmMst1 + 696*lmMst2)*pow2(Mt) - (240379 +
        10800*lmMsq - 21420*lmMst2 - 1080*(1 + 2*lmMst2)*shiftst3 + 180*lmMst1*
        (55 + 12*shiftst3))*pow2(Mst2)*pow2(s2t))*pow4(Mst1) - s2t*(4*(412663 -
        226800*lmMsq + 396360*lmMst1 - 119880*lmMst2)*Mt + Mst2*s2t*(3647353 +
        64800*lmMsq - 52380*lmMst2 - 6480*(1 + 3*lmMst2)*shiftst3 + 540*lmMst1*
        (-31 + 36*shiftst3)))*pow6(Mst1) + 1080*(49 + 3*shiftst3)*pow2(s2t)*
        pow7(Mst2))))/pow4(Msq)))/(14580.*Tbeta*pow2(Mst1)*pow5(Mst2))) - (Mt*
        MuSUSY*((xDmglst2*pow2(Dmglst2)*(-22680*twoLoopFlag*(-2*Mt*pow2(Mst1)*
        pow2(Mst2)*(-24*Mt*MuSUSY*s2t + 4*Tbeta*pow2(Mt) + Mst2*(MuSUSY + 9*
        Mst2*Tbeta)*pow2(s2t)) - 2*Mt*(-36*Mt*MuSUSY*s2t + 28*Tbeta*pow2(Mt) +
        Mst2*(MuSUSY + 9*Mst2*Tbeta)*pow2(s2t))*pow4(Mst1) + s2t*(-2*Mst2*Mt*
        s2t*(MuSUSY + 9*Mst2*Tbeta) + 24*MuSUSY*pow2(Mt) + Tbeta*pow2(s2t)*
        pow3(Mst2))*pow4(Mst2)) + Al4p*threeLoopFlag*pow2(Mst2)*(pow2(Mst1)*(4*
        s2t*(35*(-9883 + 155520*lmMsq + 41688*lmMst1 - 290520*lmMst2)*MuSUSY +
        36*(221716 - 20265*lmMst1 + 231945*lmMst2)*Mst2*Tbeta)*pow2(Mt) - 7*
        Mst2*Mt*(4*(249097 + 24300*lmMsq + 30510*lmMst1 - 78030*lmMst2)*MuSUSY
        + 3*(708829 + 388800*lmMsq + 198720*lmMst1 - 820800*lmMst2)*Mst2*Tbeta)
        *pow2(s2t) - 16*(-5973901 + 192780*lmMst1 - 582120*lmMst2 + 147420*
        lmMt)*Tbeta*pow3(Mt) + 7*(472397 + 24300*lmMst1 - 24300*lmMst2)*Tbeta*
        pow3(Mst2)*pow3(s2t)) + (56700*pow2(Mst2)*(-(pow2(Mst1)*(-544*MuSUSY*
        s2t*pow2(Mt) + 12*Mst2*Mt*(3*MuSUSY + 16*Mst2*Tbeta)*pow2(s2t) + 64*
        Tbeta*pow3(Mt) + Tbeta*pow3(Mst2)*pow3(s2t))) + pow2(Mst2)*(288*MuSUSY*
        s2t*pow2(Mt) - 2*Mst2*Mt*(19*MuSUSY + 108*Mst2*Tbeta)*pow2(s2t) - 64*
        Tbeta*pow3(Mt) + 19*Tbeta*pow3(Mst2)*pow3(s2t))))/pow2(Msq) + (28350*(-
        (pow2(Mst1)*(-656*MuSUSY*s2t*pow2(Mt) + 2*Mst2*Mt*(25*MuSUSY + 108*
        Mst2*Tbeta)*pow2(s2t) + 192*Tbeta*pow3(Mt) + 5*Tbeta*pow3(Mst2)*pow3(
        s2t))) + 2*pow2(Mst2)*(184*MuSUSY*s2t*pow2(Mt) - 6*Mst2*Mt*(5*MuSUSY +
        23*Mst2*Tbeta)*pow2(s2t) - 96*Tbeta*pow3(Mt) + 15*Tbeta*pow3(Mst2)*
        pow3(s2t)))*pow4(Mst2))/pow4(Msq) + 3*Mst2*(4*Mst2*s2t*(28*(-63187 +
        32400*lmMsq + 810*lmMst1 - 52650*lmMst2)*MuSUSY + 3*(85649 + 2100*
        lmMst1 + 441420*lmMst2)*Mst2*Tbeta)*pow2(Mt) - 14*Mt*((8599 + 16200*
        lmMsq + 12240*lmMst1 - 43920*lmMst2)*MuSUSY + 6*(-63187 + 32400*lmMsq +
        810*lmMst1 - 52650*lmMst2)*Mst2*Tbeta)*pow2(Mst2)*pow2(s2t) - 32*(84*(
        17 + 1320*lmMst2)*MuSUSY + (9356 - 2835*lmMst1 - 3150*lmMst2 + 5985*
        lmMt)*Mst2*Tbeta)*pow3(Mt) + 7*(17959 + 16200*lmMsq + 12240*lmMst1 -
        43920*lmMst2)*Tbeta*pow3(s2t)*pow4(Mst2)) - (196560*s2t*(-2*Mt*MuSUSY*
        s2t - 4*Tbeta*pow2(Mt) + Tbeta*pow2(Mst2)*pow2(s2t))*pow5(Mst2))/pow2(
        Mst1))))/(34020.*pow7(Mst2)) + (twoLoopFlag*(8*(-(MuSUSY*s2t) + 6*Mt*
        Tbeta)*pow2(Mt)*pow4(Mst1)*pow4(Mst2) + 8*(-(MuSUSY*s2t) + 8*Mt*Tbeta)*
        xMst*pow2(Mst2)*pow2(Mt)*pow6(Mst1) + 4*Mt*xDmglst2*xMst*pow2(Dmglst2)*
        (-48*Mt*MuSUSY*s2t + 88*Tbeta*pow2(Mt) + Mst2*(MuSUSY + 9*Mst2*Tbeta)*
        pow2(s2t))*pow6(Mst1) + pow2(Mst1)*(-8*MuSUSY*s2t*pow2(Mt) + 32*Tbeta*
        pow3(Mt) + Tbeta*pow3(Mst2)*pow3(s2t))*pow6(Mst2) + 2*Dmglst2*(4*Mt*(-
        13*Mt*MuSUSY*s2t - 6*Tbeta*pow2(Mt) + Mst2*(MuSUSY + 3*Mst2*Tbeta)*
        pow2(s2t))*pow3(Mst2)*pow4(Mst1) + 4*Mt*s2t*(-9*Mt*MuSUSY + Mst2*s2t*(
        MuSUSY + 3*Mst2*Tbeta))*pow2(Mst1)*pow5(Mst2) + 4*Mst2*Mt*xMst*(-17*Mt*
        MuSUSY*s2t - 16*Tbeta*pow2(Mt) + Mst2*(MuSUSY + 3*Mst2*Tbeta)*pow2(s2t)
        )*pow6(Mst1) + (-20*MuSUSY*s2t*pow2(Mt) + Mst2*Mt*(4*MuSUSY + 15*Mst2*
        Tbeta)*pow2(s2t) + 8*Tbeta*pow3(Mt) - 2*Tbeta*pow3(Mst2)*pow3(s2t))*
        pow7(Mst2)) + (-8*MuSUSY*s2t*pow2(Mt) + 2*Mst2*Mt*(MuSUSY + 3*Mst2*
        Tbeta)*pow2(s2t) + 16*Tbeta*pow3(Mt) - Tbeta*pow3(Mst2)*pow3(s2t))*
        pow8(Mst2)))/(3.*pow9(Mst2))))/Tbeta) + (Mt*xMst*(27*(lmMst1 - lmMst2)*
        Mt*oneLoopFlag*pow2(MuSUSY)*pow2(s2t)*pow3(Mst2) - Al4p*twoLoopFlag*(2*
        xDmglst2*pow2(Dmglst2)*(-(Mst2*Mt*pow2(s2t)*(18*(85 - 36*lmMst1 + 36*
        lmMst2)*Mst2*MuSUSY*Tbeta + (314 + 24*lmMst2*(29 - 6*xDR2DRMOD) - 24*
        lmMst1*(29 + 3*lmMst2*(-1 + xDR2DRMOD) - 6*xDR2DRMOD) + 72*(-1 +
        xDR2DRMOD)*pow2(lmMst2))*pow2(MuSUSY) + 15*(-43 + 60*lmMst1 - 60*
        lmMst2)*pow2(Mst2)*(-1 + pow2(Sbeta))*pow2(Sbeta))) + MuSUSY*(6*s2t*
        pow2(Mt)*(8*(143 - 18*lmMst1 + 18*lmMst2)*MuSUSY + Mst2*Tbeta*(371 +
        lmMst2*(552 - 600*pow2(Sbeta)) - 430*pow2(Sbeta) + 24*lmMst1*(-23 + 25*
        pow2(Sbeta)))) + Tbeta*(8*(398 + lmMst2*(2085 - 396*lmMt) - 18*lmMst1*(
        95 + 22*lmMst2 - 22*lmMt) - 375*lmMt + 396*pow2(lmMst2))*pow3(Mt) + 4*(
        1 - 24*lmMst1 + 24*lmMst2)*pow3(Mst2)*pow3(s2t)))) + Mst2*MuSUSY*(8*
        Dmglst2*(s2t*(36*(-1 + 3*lmMst1 - 3*lmMst2)*Mst2*Tbeta + MuSUSY*(785 +
        6*lmMst1*(85 - 42*lmMst2) - 438*lmMst2 + 252*pow2(lmMst2)))*pow2(Mt) -
        Mst2*Mt*(3*Mst2*Tbeta*(50 + lmMst1*(51 - 18*lmMst2) - 51*lmMst2 + 18*
        pow2(lmMst2)) + MuSUSY*(49 - 84*lmMst2 + lmMst1*(84 - 36*lmMst2*(-1 +
        xDR2DRMOD)) + 36*(-1 + xDR2DRMOD)*pow2(lmMst2)))*pow2(s2t) + Tbeta*(4*(
        7 - 381*lmMst2 + lmMst1*(327 + 72*lmMst2 - 81*lmMt) + 54*lmMt + 81*
        lmMst2*lmMt - 72*pow2(lmMst2))*pow3(Mt) + 2*(1 + 3*lmMst1 - 3*lmMst2)*
        pow3(Mst2)*pow3(s2t))) - Mst2*(-8*s2t*(18*Mst2*Tbeta*(-lmMst1 + lmMst2
        - 2*lmMst1*lmMst2 + pow2(lmMst1) + pow2(lmMst2)) + MuSUSY*(193 + 474*
        lmMst2 - 6*lmMst1*(67 + 42*lmMst2) + 252*pow2(lmMst2)))*pow2(Mt) + 8*
        Mst2*Mt*(3*Mst2*Tbeta*(1 + 33*lmMst2 - 3*lmMst1*(11 + 6*lmMst2) + 18*
        pow2(lmMst2)) + MuSUSY*(1 + 3*lmMst2*(-37 + 6*xDR2DRMOD) - 3*lmMst1*(-
        37 + 6*lmMst2*(-12 + xDR2DRMOD) + 6*xDR2DRMOD) - 81*pow2(lmMst1) + 9*(-
        15 + 2*xDR2DRMOD)*pow2(lmMst2)))*pow2(s2t) + Tbeta*(32*(83 + 3*lmMst1*(
        29 + 12*lmMst2 - 9*lmMt) + 9*lmMt + 3*lmMst2*(-32 + 9*lmMt) - 36*pow2(
        lmMst2))*pow3(Mt) + (5 + lmMst1*(6 - 288*lmMst2) - 6*lmMst2 + 144*(
        pow2(lmMst1) + pow2(lmMst2)))*pow3(Mst2)*pow3(s2t))))))*pow6(Mst1))/(
        108.*Tbeta*pow9(Mst2)) + MuSUSY*((Mt*oneLoopFlag*s2t*(4*(lmMst1 -
        lmMst2)*pow2(Mt) - ((2 + lmMst1 - lmMst2)*pow2(Mst1) + (-2 + lmMst1 -
        lmMst2)*pow2(Mst2))*pow2(s2t) - (2*Mt*MuSUSY*s2t*(2 - lmMst1 + lmMst2 -
        (2*(lmMst1 - lmMst2)*pow2(Mst1)*(pow2(Mst1) + pow2(Mst2)))/pow4(Mst2)))
        /Tbeta))/16. - (Mt*threeLoopFlag*pow2(Al4p)*((24300*(1 - 2*lmMsq)*s2t*
        xMsq*pow2(Msq)*(Tbeta*pow2(Mst2)*((-1 + shiftst1)*pow2(Mst2)*(-4*pow2(
        Mt) + pow2(Mst2)*pow2(s2t)) + pow2(Mst1)*(4*(-1 + shiftst2)*pow2(Mt) +
        (-shiftst1 + shiftst2)*pow2(Mst2)*pow2(s2t)) - (-1 + shiftst2)*pow2(
        s2t)*pow4(Mst1)) + s2t*(Mt*MuSUSY*(2*(1 - 2*shiftst1 + shiftst2)*pow2(
        Mst1)*pow2(Mst2) - 4*(shiftst1 - shiftst2)*pow4(Mst1) - 2*(-1 +
        shiftst1)*pow4(Mst2)) + (-lmMst1 + lmMst2)*pow2(Mst1)*(4*Mt*MuSUSY*(1 -
        2*shiftst1 + shiftst2)*pow2(Mst1) - 4*Mt*MuSUSY*(-1 + shiftst1)*pow2(
        Mst2) + s2t*(-2 + shiftst1 + shiftst2)*Tbeta*pow4(Mst2)))))/(pow2(Mst1)
        *pow4(Mst2)) + (5*(Dmglst2*(2*s2t*(72*Tbeta*pow2(Mt)*(2806*z3 + 1514*z4
        + 2757*pow2(z2)) - 4*Tbeta*pow2(Mst2)*pow2(s2t)*(89947*z3 + 6332*z4 +
        9498*pow2(z2)) + Mt*s2t*(8*MuSUSY*(338536*z3 + 11327*z4 + 15897*pow2(
        z2)) + 3*Mst2*Tbeta*(-844756*z3 + 10126*z4 + 105585*pow2(z2))))*pow4(
        Mst1) - 9*pow2(Mst1)*(-16*Mst2*s2t*pow2(Mt)*(MuSUSY*(59336*z3 + 709*z4
        - 8535*pow2(z2)) + 6*Mst2*Tbeta*(590*z3 - 4*z4 + 75*pow2(z2))) - 3*Mt*
        pow2(Mst2)*pow2(s2t)*(16*MuSUSY*(9207*z3 + 185*z4 + 237*pow2(z2)) +
        Mst2*Tbeta*(-124922*z3 - 1210*z4 + 18273*pow2(z2))) + 96*(MuSUSY*(250*
        z3 - 94*z4 + 21*pow2(z2)) + 2*Mst2*Tbeta*(-955*z3 + 26*z4 + 93*pow2(z2)
        ))*pow3(Mt) + 6*Tbeta*(20653*z3 + 316*z4 + 474*pow2(z2))*pow3(s2t)*
        pow4(Mst2)) - 27*pow2(Mst2)*(-4*Mt*s2t*pow2(Mst2)*(4*Mt*Tbeta*(439*z3 -
        108*z4) + MuSUSY*s2t*(16175*z3 + 424*z4 + 474*pow2(z2))) + 4*Mst2*pow2(
        Mt)*(Mt*Tbeta*(5742*z3 - 506*z4 + 105*pow2(z2)) + MuSUSY*s2t*(-37474*z3
        - 542*z4 + 5289*pow2(z2))) - 3*Mt*Tbeta*pow2(s2t)*(-37474*z3 - 542*z4 +
        5289*pow2(z2))*pow3(Mst2) + 64*MuSUSY*(83*z3 - 27*z4)*pow3(Mt) + 2*
        Tbeta*(16175*z3 + 424*z4 + 474*pow2(z2))*pow3(s2t)*pow4(Mst2))) + 3*((
        4*s2t*pow2(Mt)*(MuSUSY*(134464*z3 + 23528*z4 - 41010*pow2(z2)) + Mst2*
        Tbeta*(22466*z3 - 11402*z4 - 17103*pow2(z2))) + 2*Mst2*Mt*pow2(s2t)*(
        MuSUSY*(8*(76813 + 162*lmMst1 - 162*lmMst2)*z3 - 15686*z4 - 18183*pow2(
        z2)) + 3*Mst2*Tbeta*(1144*z3 - 4954*z4 + 6177*pow2(z2))) + 16*Tbeta*(-
        107072*z3 + 3326*z4 + 3045*pow2(z2))*pow3(Mt) + Tbeta*(-272636*z3 +
        8870*z4 + 13305*pow2(z2))*pow3(Mst2)*pow3(s2t))*pow4(Mst1) - 9*pow3(
        Mst2)*(-4*Mst2*pow2(Mt)*(5*MuSUSY*s2t*(2098*z3 + 102*z4 - 333*pow2(z2))
        + 3*Mt*Tbeta*(-1922*z3 + 206*z4 + 21*pow2(z2))) + 6*Mt*s2t*pow2(Mst2)*(
        -2*MuSUSY*s2t*((1913 + 12*lmMst1 - 12*lmMst2)*z3 - 10*z4 + 48*pow2(z2))
        + Mt*Tbeta*((1622 - 48*lmMst1 + 48*lmMst2)*z3 + 142*z4 + 69*pow2(z2)))
        + 15*Mt*Tbeta*pow2(s2t)*(2098*z3 + 102*z4 - 333*pow2(z2))*pow3(Mst2) -
        32*MuSUSY*(230*z3 + 27*z4)*pow3(Mt) + 6*Tbeta*((1913 + 12*lmMst1 - 12*
        lmMst2)*z3 - 10*z4 + 48*pow2(z2))*pow3(s2t)*pow4(Mst2)) + 3*Mst2*pow2(
        Mst1)*(-2*Mst2*s2t*pow2(Mt)*(8*MuSUSY*(-11396*z3 - 1135*z4 + 2388*pow2(
        z2)) + Mst2*Tbeta*(7078*z3 + 2990*z4 + 4485*pow2(z2))) + Mt*pow2(Mst2)*
        pow2(s2t)*(-4*MuSUSY*((-56978 - 216*lmMst1 + 216*lmMst2)*z3 + 1136*z4 +
        813*pow2(z2)) + 3*Mst2*Tbeta*(-14114*z3 - 3010*z4 + 4557*pow2(z2))) +
        16*(9*MuSUSY*(218*z3 + 50*z4 + 21*pow2(z2)) + 2*Mst2*Tbeta*(-8219*z3 +
        326*z4 + 165*pow2(z2)))*pow3(Mt) + 2*Tbeta*((-39761 - 108*lmMst1 + 108*
        lmMst2)*z3 + 1046*z4 + 1245*pow2(z2))*pow3(s2t)*pow4(Mst2))) + 3*
        xDmglst2*pow2(Dmglst2)*(pow2(Mst1)*(8*s2t*pow2(Mt)*(MuSUSY*(343276*z3 +
        8090*z4 - 37437*pow2(z2)) + 36*Mst2*Tbeta*(2534*z3 + 33*z4 + 90*pow2(
        z2))) + 2*Mst2*Mt*pow2(s2t)*(MuSUSY*(974888*z3 + 29956*z4 - 3180*pow2(
        z2)) + 3*Mst2*Tbeta*(-127198*z3 - 6782*z4 + 14613*pow2(z2))) + 8*Tbeta*
        (-186761*z3 + 8792*z4 + 13188*pow2(z2))*pow3(Mt) + Tbeta*(-586697*z3 -
        10924*z4 + 6942*pow2(z2))*pow3(Mst2)*pow3(s2t)) - 3*Mst2*(-8*Mst2*pow2(
        Mt)*(MuSUSY*s2t*(72026*z3 + 436*z4 - 7608*pow2(z2)) + Mt*Tbeta*(-19363*
        z3 + 112*z4 + 168*pow2(z2))) - 2*Mt*s2t*pow2(Mst2)*(3*Mt*Tbeta*(55597*
        z3 - 264*z4 + 252*pow2(z2)) + MuSUSY*s2t*(129397*z3 + 6344*z4 + 1254*
        pow2(z2))) + 12*Mt*Tbeta*pow2(s2t)*(36013*z3 + 218*z4 - 3804*pow2(z2))*
        pow3(Mst2) + 288*MuSUSY*(608*z3 - 9*z4)*pow3(Mt) + Tbeta*(129397*z3 +
        6344*z4 + 1254*pow2(z2))*pow3(s2t)*pow4(Mst2)))))/pow5(Mst2) + Mt*
        MuSUSY*(-(Mt*s2t*(90*Mst2*(48*((5*(71 + 12*lmMsq*(-2 + lmMst1 - lmMst2)
        + 40*lmMst2 - 4*lmMst1*(4 + 3*lmMst2) + 12*pow2(lmMst2)))/pow2(Msq) - (
        48*pow2(1 + lmMst2))/pow2(Mst1)) + (5*(Mst2*(Mst2*(911 + 12*lmMsq*(-37
        + 18*lmMst1 - 18*lmMst2) + 606*lmMst2 - 54*lmMst1*(3 + 4*lmMst2) + 216*
        pow2(lmMst2)) + Dmglst2*(5591 + 12*lmMsq*(-181 + 90*lmMst1 - 90*lmMst2)
        + 2550*lmMst2 - 54*lmMst1*(7 + 20*lmMst2) + 1080*pow2(lmMst2))) + (1153
        + 12*lmMsq*(-35 + 54*lmMst1 - 54*lmMst2) + 906*lmMst2 - 162*lmMst1*(3 +
        4*lmMst2) + 648*pow2(lmMst2))*pow2(Mst1)))/pow4(Msq)) - (60*(2*pow2(
        Msq)*pow2(Mst1)*(125789 + 13716*B4 - 54*DN - 29160*lmMsq + 3240*pow2(
        lmMsq) - 108*lmMst1*(422 - 45*lmMsq + 45*pow2(lmMsq)) - 2376*pow2(
        lmMst1) + 108*lmMst2*(1096 - 551*lmMst1 + 15*lmMsq*(-7 + 6*lmMst1) +
        45*pow2(lmMsq) + 8*pow2(lmMst1)) - 108*(-717 + 90*lmMsq + 248*lmMst1)*
        pow2(lmMst2) + 25920*pow3(lmMst2)) - 720*(55 + 6*lmMsq*(-2 + 5*lmMst1 -
        5*lmMst2) + 52*lmMst2 - 10*lmMst1*(4 + 3*lmMst2) + 30*pow2(lmMst2))*
        pow4(Mst1)))/(pow2(Msq)*pow3(Mst2)) - (135*(-640*(23 + lmMsq*(-6 + 9*
        lmMst1 - 9*lmMst2) + 18*lmMst2 - 3*lmMst1*(4 + 3*lmMst2) + 9*pow2(
        lmMst2))*pow2(Msq)*pow2(Mst1) + 3*(18201 + 1760*B4 - 16*DN - 5760*lmMsq
        + 960*pow2(lmMsq) - 16*lmMst1*(199 - 30*lmMsq + 30*pow2(lmMsq)) + 16*
        lmMst2*(1291 - 322*lmMst1 + 30*lmMsq*(-5 + 2*lmMst1) + 30*pow2(lmMsq) -
        16*pow2(lmMst1)) - 256*pow2(lmMst1) - 32*(-313 + 30*lmMsq + 72*lmMst1)*
        pow2(lmMst2) + 2560*pow3(lmMst2))*pow4(Msq) - 20*(233 + 36*lmMsq*(-2 +
        5*lmMst1 - 5*lmMst2) + 207*lmMst2 - 45*lmMst1*(3 + 4*lmMst2) + 180*
        pow2(lmMst2))*pow4(Mst1)))/(Mst2*pow4(Msq)) + 14580*Dmglst2*((40*(95 +
        12*lmMsq*(-2 + lmMst1 - lmMst2) + 32*lmMst2 - 4*lmMst1*(2 + 3*lmMst2) +
        12*pow2(lmMst2)))/(9.*pow2(Msq)) - (128*(-1 + 2*lmMst2 + 3*pow2(lmMst2)
        ))/(9.*pow2(Mst1)) - (481.4302469135802 + (1048*B4)/9. - (20*DN)/9. -
        400*lmMsq + (80*pow2(lmMsq))/3. + (4*(lmMst1*(173 - 90*lmMsq - 30*pow2(
        lmMsq)) + lmMst2*(1623 - 130*lmMst1 + 30*lmMsq*(-1 + 2*lmMst1) + 30*
        pow2(lmMsq) - 16*pow2(lmMst1))))/9. + (64*pow2(lmMst1))/9. - (8*(-265 +
        30*lmMsq + 136*lmMst1)*pow2(lmMst2))/9. - (80*(77 + 6*lmMsq*(-2 + 3*
        lmMst1 - 3*lmMst2) + 24*lmMst2 - 6*lmMst1*(2 + 3*lmMst2) + 18*pow2(
        lmMst2))*pow2(Mst1))/(9.*pow2(Msq)) + 128*pow3(lmMst2) - (5*(1961 +
        180*lmMsq*(-2 + 5*lmMst1 - 5*lmMst2) + 675*lmMst2 - 45*lmMst1*(7 + 20*
        lmMst2) + 900*pow2(lmMst2))*pow4(Mst1))/(27.*pow4(Msq)))/pow2(Mst2) +
        pow2(Mst1)*((268.91975308641975 - (1790*lmMsq)/27. + 5*(-7 + 20*lmMsq)*
        lmMst1 + (101.29629629629629 - 100*(lmMsq + lmMst1))*lmMst2 + 100*pow2(
        lmMst2))/pow4(Msq) - (881.6139917695473 + 248*B4 - 4*DN - (2560*lmMsq)/
        3. + lmMst1*(130.96296296296296 - 120*lmMsq - 40*pow2(lmMsq)) + (80*
        pow2(lmMsq))/3. + (176*pow2(lmMst1))/9. + (8*lmMst2*(4364 - 573*lmMst1
        + 45*lmMsq*(5 + 6*lmMst1) + 135*pow2(lmMsq) + 24*pow2(lmMst1)))/27. - (
        8*(-377 + 90*lmMsq + 376*lmMst1)*pow2(lmMst2))/9. - (80*(107 + 6*lmMsq*
        (-2 + 5*lmMst1 - 5*lmMst2) + 32*lmMst2 - 10*lmMst1*(2 + 3*lmMst2) + 30*
        pow2(lmMst2))*pow2(Mst1))/(9.*pow2(Msq)) + (2944*pow3(lmMst2))/9.)/
        pow4(Mst2))) + (2*(-5*(2552929 + 257904*B4 - 648*DN - 456840*lmMsq +
        38880*pow2(lmMsq) - 216*lmMst1*(4591 - 360*lmMsq + 450*pow2(lmMsq)) +
        41904*pow2(lmMst1) + 216*lmMst2*(9211 - 6466*lmMst1 + 180*lmMsq*(-4 +
        5*lmMst1) + 450*pow2(lmMsq) + 576*pow2(lmMst1)) - 864*(-1784 + 225*
        lmMsq + 840*lmMst1)*pow2(lmMst2) - 3456*pow3(lmMst1) + 604800*pow3(
        lmMst2))*pow4(Mst1) + 80*OepS2*(12*Mst2*(-253*Dmglst2 + 68*Mst2)*pow2(
        Mst1) + 63*(-5*Dmglst2 + 3*Mst2)*pow3(Mst2) + 1882*pow4(Mst1)) - 54*S2*
        (-12*Mst2*(Dmglst2*(4978 + 7590*lmMst1 - 7590*lmMst2) - 15*(169 + 136*
        lmMst1 - 136*lmMst2)*Mst2)*pow2(Mst1) + 27*(Dmglst2*(453 - 350*lmMst1 +
        350*lmMst2) + 15*(-1 + 14*lmMst1 - 14*lmMst2)*Mst2)*pow3(Mst2) + 10*(
        9185 + 5646*lmMst1 - 5646*lmMst2)*pow4(Mst1))))/pow5(Mst2))) + (180*
        pow2(Mt)*(144*Dmglst2*pow2(Mst2)*(180 - 2*B4 + 2*D3 - DN + 144*lmMst2 -
        216*S2 - 16*(-2 + lmMst1)*pow2(lmMst2) + 16*(lmMst1 + pow3(lmMst2))) +
        pow2(Mst1)*(3*Mst2*(10667 - 96*B4 + 96*D3 - 48*DN - 3072*lmMst1 - 384*(
        -25 + 9*lmMst1)*lmMst2 - 384*lmMst1*lmMt + 384*(1 + lmMst2)*lmMt - 224*
        OepS2 + 324*(-43 + 14*lmMst1 - 14*lmMst2)*S2 - 384*(-13 + 4*lmMst1)*
        pow2(lmMst2) + 1536*pow3(lmMst2)) + 2*Dmglst2*(28405 - 288*B4 + 288*D3
        - 144*DN + 5472*lmMst1 - 288*(-89 + 2*lmMst1)*lmMst2 + 576*(-2 + lmMst1
        - lmMst2)*lmMt + 224*OepS2 - 324*(65 + 14*lmMst1 - 14*lmMst2)*S2 - 576*
        (-9 + 8*lmMst1)*pow2(lmMst2) + 4608*pow3(lmMst2))) + 24*(436 - 6*B4 +
        6*D3 - 3*DN - 48*lmMst1 + (408 - 96*lmMst1)*lmMst2 + 24*lmMt - 972*S2 -
        48*(-4 + lmMst1)*pow2(lmMst2) + 48*pow3(lmMst2))*pow3(Mst2)) + 40*T1ep*
        (18*Mst2*pow2(Mst1)*(272*Mst2*Mt*s2t + 252*pow2(Mt) - 487*pow2(Mst2)*
        pow2(s2t)) - 72*Dmglst2*pow2(Mst1)*(253*Mst2*Mt*s2t + 42*pow2(Mt) -
        129*pow2(Mst2)*pow2(s2t)) + 378*Dmglst2*s2t*(-5*Mt + 2*Mst2*s2t)*pow3(
        Mst2) + 3*s2t*(3764*Mt - 7357*Mst2*s2t)*pow4(Mst1) + 34616*Dmglst2*
        pow2(s2t)*pow4(Mst1) + 162*s2t*(7*Mt - 8*Mst2*s2t)*pow4(Mst2)))/pow5(
        Mst2) + 90*pow2(s2t)*(15829 - 720*B4 + 72*D3 - 36*DN - 3330*lmMsq +
        1350*pow2(lmMsq) - 3*lmMst1*(5279 - 1950*lmMsq + 630*pow2(lmMsq)) -
        954*pow2(lmMst1) + 3*lmMst2*(10421 - 6558*lmMst1 + 30*lmMsq*(-95 + 42*
        lmMst1) + 630*pow2(lmMsq) + 234*pow2(lmMst1)) - 18*(-1460 + 210*lmMsq +
        399*lmMst1)*pow2(lmMst2) - 288*pow3(lmMst1) + (pow2(Mst1)*((180*(111 -
        3028*lmMst2 + 90*lmMsq*(4 - 13*lmMst1 + 13*lmMst2) + 2*lmMst1*(1334 +
        675*lmMst2) - 90*pow2(lmMst1) - 1260*pow2(lmMst2)))/pow2(Msq) - (
        4962073 + 205200*B4 - 5400*DN - 405000*lmMsq + 120*lmMst1*(57971 -
        14625*lmMsq + 2700*pow2(lmMsq)) - 120*lmMst2*(52436 - 70455*lmMst1 +
        225*lmMsq*(-65 + 36*lmMst1) + 2700*pow2(lmMsq) + 8280*pow2(lmMst1)) -
        900*((1331 - 180*lmMsq)*pow2(lmMst1) + (8063 - 900*lmMsq - 3792*lmMst1)
        *pow2(lmMst2)) + (90*(-1725 + lmMst1*(-7006 + 2640*lmMsq - 4800*lmMst2)
        + (7006 - 2640*lmMsq)*lmMst2 + 1080*pow2(lmMst1) + 3720*pow2(lmMst2))*
        pow2(Mst1))/pow2(Msq) - 55800*pow3(lmMst1) - 2363400*pow3(lmMst2))/
        pow2(Mst2)))/150. + 6768*pow3(lmMst2) + (pow2(Mst2)*(257250*Dmglst2*
        Mst2*(557 - 224*lmMst2 + 12*lmMsq*(1 - 32*lmMst1 + 32*lmMst2) + 4*
        lmMst1*(53 + 96*lmMst2) - 384*pow2(lmMst2))*pow2(Mst1) + 20580*pow2(
        Msq)*(15*(341 + 642*lmMst2 + 64*lmMst1*(1 + lmMst2) - 30*lmMsq*(5 + 6*
        lmMst2) + 90*pow2(lmMsq) + 272*pow2(lmMst2))*pow2(Msq) + (1672 - 4470*
        lmMst2 + 150*lmMst1*(27 + 16*lmMst2) + 60*lmMsq*(7 - 40*lmMst1 + 40*
        lmMst2) - 2400*pow2(lmMst2))*pow2(Mst1)) - 128625*(18*Dmglst2*(13 - 28*
        lmMsq + 28*lmMst2) + (67 - 84*lmMsq + 84*lmMst2)*Mst2)*pow3(Mst2) + (
        4167613 - 19932360*lmMst2 + 20580*lmMst1*(701 + 540*lmMst2) + 420*
        lmMsq*(13109 - 26460*lmMst1 + 25620*lmMst2) + 176400*pow2(lmMsq) -
        10936800*pow2(lmMst2))*pow4(Mst1)))/(34300.*pow2(Mst1)*pow4(Msq)) + (
        30*(-43 + 30*lmMsq - 30*lmMst2)*pow4(Mst2))/(pow2(Msq)*pow2(Mst1)) - ((
        -41220947 + 420*lmMsq*(12479 + 69090*lmMst1 - 69930*lmMst2) + 21234990*
        lmMst2 - 10290*lmMst1*(2573 + 2820*lmMst2) + 176400*pow2(lmMsq) +
        29194200*pow2(lmMst2))*pow4(Mst2))/(68600.*pow4(Msq)) - (288*pow2(1 +
        lmMst2)*pow4(Mst2))/pow4(Mst1) + 162*pow4(Mst1)*((1.0702990137854083 -
        (4519*lmMst2)/980. + lmMsq*(0.36171579743008314 - (169*lmMst1)/63. + (
        19*lmMst2)/7.) + lmMst1*(4.249508692365835 + (31*lmMst2)/9.) - pow2(
        lmMsq)/63. - (8*pow2(lmMst1))/21. - (194*pow2(lmMst2))/63.)/pow4(Msq) -
        (363.3804294212688 + (76*B4)/9. - (2*DN)/9. - (35*lmMsq)/2. + lmMst1*(
        211.3489518770471 - (695*lmMsq)/9. + (40*pow2(lmMsq))/3.) - (
        214.87936507936507 - 20*lmMsq)*pow2(lmMst1) - lmMst2*(
        190.46006298815823 - (71398*lmMst1)/105. + (5*lmMsq*(-139 + 120*lmMst1)
        )/9. + (40*pow2(lmMsq))/3. + (334*pow2(lmMst1))/3.) + ((-146507 +
        14700*lmMsq + 91070*lmMst1)*pow2(lmMst2))/315. - (44*pow3(lmMst1))/9. -
        (1556*pow3(lmMst2))/9.)/pow4(Mst2) + (Dmglst2*(536.1152102791342 - (8*
        B4)/3. + (32*D3)/9. - (20*DN)/9. + 90*lmMsq - (123.11224321827497 + 20*
        lmMsq*(1 + lmMsq))*lmMst1 - lmMst2*(17.33220122616948 - 20*lmMsq*(1 +
        lmMsq) + (133.04550264550264 - 40*lmMsq)*lmMst1 - (1180*pow2(lmMst1))/
        9.) - (15886*pow2(lmMst1))/945. + (149.85608465608465 - 40*lmMsq - (
        2812*lmMst1)/9.)*pow2(lmMst2) - (92*pow3(lmMst1))/27. + (4988*pow3(
        lmMst2))/27.))/pow5(Mst2)) - (4*Dmglst2*(54*(344*OepS2 + 9*(15643 -
        774*lmMst1 + 774*lmMst2)*S2)*pow2(Mst1)*pow2(Mst2) + 4*(17308*OepS2 +
        27*(93919 - 12981*lmMst1 + 12981*lmMst2)*S2)*pow4(Mst1) + 27*(56*OepS2
        - 81*(-1677 + 14*lmMst1 - 14*lmMst2)*S2)*pow4(Mst2)) + 6*(-3*(3896*
        OepS2 - 81*(9473 + 974*lmMst1 - 974*lmMst2)*S2)*pow2(Mst1)*pow3(Mst2) +
        Mst2*(-29428*OepS2 + 27*(160997 + 22071*lmMst1 - 22071*lmMst2)*S2)*
        pow4(Mst1) + 432*(-4*OepS2 + 81*(22 + lmMst1 - lmMst2)*S2)*pow5(Mst2)))
        /(27.*pow5(Mst2)) + (54*(-1 + 2*lmMst2)*shiftst3*(2*(1 - 2*lmMst1 + 2*
        lmMst2)*pow2(Mst2)*pow4(Mst1) + 2*(1 - lmMst1 + lmMst2)*pow2(Mst1)*
        pow4(Mst2) + (2 - 6*lmMst1 + 6*lmMst2)*pow6(Mst1) + pow6(Mst2)))/(pow2(
        Mst1)*pow4(Mst2)) + (Dmglst2*(162*pow2(Mst1)*(109.11799176954733 - (8*
        B4)/3. + (32*D3)/9. - (20*DN)/9. + 80*lmMsq - lmMst1*(78.19061728395062
         + 20*pow2(lmMsq)) - (2888*pow2(lmMst1))/135. + lmMst2*(40*lmMsq*lmMst1
        + 20*pow2(lmMsq) + (4*(-21616 - 64515*lmMst1 + 31275*pow2(lmMst1)))/
        2025.) - (4*(-5023 + 1350*lmMsq + 6285*lmMst1)*pow2(lmMst2))/135. + (5*
        (291 + 2*(-103 + 84*lmMsq)*lmMst2 + lmMst1*(206 - 168*lmMsq + 168*
        lmMst2) - 168*pow2(lmMst2))*pow2(Mst1))/(27.*pow2(Msq)) + (20*pow3(
        lmMst1))/27. + (3340*pow3(lmMst2))/27.) + (pow2(Mst2)*(120*(69 + (-53 +
        42*lmMsq)*lmMst2 + lmMst1*(53 - 42*lmMsq + 42*lmMst2) - 42*pow2(lmMst2)
        )*pow2(Msq)*pow2(Mst1) + (3197 - 432*B4 + 576*D3 - 360*DN + 9180*lmMsq
        + 1620*pow2(lmMsq) - 216*lmMst1*(-7 + 15*pow2(lmMsq)) - 1404*pow2(
        lmMst1) + 36*lmMst2*(-291 - 464*lmMst1 + 90*lmMsq*(-1 + 2*lmMst1) + 90*
        pow2(lmMsq) + 32*pow2(lmMst1)) - 36*(-607 + 180*lmMsq + 336*lmMst1)*
        pow2(lmMst2) + 10944*pow3(lmMst2))*pow4(Msq) + 15*(216 + (-95 + 132*
        lmMsq)*lmMst2 + lmMst1*(95 - 132*lmMsq + 132*lmMst2) - 132*pow2(lmMst2)
        )*pow4(Mst1) + (3*pow2(Mst2)*(-20*(-118 - 109*lmMst1 + 6*lmMsq*(-4 +
        17*lmMst1 - 17*lmMst2) + 133*lmMst2 - 102*lmMst1*lmMst2 + 102*pow2(
        lmMst2))*pow2(Msq)*pow2(Mst1) + 6*(-115 + 366*lmMst2 + 64*lmMst1*lmMst2
        - 30*lmMsq*(-1 + 6*lmMst2) + 90*pow2(lmMsq) + 336*pow2(lmMst2))*pow4(
        Msq) + 5*(219 - 95*lmMst2 + 132*lmMsq*lmMst2 + lmMst1*(95 - 132*lmMsq +
        132*lmMst2) - 132*pow2(lmMst2))*pow4(Mst1)))/pow2(Mst1)))/pow4(Msq) - (
        6*(192*lmMst2*(1 + lmMst2) + ((560 - 600*lmMsq + 600*lmMst2)*pow2(Mst1)
        )/pow2(Msq))*pow6(Mst2))/pow4(Mst1)))/pow3(Mst2)))))/(14580.*Tbeta) -
        Al4p*xDmglst2*pow2(Dmglst2)*(Al4p*threeLoopFlag*(-(pow2(Mt)*pow2(s2t)*(
        (-2*Mst2*((5*(509 + lmMst1*(12 - 36*lmMst2) + 36*lmMsq*(-2 + lmMst1 -
        lmMst2) + 60*lmMst2 + 36*pow2(lmMst2)))/pow2(Msq) + (48*(3 + 4*lmMst2 -
        3*pow2(lmMst2)))/pow2(Mst1)))/9. - (262.1827160493827 - 76*B4 + 2*DN +
        (1070*lmMsq)/3. + 2*(-31 + 10*lmMsq)*lmMst1 - (2*(2009 + 90*lmMsq +
        528*lmMst1)*lmMst2)/9. - (16*pow2(lmMst1))/3. + (16*(17 + 12*lmMst1)*
        pow2(lmMst2))/3. + (20*(79 + 4*lmMst1*(1 + 3*lmMsq - 3*lmMst2) - 4*(1 +
        3*lmMsq)*lmMst2 + 12*pow2(lmMst2))*pow2(Mst1))/(3.*pow2(Msq)) - 64*
        pow3(lmMst2))/Mst2 + ((-28*OepS2*(415*pow2(Mst1) + 24*pow2(Mst2)))/243.
         + (S2*((21422 + 87150*lmMst1 - 87150*lmMst2)*pow2(Mst1) + 9*(-1373 +
        560*lmMst1 - 560*lmMst2)*pow2(Mst2)))/90.)/pow3(Mst2) + (pow2(Mst1)*(
        690.0618312757201 + 76*B4 - 2*DN - 380*lmMsq - (222.07407407407408 +
        40*lmMsq)*lmMst1 + (4*(5963 + 270*lmMsq + 1422*lmMst1)*lmMst2)/27. - (
        92*pow2(lmMst1))/3. - 4*(45 + 16*lmMst1)*pow2(lmMst2) + 64*pow3(lmMst2)
        ))/pow3(Mst2) - (5*(6*Mst2*(178 + lmMst1*(9 + 60*lmMsq - 60*lmMst2) -
        9*lmMst2 - 60*lmMsq*lmMst2 + 60*pow2(lmMst2))*pow2(Mst1) + (1307 + 27*
        lmMst1 + 180*lmMsq*(-2 + lmMst1 - lmMst2) + 333*lmMst2 - 180*lmMst1*
        lmMst2 + 180*pow2(lmMst2))*pow3(Mst2)))/(18.*pow4(Msq)))) + s2t*pow3(
        Mt)*((10*(17 - 60*lmMsq + 60*lmMst2))/(9.*pow2(Msq)) - (
        1071.816402116402 - (16*B4)/9. + (16*D3)/9. - (8*DN)/9. + (140*lmMsq)/
        3. + (184*lmMst1)/3. - (32*lmMt)/9. + 10*pow2(lmMsq) + (2*lmMst2*(6065
        - 270*lmMsq + 1002*lmMst1 - 96*pow2(lmMst1)))/27. + (82*pow2(lmMst1))/
        9. - (2*(143 + 32*lmMst1)*pow2(lmMst2))/9. + (20*pow2(Mst1))/(9.*pow2(
        Msq)) + (128*pow3(lmMst2))/9.)/pow2(Mst2) - pow2(Mst2)*((
        0.2777777777777778 + 60*lmMsq - 60*lmMst2)/pow4(Msq) - (64*(2 + lmMst2
        - 3*pow2(lmMst2)))/(9.*pow4(Mst1))) + S2*((89.65714285714286 + 84*
        lmMst1 - 84*lmMst2)/pow2(Mst2) + ((847.3523809523809 + 480*lmMst1 -
        480*lmMst2)*pow2(Mst1))/pow4(Mst2)) + pow2(Mst1)*((5*(31 + 12*lmMsq -
        12*lmMst2))/(36.*pow4(Msq)) - (1275.5838871252204 - (16*B4)/9. + (16*
        D3)/9. - (8*DN)/9. - (220*lmMsq)/3. + (333.517037037037 + 20*lmMsq)*
        lmMst1 - 20*lmMsq*lmMst2 + (32*(17 - 5*lmMst1 + 5*lmMst2)*lmMt)/9. + (
        4*lmMst2*(148919 + 50160*lmMst1 - 1425*pow2(lmMst1)))/675. + (224*pow2(
        lmMst1))/15. - (4*(3032 + 545*lmMst1)*pow2(lmMst2))/45. + (4*pow3(
        lmMst1))/9. + (508*pow3(lmMst2))/9.)/pow4(Mst2)) - (16*OepS2*(40*pow2(
        Mst1) + 7*pow2(Mst2)))/(27.*pow4(Mst2)) + (160*(-4 + 15*lmMsq - 15*
        lmMst2)*pow2(Msq)*pow2(Mst2) + 4*(-599 + 64*lmMst1*(-2 + lmMst2) - 530*
        lmMst2 - 30*lmMsq*(-13 + 6*lmMst2) + 90*pow2(lmMsq) + 208*pow2(lmMst2))
        *pow4(Msq) + 15*(-9 + 140*lmMsq - 140*lmMst2)*pow4(Mst2))/(36.*pow2(
        Mst1)*pow4(Msq))) - Mt*pow3(s2t)*(71.74372427983539 - (38*B4)/9. + (40*
        D3)/9. - (7*DN)/3. + (175*lmMsq)/9. - (lmMst1*(-178 + 70*lmMsq + 15*
        pow2(lmMsq)))/3. - (25*pow2(lmMst1))/18. + (lmMst2*(-2738 + 606*lmMst1
        + 90*lmMsq*(7 + 3*lmMst1) + 135*pow2(lmMsq) + 48*pow2(lmMst1)))/27. - (
        (667 + 180*lmMsq + 336*lmMst1)*pow2(lmMst2))/18. + (4*OepS2*(21 + (
        1157*pow2(Mst1))/pow2(Mst2)))/729. + (((10*(496 - 18*lmMsq*(2 + 19*
        lmMst1 - 19*lmMst2) - 93*lmMst2 + 3*lmMst1*(43 + 114*lmMst2) - 342*
        pow2(lmMst2)))/pow2(Msq) + (3*(-1047 + 64*lmMst1*(-2 + lmMst2) - 530*
        lmMst2 - 30*lmMsq*(-13 + 6*lmMst2) + 90*pow2(lmMsq) + 784*pow2(lmMst2))
        )/pow2(Mst1))*pow2(Mst2))/108. - (S2*((648463 + 34710*lmMst1 - 34710*
        lmMst2)*pow2(Mst1) + 9*(65819 + 70*lmMst1 - 70*lmMst2)*pow2(Mst2)))/(
        270.*pow2(Mst2)) + pow2(Mst1)*((5*(86 + lmMst1*(33 - 18*lmMst2) + 18*
        lmMsq*(-8 + lmMst1 - lmMst2) + 111*lmMst2 + 18*pow2(lmMst2)))/(54.*
        pow2(Msq)) - (137.6673072702332 + (32*B4)/9. - (32*D3)/9. + (16*DN)/9.
         + (485*lmMsq)/
        18. - (2*(31991 + 3375*lmMsq)*lmMst1)/2025. + (5*pow2(lmMsq))/2. +
        lmMst2*(12.133086419753086 - (5*lmMsq)/3. - (637*lmMst1)/15. - (143*
        pow2(lmMst1))/9.) + (159*pow2(lmMst1))/10. + ((2591 + 2710*lmMst1)*
        pow2(lmMst2))/90. + (5*pow3(lmMst1))/9. - (133*pow3(lmMst2))/9.)/pow2(
        Mst2)) + (152*pow3(lmMst2))/9. + (16*(2 + lmMst2 - 3*pow2(lmMst2))*
        pow4(Mst2))/(9.*pow4(Mst1)) + (5*((-167 + 180*lmMsq*(-3 + 2*lmMst1 - 2*
        lmMst2) + 366*lmMst2 - 6*lmMst1*(-29 + 60*lmMst2) + 360*pow2(lmMst2))*
        pow2(Mst2)*pow4(Mst1) + 96*(-4 + 15*lmMsq - 15*lmMst2)*pow2(Msq)*pow4(
        Mst2) + 2*(1603 - 360*lmMsq*(1 + 3*lmMst1 - 3*lmMst2) + 192*lmMst2 +
        24*lmMst1*(7 + 45*lmMst2) - 1080*pow2(lmMst2))*pow2(Mst1)*pow4(Mst2) +
        9*(-9 + 140*lmMsq - 140*lmMst2)*pow6(Mst2)))/(432.*pow2(Mst1)*pow4(Msq)
        )) + (Mt*(210*T1ep*(21*pow2(Mst2)*(36*Mst2*s2t*pow2(Mt) - 24*Mt*pow2(
        Mst2)*pow2(s2t) + 32*pow3(Mt) + pow3(Mst2)*pow3(s2t)) + pow2(Mst1)*(
        4320*Mst2*s2t*pow2(Mt) - 8715*Mt*pow2(Mst2)*pow2(s2t) + 17584*pow3(Mt)
        + 1157*pow3(Mst2)*pow3(s2t))) + (pow3(Mt)*(-2*pow4(Msq)*(-3*(4455287 -
        18900*lmMsq*(5 + 6*lmMst1 - 6*lmMst2) + 493080*lmMst2 + 1260*(31 + 56*
        lmMst2)*lmMt - 15680*OepS2 - 648*(1471 + 490*lmMst2)*S2 + 3780*lmMst1*(
        -83 + 96*lmMst2 - 16*lmMt + 84*S2) + 30240*pow2(lmMst1) - 418320*pow2(
        lmMst2) + 15120*pow2(lmMt))*pow2(Mst1)*pow2(Mst2) - 2*(15392987 +
        7445655*lmMst2 + 28350*lmMsq*(43 - 12*lmMst1 + 18*lmMst2 - 6*lmMt) -
        615440*OepS2 - 54*(80377 + 230790*lmMst2)*S2 - 11340*(35 + 4*lmMst2 -
        4*lmMt)*pow2(lmMst1) + 1361745*pow2(lmMst2) - 945*lmMt*(-840 + 13*
        lmMst2 + 48*pow2(lmMst2)) - 315*lmMst1*(17917 + 1599*lmMst2 + 201*lmMt
        - 39564*S2 + 432*pow2(lmMst2) - 432*pow2(lmMt)) - 22680*(17 + 6*lmMst2)
        *pow2(lmMt) + 181440*pow3(lmMst2))*pow4(Mst1) + 181440*(3 + 4*lmMst2 -
        3*pow2(lmMst2))*pow4(Mst2)) + 37800*pow2(Msq)*((113 - 18*lmMst1*(1 + 2*
        lmMsq - 2*lmMst2) + 18*lmMst2 + 36*lmMsq*lmMst2 - 36*pow2(lmMst2))*
        pow2(Mst2)*pow4(Mst1) + (137 - 12*lmMst1*(1 + 3*lmMsq - 3*lmMst2) + 6*
        lmMst2 + 36*lmMsq*lmMst2 + 6*lmMt - 36*pow2(lmMst2))*pow2(Mst1)*pow4(
        Mst2)) + 4725*(6*(241 + 4*(5 + 18*lmMsq)*lmMst2 + lmMst1*(-22 - 72*
        lmMsq + 72*lmMst2) + 2*lmMt - 72*pow2(lmMst2))*pow4(Mst1)*pow4(Mst2) +
        (1391 - 48*lmMst2 + 18*lmMst1*(-3 + 20*lmMst2) + 90*lmMt - 72*lmMst2*
        lmMt + 12*lmMsq*(1 - 30*lmMst1 + 36*lmMst2 + 6*lmMt) - 72*pow2(lmMsq) -
        360*pow2(lmMst2))*pow2(Mst1)*pow6(Mst2))))/(pow2(Mst1)*pow4(Msq))))/(
        25515.*pow5(Mst2)) + (MuSUSY*pow2(Mt)*((144*pow2(Mt)*(33934 - 90*B4 +
        90*D3 - 45*DN + 720*lmMst1 + 120*(163 + 24*lmMst1)*lmMst2 - 120*lmMt -
        10206*S2 - 720*(2 + lmMst1)*pow2(lmMst2) + 720*pow3(lmMst2)))/pow4(
        Mst2) + 7290*pow2(s2t)*((5*(448 - 273*lmMst2 + 18*lmMsq*(8 - 19*lmMst1
        + 19*lmMst2) + 3*lmMst1*(43 + 114*lmMst2) - 342*pow2(lmMst2)))/(27.*
        pow2(Msq)) + (92.43189300411522 - (76*B4)/9. + (80*D3)/9. - (14*DN)/3.
         + (545*lmMsq)/
        9. + 5*pow2(lmMsq) - (2*lmMst1*(-502 + 210*lmMsq + 45*pow2(lmMsq)))/9.
         - (25*pow2(lmMst1))/
        9. + (lmMst2*(-6175 + 1308*lmMst1 + 90*lmMsq*(11 + 6*lmMst1) + 270*
        pow2(lmMsq) + 96*pow2(lmMst1)))/27. - ((371 + 180*lmMsq + 336*lmMst1)*
        pow2(lmMst2))/9. + ((98.88888888888889 + 30*(1 - 2*lmMsq)*lmMst1 - 30*
        lmMst2 + 60*(lmMsq + lmMst1)*lmMst2 - 60*pow2(lmMst2))*pow2(Mst1))/
        pow2(Msq) + (304*pow3(lmMst2))/9.)/pow2(Mst2) + (pow2(Mst2)*((5*(3125 -
        876*lmMst2 + 540*lmMsq*(1 - 4*lmMst1 + 4*lmMst2) + 48*lmMst1*(7 + 45*
        lmMst2) - 2160*pow2(lmMst2)))/pow4(Msq) + (768*(2 + lmMst2 - 3*pow2(
        lmMst2)))/pow4(Mst1)))/216. + pow2(Mst1)*((5*(493 + 5*(-17 + 60*lmMsq)*
        lmMst2 + lmMst1*(85 - 300*lmMsq + 300*lmMst2) - 300*pow2(lmMst2)))/(36.
        *pow4(Msq)) - (182.90272153635118 + (140*B4)/9. - 16*D3 + (74*DN)/9. -
        (20*lmMsq)/3. + lmMst1*(-174.74765432098766 + 40*lmMsq + 10*pow2(lmMsq)
        ) + lmMst2*(252.96987654320986 - (6002*lmMst1)/45. - 20*lmMsq*(2 +
        lmMst1) - 10*pow2(lmMsq) - (106*pow2(lmMst1))/3.) + (1556*pow2(lmMst1))
        /45. + (98.8 + 20*lmMsq + (878*lmMst1)/9.)*pow2(lmMst2) + (10*pow3(
        lmMst1))/9. - (190*pow3(lmMst2))/3.)/pow4(Mst2)) + ((47120*OepS2 - 54*(
        620417 + 17670*lmMst1 - 17670*lmMst2)*S2)*pow2(Mst1) + 3*(280*OepS2 -
        81*(65819 + 70*lmMst1 - 70*lmMst2)*S2)*pow2(Mst2))/(3645.*pow4(Mst2)) +
        (160*(-4 + 15*lmMsq - 15*lmMst2)*pow2(Msq)*pow2(Mst2) + 4*(-919 + 64*
        lmMst1*(-2 + lmMst2) - 466*lmMst2 - 30*lmMsq*(-13 + 6*lmMst2) + 90*
        pow2(lmMsq) + 592*pow2(lmMst2))*pow4(Msq) + 15*(-9 + 140*lmMsq - 140*
        lmMst2)*pow4(Mst2))/(72.*pow2(Mst1)*pow4(Msq))) + (120*s2t*T1ep*(2*(
        3073*Mt - 589*Mst2*s2t)*pow2(Mst1) - 21*(-16*Mt + Mst2*s2t)*pow2(Mst2))
        )/pow5(Mst2) - (Mt*s2t*(pow4(Msq)*(12*pow2(Mst1)*pow2(Mst2)*(238288 -
        61560*B4 + 1620*DN + 288900*lmMsq + 1620*(-31 + 10*lmMsq)*lmMst1 - 180*
        (1817 + 90*lmMsq + 528*lmMst1)*lmMst2 + 2240*OepS2 - 81*(-1373 + 560*
        lmMst1 - 560*lmMst2)*S2 - 4320*pow2(lmMst1) + 4320*(11 + 12*lmMst1)*
        pow2(lmMst2) - 51840*pow3(lmMst2)) - 5*(769589 + 295488*B4 - 7776*DN -
        1432080*lmMsq - 144*(2161 + 810*lmMsq)*lmMst1 + 144*(17377 + 810*lmMsq
        + 4428*lmMst1)*lmMst2 - 98336*OepS2 + 756*(259 + 2634*lmMst1 - 2634*
        lmMst2)*S2 - 49248*pow2(lmMst1) - 2592*(179 + 96*lmMst1)*pow2(lmMst2) +
        248832*pow3(lmMst2))*pow4(Mst1) + 103680*(3 + 4*lmMst2 - 3*pow2(lmMst2)
        )*pow4(Mst2)) + 10800*pow2(Msq)*((983 + 36*lmMst1 + 36*lmMsq*(-2 + 3*
        lmMst1 - 3*lmMst2) + 36*lmMst2 - 108*lmMst1*lmMst2 + 108*pow2(lmMst2))*
        pow2(Mst2)*pow4(Mst1) + (509 + 12*lmMst1 + 36*lmMsq*(-2 + lmMst1 -
        lmMst2) + 60*lmMst2 - 36*lmMst1*lmMst2 + 36*pow2(lmMst2))*pow2(Mst1)*
        pow4(Mst2)) + 2700*((2375 + 81*lmMst1 + 180*lmMsq*(-2 + 3*lmMst1 - 3*
        lmMst2) + 279*lmMst2 - 540*lmMst1*lmMst2 + 540*pow2(lmMst2))*pow4(Mst1)
        *pow4(Mst2) + (1307 + 27*lmMst1 + 180*lmMsq*(-2 + lmMst1 - lmMst2) +
        333*lmMst2 - 180*lmMst1*lmMst2 + 180*pow2(lmMst2))*pow2(Mst1)*pow6(
        Mst2))))/(pow2(Mst1)*pow4(Msq)*pow5(Mst2))))/(7290.*Tbeta)) + (Mt*
        twoLoopFlag*(2*(pow2(Mst2)*(36*(5 - 3*lmMst1 + 3*lmMst2)*Mst2*s2t*pow2(
        Mt) - 54*(12 + lmMst1 - lmMst2)*Mt*pow2(Mst2)*pow2(s2t) + 4*(155 + 3*
        lmMst2*(41 - 6*lmMt) - 18*lmMst1*(4 + lmMst2 - lmMt) - 51*lmMt + 18*
        pow2(lmMst2))*pow3(Mt) - 9*(4*lmMst1 - 5*lmMst2)*pow3(Mst2)*pow3(s2t))*
        pow4(Mst1) + pow2(Mst1)*(6*(17 - 6*lmMst2)*Mst2*s2t*pow2(Mt) - 54*(8 +
        lmMst1 - lmMst2)*Mt*pow2(Mst2)*pow2(s2t) + 4*(11 + 18*lmMst1 - 21*
        lmMst2 + 3*lmMt)*pow3(Mt) + 9*(6 + 5*lmMst2 + lmMst1*(-5 + 2*lmMst2) -
        2*pow2(lmMst2))*pow3(Mst2)*pow3(s2t))*pow4(Mst2)) + (72*(9 - 10*lmMst1
        + 10*lmMst2)*Mst2*s2t*pow2(Mt) + 54*(-27 + 4*lmMst1 - 4*lmMst2)*Mt*
        pow2(Mst2)*pow2(s2t) + 8*(335 + 699*lmMst2 - 18*lmMst1*(29 + 7*lmMst2 -
        7*lmMt) - 3*(59 + 42*lmMst2)*lmMt + 126*pow2(lmMst2))*pow3(Mt) + 9*(1 -
        10*lmMst1 + 10*lmMst2)*pow3(Mst2)*pow3(s2t))*pow6(Mst1) - 18*(-2 +
        lmMst2)*s2t*(-4*pow2(Mt) + pow2(Mst2)*pow2(s2t))*pow7(Mst2) + (18*Mt*
        MuSUSY*s2t*(-2*(-8*(10 + lmMst1 - lmMst2)*Mt + Mst2*s2t*(8 - 9*lmMst1 +
        9*lmMst2 + 2*lmMst1*lmMst2 - 2*pow2(lmMst2)))*pow2(Mst2)*pow4(Mst1) -
        2*(-4*(8 + lmMst1 - lmMst2)*Mt + Mst2*s2t*(8 - 5*lmMst1 + 4*lmMst2 + 2*
        lmMst1*lmMst2 - 2*pow2(lmMst2)))*pow2(Mst1)*pow4(Mst2) + (268*Mt +
        Mst2*s2t*(-17 - 4*lmMst1*(-7 + lmMst2) - 28*lmMst2 + 4*pow2(lmMst2)))*
        pow6(Mst1) + 2*(-2 + lmMst2)*s2t*pow7(Mst2)))/Tbeta))/(54.*pow2(Mst1)*
        pow7(Mst2))) - threeLoopFlag*pow2(Al4p)*(-(pow2(Mt)*pow2(s2t)*((pow2(
        Mst1)*(397.2878086419753 + 48*B4 - 60*lmMsq - 5*lmMst1*(43 - 4*lmMsq +
        4*pow2(lmMsq)) - (28*pow2(lmMst1))/3. + (lmMst2*(901 - 780*lmMst1 + 60*
        lmMsq*(-1 + 2*lmMst1) + 60*pow2(lmMsq) + 32*pow2(lmMst1)))/3. - (8*(-
        101 + 15*lmMsq + 44*lmMst1)*pow2(lmMst2))/3. - (20*(9 + 4*lmMst1*(-4 +
        3*lmMsq - 3*lmMst2) + 4*(4 - 3*lmMsq)*lmMst2 + 12*pow2(lmMst2))*pow2(
        Mst1))/(9.*pow2(Msq)) + (320*pow3(lmMst2))/3.))/Mst2 - (2*((5*(71 + 12*
        lmMsq*(-2 + lmMst1 - lmMst2) + 40*lmMst2 - 4*lmMst1*(4 + 3*lmMst2) +
        12*pow2(lmMst2)))/pow2(Msq) - (48*pow2(1 + lmMst2))/pow2(Mst1))*pow3(
        Mst2))/9. - (5*(Mst2*(Mst2*(911 + 12*lmMsq*(-37 + 18*lmMst1 - 18*
        lmMst2) + 606*lmMst2 - 54*lmMst1*(3 + 4*lmMst2) + 216*pow2(lmMst2)) +
        Dmglst2*(5591 + 12*lmMsq*(-181 + 90*lmMst1 - 90*lmMst2) + 2550*lmMst2 -
        54*lmMst1*(7 + 20*lmMst2) + 1080*pow2(lmMst2))) + (242 + 24*lmMsq*(1 +
        18*lmMst1 - 18*lmMst2) + 300*lmMst2 - 108*lmMst1*(3 + 4*lmMst2) + 432*
        pow2(lmMst2))*pow2(Mst1))*pow3(Mst2))/(216.*pow4(Msq)) + ((
        536.7597736625514 + 48*B4 - 55*lmMsq - lmMst1*(228.77777777777777 - 10*
        lmMsq + 20*pow2(lmMsq)) + (326*pow2(lmMst1))/9. + (lmMst2*(2635 - 3160*
        lmMst1 + 90*lmMsq*(-1 + 4*lmMst1) + 180*pow2(lmMsq) + 528*pow2(lmMst1))
        )/9. + (314.8888888888889 - 40*lmMsq - 208*lmMst1)*pow2(lmMst2) - (16*
        pow3(lmMst1))/9. + (1360*pow3(lmMst2))/9.)*pow4(Mst1))/pow3(Mst2) +
        Mst2*(368.5208333333333 + (110*B4)/3. - DN/3. - 120*lmMsq + 20*pow2(
        lmMsq) - (lmMst1*(199 - 30*lmMsq + 30*pow2(lmMsq)))/3. + (lmMst2*(1227
        - 322*lmMst1 + 30*lmMsq*(-5 + 2*lmMst1) + 30*pow2(lmMsq) - 16*pow2(
        lmMst1)))/3. - (16*pow2(lmMst1))/3. - 2*(-99 + 10*lmMsq + 24*lmMst1)*
        pow2(lmMst2) - (10*(21 + 8*lmMst1*(-4 + 3*lmMsq - 3*lmMst2) + 8*(4 - 3*
        lmMsq)*lmMst2 + 24*pow2(lmMst2))*pow2(Mst1))/(9.*pow2(Msq)) + (160*
        pow3(lmMst2))/3. - (5*(245 + 12*lmMsq*(-1 + 36*lmMst1 - 36*lmMst2) +
        336*lmMst2 - 108*lmMst1*(3 + 4*lmMst2) + 432*pow2(lmMst2))*pow4(Mst1))/
        (216.*pow4(Msq))) + Dmglst2*(371.73935185185184 + (262*B4)/3. - (5*DN)/
        3. - 300*lmMsq + lmMst1*(57.666666666666664 - 30*lmMsq - 10*pow2(lmMsq)
        ) + 20*pow2(lmMsq) + (lmMst2*(1559 - 130*lmMst1 + 30*lmMsq*(-1 + 2*
        lmMst1) + 30*pow2(lmMsq) - 16*pow2(lmMst1)))/3. + (16*pow2(lmMst1))/3.
         - (2*(-217 + 30*lmMsq + 136*lmMst1)*pow2(lmMst2))/
        3. - (10*(59 + 8*lmMst1*(-2 + 3*lmMsq - 3*lmMst2) + 8*(2 - 3*lmMsq)*
        lmMst2 + 24*pow2(lmMst2))*pow2(Mst1))/(3.*pow2(Msq)) + 96*pow3(lmMst2)
        + (pow2(Mst1)*(300.1378086419753 + (296*B4)/3. - (4*DN)/3. - 340*lmMsq
        + lmMst1*(40.55555555555556 - 60*lmMsq - 20*pow2(lmMsq)) + (28*pow2(
        lmMst1))/3. + lmMst2*(428.77777777777777 - 84*lmMst1 + 20*lmMsq*(3 + 2*
        lmMst1) + 20*pow2(lmMsq) + (32*pow2(lmMst1))/3.) - (8*(-28 + 15*lmMsq +
        60*lmMst1)*pow2(lmMst2))/3. - (40*(15 + lmMst1*(-4 + 6*lmMsq - 6*
        lmMst2) + (4 - 6*lmMsq)*lmMst2 + 6*pow2(lmMst2))*pow2(Mst1))/(3.*pow2(
        Msq)) + (448*pow3(lmMst2))/3.))/pow2(Mst2) - (pow2(Mst2)*(360*(95 + 12*
        lmMsq*(-2 + lmMst1 - lmMst2) + 32*lmMst2 - 4*lmMst1*(2 + 3*lmMst2) +
        12*pow2(lmMst2))*pow2(Msq)*pow2(Mst1) - 1152*(-1 + 2*lmMst2 + 3*pow2(
        lmMst2))*pow4(Msq) + 5*(1561 + 12*lmMsq*(1 + 90*lmMst1 - 90*lmMst2) +
        366*lmMst2 - 54*lmMst1*(7 + 20*lmMst2) + 1080*pow2(lmMst2))*pow4(Mst1))
        )/(108.*pow2(Mst1)*pow4(Msq)) - pow4(Mst1)*((70.67129629629629 - (5*
        lmMsq)/18. + (5*(-7 + 20*lmMsq)*lmMst1)/2. + (17.77777777777778 - 50*(
        lmMsq + lmMst1))*lmMst2 + 50*pow2(lmMst2))/pow4(Msq) + (
        309.29708504801096 - (296*B4)/3. + (4*DN)/3. + 375*lmMsq + lmMst1*(
        66.91358024691358 + 30*lmMsq + 20*pow2(lmMsq)) + (46*pow2(lmMst1))/3. -
        lmMst2*(10*lmMsq*(3 + 4*lmMst1) + 20*pow2(lmMsq) + (4*(11723 - 702*
        lmMst1 + 756*pow2(lmMst1)))/81.) + (2*(-75 + 60*lmMsq + 344*lmMst1)*
        pow2(lmMst2))/3. - (16*pow3(lmMst1))/3. - (560*pow3(lmMst2))/3.)/pow4(
        Mst2))) + ((2*OepS2*(Dmglst2*(8163*pow2(Mst1)*pow2(Mst2) + 23734*pow4(
        Mst1) + 945*pow4(Mst2)) - 3*(627*pow2(Mst1)*pow3(Mst2) + 1066*Mst2*
        pow4(Mst1) + 189*pow5(Mst2))))/729. - (S2*(Dmglst2*(9*(23989 + 27210*
        lmMst1 - 27210*lmMst2)*pow2(Mst1)*pow2(Mst2) + 2*(525961 + 356010*
        lmMst1 - 356010*lmMst2)*pow4(Mst1) + 81*(-453 + 350*lmMst1 - 350*
        lmMst2)*pow4(Mst2)) - 15*(9*(685 + 418*lmMst1 - 418*lmMst2)*pow2(Mst1)*
        pow3(Mst2) + 2*(6143 + 3198*lmMst1 - 3198*lmMst2)*Mst2*pow4(Mst1) + 81*
        (-1 + 14*lmMst1 - 14*lmMst2)*pow5(Mst2))))/540.)/pow4(Mst2))) + (pow4(
        Mt)*(48*pow2(Mst1)*pow2(Mst2)*(71744 - 28755*lmMst2 - 4050*(lmMst1 -
        lmMst2)*pow2(lmMsq) + 540*(-15 + 4*lmMt)*pow2(lmMst1) + 4050*lmMsq*(11
        - lmMst1 + 2*(1 + lmMst1)*lmMst2 - lmMt - 2*pow2(lmMst2)) + 89910*pow2(
        lmMst2) - 135*lmMt*(113 + 132*lmMst2 + 48*pow2(lmMst2)) - 270*lmMst1*(
        221 + lmMst2*(279 - 16*lmMt) - 66*lmMt + 112*pow2(lmMst2) - 24*pow2(
        lmMt)) - 6480*(1 + lmMst2)*pow2(lmMt) - (1350*(-25 + lmMst1*(-5 + 6*
        lmMsq - 6*lmMst2) + (4 - 6*lmMsq)*lmMst2 + lmMt + 6*pow2(lmMst2))*pow2(
        Mst1))/pow2(Msq) + 30240*pow3(lmMst2)) + 20*(567143 - 125172*lmMst2 +
        648*lmMst2*(-38 + 5*lmMst2)*lmMt - 9720*(lmMsq*(lmMst1*(9 - 2*lmMt) +
        2*lmMst2*(-6 + lmMt) + 3*(-4 + lmMt)) + lmMt) - 9720*(lmMst1 - lmMst2)*
        pow2(lmMsq) + 240300*pow2(lmMst2) - 108*lmMst1*(2111 + lmMst1*(457 -
        12*lmMst2 - 126*lmMt) - 420*lmMt + 4*lmMst2*(454 + 39*lmMt) + 750*pow2(
        lmMst2) - 288*pow2(lmMt)) - 15552*(1 + 2*lmMst2)*pow2(lmMt) + 216*pow3(
        lmMst1) + 79488*pow3(lmMst2))*pow4(Mst1) - 160*OepS2*(-24*(186*Dmglst2
        - 55*Mst2)*Mst2*pow2(Mst1) + 63*(-5*Dmglst2 + 3*Mst2)*pow3(Mst2) +
        4060*pow4(Mst1)) + 108*S2*(-24*Mst2*(2*Dmglst2*(601 + 2790*lmMst1 -
        2790*lmMst2) - 3*(457 + 550*lmMst1 - 550*lmMst2)*Mst2)*pow2(Mst1) + 27*
        (25*Dmglst2*(27 - 14*lmMst1 + 14*lmMst2) + 3*(-79 + 70*lmMst1 - 70*
        lmMst2)*Mst2)*pow3(Mst2) + 40*(4082 + 3045*lmMst1 - 3045*lmMst2)*pow4(
        Mst1)) + (81*(800*(19 + (-8 + 6*lmMsq)*lmMst2 + lmMst1*(7 - 6*lmMsq +
        6*lmMst2) + lmMt - 6*pow2(lmMst2))*pow2(Msq)*pow2(Mst1) + (813 - 3360*
        lmMst2 - 2400*(lmMst1 - lmMst2)*pow2(lmMsq) - 1280*(1 + lmMst2)*pow2(
        lmMst1) + 2400*lmMsq*(7 + lmMst1 - 2*lmMst2 + 2*lmMst1*lmMst2 + lmMt -
        2*pow2(lmMst2)) + 30880*pow2(lmMst2) - 80*lmMt*(175 + 96*lmMst2 + 32*
        pow2(lmMst2)) - 80*lmMst1*(167 + 258*lmMst2 - 32*(1 + lmMst2)*lmMt +
        112*pow2(lmMst2)) - 3840*pow2(lmMt) + 10240*pow3(lmMst2))*pow4(Msq) +
        200*(38 + 4*(-2 + 3*lmMsq)*lmMst2 + lmMst1*(7 - 12*lmMsq + 12*lmMst2) +
        lmMt - 12*pow2(lmMst2))*pow4(Mst1))*pow4(Mst2))/pow4(Msq) - (9*Dmglst2*
        Mst2*(pow4(Msq)*(5*pow2(Mst1)*pow2(Mst2)*(7335 + 169856*lmMst2 + 4320*(
        lmMst1 - lmMst2)*pow2(lmMsq) + 2304*(-1 + lmMst2)*pow2(lmMst1) + 21984*
        pow2(lmMst2) + 480*lmMsq*(-79 - 30*lmMst2 - 9*lmMst1*(-3 + 2*lmMst2) +
        3*lmMt + 18*pow2(lmMst2)) + 48*lmMt*(31 - 128*lmMst2 + 96*pow2(lmMst2))
        + 144*lmMst1*(-269 + 32*lmMt - 2*lmMst2*(63 + 16*lmMt) + 112*pow2(
        lmMst2)) - 18432*pow3(lmMst2)) + 8*(93487 + 196530*lmMst2 + 2700*(
        lmMst1 - lmMst2)*pow2(lmMsq) - 360*(23 + 8*lmMst2 - 4*lmMt)*pow2(
        lmMst1) + 68040*pow2(lmMst2) + 300*lmMsq*(29 + lmMst1*(9 - 18*lmMst2) +
        6*lmMst2 - 15*lmMt + 18*pow2(lmMst2)) + 30*lmMt*(301 - 246*lmMst2 + 48*
        pow2(lmMst2)) - 4320*(2 + lmMst2)*pow2(lmMt) + 20*lmMst1*(-5846 + 321*
        lmMt - 12*lmMst2*(209 + 12*lmMt) + 576*pow2(lmMst2) + 216*pow2(lmMt)) -
        8640*pow3(lmMst2))*pow4(Mst1) - 23040*(-1 + 2*lmMst2 + 3*pow2(lmMst2))*
        pow4(Mst2)) - 800*pow2(Msq)*((439 + 6*(-8 + 21*lmMsq)*lmMst2 + 3*
        lmMst1*(17 - 42*lmMsq + 42*lmMst2) - 3*lmMt - 126*pow2(lmMst2))*pow2(
        Mst2)*pow4(Mst1) + 6*(64 - 20*lmMst2 + 6*lmMst1*(2 + 3*lmMst2) + 3*lmMt
        - 3*lmMst2*lmMt + lmMsq*(5 - 18*lmMst1 + 21*lmMst2 + 3*lmMt) - 3*pow2(
        lmMsq) - 18*pow2(lmMst2))*pow2(Mst1)*pow4(Mst2) + (313 + 42*(-2 + 3*
        lmMsq)*lmMst2 + 3*lmMst1*(23 - 42*lmMsq + 42*lmMst2) + 15*lmMt - 126*
        pow2(lmMst2))*pow6(Mst1)) - 50*(3*(1693 - 136*lmMst2 + 6*lmMst1*(19 +
        84*lmMst2) - 4*lmMsq*(5 + 126*lmMst1 - 132*lmMst2 - 6*lmMt) + 42*lmMt -
        24*lmMst2*lmMt - 24*pow2(lmMsq) - 504*pow2(lmMst2))*pow4(Mst1)*pow4(
        Mst2) + 4*(1330 + 12*(-4 + 33*lmMsq)*lmMst2 + lmMst1*(51 - 396*lmMsq +
        396*lmMst2) - 3*lmMt - 396*pow2(lmMst2))*pow2(Mst2)*pow6(Mst1))))/(
        pow2(Mst1)*pow4(Msq)) + 90*(32*((5*(58 - 36*lmMst2 + 6*lmMst1*(4 + 3*
        lmMst2) - 3*lmMt - 9*lmMst2*lmMt + 3*lmMsq*(5 - 6*lmMst1 + 9*lmMst2 +
        3*lmMt) - 9*pow2(lmMsq) - 18*pow2(lmMst2)))/pow2(Msq) + (72*pow2(1 +
        lmMst2))/pow2(Mst1)) - (5*(3*Mst2*(Mst2*(-403 + 24*lmMst2 - 18*lmMst1*(
        3 + 4*lmMst2) + 12*lmMsq*(1 + 6*lmMst1 - 12*lmMst2 - 6*lmMt) + 18*lmMt
        + 72*lmMst2*lmMt + 72*pow2(lmMsq) + 72*pow2(lmMst2)) + Dmglst2*(-1535 +
        184*lmMst2 - 18*lmMst1*(7 + 20*lmMst2) + 4*lmMsq*(-1 + 90*lmMst1 - 132*
        lmMst2 - 42*lmMt) - 54*lmMt + 168*lmMst2*lmMt + 168*pow2(lmMsq) + 360*
        pow2(lmMst2))) + (-1271 + 216*lmMst2 - 6*lmMst1*(41 + 60*lmMst2) + 12*
        lmMsq*(5 + 30*lmMst1 - 36*lmMst2 - 6*lmMt) - 30*lmMt + 72*lmMst2*lmMt +
        72*pow2(lmMsq) + 360*pow2(lmMst2))*pow2(Mst1)))/pow4(Msq))*pow6(Mst2)))
        /(14580.*pow5(Mst2)) + Mt*(-((-1 + 2*lmMst2)*s2t*shiftst3*pow2(Mst2)*(-
        4*pow2(Mt) + ((-1 - lmMst1 + lmMst2)*pow2(Mst1) + pow2(Mst2))*pow2(s2t)
        ))/(6.*pow2(Mst1)) + (T1ep*(-27*pow3(Mst2)*(3*Mst2*(46*Mst2*s2t*pow2(
        Mt) + 21*Mt*pow2(Mst2)*pow2(s2t) - 28*pow3(Mt) - 16*pow3(Mst2)*pow3(
        s2t)) + 7*Dmglst2*(-15*Mt*pow2(Mst2)*pow2(s2t) + 20*pow3(Mt) + 4*pow3(
        Mst2)*pow3(s2t))) - 9*Mst2*pow2(Mst1)*(Mst2*(2990*Mst2*s2t*pow2(Mt) +
        627*Mt*pow2(Mst2)*pow2(s2t) - 1760*pow3(Mt) - 830*pow3(Mst2)*pow3(s2t))
        + 3*Dmglst2*(-800*Mst2*s2t*pow2(Mt) - 907*Mt*pow2(Mst2)*pow2(s2t) +
        1984*pow3(Mt) + 316*pow3(Mst2)*pow3(s2t))) + (12*(11028*Dmglst2 - 5701*
        Mst2)*s2t*pow2(Mt) + 6*(11867*Dmglst2 - 1599*Mst2)*Mst2*Mt*pow2(s2t) +
        48720*pow3(Mt) + (-25328*Dmglst2 + 13305*Mst2)*pow2(Mst2)*pow3(s2t))*
        pow4(Mst1)))/(729.*pow5(Mst2)) + pow3(s2t)*(pow2(Mst1)*(
        150.95520576131688 + 2*B4 + (2*D3)/9. - (2*DN)/9. - (335*lmMsq)/18. +
        lmMst1*(94.25864197530865 - (325*lmMsq)/18. + (5*pow2(lmMsq))/6.) + (
        25*pow2(lmMsq))/6. + (5*(-149 + 18*lmMsq)*pow2(lmMst1))/27. - (lmMst2*(
        53429 - 183450*lmMst1 + 2250*lmMsq*(-7 + 6*lmMst1) + 1350*pow2(lmMsq) +
        29610*pow2(lmMst1)))/1620. + ((-3683 + 270*lmMsq + 2595*lmMst1)*pow2(
        lmMst2))/54. - (55*pow3(lmMst1))/27. - (749*pow3(lmMst2))/27.) - (-(-
        1503 + 60*lmMsq*(12 + 5*lmMst1 - 5*lmMst2) + 950*lmMst2 - 10*lmMst1*(
        167 + 210*lmMst2) + 900*pow2(lmMst1) + 1200*pow2(lmMst2))/(540.*pow2(
        Msq)) - (79.58994722092247 + (27180961*lmMst2)/793800. - ((78238 +
        33285*lmMst2)*pow2(lmMst1))/945. - (78658*pow2(lmMst2))/945. + (5*
        lmMsq*(-1 + 6*lmMst2 - 2*lmMst1*(3 + 16*lmMst2) + 16*pow2(lmMst1) + 16*
        pow2(lmMst2)))/12. + lmMst1*(-37.46379566641471 + (156896*lmMst2)/945.
         + (223*pow2(lmMst2))/
        3.) - (35*pow3(lmMst1))/27. - (1021*pow3(lmMst2))/27.)/pow2(Mst2) + (
        Dmglst2*(213.49860925479342 + (1510169*lmMst2)/119070. + lmMsq*(5 - 10*
        lmMst1 + 10*lmMst2) + ((433 + 6552*lmMst2)*pow2(lmMst1))/189. + (97*
        pow2(lmMst2))/189. - (lmMst1*(2674409 + 333900*lmMst2 + 7514640*pow2(
        lmMst2)))/119070. - (56*pow3(lmMst1))/27. + (824*pow3(lmMst2))/27.))/
        pow3(Mst2))*pow4(Mst1) - (pow2(Mst2)*(-82320*(725 - 30*lmMsq*(5 +
        lmMst1 - lmMst2) + 793*lmMst2 - lmMst1*(643 + 150*lmMst2) + 90*pow2(
        lmMst1) + 60*pow2(lmMst2))*pow2(Msq)*pow2(Mst1) + 68600*(12760 - 720*B4
        + 72*D3 - 36*DN - 1980*lmMsq + 540*pow2(lmMsq) - 3*lmMst1*(5471 - 1950*
        lmMsq + 630*pow2(lmMsq)) - 954*pow2(lmMst1) + 3*lmMst2*(8495 - 6750*
        lmMst1 + 210*lmMsq*(-11 + 6*lmMst1) + 630*pow2(lmMsq) + 234*pow2(
        lmMst1)) - 18*(-1324 + 210*lmMsq + 399*lmMst1)*pow2(lmMst2) - 288*pow3(
        lmMst1) + 6768*pow3(lmMst2))*pow4(Msq) + 3*(1186407 - 420*lmMsq*(5549 +
        6020*lmMst1 - 6860*lmMst2) - 3793580*lmMst2 + 560*lmMst1*(10936 + 9555*
        lmMst2) - 176400*pow2(lmMsq) - 1411200*pow2(lmMst1) - 4116000*pow2(
        lmMst2))*pow4(Mst1)))/(2.22264e7*pow4(Msq)) - (((2*(637 - 20*lmMsq*(9 +
        20*lmMst1 - 20*lmMst2) - 495*lmMst2 + 25*lmMst1*(27 + 16*lmMst2) - 400*
        pow2(lmMst2)))/pow2(Msq) + (5*(373 + 706*lmMst2 + 64*lmMst1*(1 +
        lmMst2) - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) + 304*pow2(lmMst2)))
        /pow2(Mst1))*pow4(Mst2))/180. + ((-343000*Dmglst2*Mst2*(337 - 24*lmMsq*
        (5 + 8*lmMst1 - 8*lmMst2) + 14*lmMst2 + 2*lmMst1*(53 + 96*lmMst2) -
        192*pow2(lmMst2)) + 3*(3653969 - 140*lmMsq*(12899 + 5390*lmMst1 - 6230*
        lmMst2) + 2069970*lmMst2 + 37730*lmMst1*(-7 + 20*lmMst2) - 58800*pow2(
        lmMsq) - 813400*pow2(lmMst2))*pow2(Mst1))*pow4(Mst2))/(7.4088e6*pow4(
        Msq)) + (54*Dmglst2*(632*OepS2 + 9*(16193 - 1422*lmMst1 + 1422*lmMst2)*
        S2)*pow2(Mst1)*pow2(Mst2) - 45*(664*OepS2 - 81*(1261 + 166*lmMst1 -
        166*lmMst2)*S2)*pow2(Mst1)*pow3(Mst2) - 15*Mst2*(3548*OepS2 - 27*(15148
        + 2661*lmMst1 - 2661*lmMst2)*S2)*pow4(Mst1) + 4*Dmglst2*(25328*OepS2 +
        27*(47051 - 18996*lmMst1 + 18996*lmMst2)*S2)*pow4(Mst1) + 54*Dmglst2*(
        56*OepS2 - 81*(-1677 + 14*lmMst1 - 14*lmMst2)*S2)*pow4(Mst2) + 1296*(-
        4*OepS2 + 81*(22 + lmMst1 - lmMst2)*S2)*pow5(Mst2))/(4374.*pow3(Mst2))
        + Dmglst2*((pow2(Mst1)*((-5*lmMsq*(7 + 6*lmMst2))/3. + 5*pow2(lmMsq) +
        (5*(-5 + 2*lmMst1 - 2*lmMst2)*pow2(Mst1))/(18.*pow2(Msq)) + (-5430043 -
        1334580*lmMst2 + 900*(859 - 3690*lmMst2)*pow2(lmMst1) - 846900*pow2(
        lmMst2) + 120*lmMst1*(44309 + 12315*lmMst2 + 56475*pow2(lmMst2)) -
        45000*pow3(lmMst1) - 3411000*pow3(lmMst2))/121500.))/Mst2 - (pow3(Mst2)
        *(-40*(-174 + 6*lmMsq*(6 + 17*lmMst1 - 17*lmMst2) + 73*lmMst2 - lmMst1*
        (109 + 102*lmMst2) + 102*pow2(lmMst2))*pow2(Msq)*pow2(Mst1) + 12*(-115
        + (430 + 64*lmMst1)*lmMst2 - 30*lmMsq*(-1 + 6*lmMst2) + 90*pow2(lmMsq)
        + 400*pow2(lmMst2))*pow4(Msq) + 5*(-119 + 12*lmMsq*(-1 + 10*lmMst1 -
        10*lmMst2) + 34*lmMst2 - 2*lmMst1*(11 + 60*lmMst2) + 120*pow2(lmMst2))*
        pow4(Mst1)))/(216.*pow2(Mst1)*pow4(Msq)) - (Mst2*(5267 - 432*B4 + 576*
        D3 - 360*DN + 8640*lmMsq + 216*lmMst1*(7 - 15*pow2(lmMsq)) - 1404*pow2(
        lmMst1) + 72*lmMst2*(-237 - 248*lmMst1 + 90*lmMsq*lmMst1 + 45*pow2(
        lmMsq) + 16*pow2(lmMst1)) - 36*(-439 + 180*lmMsq + 336*lmMst1)*pow2(
        lmMst2) + (60*(20 + 6*lmMsq*(-4 + 3*lmMst1 - 3*lmMst2) + 27*lmMst2 - 3*
        lmMst1*(1 + 6*lmMst2) + 18*pow2(lmMst2))*pow2(Mst1))/pow2(Msq) + 10944*
        pow3(lmMst2) - (45*pow4(Mst1))/pow4(Msq)))/324. + (4*(24*lmMst2*(1 +
        lmMst2) + (5*(14 - 15*lmMsq + 15*lmMst2)*pow2(Mst1))/pow2(Msq))*pow5(
        Mst2))/(27.*pow4(Mst1))) + ((5*(43 - 30*lmMsq + 30*lmMst2))/(54.*pow2(
        Msq)*pow2(Mst1)) + (-58456697 + 420*lmMsq*(63929 + 69090*lmMst1 -
        69930*lmMst2) - 374010*lmMst2 - 10290*lmMst1*(2573 + 2820*lmMst2) +
        176400*pow2(lmMsq) + 29194200*pow2(lmMst2))/(2.22264e7*pow4(Msq)) + (8*
        pow2(1 + lmMst2))/(9.*pow4(Mst1)))*pow6(Mst2))) - s2t*pow3(Mt)*(
        227.09043209876543 + (8*D3)/9. - (8*DN)/9. - (50*lmMsq)/3. + (32*lmMt)/
        3. + 10*pow2(lmMsq) + (2*lmMst1*(667 - 300*lmMsq + 45*pow2(lmMsq)))/27.
         + (2*lmMst2*(1946 + lmMsq*(30 - 90*lmMst1) + 294*lmMst1 - 45*pow2(
        lmMsq) - 117*pow2(lmMst1)))/27. + (124*pow2(lmMst1))/9. + (2*(152 + 30*
        lmMsq + 31*lmMst1)*pow2(lmMst2))/9. - (2*(9608 - 15*lmMsq*(218 + 195*
        lmMst1 - 75*lmMst2) - 5250*lmMst2 + 15*lmMst1*(568 + 375*lmMst2) + 900*
        pow2(lmMsq) - 1350*pow2(lmMst1) - 3375*pow2(lmMst2))*pow2(Mst1))/(2025.
        *pow2(Msq)) - (((2*(6517 - 15*lmMsq*(532 + 75*lmMst1 - 195*lmMst2) +
        4980*lmMst2 + 375*lmMst1*(8 + 3*lmMst2) - 900*pow2(lmMsq) - 2025*pow2(
        lmMst2)))/pow2(Msq) + (225*(277 + 514*lmMst2 + 64*lmMst1*(1 + lmMst2) -
        30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) + 208*pow2(lmMst2)))/pow2(
        Mst1))*pow2(Mst2))/2025. + (32*pow3(lmMst1))/9. - (16*pow3(lmMst2))/9.
         + (pow2(Mst1)*(19319827 + 19042320*lmMst2 + 518400*lmMt + 1036800*
        lmMst2*lmMt - 3600*(701 + 186*lmMst2)*pow2(lmMst1) + 5943600*pow2(
        lmMst2) + 324000*lmMsq*(-2 + lmMst1 - lmMst2 - 2*lmMst1*lmMst2 + pow2(
        lmMst1) + pow2(lmMst2)) - 240*lmMst1*(35693 + 5610*lmMst2 + 4320*lmMt +
        3690*pow2(lmMst2)) + (54000*(-4 + lmMst1*(2 - 6*lmMst2) - 2*lmMst2 + 3*
        pow2(lmMst1) + 3*pow2(lmMst2))*pow2(Mst1))/pow2(Msq) - 295200*pow3(
        lmMst1) + 1850400*pow3(lmMst2)))/(48600.*pow2(Mst2)) - (((68600*(-43 +
        30*lmMsq - 30*lmMst2))/(pow2(Msq)*pow2(Mst1)) + (393767 - 420*lmMsq*(
        2194 + 49*lmMst1 - 259*lmMst2) + 878948*lmMst2 + 1372*lmMst1*(31 + 15*
        lmMst2) - 44100*pow2(lmMsq) - 64680*pow2(lmMst2))/pow4(Msq) - (658560*
        pow2(1 + lmMst2))/pow4(Mst1))*pow4(Mst2))/185220. - pow4(Mst1)*((309383
        - 420*lmMsq*(256 + 259*lmMst1 - 49*lmMst2) - 145432*lmMst2 + 28*lmMst1*
        (9034 + 13965*lmMst2) + 44100*pow2(lmMsq) - 141120*pow2(lmMst1) -
        205800*pow2(lmMst2))/(185220.*pow4(Msq)) - (594.0859288013853 - (40*
        lmMsq)/3. + (-480.17666414714034 + 20*lmMsq)*lmMst1 + lmMst2*(
        707.7322197026959 - (69154*lmMst1)/945. - (20*lmMsq*(3 + 2*lmMst1))/3.
         - (410*pow2(lmMst1))/
        9.) + ((-94783 + 6300*lmMsq)*pow2(lmMst1))/945. + (216.14497354497354 +
        (20*lmMsq)/3. - (166*lmMst1)/9.)*pow2(lmMst2) + (32*lmMt*(1 + 4*lmMst2
        - 2*lmMst1*(2 + lmMst2) + pow2(lmMst1) + pow2(lmMst2)))/3. + (26*pow3(
        lmMst1))/27. + (1702*pow3(lmMst2))/27.)/pow4(Mst2) - (Dmglst2*(
        386.22610253272387 - (32*B4)/9. + (32*D3)/9. - (16*DN)/9. + (280*lmMsq)
        /3. + (756.6053917863442 - 80*lmMsq)*lmMst1 + (19352*pow2(lmMst1))/189.
         + lmMst2*(196.28349710254471 + 80*lmMsq - (57520*lmMst1)/189. + (88*
        pow2(lmMst1))/9.) + (8*(6787 - 5439*lmMst1)*pow2(lmMst2))/189. - (64*
        lmMt*(4 + 6*lmMst2 - 2*lmMst1*(3 + lmMst2) + pow2(lmMst1) + pow2(
        lmMst2)))/3. - (8*pow3(lmMst1))/9. + (664*pow3(lmMst2))/3.))/pow5(Mst2)
        ) + ((-4*OepS2*(4485*pow2(Mst1)*pow3(Mst2) + 11402*Mst2*pow4(Mst1) -
        24*Dmglst2*(150*pow2(Mst1)*pow2(Mst2) + 919*pow4(Mst1)) + 621*pow5(
        Mst2)))/729. + (S2*(9*(11113 + 14950*lmMst1 - 14950*lmMst2)*pow2(Mst1)*
        pow3(Mst2) + 4*(126661 + 85515*lmMst1 - 85515*lmMst2)*Mst2*pow4(Mst1) -
        24*Dmglst2*(45*(139 + 100*lmMst1 - 100*lmMst2)*pow2(Mst1)*pow2(Mst2) +
        (34538 + 27570*lmMst1 - 27570*lmMst2)*pow4(Mst1) + 5697*pow4(Mst2)) +
        81*(-891 + 230*lmMst1 - 230*lmMst2)*pow5(Mst2)))/270.)/pow5(Mst2) + ((
        pow2(Mst2)*(20*Dmglst2*(-43 + 144*lmMsq - 144*lmMst2)*Mst2*pow2(Mst1) +
        5*(18*Dmglst2*(13 - 28*lmMsq + 28*lmMst2) + (67 - 84*lmMsq + 84*lmMst2)
        *Mst2)*pow3(Mst2) + (75 + 60*lmMsq*(-3 + 2*lmMst1 - 2*lmMst2) + 356*
        lmMst2 - 8*lmMst1*(22 + 15*lmMst2) + 120*pow2(lmMst2))*pow4(Mst1)))/(
        108.*pow2(Mst1)) - (Dmglst2*(-4*pow4(Msq)*(450*pow2(Mst2)*(15529 - 120*
        B4 + 120*D3 - 60*DN + 150*lmMsq + 690*lmMst1 + 675*pow2(lmMsq) + 345*
        pow2(lmMst1) - 5*lmMst2*(-3091 + 270*lmMsq + 42*lmMst1 + 96*pow2(
        lmMst1)) + (4305 - 480*lmMst1)*pow2(lmMst2) + 960*pow3(lmMst2))*pow4(
        Mst1) - 3375*(-179 + 2*(87 + 32*lmMst1)*lmMst2 - 30*lmMsq*(-1 + 6*
        lmMst2) + 90*pow2(lmMsq) + 208*pow2(lmMst2))*pow2(Mst1)*pow4(Mst2) + (
        9080791 - 54000*B4 + 54000*D3 - 27000*DN + 607500*lmMsq - 60*(-42359 +
        3375*lmMsq)*lmMst1 + 324000*(-2 + lmMst1 - lmMst2)*lmMt + 60*lmMst2*(
        143041 + 3375*lmMsq - 14460*lmMst1 - 3375*pow2(lmMst1)) + 1800*pow2(
        lmMst1) - 900*(-2402 + 1695*lmMst1)*pow2(lmMst2) - 4500*pow3(lmMst1) +
        1732500*pow3(lmMst2))*pow6(Mst1) + 216000*lmMst2*(1 + lmMst2)*pow6(
        Mst2)) - 45000*pow2(Msq)*(3*(-17 + 20*lmMsq - 20*lmMst2)*pow4(Mst1)*
        pow4(Mst2) - 6*pow2(Mst2)*pow6(Mst1) + 4*(14 - 15*lmMsq + 15*lmMst2)*
        pow2(Mst1)*pow6(Mst2) + 6*(3 - lmMst1 + lmMst2)*pow8(Mst1)) + 16875*((7
        + 12*lmMsq - 12*lmMst2)*pow4(Mst2)*pow6(Mst1) + 4*pow2(Mst2)*pow8(Mst1)
        )))/(60750.*pow3(Mst2)*pow4(Mst1)))/pow4(Msq))) + (Al4p*Mt*twoLoopFlag*
        (12*Mt*pow2(s2t)*(2*(Mst2*(4 + 3*lmMst2 - lmMst1*(1 + lmMst2) + pow2(
        lmMst2)) + ((Dmglst2*(5 + lmMst1*(3 - 2*lmMst2) - 3*lmMst2 + 2*pow2(
        lmMst2)) + Mst2*(1 + 3*lmMst2 - lmMst1*(3 + 2*lmMst2) + 2*pow2(lmMst2))
        )*pow2(Mst1))/pow2(Mst2)) + ((0.5 + 7*lmMst2 - lmMst1*(7 + 4*lmMst2) +
        4*pow2(lmMst2))*pow4(Mst1))/pow3(Mst2) + Dmglst2*(12 - 2*lmMst1*lmMst2
        + 2*(lmMst1 + lmMst2 + pow2(lmMst2)) + ((10.5 + lmMst1*(9 - 4*lmMst2) -
        9*lmMst2 + 4*pow2(lmMst2))*pow4(Mst1))/pow4(Mst2))) + 4*s2t*pow2(Mt)*(
        12*lmMst1 + 4*(-2 + lmMst1)*lmMst2 + 2*pow2(lmMst1) - 6*pow2(lmMst2) -
        (4*(1 + lmMst2)*pow2(Mst2))/pow2(Mst1) + (Dmglst2*(4 + lmMst2*(8 - (8*
        pow2(Mst2))/pow2(Mst1))))/Mst2 + (4*(1 - lmMst1 + lmMst2)*(2*Dmglst2 +
        (lmMst1 - lmMst2)*Mst2)*pow2(Mst1))/pow3(Mst2) - (4*(Dmglst2*(-2 + 4*
        lmMst1 - 4*lmMst2) + Mst2*(-lmMst1 + lmMst2 - 2*lmMst1*lmMst2 + pow2(
        lmMst1) + pow2(lmMst2)))*pow4(Mst1))/pow5(Mst2)) - (16*pow3(Mt)*(
        Dmglst2*(2*(-7 + lmMst2*(-7 + lmMt) - lmMst1*(-5 + lmMt) + 2*lmMt)*
        pow2(Mst1)*pow2(Mst2) + (-11 + lmMst1*(32 + 6*lmMst2 - 8*lmMt) + 8*
        lmMst2*(-5 + lmMt) + 8*lmMt - 6*pow2(lmMst2))*pow4(Mst1) - 2*(5 +
        lmMst1*(-1 + lmMst2) + lmMst2 - pow2(lmMst2))*pow4(Mst2)) - Mst2*(2*(4
        + lmMst1*(3 + 2*lmMst2 - lmMt) + lmMst2*(-4 + lmMt) + lmMt - 2*pow2(
        lmMst2))*pow2(Mst1)*pow2(Mst2) + (13 + 2*lmMst1*(6 + 3*lmMst2 - 2*lmMt)
        + 2*lmMt + 2*lmMst2*(-7 + 2*lmMt) - 6*pow2(lmMst2))*pow4(Mst1) + 2*(2 +
        lmMst1 - 2*lmMst2 + lmMst1*lmMst2 + lmMt - pow2(lmMst2))*pow4(Mst2))))/
        pow6(Mst2) + ((pow3(s2t)*(2*(-16 + 6*lmMst2 - 2*lmMst1*(8 + 5*lmMst2) +
        3*pow2(lmMst1) + 7*pow2(lmMst2))*pow3(Mst2)*pow4(Mst1) - 2*(-12 - 18*
        lmMst2 + 2*lmMst1*(5 + 3*lmMst2) + pow2(lmMst1) - 7*pow2(lmMst2))*pow2(
        Mst1)*pow5(Mst2) + Mst2*(3 + lmMst1*(2 - 32*lmMst2) - 2*lmMst2 + 16*
        pow2(lmMst1) + 16*pow2(lmMst2))*pow6(Mst1) - 4*Dmglst2*(2*(1 + 2*lmMst1
        - lmMst2)*pow2(Mst2)*pow4(Mst1) + 2*(1 + lmMst1 - lmMst2 + 2*lmMst1*
        lmMst2 - 2*pow2(lmMst2))*pow2(Mst1)*pow4(Mst2) + (1 + 2*lmMst1 - 2*
        lmMst2)*pow6(Mst1) - 2*lmMst2*pow6(Mst2)) + 4*(1 + lmMst2)*pow7(Mst2)))
        /pow3(Mst2) - (2*Mt*MuSUSY*s2t*(4*(4*Mt*(5 + 6*lmMst2 - lmMst1*(4 + 3*
        lmMst2) + 3*pow2(lmMst2)) + Mst2*s2t*(-1 + 13*lmMst2 - lmMst1*(13 + 8*
        lmMst2) + pow2(lmMst1) + 7*pow2(lmMst2)))*pow3(Mst2)*pow4(Mst1) + 2*(-(
        Mst2*s2t*(-14 - 20*lmMst2 + 2*lmMst1*(5 + 3*lmMst2) + pow2(lmMst1) - 7*
        pow2(lmMst2))) + 8*Mt*(4 + 3*lmMst2 - lmMst1*(1 + lmMst2) + pow2(
        lmMst2)))*pow2(Mst1)*pow5(Mst2) + 4*Dmglst2*(Mst2*s2t*(-5 + 8*lmMst2 -
        4*lmMst1*(2 + lmMst2) + 4*pow2(lmMst2)) + Mt*(65 + lmMst1*(34 - 20*
        lmMst2) - 26*lmMst2 + 20*pow2(lmMst2)))*pow6(Mst1) + Mst2*(Mst2*s2t*(-1
        + 50*lmMst2 - 2*lmMst1*(25 + 32*lmMst2) + 20*pow2(lmMst1) + 44*pow2(
        lmMst2)) + Mt*(84 + 152*lmMst2 - 40*lmMst1*(3 + 2*lmMst2) + 80*pow2(
        lmMst2)))*pow6(Mst1) + 8*Dmglst2*((Mst2*s2t*(-2 + 3*lmMst2 - lmMst1*(3
        + 2*lmMst2) + 2*pow2(lmMst2)) + Mt*(22 + lmMst1*(8 - 6*lmMst2) - 4*
        lmMst2 + 6*pow2(lmMst2)))*pow2(Mst2)*pow4(Mst1) + (-(Mst2*s2t*(1 +
        lmMst1 - 2*lmMst2 + 2*lmMst1*lmMst2 - 2*pow2(lmMst2))) + 2*Mt*(6 +
        lmMst1 + lmMst2 - lmMst1*lmMst2 + pow2(lmMst2)))*pow2(Mst1)*pow4(Mst2)
        + lmMst2*s2t*pow7(Mst2)) + 4*(1 + lmMst2)*s2t*pow8(Mst2)))/(Tbeta*pow6(
        Mst2)))/pow2(Mst1)))/12. - (Al4p*Mt*xDR2DRMOD*(-(xDmglst2*pow2(Dmglst2)
        *(432*(2 - lmMst2)*s2t*twoLoopFlag*pow2(Mst1)*(-((-4*pow2(Mst1)*pow2(
        Mt) + 4*pow2(Mst2)*pow2(Mt) + 2*(lmMst1 - lmMst2)*pow2(Mst1)*pow2(Mst2)
        *pow2(s2t) + pow2(s2t)*(pow4(Mst1) - pow4(Mst2)))*pow4(Mst2)) - (2*Mt*
        MuSUSY*s2t*((pow2(Mst1) + pow2(Mst2))*pow4(Mst2) - 2*(lmMst1 - lmMst2)*
        pow2(Mst1)*(pow2(Mst1)*pow2(Mst2) + pow4(Mst1) + pow4(Mst2))))/Tbeta) +
        Al4p*Mst2*threeLoopFlag*(256*Mt*pow2(Mst1)*(-9*pow2(Mst2)*pow2(s2t)*(
        pow2(Mst1)*pow2(Mst2)*(13 - 125*lmMst2 + 6*lmMst1*(7 + 8*lmMst2 - 6*
        pow2(lmMst2)) - 30*pow2(lmMst2) + 36*pow3(lmMst2)) + 6*(15 - 11*lmMst2
        + lmMst1*(4 + 7*lmMst2 - 6*pow2(lmMst2)) - 7*pow2(lmMst2) + 6*pow3(
        lmMst2))*pow4(Mst1) + 6*(-3 - 4*lmMst2 + 3*pow2(lmMst2))*pow4(Mst2)) +
        4*pow2(Mt)*((215 + 37*lmMst2 + 3*(-5 + 8*lmMst2)*lmMt - 18*lmMst1*(1 -
        2*lmMst2 + lmMt) - 42*pow2(lmMst2))*pow2(Mst1)*pow2(Mst2) + (1097 +
        lmMst2*(1531 - 246*lmMt) - 339*lmMt + (444 - 36*lmMt)*pow2(lmMst2) -
        18*lmMst1*(39 - 2*lmMst2*(-9 + lmMt) - 7*lmMt + 2*pow2(lmMst2)) + 36*
        pow3(lmMst2))*pow4(Mst1) + 18*(-3 - 4*lmMst2 + 3*pow2(lmMst2))*pow4(
        Mst2))) + (s2t*(-3*pow2(s2t)*pow3(Mst2)*(pow2(Mst1)*(2*(lmMst1 -
        lmMst2)*pow2(Mst1)*pow2(Mst2) + pow4(Mst1) - pow4(Mst2))*(480*(4 - 15*
        lmMsq + 15*lmMst2)*pow2(Msq)*pow2(Mst2) + 45*(9 - 140*lmMsq + 140*
        lmMst2)*pow4(Mst2)) - 4*pow4(Msq)*(2*pow2(Mst2)*(400 + lmMst2*(2617 -
        1170*lmMsq - 270*pow2(lmMsq)) - 96*(-2 + lmMst2)*pow2(lmMst1) + 54*(33
        + 10*lmMsq)*pow2(lmMst2) + 3*lmMst1*(-1059 - 370*lmMst2 - 30*lmMsq*(-13
        + 6*lmMst2) + 90*pow2(lmMsq) + 336*pow2(lmMst2)) - 912*pow3(lmMst2))*
        pow4(Mst1) + 3*(963 - 64*lmMst1*(-2 + lmMst2) + 530*lmMst2 + 30*lmMsq*(
        -13 + 6*lmMst2) - 90*pow2(lmMsq) - 784*pow2(lmMst2))*pow2(Mst1)*pow4(
        Mst2) + (727 + 90*lmMsq*(13 - 6*lmMst2) + 2410*lmMst2 + 270*pow2(lmMsq)
        - 1536*(-2 + lmMst2)*pow2(lmMst1) + 3504*pow2(lmMst2) + 192*lmMst1*(-13
        - 33*lmMst2 + 16*pow2(lmMst2)) - 1536*pow3(lmMst2))*pow6(Mst1) - 192*(2
        + lmMst2 - 3*pow2(lmMst2))*pow6(Mst2))) + 12*Mst2*pow2(Mt)*(pow2(Mst1)*
        (pow2(Mst1) - pow2(Mst2))*(480*(4 - 15*lmMsq + 15*lmMst2)*pow2(Msq)*
        pow4(Mst2) + 45*(9 - 140*lmMsq + 140*lmMst2)*pow6(Mst2)) + 4*pow4(Msq)*
        (pow2(Mst2)*(2153 + 1782*lmMst2 + 90*lmMsq*(-13 + 6*lmMst2) - 270*pow2(
        lmMsq) + 192*(-2 + lmMst2)*pow2(lmMst1) - 432*pow2(lmMst2) + 192*(
        lmMst1 + 3*lmMst1*lmMst2 - 2*lmMst1*pow2(lmMst2) + pow3(lmMst2)))*pow4(
        Mst1) + 3*(-515 + 64*lmMst1*(-2 + lmMst2) - 530*lmMst2 - 30*lmMsq*(-13
        + 6*lmMst2) + 90*pow2(lmMsq) + 208*pow2(lmMst2))*pow2(Mst1)*pow4(Mst2)
        + 192*((26 + 29*lmMst2 + (-2 + lmMst2)*pow2(lmMst1) + 4*pow2(lmMst2) -
        2*lmMst1*(8 + lmMst2 + pow2(lmMst2)) + pow3(lmMst2))*pow6(Mst1) + (2 +
        lmMst2 - 3*pow2(lmMst2))*pow6(Mst2)))) + (6*Mt*MuSUSY*(s2t*pow2(Mst1)*(
        2*(lmMst1 - lmMst2)*pow2(Mst1) - pow2(Mst2))*(pow2(Mst1) + pow2(Mst2))*
        (480*(4 - 15*lmMsq + 15*lmMst2)*pow2(Msq)*pow3(Mst2) + 45*(9 - 140*
        lmMsq + 140*lmMst2)*pow5(Mst2)) + 4*pow4(Msq)*(-(pow2(Mst2)*(Mst2*s2t*(
        3305 + 6632*lmMst2 - 90*lmMsq*(13 + 3*lmMsq - 6*lmMst2)*(1 - 2*lmMst1 +
        2*lmMst2) - 192*(-2 + lmMst2)*pow2(lmMst1) + 1788*pow2(lmMst2) + 6*
        lmMst1*(-995 - 402*lmMst2 + 336*pow2(lmMst2)) - 1824*pow3(lmMst2)) +
        128*Mt*(5 + 149*lmMst2 + 12*pow2(lmMst2) + 6*lmMst1*(-7 - 8*lmMst2 + 6*
        pow2(lmMst2)) - 36*pow3(lmMst2)))*pow4(Mst1)) - 3*(Mst2*s2t*(835 - 64*
        lmMst1*(-2 + lmMst2) + 466*lmMst2 + 30*lmMsq*(-13 + 6*lmMst2) - 90*
        pow2(lmMsq) - 592*pow2(lmMst2)) + 256*Mt*(3 + 4*lmMst2 - 3*pow2(lmMst2)
        ))*pow2(Mst1)*pow4(Mst2) + 2*(-3*Mst2*s2t*(672 + lmMst2*(1507 - 390*
        lmMsq - 90*pow2(lmMsq)) - 288*(-2 + lmMst2)*pow2(lmMst1) + 18*(49 + 10*
        lmMsq)*pow2(lmMst2) + lmMst1*(-1411 - 1458*lmMst2 - 30*lmMsq*(-13 + 6*
        lmMst2) + 90*pow2(lmMsq) + 848*pow2(lmMst2)) - 560*pow3(lmMst2)) + 64*
        Mt*(85 - 215*lmMst2 + lmMst1*(66 + 90*lmMst2 - 72*pow2(lmMst2)) - 54*
        pow2(lmMst2) + 72*pow3(lmMst2)))*pow6(Mst1) + 192*s2t*(2 + lmMst2 - 3*
        pow2(lmMst2))*pow7(Mst2))))/Tbeta))/pow4(Msq)))) + Mst2*((432*(2*
        Dmglst2*lmMst2 + Mst2 + lmMst2*Mst2)*s2t*twoLoopFlag*pow2(Mst1)*(-((2*
        Mt*(MuSUSY*s2t - 2*Mt*Tbeta)*pow2(Mst1) + pow2(Mst2)*(2*Mt*MuSUSY*s2t +
        4*Tbeta*pow2(Mt) - Tbeta*pow2(Mst2)*pow2(s2t)) + Tbeta*pow2(s2t)*pow4(
        Mst1))*pow4(Mst2)) + 2*(lmMst1 - lmMst2)*s2t*pow2(Mst1)*(2*Mt*MuSUSY*(
        pow2(Mst1)*pow2(Mst2) + pow4(Mst1) + pow4(Mst2)) - s2t*Tbeta*pow6(Mst2)
        )))/Tbeta + Al4p*threeLoopFlag*(256*Mt*pow2(Mst1)*(4*pow2(Mt)*(pow2(
        Mst1)*(-9*(1 + lmMst2)*Mst2*(1 + lmMst2*(7 - 2*lmMt) - 2*lmMst1*(2 +
        lmMst2 - lmMt) - 3*lmMt + 2*pow2(lmMst2)) + Dmglst2*(119 + lmMst2*(224
        - 15*lmMt) - 51*lmMt + 3*(5 + 6*lmMt)*pow2(lmMst2) + 18*lmMst1*(-4 +
        lmMt - lmMst2*(1 + lmMt) + pow2(lmMst2)) - 18*pow3(lmMst2)))*pow3(Mst2)
        + Mst2*(9*(1 + lmMst2)*Mst2*(3 + 2*lmMst1*(8 + 3*lmMst2 - 3*lmMt) + 5*
        lmMt + 3*lmMst2*(-7 + 2*lmMt) - 6*pow2(lmMst2)) + Dmglst2*(263 +
        lmMst2*(962 - 213*lmMt) - 177*lmMt + (393 - 18*lmMt)*pow2(lmMst2) - 18*
        lmMst1*(26 - lmMst2*(-17 + lmMt) - 7*lmMt + pow2(lmMst2)) + 18*pow3(
        lmMst2)))*pow4(Mst1) + (1 + lmMst2)*(-18*(-Dmglst2 + 3*Dmglst2*lmMst2 +
        Mst2 + lmMst2*Mst2)*pow5(Mst2) + 9*(12 - 45*lmMst2 + 2*lmMst1*(19 + 6*
        lmMst2 - 6*lmMt) + 7*lmMt + 12*lmMst2*lmMt - 12*pow2(lmMst2))*pow6(
        Mst1))) - 27*Mst2*pow2(s2t)*((1 + lmMst2)*Mst2*(2*(2 - 5*lmMst2 +
        lmMst1*(5 + 2*lmMst2) - 2*pow2(lmMst2))*pow2(Mst2)*pow4(Mst1) + 2*(-2*
        lmMst2*(2 + lmMst2) + lmMst1*(3 + 2*lmMst2))*pow2(Mst1)*pow4(Mst2) + (5
        - 12*lmMst2 + 4*lmMst1*(3 + lmMst2) - 4*pow2(lmMst2))*pow6(Mst1) - 2*(1
        + lmMst2)*pow6(Mst2)) + Dmglst2*(2*pow2(Mst2)*(8 + 19*lmMst2 - 5*pow2(
        lmMst2) + lmMst1*(-7 + 5*lmMst2 + 6*pow2(lmMst2)) - 6*pow3(lmMst2))*
        pow4(Mst1) + 2*pow2(Mst1)*(7 + 9*lmMst2 - 8*pow2(lmMst2) + lmMst1*(-3 +
        5*lmMst2 + 6*pow2(lmMst2)) - 6*pow3(lmMst2))*pow4(Mst2) + (17 + 51*
        lmMst2 - 4*pow2(lmMst2) + 4*lmMst1*(-6 + lmMst2 + 3*pow2(lmMst2)) - 12*
        pow3(lmMst2))*pow6(Mst1) - 2*(-1 + 2*lmMst2 + 3*pow2(lmMst2))*pow6(
        Mst2)))) + (6*Mt*MuSUSY*s2t*(1536*Mt*pow2(Mst1)*(2*(pow2(Mst1)*((1 +
        lmMst2)*Mst2*(-1 - 5*lmMst2 + lmMst1*(3 + 2*lmMst2) - 2*pow2(lmMst2)) +
        Dmglst2*(8 + 7*lmMst2 - 11*pow2(lmMst2) + lmMst1*(-3 + 5*lmMst2 + 6*
        pow2(lmMst2)) - 6*pow3(lmMst2)))*pow3(Mst2) + Mst2*((1 + lmMst2)*Mst2*(
        1 - 10*lmMst2 + 4*lmMst1*(2 + lmMst2) - 4*pow2(lmMst2)) + 2*Dmglst2*(8
        + 13*lmMst2 - 8*pow2(lmMst2) + lmMst1*(-5 + 5*lmMst2 + 6*pow2(lmMst2))
        - 6*pow3(lmMst2)))*pow4(Mst1)) + (1 + lmMst2)*(-2*(-Dmglst2 + 3*
        Dmglst2*lmMst2 + Mst2 + lmMst2*Mst2)*pow5(Mst2) + (7 - 32*lmMst2 + 4*
        lmMst1*(7 + 3*lmMst2) - 12*pow2(lmMst2))*pow6(Mst1))) - (s2t*(pow2(
        Mst1)*((pow2(Mst1) + pow2(Mst2))*pow4(Mst2) - 2*(lmMst1 - lmMst2)*pow2(
        Mst1)*(pow2(Mst1)*pow2(Mst2) + pow4(Mst1) + pow4(Mst2)))*(40*(-43 + 30*
        lmMsq - 30*lmMst2)*pow2(Msq)*pow3(Mst2) + 5*(-67 + 84*lmMsq - 84*
        lmMst2)*pow5(Mst2)) + 12*Mst2*pow4(Msq)*((303 + 1452*lmMst2 + 30*lmMsq*
        (-1 + 2*lmMst1 - 2*lmMst2)*(5 - 3*lmMsq + 6*lmMst2) + 64*(1 + lmMst2)*
        pow2(lmMst1) + 1804*pow2(lmMst2) - 2*lmMst1*(431 + 814*lmMst2 + 336*
        pow2(lmMst2)) + 608*pow3(lmMst2))*pow4(Mst1)*pow4(Mst2) + (335 + 654*
        lmMst2 + 64*lmMst1*(1 + lmMst2) - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(
        lmMsq) + 272*pow2(lmMst2))*pow2(Mst1)*pow6(Mst2) + 2*(pow2(Mst2)*(64 +
        15*lmMst2*(33 - 10*lmMsq + 6*pow2(lmMsq)) + 288*(1 + lmMst2)*pow2(
        lmMst1) + 6*(173 - 30*lmMsq)*pow2(lmMst2) - lmMst1*(431 + 1326*lmMst2 -
        30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) + 848*pow2(lmMst2)) + 560*
        pow3(lmMst2))*pow6(Mst1) + (80 + lmMst2*(479 - 150*lmMsq + 90*pow2(
        lmMsq)) + 544*(1 + lmMst2)*pow2(lmMst1) + 2*(631 - 90*lmMsq)*pow2(
        lmMst2) - lmMst1*(399 + 1806*lmMst2 - 30*lmMsq*(5 + 6*lmMst2) + 90*
        pow2(lmMsq) + 1360*pow2(lmMst2)) + 816*pow3(lmMst2))*pow8(Mst1)) - 32*
        pow2(1 + lmMst2)*pow8(Mst2)) + 2*Dmglst2*(pow2(Mst1)*(160*(-14 + 15*
        lmMsq - 15*lmMst2)*pow2(Msq)*pow2(Mst2) + 45*(-13 + 28*lmMsq - 28*
        lmMst2)*pow4(Mst2))*((pow2(Mst1) + pow2(Mst2))*pow4(Mst2) - 2*(lmMst1 -
        lmMst2)*pow2(Mst1)*(pow2(Mst1)*pow2(Mst2) + pow4(Mst1) + pow4(Mst2))) +
        12*pow4(Msq)*((45 + 30*lmMsq*(1 + 3*lmMsq - 6*lmMst2)*(1 - 2*lmMst1 +
        2*lmMst2) + 8*lmMst2*(29 + 8*pow2(lmMst1)) + 1068*pow2(lmMst2) - 2*
        lmMst1*(-83 + 430*lmMst2 + 336*pow2(lmMst2)) + 608*pow3(lmMst2))*pow4(
        Mst1)*pow4(Mst2) + (-115 + (366 + 64*lmMst1)*lmMst2 - 30*lmMsq*(-1 + 6*
        lmMst2) + 90*pow2(lmMsq) + 336*pow2(lmMst2))*pow2(Mst1)*pow6(Mst2) + 2*
        (pow2(Mst2)*(96 + lmMst2*(173 + 30*lmMsq + 90*pow2(lmMsq) + 288*pow2(
        lmMst1)) + (526 - 180*lmMsq)*pow2(lmMst2) - lmMst1*(13 + 30*lmMsq*(1 -
        6*lmMst2) + 526*lmMst2 + 90*pow2(lmMsq) + 848*pow2(lmMst2)) + 560*pow3(
        lmMst2))*pow6(Mst1) + (96 + lmMst2*(285 + 30*lmMsq + 90*pow2(lmMsq) +
        544*pow2(lmMst1)) + (590 - 180*lmMsq)*pow2(lmMst2) - lmMst1*(109 + 30*
        lmMsq*(1 - 6*lmMst2) + 590*lmMst2 + 90*pow2(lmMsq) + 1360*pow2(lmMst2))
        + 816*pow3(lmMst2))*pow8(Mst1)) - 64*lmMst2*(1 + lmMst2)*pow8(Mst2)))))
        /pow4(Msq)))/Tbeta + (3*pow2(Mst2)*pow3(s2t)*(40*(43 - 30*lmMsq + 30*
        lmMst2)*pow2(Msq)*pow2(Mst1)*(2*(lmMst1 - lmMst2)*pow2(Mst1)*pow2(Mst2)
        + pow4(Mst1) - pow4(Mst2))*pow5(Mst2) + 5*(67 - 84*lmMsq + 84*lmMst2)*(
        pow2(Mst1) + 2*(lmMst1 - lmMst2)*pow2(Mst2))*pow4(Mst1)*pow7(Mst2) +
        12*Mst2*pow4(Msq)*(-2*(16 - 3*lmMst2*(133 - 50*lmMsq + 30*pow2(lmMsq))
        - 32*(1 + lmMst2)*pow2(lmMst1) + 2*(-383 + 90*lmMsq)*pow2(lmMst2) +
        lmMst1*(463 + 846*lmMst2 - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) +
        336*pow2(lmMst2)) - 304*pow3(lmMst2))*pow4(Mst1)*pow4(Mst2) - pow2(
        Mst2)*(175 - 30*lmMsq*(5 + 6*lmMst2) + lmMst2*(462 + 1024*lmMst1*(1 +
        lmMst2)) + 90*pow2(lmMsq) - 272*pow2(lmMst2) - 512*((1 + lmMst2)*pow2(
        lmMst1) + pow3(lmMst2)))*pow6(Mst1) + (367 + 718*lmMst2 + 64*lmMst1*(1
        + lmMst2) - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(lmMsq) + 304*pow2(lmMst2)
        )*pow2(Mst1)*pow6(Mst2) + 32*(1 + lmMst2)*(1 + lmMst1*(2 - 32*lmMst2) -
        2*lmMst2 + 16*(pow2(lmMst1) + pow2(lmMst2)))*pow8(Mst1) - 32*pow2(1 +
        lmMst2)*pow8(Mst2)) + 2*Dmglst2*(384*lmMst2*pow4(Msq)*pow4(Mst1)*(2*
        pow2(lmMst1)*(8*pow2(Mst1)*pow2(Mst2) + 8*pow4(Mst1) + pow4(Mst2)) +
        pow2(lmMst2)*(16*pow2(Mst1)*(pow2(Mst1) + pow2(Mst2)) + 19*pow4(Mst2)))
        + pow2(Mst1)*pow2(Mst2)*(45*(13 - 28*lmMsq)*pow4(Mst1)*pow4(Mst2) +
        160*(-14 + 15*lmMsq)*pow2(Msq)*pow2(Mst2)*(-pow4(Mst1) + pow4(Mst2)) +
        12*pow4(Msq)*(160*pow2(Mst1)*pow2(Mst2) - 3*(-49 + 10*lmMsq + 30*pow2(
        lmMsq))*pow4(Mst1) + 5*(-23 + 6*lmMsq + 18*pow2(lmMsq))*pow4(Mst2))) -
        24*pow2(lmMst2)*(200*pow2(Msq)*pow4(Mst1)*pow6(Mst2) + 105*pow4(Mst1)*
        pow8(Mst2) + pow4(Msq)*(6*(-61 + 30*lmMsq)*pow4(Mst1)*pow4(Mst2) + 8*
        pow2(Mst2)*pow6(Mst1) - 200*pow2(Mst1)*pow6(Mst2) - 64*pow8(Mst1) + 32*
        pow8(Mst2))) + 2*(45*lmMst2*(14*pow2(Mst1) + (-13 + 28*lmMsq)*pow2(
        Mst2))*pow4(Mst1)*pow6(Mst2) - 12*lmMst1*pow4(Msq)*((-83 + 462*lmMst2 -
        30*lmMsq*(-1 + 6*lmMst2) + 90*pow2(lmMsq) + 336*pow2(lmMst2))*pow4(
        Mst1)*pow4(Mst2) + 32*(3 + 3*lmMst2 + 16*pow2(lmMst2))*pow2(Mst2)*pow6(
        Mst1) - 32*lmMst2*pow2(Mst1)*pow6(Mst2) + 32*(3 + 2*lmMst2 + 16*pow2(
        lmMst2))*pow8(Mst1)) + 12*lmMst2*pow4(Msq)*((-67 + 30*lmMsq + 90*pow2(
        lmMsq))*pow4(Mst1)*pow4(Mst2) + 3*(19 + 30*lmMsq)*pow2(Mst2)*pow6(Mst1)
        + 5*(43 - 18*lmMsq)*pow2(Mst1)*pow6(Mst2) + 112*pow8(Mst1) - 32*pow8(
        Mst2)) + lmMst1*pow4(Mst1)*(160*(14 - 15*lmMsq + 15*lmMst2)*pow2(Msq)*
        pow6(Mst2) + 45*(13 - 28*lmMsq + 28*lmMst2)*pow8(Mst2)) + 80*lmMst2*
        pow2(Msq)*(15*pow4(Mst2)*pow6(Mst1) + 2*(-14 + 15*lmMsq)*pow4(Mst1)*
        pow6(Mst2) - 15*pow2(Mst1)*pow8(Mst2))))) - 12*s2t*pow2(Mt)*(12*Mst2*
        pow4(Msq)*(-((143 + 302*lmMst2 - 30*lmMsq*(5 + 6*lmMst2) + 90*pow2(
        lmMsq) + 48*pow2(lmMst2) + 128*lmMst1*pow2(1 + lmMst2) - 64*((1 +
        lmMst2)*pow2(lmMst1) + pow3(lmMst2)))*pow4(Mst1)*pow4(Mst2)) + (271 +
        526*lmMst2 + 64*lmMst1*(1 + lmMst2) - 30*lmMsq*(5 + 6*lmMst2) + 90*
        pow2(lmMsq) + 208*pow2(lmMst2))*pow2(Mst1)*pow6(Mst2) + 64*(1 + lmMst2)
        *(pow2(1 - lmMst1 + lmMst2)*pow2(Mst2)*pow6(Mst1) + (1 + 3*lmMst2 -
        lmMst1*(3 + 2*lmMst2) + pow2(lmMst1) + pow2(lmMst2))*pow8(Mst1)) - 32*
        pow2(1 + lmMst2)*pow8(Mst2)) + 2*Dmglst2*(pow2(Mst1)*(pow2(Mst1) -
        pow2(Mst2))*(160*(14 - 15*lmMsq + 15*lmMst2)*pow2(Msq)*pow6(Mst2) + 45*
        (13 - 28*lmMsq + 28*lmMst2)*pow8(Mst2)) - 4*pow4(Msq)*(-((121 + 90*
        lmMsq*(-1 + 6*lmMst2) - 270*pow2(lmMsq) + 2*lmMst2*(-325 + 96*pow2(
        lmMst1)) - 432*pow2(lmMst2) + 192*(-(lmMst1*(-1 + lmMst2 + 2*pow2(
        lmMst2))) + pow3(lmMst2)))*pow4(Mst1)*pow4(Mst2)) - 192*(lmMst2*pow2(
        lmMst1) + lmMst1*(4 + 2*lmMst2 - 2*pow2(lmMst2)) + (-4 + lmMst2)*pow2(1
        + lmMst2))*pow2(Mst2)*pow6(Mst1) - 3*(-179 + 2*(87 + 32*lmMst1)*lmMst2
        - 30*lmMsq*(-1 + 6*lmMst2) + 90*pow2(lmMsq) + 208*pow2(lmMst2))*pow2(
        Mst1)*pow6(Mst2) + 192*(-((-6 - 14*lmMst2 + lmMst2*pow2(lmMst1) +
        lmMst1*(9 + 6*lmMst2 - 2*pow2(lmMst2)) - 6*pow2(lmMst2) + pow3(lmMst2))
        *pow8(Mst1)) + lmMst2*(1 + lmMst2)*pow8(Mst2)))) + pow2(Mst1)*(pow2(
        Mst1) - pow2(Mst2))*(40*(43 - 30*lmMsq + 30*lmMst2)*pow2(Msq)*pow7(
        Mst2) + 5*(67 - 84*lmMsq + 84*lmMst2)*pow9(Mst2))))/pow4(Msq)))))/(
        1296.*pow4(Mst1)*pow6(Mst2)));
}

double himalaya::H6::calc_coef_at_as2_no_sm_logs_log0() const {

   const double result =
      4832.548148148148 + (7949728*Dmglst2)/(945.*Mst2) + (448*OepS2)/9. + (
        1792*Dmglst2*OepS2)/(27.*Mst2) - (7848*S2)/5. + (76464*Dmglst2*S2)/(35.
        *Mst2) + (1322*Dmglst2*s2t)/(9.*Mt) - (3502*Mst2*s2t)/(5.*Mt) - (2240*
        Dmglst2*OepS2*s2t)/(27.*Mt) + (448*Mst2*OepS2*s2t)/(9.*Mt) - (3240*
        Dmglst2*S2*s2t)/Mt + (5688*Mst2*S2*s2t)/(5.*Mt) - (224*T1ep)/3. - (896*
        Dmglst2*T1ep)/(9.*Mst2) + (1120*Dmglst2*s2t*T1ep)/(9.*Mt) - (224*Mst2*
        s2t*T1ep)/(3.*Mt) + (217532*z2)/45. - (1837456*Dmglst2*z2)/(315.*Mst2)
        + (8116*Dmglst2*s2t*z2)/(3.*Mt) - (346772*Mst2*s2t*z2)/(45.*Mt) - (
        31216*z3)/3. - (95176*Dmglst2*z3)/(9.*Mst2) + (5104*Dmglst2*s2t*z3)/Mt
        + (15376*Mst2*s2t*z3)/(3.*Mt) - (112*z4)/3. - (448*Dmglst2*z4)/(9.*
        Mst2) - (4048*Dmglst2*s2t*z4)/(9.*Mt) - (1648*Mst2*s2t*z4)/(3.*Mt) - (
        73875152*s2t*pow2(Dmglst2))/(2835.*Mst2*Mt) + (7168*OepS2*s2t*pow2(
        Dmglst2))/(81.*Mst2*Mt) + (188288*S2*s2t*pow2(Dmglst2))/(35.*Mst2*Mt) -
        (3584*s2t*T1ep*pow2(Dmglst2))/(27.*Mst2*Mt) + (407264*s2t*z2*pow2(
        Dmglst2))/(945.*Mst2*Mt) + (309808*s2t*z3*pow2(Dmglst2))/(27.*Mst2*Mt)
        - (1792*s2t*z4*pow2(Dmglst2))/(27.*Mst2*Mt) - (800*Dmglst2*Mst2)/(3.*
        pow2(Msq)) + (640*Dmglst2*Mst2*z2)/pow2(Msq) - (20768*pow2(Dmglst2))/(
        45.*pow2(Msq)) - (42880*Mst2*s2t*pow2(Dmglst2))/(9.*Mt*pow2(Msq)) + (
        320*z2*pow2(Dmglst2))/pow2(Msq) + (2560*Mst2*s2t*z2*pow2(Dmglst2))/(Mt*
        pow2(Msq)) - (1296*Dmglst2*Mst2)/pow2(Mst1) + (2944*Dmglst2*Mst2*z2)/(
        3.*pow2(Mst1)) - (4280*pow2(Dmglst2))/(3.*pow2(Mst1)) + (1024*Mst2*s2t*
        pow2(Dmglst2))/(Mt*pow2(Mst1)) + (1664*z2*pow2(Dmglst2))/(3.*pow2(Mst1)
        ) + (160*pow2(Msq))/pow2(Mst1) - (320*z2*pow2(Msq))/pow2(Mst1) - (
        2117986*s2t*pow2(Mst1))/(405.*Mst2*Mt) + (24128*OepS2*s2t*pow2(Mst1))/(
        81.*Mst2*Mt) - (34936*S2*s2t*pow2(Mst1))/(5.*Mst2*Mt) - (12064*s2t*
        T1ep*pow2(Mst1))/(27.*Mst2*Mt) - (1882628*s2t*z2*pow2(Mst1))/(135.*
        Mst2*Mt) + (387632*s2t*z3*pow2(Mst1))/(27.*Mst2*Mt) - (6032*s2t*z4*
        pow2(Mst1))/(27.*Mst2*Mt) + (359728*pow2(Mst1))/(675.*pow2(Msq)) - (
        320*Dmglst2*pow2(Mst1))/(3.*Mst2*pow2(Msq)) - (20960*Dmglst2*s2t*pow2(
        Mst1))/(27.*Mt*pow2(Msq)) - (15200*Mst2*s2t*pow2(Mst1))/(27.*Mt*pow2(
        Msq)) + (2240*s2t*pow2(Dmglst2)*pow2(Mst1))/(3.*Mst2*Mt*pow2(Msq)) + (
        506176388*pow2(Dmglst2))/(14175.*pow2(Mst2)) - (17024*OepS2*pow2(
        Dmglst2))/(81.*pow2(Mst2)) - (293128*S2*pow2(Dmglst2))/(35.*pow2(Mst2))
        + (8512*T1ep*pow2(Dmglst2))/(27.*pow2(Mst2)) + (10891448*z2*pow2(
        Dmglst2))/(945.*pow2(Mst2)) - (618164*z3*pow2(Dmglst2))/(27.*pow2(Mst2)
        ) + (4256*z4*pow2(Dmglst2))/(27.*pow2(Mst2)) + (160*pow2(Msq))/pow2(
        Mst2) - (320*z2*pow2(Msq))/pow2(Mst2) + (13813838*pow2(Mst1))/(2025.*
        pow2(Mst2)) + (13376*OepS2*pow2(Mst1))/(81.*pow2(Mst2)) - (33304*S2*
        pow2(Mst1))/(5.*pow2(Mst2)) + (1357642*Dmglst2*s2t*pow2(Mst1))/(135.*
        Mt*pow2(Mst2)) - (29504*Dmglst2*OepS2*s2t*pow2(Mst1))/(27.*Mt*pow2(
        Mst2)) + (125528*Dmglst2*S2*s2t*pow2(Mst1))/(15.*Mt*pow2(Mst2)) - (
        6688*T1ep*pow2(Mst1))/(27.*pow2(Mst2)) + (14752*Dmglst2*s2t*T1ep*pow2(
        Mst1))/(9.*Mt*pow2(Mst2)) + (1240156*z2*pow2(Mst1))/(135.*pow2(Mst2)) +
        (1752596*Dmglst2*s2t*z2*pow2(Mst1))/(45.*Mt*pow2(Mst2)) - (552400*z3*
        pow2(Mst1))/(27.*pow2(Mst2)) - (168176*Dmglst2*s2t*z3*pow2(Mst1))/(9.*
        Mt*pow2(Mst2)) - (3344*z4*pow2(Mst1))/(27.*pow2(Mst2)) + (7376*Dmglst2*
        s2t*z4*pow2(Mst1))/(9.*Mt*pow2(Mst2)) - (1120*pow2(Dmglst2)*pow2(Mst1))
        /(3.*pow2(Msq)*pow2(Mst2)) - (13936*pow2(Mst2))/(25.*pow2(Msq)) - (
        40000*Dmglst2*s2t*pow2(Mst2))/(9.*Mt*pow2(Msq)) + (1600*z2*pow2(Mst2))/
        (3.*pow2(Msq)) + (8960*Dmglst2*s2t*z2*pow2(Mst2))/(3.*Mt*pow2(Msq)) + (
        2216*pow2(Mst2))/(3.*pow2(Mst1)) + (1024*Dmglst2*s2t*pow2(Mst2))/(3.*
        Mt*pow2(Mst1)) + (1568*z2*pow2(Mst2))/(3.*pow2(Mst1)) - (1280*pow2(
        Dmglst2)*pow2(Mst2))/(3.*pow2(Msq)*pow2(Mst1)) + (226984*Dmglst2*Mst2*
        pow2(s2t))/(45.*pow2(Mt)) - (128*B4*Dmglst2*Mst2*pow2(s2t))/(3.*pow2(
        Mt)) + (128*D3*Dmglst2*Mst2*pow2(s2t))/(3.*pow2(Mt)) - (64*Dmglst2*DN*
        Mst2*pow2(s2t))/(3.*pow2(Mt)) - (30384*Dmglst2*Mst2*S2*pow2(s2t))/(5.*
        pow2(Mt)) - (55984*Dmglst2*Mst2*z2*pow2(s2t))/(45.*pow2(Mt)) - (7024*
        Dmglst2*Mst2*z3*pow2(s2t))/(9.*pow2(Mt)) + (192*Dmglst2*Mst2*z4*pow2(
        s2t))/pow2(Mt) + (3806606*pow2(Dmglst2)*pow2(s2t))/(315.*pow2(Mt)) - (
        64*B4*pow2(Dmglst2)*pow2(s2t))/(3.*pow2(Mt)) + (64*D3*pow2(Dmglst2)*
        pow2(s2t))/(3.*pow2(Mt)) - (32*DN*pow2(Dmglst2)*pow2(s2t))/(3.*pow2(Mt)
        ) + (448*OepS2*pow2(Dmglst2)*pow2(s2t))/(9.*pow2(Mt)) - (37656*S2*pow2(
        Dmglst2)*pow2(s2t))/(35.*pow2(Mt)) - (224*T1ep*pow2(Dmglst2)*pow2(s2t))
        /(3.*pow2(Mt)) - (255236*z2*pow2(Dmglst2)*pow2(s2t))/(315.*pow2(Mt)) -
        (111194*z3*pow2(Dmglst2)*pow2(s2t))/(9.*pow2(Mt)) + (176*z4*pow2(
        Dmglst2)*pow2(s2t))/(3.*pow2(Mt)) + (160*pow2(Msq)*pow2(s2t))/pow2(Mt)
        - (320*z2*pow2(Msq)*pow2(s2t))/pow2(Mt) + (4141616*pow2(Mst1)*pow2(s2t)
        )/(2025.*pow2(Mt)) - (32*D3*pow2(Mst1)*pow2(s2t))/(3.*pow2(Mt)) + (32*
        DN*pow2(Mst1)*pow2(s2t))/(3.*pow2(Mt)) + (19333928*Dmglst2*pow2(Mst1)*
        pow2(s2t))/(10125.*Mst2*pow2(Mt)) - (20608*OepS2*pow2(Mst1)*pow2(s2t))/
        (81.*pow2(Mt)) + (6400*Dmglst2*OepS2*pow2(Mst1)*pow2(s2t))/(27.*Mst2*
        pow2(Mt)) + (38264*S2*pow2(Mst1)*pow2(s2t))/(5.*pow2(Mt)) - (2976*
        Dmglst2*S2*pow2(Mst1)*pow2(s2t))/(5.*Mst2*pow2(Mt)) + (10304*T1ep*pow2(
        Mst1)*pow2(s2t))/(27.*pow2(Mt)) - (3200*Dmglst2*T1ep*pow2(Mst1)*pow2(
        s2t))/(9.*Mst2*pow2(Mt)) + (591448*z2*pow2(Mst1)*pow2(s2t))/(135.*pow2(
        Mt)) - (17600*Dmglst2*z2*pow2(Mst1)*pow2(s2t))/(3.*Mst2*pow2(Mt)) - (
        15040*z3*pow2(Mst1)*pow2(s2t))/(27.*pow2(Mt)) - (3952*Dmglst2*z3*pow2(
        Mst1)*pow2(s2t))/(3.*Mst2*pow2(Mt)) + (3424*z4*pow2(Mst1)*pow2(s2t))/(
        27.*pow2(Mt)) - (1600*Dmglst2*z4*pow2(Mst1)*pow2(s2t))/(9.*Mst2*pow2(
        Mt)) + (400*Dmglst2*Mst2*pow2(Mst1)*pow2(s2t))/(pow2(Msq)*pow2(Mt)) + (
        760*pow2(Dmglst2)*pow2(Mst1)*pow2(s2t))/(3.*pow2(Msq)*pow2(Mt)) + (
        48696082*pow2(Dmglst2)*pow2(Mst1)*pow2(s2t))/(23625.*pow2(Mst2)*pow2(
        Mt)) + (704*OepS2*pow2(Dmglst2)*pow2(Mst1)*pow2(s2t))/(3.*pow2(Mst2)*
        pow2(Mt)) - (318232*S2*pow2(Dmglst2)*pow2(Mst1)*pow2(s2t))/(35.*pow2(
        Mst2)*pow2(Mt)) - (352*T1ep*pow2(Dmglst2)*pow2(Mst1)*pow2(s2t))/(pow2(
        Mst2)*pow2(Mt)) - (640972*z2*pow2(Dmglst2)*pow2(Mst1)*pow2(s2t))/(63.*
        pow2(Mst2)*pow2(Mt)) + (30106*z3*pow2(Dmglst2)*pow2(Mst1)*pow2(s2t))/(
        9.*pow2(Mst2)*pow2(Mt)) - (176*z4*pow2(Dmglst2)*pow2(Mst1)*pow2(s2t))/(
        pow2(Mst2)*pow2(Mt)) - (80*pow2(Msq)*pow2(Mst1)*pow2(s2t))/(pow2(Mst2)*
        pow2(Mt)) + (160*z2*pow2(Msq)*pow2(Mst1)*pow2(s2t))/(pow2(Mst2)*pow2(
        Mt)) + (818213*pow2(Mst2)*pow2(s2t))/(270.*pow2(Mt)) + (32*D3*pow2(
        Mst2)*pow2(s2t))/(3.*pow2(Mt)) - (32*DN*pow2(Mst2)*pow2(s2t))/(3.*pow2(
        Mt)) - (368*OepS2*pow2(Mst2)*pow2(s2t))/(9.*pow2(Mt)) - (16038*S2*pow2(
        Mst2)*pow2(s2t))/(5.*pow2(Mt)) + (184*T1ep*pow2(Mst2)*pow2(s2t))/(3.*
        pow2(Mt)) - (17923*z2*pow2(Mst2)*pow2(s2t))/(45.*pow2(Mt)) + (3244*z3*
        pow2(Mst2)*pow2(s2t))/(3.*pow2(Mt)) + (284*z4*pow2(Mst2)*pow2(s2t))/(3.
        *pow2(Mt)) - (440*pow2(Dmglst2)*pow2(Mst2)*pow2(s2t))/(pow2(Msq)*pow2(
        Mt)) + (2908*pow2(Dmglst2)*pow2(Mst2)*pow2(s2t))/(3.*pow2(Mst1)*pow2(
        Mt)) - (832*z2*pow2(Dmglst2)*pow2(Mst2)*pow2(s2t))/(3.*pow2(Mst1)*pow2(
        Mt)) - (80*pow2(Msq)*pow2(Mst2)*pow2(s2t))/(pow2(Mst1)*pow2(Mt)) + (
        160*z2*pow2(Msq)*pow2(Mst2)*pow2(s2t))/(pow2(Mst1)*pow2(Mt)) - (24728*
        pow2(Mst1)*pow2(Mst2)*pow2(s2t))/(675.*pow2(Msq)*pow2(Mt)) - 56*pow2(
        z2) - (224*Dmglst2*pow2(z2))/(3.*Mst2) + (280*Dmglst2*s2t*pow2(z2))/(3.
        *Mt) - (56*Mst2*s2t*pow2(z2))/Mt - (896*s2t*pow2(Dmglst2)*pow2(z2))/(9.
        *Mst2*Mt) - (3016*s2t*pow2(Mst1)*pow2(z2))/(9.*Mst2*Mt) + (2128*pow2(
        Dmglst2)*pow2(z2))/(9.*pow2(Mst2)) - (1672*pow2(Mst1)*pow2(z2))/(9.*
        pow2(Mst2)) + (3688*Dmglst2*s2t*pow2(Mst1)*pow2(z2))/(3.*Mt*pow2(Mst2))
        - (56*pow2(Dmglst2)*pow2(s2t)*pow2(z2))/pow2(Mt) + (2576*pow2(Mst1)*
        pow2(s2t)*pow2(z2))/(9.*pow2(Mt)) - (800*Dmglst2*pow2(Mst1)*pow2(s2t)*
        pow2(z2))/(3.*Mst2*pow2(Mt)) - (264*pow2(Dmglst2)*pow2(Mst1)*pow2(s2t)*
        pow2(z2))/(pow2(Mst2)*pow2(Mt)) + (46*pow2(Mst2)*pow2(s2t)*pow2(z2))/
        pow2(Mt) + (184384408*Dmglst2*pow2(Mst1))/(70875.*pow3(Mst2)) + (41216*
        Dmglst2*OepS2*pow2(Mst1))/(81.*pow3(Mst2)) - (42320*Dmglst2*S2*pow2(
        Mst1))/(21.*pow3(Mst2)) - (20608*Dmglst2*T1ep*pow2(Mst1))/(27.*pow3(
        Mst2)) - (42483152*Dmglst2*z2*pow2(Mst1))/(945.*pow3(Mst2)) + (784832*
        Dmglst2*z3*pow2(Mst1))/(27.*pow3(Mst2)) - (10304*Dmglst2*z4*pow2(Mst1))
        /(27.*pow3(Mst2)) - (266958448*s2t*pow2(Dmglst2)*pow2(Mst1))/(8505.*Mt*
        pow3(Mst2)) + (541184*OepS2*s2t*pow2(Dmglst2)*pow2(Mst1))/(243.*Mt*
        pow3(Mst2)) + (3449536*S2*s2t*pow2(Dmglst2)*pow2(Mst1))/(315.*Mt*pow3(
        Mst2)) - (270592*s2t*T1ep*pow2(Dmglst2)*pow2(Mst1))/(81.*Mt*pow3(Mst2))
        - (194745344*s2t*z2*pow2(Dmglst2)*pow2(Mst1))/(2835.*Mt*pow3(Mst2)) + (
        2058752*s2t*z3*pow2(Dmglst2)*pow2(Mst1))/(81.*Mt*pow3(Mst2)) - (135296*
        s2t*z4*pow2(Dmglst2)*pow2(Mst1))/(81.*Mt*pow3(Mst2)) - (5152*Dmglst2*
        pow2(Mst1)*pow2(z2))/(9.*pow3(Mst2)) - (67648*s2t*pow2(Dmglst2)*pow2(
        Mst1)*pow2(z2))/(27.*Mt*pow3(Mst2)) - (38080*s2t*pow3(Mst2))/(27.*Mt*
        pow2(Msq)) + (1280*s2t*z2*pow3(Mst2))/(Mt*pow2(Msq)) - (1024*s2t*pow3(
        Mst2))/(3.*Mt*pow2(Mst1)) - (8960*Dmglst2*pow3(Mst2))/(9.*pow2(Msq)*
        pow2(Mst1)) - (8560*Dmglst2*pow2(s2t)*pow3(Mst2))/(9.*pow2(Msq)*pow2(
        Mt)) + (1432*Dmglst2*pow2(s2t)*pow3(Mst2))/(3.*pow2(Mst1)*pow2(Mt)) - (
        1472*Dmglst2*z2*pow2(s2t)*pow3(Mst2))/(3.*pow2(Mst1)*pow2(Mt)) - (
        745792*Mst2*pow2(Dmglst2)*pow3(s2t))/(405.*pow3(Mt)) + (608*B4*Mst2*
        pow2(Dmglst2)*pow3(s2t))/pow3(Mt) - (16*DN*Mst2*pow2(Dmglst2)*pow3(s2t)
        )/pow3(Mt) - (1792*Mst2*OepS2*pow2(Dmglst2)*pow3(s2t))/(81.*pow3(Mt)) -
        (5492*Mst2*S2*pow2(Dmglst2)*pow3(s2t))/(5.*pow3(Mt)) + (896*Mst2*T1ep*
        pow2(Dmglst2)*pow3(s2t))/(27.*pow3(Mt)) - (505496*Mst2*z2*pow2(Dmglst2)
        *pow3(s2t))/(135.*pow3(Mt)) + (288104*Mst2*z3*pow2(Dmglst2)*pow3(s2t))/
        (27.*pow3(Mt)) + (1744*Mst2*z4*pow2(Dmglst2)*pow3(s2t))/(27.*pow3(Mt))
        - (231989*Dmglst2*pow2(Mst1)*pow3(s2t))/(405.*pow3(Mt)) + (272*B4*
        Dmglst2*pow2(Mst1)*pow3(s2t))/(3.*pow3(Mt)) + (8*Dmglst2*DN*pow2(Mst1)*
        pow3(s2t))/(3.*pow3(Mt)) + (18641*Mst2*pow2(Mst1)*pow3(s2t))/(81.*pow3(
        Mt)) + (272*B4*Mst2*pow2(Mst1)*pow3(s2t))/(3.*pow3(Mt)) + (8*DN*Mst2*
        pow2(Mst1)*pow3(s2t))/(3.*pow3(Mt)) + (12832*Dmglst2*OepS2*pow2(Mst1)*
        pow3(s2t))/(81.*pow3(Mt)) - (2336*Mst2*OepS2*pow2(Mst1)*pow3(s2t))/(81.
        *pow3(Mt)) - (56132*Dmglst2*S2*pow2(Mst1)*pow3(s2t))/(15.*pow3(Mt)) + (
        1388*Mst2*S2*pow2(Mst1)*pow3(s2t))/pow3(Mt) - (6416*Dmglst2*T1ep*pow2(
        Mst1)*pow3(s2t))/(27.*pow3(Mt)) + (1168*Mst2*T1ep*pow2(Mst1)*pow3(s2t))
        /(27.*pow3(Mt)) - (467482*Dmglst2*z2*pow2(Mst1)*pow3(s2t))/(135.*pow3(
        Mt)) + (15826*Mst2*z2*pow2(Mst1)*pow3(s2t))/(27.*pow3(Mt)) + (25000*
        Dmglst2*z3*pow2(Mst1)*pow3(s2t))/(27.*pow3(Mt)) - (34712*Mst2*z3*pow2(
        Mst1)*pow3(s2t))/(27.*pow3(Mt)) - (832*Dmglst2*z4*pow2(Mst1)*pow3(s2t))
        /(27.*pow3(Mt)) + (2960*Mst2*z4*pow2(Mst1)*pow3(s2t))/(27.*pow3(Mt)) +
        (9255817*pow2(Dmglst2)*pow2(Mst1)*pow3(s2t))/(1215.*Mst2*pow3(Mt)) - (
        87584*OepS2*pow2(Dmglst2)*pow2(Mst1)*pow3(s2t))/(243.*Mst2*pow3(Mt)) +
        (135116*S2*pow2(Dmglst2)*pow2(Mst1)*pow3(s2t))/(45.*Mst2*pow3(Mt)) + (
        43792*T1ep*pow2(Dmglst2)*pow2(Mst1)*pow3(s2t))/(81.*Mst2*pow3(Mt)) + (
        2934146*z2*pow2(Dmglst2)*pow2(Mst1)*pow3(s2t))/(405.*Mst2*pow3(Mt)) - (
        355520*z3*pow2(Dmglst2)*pow2(Mst1)*pow3(s2t))/(81.*Mst2*pow3(Mt)) + (
        21896*z4*pow2(Dmglst2)*pow2(Mst1)*pow3(s2t))/(81.*Mst2*pow3(Mt)) + (
        2800*Mst2*pow2(Dmglst2)*pow2(Mst1)*pow3(s2t))/(9.*pow2(Msq)*pow3(Mt)) -
        (320*Mst2*z2*pow2(Dmglst2)*pow2(Mst1)*pow3(s2t))/(pow2(Msq)*pow3(Mt)) +
        (825997*Dmglst2*pow2(Mst2)*pow3(s2t))/(270.*pow3(Mt)) + (2096*B4*
        Dmglst2*pow2(Mst2)*pow3(s2t))/(3.*pow3(Mt)) - (40*Dmglst2*DN*pow2(Mst2)
        *pow3(s2t))/(3.*pow3(Mt)) + (560*Dmglst2*OepS2*pow2(Mst2)*pow3(s2t))/(
        27.*pow3(Mt)) + (2718*Dmglst2*S2*pow2(Mst2)*pow3(s2t))/(5.*pow3(Mt)) -
        (280*Dmglst2*T1ep*pow2(Mst2)*pow3(s2t))/(9.*pow3(Mt)) - (278347*
        Dmglst2*z2*pow2(Mst2)*pow3(s2t))/(45.*pow3(Mt)) + (74948*Dmglst2*z3*
        pow2(Mst2)*pow3(s2t))/(9.*pow3(Mt)) + (1084*Dmglst2*z4*pow2(Mst2)*pow3(
        s2t))/(9.*pow3(Mt)) + (960*Dmglst2*pow2(Mst1)*pow2(Mst2)*pow3(s2t))/(
        pow2(Msq)*pow3(Mt)) - (320*Dmglst2*z2*pow2(Mst1)*pow2(Mst2)*pow3(s2t))/
        (pow2(Msq)*pow3(Mt)) - (10144*Mst2*pow2(Dmglst2)*pow2(z2)*pow3(s2t))/(
        9.*pow3(Mt)) - (1604*Dmglst2*pow2(Mst1)*pow2(z2)*pow3(s2t))/(9.*pow3(
        Mt)) + (292*Mst2*pow2(Mst1)*pow2(z2)*pow3(s2t))/(9.*pow3(Mt)) + (10948*
        pow2(Dmglst2)*pow2(Mst1)*pow2(z2)*pow3(s2t))/(27.*Mst2*pow3(Mt)) - (
        3526*Dmglst2*pow2(Mst2)*pow2(z2)*pow3(s2t))/(3.*pow3(Mt)) + (17177*
        pow3(Mst2)*pow3(s2t))/(6.*pow3(Mt)) + (880*B4*pow3(Mst2)*pow3(s2t))/(3.
        *pow3(Mt)) - (8*DN*pow3(Mst2)*pow3(s2t))/(3.*pow3(Mt)) - (112*OepS2*
        pow3(Mst2)*pow3(s2t))/(9.*pow3(Mt)) - (18*S2*pow3(Mst2)*pow3(s2t))/
        pow3(Mt) + (56*T1ep*pow3(Mst2)*pow3(s2t))/(3.*pow3(Mt)) - (3469*z2*
        pow3(Mst2)*pow3(s2t))/(3.*pow3(Mt)) + (20980*z3*pow3(Mst2)*pow3(s2t))/(
        9.*pow3(Mt)) + (340*z4*pow3(Mst2)*pow3(s2t))/(3.*pow3(Mt)) - (40720*
        pow2(Dmglst2)*pow3(Mst2)*pow3(s2t))/(9.*pow2(Msq)*pow3(Mt)) + (2880*z2*
        pow2(Dmglst2)*pow3(Mst2)*pow3(s2t))/(pow2(Msq)*pow3(Mt)) - (256*pow2(
        Dmglst2)*pow3(Mst2)*pow3(s2t))/(pow2(Mst1)*pow3(Mt)) + (4000*pow2(Mst1)
        *pow3(Mst2)*pow3(s2t))/(9.*pow2(Msq)*pow3(Mt)) - (320*z2*pow2(Mst1)*
        pow3(Mst2)*pow3(s2t))/(3.*pow2(Msq)*pow3(Mt)) - (370*pow2(z2)*pow3(
        Mst2)*pow3(s2t))/pow3(Mt) + (160*pow3(log(pow2(Mst1)/pow2(Msq))))/3. +
        (3320*Dmglst2*Mst2*pow2(Mst1))/(9.*pow4(Msq)) + (1900*pow2(Dmglst2)*
        pow2(Mst1))/(9.*pow4(Msq)) - (3760*Mst2*s2t*pow2(Dmglst2)*pow2(Mst1))/(
        9.*Mt*pow4(Msq)) - (17942*pow2(Dmglst2)*pow2(Mst2))/(45.*pow4(Msq)) + (
        960*z2*pow2(Dmglst2)*pow2(Mst2))/pow4(Msq) + (100*pow2(Mst1)*pow2(Mst2)
        )/(9.*pow4(Msq)) - (3280*Dmglst2*s2t*pow2(Mst1)*pow2(Mst2))/(9.*Mt*
        pow4(Msq)) - (55*pow2(Dmglst2)*pow2(Mst1)*pow2(Mst2)*pow2(s2t))/(pow2(
        Mt)*pow4(Msq)) - (25340*Dmglst2*pow3(Mst2))/(27.*pow4(Msq)) + (2560*
        Dmglst2*z2*pow3(Mst2))/(3.*pow4(Msq)) - (53840*s2t*pow2(Dmglst2)*pow3(
        Mst2))/(9.*Mt*pow4(Msq)) + (3840*s2t*z2*pow2(Dmglst2)*pow3(Mst2))/(Mt*
        pow4(Msq)) - (400*s2t*pow2(Mst1)*pow3(Mst2))/(27.*Mt*pow4(Msq)) + (650*
        Dmglst2*pow2(Mst1)*pow2(s2t)*pow3(Mst2))/(9.*pow2(Mt)*pow4(Msq)) + (
        4780*pow2(Dmglst2)*pow2(Mst1)*pow3(Mst2)*pow3(s2t))/(9.*pow3(Mt)*pow4(
        Msq)) - (400*z2*pow2(Dmglst2)*pow2(Mst1)*pow3(Mst2)*pow3(s2t))/(pow3(
        Mt)*pow4(Msq)) + (1024*pow2(Dmglst2)*pow2(Mst2))/(3.*pow4(Mst1)) - (
        2240*s2t*pow4(Mst1))/(3.*Mst2*Mt*pow2(Msq)) + (1040*pow4(Mst1))/(pow2(
        Msq)*pow2(Mst2)) + (1600*Dmglst2*s2t*pow4(Mst1))/(Mt*pow2(Msq)*pow2(
        Mst2)) + (320*z2*pow4(Mst1))/(pow2(Msq)*pow2(Mst2)) + (40864*pow2(s2t)*
        pow4(Mst1))/(675.*pow2(Msq)*pow2(Mt)) + (640*Dmglst2*pow2(s2t)*pow4(
        Mst1))/(3.*Mst2*pow2(Msq)*pow2(Mt)) + (24574447162*pow2(s2t)*pow4(Mst1)
        )/(1.0418625e7*pow2(Mst2)*pow2(Mt)) - (110672*OepS2*pow2(s2t)*pow4(
        Mst1))/(243.*pow2(Mst2)*pow2(Mt)) + (813254*S2*pow2(s2t)*pow4(Mst1))/(
        45.*pow2(Mst2)*pow2(Mt)) + (55336*T1ep*pow2(s2t)*pow4(Mst1))/(81.*pow2(
        Mst2)*pow2(Mt)) + (3321329*z2*pow2(s2t)*pow4(Mst1))/(405.*pow2(Mst2)*
        pow2(Mt)) - (132332*z3*pow2(s2t)*pow4(Mst1))/(81.*pow2(Mst2)*pow2(Mt))
        + (27668*z4*pow2(s2t)*pow4(Mst1))/(81.*pow2(Mst2)*pow2(Mt)) + (13834*
        pow2(s2t)*pow2(z2)*pow4(Mst1))/(27.*pow2(Mst2)*pow2(Mt)) - (15260456*
        s2t*pow4(Mst1))/(1215.*Mt*pow3(Mst2)) + (175360*OepS2*s2t*pow4(Mst1))/(
        243.*Mt*pow3(Mst2)) - (1043008*S2*s2t*pow4(Mst1))/(45.*Mt*pow3(Mst2)) -
        (87680*s2t*T1ep*pow4(Mst1))/(81.*Mt*pow3(Mst2)) - (9196144*s2t*z2*pow4(
        Mst1))/(405.*Mt*pow3(Mst2)) + (1848256*s2t*z3*pow4(Mst1))/(81.*Mt*pow3(
        Mst2)) - (43840*s2t*z4*pow4(Mst1))/(81.*Mt*pow3(Mst2)) - (4640*Dmglst2*
        pow4(Mst1))/(pow2(Msq)*pow3(Mst2)) - (640*Dmglst2*z2*pow4(Mst1))/(pow2(
        Msq)*pow3(Mst2)) - (7932854794*Dmglst2*pow2(s2t)*pow4(Mst1))/(3.472875e6
        *pow2(Mt)*pow3(Mst2)) + (98432*Dmglst2*OepS2*pow2(s2t)*pow4(Mst1))/(
        81.*pow2(Mt)*pow3(Mst2)) - (452528*Dmglst2*S2*pow2(s2t)*pow4(Mst1))/(
        15.*pow2(Mt)*pow3(Mst2)) - (49216*Dmglst2*T1ep*pow2(s2t)*pow4(Mst1))/(
        27.*pow2(Mt)*pow3(Mst2)) - (628264*Dmglst2*z2*pow2(s2t)*pow4(Mst1))/(
        27.*pow2(Mt)*pow3(Mst2)) + (11744*Dmglst2*z3*pow2(s2t)*pow4(Mst1))/(27.
        *pow2(Mt)*pow3(Mst2)) - (24608*Dmglst2*z4*pow2(s2t)*pow4(Mst1))/(27.*
        pow2(Mt)*pow3(Mst2)) - (21920*s2t*pow2(z2)*pow4(Mst1))/(27.*Mt*pow3(
        Mst2)) - (12304*Dmglst2*pow2(s2t)*pow2(z2)*pow4(Mst1))/(9.*pow2(Mt)*
        pow3(Mst2)) + (542267*pow3(s2t)*pow4(Mst1))/(486.*Mst2*pow3(Mt)) - (
        7024*OepS2*pow3(s2t)*pow4(Mst1))/(243.*Mst2*pow3(Mt)) + (12242*S2*pow3(
        s2t)*pow4(Mst1))/(9.*Mst2*pow3(Mt)) + (3512*T1ep*pow3(s2t)*pow4(Mst1))/
        (81.*Mst2*pow3(Mt)) + (104839*z2*pow3(s2t)*pow4(Mst1))/(405.*Mst2*pow3(
        Mt)) - (89260*z3*pow3(s2t)*pow4(Mst1))/(81.*Mst2*pow3(Mt)) + (1756*z4*
        pow3(s2t)*pow4(Mst1))/(81.*Mst2*pow3(Mt)) - (80*Dmglst2*pow3(s2t)*pow4(
        Mst1))/(3.*pow2(Msq)*pow3(Mt)) + (80*Mst2*pow3(s2t)*pow4(Mst1))/(3.*
        pow2(Msq)*pow3(Mt)) - (35542243*Dmglst2*pow3(s2t)*pow4(Mst1))/(7290.*
        pow2(Mst2)*pow3(Mt)) + (249136*Dmglst2*OepS2*pow3(s2t)*pow4(Mst1))/(
        729.*pow2(Mst2)*pow3(Mt)) - (1672042*Dmglst2*S2*pow3(s2t)*pow4(Mst1))/(
        135.*pow2(Mst2)*pow3(Mt)) - (124568*Dmglst2*T1ep*pow3(s2t)*pow4(Mst1))/
        (243.*pow2(Mst2)*pow3(Mt)) - (8194483*Dmglst2*z2*pow3(s2t)*pow4(Mst1))/
        (1215.*pow2(Mst2)*pow3(Mt)) + (1130428*Dmglst2*z3*pow3(s2t)*pow4(Mst1))
        /(243.*pow2(Mst2)*pow3(Mt)) - (62284*Dmglst2*z4*pow3(s2t)*pow4(Mst1))/(
        243.*pow2(Mst2)*pow3(Mt)) + (878*pow2(z2)*pow3(s2t)*pow4(Mst1))/(27.*
        Mst2*pow3(Mt)) - (31142*Dmglst2*pow2(z2)*pow3(s2t)*pow4(Mst1))/(81.*
        pow2(Mst2)*pow3(Mt)) + (15728921*pow4(Mst1))/(77175.*pow4(Msq)) - (
        6200*Dmglst2*s2t*pow4(Mst1))/(27.*Mt*pow4(Msq)) - (1880*Mst2*s2t*pow4(
        Mst1))/(27.*Mt*pow4(Msq)) + (10*Dmglst2*Mst2*pow2(s2t)*pow4(Mst1))/(
        pow2(Mt)*pow4(Msq)) - (438008*pow2(Mst2)*pow2(s2t)*pow4(Mst1))/(15435.*
        pow2(Mt)*pow4(Msq)) + (115*Dmglst2*pow2(Mst2)*pow3(s2t)*pow4(Mst1))/(9.
        *pow3(Mt)*pow4(Msq)) - (5*pow3(Mst2)*pow3(s2t)*pow4(Mst1))/(9.*pow3(Mt)
        *pow4(Msq)) + (13776458924*pow2(Dmglst2)*pow2(Mst1))/(212625.*pow4(
        Mst2)) - (725632*OepS2*pow2(Dmglst2)*pow2(Mst1))/(243.*pow4(Mst2)) - (
        395576*S2*pow2(Dmglst2)*pow2(Mst1))/(315.*pow4(Mst2)) + (362816*T1ep*
        pow2(Dmglst2)*pow2(Mst1))/(81.*pow4(Mst2)) + (377926216*z2*pow2(
        Dmglst2)*pow2(Mst1))/(2835.*pow4(Mst2)) - (7423840*z3*pow2(Dmglst2)*
        pow2(Mst1))/(81.*pow4(Mst2)) + (181408*z4*pow2(Dmglst2)*pow2(Mst1))/(
        81.*pow4(Mst2)) + (90704*pow2(Dmglst2)*pow2(Mst1)*pow2(z2))/(27.*pow4(
        Mst2)) + (175715865103*pow4(Mst1))/(1.0418625e7*pow4(Mst2)) + (74752*
        OepS2*pow4(Mst1))/(243.*pow4(Mst2)) - (553792*S2*pow4(Mst1))/(45.*pow4(
        Mst2)) + (193667576*Dmglst2*s2t*pow4(Mst1))/(3645.*Mt*pow4(Mst2)) - (
        3293440*Dmglst2*OepS2*s2t*pow4(Mst1))/(729.*Mt*pow4(Mst2)) + (14087488*
        Dmglst2*S2*s2t*pow4(Mst1))/(135.*Mt*pow4(Mst2)) - (37376*T1ep*pow4(
        Mst1))/(81.*pow4(Mst2)) + (1646720*Dmglst2*s2t*T1ep*pow4(Mst1))/(243.*
        Mt*pow4(Mst2)) + (4885352*z2*pow4(Mst1))/(405.*pow4(Mst2)) + (
        147644272*Dmglst2*s2t*z2*pow4(Mst1))/(1215.*Mt*pow4(Mst2)) - (3102848*
        z3*pow4(Mst1))/(81.*pow4(Mst2)) - (19670464*Dmglst2*s2t*z3*pow4(Mst1))/
        (243.*Mt*pow4(Mst2)) - (18688*z4*pow4(Mst1))/(81.*pow4(Mst2)) + (
        823360*Dmglst2*s2t*z4*pow4(Mst1))/(243.*Mt*pow4(Mst2)) - (9344*pow2(z2)
        *pow4(Mst1))/(27.*pow4(Mst2)) + (411680*Dmglst2*s2t*pow2(z2)*pow4(Mst1)
        )/(81.*Mt*pow4(Mst2)) - (3440*pow4(Mst2))/(9.*pow2(Msq)*pow2(Mst1)) - (
        181136*pow2(s2t)*pow4(Mst2))/(675.*pow2(Msq)*pow2(Mt)) - (412*pow2(s2t)
        *pow4(Mst2))/(pow2(Mst1)*pow2(Mt)) - (784*z2*pow2(s2t)*pow4(Mst2))/(3.*
        pow2(Mst1)*pow2(Mt)) + (640*pow2(Dmglst2)*pow2(s2t)*pow4(Mst2))/(3.*
        pow2(Msq)*pow2(Mst1)*pow2(Mt)) - (7600*Dmglst2*pow3(s2t)*pow4(Mst2))/(
        3.*pow2(Msq)*pow3(Mt)) + (1280*Dmglst2*z2*pow3(s2t)*pow4(Mst2))/(pow2(
        Msq)*pow3(Mt)) - (256*Dmglst2*pow3(s2t)*pow4(Mst2))/(3.*pow2(Mst1)*
        pow3(Mt)) - (142320737*pow4(Mst2))/(231525.*pow4(Msq)) - (30160*
        Dmglst2*s2t*pow4(Mst2))/(9.*Mt*pow4(Msq)) + (1120*z2*pow4(Mst2))/(3.*
        pow4(Msq)) + (7040*Dmglst2*s2t*z2*pow4(Mst2))/(3.*Mt*pow4(Msq)) - (90*
        pow2(Dmglst2)*pow4(Mst2))/(pow2(Mst1)*pow4(Msq)) - (125*pow2(Dmglst2)*
        pow2(s2t)*pow4(Mst2))/(3.*pow2(Mt)*pow4(Msq)) + (522392*pow2(Mst1)*
        pow2(s2t)*pow4(Mst2))/(15435.*pow2(Mt)*pow4(Msq)) + (4115*Dmglst2*pow2(
        Mst1)*pow3(s2t)*pow4(Mst2))/(9.*pow3(Mt)*pow4(Msq)) - (200*Dmglst2*z2*
        pow2(Mst1)*pow3(s2t)*pow4(Mst2))/(pow3(Mt)*pow4(Msq)) - (256*pow4(Mst2)
        )/(3.*pow4(Mst1)) - (512*pow2(Dmglst2)*pow2(s2t)*pow4(Mst2))/(3.*pow2(
        Mt)*pow4(Mst1)) + (1727459*Dmglst2*Mst2*pow2(Mst1)*pow4(s2t))/(10125.*
        pow4(Mt)) + (8*B4*Dmglst2*Mst2*pow2(Mst1)*pow4(s2t))/pow4(Mt) - (32*D3*
        Dmglst2*Mst2*pow2(Mst1)*pow4(s2t))/(3.*pow4(Mt)) + (20*Dmglst2*DN*Mst2*
        pow2(Mst1)*pow4(s2t))/(3.*pow4(Mt)) - (128*Dmglst2*Mst2*OepS2*pow2(
        Mst1)*pow4(s2t))/(3.*pow4(Mt)) - (2200*Dmglst2*Mst2*S2*pow2(Mst1)*pow4(
        s2t))/(3.*pow4(Mt)) + (64*Dmglst2*Mst2*T1ep*pow2(Mst1)*pow4(s2t))/pow4(
        Mt) + (15568*Dmglst2*Mst2*z2*pow2(Mst1)*pow4(s2t))/(45.*pow4(Mt)) + (
        4478*Dmglst2*Mst2*z3*pow2(Mst1)*pow4(s2t))/(9.*pow4(Mt)) - (12*Dmglst2*
        Mst2*z4*pow2(Mst1)*pow4(s2t))/pow4(Mt) - (76330321*pow2(Dmglst2)*pow2(
        Mst1)*pow4(s2t))/(60750.*pow4(Mt)) + (4*B4*pow2(Dmglst2)*pow2(Mst1)*
        pow4(s2t))/pow4(Mt) - (16*D3*pow2(Dmglst2)*pow2(Mst1)*pow4(s2t))/(3.*
        pow4(Mt)) + (10*DN*pow2(Dmglst2)*pow2(Mst1)*pow4(s2t))/(3.*pow4(Mt)) +
        (9088*OepS2*pow2(Dmglst2)*pow2(Mst1)*pow4(s2t))/(243.*pow4(Mt)) - (
        56092*S2*pow2(Dmglst2)*pow2(Mst1)*pow4(s2t))/(45.*pow4(Mt)) - (4544*
        T1ep*pow2(Dmglst2)*pow2(Mst1)*pow4(s2t))/(81.*pow4(Mt)) - (41852*z2*
        pow2(Dmglst2)*pow2(Mst1)*pow4(s2t))/(81.*pow4(Mt)) + (99253*z3*pow2(
        Dmglst2)*pow2(Mst1)*pow4(s2t))/(81.*pow4(Mt)) - (4054*z4*pow2(Dmglst2)*
        pow2(Mst1)*pow4(s2t))/(81.*pow4(Mt)) - (10*pow2(Msq)*pow2(Mst1)*pow4(
        s2t))/pow4(Mt) + (20*z2*pow2(Msq)*pow2(Mst1)*pow4(s2t))/pow4(Mt) + (
        980039*pow2(Dmglst2)*pow2(Mst2)*pow4(s2t))/(1620.*pow4(Mt)) - (76*B4*
        pow2(Dmglst2)*pow2(Mst2)*pow4(s2t))/(3.*pow4(Mt)) + (80*D3*pow2(
        Dmglst2)*pow2(Mst2)*pow4(s2t))/(3.*pow4(Mt)) - (14*DN*pow2(Dmglst2)*
        pow2(Mst2)*pow4(s2t))/pow4(Mt) + (56*OepS2*pow2(Dmglst2)*pow2(Mst2)*
        pow4(s2t))/(81.*pow4(Mt)) - (65819*S2*pow2(Dmglst2)*pow2(Mst2)*pow4(
        s2t))/(5.*pow4(Mt)) - (28*T1ep*pow2(Dmglst2)*pow2(Mst2)*pow4(s2t))/(27.
        *pow4(Mt)) - (27319*z2*pow2(Dmglst2)*pow2(Mst2)*pow4(s2t))/(270.*pow4(
        Mt)) + (129397*z3*pow2(Dmglst2)*pow2(Mst2)*pow4(s2t))/(54.*pow4(Mt)) +
        (3172*z4*pow2(Dmglst2)*pow2(Mst2)*pow4(s2t))/(27.*pow4(Mt)) - (10*pow2(
        Msq)*pow2(Mst2)*pow4(s2t))/pow4(Mt) + (20*z2*pow2(Msq)*pow2(Mst2)*pow4(
        s2t))/pow4(Mt) - (9250423*pow2(Mst1)*pow2(Mst2)*pow4(s2t))/(8100.*pow4(
        Mt)) + (4*B4*pow2(Mst1)*pow2(Mst2)*pow4(s2t))/(3.*pow4(Mt)) - (8*D3*
        pow2(Mst1)*pow2(Mst2)*pow4(s2t))/(3.*pow4(Mt)) + (2*DN*pow2(Mst1)*pow2(
        Mst2)*pow4(s2t))/pow4(Mt) + (2744*OepS2*pow2(Mst1)*pow2(Mst2)*pow4(s2t)
        )/(81.*pow4(Mt)) - (3137*S2*pow2(Mst1)*pow2(Mst2)*pow4(s2t))/pow4(Mt) -
        (1372*T1ep*pow2(Mst1)*pow2(Mst2)*pow4(s2t))/(27.*pow4(Mt)) - (235099*
        z2*pow2(Mst1)*pow2(Mst2)*pow4(s2t))/(270.*pow4(Mt)) + (22544*z3*pow2(
        Mst1)*pow2(Mst2)*pow4(s2t))/(27.*pow4(Mt)) - (956*z4*pow2(Mst1)*pow2(
        Mst2)*pow4(s2t))/(27.*pow4(Mt)) - (2050*pow2(Dmglst2)*pow2(Mst1)*pow2(
        Mst2)*pow4(s2t))/(9.*pow2(Msq)*pow4(Mt)) + (200*z2*pow2(Dmglst2)*pow2(
        Mst1)*pow2(Mst2)*pow4(s2t))/(pow2(Msq)*pow4(Mt)) - (1784*pow2(Dmglst2)*
        pow2(Mst1)*pow2(z2)*pow4(s2t))/(27.*pow4(Mt)) + (209*pow2(Dmglst2)*
        pow2(Mst2)*pow2(z2)*pow4(s2t))/(9.*pow4(Mt)) - (559*pow2(Mst1)*pow2(
        Mst2)*pow2(z2)*pow4(s2t))/(9.*pow4(Mt)) + (7337*Dmglst2*pow3(Mst2)*
        pow4(s2t))/(54.*pow4(Mt)) - (8*B4*Dmglst2*pow3(Mst2)*pow4(s2t))/pow4(
        Mt) + (32*D3*Dmglst2*pow3(Mst2)*pow4(s2t))/(3.*pow4(Mt)) - (20*Dmglst2*
        DN*pow3(Mst2)*pow4(s2t))/(3.*pow4(Mt)) - (112*Dmglst2*OepS2*pow3(Mst2)*
        pow4(s2t))/(27.*pow4(Mt)) - (10062*Dmglst2*S2*pow3(Mst2)*pow4(s2t))/
        pow4(Mt) + (56*Dmglst2*T1ep*pow3(Mst2)*pow4(s2t))/(9.*pow4(Mt)) + (
        3047*Dmglst2*z2*pow3(Mst2)*pow4(s2t))/(9.*pow4(Mt)) + (16175*Dmglst2*
        z3*pow3(Mst2)*pow4(s2t))/(9.*pow4(Mt)) + (424*Dmglst2*z4*pow3(Mst2)*
        pow4(s2t))/(9.*pow4(Mt)) - (1540*Dmglst2*pow2(Mst1)*pow3(Mst2)*pow4(
        s2t))/(9.*pow2(Msq)*pow4(Mt)) + (400*Dmglst2*z2*pow2(Mst1)*pow3(Mst2)*
        pow4(s2t))/(3.*pow2(Msq)*pow4(Mt)) + (158*Dmglst2*pow2(z2)*pow3(Mst2)*
        pow4(s2t))/(3.*pow4(Mt)) + (71378675209*pow4(Mst1)*pow4(s2t))/(1.66698e8
        *pow4(Mt)) + (12*B4*pow4(Mst1)*pow4(s2t))/pow4(Mt) + (4*D3*pow4(Mst1)
        *pow4(s2t))/(3.*pow4(Mt)) - (4*DN*pow4(Mst1)*pow4(s2t))/(3.*pow4(Mt)) +
        (126628974763*Dmglst2*pow4(Mst1)*pow4(s2t))/(1.250235e8*Mst2*pow4(Mt))
        + (7780*OepS2*pow4(Mst1)*pow4(s2t))/(243.*pow4(Mt)) - (67184*Dmglst2*
        OepS2*pow4(Mst1)*pow4(s2t))/(729.*Mst2*pow4(Mt)) - (18995*S2*pow4(Mst1)
        *pow4(s2t))/(9.*pow4(Mt)) + (103270*Dmglst2*S2*pow4(Mst1)*pow4(s2t))/(
        27.*Mst2*pow4(Mt)) - (3890*T1ep*pow4(Mst1)*pow4(s2t))/(81.*pow4(Mt)) +
        (33592*Dmglst2*T1ep*pow4(Mst1)*pow4(s2t))/(243.*Mst2*pow4(Mt)) - (
        150437*z2*pow4(Mst1)*pow4(s2t))/(324.*pow4(Mt)) + (2200091*Dmglst2*z2*
        pow4(Mst1)*pow4(s2t))/(1215.*Mst2*pow4(Mt)) + (17035*z3*pow4(Mst1)*
        pow4(s2t))/(81.*pow4(Mt)) - (197843*Dmglst2*z3*pow4(Mst1)*pow4(s2t))/(
        243.*Mst2*pow4(Mt)) - (1297*z4*pow4(Mst1)*pow4(s2t))/(81.*pow4(Mt)) + (
        16796*Dmglst2*z4*pow4(Mst1)*pow4(s2t))/(243.*Mst2*pow4(Mt)) - (125*
        Dmglst2*Mst2*pow4(Mst1)*pow4(s2t))/(9.*pow2(Msq)*pow4(Mt)) - (20*
        Dmglst2*Mst2*z2*pow4(Mst1)*pow4(s2t))/(pow2(Msq)*pow4(Mt)) + (10*pow2(
        Msq)*pow4(Mst1)*pow4(s2t))/(pow2(Mst2)*pow4(Mt)) - (20*z2*pow2(Msq)*
        pow4(Mst1)*pow4(s2t))/(pow2(Mst2)*pow4(Mt)) + (2953*pow2(Mst2)*pow4(
        Mst1)*pow4(s2t))/(90.*pow2(Msq)*pow4(Mt)) - (10*z2*pow2(Mst2)*pow4(
        Mst1)*pow4(s2t))/(pow2(Msq)*pow4(Mt)) - (1945*pow2(z2)*pow4(Mst1)*pow4(
        s2t))/(54.*pow4(Mt)) + (8398*Dmglst2*pow2(z2)*pow4(Mst1)*pow4(s2t))/(
        81.*Mst2*pow4(Mt)) + (565*Dmglst2*pow3(Mst2)*pow4(Mst1)*pow4(s2t))/(36.
        *pow4(Msq)*pow4(Mt)) - (50*Dmglst2*z2*pow3(Mst2)*pow4(Mst1)*pow4(s2t))/
        (3.*pow4(Msq)*pow4(Mt)) + (9403*pow4(Mst2)*pow4(s2t))/(54.*pow4(Mt)) -
        (40*B4*pow4(Mst2)*pow4(s2t))/(3.*pow4(Mt)) + (4*D3*pow4(Mst2)*pow4(s2t)
        )/(3.*pow4(Mt)) - (2*DN*pow4(Mst2)*pow4(s2t))/(3.*pow4(Mt)) + (64*
        OepS2*pow4(Mst2)*pow4(s2t))/(9.*pow4(Mt)) - (3168*S2*pow4(Mst2)*pow4(
        s2t))/pow4(Mt) - (32*T1ep*pow4(Mst2)*pow4(s2t))/(3.*pow4(Mt)) - (529*
        z2*pow4(Mst2)*pow4(s2t))/(9.*pow4(Mt)) + (1913*z3*pow4(Mst2)*pow4(s2t))
        /(3.*pow4(Mt)) - (10*z4*pow4(Mst2)*pow4(s2t))/(3.*pow4(Mt)) + (2720*
        pow2(Dmglst2)*pow4(Mst2)*pow4(s2t))/(9.*pow2(Msq)*pow4(Mt)) - (190*z2*
        pow2(Dmglst2)*pow4(Mst2)*pow4(s2t))/(pow2(Msq)*pow4(Mt)) - (1175*pow2(
        Dmglst2)*pow4(Mst2)*pow4(s2t))/(6.*pow2(Mst1)*pow4(Mt)) + (104*z2*pow2(
        Dmglst2)*pow4(Mst2)*pow4(s2t))/(3.*pow2(Mst1)*pow4(Mt)) + (10*pow2(Msq)
        *pow4(Mst2)*pow4(s2t))/(pow2(Mst1)*pow4(Mt)) - (20*z2*pow2(Msq)*pow4(
        Mst2)*pow4(s2t))/(pow2(Mst1)*pow4(Mt)) - (2636*pow2(Mst1)*pow4(Mst2)*
        pow4(s2t))/(45.*pow2(Msq)*pow4(Mt)) + (100*z2*pow2(Mst1)*pow4(Mst2)*
        pow4(s2t))/(3.*pow2(Msq)*pow4(Mt)) + (16*pow2(z2)*pow4(Mst2)*pow4(s2t))
        /pow4(Mt) - (16865*pow2(Dmglst2)*pow2(Mst1)*pow4(Mst2)*pow4(s2t))/(72.*
        pow4(Msq)*pow4(Mt)) + (175*z2*pow2(Dmglst2)*pow2(Mst1)*pow4(Mst2)*pow4(
        s2t))/(pow4(Msq)*pow4(Mt)) + (2024719*pow4(Mst1)*pow4(Mst2)*pow4(s2t))/
        (205800.*pow4(Msq)*pow4(Mt)) - (25*z2*pow4(Mst1)*pow4(Mst2)*pow4(s2t))/
        (6.*pow4(Msq)*pow4(Mt)) - (291022313156*Dmglst2*pow4(Mst1))/(6.251175e6
        *pow5(Mst2)) + (1428992*Dmglst2*OepS2*pow4(Mst1))/(
        729.*pow5(Mst2)) - (32019872*Dmglst2*S2*pow4(Mst1))/(945.*pow5(Mst2)) -
        (714496*Dmglst2*T1ep*pow4(Mst1))/(243.*pow5(Mst2)) - (1089695168*
        Dmglst2*z2*pow4(Mst1))/(8505.*pow5(Mst2)) + (35982464*Dmglst2*z3*pow4(
        Mst1))/(243.*pow5(Mst2)) - (357248*Dmglst2*z4*pow4(Mst1))/(243.*pow5(
        Mst2)) - (178624*Dmglst2*pow2(z2)*pow4(Mst1))/(81.*pow5(Mst2)) + (4480*
        Dmglst2*pow2(s2t)*pow5(Mst2))/(9.*pow2(Msq)*pow2(Mst1)*pow2(Mt)) - (
        5680*pow3(s2t)*pow5(Mst2))/(9.*pow2(Msq)*pow3(Mt)) + (640*z2*pow3(s2t)*
        pow5(Mst2))/(3.*pow2(Msq)*pow3(Mt)) + (256*pow3(s2t)*pow5(Mst2))/(3.*
        pow2(Mst1)*pow3(Mt)) - (8240*s2t*pow5(Mst2))/(9.*Mt*pow4(Msq)) + (640*
        s2t*z2*pow5(Mst2))/(Mt*pow4(Msq)) - (260*Dmglst2*pow5(Mst2))/(pow2(
        Mst1)*pow4(Msq)) - (2030*Dmglst2*pow2(s2t)*pow5(Mst2))/(9.*pow2(Mt)*
        pow4(Msq)) - (26140*pow2(Dmglst2)*pow3(s2t)*pow5(Mst2))/(9.*pow3(Mt)*
        pow4(Msq)) + (1840*z2*pow2(Dmglst2)*pow3(s2t)*pow5(Mst2))/(pow3(Mt)*
        pow4(Msq)) + (1115*pow2(Mst1)*pow3(s2t)*pow5(Mst2))/(9.*pow3(Mt)*pow4(
        Msq)) - (40*z2*pow2(Mst1)*pow3(s2t)*pow5(Mst2))/(pow3(Mt)*pow4(Msq)) +
        (2300*Dmglst2*pow4(s2t)*pow5(Mst2))/(9.*pow2(Msq)*pow4(Mt)) - (340*
        Dmglst2*z2*pow4(s2t)*pow5(Mst2))/(3.*pow2(Msq)*pow4(Mt)) - (115*
        Dmglst2*pow4(s2t)*pow5(Mst2))/(3.*pow2(Mst1)*pow4(Mt)) + (184*Dmglst2*
        z2*pow4(s2t)*pow5(Mst2))/(3.*pow2(Mst1)*pow4(Mt)) - (3965*Dmglst2*pow2(
        Mst1)*pow4(s2t)*pow5(Mst2))/(36.*pow4(Msq)*pow4(Mt)) + (70*Dmglst2*z2*
        pow2(Mst1)*pow4(s2t)*pow5(Mst2))/(pow4(Msq)*pow4(Mt)) - (pow2(log(pow2(
        Mst1)/pow2(Msq)))*(105*log(pow2(Mst1)/pow2(Mst2))*pow2(Mst1)*pow2(Mst2)
        *pow4(Msq)*(8*(-pow2(Mst1) + pow2(Mst2))*pow2(Mt)*pow2(s2t) + 64*(
        Dmglst2 + Mst2)*s2t*pow3(Mt) - 16*(Dmglst2 + Mst2)*Mt*(pow2(Mst1) +
        pow2(Mst2))*pow3(s2t) + 16*pow4(Mt) + (pow2(Mst1) - pow2(Mst2))*(12*
        Dmglst2*Mst2 + 6*pow2(Dmglst2) + pow2(Mst1) + 7*pow2(Mst2))*pow4(s2t))
        + 105*pow2(Dmglst2)*(16*pow2(Mst1)*pow2(Mst2)*(-((Mt - 4*Mst2*s2t)*
        pow2(Mst1)) - (11*Mt + 4*Mst2*s2t)*pow2(Mst2))*pow3(Mt) - 64*pow2(Msq)*
        pow2(Mst1)*pow2(Mst2)*pow4(Mt) + 3*pow4(Msq)*(-pow2(-4*Mst2*pow2(Mt) +
        pow2(s2t)*pow3(Mst2)) + pow4(Mst1)*(8*pow2(Mt)*pow2(s2t) + pow2(Mst2)*
        pow4(s2t)) + pow2(Mst1)*(-16*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 16*pow4(
        Mt) + pow4(Mst2)*pow4(s2t)))) - 70*Dmglst2*Mst2*(-64*Mst2*pow2(Msq)*
        pow2(Mst1)*(-3*Mst2*Mt + s2t*pow2(Mst1) - s2t*pow2(Mst2))*pow3(Mt) -
        16*Mst2*pow2(Mst1)*pow3(Mt)*(3*Mst2*(-Mt + 2*Mst2*s2t)*pow2(Mst1) - (
        13*Mt + 7*Mst2*s2t)*pow3(Mst2) + s2t*pow4(Mst1)) + 3*pow4(Msq)*(3*pow2(
        -4*Mst2*pow2(Mt) + pow2(s2t)*pow3(Mst2)) - pow2(s2t)*(16*Mst2*Mt*s2t +
        24*pow2(Mt) + 3*pow2(Mst2)*pow2(s2t))*pow4(Mst1) + pow2(Mst1)*(48*pow2(
        Mst2)*pow2(Mt)*pow2(s2t) + 16*Mt*pow3(Mst2)*pow3(s2t) + 48*pow4(Mt) -
        3*pow4(Mst2)*pow4(s2t)) + 3*pow4(s2t)*pow6(Mst1))) - pow2(Mst2)*(224*
        pow2(Msq)*pow2(Mst1)*pow2(Mt)*(pow2(Mst1)*(-20*Mst2*Mt*s2t + 7*pow2(Mt)
        - 2*pow2(Mst2)*pow2(s2t)) + pow2(Mst2)*(20*Mst2*Mt*s2t + 37*pow2(Mt) +
        pow2(Mst2)*pow2(s2t)) + pow2(s2t)*pow4(Mst1)) + 4*pow2(Mst1)*pow2(Mt)*(
        840*Mst2*Mt*s2t + 1234*pow2(Mt) + 15*pow2(Mst2)*pow2(s2t))*pow4(Mst2) +
        4*pow2(Mst2)*pow4(Mst1)*(-15*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 560*Mst2*
        s2t*pow3(Mt) + 420*pow4(Mt) + pow4(Mst2)*pow4(s2t)) + 2*(-30*pow2(Mst2)
        *pow2(Mt)*pow2(s2t) - 560*Mst2*s2t*pow3(Mt) + 228*pow4(Mt) - 3*pow4(
        Mst2)*pow4(s2t))*pow6(Mst1) + 35*pow4(Msq)*(9*pow2(-4*Mst2*pow2(Mt) +
        pow2(s2t)*pow3(Mst2)) - 3*pow2(s2t)*(32*Mst2*Mt*s2t + 24*pow2(Mt) + 7*
        pow2(Mst2)*pow2(s2t))*pow4(Mst1) + pow2(Mst1)*(144*pow2(Mst2)*pow2(Mt)*
        pow2(s2t) + 96*Mt*pow3(Mst2)*pow3(s2t) + 176*pow4(Mt) - 3*pow4(Mst2)*
        pow4(s2t)) + 15*pow4(s2t)*pow6(Mst1)))))/(21.*pow2(Mst1)*pow2(Mst2)*
        pow4(Msq)*pow4(Mt)) + (1720*pow2(s2t)*pow6(Mst2))/(9.*pow2(Msq)*pow2(
        Mst1)*pow2(Mt)) - (670*pow6(Mst2))/(9.*pow2(Mst1)*pow4(Msq)) - (107588*
        pow2(s2t)*pow6(Mst2))/(1715.*pow2(Mt)*pow4(Msq)) + (45*pow2(Dmglst2)*
        pow2(s2t)*pow6(Mst2))/(pow2(Mst1)*pow2(Mt)*pow4(Msq)) - (27955*Dmglst2*
        pow3(s2t)*pow6(Mst2))/(27.*pow3(Mt)*pow4(Msq)) + (1640*Dmglst2*z2*pow3(
        s2t)*pow6(Mst2))/(3.*pow3(Mt)*pow4(Msq)) + (128*pow2(s2t)*pow6(Mst2))/(
        3.*pow2(Mt)*pow4(Mst1)) + (2986*pow4(s2t)*pow6(Mst2))/(45.*pow2(Msq)*
        pow4(Mt)) - (70*z2*pow4(s2t)*pow6(Mst2))/(3.*pow2(Msq)*pow4(Mt)) + (
        135*pow4(s2t)*pow6(Mst2))/(2.*pow2(Mst1)*pow4(Mt)) + (98*z2*pow4(s2t)*
        pow6(Mst2))/(3.*pow2(Mst1)*pow4(Mt)) - (80*pow2(Dmglst2)*pow4(s2t)*
        pow6(Mst2))/(3.*pow2(Msq)*pow2(Mst1)*pow4(Mt)) + (16435*pow2(Dmglst2)*
        pow4(s2t)*pow6(Mst2))/(72.*pow4(Msq)*pow4(Mt)) - (150*z2*pow2(Dmglst2)*
        pow4(s2t)*pow6(Mst2))/(pow4(Msq)*pow4(Mt)) - (45671209*pow2(Mst1)*pow4(
        s2t)*pow6(Mst2))/(1.8522e6*pow4(Msq)*pow4(Mt)) + (35*z2*pow2(Mst1)*
        pow4(s2t)*pow6(Mst2))/(3.*pow4(Msq)*pow4(Mt)) + (64*pow2(Dmglst2)*pow4(
        s2t)*pow6(Mst2))/(3.*pow4(Mst1)*pow4(Mt)) + (2*pow3(log(pow2(Mst1)/
        pow2(Mst2)))*(4*Mst2*pow4(Mst1)*(337*pow2(Mst2)*pow2(Mt)*pow2(s2t) -
        256*Mst2*s2t*pow3(Mt) + 400*Mt*pow3(Mst2)*pow3(s2t) - 2420*pow4(Mt) +
        68*pow4(Mst2)*pow4(s2t)) + pow2(Mst1)*pow3(Mst2)*(2152*pow2(Mst2)*pow2(
        Mt)*pow2(s2t) - 4608*Mst2*s2t*pow3(Mt) + 1920*Mt*pow3(Mst2)*pow3(s2t) -
        2320*pow4(Mt) + 185*pow4(Mst2)*pow4(s2t)) + 2*Dmglst2*(pow2(Mst1)*pow2(
        Mst2)*(2312*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 768*Mst2*s2t*pow3(Mt) +
        960*Mt*pow3(Mst2)*pow3(s2t) - 3824*pow4(Mt) - 77*pow4(Mst2)*pow4(s2t))
        + 24*pow4(Mst2)*(32*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 128*Mst2*s2t*pow3(
        Mt) + 72*Mt*pow3(Mst2)*pow3(s2t) - 64*pow4(Mt) + 19*pow4(Mst2)*pow4(
        s2t)) + pow4(Mst1)*(2896*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 2304*Mst2*s2t*
        pow3(Mt) + 672*Mt*pow3(Mst2)*pow3(s2t) + 5536*pow4(Mt) + 33*pow4(Mst2)*
        pow4(s2t))) + 12*(-8*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 512*Mst2*s2t*pow3(
        Mt) + 160*Mt*pow3(Mst2)*pow3(s2t) + 74*pow4(Mt) + 47*pow4(Mst2)*pow4(
        s2t))*pow5(Mst2) + 3*Mst2*pow2(Dmglst2)*(pow2(Mst1)*(760*pow2(Mst2)*
        pow2(Mt)*pow2(s2t) - 1024*Mst2*s2t*pow3(Mt) + 1776*pow4(Mt) - 19*pow4(
        Mst2)*pow4(s2t)) + 8*(32*pow2(Mt)*pow2(s2t)*pow4(Mst2) - 64*pow2(Mst2)*
        pow4(Mt) + 96*Mt*pow3(s2t)*pow5(Mst2) + 19*pow4(s2t)*pow6(Mst2)))))/(9.
        *pow4(Mt)*pow5(Mst2)) + (log(pow2(Mst1)/pow2(Msq))*(-88200*pow3(Mst2)*
        pow6(Msq)*(pow2(-4*Mst2*pow2(Mt) + pow2(s2t)*pow3(Mst2)) - pow4(Mst1)*(
        8*pow2(Mt)*pow2(s2t) + pow2(Mst2)*pow4(s2t)) + pow2(Mst1)*(16*pow2(
        Mst2)*pow2(Mt)*pow2(s2t) + 16*pow4(Mt) - pow4(Mst2)*pow4(s2t)) + pow4(
        s2t)*pow6(Mst1)) + 1225*Mst2*pow4(Msq)*(-2*pow2(Mst1)*pow4(Mst2)*(720*
        pow2(Mst2)*pow2(Mt)*pow2(s2t) - 288*Mst2*s2t*(-13 + 8*z2)*pow3(Mt) -
        864*Mt*(-2 + z2)*pow3(Mst2)*pow3(s2t) - 32*(13 + 18*z2)*pow4(Mt) + 3*(7
        + 30*z2)*pow4(Mst2)*pow4(s2t)) + 6*pow2(Mst2)*pow4(Mst1)*(24*pow2(Mst2)
        *pow2(Mt)*pow2(s2t) - 960*Mst2*s2t*pow3(Mt) - 96*Mt*(-3 + z2)*pow3(
        Mst2)*pow3(s2t) + 384*(3 + z2)*pow4(Mt) + (89 + 36*z2)*pow4(Mst2)*pow4(
        s2t)) + 3*(-768*Mst2*s2t*(1 + 2*z2)*pow3(Mt) + 48*Mt*pow3(Mst2)*pow3(
        s2t) + 288*(7 + 12*z2)*pow4(Mt) - (131 + 12*z2)*pow4(Mst2)*pow4(s2t))*
        pow6(Mst1) - 90*pow2(-4*pow2(Mt) + pow2(Mst2)*pow2(s2t))*pow6(Mst2)) +
        pow5(Mst2)*(8*pow2(Mst1)*pow2(Mt)*(58800*Mst2*Mt*s2t + 220709*pow2(Mt)
        + 58635*pow2(Mst2)*pow2(s2t))*pow4(Mst2) + pow2(Mst2)*pow4(Mst1)*(-
        351480*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 156800*Mst2*s2t*pow3(Mt) -
        382200*Mt*pow3(Mst2)*pow3(s2t) + 470400*pow4(Mt) + 51313*pow4(Mst2)*
        pow4(s2t)) + 8*(14865*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 39200*Mst2*s2t*
        pow3(Mt) + 3675*Mt*pow3(Mst2)*pow3(s2t) + 49209*pow4(Mt) - 3459*pow4(
        Mst2)*pow4(s2t))*pow6(Mst1) + 411600*pow4(Mt)*pow6(Mst2)) - 196*pow2(
        Msq)*pow5(Mst2)*(pow4(Mst1)*(1256*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 5600*
        Mst2*s2t*pow3(Mt) + 4800*Mt*pow3(Mst2)*pow3(s2t) - 656*pow4(Mt) - 345*
        pow4(Mst2)*pow4(s2t)) + pow2(Mst1)*pow2(Mst2)*(-5128*pow2(Mst2)*pow2(
        Mt)*pow2(s2t) + 5600*Mst2*s2t*pow3(Mt) - 4800*Mt*pow3(Mst2)*pow3(s2t) -
        1856*pow4(Mt) + 645*pow4(Mst2)*pow4(s2t)) + (872*pow2(Mt)*pow2(s2t) +
        255*pow2(Mst2)*pow4(s2t))*pow6(Mst1) + 3000*(-2*pow4(Mst2)*pow4(Mt) +
        pow2(Mt)*pow2(s2t)*pow6(Mst2))) - 2450*Dmglst2*(-(pow5(Mst2)*(4*Mt*
        pow2(Mst1)*pow3(Mst2)*(80*Mst2*s2t*pow2(Mt) + 270*Mt*pow2(Mst2)*pow2(
        s2t) + 548*pow3(Mt) + 181*pow3(Mst2)*pow3(s2t)) + 3*Mst2*pow4(Mst1)*(-
        216*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 64*Mst2*s2t*pow3(Mt) - 244*Mt*pow3(
        Mst2)*pow3(s2t) + 128*pow4(Mt) + 19*pow4(Mst2)*pow4(s2t)) + 1008*pow4(
        Mt)*pow5(Mst2) + s2t*(72*Mst2*s2t*pow2(Mt) + 12*Mt*pow2(Mst2)*pow2(s2t)
        - 128*pow3(Mt) + 3*pow3(Mst2)*pow3(s2t))*pow6(Mst1))) - 8*pow2(Msq)*
        pow5(Mst2)*(-120*pow2(Mt)*(-2*pow2(Mt) + pow2(Mst2)*pow2(s2t))*pow3(
        Mst2) + s2t*(-120*Mst2*s2t*pow2(Mt) - 144*Mt*pow2(Mst2)*pow2(s2t) + 56*
        pow3(Mt) + 3*pow3(Mst2)*pow3(s2t))*pow4(Mst1) - 8*Mst2*pow2(Mst1)*(-30*
        pow2(Mst2)*pow2(Mt)*pow2(s2t) + 7*Mst2*s2t*pow3(Mt) - 18*Mt*pow3(Mst2)*
        pow3(s2t) - 18*pow4(Mt) + 3*pow4(Mst2)*pow4(s2t)) + 6*Mst2*pow4(s2t)*
        pow6(Mst1)) + 2*pow4(Msq)*(pow2(Mst1)*pow4(Mst2)*(-120*pow2(Mst2)*pow2(
        Mt)*pow2(s2t) - 16*Mst2*s2t*(-161 + 72*z2)*pow3(Mt) - 144*Mt*(-15 + 11*
        z2)*pow3(Mst2)*pow3(s2t) + 368*pow4(Mt) + 27*(-5 + 4*z2)*pow4(Mst2)*
        pow4(s2t)) - 3*pow2(Mst2)*pow4(Mst1)*(128*pow2(Mst2)*pow2(Mt)*pow2(s2t)
        + 1248*Mst2*s2t*pow3(Mt) - 48*Mt*(2 + z2)*pow3(Mst2)*pow3(s2t) - 96*(27
        + 4*z2)*pow4(Mt) + 9*(-3 + 4*z2)*pow4(Mst2)*pow4(s2t)) + 36*(-16*pow2(
        Mst2)*pow2(Mt)*pow2(s2t) - 32*Mst2*s2t*(7 + 3*z2)*pow3(Mt) + 7*Mt*pow3(
        Mst2)*pow3(s2t) + 24*(23 + 12*z2)*pow4(Mt) + pow4(Mst2)*pow4(s2t))*
        pow6(Mst1) - 9*pow2(-4*pow2(Mt) + pow2(Mst2)*pow2(s2t))*pow6(Mst2))) +
        147*Mst2*pow2(Dmglst2)*(200*pow2(Msq)*pow4(Mst2)*(15*pow2(-4*Mst2*pow2(
        Mt) + pow2(s2t)*pow3(Mst2)) - 3*pow2(s2t)*(32*Mst2*Mt*s2t + 40*pow2(Mt)
        + 3*pow2(Mst2)*pow2(s2t))*pow4(Mst1) + 2*pow2(Mst1)*(120*pow2(Mst2)*
        pow2(Mt)*pow2(s2t) + 48*Mt*pow3(Mst2)*pow3(s2t) + 104*pow4(Mt) - 9*
        pow4(Mst2)*pow4(s2t))) + 2*pow4(Msq)*(975*pow2(-4*pow2(Mt) + pow2(Mst2)
        *pow2(s2t))*pow4(Mst2) + pow2(Mst1)*pow2(Mst2)*(16200*pow2(Mst2)*pow2(
        Mt)*pow2(s2t) + 8000*Mst2*s2t*pow3(Mt) + 400*Mt*(-107 + 72*z2)*pow3(
        Mst2)*pow3(s2t) + 4832*pow4(Mt) + 25*(31 - 36*z2)*pow4(Mst2)*pow4(s2t))
        + 25*pow4(Mst1)*(-864*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 3264*Mst2*s2t*
        pow3(Mt) - 112*Mt*pow3(Mst2)*pow3(s2t) + 96*(119 + 12*z2)*pow4(Mt) + (-
        167 + 36*z2)*pow4(Mst2)*pow4(s2t))) + 25*pow4(Mst2)*(pow2(Mst1)*pow2(
        Mst2)*(1704*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 128*Mst2*s2t*pow3(Mt) +
        960*Mt*pow3(Mst2)*pow3(s2t) + 2608*pow4(Mt) - 165*pow4(Mst2)*pow4(s2t))
        + pow4(Mst1)*(-888*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 128*Mst2*s2t*pow3(
        Mt) - 960*Mt*pow3(Mst2)*pow3(s2t) + 128*pow4(Mt) + 15*pow4(Mst2)*pow4(
        s2t)) - 840*(-2*pow4(Mst2)*pow4(Mt) + pow2(Mt)*pow2(s2t)*pow6(Mst2))))
        - 44100*pow2(Mst1)*pow2(log(pow2(Mst1)/pow2(Mst2)))*pow4(Msq)*(Mst2*(
        pow4(Mst1)*(64*Mst2*s2t*pow3(Mt) - 144*pow4(Mt) + pow4(Mst2)*pow4(s2t))
        + pow4(Mst2)*(-8*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 64*Mst2*s2t*pow3(Mt) +
        16*Mt*pow3(Mst2)*pow3(s2t) + 16*pow4(Mt) + 7*pow4(Mst2)*pow4(s2t)) - 4*
        pow2(Mst1)*(8*pow2(Mst2)*pow4(Mt) - 4*Mt*pow3(s2t)*pow5(Mst2) + pow4(
        s2t)*pow6(Mst2))) - 6*pow2(Dmglst2)*(pow2(Mst1)*(16*Mst2*pow4(Mt) +
        pow4(s2t)*pow5(Mst2)) - pow4(s2t)*pow7(Mst2)) + 4*Dmglst2*(48*(3*Mt -
        Mst2*s2t)*pow3(Mt)*pow4(Mst1) - 16*s2t*pow3(Mt)*pow5(Mst2) + pow2(Mst1)
        *(16*pow2(Mst2)*pow4(Mt) + 4*Mt*pow3(s2t)*pow5(Mst2) - 3*pow4(s2t)*
        pow6(Mst2)) + 4*Mt*pow3(s2t)*pow7(Mst2) + 3*pow4(s2t)*pow8(Mst2))) +
        105*log(pow2(Mst1)/pow2(Mst2))*(1680*pow2(Mst1)*(pow2(Mst1) - pow2(
        Mst2))*pow4(s2t)*pow5(Mst2)*pow6(Msq) + 70*Mst2*pow4(Msq)*(2*pow2(Mst2)
        *pow4(Mst1)*(-32*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 384*Mst2*s2t*pow3(Mt)
        + 72*Mt*pow3(Mst2)*pow3(s2t) + 720*pow4(Mt) + 21*pow4(Mst2)*pow4(s2t))
        - s2t*pow2(Mst1)*(128*Mst2*s2t*pow2(Mt) + 240*Mt*pow2(Mst2)*pow2(s2t) -
        384*pow3(Mt) + 59*pow3(Mst2)*pow3(s2t))*pow5(Mst2) + (-96*pow2(Mst2)*
        pow2(Mt)*pow2(s2t) - 2112*Mst2*s2t*pow3(Mt) + 48*Mt*pow3(Mst2)*pow3(
        s2t) + 4608*pow4(Mt) + 26*pow4(Mst2)*pow4(s2t))*pow6(Mst1) - 18*pow2(-
        4*pow2(Mt) + pow2(Mst2)*pow2(s2t))*pow6(Mst2)) + 28*pow2(Msq)*pow2(
        Mst1)*pow5(Mst2)*(-160*pow3(Mst2)*(6*s2t*pow3(Mt) - Mt*pow2(Mst1)*pow3(
        s2t)) + 20*pow2(Mt)*pow2(s2t)*pow4(Mst1) - pow4(Mst2)*(52*pow2(Mt)*
        pow2(s2t) + 41*pow2(Mst1)*pow4(s2t)) + pow2(Mst2)*(32*pow2(Mst1)*pow2(
        Mt)*pow2(s2t) - 304*pow4(Mt) + 6*pow4(Mst1)*pow4(s2t)) + 160*Mt*pow3(
        s2t)*pow5(Mst2) + 40*pow4(s2t)*pow6(Mst2)) + pow2(Mst1)*(-1680*pow3(
        Mst2)*(8*s2t*pow3(Mt) - Mt*pow2(Mst1)*pow3(s2t)) + 504*pow2(Mt)*pow2(
        s2t)*pow4(Mst1) - 2*pow4(Mst2)*(148*pow2(Mt)*pow2(s2t) + 211*pow2(Mst1)
        *pow4(s2t)) + pow2(Mst2)*(-264*pow2(Mst1)*pow2(Mt)*pow2(s2t) - 6304*
        pow4(Mt) + 187*pow4(Mst1)*pow4(s2t)))*pow7(Mst2) - 42*pow2(Dmglst2)*(
        10*pow2(Msq)*pow2(Mst1)*(128*Mst2*s2t*pow3(Mt) - 32*Mst2*Mt*(pow2(Mst1)
        + pow2(Mst2))*pow3(s2t) + 16*pow4(Mt) + (20*pow2(Mst1) - 19*pow2(Mst2))
        *pow2(Mst2)*pow4(s2t))*pow5(Mst2) + 2*pow4(Msq)*(pow2(Mst1)*pow3(Mst2)*
        (240*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 320*Mst2*s2t*pow3(Mt) - 80*Mt*
        pow3(Mst2)*pow3(s2t) + 416*pow4(Mt) - 85*pow4(Mst2)*pow4(s2t)) + Mst2*
        pow4(Mst1)*(640*Mst2*s2t*pow3(Mt) - 80*Mt*pow3(Mst2)*pow3(s2t) - 5200*
        pow4(Mt) + 65*pow4(Mst2)*pow4(s2t)) + 15*pow2(-4*pow2(Mt) + pow2(Mst2)*
        pow2(s2t))*pow5(Mst2)) + 5*pow2(Mst1)*(384*Mst2*s2t*pow3(Mt) - 80*Mst2*
        Mt*(pow2(Mst1) + pow2(Mst2))*pow3(s2t) + 96*pow4(Mt) + 5*(7*pow2(Mst1)
        - 6*pow2(Mst2))*pow2(Mst2)*pow4(s2t))*pow7(Mst2)) - 140*Dmglst2*(-2*
        pow2(Msq)*pow2(Mst1)*(-224*Mst2*s2t*pow3(Mt) + 48*Mst2*Mt*(pow2(Mst1) +
        pow2(Mst2))*pow3(s2t) - 48*pow4(Mt) + (-20*pow2(Mst1)*pow2(Mst2) + 3*
        pow4(Mst1) + 17*pow4(Mst2))*pow4(s2t))*pow6(Mst2) + 2*pow4(Msq)*(pow2(
        Mst1)*pow4(Mst2)*(144*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 160*Mst2*s2t*
        pow3(Mt) + 12*Mt*pow3(Mst2)*pow3(s2t) + 16*pow4(Mt) - 9*pow4(Mst2)*
        pow4(s2t)) - 3*pow2(Mst2)*pow4(Mst1)*(32*pow2(Mst2)*pow2(Mt)*pow2(s2t)
        + 64*Mst2*s2t*pow3(Mt) + 28*Mt*pow3(Mst2)*pow3(s2t) - 304*pow4(Mt) + 3*
        pow4(Mst2)*pow4(s2t)) + 12*Mt*(-164*Mst2*s2t*pow2(Mt) - 10*Mt*pow2(
        Mst2)*pow2(s2t) + 456*pow3(Mt) + 3*pow3(Mst2)*pow3(s2t))*pow6(Mst1) +
        9*pow2(-4*pow2(Mt) + pow2(Mst2)*pow2(s2t))*pow6(Mst2)) + pow2(Mst1)*(
        352*Mst2*s2t*pow3(Mt) - 60*Mst2*Mt*(pow2(Mst1) + pow2(Mst2))*pow3(s2t)
        + 128*pow4(Mt) + pow2(Mst1)*(-5*pow2(Mst1) + 21*pow2(Mst2))*pow4(s2t))*
        pow8(Mst2)))))/(4410.*pow2(Mst1)*pow4(Msq)*pow4(Mt)*pow5(Mst2)) - (log(
        pow2(Mst1)/pow2(Mst2))*(-294*Mst2*pow2(Dmglst2)*(-4*pow4(Msq)*(225*
        pow2(Mst1)*pow4(Mst2)*(-2376*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 4096*Mst2*
        s2t*pow3(Mt) + 1024*Mt*pow3(Mst2)*pow3(s2t) + 5776*pow4(Mt) + 297*pow4(
        Mst2)*pow4(s2t)) + pow2(Mst2)*pow4(Mst1)*(-1200*(2635 + 567*S2 - 3153*
        z2)*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 3200*Mst2*s2t*(-833 + 378*S2 - 45*
        z2)*pow3(Mt) + 1200*Mt*(-2201 + 252*S2 + 1755*z2)*pow3(Mst2)*pow3(s2t)
        + 32*(-317567 + 89775*S2 - 900*z2)*pow4(Mt) - 75*(-4681 + 126*S2 +
        1464*z2)*pow4(Mst2)*pow4(s2t)) + (-24*(143213 + 133650*S2 - 173700*z2)*
        pow2(Mst2)*pow2(Mt)*pow2(s2t) - 400*Mst2*s2t*(-42953 + 76104*S2 -
        10728*z2)*pow3(Mt) + 200*Mt*(-11798 + 24633*S2 + 3150*z2)*pow3(Mst2)*
        pow3(s2t) + 16*(-1582393 + 2551050*S2 - 798300*z2)*pow4(Mt) + (-361561
        - 511200*S2 + 89550*z2)*pow4(Mst2)*pow4(s2t))*pow6(Mst1) - 7200*pow2(-
        4*pow2(Mt) + pow2(Mst2)*pow2(s2t))*pow6(Mst2)) + 900*pow2(Msq)*(pow4(
        Mst1)*pow4(Mst2)*(-4800*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 640*Mst2*s2t*
        pow3(Mt) - 1600*Mt*pow3(Mst2)*pow3(s2t) - 1632*pow4(Mt) + 145*pow4(
        Mst2)*pow4(s2t)) + 20*pow2(Mst2)*(120*pow2(Mst2)*pow2(Mt)*pow2(s2t) -
        64*Mst2*s2t*pow3(Mt) + 112*Mt*pow3(Mst2)*pow3(s2t) - 32*pow4(Mt) + 17*
        pow4(Mst2)*pow4(s2t))*pow6(Mst1) - 300*pow2(Mst1)*pow2(-4*pow2(Mt) +
        pow2(Mst2)*pow2(s2t))*pow6(Mst2)) - 225*pow2(Mst1)*pow4(Mst2)*(-5*pow4(
        Mst1)*(1776*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 1408*Mst2*s2t*pow3(Mt) +
        2064*Mt*pow3(Mst2)*pow3(s2t) + 736*pow4(Mt) - 3*pow4(Mst2)*pow4(s2t)) -
        2*pow2(Mst1)*pow2(Mst2)*(-8520*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 320*
        Mst2*s2t*pow3(Mt) - 4440*Mt*pow3(Mst2)*pow3(s2t) - 8416*pow4(Mt) + 685*
        pow4(Mst2)*pow4(s2t)) - 8400*(-2*pow4(Mst2)*pow4(Mt) + pow2(Mt)*pow2(
        s2t)*pow6(Mst2)))) + 4*Dmglst2*(55125*pow2(Mst1)*pow4(Mst2)*(4*Mt*pow2(
        Mst1)*(-200*Mst2*s2t*pow2(Mt) + 540*Mt*pow2(Mst2)*pow2(s2t) + 1088*
        pow3(Mt) + 425*pow3(Mst2)*pow3(s2t))*pow4(Mst2) - 3*pow2(Mst2)*pow4(
        Mst1)*(432*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 64*Mst2*s2t*pow3(Mt) + 404*
        Mt*pow3(Mst2)*pow3(s2t) - 32*pow4(Mt) + pow4(Mst2)*pow4(s2t)) + (144*
        pow2(Mst2)*pow2(Mt)*pow2(s2t) + 480*Mst2*s2t*pow3(Mt) + 24*Mt*pow3(
        Mst2)*pow3(s2t) - 576*pow4(Mt) + 17*pow4(Mst2)*pow4(s2t))*pow6(Mst1) +
        2016*pow4(Mt)*pow6(Mst2)) - 2*pow4(Msq)*(-7350*pow4(Mst1)*pow4(Mst2)*(
        48*(-1676 + 891*z2)*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 8*Mst2*s2t*(-22768
        + 2835*S2 + 6462*z2)*pow3(Mt) - 18*Mt*(2990 + 315*S2 - 1986*z2)*pow3(
        Mst2)*pow3(s2t) + (90952 - 18144*S2 + 21888*z2)*pow4(Mt) + 9*(689 +
        126*S2 - 580*z2)*pow4(Mst2)*pow4(s2t)) - 294*pow2(Mst2)*(-8*(59657 +
        202500*S2 - 55800*z2)*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 200*Mst2*s2t*(-
        18428 + 37341*S2 - 9090*z2)*pow3(Mt) - 300*Mt*(-818 + 3609*S2 - 594*z2)
        *pow3(Mst2)*pow3(s2t) - 48*(8581 + 72450*S2 - 92700*z2)*pow4(Mt) + (-
        128893 + 291600*S2 + 100350*z2)*pow4(Mst2)*pow4(s2t))*pow6(Mst1) +
        66150*pow2(Mst1)*(-952*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 1024*Mst2*s2t*
        pow3(Mt) + 256*Mt*pow3(Mst2)*pow3(s2t) + 880*pow4(Mt) + 247*pow4(Mst2)*
        pow4(s2t))*pow6(Mst2) + (72*(-5217307 + 33912900*S2 - 73500*z2)*pow2(
        Mst2)*pow2(Mt)*pow2(s2t) - 78400*Mst2*s2t*(-3509 + 115785*S2 - 18144*
        z2)*pow3(Mt) + 4900*Mt*(24322 + 140139*S2 - 38826*z2)*pow3(Mst2)*pow3(
        s2t) + 80*(57911521 + 49233240*S2 - 40325040*z2)*pow4(Mt) + (1011403 -
        185175900*S2 + 17463600*z2)*pow4(Mst2)*pow4(s2t))*pow8(Mst1) +
        16934400*pow2(Mt)*(-2*pow2(Mt) + pow2(Mst2)*pow2(s2t))*pow8(Mst2)) +
        220500*pow2(Msq)*(-4*pow4(Mst2)*(120*pow2(Mst2)*pow2(Mt)*pow2(s2t) -
        168*Mst2*s2t*pow3(Mt) + 96*Mt*pow3(Mst2)*pow3(s2t) + 96*pow4(Mt) + 25*
        pow4(Mst2)*pow4(s2t))*pow6(Mst1) + pow4(Mst1)*(960*pow2(Mst2)*pow2(Mt)*
        pow2(s2t) - 1184*Mst2*s2t*pow3(Mt) + 768*Mt*pow3(Mst2)*pow3(s2t) + 576*
        pow4(Mt) + 13*pow4(Mst2)*pow4(s2t))*pow6(Mst2) + 24*pow2(Mst2)*(-2*
        pow2(Mst2)*pow2(Mt)*pow2(s2t) - 16*Mst2*s2t*pow3(Mt) + 120*pow4(Mt) +
        pow4(Mst2)*pow4(s2t))*pow8(Mst1) - 480*pow2(Mst1)*pow2(Mt)*(-2*pow2(Mt)
        + pow2(Mst2)*pow2(s2t))*pow8(Mst2))) - 3*Mst2*(5292000*(-1 + 2*z2)*(
        pow2(Mst1) - pow2(Mst2))*pow4(Mst1)*pow4(Mst2)*pow4(s2t)*pow6(Msq) +
        15*pow2(Mst1)*pow5(Mst2)*(-128*pow2(Mst1)*pow2(Mt)*(3675*Mst2*Mt*s2t +
        73246*pow2(Mt) + 14279*pow2(Mst2)*pow2(s2t))*pow3(Mst2) + 2*Mst2*pow4(
        Mst1)*(851136*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 1411200*Mst2*s2t*pow3(Mt)
        + 499800*Mt*pow3(Mst2)*pow3(s2t) - 1528800*pow4(Mt) - 45247*pow4(Mst2)*
        pow4(s2t)) - 1646400*pow4(Mt)*pow5(Mst2) + s2t*(-531552*Mst2*s2t*pow2(
        Mt) - 117600*Mt*pow2(Mst2)*pow2(s2t) + 1411200*pow3(Mt) + 34519*pow3(
        Mst2)*pow3(s2t))*pow6(Mst1)) + 2*pow4(Msq)*(2450*pow4(Mst1)*pow4(Mst2)*
        (-24*(-5434 + 1863*S2 + 1830*z2 - 72*z3)*pow2(Mst2)*pow2(Mt)*pow2(s2t)
        + 288*Mst2*s2t*(116 + 189*S2 - 878*z2)*pow3(Mt) - 72*Mt*(-2326 + 189*S2
        + 346*z2)*pow3(Mst2)*pow3(s2t) + 32*(722 + 1701*S2 + 3753*z2 + 108*z3)*
        pow4(Mt) + 3*(6377 + 2592*S2 + 2724*z2 - 72*z3)*pow4(Mst2)*pow4(s2t)) +
        1960*pow2(Mst2)*(-4*(-47993 + 86940*S2 + 3060*z2 + 540*z3)*pow2(Mst2)*
        pow2(Mt)*pow2(s2t) + 1080*Mst2*s2t*(102 + 377*S2 - 330*z2)*pow3(Mt) -
        180*Mt*(326 + 219*S2 - 294*z2)*pow3(Mst2)*pow3(s2t) + 8*(-37598 +
        28215*S2 + 30870*z2)*pow4(Mt) + (-18499 + 46305*S2 - 15075*z2)*pow4(
        Mst2)*pow4(s2t))*pow6(Mst1) + 44100*pow2(Mst1)*(-2312*pow2(Mst2)*pow2(
        Mt)*pow2(s2t) - 2048*Mst2*s2t*pow3(Mt) + 512*Mt*pow3(Mst2)*pow3(s2t) +
        4112*pow4(Mt) + 385*pow4(Mst2)*pow4(s2t))*pow6(Mst2) + (-24*(-20192173
        + 50839950*S2 + 2866500*z2)*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 940800*
        Mst2*s2t*(302 + 2055*S2 - 732*z2)*pow3(Mt) - 58800*Mt*(136 + 1317*S2 -
        918*z2)*pow3(Mst2)*pow3(s2t) + 32*(-47778491 + 25754400*S2 + 20374200*
        z2)*pow4(Mt) + 9*(-5929019 + 9530500*S2 + 367500*z2 + 58800*z3)*pow4(
        Mst2)*pow4(s2t))*pow8(Mst1) + 11289600*pow2(Mt)*(-2*pow2(Mt) + pow2(
        Mst2)*pow2(s2t))*pow8(Mst2)) + 5880*pow2(Msq)*pow2(Mst1)*pow2(Mst2)*(4*
        pow2(Mst2)*pow4(Mst1)*(1364*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 1200*Mst2*
        s2t*pow3(Mt) + 800*Mt*pow3(Mst2)*pow3(s2t) + 600*pow4(Mt) + 173*pow4(
        Mst2)*pow4(s2t)) - pow2(Mst1)*pow4(Mst2)*(8656*pow2(Mst2)*pow2(Mt)*
        pow2(s2t) - 33600*Mst2*s2t*pow3(Mt) + 16000*Mt*pow3(Mst2)*pow3(s2t) +
        27712*pow4(Mt) + 735*pow4(Mst2)*pow4(s2t)) + 2*(-2000*pow2(Mst2)*pow2(
        Mt)*pow2(s2t) - 9600*Mst2*s2t*pow3(Mt) + 28800*pow4(Mt) + 159*pow4(
        Mst2)*pow4(s2t))*pow6(Mst1) + 6000*(-2*pow4(Mt)*pow6(Mst2) + pow2(Mt)*
        pow2(s2t)*pow8(Mst2))))))/(793800.*pow4(Msq)*pow4(Mst1)*pow4(Mt)*pow5(
        Mst2)) + (pow2(log(pow2(Mst1)/pow2(Mst2)))*(-840*pow2(Msq)*pow3(Mst2)*
        pow4(Mst1)*(pow2(Mst1)*pow2(Mst2)*(-12*pow2(Mst2)*pow2(Mt)*pow2(s2t) -
        160*Mst2*s2t*pow3(Mt) + 80*Mt*pow3(Mst2)*pow3(s2t) + 80*pow4(Mt) - 19*
        pow4(Mst2)*pow4(s2t)) + pow4(Mst1)*(-120*pow4(Mt) + 9*pow4(Mst2)*pow4(
        s2t)) + 2*pow4(Mst2)*(-9*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 160*Mst2*s2t*
        pow3(Mt) + 40*Mt*pow3(Mst2)*pow3(s2t) + 72*pow4(Mt) + 10*pow4(Mst2)*
        pow4(s2t))) + 30*(192*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 2240*Mst2*s2t*
        pow3(Mt) - 840*Mt*pow3(Mst2)*pow3(s2t) - 1680*pow4(Mt) + 207*pow4(Mst2)
        *pow4(s2t))*pow6(Mst1)*pow7(Mst2) + 14*pow2(Dmglst2)*(450*pow2(Msq)*
        pow4(Mst1)*(128*Mst2*s2t*pow3(Mt) - 32*Mst2*Mt*(pow2(Mst1) + pow2(Mst2)
        )*pow3(s2t) - 16*pow4(Mt) + (20*pow2(Mst1) - 19*pow2(Mst2))*pow2(Mst2)*
        pow4(s2t))*pow5(Mst2) + pow4(Msq)*(pow3(Mst2)*pow4(Mst1)*(-4680*pow2(
        Mst2)*pow2(Mt)*pow2(s2t) + 152320*Mst2*s2t*pow3(Mt) - 44160*Mt*pow3(
        Mst2)*pow3(s2t) + 159056*pow4(Mt) - 15885*pow4(Mst2)*pow4(s2t)) + 120*
        pow2(Mst1)*(-200*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 384*Mst2*s2t*pow3(Mt)
        + 96*Mt*pow3(Mst2)*pow3(s2t) + 208*pow4(Mt) + 61*pow4(Mst2)*pow4(s2t))*
        pow5(Mst2) + 8*Mst2*(-16047*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 42580*Mst2*
        s2t*pow3(Mt) - 4020*Mt*pow3(Mst2)*pow3(s2t) + 99890*pow4(Mt) + 279*
        pow4(Mst2)*pow4(s2t))*pow6(Mst1) - 1440*pow2(-4*pow2(Mt) + pow2(Mst2)*
        pow2(s2t))*pow7(Mst2)) + 225*((64*Mst2*s2t*pow3(Mt) - 80*Mt*pow3(Mst2)*
        pow3(s2t) - 16*pow4(Mt) + 35*pow4(Mst2)*pow4(s2t))*pow5(Mst2)*pow6(
        Mst1) - 10*pow4(Mst1)*(-32*Mst2*s2t*pow3(Mt) + 8*Mt*pow3(Mst2)*pow3(
        s2t) + 8*pow4(Mt) + 3*pow4(Mst2)*pow4(s2t))*pow7(Mst2))) - 15*(-2240*
        Mst2*s2t*pow3(Mt) + 1680*pow4(Mt) + 223*pow4(Mst2)*pow4(s2t))*pow5(
        Mst2)*pow8(Mst1) + Mst2*pow4(Msq)*(280*pow4(Mst1)*pow4(Mst2)*(1536*
        pow2(Mst2)*pow2(Mt)*pow2(s2t) - 8880*Mst2*s2t*pow3(Mt) + 3372*Mt*pow3(
        Mst2)*pow3(s2t) + 3212*pow4(Mt) + 879*pow4(Mst2)*pow4(s2t)) - 70*pow2(
        Mst2)*(-9560*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 27648*Mst2*s2t*pow3(Mt) -
        5136*Mt*pow3(Mst2)*pow3(s2t) + 3136*pow4(Mt) + 289*pow4(Mst2)*pow4(s2t)
        )*pow6(Mst1) + 1680*pow2(Mst1)*(-120*pow2(Mst2)*pow2(Mt)*pow2(s2t) -
        128*Mst2*s2t*pow3(Mt) + 32*Mt*pow3(Mst2)*pow3(s2t) + 208*pow4(Mt) + 21*
        pow4(Mst2)*pow4(s2t))*pow6(Mst2) + (669176*pow2(Mst2)*pow2(Mt)*pow2(
        s2t) - 313600*Mst2*s2t*pow3(Mt) + 229600*Mt*pow3(Mst2)*pow3(s2t) -
        4378256*pow4(Mt) + 56822*pow4(Mst2)*pow4(s2t))*pow8(Mst1) + 26880*pow2(
        Mt)*(-2*pow2(Mt) + pow2(Mst2)*pow2(s2t))*pow8(Mst2)) + 4*Dmglst2*(-525*
        pow4(Mst1)*(20*Mt*pow3(Mst2)*(-12*Mst2*s2t*pow2(Mt) + 4*pow3(Mt) + 3*
        pow3(Mst2)*pow3(s2t)) + pow4(Mst1)*(-16*s2t*pow3(Mt) + 5*pow3(Mst2)*
        pow4(s2t)) + 3*Mst2*pow2(Mst1)*(-32*Mst2*s2t*pow3(Mt) + 20*Mt*pow3(
        Mst2)*pow3(s2t) + 16*pow4(Mt) - 7*pow4(Mst2)*pow4(s2t)))*pow5(Mst2) -
        1050*pow2(Msq)*pow2(Mst2)*pow4(Mst1)*(3*pow4(Mst1)*(16*pow4(Mt) + pow4(
        Mst2)*pow4(s2t)) + pow4(Mst2)*(-192*Mst2*s2t*pow3(Mt) + 48*Mt*pow3(
        Mst2)*pow3(s2t) + 48*pow4(Mt) + 17*pow4(Mst2)*pow4(s2t)) - 4*pow2(Mst1)
        *(8*s2t*pow3(Mst2)*pow3(Mt) - 12*Mt*pow3(s2t)*pow5(Mst2) + 5*pow4(s2t)*
        pow6(Mst2))) + pow4(Msq)*(35*pow4(Mst1)*pow4(Mst2)*(9384*pow2(Mst2)*
        pow2(Mt)*pow2(s2t) + 11168*Mst2*s2t*pow3(Mt) + 4056*Mt*pow3(Mst2)*pow3(
        s2t) - 16912*pow4(Mt) + 717*pow4(Mst2)*pow4(s2t)) - 28*pow2(Mst2)*(-
        998*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 36680*Mst2*s2t*pow3(Mt) + 3150*Mt*
        pow3(Mst2)*pow3(s2t) + 37236*pow4(Mt) + 1411*pow4(Mst2)*pow4(s2t))*
        pow6(Mst1) + 840*pow2(Mst1)*(-136*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 192*
        Mst2*s2t*pow3(Mt) + 48*Mt*pow3(Mst2)*pow3(s2t) + 208*pow4(Mt) + 29*
        pow4(Mst2)*pow4(s2t))*pow6(Mst2) + (294096*pow2(Mst2)*pow2(Mt)*pow2(
        s2t) - 394240*Mst2*s2t*pow3(Mt) - 31080*Mt*pow3(Mst2)*pow3(s2t) +
        2542880*pow4(Mt) - 6102*pow4(Mst2)*pow4(s2t))*pow8(Mst1) + 26880*pow2(
        Mt)*(-2*pow2(Mt) + pow2(Mst2)*pow2(s2t))*pow8(Mst2))) + 240*pow2(Mt)*(
        420*Mst2*Mt*s2t - 223*pow2(Mt) + 11*pow2(Mst2)*pow2(s2t))*pow4(Mst1)*
        pow9(Mst2)))/(630.*pow4(Msq)*pow4(Mst1)*pow4(Mt)*pow5(Mst2));

   return result;
}

double himalaya::H6::calc_coef_at_as2_no_sm_logs_log1() const {

   const double result =
      -474.3703703703704 - (115472*Dmglst2)/(27.*Mst2) + (204928*Dmglst2*s2t)/(
        27.*Mt) + (1472*Mst2*s2t)/(3.*Mt) + (4304*z2)/3. - (1536*Dmglst2*z2)/
        Mst2 - (6656*Dmglst2*s2t*z2)/(3.*Mt) - (7552*Mst2*s2t*z2)/(3.*Mt) +
        128*z3 - (49024*s2t*pow2(Dmglst2))/(27.*Mst2*Mt) - (1216*s2t*z2*pow2(
        Dmglst2))/(3.*Mst2*Mt) + (160*Dmglst2*Mst2)/(3.*pow2(Msq)) + (1648*
        pow2(Dmglst2))/(3.*pow2(Msq)) + (640*Mst2*s2t*pow2(Dmglst2))/(3.*Mt*
        pow2(Msq)) + (2240*Dmglst2*Mst2)/(3.*pow2(Mst1)) - (3680*pow2(Dmglst2))
        /(3.*pow2(Mst1)) + (4096*Mst2*s2t*pow2(Dmglst2))/(3.*Mt*pow2(Mst1)) - (
        320*pow2(Msq))/pow2(Mst1) + (10496*s2t*pow2(Mst1))/(3.*Mst2*Mt) - (
        8192*s2t*z2*pow2(Mst1))/(3.*Mst2*Mt) + (160*pow2(Mst1))/(9.*pow2(Msq))
        + (640*Dmglst2*pow2(Mst1))/(3.*Mst2*pow2(Msq)) - (2240*Dmglst2*s2t*
        pow2(Mst1))/(9.*Mt*pow2(Msq)) + (1600*Mst2*s2t*pow2(Mst1))/(9.*Mt*pow2(
        Msq)) - (640*s2t*pow2(Dmglst2)*pow2(Mst1))/(3.*Mst2*Mt*pow2(Msq)) + (
        6588784*pow2(Dmglst2))/(675.*pow2(Mst2)) - (704*z2*pow2(Dmglst2))/(3.*
        pow2(Mst2)) - (320*pow2(Msq))/pow2(Mst2) - (6208*pow2(Mst1))/(3.*pow2(
        Mst2)) + (11648*Dmglst2*s2t*pow2(Mst1))/(3.*Mt*pow2(Mst2)) + (1600*z2*
        pow2(Mst1))/pow2(Mst2) + (9088*Dmglst2*s2t*z2*pow2(Mst1))/(3.*Mt*pow2(
        Mst2)) - (320*pow2(Dmglst2)*pow2(Mst1))/(3.*pow2(Msq)*pow2(Mst2)) - (
        1040*pow2(Mst2))/(3.*pow2(Msq)) + (640*Dmglst2*s2t*pow2(Mst2))/(3.*Mt*
        pow2(Msq)) + (3424*pow2(Mst2))/(3.*pow2(Mst1)) - (2048*Dmglst2*s2t*
        pow2(Mst2))/(3.*Mt*pow2(Mst1)) + (57040*Dmglst2*Mst2*pow2(s2t))/(9.*
        pow2(Mt)) - (3072*Dmglst2*Mst2*z2*pow2(s2t))/pow2(Mt) + (56968*pow2(
        Dmglst2)*pow2(s2t))/(9.*pow2(Mt)) - (5632*z2*pow2(Dmglst2)*pow2(s2t))/
        pow2(Mt) - (320*pow2(Msq)*pow2(s2t))/pow2(Mt) + (304*pow2(Mst1)*pow2(
        s2t))/pow2(Mt) + (31280*Dmglst2*pow2(Mst1)*pow2(s2t))/(9.*Mst2*pow2(Mt)
        ) - (2048*Dmglst2*z2*pow2(Mst1)*pow2(s2t))/(Mst2*pow2(Mt)) + (63224*
        pow2(Dmglst2)*pow2(Mst1)*pow2(s2t))/(9.*pow2(Mst2)*pow2(Mt)) - (5120*
        z2*pow2(Dmglst2)*pow2(Mst1)*pow2(s2t))/(pow2(Mst2)*pow2(Mt)) + (160*
        pow2(Msq)*pow2(Mst1)*pow2(s2t))/(pow2(Mst2)*pow2(Mt)) + (8080*pow2(
        Mst2)*pow2(s2t))/(3.*pow2(Mt)) - (512*z2*pow2(Mst2)*pow2(s2t))/pow2(Mt)
        + (1328*pow2(Dmglst2)*pow2(Mst2)*pow2(s2t))/(3.*pow2(Mst1)*pow2(Mt)) +
        (160*pow2(Msq)*pow2(Mst2)*pow2(s2t))/(pow2(Mst1)*pow2(Mt)) + (8192*
        Dmglst2*pow2(Mst1))/(9.*pow3(Mst2)) - (8960*Dmglst2*z2*pow2(Mst1))/
        pow3(Mst2) - (102400*s2t*pow2(Dmglst2)*pow2(Mst1))/(9.*Mt*pow3(Mst2)) -
        (11968*s2t*z2*pow2(Dmglst2)*pow2(Mst1))/(3.*Mt*pow3(Mst2)) - (640*s2t*
        pow3(Mst2))/(9.*Mt*pow2(Msq)) - (2048*s2t*pow3(Mst2))/(3.*Mt*pow2(Mst1)
        ) - (2144*Dmglst2*pow2(s2t)*pow3(Mst2))/(3.*pow2(Mst1)*pow2(Mt)) + (
        14000*Mst2*pow2(Dmglst2)*pow3(s2t))/(9.*pow3(Mt)) - (1152*Mst2*z2*pow2(
        Dmglst2)*pow3(s2t))/pow3(Mt) - (1184*Dmglst2*pow2(Mst1)*pow3(s2t))/
        pow3(Mt) - (4736*Mst2*pow2(Mst1)*pow3(s2t))/(3.*pow3(Mt)) + (544*
        Dmglst2*z2*pow2(Mst1)*pow3(s2t))/(3.*pow3(Mt)) + (544*Mst2*z2*pow2(
        Mst1)*pow3(s2t))/(3.*pow3(Mt)) + (9328*pow2(Dmglst2)*pow2(Mst1)*pow3(
        s2t))/(9.*Mst2*pow3(Mt)) + (2048*Dmglst2*pow2(Mst2)*pow3(s2t))/pow3(Mt)
        - (2912*Dmglst2*z2*pow2(Mst2)*pow3(s2t))/(3.*pow3(Mt)) + (4832*pow3(
        Mst2)*pow3(s2t))/(3.*pow3(Mt)) - (608*z2*pow3(Mst2)*pow3(s2t))/(3.*
        pow3(Mt)) - (1024*pow2(Dmglst2)*pow3(Mst2)*pow3(s2t))/(3.*pow2(Mst1)*
        pow3(Mt)) + (280*Dmglst2*Mst2*pow2(Mst1))/(3.*pow4(Msq)) + (620*pow2(
        Dmglst2)*pow2(Mst1))/(3.*pow4(Msq)) - (1040*Mst2*s2t*pow2(Dmglst2)*
        pow2(Mst1))/(3.*Mt*pow4(Msq)) + (1732*pow2(Dmglst2)*pow2(Mst2))/(3.*
        pow4(Msq)) - (100*pow2(Mst1)*pow2(Mst2))/(3.*pow4(Msq)) - (80*Dmglst2*
        s2t*pow2(Mst1)*pow2(Mst2))/(3.*Mt*pow4(Msq)) + (520*Dmglst2*pow3(Mst2))
        /(9.*pow4(Msq)) + (400*s2t*pow2(Dmglst2)*pow3(Mst2))/(Mt*pow4(Msq)) + (
        560*s2t*pow2(Mst1)*pow3(Mst2))/(9.*Mt*pow4(Msq)) + (512*pow2(Dmglst2)*
        pow2(Mst2))/(3.*pow4(Mst1)) - (1024*Dmglst2*pow3(Mst2))/(3.*pow4(Mst1))
        - (640*s2t*pow4(Mst1))/(3.*Mst2*Mt*pow2(Msq)) + (1120*pow4(Mst1))/(3.*
        pow2(Msq)*pow2(Mst2)) + (640*Dmglst2*s2t*pow4(Mst1))/(3.*Mt*pow2(Msq)*
        pow2(Mst2)) + (144*pow2(s2t)*pow4(Mst1))/(pow2(Mst2)*pow2(Mt)) + (
        13024*s2t*pow4(Mst1))/(3.*Mt*pow3(Mst2)) - (7936*s2t*z2*pow4(Mst1))/(3.
        *Mt*pow3(Mst2)) - (3200*Dmglst2*pow4(Mst1))/(3.*pow2(Msq)*pow3(Mst2)) +
        (9856*Dmglst2*pow2(s2t)*pow4(Mst1))/(3.*pow2(Mt)*pow3(Mst2)) - (2048*
        Dmglst2*z2*pow2(s2t)*pow4(Mst1))/(pow2(Mt)*pow3(Mst2)) - (392*pow3(s2t)
        *pow4(Mst1))/(3.*Mst2*pow3(Mt)) + (184*Dmglst2*pow3(s2t)*pow4(Mst1))/(
        3.*pow2(Mst2)*pow3(Mt)) + (110*pow4(Mst1))/(3.*pow4(Msq)) + (80*
        Dmglst2*pow4(Mst1))/(Mst2*pow4(Msq)) - (920*Dmglst2*s2t*pow4(Mst1))/(9.
        *Mt*pow4(Msq)) + (40*Mst2*s2t*pow4(Mst1))/(9.*Mt*pow4(Msq)) + (587920*
        pow2(Dmglst2)*pow2(Mst1))/(27.*pow4(Mst2)) + (35840*z2*pow2(Dmglst2)*
        pow2(Mst1))/(3.*pow4(Mst2)) - (11092*pow4(Mst1))/(3.*pow4(Mst2)) - (
        5248*Dmglst2*s2t*pow4(Mst1))/(Mt*pow4(Mst2)) + (5632*z2*pow4(Mst1))/(3.
        *pow4(Mst2)) + (24704*Dmglst2*s2t*z2*pow4(Mst1))/(3.*Mt*pow4(Mst2)) - (
        656*pow2(s2t)*pow4(Mst2))/(pow2(Mst1)*pow2(Mt)) + (512*Dmglst2*pow3(
        s2t)*pow4(Mst2))/(3.*pow2(Mst1)*pow3(Mt)) - (770*pow4(Mst2))/(9.*pow4(
        Msq)) + (120*Dmglst2*s2t*pow4(Mst2))/(Mt*pow4(Msq)) - (512*pow4(Mst2))/
        (3.*pow4(Mst1)) - (256*pow2(Dmglst2)*pow2(s2t)*pow4(Mst2))/(3.*pow2(Mt)
        *pow4(Mst1)) + (4*Dmglst2*Mst2*pow2(Mst1)*pow4(s2t))/(3.*pow4(Mt)) - (
        344*Dmglst2*Mst2*z2*pow2(Mst1)*pow4(s2t))/(3.*pow4(Mt)) + (818*pow2(
        Dmglst2)*pow2(Mst1)*pow4(s2t))/(9.*pow4(Mt)) - (172*z2*pow2(Dmglst2)*
        pow2(Mst1)*pow4(s2t))/(3.*pow4(Mt)) + (20*pow2(Msq)*pow2(Mst1)*pow4(
        s2t))/pow4(Mt) - (820*pow2(Dmglst2)*pow2(Mst2)*pow4(s2t))/(9.*pow4(Mt))
        + (172*z2*pow2(Dmglst2)*pow2(Mst2)*pow4(s2t))/(3.*pow4(Mt)) + (20*pow2(
        Msq)*pow2(Mst2)*pow4(s2t))/pow4(Mt) - (1162*pow2(Mst1)*pow2(Mst2)*pow4(
        s2t))/(3.*pow4(Mt)) - (172*z2*pow2(Mst1)*pow2(Mst2)*pow4(s2t))/(3.*
        pow4(Mt)) - (844*Dmglst2*pow3(Mst2)*pow4(s2t))/(3.*pow4(Mt)) + (344*
        Dmglst2*z2*pow3(Mst2)*pow4(s2t))/(3.*pow4(Mt)) + (1667*pow4(Mst1)*pow4(
        s2t))/(6.*pow4(Mt)) + (98*Dmglst2*pow4(Mst1)*pow4(s2t))/(Mst2*pow4(Mt))
        + (82*z2*pow4(Mst1)*pow4(s2t))/(3.*pow4(Mt)) - (20*pow2(Msq)*pow4(Mst1)
        *pow4(s2t))/(pow2(Mst2)*pow4(Mt)) + (28*pow4(Mst2)*pow4(s2t))/pow4(Mt)
        + (30*z2*pow4(Mst2)*pow4(s2t))/pow4(Mt) - (166*pow2(Dmglst2)*pow4(Mst2)
        *pow4(s2t))/(3.*pow2(Mst1)*pow4(Mt)) - (20*pow2(Msq)*pow4(Mst2)*pow4(
        s2t))/(pow2(Mst1)*pow4(Mt)) + (158720*Dmglst2*pow4(Mst1))/(9.*pow5(
        Mst2)) - (20224*Dmglst2*z2*pow4(Mst1))/pow5(Mst2) + (512*pow3(s2t)*
        pow5(Mst2))/(3.*pow2(Mst1)*pow3(Mt)) - (40*s2t*pow5(Mst2))/(Mt*pow4(
        Msq)) + (512*Dmglst2*pow2(s2t)*pow5(Mst2))/(3.*pow2(Mt)*pow4(Mst1)) + (
        524*Dmglst2*pow4(s2t)*pow5(Mst2))/(3.*pow2(Mst1)*pow4(Mt)) + (8*log(
        pow2(Mst1)/pow2(Msq))*(6*Mst2*pow2(Dmglst2)*(2*(5*(25*Mt - 4*Mst2*s2t)*
        pow2(Mst1) - 6*Mt*pow2(Mst2))*pow4(Msq) + 20*Mt*pow2(Msq)*pow4(Mst2) +
        5*((Mt - 4*Mst2*s2t)*pow2(Mst1) + (11*Mt + 4*Mst2*s2t)*pow2(Mst2))*
        pow4(Mst2)) + 5*Mst2*(8*pow2(Msq)*((Mt - 2*Mst2*s2t)*pow2(Mst1) + 2*(2*
        Mt + Mst2*s2t)*pow2(Mst2))*pow4(Mst2) + 6*pow4(Msq)*(2*(5*Mt - 4*Mst2*
        s2t)*pow2(Mst1)*pow2(Mst2) + 2*(9*Mt - 4*Mst2*s2t)*pow4(Mst1) + (9*Mt +
        4*Mst2*s2t)*pow4(Mst2)) + pow4(Mst2)*(pow2(Mst1)*(6*Mt*pow2(Mst2) - 8*
        s2t*pow3(Mst2)) + (3*Mt - 4*Mst2*s2t)*pow4(Mst1) + (19*Mt + 12*Mst2*
        s2t)*pow4(Mst2))) + 60*log(pow2(Mst1)/pow2(Mst2))*pow4(Msq)*(6*Mst2*Mt*
        pow2(Dmglst2)*pow2(Mst1) + 2*Mt*pow2(Mst1)*pow3(Mst2) + Mst2*(9*Mt - 4*
        Mst2*s2t)*pow4(Mst1) - 4*Dmglst2*(Mt*pow2(Mst1)*pow2(Mst2) + (9*Mt - 3*
        Mst2*s2t)*pow4(Mst1)) - 2*Mt*pow5(Mst2)) - 20*Dmglst2*(2*pow4(Msq)*(3*(
        7*Mt - 2*Mst2*s2t)*pow2(Mst1)*pow2(Mst2) + (81*Mt - 30*Mst2*s2t)*pow4(
        Mst1) + (-5*Mt + Mst2*s2t)*pow4(Mst2)) - 4*pow2(Msq)*(3*Mst2*Mt - s2t*
        pow2(Mst1) + s2t*pow2(Mst2))*pow5(Mst2) + (3*Mst2*(-Mt + 2*Mst2*s2t)*
        pow2(Mst1) - (13*Mt + 7*Mst2*s2t)*pow3(Mst2) + s2t*pow4(Mst1))*pow5(
        Mst2))))/(3.*Mt*pow4(Msq)*pow5(Mst2)) + (256*pow2(s2t)*pow6(Mst2))/(3.*
        pow2(Mt)*pow4(Mst1)) + (114*pow4(s2t)*pow6(Mst2))/(pow2(Mst1)*pow4(Mt))
        + (32*pow2(Dmglst2)*pow4(s2t)*pow6(Mst2))/(3.*pow4(Mst1)*pow4(Mt)) + (
        pow2(log(pow2(Mst1)/pow2(Mst2)))*(8*pow2(Mst1)*pow3(Mst2)*(425*pow2(
        Mst2)*pow2(Mt)*pow2(s2t) - 704*Mst2*s2t*pow3(Mt) + 212*Mt*pow3(Mst2)*
        pow3(s2t) - 196*pow4(Mt) + 3*pow4(Mst2)*pow4(s2t)) + Mst2*pow4(Mst1)*(
        2688*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 3136*Mst2*s2t*pow3(Mt) + 1024*Mt*
        pow3(Mst2)*pow3(s2t) - 7168*pow4(Mt) + 41*pow4(Mst2)*pow4(s2t)) - 8*
        Dmglst2*(8*Mt*(-67*Mst2*s2t*pow2(Mt) - 108*Mt*pow2(Mst2)*pow2(s2t) +
        128*pow3(Mt) - 16*pow3(Mst2)*pow3(s2t))*pow4(Mst1) + pow4(Mst2)*(-320*
        pow2(Mst2)*pow2(Mt)*pow2(s2t) + 848*Mst2*s2t*pow3(Mt) - 532*Mt*pow3(
        Mst2)*pow3(s2t) + 512*pow4(Mt) - 99*pow4(Mst2)*pow4(s2t)) + pow2(Mst1)*
        pow2(Mst2)*(-768*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 448*Mst2*s2t*pow3(Mt)
        - 212*Mt*pow3(Mst2)*pow3(s2t) + 1656*pow4(Mt) + 35*pow4(Mst2)*pow4(s2t)
        )) + (296*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 6784*Mst2*s2t*pow3(Mt) +
        2208*Mt*pow3(Mst2)*pow3(s2t) + 288*pow4(Mt) + 519*pow4(Mst2)*pow4(s2t))
        *pow5(Mst2) + 4*Mst2*pow2(Dmglst2)*(320*pow2(Mt)*pow2(s2t)*pow4(Mst2) -
        512*pow2(Mst2)*pow4(Mt) + pow2(Mst1)*(768*pow2(Mst2)*pow2(Mt)*pow2(s2t)
        - 1152*Mst2*s2t*pow3(Mt) + 2920*pow4(Mt) - 35*pow4(Mst2)*pow4(s2t)) +
        768*Mt*pow3(s2t)*pow5(Mst2) + 99*pow4(s2t)*pow6(Mst2))))/(3.*pow4(Mt)*
        pow5(Mst2)) + (log(pow2(Mst1)/pow2(Mst2))*(2*pow2(Dmglst2)*(1800*(-((Mt
        - 4*Mst2*s2t)*pow2(Mst1)) - (11*Mt + 4*Mst2*s2t)*pow2(Mst2))*pow3(Mt)*
        pow4(Mst1)*pow5(Mst2) - 7200*pow2(Msq)*pow4(Mst1)*pow4(Mt)*pow5(Mst2) +
        pow4(Msq)*(2*pow3(Mst2)*pow4(Mst1)*(3240*pow2(Mst2)*pow2(Mt)*pow2(s2t)
        + 45920*Mst2*s2t*pow3(Mt) - 9720*Mt*pow3(Mst2)*pow3(s2t) + 39568*pow4(
        Mt) - 4635*pow4(Mst2)*pow4(s2t)) + 45*pow2(Mst1)*(-456*pow2(Mst2)*pow2(
        Mt)*pow2(s2t) - 1024*Mst2*s2t*pow3(Mt) + 256*Mt*pow3(Mst2)*pow3(s2t) +
        400*pow4(Mt) + 153*pow4(Mst2)*pow4(s2t))*pow5(Mst2) + 2*Mst2*(-34080*
        pow2(Mst2)*pow2(Mt)*pow2(s2t) - 140360*Mst2*s2t*pow3(Mt) - 5880*Mt*
        pow3(Mst2)*pow3(s2t) + 370932*pow4(Mt) + 1005*pow4(Mst2)*pow4(s2t))*
        pow6(Mst1) - 1440*pow2(-4*pow2(Mt) + pow2(Mst2)*pow2(s2t))*pow7(Mst2)))
        - 20*Dmglst2*(240*pow2(Msq)*pow2(Mst2)*pow3(Mt)*pow4(Mst1)*(-2*s2t*
        pow2(Mst1)*pow3(Mst2) + 3*Mt*pow4(Mst1) + 2*(3*Mt + Mst2*s2t)*pow4(
        Mst2)) - 120*pow3(Mt)*pow4(Mst1)*(3*Mst2*(-Mt + 2*Mst2*s2t)*pow2(Mst1)
        - (13*Mt + 7*Mst2*s2t)*pow3(Mst2) + s2t*pow4(Mst1))*pow5(Mst2) + pow4(
        Msq)*(-4*pow4(Mst1)*pow4(Mst2)*(1860*pow2(Mst2)*pow2(Mt)*pow2(s2t) +
        1532*Mst2*s2t*pow3(Mt) + 774*Mt*pow3(Mst2)*pow3(s2t) - 2764*pow4(Mt) +
        15*pow4(Mst2)*pow4(s2t)) + 2*pow2(Mst2)*(-432*pow2(Mst2)*pow2(Mt)*pow2(
        s2t) - 10936*Mst2*s2t*pow3(Mt) + 996*Mt*pow3(Mst2)*pow3(s2t) + 16228*
        pow4(Mt) + 207*pow4(Mst2)*pow4(s2t))*pow6(Mst1) - 3*pow2(Mst1)*(-856*
        pow2(Mst2)*pow2(Mt)*pow2(s2t) - 1536*Mst2*s2t*pow3(Mt) + 384*Mt*pow3(
        Mst2)*pow3(s2t) + 1200*pow4(Mt) + 203*pow4(Mst2)*pow4(s2t))*pow6(Mst2)
        + 4*(-738*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 8204*Mst2*s2t*pow3(Mt) + 135*
        Mt*pow3(Mst2)*pow3(s2t) + 12492*pow4(Mt) + 12*pow4(Mst2)*pow4(s2t))*
        pow8(Mst1) - 768*pow2(Mt)*(-2*pow2(Mt) + pow2(Mst2)*pow2(s2t))*pow8(
        Mst2))) + 5*(480*pow2(Msq)*pow3(Mst2)*pow3(Mt)*pow4(Mst1)*(2*(-Mt + 2*
        Mst2*s2t)*pow2(Mst1)*pow2(Mst2) + 3*Mt*pow4(Mst1) - 4*(2*Mt + Mst2*s2t)
        *pow4(Mst2)) + 120*pow3(Mt)*pow4(Mst1)*(2*(-3*Mt + 4*Mst2*s2t)*pow2(
        Mst1)*pow2(Mst2) + (-3*Mt + 4*Mst2*s2t)*pow4(Mst1) - (19*Mt + 12*Mst2*
        s2t)*pow4(Mst2))*pow5(Mst2) + 360*(pow2(Mst1) - pow2(Mst2))*pow4(Mst1)*
        pow4(s2t)*pow5(Mst2)*pow6(Msq) + pow4(Msq)*(4*pow4(Mst1)*(3372*pow2(
        Mst2)*pow2(Mt)*pow2(s2t) - 9744*Mst2*s2t*pow3(Mt) + 3912*Mt*pow3(Mst2)*
        pow3(s2t) + 4492*pow4(Mt) + 669*pow4(Mst2)*pow4(s2t))*pow5(Mst2) - 48*
        pow3(Mst2)*(-267*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 792*Mst2*s2t*pow3(Mt)
        - 30*Mt*pow3(Mst2)*pow3(s2t) - 317*pow4(Mt) + 56*pow4(Mst2)*pow4(s2t))*
        pow6(Mst1) + 6*pow2(Mst1)*(-728*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 1024*
        Mst2*s2t*pow3(Mt) + 256*Mt*pow3(Mst2)*pow3(s2t) + 1200*pow4(Mt) + 139*
        pow4(Mst2)*pow4(s2t))*pow7(Mst2) + 3*Mst2*(4640*pow2(Mst2)*pow2(Mt)*
        pow2(s2t) - 13376*Mst2*s2t*pow3(Mt) + 240*Mt*pow3(Mst2)*pow3(s2t) +
        2328*pow4(Mt) - 279*pow4(Mst2)*pow4(s2t))*pow8(Mst1) + 768*pow2(Mt)*(-
        2*pow2(Mt) + pow2(Mst2)*pow2(s2t))*pow9(Mst2)))))/(45.*pow4(Msq)*pow4(
        Mst1)*pow4(Mt)*pow5(Mst2));

   return result;
}

double himalaya::H6::calc_coef_at_as2_no_sm_logs_log2() const {

   const double result =
      (1920*pow11(Mst2)*pow2(Mt)*pow2(s2t) + 52480*s2t*pow2(Dmglst2)*pow3(Mt)*
        pow4(Mst1)*pow4(Mst2) + 18896*pow2(Dmglst2)*pow3(Mst2)*pow4(Mst1)*pow4(
        Mt) - 48160*Dmglst2*pow4(Mst1)*pow4(Mst2)*pow4(Mt) - 1440*pow11(Mst2)*
        pow2(Dmglst2)*pow4(s2t) + 2325*pow11(Mst2)*pow2(Mst1)*pow4(s2t) +
        33360*pow2(Dmglst2)*pow2(Mt)*pow2(s2t)*pow4(Mst1)*pow5(Mst2) + 46400*
        Dmglst2*s2t*pow3(Mt)*pow4(Mst1)*pow5(Mst2) + 21840*pow2(Dmglst2)*pow2(
        Mst1)*pow4(Mt)*pow5(Mst2) + 21280*pow4(Mst1)*pow4(Mt)*pow5(Mst2) -
        7200*log(pow2(Mst1)/pow2(Msq))*pow4(Mst1)*pow4(Mt)*pow5(Mst2) + 600*
        pow2(Dmglst2)*pow2(Mt)*pow2(s2t)*pow3(Mst2)*pow6(Mst1) - 113280*s2t*
        pow2(Dmglst2)*pow2(Mst2)*pow3(Mt)*pow6(Mst1) + 74880*Dmglst2*s2t*pow3(
        Mst2)*pow3(Mt)*pow6(Mst1) - 14160*Dmglst2*pow2(Mt)*pow2(s2t)*pow4(Mst2)
        *pow6(Mst1) + 1920*s2t*pow3(Mt)*pow4(Mst2)*pow6(Mst1) - 11520*Mt*pow2(
        Dmglst2)*pow3(s2t)*pow4(Mst2)*pow6(Mst1) + 261920*Mst2*pow2(Dmglst2)*
        pow4(Mt)*pow6(Mst1) - 100800*Dmglst2*pow2(Mst2)*pow4(Mt)*pow6(Mst1) -
        10080*pow3(Mst2)*pow4(Mt)*pow6(Mst1) - 9000*pow2(Mt)*pow2(s2t)*pow5(
        Mst2)*pow6(Mst1) - 31200*Dmglst2*Mt*pow3(s2t)*pow5(Mst2)*pow6(Mst1) +
        4395*pow2(Dmglst2)*pow4(s2t)*pow5(Mst2)*pow6(Mst1) - 46080*s2t*pow2(
        Dmglst2)*pow2(Mst1)*pow3(Mt)*pow6(Mst2) + 82080*Dmglst2*pow2(Mt)*pow2(
        s2t)*pow4(Mst1)*pow6(Mst2) - 24000*s2t*pow3(Mt)*pow4(Mst1)*pow6(Mst2) +
        43680*Dmglst2*pow2(Mst1)*pow4(Mt)*pow6(Mst2) - 23520*Mt*pow3(s2t)*pow6(
        Mst1)*pow6(Mst2) + 1110*Dmglst2*pow4(s2t)*pow6(Mst1)*pow6(Mst2) + 30*
        log(pow2(Mst1)/pow2(Mst2))*pow4(Mst1)*(pow2(Mst1)*pow3(Mst2)*(1096*
        pow2(Mst2)*pow2(Mt)*pow2(s2t) - 1280*Mst2*s2t*pow3(Mt) + 328*Mt*pow3(
        Mst2)*pow3(s2t) - 32*pow4(Mt) - 91*pow4(Mst2)*pow4(s2t)) - Mst2*pow4(
        Mst1)*(-768*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 1216*Mst2*s2t*pow3(Mt) +
        400*pow4(Mt) + 41*pow4(Mst2)*pow4(s2t)) - 2*Dmglst2*(32*pow2(Mt)*(-57*
        Mst2*Mt*s2t + 119*pow2(Mt) - 24*pow2(Mst2)*pow2(s2t))*pow4(Mst1) +
        pow4(Mst2)*(-384*pow2(Mst2)*pow2(Mt)*pow2(s2t) + 912*Mst2*s2t*pow3(Mt)
        - 548*Mt*pow3(Mst2)*pow3(s2t) + 512*pow4(Mt) - 91*pow4(Mst2)*pow4(s2t))
        + pow2(Mst1)*pow2(Mst2)*(-768*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 640*Mst2*
        s2t*pow3(Mt) - 164*Mt*pow3(Mst2)*pow3(s2t) + 2016*pow4(Mt) + 91*pow4(
        Mst2)*pow4(s2t))) + 4*(14*pow2(Mst2)*pow2(Mt)*pow2(s2t) - 456*Mst2*s2t*
        pow3(Mt) + 146*Mt*pow3(Mst2)*pow3(s2t) - 106*pow4(Mt) + 33*pow4(Mst2)*
        pow4(s2t))*pow5(Mst2) + Mst2*pow2(Dmglst2)*(384*pow2(Mt)*pow2(s2t)*
        pow4(Mst2) - 512*pow2(Mst2)*pow4(Mt) + pow2(Mst1)*(768*pow2(Mst2)*pow2(
        Mt)*pow2(s2t) - 1280*Mst2*s2t*pow3(Mt) + 4000*pow4(Mt) - 91*pow4(Mst2)*
        pow4(s2t)) + 768*Mt*pow3(s2t)*pow5(Mst2) + 91*pow4(s2t)*pow6(Mst2))) -
        22440*pow2(Dmglst2)*pow2(Mst1)*pow2(Mt)*pow2(s2t)*pow7(Mst2) - 46080*
        Dmglst2*s2t*pow2(Mst1)*pow3(Mt)*pow7(Mst2) + 42960*pow2(Mt)*pow2(s2t)*
        pow4(Mst1)*pow7(Mst2) + 19680*Dmglst2*Mt*pow3(s2t)*pow4(Mst1)*pow7(
        Mst2) - 23040*pow2(Dmglst2)*pow4(Mt)*pow7(Mst2) + 21840*pow2(Mst1)*
        pow4(Mt)*pow7(Mst2) - 10005*pow2(Dmglst2)*pow4(Mst1)*pow4(s2t)*pow7(
        Mst2) - 5325*pow4(s2t)*pow6(Mst1)*pow7(Mst2) + 67200*Dmglst2*Mst2*s2t*
        pow3(Mt)*pow8(Mst1) + 1920*s2t*pow2(Mst2)*pow3(Mt)*pow8(Mst1) - 112320*
        Dmglst2*pow4(Mt)*pow8(Mst1) - 12000*Mst2*pow4(Mt)*pow8(Mst1) + 1770*
        Dmglst2*pow4(Mst2)*pow4(s2t)*pow8(Mst1) + 3585*pow4(s2t)*pow5(Mst2)*
        pow8(Mst1) - 29520*Dmglst2*pow2(Mst1)*pow2(Mt)*pow2(s2t)*pow8(Mst2) -
        15360*s2t*pow2(Mst1)*pow3(Mt)*pow8(Mst2) + 11520*Mt*pow2(Dmglst2)*pow2(
        Mst1)*pow3(s2t)*pow8(Mst2) + 19680*Mt*pow3(s2t)*pow4(Mst1)*pow8(Mst2) -
        15360*Dmglst2*pow4(Mt)*pow8(Mst2) - 8490*Dmglst2*pow4(Mst1)*pow4(s2t)*
        pow8(Mst2) + 11520*pow2(Dmglst2)*pow2(Mt)*pow2(s2t)*pow9(Mst2) - 12840*
        pow2(Mst1)*pow2(Mt)*pow2(s2t)*pow9(Mst2) + 11520*Dmglst2*Mt*pow2(Mst1)*
        pow3(s2t)*pow9(Mst2) - 3840*pow4(Mt)*pow9(Mst2) + 7125*pow2(Dmglst2)*
        pow2(Mst1)*pow4(s2t)*pow9(Mst2) - 345*pow4(Mst1)*pow4(s2t)*pow9(Mst2) +
        7680*Dmglst2*pow2(Mt)*pow2(s2t)*power10(Mst2) + 3840*Mt*pow2(Mst1)*
        pow3(s2t)*power10(Mst2) + 6570*Dmglst2*pow2(Mst1)*pow4(s2t)*power10(
        Mst2))/(45.*pow4(Mst1)*pow4(Mt)*pow5(Mst2));

   return result;
}

double himalaya::H6::calc_coef_at_as2_no_sm_logs_log3() const {

   const double result =
      -298.6666666666667;

   return result;
}




