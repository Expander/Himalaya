#include "H32q2g.hpp"
#include "HierarchyCalculator.hpp"
#include "Constants.hpp"
#include "Utils.hpp"
#include <cmath>
#include <type_traits>

/**
 * 	Constuctor
 * 	@param flagMap the flagMap for the truncation of expansion variables
 * 	@param Al4p a double alpha_s/4/Pi
 * 	@param beta a double which is the mixing angle beta
 * 	@param Dmglst1 a double Mgl - Mst1
 * 	@param Dmst12 a double Mst1^2 - Mst2^2
 * 	@param Dmsqst1 a double Msq^2 - Mst1^2
 * 	@param lmMt a double log((<renormalization scale> / Mt)^2)
 * 	@param lmMst1 a double log((<renormalization scale> / Mst1)^2)
 * 	@param Mt a double top/bottom quark mass
 * 	@param Mst1 a double stop 1 mass
 * 	@param Mst2 a double stop 2 mass
 * 	@param MuSUSY a double mu parameter
 * 	@param s2t a double 2 times the sine of the stop/sbottom quark mixing angle
 * 	@param mdrFlag an int 0 for DR and 1 for MDR scheme
 * 	@param oneLoopFlag an int flag to consider the one-loop expansion terms
 * 	@param twoLoopFlag an int flag to consider the two-loop expansion terms
 * 	@param threeLoopFlag an int flag to consider the three-loop expansion terms
 */
himalaya::H32q2g::H32q2g(const std::map<unsigned int, unsigned int>& flagMap, double Al4p, double beta,
		 double Dmglst1, double Dmst12, double Dmsqst1, double lmMt, double lmMst1,
		 double Mt, double Mst1, double Mst2, double MuSUSY,
		 double s2t,
		 int mdrFlag, int oneLoopFlag, int twoLoopFlag, int threeLoopFlag){
   // abbrev for tan(beta) and sin(beta)
   Tbeta = tan(beta);
   Sbeta = sin(beta);
   this -> Dmglst1 = Dmglst1;
   this -> Dmst12 = Dmst12;
   this -> Dmsqst1 = Dmsqst1;
   this -> lmMst1 = lmMst1;
   this -> Mt = Mt;
   this -> Mst1 = Mst1;
   this -> Mst2 = Mst2;
   this -> MuSUSY = MuSUSY;
   this -> s2t = s2t;
   // mdr flags, indicates if one wants to shift the dr stop mass to the mdr stop mass
   shiftst1 = mdrFlag;
   shiftst2 = mdrFlag;
   shiftst3 = mdrFlag;
   // expansion flags
   xDmglst1 = flagMap.at (HierarchyCalculator::xxDmglst1);
   xDmst12 = flagMap.at (HierarchyCalculator::xxDmglst1);
   xDmsqst1 = flagMap.at (HierarchyCalculator::xxDmsqst1);
   
   s1 = 
   #include "../hierarchies/h32q2g/sigS1Full.inc"
   ;
   s2 = 
   #include "../hierarchies/h32q2g/sigS2Full.inc"
   ;
   s12 = 
   #include "../hierarchies/h32q2g/sigS12Full.inc"
   ;
}

/**
 * 	@return The diagonal (1, 1) matrix element of the Higgs mass matrix as a double for the hierarchy 'H32q2g'
 */
double himalaya::H32q2g::getS1(){
   return s1;
}

/**
 * 	@return The diagonal (2, 2) matrix element of the Higgs mass matrix as a double for the hierarchy 'H32q2g'
 */
double himalaya::H32q2g::getS2(){
   return s2;
}

/**
 * 	@return The off-diagonal (1, 2) = (2, 1) matrix element of the Higgs mass matrix as a double for the hierarchy 'H32q2g'
 */
double himalaya::H32q2g::getS12(){
   return s12;
}

/**
 * 	@return returns the susy log^0 term of Mh^2 @ O(at*as^2) without any log(mu^2) terms normalized to DO (H3m*12/Mt^4/Sbeta^2)
 */
double himalaya::H32q2g::calc_coef_at_as2_no_sm_logs_log0(){

   const double result =
      ((Mt*pow2(Sbeta)*(-105840000*Mt*pow2(Mst1)*pow3(Dmsqst1)*pow6(Mst2) +
        1470*Mt*pow2(Dmsqst1)*(-125*pow2(Dmst12)*pow2(Mst1)*pow2(Mst2)*(32*(19*
        Dmglst1 + 52*Mst1)*Mt*s2t + 4*(-566 + 567*z3)*pow2(Mt) + 3*(-410 + 441*
        z3)*pow2(Mst1)*pow2(s2t)) + 250*pow2(Mst1)*(16*(19*Dmglst1 + 52*Mst1)*
        Mt*s2t + 2*(-566 + 567*z3)*pow2(Mt) + 3*(-410 + 441*z3)*pow2(Mst1)*
        pow2(s2t))*pow3(Dmst12) + 500*Dmst12*Mt*(8*(19*Dmglst1 + 52*Mst1)*s2t +
        Mt*(-566 + 567*z3))*pow2(Mst1)*pow4(Mst2) + 8*(-66000*Dmglst1*Mst1 +
        1572*pow2(Dmglst1) + 125*(-598 + 567*z3)*pow2(Mst1))*pow2(Mt)*pow6(
        Mst2)) + 1470*Dmsqst1*Mst1*(-128*pow2(Mt)*pow3(Dmglst1)*(-1331*Mst1*
        s2t*pow2(Dmst12)*pow2(Mst2) + 1331*Mst1*s2t*pow3(Dmst12) + 1331*Dmst12*
        Mst1*s2t*pow4(Mst2) + 2579*Mt*pow6(Mst2)) + 48*Mst1*Mt*pow2(Dmglst1)*(-
        (pow2(Dmst12)*pow2(Mst2)*(7940*Mst1*Mt*s2t + 914*pow2(Mt) + 225*pow2(
        Mst1)*pow2(s2t))) + (7940*Mst1*Mt*s2t + 914*pow2(Mt) + 450*pow2(Mst1)*
        pow2(s2t))*pow3(Dmst12) + 2*Dmst12*Mt*(457*Mt + 3970*Mst1*s2t)*pow4(
        Mst2) + 4760*pow2(Mt)*pow6(Mst2)) + 80*Dmglst1*pow2(Mst1)*(-2*Mt*pow2(
        Dmst12)*pow2(Mst2)*(-1516*Mst1*Mt*s2t + 7614*pow2(Mt) + 3525*pow2(Mst1)
        *pow2(s2t)) + pow3(Dmst12)*(-564*Mst1*s2t*pow2(Mt) + 14100*Mt*pow2(
        Mst1)*pow2(s2t) + 15228*pow3(Mt) + 125*pow3(Mst1)*pow3(s2t)) + 4*
        Dmst12*(3807*Mt - 1375*Mst1*s2t)*pow2(Mt)*pow4(Mst2) + 29200*pow3(Mt)*
        pow6(Mst2)) + 5*pow3(Mst1)*(4*Mt*pow2(Dmst12)*pow2(Mst2)*(19600*Mst1*
        Mt*s2t + (-9122 + 14175*z3)*pow2(Mt) + 450*(2 - 21*z3)*pow2(Mst1)*pow2(
        s2t)) + pow3(Dmst12)*(20800*Mst1*s2t*pow2(Mt) + 75*Mt*(274 + 63*z3)*
        pow2(Mst1)*pow2(s2t) - 16*(-6536 + 14175*z3)*pow3(Mt) + 8400*pow3(Mst1)
        *pow3(s2t)) + 200*Dmst12*(-888*Mst1*s2t + Mt*(-158 + 567*z3))*pow2(Mt)*
        pow4(Mst2) + 201600*pow3(Mt)*pow6(Mst2))) - pow3(Mst1)*(49*pow3(Mst1)*(
        -20*Mt*pow2(Dmst12)*pow2(Mst2)*(300*Mst1*Mt*s2t*(-17512 + 14805*z3) + (
        -375892 + 621675*z3)*pow2(Mt) - 900*(-1226 + 495*z3)*pow2(Mst1)*pow2(
        s2t)) + pow3(Dmst12)*(-30*Mst1*s2t*(-1117238 + 877575*z3)*pow2(Mt) -
        2250*Mt*(-5570 + 333*z3)*pow2(Mst1)*pow2(s2t) + (-41715182 + 38174625*
        z3)*pow3(Mt) + 3000*(-2722 + 2259*z3)*pow3(Mst1)*pow3(s2t)) - 6000*
        Dmst12*(81*Mt*(-406 + 285*z3) + 8*Mst1*s2t*(-694 + 477*z3))*pow2(Mt)*
        pow4(Mst2) + 48000*(623 + 963*z3)*pow3(Mt)*pow6(Mst2)) + 196*Dmglst1*
        pow2(Mst1)*(Mt*pow2(Dmst12)*pow2(Mst2)*(-40*Mst1*Mt*s2t*(-4511549 +
        3729375*z3) + (-45149198 + 35285625*z3)*pow2(Mt) + 47250*(-430 + 207*
        z3)*pow2(Mst1)*pow2(s2t)) + pow3(Dmst12)*(2*Mst1*s2t*(28188929 -
        23099625*z3)*pow2(Mt) - 225*Mt*(-160136 + 100785*z3)*pow2(Mst1)*pow2(
        s2t) + 6*(-19196074 + 14320125*z3)*pow3(Mt) + 1125*(-22174 + 18791*z3)*
        pow3(Mst1)*pow3(s2t)) - 40*Dmst12*(150*Mst1*s2t*(-19856 + 17667*z3) +
        Mt*(-5136871 + 3912300*z3))*pow2(Mt)*pow4(Mst2) - 6000*(-31142 + 22653*
        z3)*pow3(Mt)*pow6(Mst2)) + 2*Mst1*pow2(Dmglst1)*(Mt*pow2(Dmst12)*pow2(
        Mst2)*(-196*Mst1*Mt*s2t*(-263717842 + 218365875*z3) + (-27129768542 +
        22522586625*z3)*pow2(Mt) - 22050*(-63802 + 92895*z3)*pow2(Mst1)*pow2(
        s2t)) + pow3(Dmst12)*(392*Mst1*s2t*(-384557822 + 319453875*z3)*pow2(Mt)
        - 66150*Mt*(-150737 + 112420*z3)*pow2(Mst1)*pow2(s2t) + (25287306692 -
        22556819250*z3)*pow3(Mt) + 3675*(-1908362 + 1581075*z3)*pow3(Mst1)*
        pow3(s2t)) - 392*Dmst12*(20*Mst1*s2t*(-6041999 + 5054400*z3) + Mt*(-
        73908751 + 57368250*z3))*pow2(Mt)*pow4(Mst2) - 3920*(-8223692 +
        6125625*z3)*pow3(Mt)*pow6(Mst2)) - 8*pow3(Dmglst1)*(Mt*pow2(Dmst12)*
        pow2(Mst2)*(Mst1*Mt*s2t*(-21284082326 + 17749864125*z3) + (673066160 -
        615195000*z3)*pow2(Mt) + 7350*(-520877 + 430155*z3)*pow2(Mst1)*pow2(
        s2t)) + pow3(Dmst12)*(2*Mst1*s2t*(31902674758 - 26534253375*z3)*pow2(
        Mt) - 14700*Mt*(-520877 + 430155*z3)*pow2(Mst1)*pow2(s2t) + 40*(-
        16826654 + 15379875*z3)*pow3(Mt) + 245*(14217821 - 11852775*z3)*pow3(
        Mst1)*pow3(s2t)) + 4*Dmst12*(10*Mt*(-16826654 + 15379875*z3) + 49*Mst1*
        s2t*(-108352984 + 89636625*z3))*pow2(Mt)*pow4(Mst2) + 392*(-10583177 +
        8913375*z3)*pow3(Mt)*pow6(Mst2)))))/(2.3814e7*pow6(Mst1)*pow6(Mst2)))/
        pow4(Mt)/pow2(Sbeta)*12.;

   return result;
}

/**
 * 	@return returns the susy log^1 term of Mh^2 @ O(at*as^2) without any log(mu^2) terms normalized to DO (H3m*12/Mt^4/Sbeta^2)
 */
double himalaya::H32q2g::calc_coef_at_as2_no_sm_logs_log1(){

   const double result =
      ((Mt*pow2(Sbeta)*(2*Mst1*pow3(Dmglst1)*(-2*Mst1*Mt*pow2(Dmst12)*pow2(Mst2)
        *(82320*Dmsqst1*Mt*s2t + 346639*Mt*s2t*pow2(Mst1) + 555463*Mst1*pow2(
        Mt) + 1051785*pow2(s2t)*pow3(Mst1)) + Mst1*pow3(Dmst12)*(164640*
        Dmsqst1*s2t*pow2(Mt) + 8778304*s2t*pow2(Mst1)*pow2(Mt) + 4207140*Mt*
        pow2(s2t)*pow3(Mst1) + 1110926*Mst1*pow3(Mt) + 661255*pow3(s2t)*pow4(
        Mst1)) + 2*Dmst12*Mst1*(555463*Mst1*Mt + 82320*Dmsqst1*s2t - 3695874*
        s2t*pow2(Mst1))*pow2(Mt)*pow4(Mst2) - 4704*(10*Dmsqst1 + 141*pow2(Mst1)
        )*pow3(Mt)*pow6(Mst2)) + pow2(Dmglst1)*(-246960*pow2(Dmsqst1)*pow3(Mt)*
        pow6(Mst2) - 17640*Dmsqst1*pow2(Mst1)*pow2(Mt)*((17*Mt - 10*Mst1*s2t)*
        pow2(Dmst12)*pow2(Mst2) + (-17*Mt + 10*Mst1*s2t)*pow3(Dmst12) + Dmst12*
        (-17*Mt + 10*Mst1*s2t)*pow4(Mst2) - 35*Mt*pow6(Mst2)) + pow4(Mst1)*(Mt*
        pow2(Dmst12)*pow2(Mst2)*(-4088560*Mst1*Mt*s2t + 968629*pow2(Mt) +
        1853670*pow2(Mst1)*pow2(s2t)) + pow3(Dmst12)*(7502488*Mst1*s2t*pow2(Mt)
        - 3134775*Mt*pow2(Mst1)*pow2(s2t) + 2019394*pow3(Mt) + 689430*pow3(
        Mst1)*pow3(s2t)) - 196*Dmst12*(20187*Mt - 3442*Mst1*s2t)*pow2(Mt)*pow4(
        Mst2) - 8199072*pow3(Mt)*pow6(Mst2))) + 98*Dmglst1*Mst1*(600*Dmsqst1*
        pow2(Mst1)*pow2(Mt)*((6*Mt + Mst1*s2t)*pow2(Dmst12)*pow2(Mst2) + (-6*Mt
        + 3*Mst1*s2t)*pow3(Dmst12) - Dmst12*(6*Mt + 5*Mst1*s2t)*pow4(Mst2) -
        25*Mt*pow6(Mst2)) + 1500*pow2(Dmsqst1)*pow2(Mt)*(-(Mst1*s2t*pow2(
        Dmst12)*pow2(Mst2)) + Mst1*s2t*pow3(Dmst12) + Dmst12*Mst1*s2t*pow4(
        Mst2) + 3*Mt*pow6(Mst2)) - pow4(Mst1)*(Mt*pow2(Dmst12)*pow2(Mst2)*(
        29758*Mst1*Mt*s2t + 6677*pow2(Mt) + 22350*pow2(Mst1)*pow2(s2t)) + pow3(
        Dmst12)*(-34587*Mst1*s2t*pow2(Mt) - 18045*Mt*pow2(Mst1)*pow2(s2t) +
        22414*pow3(Mt) + 3325*pow3(Mst1)*pow3(s2t)) - 8*Dmst12*(4471*Mt + 6875*
        Mst1*s2t)*pow2(Mt)*pow4(Mst2) + 50800*pow3(Mt)*pow6(Mst2))) - 49*pow2(
        Mst1)*(750*pow2(Dmsqst1)*pow2(Mt)*(-((Mt + 8*Mst1*s2t)*pow2(Dmst12)*
        pow2(Mst2)) + (Mt + 8*Mst1*s2t)*pow3(Dmst12) + Dmst12*(Mt + 8*Mst1*s2t)
        *pow4(Mst2) - 14*Mt*pow6(Mst2)) + 150*Dmsqst1*Mt*pow2(Mst1)*(pow2(
        Dmst12)*pow2(Mst2)*(80*Mst1*Mt*s2t + 17*pow2(Mt) - 180*pow2(Mst1)*pow2(
        s2t)) - 2*(20*Mst1*Mt*s2t + 187*pow2(Mt) - 90*pow2(Mst1)*pow2(s2t))*
        pow3(Dmst12) + 20*Dmst12*Mt*(17*Mt - 6*Mst1*s2t)*pow4(Mst2) + 1080*
        pow2(Mt)*pow6(Mst2)) - pow4(Mst1)*(-2*Mt*pow2(Dmst12)*pow2(Mst2)*(
        25850*Mst1*Mt*s2t + 21033*pow2(Mt) + 75*pow2(Mst1)*pow2(s2t)) + pow3(
        Dmst12)*(68264*Mst1*s2t*pow2(Mt) + 5700*Mt*pow2(Mst1)*pow2(s2t) +
        36107*pow3(Mt) + 250*pow3(Mst1)*pow3(s2t)) + 200*Dmst12*(131*Mt + 100*
        Mst1*s2t)*pow2(Mt)*pow4(Mst2) + 400*(-533 + 54*z3)*pow3(Mt)*pow6(Mst2))
        )))/(99225.*pow6(Mst1)*pow6(Mst2)))/pow4(Mt)/pow2(Sbeta)*12.; 

   return result;
}

/**
 * 	@return returns the susy log^2 term of Mh^2 @ O(at*as^2) without any log(mu^2) terms normalized to DO (H3m*12/Mt^4/Sbeta^2)
 */
double himalaya::H32q2g::calc_coef_at_as2_no_sm_logs_log2(){

   const double result =
      ((-2*Mt*pow2(Sbeta)*(7*Dmglst1*pow3(Mst1)*(2*Mt*pow2(Dmst12)*pow2(Mst2)*(-
        1304*Mst1*Mt*s2t + 888*pow2(Mt) + 645*pow2(Mst1)*pow2(s2t)) + pow3(
        Dmst12)*(1544*Mst1*s2t*pow2(Mt) - 2250*Mt*pow2(Mst1)*pow2(s2t) - 1640*
        pow3(Mt) + 275*pow3(Mst1)*pow3(s2t)) - 8*Dmst12*(239*Mt - 35*Mst1*s2t)*
        pow2(Mt)*pow4(Mst2) - 6520*pow3(Mt)*pow6(Mst2)) + pow2(Dmglst1)*pow2(
        Mst1)*(Mt*pow2(Dmst12)*pow2(Mst2)*(-4088*Mst1*Mt*s2t + 22884*pow2(Mt) -
        8925*pow2(Mst1)*pow2(s2t)) + pow3(Dmst12)*(42728*Mst1*s2t*pow2(Mt) -
        1155*Mt*pow2(Mst1)*pow2(s2t) - 56772*pow3(Mt) + 3360*pow3(Mst1)*pow3(
        s2t)) + 28*Dmst12*(393*Mt - 1234*Mst1*s2t)*pow2(Mt)*pow4(Mst2) - 3948*
        pow3(Mt)*pow6(Mst2)) + 8*Mst1*pow3(Dmglst1)*(-3*Mt*pow2(Dmst12)*pow2(
        Mst2)*(-75*Mst1*Mt*s2t + 1122*pow2(Mt) + 560*pow2(Mst1)*pow2(s2t)) +
        pow3(Dmst12)*(1671*Mst1*s2t*pow2(Mt) + 3360*Mt*pow2(Mst1)*pow2(s2t) +
        3366*pow3(Mt) + 140*pow3(Mst1)*pow3(s2t)) + 3*Dmst12*(1122*Mt - 707*
        Mst1*s2t)*pow2(Mt)*pow4(Mst2) + 2163*pow3(Mt)*pow6(Mst2)) + 7*(-(Mt*
        pow2(Dmst12)*pow2(Mst2)*(1760*Mst1*Mt*s2t + 538*pow2(Mt) + 105*pow2(
        Mst1)*pow2(s2t))*pow4(Mst1)) + pow3(Dmst12)*(1032*Mst1*s2t*pow2(Mt) +
        480*Mt*pow2(Mst1)*pow2(s2t) + 644*pow3(Mt) - 45*pow3(Mst1)*pow3(s2t))*
        pow4(Mst1) + 40*Dmst12*(11*Mt + 61*Mst1*s2t)*pow2(Mt)*pow4(Mst1)*pow4(
        Mst2) + 10*pow3(Mt)*(45*pow2(Dmsqst1) - 90*Dmsqst1*pow2(Mst1) - 442*
        pow4(Mst1))*pow6(Mst2))))/(945.*pow4(Mst1)*pow6(Mst2)))/pow4(Mt)/
        pow2(Sbeta)*12.; 

   return result;
}

/**
 * 	@return returns the susy log^3 term of Mh^2 @ O(at*as^2) without any log(mu^2) terms normalized to DO (H3m*12/Mt^4/Sbeta^2)
 */
double himalaya::H32q2g::calc_coef_at_as2_no_sm_logs_log3(){

   const double result =
      ((-224*pow2(Sbeta)*pow4(Mt))/9.)/pow4(Mt)/pow2(Sbeta)*12.; 

   return result;
}



