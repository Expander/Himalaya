// ====================================================================
// This file is part of Himalaya.
//
// Himalaya is licenced under the GNU General Public License (GNU GPL)
// version 3.
// ====================================================================

#include "ThresholdCalculator.hpp"
#include "Hierarchies.hpp"
#include "Logger.hpp"
#include "dilog.h"
#include <cmath>

namespace {
   template <typename T> T pow2(T x)  { return x*x; }
   template <typename T> T pow3(T x)  { return x*x*x; }
   template <typename T> T pow4(T x)  { return x*x*x*x; }
   template <typename T> T pow5(T x)  { return x*x*x*x*x; }
   template <typename T> T pow6(T x)  { return x*x*x*x*x*x; }
   template <typename T> T pow7(T x)  { return x*x*x*x*x*x*x; }
   template <typename T> T pow8(T x)  { return x*x*x*x*x*x*x*x; }
   template <typename T> T pow9(T x)  { return x*x*x*x*x*x*x*x*x; }
   template <typename T> T power10(T x) { return pow9(x)*x; }
   template <typename T> T pow11(T x) { return pow2(x)*pow9(x); }
   template <typename T> T pow12(T x) { return pow2(pow6(x)); }
   template <typename T> T pow13(T x) { return pow4(x)*pow9(x); }
   template <typename T> T pow14(T x) { return pow2(pow7(x)); }
   template <typename T> T pow15(T x) { return pow6(x)*pow9(x); }
   template <typename T> T pow16(T x) { return pow2(pow8(x)); }
   template <typename T> T pow17(T x) { return pow16(x)*x; }
   template <typename T> T pow18(T x) { return pow2(pow9(x)); }
   template <typename T> T pow19(T x) { return pow18(x)*x; }
   template <typename T> T pow20(T x) { return pow19(x)*x; }
   
   const double Pi  = 3.1415926535897932384626433832795;
   
   // The parts below are taken from FlexibleSUSY
   /// lambda^2(u,v)
   double lambda_2(double u, double v)
   {
      return pow2(1 - u - v) - 4*u*v;
   }

   /// u < 1 && v < 1, lambda^2(u,v) > 0
   double phi_pos(double u, double v)
   {
      using std::log;
      using gm2calc::dilog;
      const auto lambda = std::sqrt(lambda_2(u,v));

      return (-(log(u)*log(v))
              + 2*log((1 - lambda + u - v)/2.)*log((1 - lambda - u + v)/2.)
              - 2*dilog((1 - lambda + u - v)/2.)
              - 2*dilog((1 - lambda - u + v)/2.)
              + pow2(Pi)/3.)/lambda;
   }

   /// lambda^2(u,v) < 0
   double phi_neg(double u, double v)
   {
      using std::acos;
      using std::sqrt;
      using gm2calc::clausen_2;
      const auto lambda = std::sqrt(-lambda_2(u,v));

      return 2*(+ clausen_2(2*acos((1 + u - v)/(2.*sqrt(u))))
                + clausen_2(2*acos((1 - u + v)/(2.*sqrt(v))))
                + clausen_2(2*acos((-1 + u + v)/(2.*sqrt(u*v)))))/lambda;
   }

   /**
    * Phi(u,v) with u = x/z, v = y/z.
    *
    * The following identities hold:
    * Phi(u,v) = Phi(v,u) = Phi(1/u,v/u)/u = Phi(1/v,u/v)/v
    */
   double phi_uv(double u, double v)
   {
      const auto lambda = lambda_2(u,v);

      if (lambda > 0.) {
         if (u <= 1 && v <= 1)
            return phi_pos(u,v);
         if (u >= 1 && v/u <= 1)
            return phi_pos(1./u,v/u)/u;
         // v >= 1 && u/v <= 1
         return phi_pos(1./v,u/v)/v;
      }

      return phi_neg(u,v);
   }

} // anonymous namespace

/**
 * 	Constructor
 * 	@param p_ a HimalayaInterface struct
 * 	@param msq2_ the averaged squark mass of the first two generations squared
 * 	@param verbose a bool enable the output of the parameter validation. Enabled by default
 * 	@param check a boolean which indicates if the threshold corrections should be tested
 */
himalaya::ThresholdCalculator::ThresholdCalculator(
   const Parameters& p_, double msq2_, bool verbose, bool check)
   : p(p_), msq2(msq2_)
{
   p.validate(verbose);

   if (!std::isfinite(msq2_))
      msq2 = p.calculateMsq2();

   if(!check){
      // Set mass limit for threshold corrections
      const double mQ3 = sqrt(p.mq2(2,2));
      const double mU3 = sqrt(p.mu2(2,2));
      const double m3 = p.MG;
      const double eps = mQ3*0.01;
      const double eps2 = mU3*0.01;
      const double msq2Save = msq2;
      const double Pi  = 3.1415926535897932384626433832795;
      const double v = sqrt(pow2(p.vu) + pow2(p.vd));
      const double pref = sqrt(2.)*p.Mt*pow4(p.g3/(4*Pi))/v;

      if(std::abs(mQ3-mU3) < eps && std::abs(mU3 - m3) < eps && std::abs(m3 - sqrt(msq2)) < eps){
	 const double lim = pref*getDeltaYtAlphas2(Limits::DEGENERATE, 1);
	 const double exact = pref*getDeltaYtAlphas2(Limits::GENERAL, 1);
	 p.mu2(2,2) = pow2(mQ3 + std::abs(mU3 - mQ3)/2.);
	 p.MG = mQ3 + std::abs(m3 - mQ3)/2.;
	 msq2 = pow2(mQ3 + std::abs(sqrt(msq2) - mQ3)/2.);
	 const double exactShifted = pref*getDeltaYtAlphas2(Limits::GENERAL, 1);
	 if(!isfinite(exact, exactShifted, lim)) p.massLimit3LThreshold = Limits::DEGENERATE;
      } else if(std::abs(mQ3 - mU3) < eps && std::abs(mU3 - m3) < eps){
	 const double lim = pref*getDeltaYtAlphas2(Limits::MQ3_EQ_MU3_EQ_M3, 1);
	 const double exact = pref*getDeltaYtAlphas2(Limits::GENERAL, 1);
	 p.mu2(2,2) = pow2(mQ3 + std::abs(mU3 - mQ3)/2.);
	 p.MG = mQ3 + std::abs(m3 - mQ3)/2.;
	 const double exactShifted = pref*getDeltaYtAlphas2(Limits::GENERAL, 1);
	 if(!isfinite(exact, exactShifted, lim)) p.massLimit3LThreshold = Limits::MQ3_EQ_MU3_EQ_M3;
      } else if(std::abs(mQ3 - mU3) < eps){
	 const double lim = pref*getDeltaYtAlphas2(Limits::MQ3_EQ_MU3, 1);
	 const double exact = pref*getDeltaYtAlphas2(Limits::GENERAL, 1);
	 p.mu2(2,2) = pow2(mQ3 + std::abs(mU3 - mQ3)/2.);
	 p.MG = mQ3 + std::abs(m3 - mQ3)/2.;
	 const double exactShifted = pref*getDeltaYtAlphas2(Limits::GENERAL, 1);
	 if(!isfinite(exact, exactShifted, lim)) p.massLimit3LThreshold = Limits::MQ3_EQ_MU3;
      } else if(std::abs(mQ3 - m3) < eps){
	 const double lim = pref*getDeltaYtAlphas2(Limits::MQ3_EQ_M3, 1);
	 const double exact = pref*getDeltaYtAlphas2(Limits::GENERAL, 1);
	 p.mu2(2,2) = pow2(mQ3 + std::abs(mU3 - mQ3)/2.);
	 p.MG = mQ3 + std::abs(m3 - mQ3)/2.;
	 const double exactShifted = pref*getDeltaYtAlphas2(Limits::GENERAL, 1);
	 if(!isfinite(exact, exactShifted, lim)) p.massLimit3LThreshold = Limits::MQ3_EQ_M3;
      } else if(std::abs(mU3 - m3) < eps2){
	 const double lim = pref*getDeltaYtAlphas2(Limits::MU3_EQ_M3, 1);
	 const double exact = pref*getDeltaYtAlphas2(Limits::GENERAL, 1);
	 p.mu2(2,2) = pow2(mQ3 + std::abs(mU3 - mQ3)/2.);
	 p.MG = mQ3 + std::abs(m3 - mQ3)/2.;
	 const double exactShifted = pref*getDeltaYtAlphas2(Limits::GENERAL, 1);
	 if(!isfinite(exact, exactShifted, lim)) p.massLimit3LThreshold = Limits::MU3_EQ_M3;
      }

      // reset possible parameter shifts
      p.mu2(2,2) = pow2(mU3);
      p.MG = m3;
      msq2 = msq2Save;
   }
}

bool himalaya::ThresholdCalculator::isfinite(double exact, double shifted, double limit){
   // checks if the threshold correction in the general mass case is finite and smaller than 1
   if(!std::isfinite(exact) || !std::isfinite(shifted) 
      || (std::abs(exact) > 1. && std::abs(shifted) > 1.)) return false;
   // checks if the difference of the shifted result to the limit is greater than the difference
   // of the exact result to the limit which should indicate the divergence of the general mass case
   if(std::abs(shifted - limit) >= std::abs(exact - limit)) return false;
   
   return true;
}


/**
 * 	Returns a specific threshold corrections for a given mass limit
 * 	@param variable an integer key for a threshold correctionn
 * 	@param scheme an integer key to set the scheme. Choices are {DRbar'} only!
 * 	@param omitLogs an integer to omit all log mu terms
 * 	@return a threshold correction for a given variable in a given scheme for a suitable mass limit
 */
double himalaya::ThresholdCalculator::getThresholdCorrection(int variable, int scheme, int omitLogs){
   double thresholdCorrection = 0.;
   const double limit = p.massLimit3LThreshold;
   
   if(scheme != RenSchemes::TEST && scheme != RenSchemes::DRBARPRIME){
      INFO_MSG("Your renormalization scheme is not compatible with the"
               " implemented threshold corrections!");
   }

   switch(variable){
      case(ThresholdVariables::G3_AS):{
	 thresholdCorrection = getDeltaG3Alphas(omitLogs);
	 switch(scheme){
	    case(RenSchemes::DRBARPRIME):
	       thresholdCorrection = - thresholdCorrection;
	       break;
	 }
      }
      break;
      case(ThresholdVariables::YT_AS):{
	 thresholdCorrection = getDeltaYtAlphas(limit, omitLogs);
	 switch(scheme){
	    case(RenSchemes::DRBARPRIME):
	       thresholdCorrection = - thresholdCorrection;
	       break;
	 }
      }
      break;
      case(ThresholdVariables::YT_AS2):{
	 thresholdCorrection = getDeltaYtAlphas2(limit, omitLogs);
	 switch(scheme){
	    case(RenSchemes::DRBARPRIME):{
	       const double dytas = getDeltaYtAlphas(limit, omitLogs);
	       thresholdCorrection = 2 * getDeltaG3Alphas(omitLogs) * dytas
		  + pow2(dytas) - thresholdCorrection;
	    }
	    break;
	 }
      }
      break;
      case(ThresholdVariables::LAMBDA_AT):{
	 thresholdCorrection = getDeltaLambdaAlphat(limit, omitLogs);
      }
      break;
      case(ThresholdVariables::LAMBDA_AT_AS):{
	 thresholdCorrection = getDeltaLambdaAlphatAlphas(limit, omitLogs);
	 switch(scheme){
	    case(RenSchemes::DRBARPRIME):
	       thresholdCorrection = thresholdCorrection
		  - 4 * getDeltaLambdaAlphat(limit, omitLogs) *
		  getDeltaYtAlphas(limit, omitLogs);
	       break;
	 }
      }
      break;
      // Note that the genuine contribution of lambda_atas2 is unknown and thus set to 0 (note: here are the reconstructed DR' logs included)
      // The lines below just convert it from MSbar to DRbar
      case(ThresholdVariables::LAMBDA_AT_AS2):{
	 thresholdCorrection = getDeltaLambdaAlphatAlphas2(limit, omitLogs);
	 switch(scheme){
	    case(RenSchemes::DRBARPRIME):{
	       const double dg3as = getDeltaG3Alphas(omitLogs);
	       const double dytas = getDeltaYtAlphas(limit, omitLogs);
	       thresholdCorrection = thresholdCorrection +
	       (getDeltaLambdaAlphatAlphas(limit, omitLogs)
	       * (-2 * dg3as - 4 * dytas)
	       + getDeltaLambdaAlphat(limit, omitLogs) * (8 * dg3as * dytas 
		  + 10 * pow2(dytas) - 4 * getDeltaYtAlphas2(limit, omitLogs)));
	    }
	    break;
	 }
      }
      break;
      case(ThresholdVariables::LAMBDA_YB2_G12):{
	 return getDeltaLambdaYb2G12(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_G14):{
	 return getDeltaLambdaG14(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_REG_G14):{
	 return getDeltaLambdaRegG14();
      }
      case(ThresholdVariables::LAMBDA_CHI_G14):{
	 return getDeltaLambdaChiG14(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_CHI_G24):{
	 return getDeltaLambdaChiG24(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_G24):{
	 return getDeltaLambdaG24(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_REG_G24):{
	 return getDeltaLambdaRegG24();
      }
      case(ThresholdVariables::LAMBDA_G12_G22):{
	 return getDeltaLambdaG12G22(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_REG_G12_G22):{
	 return getDeltaLambdaRegG12G22();
      }
      case(ThresholdVariables::LAMBDA_CHI_G12_G22):{
	 return getDeltaLambdaChiG12G22();
      }
      case(ThresholdVariables::LAMBDA_YB2_G22):{
	 return getDeltaLambdaYb2G22(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_YB4):{
	 return getDeltaLambdaYb4(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_YT2_G12):{
	 return getDeltaLambdaYt2G12(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_YT2_G22):{
	 return getDeltaLambdaYt2G22(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_YTAU2_G12):{
	 return getDeltaLambdaYtau2G12(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_YTAU2_G22):{
	 return getDeltaLambdaYtau2G22(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_YTAU4):{
	 return getDeltaLambdaYtau4(omitLogs);
      }
      case(ThresholdVariables::G1_G1):{
	 return getDeltaG1G1(omitLogs);
      }
      case(ThresholdVariables::G2_G2):{
	 return getDeltaG2G2(omitLogs);
      }
      case(ThresholdVariables::VEV_YT2):{
	 return getDeltaVevYt2(limit);
      }
      case(ThresholdVariables::YT_YB):{
	 return getDeltaYtYb(omitLogs);
      }
      case(ThresholdVariables::YT_YT):{
	 return getDeltaYtYt(omitLogs);
      }
      case(ThresholdVariables::YTAU_YTAU):{
	 return getDeltaYtauYtau(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_YB4_G32):{
	 return getDeltaLambdaYb4G32(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_YB6):{
	 return getDeltaLambdaYb6(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_YT6):{
	 return getDeltaLambdaYt6(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_YTAU6):{
	 return getDeltaLambdaYtau6(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_YT2_YB4):{
	 return getDeltaLambdaYt2Yb4(omitLogs);
      }
      case(ThresholdVariables::LAMBDA_YT4_YB2):{
	 return getDeltaLambdaYt4Yb2(omitLogs);
      }
   };
   
   return thresholdCorrection;
}

double himalaya::ThresholdCalculator::getDeltaLambdaYb2G12(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double mQ32 = p.mq2(2,2);
   const double mD32 = p.md2(2,2);
   const double mQ3 = sqrt(mQ32);
   const double mD3 = sqrt(mD32);
   const double Xb2 = pow2(p.Ad(2,2) - p.mu*p.vu/p.vd);
   const double c2beta = cos(2*atan(p.vu/p.vd));
   const double lmD3MR = omitLogs*log(mD32 / MR2);
   const double lmQ3MR = omitLogs*log(mQ32 / MR2);
   
   return -3*c2beta*(Xb2*(3*(F3(mQ3/mD3) + F4(mQ3/mD3)) + 2*c2beta*F5(mQ3/mD3)) +
      4*mD3*mQ3*(2*lmD3MR + lmQ3MR))/(20.*mD3*mQ3);
}

double himalaya::ThresholdCalculator::getDeltaLambdaG14(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double beta = atan(p.vu/p.vd);
   const double c2beta = cos(2*beta);
   const double c4beta = cos(4*beta);
   const double c8beta = cos(8*beta);
   const double s4beta2 = pow2(sin(4*beta));
   const double lmAMR = omitLogs*log(pow2(p.MA) / MR2);
   const double lmD1MR = omitLogs*log(p.md2(0,0) / MR2);
   const double lmD2MR = omitLogs*log(p.md2(1,1) / MR2);
   const double lmD3MR = omitLogs*log(p.md2(2,2) / MR2);
   const double lmE1MR = omitLogs*log(p.me2(0,0) / MR2);
   const double lmE2MR = omitLogs*log(p.me2(1,1) / MR2);
   const double lmE3MR = omitLogs*log(p.me2(2,2) / MR2);
   const double lmL1MR = omitLogs*log(p.ml2(0,0) / MR2);
   const double lmL2MR = omitLogs*log(p.ml2(1,1) / MR2);
   const double lmL3MR = omitLogs*log(p.ml2(2,2) / MR2);
   const double lmQ1MR = omitLogs*log(p.mq2(0,0) / MR2);
   const double lmQ2MR = omitLogs*log(p.mq2(1,1) / MR2);
   const double lmQ3MR = omitLogs*log(p.mq2(2,2) / MR2);
   const double lmU1MR = omitLogs*log(p.mu2(0,0) / MR2);
   const double lmU2MR = omitLogs*log(p.mu2(1,1) / MR2);
   const double lmU3MR = omitLogs*log(p.mu2(2,2) / MR2);
   
   return (-108*s4beta2 - 3*(-29 + 4*c4beta + 9*c8beta)*lmAMR + 16*pow2(c2beta)*
      (2*(lmD1MR + lmD2MR + lmD3MR + 3*(lmE1MR + lmE2MR + lmE3MR)) + 3*(lmL1MR +
      lmL2MR + lmL3MR) + lmQ1MR + lmQ2MR + lmQ3MR + 8*(lmU1MR + lmU2MR + lmU3MR)
      ))/800.;
}

double himalaya::ThresholdCalculator::getDeltaLambdaRegG14(){
   return -9/50.;
}

double himalaya::ThresholdCalculator::getDeltaLambdaChiG14(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double beta = atan(p.vu/p.vd);
   const double c2beta2 = pow2(cos(2*beta));
   const double c4beta = cos(4*beta);
   const double s2beta = sin(2*beta);
   const double lmUMR = omitLogs*log(pow2(p.mu) / MR2);
   
   return 3*(-7*(3 + c4beta)*f1(p.M1/p.mu) + 9*(c4beta -1)*f3(p.M1/p.mu) + 4*
      c2beta2*(F7(p.M1/p.mu) - 2*lmUMR) + 4*(c2beta2 - 8)*F5(p.M1/p.mu)*s2beta)
      /200.;
}

double himalaya::ThresholdCalculator::getDeltaLambdaChiG24(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double beta = atan(p.vu/p.vd);
   const double cbeta = cos(beta);
   const double c2beta = cos(2*beta);
   const double c4beta = cos(4*beta);
   const double sbeta = sin(beta);
   const double lm2MR = omitLogs*log(pow2(p.M2) / MR2);
   const double lmUMR = omitLogs*log(pow2(p.mu) / MR2);
   
   return -9*(3 + c4beta)*f2(p.M2/p.mu)/8. + cbeta*sbeta*(-7*cbeta*sbeta*
      f4(p.M2/p.mu) + (-8 + pow2(c2beta))*F5(p.M2/p.mu)) + pow2(c2beta)/2.*
      F7(p.M2/p.mu) - pow2(c2beta)*(2*lm2MR + lmUMR)/3.;
}

double himalaya::ThresholdCalculator::getDeltaLambdaG24(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double beta = atan(p.vu/p.vd);
   const double c2beta = cos(2*beta);
   const double c4beta = cos(4*beta);
   const double c8beta = cos(8*beta);
   const double s4beta = sin(4*beta);
   const double lmAMR = omitLogs*log(pow2(p.MA) / MR2);
   const double lmL1MR = omitLogs*log(p.ml2(0,0) / MR2);
   const double lmL2MR = omitLogs*log(p.ml2(1,1) / MR2);
   const double lmL3MR = omitLogs*log(p.ml2(2,2) / MR2);
   const double lmQ1MR = omitLogs*log(p.mq2(0,0) / MR2);
   const double lmQ2MR = omitLogs*log(p.mq2(1,1) / MR2);
   const double lmQ3MR = omitLogs*log(p.mq2(2,2) / MR2);
   
   return (-36*pow2(s4beta) + (53 - 28*c4beta - 9*c8beta)*lmAMR + 16*pow2(c2beta)
      *(lmL1MR + lmL2MR + lmL3MR + 3*(lmQ1MR + lmQ2MR + lmQ3MR)))/96.;
}

double himalaya::ThresholdCalculator::getDeltaLambdaRegG24(){
   const double beta = atan(p.vu/p.vd);
   const double c2beta = cos(2*beta);
   
   return (-9 + 2*pow2(c2beta))/6.;
}

double himalaya::ThresholdCalculator::getDeltaLambdaG12G22(int omitLogs){
   using std::log;
   
   const double MR2 = pow2(p.scale);
   const double beta = atan(p.vu/p.vd);
   const double c4beta = cos(4*beta);
   const double c8beta = cos(8*beta);
   const double s4beta = sin(4*beta);
   const double lmAMR = omitLogs*log(pow2(p.MA) / MR2);
   
   return -3*(12*pow2(s4beta) + (-7 + 4*c4beta + 3*c8beta)*lmAMR)/80.;
}

double himalaya::ThresholdCalculator::getDeltaLambdaRegG12G22(){
   return -3/5.;
}

double himalaya::ThresholdCalculator::getDeltaLambdaChiG12G22(){
   const double beta = atan(p.vu/p.vd);
   const double c2beta = cos(2*beta);
   const double c4beta = cos(4*beta);
   const double s2beta = sin(2*beta);
   const double m1mu = p.M1/p.mu;
   const double m2mu = p.M2/p.mu;
   
   return (-14*(3 + c4beta)*f6(m1mu,m2mu) - 4*pow2(s2beta)*(8*f5(m1mu,m2mu) + 
      f7(m1mu,m2mu)) + 4*pow2(c2beta)*(F7(m1mu) + 3*F7(m2mu)) + 4*s2beta
      *(pow2(c2beta)*(F5(m1mu) + 3*F5(m2mu)) - 16*f8(m1mu,m2mu)))/40.;
}

double himalaya::ThresholdCalculator::getDeltaLambdaYb2G22(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double Xb2 = pow2(p.Ad(2,2) - p.mu*p.vu/p.vd);
   const double mQ32 = p.mq2(2,2);
   const double mD32 = p.md2(2,2);
   const double mQ3 = sqrt(mQ32);
   const double mD3 = sqrt(mD32);
   const double beta = atan(p.vu/p.vd);
   const double c2beta = cos(2*beta);
   const double lmQ3MR = omitLogs*log(mQ32 / MR2);
   
   return -(c2beta*Xb2*(3*F4(mQ3/mD3) + c2beta*F5(mQ3/mD3)))/(2.*mD3*mQ3)
      - 3*c2beta*lmQ3MR;
}

double himalaya::ThresholdCalculator::getDeltaLambdaYb4(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double Xb2 = pow2(p.Ad(2,2) - p.mu*p.vu/p.vd);
   const double mQ32 = p.mq2(2,2);
   const double mD32 = p.md2(2,2);
   const double mQ3 = sqrt(mQ32);
   const double mD3 = sqrt(mD32);
   const double lmQ3MR = omitLogs*log(mQ32 / MR2);
   const double lmD3MR = omitLogs*log(mD32 / MR2);
   
   return (12*mD3*mQ3*Xb2*F1(mQ3/mD3) - pow2(Xb2)*F2(mQ3/mD3))/(mD32*mQ32) 
      + 6*(lmD3MR + lmQ3MR);
}

double himalaya::ThresholdCalculator::getDeltaLambdaYt2G12(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double Xt2 = pow2(p.Au(2,2) - p.mu*p.vd/p.vu);
   const double mQ32 = p.mq2(2,2);
   const double mU32 = p.mu2(2,2);
   const double mQ3 = sqrt(mQ32);
   const double mU3 = sqrt(mU32);
   const double beta = atan(p.vu/p.vd);
   const double cbeta = cos(beta);
   const double c2beta = cos(2*beta);
   const double lmQ3MR = omitLogs*log(mQ32 / MR2);
   const double lmU3MR = omitLogs*log(mU32 / MR2);
   
   return 3*((c2beta*Xt2*(3*F3(mQ3/mU3) - c2beta*F5(mQ3/mU3)))/(mQ3*mU3) - 2*
      c2beta*lmQ3MR + 8*cbeta*lmU3MR)/10.;
}

double himalaya::ThresholdCalculator::getDeltaLambdaYt2G22(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double Xt2 = pow2(p.Au(2,2) - p.mu*p.vd/p.vu);
   const double mQ32 = p.mq2(2,2);
   const double mU32 = p.mu2(2,2);
   const double mQ3 = sqrt(mQ32);
   const double mU3 = sqrt(mU32);
   const double beta = atan(p.vu/p.vd);
   const double c2beta = cos(2*beta);
   const double lmQ3MR = omitLogs*log(mQ32 / MR2);
   
   return -c2beta*Xt2*(-3*F4(mQ3/mU3) + c2beta*F5(mQ3/mU3))/(2.*mQ3*mU3) + 3*
   c2beta*lmQ3MR;
}

double himalaya::ThresholdCalculator::getDeltaLambdaYtau2G12(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double Xtau2 = pow2(p.Ae(2,2) - p.mu*p.vu/p.vd);
   const double mL32 = p.ml2(2,2);
   const double mE32 = p.me2(2,2);
   const double mL3 = sqrt(mL32);
   const double mE3 = sqrt(mE32);
   const double beta = atan(p.vu/p.vd);
   const double c2beta = cos(2*beta);
   const double lmE3MR = omitLogs*log(mE32 / MR2);
   const double lmL3MR = omitLogs*log(mL32 / MR2);
   
   return c2beta*((Xtau2*(-9*F3(mL3/mE3) + 3*F4(mL3/mE3) - 2*c2beta*F5(mL3/mE3))
   )/(mE3*mL3) + 12*(-2*lmE3MR + lmL3MR))/20.;
}

double himalaya::ThresholdCalculator::getDeltaLambdaYtau2G22(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double Xtau2 = pow2(p.Ae(2,2) - p.mu*p.vu/p.vd);
   const double mL32 = p.ml2(2,2);
   const double mE32 = p.me2(2,2);
   const double mL3 = sqrt(mL32);
   const double mE3 = sqrt(mE32);
   const double beta = atan(p.vu/p.vd);
   const double c2beta = cos(2*beta);
   const double lmL3MR = omitLogs*log(mL32 / MR2);
   
   return -c2beta*Xtau2*(3*F4(mL3/mE3) + c2beta*F5(mL3/mE3))/(6.*mE3*mL3) 
   - c2beta*lmL3MR;
}

double himalaya::ThresholdCalculator::getDeltaLambdaYtau4(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double Xtau2 = pow2(p.Ae(2,2) - p.mu*p.vu/p.vd);
   const double mL32 = p.ml2(2,2);
   const double mE32 = p.me2(2,2);
   const double mL3 = sqrt(mL32);
   const double mE3 = sqrt(mE32);
   const double lmE3MR = omitLogs*log(mE32 / MR2);
   const double lmL3MR = omitLogs*log(mL32 / MR2);
   
   return (12*mE3*mL3*Xtau2*F1(mL3/mE3) - pow2(Xtau2)*F2(mL3/mE3))/(3.*mE32*mL32)
   + 2*(lmE3MR + lmL3MR);
}

double himalaya::ThresholdCalculator::getDeltaG1G1(int omitLogs){
   using std::log;
   
   const double MR2 = pow2(p.scale);
   const double lmAMR = omitLogs*log(pow2(p.MA) / MR2);
   const double lmUMR = omitLogs*log(pow2(p.mu) / MR2);
   const double lmD1MR = omitLogs*log(p.md2(0,0) / MR2);
   const double lmD2MR = omitLogs*log(p.md2(1,1) / MR2);
   const double lmD3MR = omitLogs*log(p.md2(2,2) / MR2);
   const double lmE1MR = omitLogs*log(p.me2(0,0) / MR2);
   const double lmE2MR = omitLogs*log(p.me2(1,1) / MR2);
   const double lmE3MR = omitLogs*log(p.me2(2,2) / MR2);
   const double lmL1MR = omitLogs*log(p.ml2(0,0) / MR2);
   const double lmL2MR = omitLogs*log(p.ml2(1,1) / MR2);
   const double lmL3MR = omitLogs*log(p.ml2(2,2) / MR2);
   const double lmQ1MR = omitLogs*log(p.mq2(0,0) / MR2);
   const double lmQ2MR = omitLogs*log(p.mq2(1,1) / MR2);
   const double lmQ3MR = omitLogs*log(p.mq2(2,2) / MR2);
   const double lmU1MR = omitLogs*log(p.mu2(0,0) / MR2);
   const double lmU2MR = omitLogs*log(p.mu2(1,1) / MR2);
   const double lmU3MR = omitLogs*log(p.mu2(2,2) / MR2);
   
   return (-3*lmAMR - 2*(lmD1MR + lmD2MR + lmD3MR + 3*(lmE1MR + lmE2MR + lmE3MR)
      ) - 3*(lmL1MR + lmL2MR + lmL3MR) - lmQ1MR - lmQ2MR - lmQ3MR - 12*lmUMR - 8
      *(lmU1MR + lmU2MR + lmU3MR))/60.;
}

double himalaya::ThresholdCalculator::getDeltaG2G2(int omitLogs){
   using std::log;
   
   const double MR2 = pow2(p.scale);
   const double lm2MR = omitLogs*log(pow2(p.M2) / MR2);
   const double lmAMR = omitLogs*log(pow2(p.MA) / MR2);
   const double lmUMR = omitLogs*log(pow2(p.mu) / MR2);
   const double lmL1MR = omitLogs*log(p.ml2(0,0) / MR2);
   const double lmL2MR = omitLogs*log(p.ml2(1,1) / MR2);
   const double lmL3MR = omitLogs*log(p.ml2(2,2) / MR2);
   const double lmQ1MR = omitLogs*log(p.mq2(0,0) / MR2);
   const double lmQ2MR = omitLogs*log(p.mq2(1,1) / MR2);
   const double lmQ3MR = omitLogs*log(p.mq2(2,2) / MR2);

   return (4 - 8*lm2MR - lmAMR - lmL1MR - lmL2MR - lmL3MR - 3*(lmQ1MR + lmQ2MR 
      + lmQ3MR) - 4*lmUMR)/12.;
}

double himalaya::ThresholdCalculator::getDeltaYtYt(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double beta = atan(p.vu/p.vd);
   const double cbeta = cos(beta);
   const double sbeta = sin(beta);
   const double Xt2 = pow2(p.Au(2,2) - p.mu*p.vd/p.vu);
   const double mQ3 = sqrt(p.mq2(2,2));
   const double mU3 = sqrt(p.mu2(2,2));
   const double lmAMR = omitLogs*log(pow2(p.MA) / MR2);
   const double lmUMR = omitLogs*log(pow2(p.mu) / MR2);
   
   return (-2*pow2(sbeta)*Xt2*F5(mQ3/mU3)/(mQ3*mU3) + 8*F6(mQ3/p.mu) + 4
      *F6(mU3/p.mu) + pow2(cbeta)*(-3 + 6*lmAMR) + 6*lmUMR)/8.;
}

double himalaya::ThresholdCalculator::getDeltaYtauYtau(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double beta = atan(p.vu/p.vd);
   const double cbeta = cos(beta);
   const double sbeta = sin(beta);
   const double Xtau2 = pow2(p.Ae(2,2) - p.mu*p.vu/p.vd);
   const double mL3 = sqrt(p.ml2(2,2));
   const double mE3 = sqrt(p.me2(2,2));
   const double lmAMR = omitLogs*log(pow2(p.MA) / MR2);
   const double lmUMR = omitLogs*log(pow2(p.mu) / MR2);
   
   return - pow2(cbeta)*Xtau2*F5(mL3/mE3)/(12.*mE3*mL3) + (4*F6(mE3/p.mu) + 8
      *F6(mL3/p.mu) + pow2(sbeta)*(-3 + 6*lmAMR) + 6*lmUMR)/8.;
}

double himalaya::ThresholdCalculator::getDeltaYtYb(int omitLogs){
   using std::log;
   using std::sqrt;
   
   const double MR2 = pow2(p.scale);
   const double beta = atan(p.vu/p.vd);
   const double cbeta = cos(beta);
   const double c2beta = cos(2*beta);
   const double tbeta = tan(beta);
   const double mD3 = sqrt(p.md2(2,2));
   const double mQ3 = sqrt(p.mq2(2,2));
   const double Xb = p.Ad(2,2) - p.mu*p.vu/p.vd;
   const double lmAMR = omitLogs*log(pow2(p.MA) / MR2);
   const double lmUMR = omitLogs*log(pow2(p.mu) / MR2);
   
   return (- (9 + 7*c2beta)/2. - 2*pow2(cbeta)*pow2(Xb)*F5(mQ3/mD3)/(mD3*mQ3) 
      + 4*F6(mD3/p.mu) + 4*Xb*F9(mQ3/p.mu, mD3/p.mu)/(p.mu*tbeta) + (5 + 3*c2beta)
      *lmAMR + 2*lmUMR)/8.;
}

double himalaya::ThresholdCalculator::getDeltaVevYt2(int limit){
   const double Xt2 = pow2(p.Au(2,2) - p.mu*p.vd/p.vu);
   const double mQ32 = p.mq2(2,2);
   const double mU32 = p.mu2(2,2);
   const double theta = asin(p.s2t)/2.;
   const double c2t = cos(2*theta);
   
   if(limit == Limits::MQ3_EQ_M3 || limit == Limits::MU3_EQ_M3) limit = Limits::GENERAL;
   
   switch (limit){
      case himalaya::Limits::GENERAL:
	 return (-2*pow5(mQ32 - mU32)*pow2(p.s2t) + Xt2*((pow2(mQ32) - pow2(mU32))
	    *(6*mQ32*mU32 + (pow2(mQ32) - 8*mQ32*mU32 + pow2(mU32))*pow2(p.s2t))
	    - 12*pow2(c2t)*pow2(mQ32)*pow2(mU32)*log(mQ32/mU32)))/(8*mQ32*mU32*
	    pow3(mQ32 - mU32));
      default:
	 return Xt2/(4.*mQ32);
   }
}

double himalaya::ThresholdCalculator::getDeltaLambdaYb4G32(int omitLogs){
   using std::log;
   using std::sqrt;
   using gm2calc::dilog;
   
   const double MR2 = pow2(p.scale);
   const double lMR = omitLogs*log(MR2);
   double mQ32 = p.mq2(2,2);
   double mD32 = p.md2(2,2);
   const double m32 = pow2(p.MG);
   double mQ3 = sqrt(mQ32);
   double mD3 = sqrt(mD32);
   const double m3 = sqrt(m32);
   const double Xb = p.Ad(2,2) - p.mu*p.vu/p.vd;
   const double eps = 0.01*mQ3;
   
   if(std::abs(mQ3 - m3) < eps && std::abs(mD3 - m3) < eps && std::abs(mQ3 - mD3)){
      return (-8*(Xb*(-12*mQ32*Xb + 4*mQ3*pow2(Xb) - 24*pow3(mQ3) + pow3(Xb)) + 8*mQ3*
        lMR*(-3*mQ32*Xb - 3*mQ3*pow2(Xb) + 3*pow3(mQ3) + pow3(Xb)) -
        16*mQ3*log(mQ3)*(-3*mQ32*Xb - 3*mQ3*pow2(Xb) + 3*pow3(mQ3) + 3*lMR
        *pow3(mQ3) + pow3(Xb)) + 48*pow2(log(mQ3))*pow4(mQ3) + 12*
        pow2(lMR)*pow4(mQ3)))/(3.*pow4(mQ3));
   } else{
      mQ32 = p.mq2(2,2)*(1 + 0.02);
      mD32 = p.md2(2,2)*(1 - 0.015);
      mQ3 = sqrt(mQ32);
      mD3 = sqrt(mD32);
      return 16*(-2*pow2(lMR) + (16*Xb*(m3*(m32 - mD32)*(m32 - mQ32)*log(mD3) - (m32 -
        mQ32)*dilog(1 - m32/mD32)*pow3(m3) + (m32 - mD32)*dilog(1 - m32/mQ32)*
        pow3(m3) + lMR*(m32 - mQ32)*log(mD3)*pow3(m3) + log(m3)*(lMR*(-mD32 +
        mQ32) + 2*(m32 - mQ32)*log(mD3) - 2*(m32 - mD32)*log(mQ3))*pow3(m3) -
        2*(m32 - mQ32)*pow2(log(mD3))*pow3(m3) + 2*(m32 - mD32)*pow2(log(mQ3))*
        pow3(m3) - (m32 - mD32)*log(mQ3)*(-(m3*mQ32) + (1 + lMR)*pow3(m3))))/((
        m32 - mD32)*(m32 - mQ32)*(mD32 - mQ32)) - (8*m3*((2*m32 - mD32 - mQ32)*
        dilog(1 - m32/mD32) + (-2*m32 + mD32 + mQ32)*dilog(1 - m32/mQ32) + 2*(-
        ((2 + lMR)*(mD32 - mQ32)) + ((3 + lMR)*mD32 + (1 + lMR)*mQ32 - 4*m32*
        log(m3))*log(mD3) - ((1 + lMR)*mD32 + (3 + lMR)*mQ32 - 4*m32*log(m3))*
        log(mQ3) + (2*m32 - mD32 - mQ32)*pow2(log(mD3)) + (-2*m32 + mD32 +
        mQ32)*pow2(log(mQ3))))*pow3(Xb))/pow3(mD32 - mQ32) - (4*dilog(1 - m32/
        mD32)*pow4(m3))/pow2(m32 - mD32) - (4*dilog(1 - m32/mQ32)*pow4(m3))/
        pow2(m32 - mQ32) + 2*pow2(Xb)*((-2*m32)/(mD32*mQ32) - (2*lMR*m32)/(
        mD32*mQ32) + (8*lMR*log(mD3))/(mD32 - mQ32) + (4*(3*m32 - 2*mD32)*log(
        mD3))/((m32 - mD32)*(mD32 - mQ32)) - (8*lMR*log(mQ3))/(mD32 - mQ32) + (
        4*(3*m32 - 2*mQ32)*log(mQ3))/((m32 - mQ32)*(-mD32 + mQ32)) + (8*(mD32 +
        mQ32)*log(mD3)*log(mQ3))/pow2(mD32 - mQ32) - (4*(3*mD32 - mQ32)*pow2(
        log(mD3)))/pow2(mD32 - mQ32) + (4*(mD32 - 3*mQ32)*pow2(log(mQ3)))/pow2(
        mD32 - mQ32) + (4*(m32 - mD32 - mQ32)*log(m3)*pow4(m3))/((m32 - mD32)*
        mD32*(m32 - mQ32)*mQ32)) + (6*log(mD3)*(-2*m32*mD32 + 2*pow4(m3) +
        pow4(mD3)))/pow2(m32 - mD32) - (4*pow2(log(mD3))*(-2*m32*mD32 + 3*pow4(
        m3) + pow4(mD3)))/pow2(m32 - mD32) + (2*(3 + 2*lMR)*log(mQ3)*(-2*m32*
        mQ32 + 2*pow4(m3) + pow4(mQ3)))/pow2(m32 - mQ32) - (4*pow2(log(mQ3))*(-
        2*m32*mQ32 + 3*pow4(m3) + pow4(mQ3)))/pow2(m32 - mQ32) + (2*(-(mD32*(
        mD32 - mQ32)*mQ32*(-2*m32 + mD32 + mQ32)*dilog(1 - m32/mD32)) + mD32*(
        mD32 - mQ32)*mQ32*(-2*m32 + mD32 + mQ32)*dilog(1 - m32/mQ32) - 2*mD32*(
        mD32 - mQ32)*mQ32*(2*m32 + 7*mD32 + 3*mQ32)*log(mD3) + lMR*(mD32 -
        mQ32)*((mD32 - mQ32)*(4*mD32*mQ32 + m32*(mD32 + mQ32)) - 4*mD32*mQ32*(
        m32 + mD32 + mQ32)*log(mD3)) + (6*mD32*mQ32 + m32*(mD32 + mQ32))*pow2(
        mD32 - mQ32) - 2*m32*(mD32 + mQ32)*log(m3)*pow2(mD32 - mQ32) + 2*mD32*
        mQ32*log(mQ3)*((mD32 - mQ32)*(2*(1 + lMR)*m32 + (3 + 2*lMR)*mD32 + (7 +
        2*lMR)*mQ32) - 4*log(mD3)*pow2(mD32 + mQ32)) + 4*mD32*mQ32*(m32*(mD32 -
        mQ32) + 2*mD32*(mD32 + mQ32))*pow2(log(mD3)) + 4*mD32*mQ32*(m32*(-mD32
        + mQ32) + 2*mQ32*(mD32 + mQ32))*pow2(log(mQ3)))*pow4(Xb))/(mD32*mQ32*
        pow4(mD32 - mQ32)) + (-6*m32*mD32*mQ32*(mD32 + mQ32) + 3*pow4(mD3)*
        pow4(mQ3) + pow4(m3)*(9*mD32*mQ32 + 2*pow4(mD3) + 2*pow4(mQ3)) - 2*(
        mD32 + mQ32)*pow6(m3))/(mD32*(-m32 + mD32)*(m32 - mQ32)*mQ32) - (2*log(
        m3)*pow4(m3)*(-4*mD32*mQ32*log(mQ3)*pow2(m32 - mD32) - 4*mD32*mQ32*log(
        mD3)*pow2(m32 - mQ32) + 2*m32*(mD32 + mQ32)*pow2(mD32 - mQ32) + pow4(
        m3)*(2*mD32*mQ32 - 4*pow4(mD3) - 4*pow4(mQ3)) + mD32*mQ32*(pow4(mD3) +
        pow4(mQ3)) + 2*lMR*mD32*mQ32*(-2*m32*(mD32 + mQ32) + 2*pow4(m3) + pow4(
        mD3) + pow4(mQ3)) + 2*(mD32 + mQ32)*pow6(m3)))/(mD32*mQ32*pow2(m32 -
        mD32)*pow2(m32 - mQ32)) + (2*lMR*(2*log(mD3)*(-2*m32*mD32 + 2*pow4(m3)
        + pow4(mD3)) + ((m32 - mD32)*(3*m32*mD32*mQ32*(mD32 + mQ32) - 3*pow4(
        mD3)*pow4(mQ3) - pow4(m3)*(3*mD32*mQ32 + pow4(mD3) + pow4(mQ3)) + (mD32
        + mQ32)*pow6(m3)))/(mD32*(m32 - mQ32)*mQ32)))/pow2(m32 - mD32));
   }
}

double himalaya::ThresholdCalculator::getDeltaLambdaYb6(int omitLogs){
   using std::log;
   using std::sqrt;
   using gm2calc::dilog;

   
   const double MR2 = pow2(p.scale);
   const double lMR = omitLogs*log(MR2);
   const double mQ32 = p.mq2(2,2);
   const double mD32 = p.md2(2,2);
   const double mQ3 = sqrt(mQ32);
   const double mD3 = sqrt(mD32);
   const double mA = p.MA;
   const double beta = atan(p.vu/p.vd);
   const double sbeta = sin(beta);
   const double cbeta = cos(beta);
   const double Xb = p.Ad(2,2) - p.mu*p.vu/p.vd;
   const double Yb = p.Ad(2,2) + p.mu*p.vd/p.vu;
   const double Mu = p.mu;
   
   return 6*(-8*(1 + lMR)*log(mD3) - 12*(1 + lMR)*log(mQ3) + 5*pow2(1 + lMR) + 8*
        pow2(log(mD3)) + 12*pow2(log(mQ3))) + (3*pow2(sbeta)*(2*(1.5 - 8*(1 +
        lMR)*log(mD3) - 12*(1 + lMR)*log(mQ3) + 8*pow2(lMR) - (2*pow2(mA))/mD32
        - pow2(mA)/mQ32 + lMR*(3 - (2/mD32 + 1/mQ32)*pow2(mA)) + 2*log(mA)*(7 -
        6*lMR + (2/mD32 + 1/mQ32)*pow2(mA)) + pow2(Pi) + 12*pow2(log(mA)) + 8*
        pow2(log(mD3)) + 12*pow2(log(mQ3)) + (phixyz(pow2(mA),mD32,mD32)*(-
        deltaxyz(pow2(mA),mD32,mD32) - 6*mD32*pow2(mA) + pow4(mA)))/pow4(mD3) +
        (phixyz(pow2(mA),mQ32,mQ32)*(-2*deltaxyz(pow2(mA),mQ32,mQ32) - 11*mQ32*
        pow2(mA) + 2*pow4(mA)))/pow4(mQ3)) + (4*Xb*Yb*(phixyz(pow2(mA),mD32,
        mD32)*(-deltaxyz(pow2(mA),mD32,mD32) - 6*mD32*pow2(mA) + pow4(mA))*
        pow4(mQ3) + phixyz(pow2(mA),mQ32,mD32)*pow4(mQ3)*(-3*mD32*mQ32 -
        deltaxyz(pow2(mA),mQ32,mD32) - (3*mD32 + 2*mQ32)*pow2(mA) + pow4(mA) +
        2*pow4(mD3) + pow4(mQ3)) + pow4(mD3)*(phixyz(pow2(mA),mQ32,mQ32)*(2*
        deltaxyz(pow2(mA),mQ32,mQ32) + 11*mQ32*pow2(mA) - 2*pow4(mA)) + 4*log(
        mD3/mQ3)*(-3 - 3*lMR + log(mA) + 2*log(mD3) + 3*log(mQ3))*pow4(mQ3))))/
        ((mD32 - mQ32)*pow4(mD3)*pow4(mQ3)) + (4*Yb*pow3(Xb)*(phixyz(pow2(mA),
        mD32,mD32)*((-3*mD32 + mQ32)*deltaxyz(pow2(mA),mD32,mD32) + (mD32 -
        mQ32)*pow2(mA)*(-6*mD32 + pow2(mA)))*pow4(mQ3) + pow4(mD3)*(phixyz(
        pow2(mA),mQ32,mQ32)*(-2*(mD32 - 3*mQ32)*deltaxyz(pow2(mA),mQ32,mQ32) +
        (mD32 - mQ32)*pow2(mA)*(-11*mQ32 + 2*pow2(mA))) + 4*(-3*(2 + lMR)*(mD32
        - mQ32) - log(mQ3)*(3*(1 + lMR)*mD32 + 3*(3 + lMR)*mQ32 + log(mA)*(-
        mD32 + mQ32 - 6*pow2(mA))) + log(mD3)*(3*(3 + lMR)*mD32 + 3*(1 + lMR)*
        mQ32 + log(mA)*(-mD32 + mQ32 - 6*pow2(mA)) + log(mQ3)*(-mD32 - 3*mQ32 +
        2*pow2(mA))) + 2*(-mD32 - mQ32 + pow2(mA))*pow2(log(mD3)) + (3*mD32 +
        5*mQ32 - 4*pow2(mA))*pow2(log(mQ3)))*pow4(mQ3)) + phixyz(pow2(mA),mQ32,
        mD32)*pow4(mQ3)*((-3*mD32 + mQ32)*deltaxyz(pow2(mA),mQ32,mD32) + (mD32
        - mQ32)*(-3*mD32*mQ32 - (3*mD32 + 2*mQ32)*pow2(mA) + pow4(mA) + 2*pow4(
        mD3) + pow4(mQ3)))))/(pow3(mD32 - mQ32)*pow4(mD3)*pow4(mQ3)) + (pow2(
        Yb)*(4*mQ32*phixyz(pow2(mA),mQ32,mD32)*pow2(deltaxyz(pow2(mA),mQ32,
        mD32)) - deltaxyz(pow2(mA),mQ32,mD32)*(mQ32*phixyz(pow2(mA),mQ32,mD32)*
        (-12*mD32*mQ32 - 12*(mD32 + mQ32)*pow2(mA) + 6*pow4(mA) + 5*pow4(mD3) +
        6*pow4(mQ3)) + 2*mD32*(2*mD32*mQ32 + 2*lMR*mD32*mQ32 - 3*mD32*mQ32*log(
        mQ3) + 2*mQ32*log(mQ3)*pow2(mA) + mD32*log(mD3)*(-3*mD32 + 3*mQ32 +
        pow2(mA)) + pow4(mD3) + lMR*pow4(mD3) - 2*log(mQ3)*pow4(mQ3) + log(mA)*
        (-4*mD32*mQ32 - (mD32 + 2*mQ32)*pow2(mA) + pow4(mD3) + 2*pow4(mQ3)))) +
        2*mD32*(log(mA)*(pow2(mD32 - mQ32)*(-4*mD32*mQ32 + pow4(mD3) + 2*pow4(
        mQ3)) + pow4(mA)*(6*mD32*mQ32 + 5*pow4(mD3) + 6*pow4(mQ3)) - (mD32 + 2*
        mQ32)*pow6(mA) + pow2(mA)*(8*mQ32*pow4(mD3) + 3*mD32*pow4(mQ3) - 5*
        pow6(mD3) - 6*pow6(mQ3))) - mQ32*log(mQ3)*((7*mD32 + 6*mQ32)*pow4(mA) +
        2*mQ32*pow4(mD3) - 5*mD32*pow4(mQ3) - 2*pow2(mA)*(mD32*mQ32 + pow4(mD3)
        + 3*pow4(mQ3)) - 2*pow6(mA) + pow6(mD3) + 2*pow6(mQ3)) + mD32*log(mD3)*
        ((-5*mD32 + mQ32)*pow4(mA) + 7*mQ32*pow4(mD3) + 5*pow2(mA)*(-2*mD32*
        mQ32 + pow4(mD3) - pow4(mQ3)) - 9*mD32*pow4(mQ3) + pow6(mA) - pow6(mD3)
        + 3*pow6(mQ3))) + mQ32*phixyz(pow2(mA),mQ32,mD32)*(pow2(mD32 - mQ32)*(-
        4*mD32*mQ32 + pow4(mD3) + 2*pow4(mQ3)) + pow4(mA)*(8*mD32*mQ32 + 7*
        pow4(mD3) + 12*pow4(mQ3)) - 8*(mD32 + mQ32)*pow6(mA) + pow2(mA)*(6*
        mQ32*pow4(mD3) + 8*mD32*pow4(mQ3) - 6*pow6(mD3) - 8*pow6(mQ3)) + 2*
        pow8(mA))))/(mQ32*deltaxyz(pow2(mA),mQ32,mD32)*pow6(mD3)) + (pow4(Xb)*(
        -24*(1 + lMR)*mQ32*log(mD3)*pow2(mA) + 48*mQ32*log(mA)*log(mD3)*pow2(
        mA) + 24*(1 + lMR)*mQ32*log(mQ3)*pow2(mA) - 48*mQ32*log(mA)*log(mQ3)*
        pow2(mA) + 16*(-3*mD32 - mQ32 + 2*pow2(mA))*pow2(Yb)*pow2(log(mD3)) +
        8*(-3*mD32 - 11*mQ32 + 7*pow2(mA))*pow2(Yb)*pow2(log(mQ3)) + (2*phixyz(
        pow2(mA),mD32,mD32)*((-5*mD32 + mQ32)*deltaxyz(pow2(mA),mD32,mD32) + (
        mD32 - mQ32)*pow2(mA)*(-6*mD32 + pow2(mA)))*pow2(Yb))/pow4(mD3) - (2*(
        mD32 - mQ32)*pow2(Yb)*(-11*mD32*mQ32 + pow4(mD3) - 2*pow4(mQ3)))/(mD32*
        mQ32) - (2*(mD32 - mQ32)*pow2(mA)*(-5*mD32*mQ32 + pow4(mD3) - 2*pow4(
        mQ3)))/(mD32*mQ32) - (2*lMR*(mD32 - mQ32)*pow2(mA)*(-5*mD32*mQ32 +
        pow4(mD3) - 2*pow4(mQ3)))/(mD32*mQ32) + (4*(mD32 - mQ32)*log(mA)*pow2(
        mA)*(-5*mD32*mQ32 + pow4(mD3) - 2*pow4(mQ3)))/(mD32*mQ32) - (2*lMR*(
        mD32 - mQ32)*pow2(Yb)*(-5*mD32*mQ32 + pow4(mD3) - 2*pow4(mQ3)))/(mD32*
        mQ32) + (2*phixyz(pow2(mA),mQ32,mQ32)*((2*mD32 - 9*mQ32)*deltaxyz(pow2(
        mA),mQ32,mQ32) + (-mD32 + mQ32)*pow2(mA)*(-11*mQ32 + 2*pow2(mA)))*pow2(
        Yb))/pow4(mQ3) + 2*log(mA)*pow2(Yb)*(12*log(mD3)*(mD32 - mQ32 + pow2(
        mA)) - 12*log(mQ3)*(mD32 - mQ32 + pow2(mA)) + (pow2(mD32 - mQ32)*(-(
        pow2(mD32 - mQ32)*(4*mD32*mQ32 + 3*pow4(mD3) - 2*pow4(mQ3))) +
        deltaxyz(pow2(mA),mQ32,mD32)*(4*mD32*mQ32 - (mD32 - 2*mQ32)*pow2(mA) +
        3*pow4(mD3) - 2*pow4(mQ3)) + pow4(mA)*(2*mD32*mQ32 - 3*pow4(mD3) + 6*
        pow4(mQ3)) + (mD32 - mQ32)*pow2(mA)*(mD32*mQ32 + 5*pow4(mD3) + 6*pow4(
        mQ3)) + (mD32 - 2*mQ32)*pow6(mA)))/(mQ32*deltaxyz(pow2(mA),mQ32,mD32)*
        pow4(mD3))) + (2*log(mQ3)*pow2(Yb)*(-2*pow2(mA)*pow2(mD32 - mQ32) + 4*(
        4 + 3*lMR)*mQ32*pow4(mD3) + mD32*pow4(mQ3) + 5*pow6(mD3) + (pow2(mD32 -
        mQ32)*(-3*(3*mD32 + 2*mQ32)*pow4(mA) + 10*mQ32*pow4(mD3) + 3*mD32*pow4(
        mQ3) + 2*pow2(mA)*(3*mD32*mQ32 + 7*pow4(mD3) + 3*pow4(mQ3)) + 2*pow6(
        mA) - 11*pow6(mD3) - 2*pow6(mQ3)))/deltaxyz(pow2(mA),mQ32,mD32) + 2*
        pow6(mQ3)))/pow4(mD3) + (2*log(mD3)*pow2(Yb)*(deltaxyz(pow2(mA),mQ32,
        mD32)*(4*mD32*mQ32*log(mQ3)*(9*mD32 + 13*mQ32 - 11*pow2(mA)) + pow2(mA)
        *pow2(mD32 - mQ32) - 15*mQ32*pow4(mD3) - 3*mD32*pow4(mQ3) - 12*lMR*
        mD32*pow4(mQ3) - pow6(mD3) - 5*pow6(mQ3)) + pow2(mD32 - mQ32)*((3*mD32
        + 7*mQ32)*pow4(mA) + 9*mQ32*pow4(mD3) - 17*mD32*pow4(mQ3) - pow2(mA)*(
        10*mD32*mQ32 + 5*pow4(mD3) + 11*pow4(mQ3)) - pow6(mA) + 3*pow6(mD3) +
        5*pow6(mQ3))))/(mD32*mQ32*deltaxyz(pow2(mA),mQ32,mD32)) + (phixyz(pow2(
        mA),mQ32,mD32)*pow2(Yb)*(2*pow2(deltaxyz(pow2(mA),mQ32,mD32))*(-9*mD32*
        mQ32 + 18*pow4(mD3) + 2*pow4(mQ3)) - (mD32 - mQ32)*deltaxyz(pow2(mA),
        mQ32,mD32)*(2*(8*mD32 - 3*mQ32)*pow4(mA) - 49*mQ32*pow4(mD3) - 4*pow2(
        mA)*(5*mD32*mQ32 + 9*pow4(mD3) - 3*pow4(mQ3)) + 28*mD32*pow4(mQ3) + 27*
        pow6(mD3) - 6*pow6(mQ3)) + pow2(mD32 - mQ32)*(-2*(mD32 - mQ32)*pow2(mA)
        *(3*pow4(mD3) - 4*pow4(mQ3)) - pow2(mD32 - mQ32)*(4*mD32*mQ32 + 3*pow4(
        mD3) - 2*pow4(mQ3)) + pow4(mA)*(8*mD32*mQ32 + 11*pow4(mD3) + 12*pow4(
        mQ3)) - 8*(mD32 + mQ32)*pow6(mA) + 2*pow8(mA))))/(deltaxyz(pow2(mA),
        mQ32,mD32)*pow6(mD3))))/pow4(mD32 - mQ32) + 2*pow2(Xb)*((2*lMR*(mD32*
        mQ32 + (mD32 - 2*mQ32)*pow2(mA)))/(mD32*(mD32 - mQ32)*mQ32) + (4*mD32*
        mQ32 + 2*(mD32 - 2*mQ32)*pow2(mA))/(mD32*(mD32 - mQ32)*mQ32) - (4*log(
        mQ3)*(-3*(1 + lMR)*mD32 + (1 + 2*lMR)*mQ32 + (1 + lMR)*pow2(mA)))/pow2(
        mD32 - mQ32) + (4*log(mD3)*(-4*mD32 - 3*lMR*mD32 + 2*mQ32 + 2*lMR*mQ32
        + log(mQ3)*(mD32 + mQ32 - pow2(mA)) + (1 + lMR)*pow2(mA)))/pow2(mD32 -
        mQ32) + (8*pow2(log(mD3)))/(mD32 - mQ32) + (4*(-3*mD32 + mQ32 + pow2(
        mA))*pow2(log(mQ3)))/pow2(mD32 - mQ32) + (deltaxyz(pow2(mA),mQ32,mD32)*
        phixyz(pow2(mA),mQ32,mD32))/pow2(-(mD3*mQ32) + pow3(mD3)) + (phixyz(
        pow2(mA),mQ32,mQ32)*((2*mD32 - 3*mQ32)*deltaxyz(pow2(mA),mQ32,mQ32) + (
        -mD32 + mQ32)*pow2(mA)*(-11*mQ32 + 2*pow2(mA))))/(pow2(mD32 - mQ32)*
        pow4(mQ3)) - (4*log(mA)*(mD32*mQ32*log(mD3)*(-mD32 + mQ32 + pow2(mA)) -
        mD32*mQ32*log(mQ3)*(-mD32 + mQ32 + pow2(mA)) + pow2(mA)*(-3*mD32*mQ32 +
        pow4(mD3) + 2*pow4(mQ3))))/(mD32*mQ32*pow2(mD32 - mQ32)) + (phixyz(
        pow2(mA),mD32,mD32)*(-deltaxyz(pow2(mA),mD32,mD32) - 6*mD32*pow2(mA) +
        pow4(mA)))/(-(mQ32*pow4(mD3)) + pow6(mD3)) + pow2(Yb)*((2*lMR*(mD32 -
        2*mQ32))/(mD32*(mD32 - mQ32)*mQ32) + (8*pow2(log(mD3)))/pow2(mD32 -
        mQ32) + (12*pow2(log(mQ3)))/pow2(mD32 - mQ32) + (phixyz(pow2(mA),mD32,
        mD32)*(-deltaxyz(pow2(mA),mD32,mD32) - 6*mD32*pow2(mA) + pow4(mA)))/(
        pow2(mD32 - mQ32)*pow4(mD3)) + (phixyz(pow2(mA),mQ32,mQ32)*(-2*
        deltaxyz(pow2(mA),mQ32,mQ32) - 11*mQ32*pow2(mA) + 2*pow4(mA)))/(pow2(
        mD32 - mQ32)*pow4(mQ3)) + (2*mD32 - 4*mQ32)/(mQ32*pow4(mD3) - mD32*
        pow4(mQ3)) - (2*log(mD3)*(deltaxyz(pow2(mA),mQ32,mD32)*((1 - 2*lMR)*
        mD32*mQ32 + 10*mD32*mQ32*log(mQ3) + pow4(mD3) - 4*pow4(mQ3)) + (mD32 -
        mQ32)*(-((mD32 + 4*mQ32)*pow4(mA)) - 8*mQ32*pow4(mD3) + 11*mD32*pow4(
        mQ3) + 2*pow2(mA)*(7*mD32*mQ32 + 4*pow4(mQ3)) + pow6(mD3) - 4*pow6(mQ3)
        )))/(mD32*mQ32*deltaxyz(pow2(mA),mQ32,mD32)*pow2(mD32 - mQ32)) + (4*
        log(mQ3)*(-(mD32*mQ32) - (mD32 - mQ32)*pow2(mA) - (-1 + lMR)*pow4(mD3)
        - pow4(mQ3) + ((mD32 - mQ32)*(-((4*mD32 + 3*mQ32)*pow4(mA)) + 2*mD32*
        pow4(mQ3) + pow2(mA)*(2*mD32*mQ32 + 6*pow4(mD3) + 3*pow4(mQ3)) + pow6(
        mA) - pow6(mD3) - pow6(mQ3)))/deltaxyz(pow2(mA),mQ32,mD32)))/(pow2(mD32
        - mQ32)*pow4(mD3)) + (2*log(mA)*(log(mQ32/mD32) - ((mD32 - mQ32)*(
        deltaxyz(pow2(mA),mQ32,mD32)*(-4*mD32*mQ32 - 2*mQ32*pow2(mA) + pow4(
        mD3) + 2*pow4(mQ3)) + (mD32 - mQ32 + pow2(mA))*(2*mQ32*pow4(mA) + 5*
        mQ32*pow4(mD3) + pow2(mA)*(-6*mD32*mQ32 + pow4(mD3) - 4*pow4(mQ3)) - 6*
        mD32*pow4(mQ3) - pow6(mD3) + 2*pow6(mQ3))))/(mQ32*deltaxyz(pow2(mA),
        mQ32,mD32)*pow4(mD3))))/pow2(mD32 - mQ32) + (phixyz(pow2(mA),mQ32,mD32)
        *(deltaxyz(pow2(mA),mQ32,mD32)*((7*mD32 - 4*mQ32)*deltaxyz(pow2(mA),
        mQ32,mD32) - 3*(3*mD32 - 2*mQ32)*pow2(mD32 - mQ32) + (-9*mD32 + 6*mQ32)
        *pow4(mA) + pow2(mA)*(6*mD32*mQ32 + 23*pow4(mD3) - 12*pow4(mQ3))) + (
        mD32 - mQ32)*(pow2(mD32 - mQ32)*(-4*mD32*mQ32 + pow4(mD3) + 2*pow4(mQ3)
        ) + pow4(mA)*(8*mD32*mQ32 + 11*pow4(mD3) + 12*pow4(mQ3)) - 8*(mD32 +
        mQ32)*pow6(mA) - 2*pow2(mA)*(-(mQ32*pow4(mD3)) - 4*mD32*pow4(mQ3) +
        pow6(mD3) + 4*pow6(mQ3)) + 2*pow8(mA))))/(deltaxyz(pow2(mA),mQ32,mD32)*
        pow2(mD32 - mQ32)*pow6(mD3))))))/pow2(cbeta) + (12*pow4(Xb)*(3*mD32*
        mQ32*dilog(1 - mD32/mQ32)*pow2(mD32 - mQ32) - 4*mD32*mQ32*pow2(log(mD3)
        )*(-4*mD32*mQ32 + pow4(mD3) - 3*pow4(mQ3)) + 57*pow4(mD3)*pow4(mQ3) +
        33*lMR*pow4(mD3)*pow4(mQ3) - 28*log(mQ3)*pow4(mD3)*pow4(mQ3) + 4*lMR*
        log(mQ3)*pow4(mD3)*pow4(mQ3) + 8*pow2(log(mQ3))*pow4(mD3)*pow4(mQ3) -
        29*mQ32*pow6(mD3) - 17*lMR*mQ32*pow6(mD3) - 6*mQ32*log(mQ3)*pow6(mD3) -
        14*lMR*mQ32*log(mQ3)*pow6(mD3) + 24*mQ32*pow2(log(mQ3))*pow6(mD3) - 31*
        mD32*pow6(mQ3) - 19*lMR*mD32*pow6(mQ3) + 38*mD32*log(mQ3)*pow6(mQ3) +
        10*lMR*mD32*log(mQ3)*pow6(mQ3) - 8*mD32*pow2(log(mQ3))*pow6(mQ3) - 2*
        mD32*log(mD3)*((mD32 - mQ32)*(-((19 + 7*lMR)*mD32*mQ32) + pow4(mD3) -
        5*lMR*pow4(mQ3)) + 2*log(mQ3)*(5*mQ32*pow4(mD3) + 6*mD32*pow4(mQ3) +
        pow6(mQ3))) + pow8(mD3) + lMR*pow8(mD3) + 2*pow8(mQ3) + 2*lMR*pow8(mQ3)
        - 4*log(mQ3)*pow8(mQ3)))/(mD32*mQ32*pow4(mD32 - mQ32)) - (6*pow2(Xb)*(
        6*mD32*mQ32*dilog(1 - mD32/mQ32)*pow2(mD32 - mQ32) + 5*pow4(mD3)*pow4(
        mQ3) + lMR*pow4(mD3)*pow4(mQ3) + 72*log(mQ3)*pow4(mD3)*pow4(mQ3) + 44*
        lMR*log(mQ3)*pow4(mD3)*pow4(mQ3) - 64*pow2(log(mQ3))*pow4(mD3)*pow4(
        mQ3) - 24*mD32*mQ32*pow2(log(mD3))*(-(mD32*mQ32) + pow4(mD3) + pow4(
        mQ3)) - 5*mQ32*pow6(mD3) - 9*lMR*mQ32*pow6(mD3) - 32*mQ32*log(mQ3)*
        pow6(mD3) - 36*lMR*mQ32*log(mQ3)*pow6(mD3) + 48*mQ32*pow2(log(mQ3))*
        pow6(mD3) + mD32*pow6(mQ3) + 9*lMR*mD32*pow6(mQ3) - 44*mD32*log(mQ3)*
        pow6(mQ3) - 32*lMR*mD32*log(mQ3)*pow6(mQ3) + 40*mD32*pow2(log(mQ3))*
        pow6(mQ3) - 2*mD32*log(mD3)*(-((25 + 18*lMR)*mQ32*pow4(mD3)) + (37 +
        22*lMR)*mD32*pow4(mQ3) + pow6(mD3) - (13 + 16*lMR)*pow6(mQ3) + 4*log(
        mQ3)*(3*mQ32*pow4(mD3) - 5*mD32*pow4(mQ3) + 2*pow6(mQ3))) + pow8(mD3) +
        lMR*pow8(mD3) - 2*pow8(mQ3) - 2*lMR*pow8(mQ3) + 4*log(mQ3)*pow8(mQ3)))/
        (mD32*mQ32*pow3(mD32 - mQ32)) + (6*(-2.5 + 4*pow2(lMR) + mD32*(-(1/
        mQ32) + 1/(mD32 - pow2(Mu))) - (2*(mQ32 - 2*pow2(Mu)))/mD32 + 2*(1/mQ32
        + 1/(mQ32 - pow2(Mu)))*pow2(Mu) + lMR*(2 - (2*(mQ32 - 2*pow2(Mu)))/mD32
        + (2*pow2(Mu))/mQ32 + (4*pow2(Mu))/(-mQ32 + pow2(Mu)) + mD32*(-(1/mQ32)
        + 2/(-mD32 + pow2(Mu)))) + log(pow2(Mu))*pow2(Mu)*(-2/mQ32 + 2*lMR*
        pow2(Mu)*(-(1/pow2(mD32 - pow2(Mu))) - 2/pow2(mQ32 - pow2(Mu))) - (6*
        pow2(Mu))/pow2(mQ32 - pow2(Mu)) + (8*log(mQ3)*pow2(Mu))/pow2(mQ32 -
        pow2(Mu)) + (3*mD32*pow2(Mu) - 2*pow4(mD3) - 4*pow4(Mu))/pow2(-(mD3*
        pow2(Mu)) + pow3(mD3))) + (2*dilog(1 - pow2(Mu)/mD32)*(-2*mD32*pow2(Mu)
        + pow4(mD3) - pow4(Mu)))/pow2(mD32 - pow2(Mu)) + (4*pow2(log(mD3))*(-2*
        mD32*pow2(Mu) + pow4(mD3) - pow4(Mu)))/pow2(mD32 - pow2(Mu)) + dilog(1
        - pow2(Mu)/mQ32)*(2 - (8*pow4(Mu))/pow2(mQ32 - pow2(Mu))) + 4*pow2(log(
        mQ3))*(1 - (4*pow4(Mu))/pow2(mQ32 - pow2(Mu))) + 2*log(mD3)*(mD32/mQ32
        + 4*log(mQ3) + (2*log(pow2(Mu))*pow4(Mu))/pow2(mD32 - pow2(Mu)) - (2*
        lMR*(-4*mD32*pow2(Mu) + 2*pow4(mD3) + pow4(Mu)))/pow2(mD32 - pow2(Mu))
        + (2*mD32*pow2(Mu) - pow4(mD3) + 2*pow4(Mu))/pow2(mD32 - pow2(Mu))) +
        2*log(mQ3)*((2*mQ32)/mD32 + (2*mQ32*pow2(Mu) + pow4(mQ3) + 3*pow4(Mu))/
        pow2(mQ32 - pow2(Mu)) + lMR*(-4 + (4*pow4(Mu))/pow2(mQ32 - pow2(Mu))))
        + 2*pow2(Xb)*((-6*((1 + lMR)*(mD32 - mQ32) - 2*mD32*log(mD3) + 2*mQ32*
        log(mQ3))*log(mD32/mQ32))/pow2(mD32 - mQ32) + (2*dilog(1 - pow2(Mu)/
        mQ32)*(mQ32 - pow2(Mu)))/pow2(mD32 - mQ32) + (2*dilog(1 - pow2(Mu)/
        mD32)*(-mQ32 + pow2(Mu)))/pow2(mD32 - mQ32) + 2*log(pow2(Mu))*pow2(Mu)*
        ((1/mQ32 + 1/(-mD32 + mQ32))/mD32 + 1/((mD32 - pow2(Mu))*(-mQ32 + pow2(
        Mu)))) + (4*(2*mD32 - mQ32 + pow2(Mu))*pow2(log(mD3)))/pow2(mD32 -
        mQ32) + (4*(4*mQ32 - pow2(Mu))*pow2(log(mQ3)))/pow2(mD32 - mQ32) + (
        lMR*(4*mQ32*pow2(Mu) - 2*mD32*(mQ32 + pow2(Mu)) + pow4(mD3) - 2*pow4(
        mQ3)))/(mD32*(mD32 - mQ32)*mQ32) + (-3*mD32*mQ32 - 2*mD32*pow2(Mu) + 4*
        mQ32*pow2(Mu) + pow4(mD3) - 2*pow4(mQ3))/(mQ32*pow4(mD3) - mD32*pow4(
        mQ3)) - (2*log(mD3)*(2*mQ32*(2*mD32 + 3*mQ32)*log(mQ3)*(mD32 - pow2(Mu)
        ) + (2*lMR*mQ32 - pow2(Mu))*pow4(mD3) + (3 + 5*lMR)*pow2(Mu)*pow4(mQ3)
        + mD32*(4*mQ32*pow2(Mu) - 5*(1 + lMR)*pow4(mQ3)) - 2*(1 + lMR)*mQ32*
        pow4(Mu) + pow6(mD3)))/((mD32 - pow2(Mu))*pow2(-(mD32*mQ3) + pow3(mQ3))
        ) + (2*log(mQ3)*(((3 + 2*lMR)*mQ32 - (1 + 2*lMR)*pow2(Mu))*pow4(mD3) +
        2*pow2(Mu)*pow4(mQ3) - mD32*(-((5 + 7*lMR)*mQ32*pow2(Mu)) + 5*(1 + lMR)
        *pow4(mQ3) + 2*(1 + lMR)*pow4(Mu)) - 2*pow6(mQ3)))/((mQ32 - pow2(Mu))*
        pow2(-(mD3*mQ32) + pow3(mD3)))) - (pow4(Xb)*(18*mQ32*pow2(Mu)*pow4(mD3)
        + 12*lMR*mQ32*pow2(Mu)*pow4(mD3) - 12*mQ32*log(mD3)*pow2(Mu)*pow4(mD3)
        - 6*mQ32*log(pow2(Mu))*pow2(Mu)*pow4(mD3) - 12*mD32*pow2(Mu)*pow4(mQ3)
        - 6*lMR*mD32*pow2(Mu)*pow4(mQ3) - 24*mD32*log(mD3)*pow2(Mu)*pow4(mQ3) -
        24*lMR*mD32*log(mD3)*pow2(Mu)*pow4(mQ3) + 36*mD32*log(mQ3)*pow2(Mu)*
        pow4(mQ3) + 24*lMR*mD32*log(mQ3)*pow2(Mu)*pow4(mQ3) + 24*mD32*pow2(Mu)*
        pow2(log(mD3))*pow4(mQ3) - 24*mD32*pow2(Mu)*pow2(log(mQ3))*pow4(mQ3) -
        85*pow4(mD3)*pow4(mQ3) - 75*lMR*pow4(mD3)*pow4(mQ3) + 90*log(mD3)*pow4(
        mD3)*pow4(mQ3) + 12*lMR*log(mD3)*pow4(mD3)*pow4(mQ3) + 60*log(mQ3)*
        pow4(mD3)*pow4(mQ3) - 12*lMR*log(mQ3)*pow4(mD3)*pow4(mQ3) - 136*log(
        mD3)*log(mQ3)*pow4(mD3)*pow4(mQ3) + 56*pow2(log(mD3))*pow4(mD3)*pow4(
        mQ3) + 80*pow2(log(mQ3))*pow4(mD3)*pow4(mQ3) + 2*mD32*mQ32*dilog(1 -
        pow2(Mu)/mD32)*(-2*mD32*mQ32 + 6*mQ32*pow2(Mu) + pow4(mD3) - 2*pow4(
        mQ3) - 3*pow4(Mu)) - 2*mD32*mQ32*dilog(1 - pow2(Mu)/mQ32)*(-2*mD32*mQ32
        + 6*mQ32*pow2(Mu) + pow4(mD3) - 2*pow4(mQ3) - 3*pow4(Mu)) + 12*mD32*
        mQ32*log(mD3)*log(pow2(Mu))*pow4(Mu) - 12*mD32*mQ32*log(mQ3)*log(pow2(
        Mu))*pow4(Mu) - 12*mD32*mQ32*pow2(log(mD3))*pow4(Mu) + 12*mD32*mQ32*
        pow2(log(mQ3))*pow4(Mu) + 33*mQ32*pow6(mD3) + 31*lMR*mQ32*pow6(mD3) -
        100*mQ32*log(mD3)*pow6(mD3) - 36*lMR*mQ32*log(mD3)*pow6(mD3) + 38*mQ32*
        log(mQ3)*pow6(mD3) + 36*lMR*mQ32*log(mQ3)*pow6(mD3) - 64*mQ32*log(mD3)*
        log(mQ3)*pow6(mD3) - 2*pow2(Mu)*pow6(mD3) - 2*lMR*pow2(Mu)*pow6(mD3) +
        2*log(pow2(Mu))*pow2(Mu)*pow6(mD3) + 68*mQ32*pow2(log(mD3))*pow6(mD3) -
        4*mQ32*pow2(log(mQ3))*pow6(mD3) + 49*mD32*pow6(mQ3) + 41*lMR*mD32*pow6(
        mQ3) + 48*mD32*log(mD3)*pow6(mQ3) + 48*lMR*mD32*log(mD3)*pow6(mQ3) -
        130*mD32*log(mQ3)*pow6(mQ3) - 48*lMR*mD32*log(mQ3)*pow6(mQ3) - 80*mD32*
        log(mD3)*log(mQ3)*pow6(mQ3) - 4*pow2(Mu)*pow6(mQ3) - 4*lMR*pow2(Mu)*
        pow6(mQ3) + 4*log(pow2(Mu))*pow2(Mu)*pow6(mQ3) - 8*mD32*pow2(log(mD3))*
        pow6(mQ3) + 88*mD32*pow2(log(mQ3))*pow6(mQ3) + pow8(mD3) + lMR*pow8(
        mD3) - 2*log(mD3)*pow8(mD3) + 2*pow8(mQ3) + 2*lMR*pow8(mQ3) - 4*log(
        mQ3)*pow8(mQ3)))/(mD32*mQ32*pow4(mD32 - mQ32))))/pow2(cbeta) - (6*pow6(
        Xb)*(6*mD32*mQ32*dilog(1 - mD32/mQ32)*pow3(mD32 - mQ32) + 28*pow4(mQ3)*
        pow6(mD3) + 16*lMR*pow4(mQ3)*pow6(mD3) - 8*log(mQ3)*pow4(mQ3)*pow6(mD3)
        - 12*lMR*log(mQ3)*pow4(mQ3)*pow6(mD3) + 8*pow2(log(mQ3))*pow4(mQ3)*
        pow6(mD3) - 34*pow4(mD3)*pow6(mQ3) - 10*lMR*pow4(mD3)*pow6(mQ3) + 24*
        log(mQ3)*pow4(mD3)*pow6(mQ3) + 24*lMR*log(mQ3)*pow4(mD3)*pow6(mQ3) -
        16*pow2(log(mQ3))*pow4(mD3)*pow6(mQ3) + 8*mD32*mQ32*pow2(log(mD3))*(-2*
        mQ32*pow4(mD3) + 4*mD32*pow4(mQ3) + 3*pow6(mD3) + pow6(mQ3)) - 8*mQ32*
        pow8(mD3) - 8*lMR*mQ32*pow8(mD3) + 12*mQ32*log(mQ3)*pow8(mD3) + 24*
        mQ32*pow2(log(mQ3))*pow8(mD3) + 11*mD32*pow8(mQ3) - lMR*mD32*pow8(mQ3)
        - 24*mD32*log(mQ3)*pow8(mQ3) - 12*lMR*mD32*log(mQ3)*pow8(mQ3) + 32*
        mD32*pow2(log(mQ3))*pow8(mQ3) - 2*mD32*log(mD3)*((mD32 - mQ32)*(-(mQ32*
        pow4(mD3)) + (11 - 6*lMR)*mD32*pow4(mQ3) + pow6(mD3) + (13 + 6*lMR)*
        pow6(mQ3)) + 4*log(mQ3)*(-(pow4(mD3)*pow4(mQ3)) + 6*mQ32*pow6(mD3) + 2*
        mD32*pow6(mQ3) + 5*pow8(mQ3))) + power10(mD3) + lMR*power10(mD3) + 2*
        power10(mQ3) + 2*lMR*power10(mQ3) - 4*log(mQ3)*power10(mQ3)))/(mD32*
        mQ32*pow6(mD32 - mQ32));
}

double himalaya::ThresholdCalculator::getDeltaLambdaYt6(int omitLogs){
   using std::log;
   using std::sqrt;
   using gm2calc::dilog;
   
   const double MR2 = pow2(p.scale);
   const double lMR = omitLogs*log(MR2);
   double mQ32 = p.mq2(2,2);
   double mU32 = p.mu2(2,2);
   double mQ3 = sqrt(mQ32);
   double mU3 = sqrt(mU32);
   double mA = p.MA;
   const double beta = atan(p.vu/p.vd);
   const double sbeta = sin(beta);
   const double cbeta = cos(beta);
   const double Xt = p.Au(2,2) - p.mu*p.vd/p.vu;
   const double Yt = p.Au(2,2) + p.mu*p.vu/p.vd;
   const double Xt4 = pow4(Xt);
   const double Xt6 = pow6(Xt);
   double Mu = p.mu;
   
   return 6*(-12*(1 + lMR)*log(mQ3) - 8*(1 + lMR)*log(mU3) + 5*pow2(1 + lMR) + 12*
        pow2(log(mQ3)) + 8*pow2(log(mU3))) - (6*pow2(Xt)*(6*mQ32*(mQ32 - mU32)*
        mU32*dilog(1 - mQ32/mU32) + mU32*pow4(mQ3) - lMR*mU32*pow4(mQ3) - 32*
        mU32*log(mU3)*pow4(mQ3) - 32*lMR*mU32*log(mU3)*pow4(mQ3) + 36*mU32*
        pow2(log(mU3))*pow4(mQ3) - 4*mQ32*pow4(mU3) - 2*lMR*mQ32*pow4(mU3) +
        42*mQ32*log(mU3)*pow4(mU3) + 36*lMR*mQ32*log(mU3)*pow4(mU3) - 36*mQ32*
        pow2(log(mU3))*pow4(mU3) - 2*mQ32*log(mQ3)*(-((17 + 16*lMR)*mQ32*mU32)
        + 4*mQ32*mU32*log(mU3) + 2*pow4(mQ3) + (19 + 18*lMR)*pow4(mU3)) - 4*
        pow2(log(mQ3))*(7*mU32*pow4(mQ3) - 9*mQ32*pow4(mU3)) + 2*pow6(mQ3) + 2*
        lMR*pow6(mQ3) + pow6(mU3) + lMR*pow6(mU3) - 2*log(mU3)*pow6(mU3)))/(
        mQ32*mU32*pow2(mQ32 - mU32)) - (6*Xt6*(2*mQ32*(mQ32 - mU32)*mU32*dilog(
        1 - mQ32/mU32) + 9*mU32*pow4(mQ3) + 3*lMR*mU32*pow4(mQ3) - 26*mU32*log(
        mQ3)*pow4(mQ3) - 12*lMR*mU32*log(mQ3)*pow4(mQ3) + 20*mU32*log(mU3)*
        pow4(mQ3) + 12*lMR*mU32*log(mU3)*pow4(mQ3) - 48*mU32*log(mQ3)*log(mU3)*
        pow4(mQ3) + 36*mU32*pow2(log(mQ3))*pow4(mQ3) + 12*mU32*pow2(log(mU3))*
        pow4(mQ3) - 12*mQ32*pow4(mU3) - 6*lMR*mQ32*pow4(mU3) + 6*mQ32*log(mQ3)*
        pow4(mU3) + 6*mQ32*log(mU3)*pow4(mU3) - 40*mQ32*log(mQ3)*log(mU3)*pow4(
        mU3) + 20*mQ32*pow2(log(mQ3))*pow4(mU3) + 20*mQ32*pow2(log(mU3))*pow4(
        mU3) + dilog(1 - mU32/mQ32)*(-4*mU32*pow4(mQ3) + 4*mQ32*pow4(mU3)) + 2*
        pow6(mQ3) + 2*lMR*pow6(mQ3) - 4*log(mQ3)*pow6(mQ3) + pow6(mU3) + lMR*
        pow6(mU3) - 2*log(mU3)*pow6(mU3)))/(mQ32*mU32*pow4(mQ32 - mU32)) + (12*
        Xt4*(-29*mU32*pow4(mQ3) - 17*lMR*mU32*pow4(mQ3) - 6*mU32*log(mU3)*pow4(
        mQ3) - 10*lMR*mU32*log(mU3)*pow4(mQ3) + 6*mU32*pow2(log(mU3))*pow4(mQ3)
        + 28*mQ32*pow4(mU3) + 16*lMR*mQ32*pow4(mU3) - 44*mQ32*log(mU3)*pow4(
        mU3) - 14*lMR*mQ32*log(mU3)*pow4(mU3) + 10*mQ32*pow2(log(mU3))*pow4(
        mU3) + 2*mQ32*log(mQ3)*(5*(4 + lMR)*mQ32*mU32 + 4*mU32*(mQ32 + mU32)*
        log(mU3) - 2*pow4(mQ3) + (6 + 7*lMR)*pow4(mU3)) + dilog(1 - mQ32/mU32)*
        (-3*mU32*pow4(mQ3) + 3*mQ32*pow4(mU3)) - 2*pow2(log(mQ3))*(7*mU32*pow4(
        mQ3) + 9*mQ32*pow4(mU3)) + 2*pow6(mQ3) + 2*lMR*pow6(mQ3) - pow6(mU3) -
        lMR*pow6(mU3) + 2*log(mU3)*pow6(mU3)))/(mQ32*mU32*pow3(mQ32 - mU32)) +
        (2*(pow2(cbeta)*(3*(1.5 - 3*(3 + 2*lMR)*log(mQ3) - 5*log(mU3) + 2*pow2(
        lMR) - pow2(mA)/mQ32 - (2*pow2(mA))/mU32 + lMR*(-2*log(mU3) + (-(1/
        mQ32) - 2/mU32)*pow2(mA)) + 2*log(mA)*(7 - 6*log(mQ3) - 6*log(mU3) + (
        1/mQ32 + 2/mU32)*pow2(mA)) + pow2(Pi) + 12*pow2(log(mA)) + 12*pow2(log(
        mQ3)) + 8*pow2(log(mU3)) + (phixyz(pow2(mA),mQ32,mQ32)*(-deltaxyz(pow2(
        mA),mQ32,mQ32) - 7*mQ32*pow2(mA) + pow4(mA)))/pow4(mQ3) + (phixyz(pow2(
        mA),mU32,mU32)*(-deltaxyz(pow2(mA),mU32,mU32) - 6*mU32*pow2(mA) + pow4(
        mA)))/pow4(mU3)) + (6*Xt*Yt*(mQ32*phixyz(pow2(mA),mU32,mQ32)*(mQ32 -
        mU32 + pow2(mA))*pow4(mU3) + phixyz(pow2(mA),mQ32,mQ32)*(-deltaxyz(
        pow2(mA),mQ32,mQ32) - 7*mQ32*pow2(mA) + pow4(mA))*pow4(mU3) + pow4(mQ3)
        *(phixyz(pow2(mA),mU32,mU32)*(deltaxyz(pow2(mA),mU32,mU32) + 6*mU32*
        pow2(mA) - pow4(mA)) + 4*log(mQ3/mU3)*(-3 - 3*lMR + log(mA) + 3*log(
        mQ3) + 2*log(mU3))*pow4(mU3))))/((mQ32 - mU32)*pow4(mQ3)*pow4(mU3)) + (
        6*Yt*pow3(Xt)*(phixyz(pow2(mA),mQ32,mQ32)*((-5*mQ32 + mU32)*deltaxyz(
        pow2(mA),mQ32,mQ32) + (mQ32 - mU32)*pow2(mA)*(-7*mQ32 + pow2(mA)))*
        pow4(mU3) - mQ32*phixyz(pow2(mA),mU32,mQ32)*(-2*deltaxyz(pow2(mA),mU32,
        mQ32) + (mQ32 - mU32)*(mQ32 - mU32 + pow2(mA)))*pow4(mU3) + pow4(mQ3)*(
        phixyz(pow2(mA),mU32,mU32)*(-((mQ32 - 3*mU32)*deltaxyz(pow2(mA),mU32,
        mU32)) + (mQ32 - mU32)*pow2(mA)*(-6*mU32 + pow2(mA))) + 4*(-3*(2 + lMR)
        *(mQ32 - mU32) - log(mU3)*(3*(1 + lMR)*mQ32 + 3*(3 + lMR)*mU32 + log(
        mA)*(mQ32 - mU32 - 6*pow2(mA))) + log(mQ3)*(3*(3 + lMR)*mQ32 + 3*(1 +
        lMR)*mU32 + log(mA)*(mQ32 - mU32 - 6*pow2(mA)) + log(mU3)*(3*mQ32 +
        mU32 - 2*pow2(mA))) + (-5*mQ32 - 3*mU32 + 4*pow2(mA))*pow2(log(mQ3)) +
        2*(mQ32 + mU32 - pow2(mA))*pow2(log(mU3)))*pow4(mU3))))/(pow3(mQ32 -
        mU32)*pow4(mQ3)*pow4(mU3)) + (3*pow2(Yt)*(-2/mQ32 + lMR*(-2/mQ32 - 4/
        mU32) - 4/mU32 + (phixyz(pow2(mA),mU32,mQ32)*(deltaxyz(pow2(mA),mU32,
        mQ32) - pow2(mQ32 - mU32 + pow2(mA))))/(mQ32*deltaxyz(pow2(mA),mU32,
        mQ32)) + (2*log(mU3)*(-mQ32 + 3*mU32 - pow2(mA) - (2*mQ32*mU32*(mQ32 -
        mU32 + pow2(mA)))/deltaxyz(pow2(mA),mU32,mQ32) + (pow3(mQ32 - mU32) - (
        mQ32 + 5*mU32)*pow4(mA) - pow2(mA)*(4*mQ32*mU32 + pow4(mQ3) - 5*pow4(
        mU3)) + pow6(mA))/deltaxyz(pow2(mA),mQ32,mU32)))/(mQ32*mU32) + (2*log(
        mQ3)*(mQ32 + 2*mU32 - pow2(mA) - (mU32*(-2*mU32*pow2(mA) + pow4(mA) -
        pow4(mQ3) + pow4(mU3)))/deltaxyz(pow2(mA),mU32,mQ32) + (-pow3(mQ32 -
        mU32) - 3*(mQ32 + mU32)*pow4(mA) + 3*pow2(mA)*(pow4(mQ3) - pow4(mU3)) +
        pow6(mA))/deltaxyz(pow2(mA),mQ32,mU32)))/pow4(mU3) + (2*log(mA)*(3*
        mQ32*mU32 + (mQ32 + mU32)*pow2(mA) - (mQ32*mU32*(pow2(mQ32 - mU32) -
        pow4(mA)))/deltaxyz(pow2(mA),mU32,mQ32) - pow4(mQ3) - pow4(mU3) + (-((
        3*mQ32 + 5*mU32)*pow2(mA)*pow2(mQ32 - mU32)) + pow4(mA)*(4*mQ32*mU32 +
        3*pow4(mQ3) + 5*pow4(mU3)) + pow4(mQ32 - mU32) - (mQ32 + mU32)*pow6(mA)
        )/deltaxyz(pow2(mA),mQ32,mU32)))/(mQ32*pow4(mU3)) + (phixyz(pow2(mA),
        mQ32,mU32)*(-4*(mQ32 + mU32)*pow2(mA)*pow2(mQ32 - mU32) + 2*pow2(
        deltaxyz(pow2(mA),mQ32,mU32)) - 3*deltaxyz(pow2(mA),mQ32,mU32)*(-2*(
        mQ32 + mU32)*pow2(mA) + pow2(mQ32 - mU32) + pow4(mA)) + 2*pow4(mA)*(2*
        mQ32*mU32 + 3*pow4(mQ3) + pow4(mU3)) + pow4(mQ32 - mU32) - 4*(mQ32 +
        mU32)*pow6(mA) + pow8(mA)))/(deltaxyz(pow2(mA),mQ32,mU32)*pow6(mU3))))/
        2. + 3*Xt4*((3*(mQ32 - mU32) + (-5 - (2*mQ32)/mU32 + mU32/mQ32)*pow2(
        mA))/pow3(mQ32 - mU32) + (3*log(mQ3)*(4*(1 + lMR)*mQ32*pow2(mA) - (1 +
        2*lMR)*(pow4(mQ3) - pow4(mU3))))/pow4(mQ32 - mU32) + (3*log(mU3)*(-4*
        mQ32*pow2(mA) + pow4(mQ3) - pow4(mU3)))/pow4(mQ32 - mU32) + (2*log(mA)*
        (6*mQ32*mU32*log(mQ3)*(-2*mQ32*pow2(mA) + pow4(mQ3) - pow4(mU3)) + 6*
        mQ32*mU32*log(mU3)*(2*mQ32*pow2(mA) - pow4(mQ3) + pow4(mU3)) + (mQ32 -
        mU32)*(-6*mU32*pow4(mQ3) + pow2(mA)*(5*mQ32*mU32 + 2*pow4(mQ3) - pow4(
        mU3)) + 6*mQ32*pow4(mU3))))/(mQ32*mU32*pow4(mQ32 - mU32)) + (lMR*(-6*
        log(mU3)*(2*mQ32*pow2(mA) - pow4(mQ3) + pow4(mU3)) + ((mQ32 - mU32)*(6*
        mQ32*(mQ32 - mU32)*mU32 + pow2(mA)*(-5*mQ32*mU32 - 2*pow4(mQ3) + pow4(
        mU3))))/(mQ32*mU32)))/pow4(mQ32 - mU32) + pow2(Yt)*((-11*mQ32*mU32 - 2*
        pow4(mQ3) + pow4(mU3))/(mQ32*mU32*pow3(mQ32 - mU32)) + (4*(-11*mQ32 -
        3*mU32 + 7*pow2(mA))*pow2(log(mQ3)))/pow4(mQ32 - mU32) - (8*(mQ32 + 3*
        mU32 - 2*pow2(mA))*pow2(log(mU3)))/pow4(mQ32 - mU32) - (phixyz(pow2(mA)
        ,mU32,mQ32)*((mQ32 - mU32)*deltaxyz(pow2(mA),mU32,mQ32)*(3*mQ32 - 3*
        mU32 + 4*pow2(mA)) - 6*pow2(deltaxyz(pow2(mA),mU32,mQ32)) + pow2(mQ32 -
        mU32)*pow2(mQ32 - mU32 + pow2(mA))))/(2.*mQ32*deltaxyz(pow2(mA),mU32,
        mQ32)*pow4(mQ32 - mU32)) + (phixyz(pow2(mA),mU32,mU32)*((mQ32 - 5*mU32)
        *deltaxyz(pow2(mA),mU32,mU32) + (-mQ32 + mU32)*pow2(mA)*(-6*mU32 +
        pow2(mA))))/(pow4(mU3)*pow4(mQ32 - mU32)) + (phixyz(pow2(mA),mQ32,mQ32)
        *((-8*mQ32 + mU32)*deltaxyz(pow2(mA),mQ32,mQ32) + (mQ32 - mU32)*pow2(
        mA)*(-7*mQ32 + pow2(mA))))/pow4(-(mQ3*mU32) + pow3(mQ3)) - (lMR*(3*
        mU32*pow4(mQ3) + 12*mU32*log(mU3)*pow4(mQ3) - 6*mQ32*pow4(mU3) + 2*
        pow6(mQ3) + pow6(mU3)))/(mQ32*mU32*pow4(mQ32 - mU32)) + (log(mA)*(-12*
        log(mQ3)*(-mQ32 + mU32 + pow2(mA)) + 12*log(mU3)*(-mQ32 + mU32 + pow2(
        mA)) - (pow2(mQ32 - mU32)*(mQ32*mU32*(pow2(mQ32 - mU32) - pow4(mA)) +
        deltaxyz(pow2(mA),mU32,mQ32)*(-3*mQ32*mU32 + (-mQ32 + mU32)*pow2(mA) +
        pow4(mQ3) - 3*pow4(mU3))))/(mQ32*deltaxyz(pow2(mA),mU32,mQ32)*pow4(mU3)
        ) + (pow3(mQ32 - mU32)*(3*(mQ32 + mU32)*pow4(mA) - 3*mU32*pow4(mQ3) -
        mQ32*pow4(mU3) - pow2(mA)*(3*pow4(mQ3) + 5*pow4(mU3)) - pow6(mA) +
        pow6(mQ3) + 3*pow6(mU3)))/(mQ32*deltaxyz(pow2(mA),mQ32,mU32)*pow4(mU3))
        ))/pow4(mQ32 - mU32) + (log(mU3)*(pow2(mA)*pow2(mQ32 - mU32) - (2*mQ32*
        mU32*(mQ32 - mU32 + pow2(mA))*pow2(mQ32 - mU32))/deltaxyz(pow2(mA),
        mU32,mQ32) - 7*mU32*pow4(mQ3) - 13*mQ32*pow4(mU3) - 3*pow6(mQ3) - pow6(
        mU3) + (pow2(mQ32 - mU32)*((5*mQ32 + 3*mU32)*pow4(mA) - 11*mU32*pow4(
        mQ3) + 5*mQ32*pow4(mU3) - pow2(mA)*(4*mQ32*mU32 + 7*pow4(mQ3) + 5*pow4(
        mU3)) - pow6(mA) + 3*pow6(mQ3) + 3*pow6(mU3)))/deltaxyz(pow2(mA),mQ32,
        mU32)))/(mQ32*mU32*pow4(mQ32 - mU32)) + (log(mQ3)*(12*lMR*mQ32 + 4*log(
        mU3)*(13*mQ32 + 9*mU32 - 11*pow2(mA)) + (pow2(mQ32 - mU32)*(-((3*mQ32 +
        5*mU32)*pow4(mA)) + mU32*pow4(mQ3) + 9*mQ32*pow4(mU3) + pow2(mA)*(4*
        mQ32*mU32 + 3*pow4(mQ3) + 9*pow4(mU3)) + pow6(mA) - pow6(mQ3) - 9*pow6(
        mU3)))/(deltaxyz(pow2(mA),mQ32,mU32)*pow4(mU3)) + (-(pow2(mA)*pow2(mQ32
        - mU32)) + 2*mU32*pow4(mQ3) + 17*mQ32*pow4(mU3) - (pow2(-(mQ32*mU3) +
        pow3(mU3))*(-2*mU32*pow2(mA) + pow4(mA) - pow4(mQ3) + pow4(mU3)))/
        deltaxyz(pow2(mA),mU32,mQ32) + pow6(mQ3) + 4*pow6(mU3))/pow4(mU3)))/
        pow4(mQ32 - mU32) + (phixyz(pow2(mA),mQ32,mU32)*(2*pow2(deltaxyz(pow2(
        mA),mQ32,mU32))*(-5*mQ32*mU32 + pow4(mQ3) + 12*pow4(mU3)) - (mQ32 -
        mU32)*deltaxyz(pow2(mA),mQ32,mU32)*(3*(mQ32 - 3*mU32)*pow4(mA) - 15*
        mU32*pow4(mQ3) - 6*pow2(mA)*(-2*mQ32*mU32 + pow4(mQ3) - 3*pow4(mU3)) +
        29*mQ32*pow4(mU3) + 3*pow6(mQ3) - 17*pow6(mU3)) + pow2(mQ32 - mU32)*(-
        4*(mQ32 + mU32)*pow2(mA)*pow2(mQ32 - mU32) + pow2(mQ32 - mU32)*(-2*
        mQ32*mU32 + pow4(mQ3) - 3*pow4(mU3)) + pow4(mA)*(4*mQ32*mU32 + 6*pow4(
        mQ3) + 6*pow4(mU3)) - 4*(mQ32 + mU32)*pow6(mA) + pow8(mA))))/(2.*
        deltaxyz(pow2(mA),mQ32,mU32)*pow4(mQ32 - mU32)*pow6(mU3)))) + 3*pow2(
        Xt)*((2*log(mU3)*(mQ32 - 5*mU32 + 2*pow2(mA)))/pow2(mQ32 - mU32) + (2*
        log(mQ3)*(mQ32 + 2*lMR*mQ32 + 3*mU32 + 2*log(mU3)*(mQ32 + mU32 - pow2(
        mA)) - 2*(1 + lMR)*pow2(mA)))/pow2(mQ32 - mU32) + (2*lMR*(-mQ32 + mU32
        + (-3 + (2*mQ32)/mU32 + mU32/mQ32)*pow2(mA) + 2*log(mU3)*(-mQ32 + pow2(
        mA))))/pow2(mQ32 - mU32) + (4*(mQ32 - 3*mU32 + pow2(mA))*pow2(log(mQ3))
        )/pow2(mQ32 - mU32) - (8*pow2(log(mU3)))/(mQ32 - mU32) + (deltaxyz(
        pow2(mA),mU32,mQ32)*phixyz(pow2(mA),mU32,mQ32))/pow2(-(mQ3*mU32) +
        pow3(mQ3)) + (phixyz(pow2(mA),mQ32,mQ32)*((-2*mQ32 + mU32)*deltaxyz(
        pow2(mA),mQ32,mQ32) + (mQ32 - mU32)*pow2(mA)*(-7*mQ32 + pow2(mA))))/(
        pow2(mQ32 - mU32)*pow4(mQ3)) + (phixyz(pow2(mA),mU32,mU32)*(deltaxyz(
        pow2(mA),mU32,mU32) + 6*mU32*pow2(mA) - pow4(mA)))/((mQ32 - mU32)*pow4(
        mU3)) + (-4*mQ32*mU32 + 4*mQ32*pow2(mA) - 2*mU32*pow2(mA))/(mU32*pow4(
        mQ3) - mQ32*pow4(mU3)) - (4*log(mA)*(-(mQ32*mU32*log(mQ3)*(-5*mQ32 + 5*
        mU32 + pow2(mA))) + mQ32*mU32*log(mU3)*(-5*mQ32 + 5*mU32 + pow2(mA)) +
        pow2(mA)*(-3*mQ32*mU32 + 2*pow4(mQ3) + pow4(mU3))))/(mQ32*mU32*pow2(
        mQ32 - mU32)) + pow2(Yt)*((12*pow2(log(mQ3)))/pow2(mQ32 - mU32) + (8*
        pow2(log(mU3)))/pow2(mQ32 - mU32) + (phixyz(pow2(mA),mU32,mQ32)*(
        deltaxyz(pow2(mA),mU32,mQ32)*pow2(mA) + (mQ32 - mU32)*pow2(mQ32 - mU32
        + pow2(mA))))/(deltaxyz(pow2(mA),mU32,mQ32)*pow2(-(mQ3*mU32) + pow3(
        mQ3))) + (phixyz(pow2(mA),mQ32,mQ32)*(-deltaxyz(pow2(mA),mQ32,mQ32) -
        7*mQ32*pow2(mA) + pow4(mA)))/(pow2(mQ32 - mU32)*pow4(mQ3)) + (2*log(
        mU3)*((-mQ32 + mU32)*deltaxyz(pow2(mA),mU32,mQ32)*((2*mQ32 - mU32)*
        pow2(mQ32 - mU32) + (2*mQ32 + mU32)*pow4(mA) - 4*pow2(mA)*(2*mQ32*mU32
        + pow4(mQ3))) + deltaxyz(pow2(mA),mQ32,mU32)*(2*mQ32*(mQ32 - mU32)*
        mU32*(mQ32 - mU32 + pow2(mA)) + deltaxyz(pow2(mA),mU32,mQ32)*(mQ32*mU32
        + 2*pow4(mQ3) - pow4(mU3)))))/(mQ32*mU32*deltaxyz(pow2(mA),mQ32,mU32)*
        deltaxyz(pow2(mA),mU32,mQ32)*pow2(mQ32 - mU32)) + (phixyz(pow2(mA),
        mU32,mU32)*(-deltaxyz(pow2(mA),mU32,mU32) - 6*mU32*pow2(mA) + pow4(mA))
        )/(pow2(mQ32 - mU32)*pow4(mU3)) + (2*lMR*(-3*mQ32*mU32 + 2*mQ32*mU32*
        log(mU3) + 2*pow4(mQ3) + pow4(mU3)))/(mQ32*mU32*pow2(mQ32 - mU32)) + (
        4*mQ32 - 2*mU32)/(mU32*pow4(mQ3) - mQ32*pow4(mU3)) + (2*log(mQ3)*(-2*
        lMR - 10*log(mU3) + (-2*mQ32*mU32 + (mQ32 - mU32)*pow2(mA) - pow4(mQ3)
        + pow4(mU3) - (mU32*(-mQ32 + mU32)*(-2*mU32*pow2(mA) + pow4(mA) - pow4(
        mQ3) + pow4(mU3)))/deltaxyz(pow2(mA),mU32,mQ32))/pow4(mU3) + ((mQ32 -
        mU32)*(pow2(-(mQ3*mU32) + pow3(mQ3)) + (3*mQ32 + 4*mU32)*pow4(mA) -
        pow2(mA)*(2*mQ32*mU32 + 3*pow4(mQ3) + 7*pow4(mU3)) - pow6(mA)))/(
        deltaxyz(pow2(mA),mQ32,mU32)*pow4(mU3))))/pow2(mQ32 - mU32) + (2*log(
        mA)*(log(mQ32/mU32) + ((mQ32 - mU32)*(deltaxyz(pow2(mA),mQ32,mU32)*(
        mQ32*mU32*(pow2(mQ32 - mU32) - pow4(mA)) + deltaxyz(pow2(mA),mU32,mQ32)
        *(-3*mQ32*mU32 - mQ32*pow2(mA) + pow4(mQ3) + pow4(mU3))) + deltaxyz(
        pow2(mA),mU32,mQ32)*(pow4(mA)*(-2*mQ32*mU32 - 3*pow4(mQ3) + pow4(mU3))
        - pow4(mQ32 - mU32) + mQ32*pow6(mA) + pow2(mA)*(-2*mU32*pow4(mQ3) -
        mQ32*pow4(mU3) + 3*pow6(mQ3)))))/(mQ32*deltaxyz(pow2(mA),mQ32,mU32)*
        deltaxyz(pow2(mA),mU32,mQ32)*pow4(mU3))))/pow2(mQ32 - mU32) + (phixyz(
        pow2(mA),mQ32,mU32)*(-2*(mQ32 - 2*mU32)*pow2(deltaxyz(pow2(mA),mQ32,
        mU32)) + deltaxyz(pow2(mA),mQ32,mU32)*((3*mQ32 - 5*mU32)*pow2(mQ32 -
        mU32) + (3*mQ32 - 5*mU32)*pow4(mA) + pow2(mA)*(4*mQ32*mU32 - 6*pow4(
        mQ3) + 14*pow4(mU3))) + (-mQ32 + mU32)*(pow4(mA)*(4*mQ32*mU32 + 6*pow4(
        mQ3) + 6*pow4(mU3)) + pow4(mQ32 - mU32) - 4*(mQ32 + mU32)*pow6(mA) - 4*
        pow2(mA)*(-(mU32*pow4(mQ3)) + pow6(mQ3)) + pow8(mA))))/(deltaxyz(pow2(
        mA),mQ32,mU32)*pow2(mQ32 - mU32)*pow6(mU3))))) + (3*(-4*mQ32*mU32*pow2(
        lMR)*pow2(mQ32 - pow2(Mu))*pow2(-mU32 + pow2(Mu)) - 2*lMR*(-2*mQ32*
        mU32*log(mU3) + (2*mQ32 + mU32)*(mQ32 + mU32 - 2*pow2(Mu)))*pow2(mQ32 -
        pow2(Mu))*pow2(-mU32 + pow2(Mu)) - 16*mU32*(mQ32 - 2*pow2(Mu))*pow2(
        log(mQ3))*pow2(-mU32 + pow2(Mu))*pow4(mQ3) - 4*mQ32*mU32*dilog(1 -
        mQ32/pow2(Mu))*pow2(-mU32 + pow2(Mu))*(-2*mQ32*pow2(Mu) + pow4(mQ3) -
        pow4(Mu)) + 8*mQ32*mU32*dilog(1 - mQ32/pow2(Mu))*pow2(-mU32 + pow2(Mu))
        *pow4(Mu) + 4*mQ32*mU32*dilog(1 - mU32/pow2(Mu))*pow2(mQ32 - pow2(Mu))*
        (2*mU32*pow2(Mu) + pow4(Mu) - pow4(mU3)) + 8*mQ32*(-mU32 + 2*pow2(Mu))*
        pow2(log(mU3))*pow2(mQ32 - pow2(Mu))*pow4(mU3) - (mQ32 - pow2(Mu))*(-
        mU32 + pow2(Mu))*(-(mQ32*mU32*(3*mQ32*mU32 + 4*pow4(mQ3) + 2*pow4(mU3))
        ) - 3*pow4(Mu)*(7*mQ32*mU32 + 4*pow4(mQ3) + 2*pow4(mU3)) + 4*(2*mQ32 +
        mU32)*pow6(Mu) + pow2(Mu)*(17*mU32*pow4(mQ3) + 13*mQ32*pow4(mU3) + 4*
        pow6(mQ3) + 2*pow6(mU3))) + 2*mU32*log(mU3)*(mQ32 - pow2(Mu))*(-2*mU32*
        pow2(Mu)*pow2(-mU32 + pow2(Mu)) + pow4(mQ3)*(-4*mU32*pow2(Mu) + 9*pow4(
        Mu) + pow4(mU3)) + mQ32*(14*mU32*pow4(Mu) - 9*pow2(Mu)*pow4(mU3) - 13*
        pow6(Mu) + 2*pow6(mU3))) + 2*mQ32*mU32*pow2(log(pow2(Mu)))*(4*mQ32*
        mU32*(mQ32 + mU32)*pow2(Mu) + 2*mU32*(-4*mQ32 + mU32)*pow4(Mu) - 2*
        pow4(mQ3)*pow4(mU3) - 4*mU32*pow6(Mu) + 4*pow8(Mu)) + 2*mQ32*log(mQ3)*(
        (-mU32 + pow2(Mu))*(-((13 + 6*lMR)*pow4(Mu)*pow4(mU3)) + 2*mQ32*pow2(
        Mu)*(-((7 + 6*lMR)*mU32*pow2(Mu)) + 2*pow4(Mu) + 3*(1 + 2*lMR)*pow4(
        mU3)) + pow4(mQ3)*(3*(5 + 2*lMR)*mU32*pow2(Mu) - 8*pow4(Mu) - (5 + 6*
        lMR)*pow4(mU3)) + 4*(-mU32 + pow2(Mu))*pow6(mQ3) + 3*(5 + 2*lMR)*mU32*
        pow6(Mu)) + 4*log(mU3)*(2*mQ32*(mQ32 + mU32)*pow2(Mu)*pow4(mU3) - 2*
        pow4(mU3)*pow6(Mu) - pow4(mQ3)*pow6(mU3) + pow4(Mu)*(-4*mQ32*pow4(mU3)
        + pow6(mU3)) + 2*mU32*pow8(Mu))) - 2*log(pow2(Mu))*(-2*mQ32*mU32*log(
        mQ3)*(-4*pow4(Mu)*pow4(mU3) + pow4(mQ3)*(-4*mU32*pow2(Mu) + pow4(Mu) +
        2*pow4(mU3)) + 8*mU32*pow6(Mu) - 2*mQ32*(-4*mU32*pow4(Mu) + 2*pow2(Mu)*
        pow4(mU3) + pow6(Mu)) - 5*pow8(Mu)) + 2*mQ32*mU32*log(mU3)*(pow4(mQ3)*(
        4*mU32*pow2(Mu) + pow4(Mu) - 2*pow4(mU3)) - 2*mQ32*(4*mU32*pow4(Mu) -
        2*pow2(Mu)*pow4(mU3) + pow6(Mu)) + 3*pow8(Mu)) + pow2(Mu)*(2*mU32*pow2(
        -mU32 + pow2(Mu))*pow4(Mu) + (-3*mU32*pow2(Mu) + 4*pow4(Mu) + 2*pow4(
        mU3))*pow6(mQ3) + pow4(mQ3)*(8*mU32*pow4(Mu) - 8*pow2(Mu)*pow4(mU3) -
        8*pow6(Mu) + 2*pow6(mU3)) + mQ32*(-2*pow4(Mu)*pow4(mU3) - mU32*pow6(Mu)
        + 2*pow2(Mu)*pow6(mU3) + 4*pow8(Mu))))))/(2.*mQ32*mU32*pow2(mQ32 -
        pow2(Mu))*pow2(-mU32 + pow2(Mu))) - (3*Xt4*(24*mQ32*mU32*(-((1 + lMR)*(
        mQ32 - mU32)) + 2*mQ32*log(mQ3) - 2*mU32*log(mU3))*(-mQ32 + mU32 + (
        mQ32 + mU32)*log(mQ3) - (mQ32 + mU32)*log(mU3))*pow2(mQ32 - pow2(Mu))*
        pow2(-mU32 + pow2(Mu)) - 2*mQ32*mU32*dilog(1 - mQ32/pow2(Mu))*pow2(mQ32
        - pow2(Mu))*pow2(-mU32 + pow2(Mu))*(mQ32*(2*mU32 - 6*pow2(Mu)) + 2*
        pow4(mQ3) + 3*pow4(Mu) - pow4(mU3)) + 2*mQ32*mU32*dilog(1 - mU32/pow2(
        Mu))*pow2(mQ32 - pow2(Mu))*pow2(-mU32 + pow2(Mu))*(mQ32*(2*mU32 - 6*
        pow2(Mu)) + 2*pow4(mQ3) + 3*pow4(Mu) - pow4(mU3)) + 4*mQ32*(mQ32 +
        mU32)*pow2(log(mU3))*pow2(mQ32 - pow2(Mu))*pow4(mU3)*(mQ32*(mU32 - 2*
        pow2(Mu)) - 6*mU32*pow2(Mu) + 4*pow4(Mu) + 3*pow4(mU3)) + 8*mU32*pow2(
        log(mQ3))*pow2(-mU32 + pow2(Mu))*pow4(mQ3)*((4*mQ32 + 3*mU32)*pow4(Mu)
        + mQ32*(3*mQ32*mU32 + 3*pow4(mQ3) + pow4(mU3)) - 2*pow2(Mu)*(3*mQ32*
        mU32 + 3*pow4(mQ3) + pow4(mU3))) + lMR*pow2(mQ32 - pow2(Mu))*pow2(-mU32
        + pow2(Mu))*(6*mQ32*mU32*log(mU3)*(mQ32*(2*mU32 - 4*pow2(Mu)) + 3*pow4(
        mQ3) - pow4(mU3)) + (mQ32 - mU32)*((13*mU32 - 4*pow2(Mu))*pow4(mQ3) +
        2*pow2(Mu)*pow4(mU3) - 2*mQ32*(5*mU32*pow2(Mu) + pow4(mU3)) + 2*pow6(
        mQ3) - pow6(mU3))) + mU32*log(mU3)*(mQ32 - pow2(Mu))*(2*pow2(-(Mu*mU32)
        + pow3(Mu))*pow6(mU3) + pow6(mQ3)*(130*mU32*pow4(Mu) - 113*pow2(Mu)*
        pow4(mU3) - 39*pow6(Mu) + 34*pow6(mU3)) + (-48*mU32*pow2(Mu) + 19*pow4(
        Mu) + 25*pow4(mU3))*pow8(mQ3) + 3*pow4(mQ3)*(31*pow4(Mu)*pow4(mU3) -
        34*mU32*pow6(Mu) - 2*pow2(Mu)*pow6(mU3) + 8*pow8(Mu) - 7*pow8(mU3)) +
        mQ32*mU32*(-22*pow4(Mu)*pow4(mU3) - 5*mU32*pow6(Mu) + 21*pow2(Mu)*pow6(
        mU3) + 12*pow8(Mu) - 2*pow8(mU3))) + (mQ32 - mU32)*(mQ32 - pow2(Mu))*(-
        mU32 + pow2(Mu))*(-(pow2(Mu)*pow4(mU3)*(-3*mU32*pow2(Mu) + 2*pow4(Mu) +
        pow4(mU3))) + (28*mU32*pow2(Mu) - 6*pow4(Mu) - 24*pow4(mU3))*pow6(mQ3)
        + pow4(mQ3)*(-38*mU32*pow4(Mu) + 31*pow2(Mu)*pow4(mU3) + 4*pow6(Mu) +
        7*pow6(mU3)) + 2*(-mU32 + pow2(Mu))*pow8(mQ3) + mQ32*(-13*pow4(Mu)*
        pow4(mU3) + 16*mU32*pow6(Mu) - 6*pow2(Mu)*pow6(mU3) + pow8(mU3))) -
        log(pow2(Mu))*(2*(mQ32 - mU32)*pow2(Mu)*(pow2(-mU32 + pow2(Mu))*pow4(
        Mu)*pow4(mU3) + pow6(mQ3)*(-8*mU32*pow4(Mu) + 9*pow2(Mu)*pow4(mU3) + 4*
        pow6(Mu) - 2*pow6(mU3)) + (3*mU32*pow2(Mu) - 2*pow4(Mu) - 2*pow4(mU3))*
        pow8(mQ3) + mQ32*(5*pow4(mU3)*pow6(Mu) - 2*pow4(Mu)*pow6(mU3) - 2*mU32*
        pow8(Mu)) + pow4(mQ3)*(-7*pow4(Mu)*pow4(mU3) + 5*mU32*pow6(Mu) - 2*
        pow8(Mu) + pow8(mU3))) + 2*mQ32*mU32*log(mQ3)*(2*pow6(mQ3)*(-22*mU32*
        pow4(Mu) + 14*pow2(Mu)*pow4(mU3) + 9*pow6(Mu) - 2*pow6(mU3)) + (8*mU32*
        pow2(Mu) - 3*pow4(Mu) - 4*pow4(mU3))*pow8(mQ3) - pow4(mU3)*pow8(Mu) +
        2*mQ32*pow2(Mu)*(9*pow4(Mu)*pow4(mU3) - 14*mU32*pow6(Mu) + 2*pow2(Mu)*
        pow6(mU3) + 6*pow8(Mu) - 2*pow8(mU3)) + pow4(mQ3)*(-41*pow4(Mu)*pow4(
        mU3) + 60*mU32*pow6(Mu) + 4*pow2(Mu)*pow6(mU3) - 25*pow8(Mu) + 2*pow8(
        mU3))) - 2*mQ32*mU32*log(mU3)*(2*pow6(mQ3)*(-22*mU32*pow4(Mu) + 14*
        pow2(Mu)*pow4(mU3) + 9*pow6(Mu) - 2*pow6(mU3)) + (8*mU32*pow2(Mu) - 3*
        pow4(Mu) - 4*pow4(mU3))*pow8(mQ3) - pow4(mU3)*pow8(Mu) + 2*mQ32*pow2(
        Mu)*(9*pow4(Mu)*pow4(mU3) - 14*mU32*pow6(Mu) + 2*pow2(Mu)*pow6(mU3) +
        6*pow8(Mu) - 2*pow8(mU3)) + pow4(mQ3)*(-41*pow4(Mu)*pow4(mU3) + 60*
        mU32*pow6(Mu) + 4*pow2(Mu)*pow6(mU3) - 25*pow8(Mu) + 2*pow8(mU3)))) -
        mQ32*log(mQ3)*(6*lMR*mU32*pow2(mQ32 - pow2(Mu))*pow2(-mU32 + pow2(Mu))*
        (mQ32*(2*mU32 - 4*pow2(Mu)) + 3*pow4(mQ3) - pow4(mU3)) + 4*mU32*log(
        mU3)*(-2*pow6(mQ3)*(-19*mU32*pow4(Mu) + 17*pow2(Mu)*pow4(mU3) + 6*pow6(
        Mu) - 5*pow6(mU3)) - 6*pow6(Mu)*pow6(mU3) + (-14*mU32*pow2(Mu) + 6*
        pow4(Mu) + 7*pow4(mU3))*pow8(mQ3) + 4*pow4(mU3)*pow8(Mu) + 3*pow4(Mu)*
        pow8(mU3) + pow4(mQ3)*(55*pow4(Mu)*pow4(mU3) - 38*mU32*pow6(Mu) - 30*
        pow2(Mu)*pow6(mU3) + 8*pow8(Mu) + 5*pow8(mU3)) + 2*mQ32*(-16*pow4(mU3)*
        pow6(Mu) + 15*pow4(Mu)*pow6(mU3) + 5*mU32*pow8(Mu) - 5*pow2(Mu)*pow8(
        mU3))) - (-mU32 + pow2(Mu))*(-2*pow6(mQ3)*(-63*mU32*pow4(Mu) + 65*pow2(
        Mu)*pow4(mU3) + 2*pow6(Mu) - 2*pow6(mU3)) + 5*pow6(Mu)*pow6(mU3) + (-
        53*mU32*pow2(Mu) + 8*pow4(Mu) + 47*pow4(mU3))*pow8(mQ3) + pow4(mQ3)*(
        131*pow4(Mu)*pow4(mU3) - 121*mU32*pow6(Mu) + 9*pow2(Mu)*pow6(mU3) - 19*
        pow8(mU3)) - 7*pow4(Mu)*pow8(mU3) + mQ32*(-24*pow4(mU3)*pow6(Mu) - 42*
        pow4(Mu)*pow6(mU3) + 36*mU32*pow8(Mu) + 34*pow2(Mu)*pow8(mU3)) - 4*(-
        mU32 + pow2(Mu))*power10(mQ3)))))/(mQ32*mU32*pow2(mQ32 - pow2(Mu))*
        pow2(-mU32 + pow2(Mu))*pow4(mQ32 - mU32)) + (3*pow2(Xt)*(12*mQ32*mU32*(
        -((1 + lMR)*(mQ32 - mU32)) + 2*mQ32*log(mQ3) - 2*mU32*log(mU3))*log(
        mQ32/mU32)*pow2(mQ32 - pow2(Mu))*pow2(-mU32 + pow2(Mu)) - 4*mQ32*mU32*
        dilog(1 - mQ32/pow2(Mu))*pow2(-mU32 + pow2(Mu))*pow3(mQ32 - pow2(Mu)) +
        4*mQ32*mU32*dilog(1 - mU32/pow2(Mu))*pow2(-mU32 + pow2(Mu))*pow3(mQ32 -
        pow2(Mu)) + 8*mU32*pow2(log(mQ3))*pow2(-mU32 + pow2(Mu))*pow4(mQ3)*(2*
        mQ32*mU32 - 2*(mQ32 + 2*mU32)*pow2(Mu) + pow4(mQ3) + 3*pow4(Mu)) + lMR*
        pow2(mQ32 - pow2(Mu))*pow2(-mU32 + pow2(Mu))*(4*mQ32*mU32*log(mU3)*(2*
        mQ32 + mU32 - 2*pow2(Mu)) + (mQ32 - mU32)*(mQ32*(4*mU32 - 8*pow2(Mu)) +
        4*mU32*pow2(Mu) + 4*pow4(mQ3) - 2*pow4(mU3))) + (mQ32 - mU32)*pow2(mQ32
        - pow2(Mu))*pow2(-mU32 + pow2(Mu))*(mQ32*(6*mU32 - 8*pow2(Mu)) + 4*
        mU32*pow2(Mu) + 4*pow4(mQ3) - 2*pow4(mU3)) + 8*mQ32*pow2(log(mU3))*
        pow2(mQ32 - pow2(Mu))*(mU32*(mQ32 + mU32) - 2*(mQ32 + mU32)*pow2(Mu) +
        2*pow4(Mu))*pow4(mU3) + 2*mU32*log(mU3)*(mQ32 - pow2(Mu))*(-mU32 +
        pow2(Mu))*(2*pow2(Mu)*(-mU32 + pow2(Mu))*pow4(mU3) - pow4(mQ3)*(-16*
        mU32*pow2(Mu) + pow4(Mu) + 3*pow4(mU3)) + (-7*mU32 + pow2(Mu))*pow6(
        mQ3) + mQ32*(-17*mU32*pow4(Mu) + 5*pow2(Mu)*pow4(mU3) + 4*pow6(Mu) + 2*
        pow6(mU3))) - 2*mQ32*log(mQ3)*((mQ32 - pow2(Mu))*(-mU32 + pow2(Mu))*(-(
        (11 + 6*lMR)*pow4(Mu)*pow4(mU3)) - mQ32*mU32*(-2*(1 + 5*lMR)*mU32*pow2(
        Mu) + (1 + 8*lMR)*pow4(Mu) + (-3 + 2*lMR)*pow4(mU3)) + pow4(mQ3)*((9 +
        4*lMR)*mU32*pow2(Mu) - 4*pow4(Mu) - (7 + 4*lMR)*pow4(mU3)) + 4*(-mU32 +
        pow2(Mu))*pow6(mQ3) + 4*(1 + lMR)*mU32*pow6(Mu) + (5 + 2*lMR)*pow2(Mu)*
        pow6(mU3)) + 4*mU32*log(mU3)*(mU32*pow4(Mu)*(-2*mU32*pow2(Mu) + 2*pow4(
        Mu) + pow4(mU3)) + (-4*mU32*pow2(Mu) + pow4(Mu) + 2*pow4(mU3))*pow6(
        mQ3) + pow4(mQ3)*(12*mU32*pow4(Mu) - 10*pow2(Mu)*pow4(mU3) - 2*pow6(Mu)
        + 3*pow6(mU3)) + mQ32*(16*pow4(Mu)*pow4(mU3) - 16*mU32*pow6(Mu) - 6*
        pow2(Mu)*pow6(mU3) + 3*pow8(Mu)))) - 2*log(pow2(Mu))*(2*(mQ32 - mU32)*(
        mQ32 - pow2(Mu))*pow2(Mu)*(-mU32 + pow2(Mu))*(mU32*pow4(mQ3) + (2*mQ32
        - mU32)*pow4(Mu) + pow2(Mu)*(-(mQ32*mU32) - 2*pow4(mQ3) + pow4(mU3))) +
        2*mQ32*mU32*log(mQ3)*(2*mU32*(2*mQ32 + 3*mU32)*pow2(Mu)*pow4(mQ3) - 2*
        pow4(mU3)*pow6(mQ3) + 2*(5*mQ32*mU32 + 2*pow4(mQ3) + 3*pow4(mU3))*pow6(
        Mu) - pow4(Mu)*(13*mU32*pow4(mQ3) + 4*mQ32*pow4(mU3) + pow6(mQ3) + 2*
        pow6(mU3)) - (3*mQ32 + 7*mU32)*pow8(Mu) + 2*power10(Mu)) - 2*mQ32*mU32*
        log(mU3)*(2*mU32*(2*mQ32 + 3*mU32)*pow2(Mu)*pow4(mQ3) - 2*pow4(mU3)*
        pow6(mQ3) + 2*(5*mQ32*mU32 + 2*pow4(mQ3) + 3*pow4(mU3))*pow6(Mu) -
        pow4(Mu)*(13*mU32*pow4(mQ3) + 4*mQ32*pow4(mU3) + pow6(mQ3) + 2*pow6(
        mU3)) - (3*mQ32 + 7*mU32)*pow8(Mu) + 2*power10(Mu)))))/(mQ32*mU32*pow2(
        mQ32 - mU32)*pow2(mQ32 - pow2(Mu))*pow2(-mU32 + pow2(Mu)))))/pow2(
        sbeta);
}

double himalaya::ThresholdCalculator::getDeltaLambdaYtau6(int omitLogs){
   using std::log;
   using std::sqrt;
   using gm2calc::dilog;
   
   const double MR2 = pow2(p.scale);
   const double lMR = omitLogs*log(MR2);
   const double mL32 = p.ml2(2,2)*(1+0.02);
   const double mE32 = p.me2(2,2)*(1-0.02);
   const double mL3 = sqrt(mL32);
   const double mE3 = sqrt(mE32);
   const double mA = p.MA;
   const double beta = atan(p.vu/p.vd);
   const double sbeta = sin(beta);
   const double cbeta = cos(beta);
   const double Xtau = p.Ae(2,2) - p.mu*p.vu/p.vd;
   const double Ytau = p.Ae(2,2) + p.mu*p.vd/p.vu;
   const double Mu = p.mu;
   
   return 2*(5 + lMR*(10 - 8*log(mE3)) - 8*log(mE3) - 12*(1 + lMR)*log(mL3) + 5*
        pow2(lMR) + 8*pow2(log(mE3)) + 12*pow2(log(mL3)) - (pow2(Xtau)*(-36*
        pow2(mE3)*(pow2(mE3) - pow2(mL3))*pow2(mL3)*pow2(log(mE3)) - 4*pow2(
        mL3)*pow4(mE3) - 2*lMR*pow2(mL3)*pow4(mE3) - 38*log(mL3)*pow2(mL3)*
        pow4(mE3) - 36*lMR*log(mL3)*pow2(mL3)*pow4(mE3) + 36*pow2(mL3)*pow2(
        log(mL3))*pow4(mE3) + pow2(mE3)*pow4(mL3) - lMR*pow2(mE3)*pow4(mL3) +
        34*log(mL3)*pow2(mE3)*pow4(mL3) + 32*lMR*log(mL3)*pow2(mE3)*pow4(mL3) -
        28*pow2(mE3)*pow2(log(mL3))*pow4(mL3) - 2*log(mE3)*pow2(mE3)*(-3*(7 +
        6*lMR)*pow2(mE3)*pow2(mL3) + pow4(mE3) + 16*(1 + lMR)*pow4(mL3) + 4*
        log(mL3)*pow4(mL3)) + dilog(1 - pow2(mL3)/pow2(mE3))*(-6*pow2(mL3)*
        pow4(mE3) + 6*pow2(mE3)*pow4(mL3)) + pow6(mE3) + lMR*pow6(mE3) + 2*
        pow6(mL3) + 2*lMR*pow6(mL3) - 4*log(mL3)*pow6(mL3)))/(pow2(mE3)*pow2(
        mL3)*pow2(pow2(mE3) - pow2(mL3))) + (2*pow4(Xtau)*(-28*pow2(mL3)*pow4(
        mE3) - 16*lMR*pow2(mL3)*pow4(mE3) - 12*log(mL3)*pow2(mL3)*pow4(mE3) -
        14*lMR*log(mL3)*pow2(mL3)*pow4(mE3) + 18*pow2(mL3)*pow2(log(mL3))*pow4(
        mE3) + 29*pow2(mE3)*pow4(mL3) + 17*lMR*pow2(mE3)*pow4(mL3) - 40*log(
        mL3)*pow2(mE3)*pow4(mL3) - 10*lMR*log(mL3)*pow2(mE3)*pow4(mL3) + 14*
        pow2(mE3)*pow2(log(mL3))*pow4(mL3) - 2*log(mE3)*pow2(mE3)*(-((22 + 7*
        lMR)*pow2(mE3)*pow2(mL3)) + 4*log(mL3)*pow2(mL3)*(pow2(mE3) + pow2(mL3)
        ) + pow4(mE3) - (3 + 5*lMR)*pow4(mL3)) + dilog(1 - pow2(mL3)/pow2(mE3))
        *(-3*pow2(mL3)*pow4(mE3) + 3*pow2(mE3)*pow4(mL3)) - 2*pow2(log(mE3))*(
        5*pow2(mL3)*pow4(mE3) + 3*pow2(mE3)*pow4(mL3)) + pow6(mE3) + lMR*pow6(
        mE3) - 2*pow6(mL3) - 2*lMR*pow6(mL3) + 4*log(mL3)*pow6(mL3)))/(pow2(
        mE3)*pow2(mL3)*pow3(pow2(mE3) - pow2(mL3))) - ((4*dilog(1 - pow2(mE3)/
        pow2(mL3))*pow2(mE3)*(pow2(mE3) - pow2(mL3))*pow2(mL3) - 12*pow2(mL3)*
        pow4(mE3) - 6*lMR*pow2(mL3)*pow4(mE3) + 6*log(mE3)*pow2(mL3)*pow4(mE3)
        + 6*log(mL3)*pow2(mL3)*pow4(mE3) - 40*log(mE3)*log(mL3)*pow2(mL3)*pow4(
        mE3) + 20*pow2(mL3)*pow2(log(mE3))*pow4(mE3) + 20*pow2(mL3)*pow2(log(
        mL3))*pow4(mE3) + 9*pow2(mE3)*pow4(mL3) + 3*lMR*pow2(mE3)*pow4(mL3) +
        20*log(mE3)*pow2(mE3)*pow4(mL3) + 12*lMR*log(mE3)*pow2(mE3)*pow4(mL3) -
        26*log(mL3)*pow2(mE3)*pow4(mL3) - 12*lMR*log(mL3)*pow2(mE3)*pow4(mL3) -
        48*log(mE3)*log(mL3)*pow2(mE3)*pow4(mL3) + 12*pow2(mE3)*pow2(log(mE3))*
        pow4(mL3) + 36*pow2(mE3)*pow2(log(mL3))*pow4(mL3) + dilog(1 - pow2(mL3)
        /pow2(mE3))*(-2*pow2(mL3)*pow4(mE3) + 2*pow2(mE3)*pow4(mL3)) + pow6(
        mE3) + lMR*pow6(mE3) - 2*log(mE3)*pow6(mE3) + 2*pow6(mL3) + 2*lMR*pow6(
        mL3) - 4*log(mL3)*pow6(mL3))*pow6(Xtau))/(pow2(mE3)*pow2(mL3)*pow4(
        pow2(mE3) - pow2(mL3))) + (pow2(sbeta)*(1.5 - 5*log(mE3) - 3*(3 + 2*
        lMR)*log(mL3) + 2*pow2(lMR) - (2*pow2(mA))/pow2(mE3) + lMR*(-2*log(mE3)
        + pow2(mA)*(-2/pow2(mE3) - 1/pow2(mL3))) + 2*log(mA)*(7 - 6*log(mE3) -
        6*log(mL3) + pow2(mA)*(2/pow2(mE3) + 1/pow2(mL3))) - pow2(mA)/pow2(mL3)
        + pow2(Pi) + 12*pow2(log(mA)) + 8*pow2(log(mE3)) + 12*pow2(log(mL3)) +
        (phixyz(pow2(mA),pow2(mE3),pow2(mE3))*(-deltaxyz(pow2(mA),pow2(mE3),
        pow2(mE3)) - 6*pow2(mA)*pow2(mE3) + pow4(mA)))/pow4(mE3) + (phixyz(
        pow2(mA),pow2(mL3),pow2(mL3))*(-deltaxyz(pow2(mA),pow2(mL3),pow2(mL3))
        - 7*pow2(mA)*pow2(mL3) + pow4(mA)))/pow4(mL3) + (2*Xtau*Ytau*(phixyz(
        pow2(mA),pow2(mE3),pow2(mE3))*(-deltaxyz(pow2(mA),pow2(mE3),pow2(mE3))
        - 6*pow2(mA)*pow2(mE3) + pow4(mA))*pow4(mL3) + pow4(mE3)*(-(phixyz(
        pow2(mA),pow2(mE3),pow2(mL3))*pow2(mL3)*(pow2(mA) - pow2(mE3) + pow2(
        mL3))) + phixyz(pow2(mA),pow2(mL3),pow2(mL3))*(deltaxyz(pow2(mA),pow2(
        mL3),pow2(mL3)) + 7*pow2(mA)*pow2(mL3) - pow4(mA)) + 4*log(mE3/mL3)*(-3
        - 3*lMR + log(mA) + 2*log(mE3) + 3*log(mL3))*pow4(mL3))))/((pow2(mE3) -
        pow2(mL3))*pow4(mE3)*pow4(mL3)) + (2*Ytau*pow3(Xtau)*(phixyz(pow2(mA),
        pow2(mE3),pow2(mE3))*(pow2(mA)*(pow2(mA) - 6*pow2(mE3))*(pow2(mE3) -
        pow2(mL3)) + deltaxyz(pow2(mA),pow2(mE3),pow2(mE3))*(-3*pow2(mE3) +
        pow2(mL3)))*pow4(mL3) + pow4(mE3)*(phixyz(pow2(mA),pow2(mL3),pow2(mL3))
        *(-(deltaxyz(pow2(mA),pow2(mL3),pow2(mL3))*(pow2(mE3) - 5*pow2(mL3))) +
        pow2(mA)*(pow2(mA) - 7*pow2(mL3))*(pow2(mE3) - pow2(mL3))) + phixyz(
        pow2(mA),pow2(mE3),pow2(mL3))*pow2(mL3)*(-2*deltaxyz(pow2(mA),pow2(mE3)
        ,pow2(mL3)) - (pow2(mE3) - pow2(mL3))*(pow2(mA) - pow2(mE3) + pow2(mL3)
        )) + 4*(-3*(2 + lMR)*(pow2(mE3) - pow2(mL3)) + log(mE3)*(3*(3 + lMR)*
        pow2(mE3) + log(mL3)*(2*pow2(mA) - pow2(mE3) - 3*pow2(mL3)) + 3*(1 +
        lMR)*pow2(mL3) + log(mA)*(-6*pow2(mA) - pow2(mE3) + pow2(mL3))) - log(
        mL3)*(3*(1 + lMR)*pow2(mE3) + 3*(3 + lMR)*pow2(mL3) + log(mA)*(-6*pow2(
        mA) - pow2(mE3) + pow2(mL3))) + 2*(pow2(mA) - pow2(mE3) - pow2(mL3))*
        pow2(log(mE3)) + (-4*pow2(mA) + 3*pow2(mE3) + 5*pow2(mL3))*pow2(log(
        mL3)))*pow4(mL3))))/(pow3(pow2(mE3) - pow2(mL3))*pow4(mE3)*pow4(mL3)) +
        (pow2(Ytau)*(-4/pow2(mE3) + lMR*(-4/pow2(mE3) - 2/pow2(mL3)) - 2/pow2(
        mL3) + (phixyz(pow2(mA),pow2(mE3),pow2(mL3))*(deltaxyz(pow2(mA),pow2(
        mE3),pow2(mL3)) - pow2(pow2(mA) - pow2(mE3) + pow2(mL3))))/(deltaxyz(
        pow2(mA),pow2(mE3),pow2(mL3))*pow2(mL3)) + (2*log(mL3)*(-pow2(mA) + 2*
        pow2(mE3) + pow2(mL3) - (pow2(mE3)*(-2*pow2(mA)*pow2(mE3) + pow4(mA) +
        pow4(mE3) - pow4(mL3)))/deltaxyz(pow2(mA),pow2(mE3),pow2(mL3)) + (pow3(
        pow2(mE3) - pow2(mL3)) - 3*(pow2(mE3) + pow2(mL3))*pow4(mA) - 3*pow2(
        mA)*(pow4(mE3) - pow4(mL3)) + pow6(mA))/deltaxyz(pow2(mA),pow2(mL3),
        pow2(mE3))))/pow4(mE3) + (2*log(mE3)*(-pow2(mA) + 3*pow2(mE3) - pow2(
        mL3) - (2*pow2(mE3)*pow2(mL3)*(pow2(mA) - pow2(mE3) + pow2(mL3)))/
        deltaxyz(pow2(mA),pow2(mE3),pow2(mL3)) + (pow3(-pow2(mE3) + pow2(mL3))
        - (5*pow2(mE3) + pow2(mL3))*pow4(mA) - pow2(mA)*(4*pow2(mE3)*pow2(mL3)
        - 5*pow4(mE3) + pow4(mL3)) + pow6(mA))/deltaxyz(pow2(mA),pow2(mL3),
        pow2(mE3))))/(pow2(mE3)*pow2(mL3)) + (2*log(mA)*(3*pow2(mE3)*pow2(mL3)
        + pow2(mA)*(pow2(mE3) + pow2(mL3)) - (pow2(mE3)*pow2(mL3)*(pow2(pow2(
        mE3) - pow2(mL3)) - pow4(mA)))/deltaxyz(pow2(mA),pow2(mE3),pow2(mL3)) -
        pow4(mE3) - pow4(mL3) + (-(pow2(mA)*(5*pow2(mE3) + 3*pow2(mL3))*pow2(
        pow2(mE3) - pow2(mL3))) + pow4(mA)*(4*pow2(mE3)*pow2(mL3) + 5*pow4(mE3)
        + 3*pow4(mL3)) + pow4(pow2(mE3) - pow2(mL3)) - (pow2(mE3) + pow2(mL3))*
        pow6(mA))/deltaxyz(pow2(mA),pow2(mL3),pow2(mE3))))/(pow2(mL3)*pow4(mE3)
        ) + (phixyz(pow2(mA),pow2(mL3),pow2(mE3))*(2*pow2(deltaxyz(pow2(mA),
        pow2(mL3),pow2(mE3))) - 4*pow2(mA)*(pow2(mE3) + pow2(mL3))*pow2(pow2(
        mE3) - pow2(mL3)) - 3*deltaxyz(pow2(mA),pow2(mL3),pow2(mE3))*(-2*pow2(
        mA)*(pow2(mE3) + pow2(mL3)) + pow2(pow2(mE3) - pow2(mL3)) + pow4(mA)) +
        2*pow4(mA)*(2*pow2(mE3)*pow2(mL3) + pow4(mE3) + 3*pow4(mL3)) + pow4(
        pow2(mE3) - pow2(mL3)) - 4*(pow2(mE3) + pow2(mL3))*pow6(mA) + pow8(mA))
        )/(deltaxyz(pow2(mA),pow2(mL3),pow2(mE3))*pow6(mE3))))/2. + pow4(Xtau)*
        ((-3*pow2(mE3) + 3*pow2(mL3) + pow2(mA)*(-5 + pow2(mE3)/pow2(mL3) - (2*
        pow2(mL3))/pow2(mE3)))/pow3(-pow2(mE3) + pow2(mL3)) - (3*log(mE3)*(4*
        pow2(mA)*pow2(mL3) + pow4(mE3) - pow4(mL3)))/pow4(pow2(mE3) - pow2(mL3)
        ) + (3*log(mL3)*(4*(1 + lMR)*pow2(mA)*pow2(mL3) + (1 + 2*lMR)*pow4(mE3)
        - (1 + 2*lMR)*pow4(mL3)))/pow4(pow2(mE3) - pow2(mL3)) + (2*log(mA)*(6*
        log(mE3)*pow2(mE3)*pow2(mL3)*(2*pow2(mA)*pow2(mL3) + pow4(mE3) - pow4(
        mL3)) - 6*log(mL3)*pow2(mE3)*pow2(mL3)*(2*pow2(mA)*pow2(mL3) + pow4(
        mE3) - pow4(mL3)) + (pow2(mE3) - pow2(mL3))*(-6*pow2(mL3)*pow4(mE3) +
        pow2(mA)*(-5*pow2(mE3)*pow2(mL3) + pow4(mE3) - 2*pow4(mL3)) + 6*pow2(
        mE3)*pow4(mL3))))/(pow2(mE3)*pow2(mL3)*pow4(pow2(mE3) - pow2(mL3))) + (
        lMR*(-6*log(mE3)*(2*pow2(mA)*pow2(mL3) + pow4(mE3) - pow4(mL3)) + ((-
        pow2(mE3) + pow2(mL3))*(-6*pow2(mL3)*pow4(mE3) + pow2(mA)*(-5*pow2(mE3)
        *pow2(mL3) + pow4(mE3) - 2*pow4(mL3)) + 6*pow2(mE3)*pow4(mL3)))/(pow2(
        mE3)*pow2(mL3))))/pow4(pow2(mE3) - pow2(mL3)) + pow2(Ytau)*((-11*pow2(
        mE3)*pow2(mL3) + pow4(mE3) - 2*pow4(mL3))/(pow2(mE3)*pow2(mL3)*pow3(-
        pow2(mE3) + pow2(mL3))) - (8*(-2*pow2(mA) + 3*pow2(mE3) + pow2(mL3))*
        pow2(log(mE3)))/pow4(pow2(mE3) - pow2(mL3)) + (4*(7*pow2(mA) - 3*pow2(
        mE3) - 11*pow2(mL3))*pow2(log(mL3)))/pow4(pow2(mE3) - pow2(mL3)) - (
        phixyz(pow2(mA),pow2(mE3),pow2(mL3))*(deltaxyz(pow2(mA),pow2(mE3),pow2(
        mL3))*(-pow2(mE3) + pow2(mL3))*(4*pow2(mA) - 3*pow2(mE3) + 3*pow2(mL3))
        - 6*pow2(deltaxyz(pow2(mA),pow2(mE3),pow2(mL3))) + pow2(pow2(mE3) -
        pow2(mL3))*pow2(pow2(mA) - pow2(mE3) + pow2(mL3))))/(2.*deltaxyz(pow2(
        mA),pow2(mE3),pow2(mL3))*pow2(mL3)*pow4(pow2(mE3) - pow2(mL3))) + (
        phixyz(pow2(mA),pow2(mE3),pow2(mE3))*(pow2(mA)*(pow2(mA) - 6*pow2(mE3))
        *(pow2(mE3) - pow2(mL3)) + deltaxyz(pow2(mA),pow2(mE3),pow2(mE3))*(-5*
        pow2(mE3) + pow2(mL3))))/pow4(-(mE3*pow2(mL3)) + pow3(mE3)) + (phixyz(
        pow2(mA),pow2(mL3),pow2(mL3))*(deltaxyz(pow2(mA),pow2(mL3),pow2(mL3))*(
        pow2(mE3) - 8*pow2(mL3)) + pow2(mA)*(pow2(mA) - 7*pow2(mL3))*(-pow2(
        mE3) + pow2(mL3))))/pow4(-(mL3*pow2(mE3)) + pow3(mL3)) - (lMR*(-6*pow2(
        mL3)*pow4(mE3) + 3*pow2(mE3)*pow4(mL3) + 12*log(mE3)*pow2(mE3)*pow4(
        mL3) + pow6(mE3) + 2*pow6(mL3)))/(pow2(mE3)*pow2(mL3)*pow4(pow2(mE3) -
        pow2(mL3))) + (log(mA)*(12*log(mE3)*(pow2(mA) + pow2(mE3) - pow2(mL3))
        - 12*log(mL3)*(pow2(mA) + pow2(mE3) - pow2(mL3)) - (pow2(pow2(mE3) -
        pow2(mL3))*(pow2(mE3)*pow2(mL3)*(pow2(pow2(mE3) - pow2(mL3)) - pow4(mA)
        ) + deltaxyz(pow2(mA),pow2(mE3),pow2(mL3))*(pow2(mA)*(pow2(mE3) - pow2(
        mL3)) - 3*pow2(mE3)*pow2(mL3) - 3*pow4(mE3) + pow4(mL3))))/(deltaxyz(
        pow2(mA),pow2(mE3),pow2(mL3))*pow2(mL3)*pow4(mE3)) + (pow3(-pow2(mE3) +
        pow2(mL3))*(3*(pow2(mE3) + pow2(mL3))*pow4(mA) - pow2(mL3)*pow4(mE3) -
        3*pow2(mE3)*pow4(mL3) - pow2(mA)*(5*pow4(mE3) + 3*pow4(mL3)) - pow6(mA)
        + 3*pow6(mE3) + pow6(mL3)))/(deltaxyz(pow2(mA),pow2(mL3),pow2(mE3))*
        pow2(mL3)*pow4(mE3))))/pow4(pow2(mE3) - pow2(mL3)) + (log(mL3)*(12*lMR*
        pow2(mL3) + 4*log(mE3)*(-11*pow2(mA) + 9*pow2(mE3) + 13*pow2(mL3)) + (
        pow2(pow2(mE3) - pow2(mL3))*(-((5*pow2(mE3) + 3*pow2(mL3))*pow4(mA)) +
        9*pow2(mL3)*pow4(mE3) + pow2(mE3)*pow4(mL3) + pow2(mA)*(4*pow2(mE3)*
        pow2(mL3) + 9*pow4(mE3) + 3*pow4(mL3)) + pow6(mA) - 9*pow6(mE3) - pow6(
        mL3)))/(deltaxyz(pow2(mA),pow2(mL3),pow2(mE3))*pow4(mE3)) + (-(pow2(mA)
        *pow2(pow2(mE3) - pow2(mL3))) + 17*pow2(mL3)*pow4(mE3) - (pow2(-(mE3*
        pow2(mL3)) + pow3(mE3))*(-2*pow2(mA)*pow2(mE3) + pow4(mA) + pow4(mE3) -
        pow4(mL3)))/deltaxyz(pow2(mA),pow2(mE3),pow2(mL3)) + 2*pow2(mE3)*pow4(
        mL3) + 4*pow6(mE3) + pow6(mL3))/pow4(mE3)))/pow4(pow2(mE3) - pow2(mL3))
        + (log(mE3)*(pow2(mA)*pow2(pow2(mE3) - pow2(mL3)) - (2*pow2(mE3)*pow2(
        mL3)*(pow2(mA) - pow2(mE3) + pow2(mL3))*pow2(pow2(mE3) - pow2(mL3)))/
        deltaxyz(pow2(mA),pow2(mE3),pow2(mL3)) - 13*pow2(mL3)*pow4(mE3) - 7*
        pow2(mE3)*pow4(mL3) - pow6(mE3) - 3*pow6(mL3) + (pow2(pow2(mE3) - pow2(
        mL3))*((3*pow2(mE3) + 5*pow2(mL3))*pow4(mA) + 5*pow2(mL3)*pow4(mE3) -
        11*pow2(mE3)*pow4(mL3) - pow2(mA)*(4*pow2(mE3)*pow2(mL3) + 5*pow4(mE3)
        + 7*pow4(mL3)) - pow6(mA) + 3*pow6(mE3) + 3*pow6(mL3)))/deltaxyz(pow2(
        mA),pow2(mL3),pow2(mE3))))/(pow2(mE3)*pow2(mL3)*pow4(pow2(mE3) - pow2(
        mL3))) + (phixyz(pow2(mA),pow2(mL3),pow2(mE3))*(2*pow2(deltaxyz(pow2(
        mA),pow2(mL3),pow2(mE3)))*(-5*pow2(mE3)*pow2(mL3) + 12*pow4(mE3) +
        pow4(mL3)) - deltaxyz(pow2(mA),pow2(mL3),pow2(mE3))*(-pow2(mE3) + pow2(
        mL3))*(3*(-3*pow2(mE3) + pow2(mL3))*pow4(mA) + 29*pow2(mL3)*pow4(mE3) +
        6*pow2(mA)*(2*pow2(mE3)*pow2(mL3) + 3*pow4(mE3) - pow4(mL3)) - 15*pow2(
        mE3)*pow4(mL3) - 17*pow6(mE3) + 3*pow6(mL3)) + pow2(pow2(mE3) - pow2(
        mL3))*(-4*pow2(mA)*(pow2(mE3) + pow2(mL3))*pow2(pow2(mE3) - pow2(mL3))
        + pow2(pow2(mE3) - pow2(mL3))*(-2*pow2(mE3)*pow2(mL3) - 3*pow4(mE3) +
        pow4(mL3)) + pow4(mA)*(4*pow2(mE3)*pow2(mL3) + 6*pow4(mE3) + 6*pow4(
        mL3)) - 4*(pow2(mE3) + pow2(mL3))*pow6(mA) + pow8(mA))))/(2.*deltaxyz(
        pow2(mA),pow2(mL3),pow2(mE3))*pow4(pow2(mE3) - pow2(mL3))*pow6(mE3))))
        + pow2(Xtau)*((2*pow2(mA)*(pow2(mE3) - 2*pow2(mL3)) + 4*pow2(mE3)*pow2(
        mL3))/(pow2(mE3)*(pow2(mE3) - pow2(mL3))*pow2(mL3)) + (8*pow2(log(mE3))
        )/(pow2(mE3) - pow2(mL3)) + (2*log(mE3)*(2*pow2(mA) - 5*pow2(mE3) +
        pow2(mL3)))/pow2(pow2(mE3) - pow2(mL3)) + (2*log(mL3)*(-2*(1 + lMR)*
        pow2(mA) + 3*pow2(mE3) + pow2(mL3) + 2*lMR*pow2(mL3) + 2*log(mE3)*(-
        pow2(mA) + pow2(mE3) + pow2(mL3))))/pow2(pow2(mE3) - pow2(mL3)) + (2*
        lMR*(pow2(mE3) + 2*log(mE3)*(pow2(mA) - pow2(mL3)) - pow2(mL3) + pow2(
        mA)*(-3 + pow2(mE3)/pow2(mL3) + (2*pow2(mL3))/pow2(mE3))))/pow2(pow2(
        mE3) - pow2(mL3)) + (4*(pow2(mA) - 3*pow2(mE3) + pow2(mL3))*pow2(log(
        mL3)))/pow2(pow2(mE3) - pow2(mL3)) + (deltaxyz(pow2(mA),pow2(mE3),pow2(
        mL3))*phixyz(pow2(mA),pow2(mE3),pow2(mL3)))/pow2(-(mL3*pow2(mE3)) +
        pow3(mL3)) + (phixyz(pow2(mA),pow2(mL3),pow2(mL3))*(deltaxyz(pow2(mA),
        pow2(mL3),pow2(mL3))*(pow2(mE3) - 2*pow2(mL3)) + pow2(mA)*(pow2(mA) -
        7*pow2(mL3))*(-pow2(mE3) + pow2(mL3))))/(pow2(pow2(mE3) - pow2(mL3))*
        pow4(mL3)) - (4*log(mA)*(log(mE3)*pow2(mE3)*(pow2(mA) + 5*pow2(mE3) -
        5*pow2(mL3))*pow2(mL3) - log(mL3)*pow2(mE3)*(pow2(mA) + 5*pow2(mE3) -
        5*pow2(mL3))*pow2(mL3) + pow2(mA)*(-3*pow2(mE3)*pow2(mL3) + pow4(mE3) +
        2*pow4(mL3))))/(pow2(mE3)*pow2(mL3)*pow2(pow2(mE3) - pow2(mL3))) + (
        phixyz(pow2(mA),pow2(mE3),pow2(mE3))*(-deltaxyz(pow2(mA),pow2(mE3),
        pow2(mE3)) - 6*pow2(mA)*pow2(mE3) + pow4(mA)))/(-(pow2(mL3)*pow4(mE3))
        + pow6(mE3)) + pow2(Ytau)*((8*pow2(log(mE3)))/pow2(pow2(mE3) - pow2(
        mL3)) + (12*pow2(log(mL3)))/pow2(pow2(mE3) - pow2(mL3)) + (phixyz(pow2(
        mA),pow2(mE3),pow2(mL3))*(deltaxyz(pow2(mA),pow2(mE3),pow2(mL3))*pow2(
        mA) - (pow2(mE3) - pow2(mL3))*pow2(pow2(mA) - pow2(mE3) + pow2(mL3))))/
        (deltaxyz(pow2(mA),pow2(mE3),pow2(mL3))*pow2(-(mL3*pow2(mE3)) + pow3(
        mL3))) + (phixyz(pow2(mA),pow2(mE3),pow2(mE3))*(-deltaxyz(pow2(mA),
        pow2(mE3),pow2(mE3)) - 6*pow2(mA)*pow2(mE3) + pow4(mA)))/(pow2(pow2(
        mE3) - pow2(mL3))*pow4(mE3)) + (phixyz(pow2(mA),pow2(mL3),pow2(mL3))*(-
        deltaxyz(pow2(mA),pow2(mL3),pow2(mL3)) - 7*pow2(mA)*pow2(mL3) + pow4(
        mA)))/(pow2(pow2(mE3) - pow2(mL3))*pow4(mL3)) + (2*lMR*(-3*pow2(mE3)*
        pow2(mL3) + 2*log(mE3)*pow2(mE3)*pow2(mL3) + pow4(mE3) + 2*pow4(mL3)))/
        (pow2(mE3)*pow2(mL3)*pow2(pow2(mE3) - pow2(mL3))) + (2*pow2(mE3) - 4*
        pow2(mL3))/(pow2(mL3)*pow4(mE3) - pow2(mE3)*pow4(mL3)) + (2*log(mE3)*(
        pow2(mE3)*pow2(mL3) + (2*pow2(mE3)*pow2(mL3)*(-pow2(mE3) + pow2(mL3))*(
        pow2(mA) - pow2(mE3) + pow2(mL3)))/deltaxyz(pow2(mA),pow2(mE3),pow2(
        mL3)) - pow4(mE3) + 2*pow4(mL3) + ((pow2(mE3) - pow2(mL3))*(-((pow2(
        mE3) - 2*pow2(mL3))*pow2(pow2(mE3) - pow2(mL3))) + (pow2(mE3) + 2*pow2(
        mL3))*pow4(mA) - 4*pow2(mA)*(2*pow2(mE3)*pow2(mL3) + pow4(mL3))))/
        deltaxyz(pow2(mA),pow2(mL3),pow2(mE3))))/(pow2(mE3)*pow2(mL3)*pow2(
        pow2(mE3) - pow2(mL3))) + (2*log(mL3)*(-2*lMR - 10*log(mE3) + (-2*pow2(
        mE3)*pow2(mL3) + pow2(mA)*(-pow2(mE3) + pow2(mL3)) + pow4(mE3) - (pow2(
        mE3)*(pow2(mE3) - pow2(mL3))*(-2*pow2(mA)*pow2(mE3) + pow4(mA) + pow4(
        mE3) - pow4(mL3)))/deltaxyz(pow2(mA),pow2(mE3),pow2(mL3)) - pow4(mL3))/
        pow4(mE3) - ((pow2(mE3) - pow2(mL3))*(pow2(-(mL3*pow2(mE3)) + pow3(mL3)
        ) + (4*pow2(mE3) + 3*pow2(mL3))*pow4(mA) - pow2(mA)*(2*pow2(mE3)*pow2(
        mL3) + 7*pow4(mE3) + 3*pow4(mL3)) - pow6(mA)))/(deltaxyz(pow2(mA),pow2(
        mL3),pow2(mE3))*pow4(mE3))))/pow2(pow2(mE3) - pow2(mL3)) + (2*log(mA)*(
        log(pow2(mL3)/pow2(mE3)) + ((-pow2(mE3) + pow2(mL3))*(deltaxyz(pow2(mA)
        ,pow2(mL3),pow2(mE3))*(pow2(mE3)*pow2(mL3)*(pow2(pow2(mE3) - pow2(mL3))
        - pow4(mA)) + deltaxyz(pow2(mA),pow2(mE3),pow2(mL3))*(-(pow2(mA)*pow2(
        mL3)) - 3*pow2(mE3)*pow2(mL3) + pow4(mE3) + pow4(mL3))) + deltaxyz(
        pow2(mA),pow2(mE3),pow2(mL3))*(pow4(mA)*(-2*pow2(mE3)*pow2(mL3) + pow4(
        mE3) - 3*pow4(mL3)) - pow2(mA)*pow2(mL3)*(2*pow2(mE3)*pow2(mL3) + pow4(
        mE3) - 3*pow4(mL3)) - pow4(pow2(mE3) - pow2(mL3)) + pow2(mL3)*pow6(mA))
        ))/(deltaxyz(pow2(mA),pow2(mE3),pow2(mL3))*deltaxyz(pow2(mA),pow2(mL3),
        pow2(mE3))*pow2(mL3)*pow4(mE3))))/pow2(pow2(mE3) - pow2(mL3)) + (
        phixyz(pow2(mA),pow2(mL3),pow2(mE3))*(-2*(-2*pow2(mE3) + pow2(mL3))*
        pow2(deltaxyz(pow2(mA),pow2(mL3),pow2(mE3))) + deltaxyz(pow2(mA),pow2(
        mL3),pow2(mE3))*((-5*pow2(mE3) + 3*pow2(mL3))*pow2(pow2(mE3) - pow2(
        mL3)) + (-5*pow2(mE3) + 3*pow2(mL3))*pow4(mA) + 2*pow2(mA)*(2*pow2(mE3)
        *pow2(mL3) + 7*pow4(mE3) - 3*pow4(mL3))) + (pow2(mE3) - pow2(mL3))*(4*
        pow2(mA)*(pow2(mE3) - pow2(mL3))*pow4(mL3) + pow4(mA)*(4*pow2(mE3)*
        pow2(mL3) + 6*pow4(mE3) + 6*pow4(mL3)) + pow4(pow2(mE3) - pow2(mL3)) -
        4*(pow2(mE3) + pow2(mL3))*pow6(mA) + pow8(mA))))/(deltaxyz(pow2(mA),
        pow2(mL3),pow2(mE3))*pow2(pow2(mE3) - pow2(mL3))*pow6(mE3))))))/pow2(
        cbeta) + (-2*pow2(lMR) + lMR*(2*log(mE3) - ((pow2(mE3) + 2*pow2(mL3))*(
        pow2(mE3) + pow2(mL3) - 2*pow2(Mu)))/(pow2(mE3)*pow2(mL3))) - (4*pow2(
        mE3)*(pow2(mE3) - 2*pow2(Mu))*pow2(log(mE3)))/pow2(pow2(mE3) - pow2(Mu)
        ) - (8*pow2(mL3)*(pow2(mL3) - 2*pow2(Mu))*pow2(log(mL3)))/pow2(pow2(
        mL3) - pow2(Mu)) + (4*dilog(1 - pow2(mL3)/pow2(Mu))*pow4(Mu))/pow2(
        pow2(mL3) - pow2(Mu)) + (2*dilog(1 - pow2(mE3)/pow2(Mu))*(2*pow2(mE3)*
        pow2(Mu) - pow4(mE3) + pow4(Mu)))/pow2(pow2(mE3) - pow2(Mu)) + (2*
        dilog(1 - pow2(mL3)/pow2(Mu))*(2*pow2(mL3)*pow2(Mu) - pow4(mL3) + pow4(
        Mu)))/pow2(pow2(mL3) - pow2(Mu)) + (-(pow2(mE3)*pow2(mL3)*(3*pow2(mE3)*
        pow2(mL3) + 2*pow4(mE3) + 4*pow4(mL3))) - 3*(7*pow2(mE3)*pow2(mL3) + 2*
        pow4(mE3) + 4*pow4(mL3))*pow4(Mu) + pow2(Mu)*(13*pow2(mL3)*pow4(mE3) +
        17*pow2(mE3)*pow4(mL3) + 2*pow6(mE3) + 4*pow6(mL3)) + 4*(pow2(mE3) + 2*
        pow2(mL3))*pow6(Mu))/(2.*pow2(mE3)*pow2(mL3)*(-pow2(mE3) + pow2(Mu))*(-
        pow2(mL3) + pow2(Mu))) + (log(mE3)*(9*pow4(mL3)*pow4(Mu) + pow4(mE3)*(-
        9*pow2(mL3)*pow2(Mu) + pow4(mL3) + 4*pow4(Mu)) + 2*(pow2(mL3) - pow2(
        Mu))*pow6(mE3) - 13*pow2(mL3)*pow6(Mu) - 2*pow2(mE3)*(2*pow2(Mu)*pow4(
        mL3) - 7*pow2(mL3)*pow4(Mu) + pow6(Mu))))/(pow2(mL3)*(pow2(mL3) - pow2(
        Mu))*pow2(pow2(mE3) - pow2(Mu))) + (log(pow2(Mu))*(2*log(mE3)*pow2(mE3)
        *pow2(mL3)*(2*pow4(mE3)*(-2*pow2(mL3)*pow2(Mu) + pow4(mL3)) - pow4(Mu)*
        (-2*pow2(mL3)*pow2(Mu) + pow4(mL3) + 3*pow4(Mu)) + pow2(mE3)*(-4*pow2(
        Mu)*pow4(mL3) + 8*pow2(mL3)*pow4(Mu))) + pow2(Mu)*(-4*pow2(-(mL3*pow2(
        Mu)) + pow3(mL3))*pow4(Mu) - 2*(pow2(mL3)*pow2(Mu) + pow4(mL3) + pow4(
        Mu))*pow6(mE3) + pow4(mE3)*(8*pow2(Mu)*pow4(mL3) + 2*pow2(mL3)*pow4(Mu)
        - 2*pow6(mL3) + 4*pow6(Mu)) + pow2(mE3)*(-8*pow4(mL3)*pow4(Mu) + 3*
        pow2(Mu)*pow6(mL3) + pow2(mL3)*pow6(Mu) - 2*pow8(Mu)))))/(pow2(mE3)*
        pow2(mL3)*pow2(pow2(mE3) - pow2(Mu))*pow2(pow2(mL3) - pow2(Mu))) + (2*
        pow2(log(pow2(Mu)))*(-2*pow2(mE3)*pow2(Mu)*(2*pow2(mL3)*pow2(Mu) -
        pow4(mL3) + pow4(Mu)) + pow4(mE3)*(2*pow2(mL3)*pow2(Mu) - pow4(mL3) +
        pow4(Mu)) + 2*pow8(Mu)))/(pow2(pow2(mE3) - pow2(Mu))*pow2(pow2(mL3) -
        pow2(Mu))) + (log(mL3)*(2*log(pow2(Mu))*pow2(mE3)*(2*pow4(mE3)*(-2*
        pow2(mL3)*pow2(Mu) + pow4(mL3) - 2*pow4(Mu)) + (-2*pow2(mL3)*pow2(Mu) +
        pow4(mL3) - 5*pow4(Mu))*pow4(Mu) + pow2(mE3)*(-4*pow2(Mu)*pow4(mL3) +
        8*pow2(mL3)*pow4(Mu) + 8*pow6(Mu))) + (pow2(mE3) - pow2(Mu))*(-4*pow2(
        mL3)*pow2(Mu)*pow2(pow2(mL3) - pow2(Mu)) + pow4(mE3)*(-6*(1 + 2*lMR)*
        pow2(mL3)*pow2(Mu) + (5 + 6*lMR)*pow4(mL3) + (13 + 6*lMR)*pow4(Mu)) +
        pow2(mE3)*(-3*(5 + 2*lMR)*pow2(Mu)*pow4(mL3) + 2*(7 + 6*lMR)*pow2(mL3)*
        pow4(Mu) + 4*pow6(mL3) - 3*(5 + 2*lMR)*pow6(Mu))) + log(mE3)*(-8*pow2(
        Mu)*pow4(mE3)*(2*pow2(mL3)*pow2(Mu) - pow4(mL3) + pow4(Mu)) + (8*pow2(
        mL3)*pow2(Mu) - 4*pow4(mL3) + 4*pow4(Mu))*pow6(mE3) + 8*pow2(mE3)*pow8(
        Mu))))/(pow2(mE3)*pow2(pow2(mE3) - pow2(Mu))*pow2(pow2(mL3) - pow2(Mu))
        ) + pow2(Xtau)*((4*dilog(1 - pow2(mE3)/pow2(Mu))*(pow2(mL3) - pow2(Mu))
        )/pow2(pow2(mE3) - pow2(mL3)) - (4*dilog(1 - pow2(mL3)/pow2(Mu))*(pow2(
        mL3) - pow2(Mu)))/pow2(pow2(mE3) - pow2(mL3)) + (2*lMR*(-2*log(mE3)*(
        pow2(mE3) - 4*pow2(mL3) + 2*pow2(Mu)) + ((pow2(mE3) - pow2(mL3))*(4*
        pow2(mL3)*pow2(Mu) - 2*pow2(mE3)*(pow2(mL3) + pow2(Mu)) + pow4(mE3) -
        2*pow4(mL3)))/(pow2(mE3)*pow2(mL3))))/pow2(pow2(mE3) - pow2(mL3)) + (4*
        pow2(mL3)*(pow2(mL3) - 2*pow2(Mu)) + pow2(mE3)*(6*pow2(mL3) + 4*pow2(
        Mu)) - 2*pow4(mE3))/(-(pow2(mL3)*pow4(mE3)) + pow2(mE3)*pow4(mL3)) + (
        8*pow2(mE3)*pow2(log(mE3))*(pow2(mE3)*(pow2(mL3) - 6*pow2(Mu)) - 2*
        pow2(mL3)*pow2(Mu) + 3*pow4(mE3) + 4*pow4(Mu)))/(pow2(pow2(mE3) - pow2(
        mL3))*pow2(pow2(mE3) - pow2(Mu))) + (8*pow2(mL3)*pow2(log(mL3))*(2*
        pow2(mE3)*(pow2(mL3) - 2*pow2(Mu)) - 6*pow2(mL3)*pow2(Mu) + 3*pow4(mL3)
        + 5*pow4(Mu)))/(pow2(pow2(mE3) - pow2(mL3))*pow2(pow2(mL3) - pow2(Mu)))
        - (2*log(mE3)*(-5*pow4(mL3)*pow4(Mu) + pow4(mE3)*(pow2(mL3)*pow2(Mu) +
        pow4(mL3) + 2*pow4(Mu)) + 2*(pow2(mL3) - pow2(Mu))*pow6(mE3) + pow2(
        mE3)*(16*pow2(Mu)*pow4(mL3) - 13*pow2(mL3)*pow4(Mu) - 11*pow6(mL3)) +
        5*pow2(Mu)*pow6(mL3) + 4*pow2(mL3)*pow6(Mu)))/((pow2(mE3) - pow2(Mu))*(
        pow2(mL3) - pow2(Mu))*pow2(-(mL3*pow2(mE3)) + pow3(mL3))) + (2*log(mL3)
        *((pow2(mE3) - pow2(Mu))*(pow2(mL3) - pow2(Mu))*(4*(pow2(mL3) - pow2(
        Mu))*pow2(Mu)*pow4(mL3) - pow4(mE3)*(-2*(1 + 5*lMR)*pow2(mL3)*pow2(Mu)
        + (11 + 8*lMR)*pow4(mL3) + (7 + 2*lMR)*pow4(Mu)) + ((7 + 2*lMR)*pow2(
        mL3) + (1 - 2*lMR)*pow2(Mu))*pow6(mE3) + pow2(mE3)*((13 + 8*lMR)*pow2(
        Mu)*pow4(mL3) - (5 + 12*lMR)*pow2(mL3)*pow4(Mu) - 4*pow6(mL3) + 4*(1 +
        lMR)*pow6(Mu))) - 4*log(mE3)*pow2(mE3)*((-10*pow2(mL3)*pow2(Mu) + 5*
        pow4(mL3) + 3*pow4(Mu))*pow6(mE3) + 3*pow4(Mu)*pow6(mL3) + 2*pow4(mE3)*
        (-9*pow2(Mu)*pow4(mL3) + 13*pow2(mL3)*pow4(Mu) + 2*pow6(mL3) - 3*pow6(
        Mu)) - 6*pow4(mL3)*pow6(Mu) + 5*pow2(mL3)*pow8(Mu) + pow2(mE3)*(22*
        pow4(mL3)*pow4(Mu) - 8*pow2(Mu)*pow6(mL3) - 24*pow2(mL3)*pow6(Mu) + 4*
        pow8(Mu))) + 2*log(pow2(Mu))*pow2(mE3)*(2*pow4(Mu)*pow6(mE3) + 2*pow4(
        mE3)*(-3*pow2(Mu)*pow4(mL3) + 2*pow2(mL3)*pow4(Mu) + pow6(mL3) - 3*
        pow6(Mu)) + pow4(Mu)*(-4*pow2(Mu)*pow4(mL3) + 3*pow2(mL3)*pow4(Mu) +
        pow6(mL3) - 2*pow6(Mu)) + pow2(mE3)*(13*pow4(mL3)*pow4(Mu) - 4*pow2(Mu)
        *pow6(mL3) - 10*pow2(mL3)*pow6(Mu) + 7*pow8(Mu)))))/(pow2(mE3)*pow2(
        pow2(mE3) - pow2(mL3))*pow2(pow2(mE3) - pow2(Mu))*pow2(pow2(mL3) -
        pow2(Mu))) - (4*log(pow2(Mu))*((pow2(mE3) - pow2(mL3))*(pow2(mE3) -
        pow2(Mu))*(pow2(mL3) - pow2(Mu))*pow2(Mu)*(pow2(Mu)*pow4(mE3) - 2*pow2(
        Mu)*pow4(mL3) + pow2(mE3)*(-(pow2(mL3)*pow2(Mu)) + pow4(mL3) - pow4(Mu)
        ) + 2*pow2(mL3)*pow4(Mu)) + log(mE3)*pow2(mE3)*pow2(mL3)*(2*pow4(Mu)*
        pow6(mE3) + 2*pow4(mE3)*(-3*pow2(Mu)*pow4(mL3) + 2*pow2(mL3)*pow4(Mu) +
        pow6(mL3) - 3*pow6(Mu)) + pow4(Mu)*(-4*pow2(Mu)*pow4(mL3) + 3*pow2(mL3)
        *pow4(Mu) + pow6(mL3) - 2*pow6(Mu)) + pow2(mE3)*(13*pow4(mL3)*pow4(Mu)
        - 4*pow2(Mu)*pow6(mL3) - 10*pow2(mL3)*pow6(Mu) + 7*pow8(Mu)))))/(pow2(
        mE3)*pow2(mL3)*pow2(pow2(mE3) - pow2(mL3))*pow2(pow2(mE3) - pow2(Mu))*
        pow2(pow2(mL3) - pow2(Mu)))) + (pow4(Xtau)*(2*dilog(1 - pow2(mE3)/pow2(
        Mu))*(-2*pow2(mE3)*pow2(mL3) + 6*pow2(mL3)*pow2(Mu) + pow4(mE3) - 2*
        pow4(mL3) - 3*pow4(Mu)) + dilog(1 - pow2(mL3)/pow2(Mu))*(4*pow2(mL3)*(
        pow2(mE3) - 3*pow2(Mu)) - 2*pow4(mE3) + 4*pow4(mL3) + 6*pow4(Mu)) - (4*
        pow2(mE3)*(pow2(mE3) + pow2(mL3))*pow2(log(mE3))*(pow2(mE3)*(pow2(mL3)
        - 14*pow2(Mu)) - 2*pow2(mL3)*pow2(Mu) + 7*pow4(mE3) + 8*pow4(Mu)))/
        pow2(pow2(mE3) - pow2(Mu)) + lMR*(2*log(mE3)*(-6*pow2(mE3)*pow2(mL3) +
        12*pow2(mL3)*pow2(Mu) + 7*pow4(mE3) - 13*pow4(mL3)) + ((-pow2(mE3) +
        pow2(mL3))*(10*pow2(mE3)*pow2(mL3)*(pow2(mE3) + pow2(Mu)) - 2*pow2(Mu)*
        pow4(mE3) + (-21*pow2(mE3) + 4*pow2(Mu))*pow4(mL3) + pow6(mE3) - 2*
        pow6(mL3)))/(pow2(mE3)*pow2(mL3))) - (8*pow2(mL3)*pow2(log(mL3))*(5*
        pow2(mE3)*pow2(pow2(mL3) - pow2(Mu)) + (pow2(mL3) - 2*pow2(Mu))*pow4(
        mE3) - 10*pow2(Mu)*pow4(mL3) + 6*pow2(mL3)*pow4(Mu) + 5*pow6(mL3)))/
        pow2(pow2(mL3) - pow2(Mu)) + ((pow2(mE3) - pow2(mL3))*(-2*pow2(Mu)*
        pow4(mL3)*(-3*pow2(mL3)*pow2(Mu) + pow4(mL3) + 2*pow4(Mu)) + (14*pow2(
        mL3)*pow2(Mu) - 15*pow4(mL3) - 3*pow4(Mu))*pow6(mE3) + pow4(mE3)*(-31*
        pow2(Mu)*pow4(mL3) + 5*pow2(mL3)*pow4(Mu) + 32*pow6(mL3) + 2*pow6(Mu))
        + (-pow2(mL3) + pow2(Mu))*pow8(mE3) + 2*pow2(mE3)*(23*pow4(mL3)*pow4(
        Mu) - 18*pow2(Mu)*pow6(mL3) - 8*pow2(mL3)*pow6(Mu) + pow8(mL3))))/(
        pow2(mE3)*pow2(mL3)*(pow2(mE3) - pow2(Mu))*(pow2(mL3) - pow2(Mu))) + (
        log(mE3)*(-2*pow2(Mu)*pow2(pow2(mE3) - pow2(Mu))*pow6(mE3) + pow6(mL3)*
        (153*pow2(Mu)*pow4(mE3) - 162*pow2(mE3)*pow4(Mu) - 50*pow6(mE3) + 47*
        pow6(Mu)) + (64*pow2(mE3)*pow2(Mu) - 33*pow4(mE3) - 27*pow4(Mu))*pow8(
        mL3) + pow4(mL3)*(-101*pow4(mE3)*pow4(Mu) - 26*pow2(Mu)*pow6(mE3) +
        118*pow2(mE3)*pow6(Mu) + 45*pow8(mE3) - 24*pow8(Mu)) + pow2(mE3)*pow2(
        mL3)*(70*pow4(mE3)*pow4(Mu) - 45*pow2(Mu)*pow6(mE3) - 19*pow2(mE3)*
        pow6(Mu) + 2*pow8(mE3) - 12*pow8(Mu))))/(pow2(mL3)*(pow2(mL3) - pow2(
        Mu))*pow2(pow2(mE3) - pow2(Mu))) - (log(mL3)*((pow2(mE3) - pow2(Mu))*(
        4*pow2(Mu)*pow2(pow2(mL3) - pow2(Mu))*pow6(mL3) + pow2(mL3)*pow4(mE3)*(
        2*(81 + 44*lMR)*pow2(Mu)*pow4(mL3) - (123 + 98*lMR)*pow2(mL3)*pow4(Mu)
        - (71 + 26*lMR)*pow6(mL3) + 4*(2 + 9*lMR)*pow6(Mu)) + pow6(mE3)*((-49 +
        10*lMR)*pow2(Mu)*pow4(mL3) + 2*(37 + 8*lMR)*pow2(mL3)*pow4(Mu) - 12*(-1
        + lMR)*pow6(mL3) - (13 + 14*lMR)*pow6(Mu)) + (-2*(25 + 14*lMR)*pow2(
        mL3)*pow2(Mu) + (27 + 14*lMR)*pow4(mL3) + (15 + 14*lMR)*pow4(Mu))*pow8(
        mE3) + pow2(mE3)*pow2(mL3)*(-2*(87 + 38*lMR)*pow4(mL3)*pow4(Mu) + (77 +
        26*lMR)*pow2(Mu)*pow6(mL3) + (145 + 74*lMR)*pow2(mL3)*pow6(Mu) - 4*
        pow8(mL3) - 12*(3 + 2*lMR)*pow8(Mu))) - 4*log(mE3)*pow2(mE3)*(2*pow4(
        mL3)*pow4(Mu)*(-10*pow2(mL3)*pow2(Mu) + 5*pow4(mL3) + 6*pow4(Mu)) + 2*
        pow6(mE3)*(-27*pow2(Mu)*pow4(mL3) + 27*pow2(mL3)*pow4(Mu) + 9*pow6(mL3)
        - 7*pow6(Mu)) + (-18*pow2(mL3)*pow2(Mu) + 9*pow4(mL3) + 7*pow4(Mu))*
        pow8(mE3) + pow4(mE3)*(95*pow4(mL3)*pow4(Mu) - 58*pow2(Mu)*pow6(mL3) -
        56*pow2(mL3)*pow6(Mu) + 11*pow8(mL3) + 8*pow8(Mu)) + pow2(mE3)*(62*
        pow4(Mu)*pow6(mL3) - 62*pow4(mL3)*pow6(Mu) - 22*pow2(Mu)*pow8(mL3) +
        18*pow2(mL3)*pow8(Mu))) - 2*log(pow2(Mu))*pow2(mE3)*(4*pow2(mL3)*(pow2(
        mL3)*pow2(Mu) - pow4(mL3) + pow4(Mu))*pow6(mE3) + 18*pow6(mL3)*pow6(Mu)
        + 2*(-2*pow2(mL3)*pow2(Mu) + pow4(mL3))*pow8(mE3) - 3*pow4(Mu)*pow8(
        mL3) - 25*pow4(mL3)*pow8(Mu) - pow4(mE3)*(41*pow4(mL3)*pow4(Mu) - 28*
        pow2(Mu)*pow6(mL3) - 18*pow2(mL3)*pow6(Mu) + 4*pow8(mL3) + pow8(Mu)) +
        4*pow2(mE3)*(-11*pow4(Mu)*pow6(mL3) + 15*pow4(mL3)*pow6(Mu) + 2*pow2(
        Mu)*pow8(mL3) - 7*pow2(mL3)*pow8(Mu)) + 12*pow2(mL3)*power10(Mu))))/(
        pow2(mE3)*pow2(pow2(mE3) - pow2(Mu))*pow2(pow2(mL3) - pow2(Mu))) - (2*
        log(pow2(Mu))*((pow2(mE3) - pow2(mL3))*pow2(Mu)*(-2*pow2(pow2(mL3) -
        pow2(Mu))*pow4(mL3)*pow4(Mu) - 2*pow6(mE3)*(pow2(mL3)*pow4(Mu) + pow6(
        mL3) + pow6(Mu)) + (pow4(mL3) + pow4(Mu))*pow8(mE3) + pow4(mE3)*(-7*
        pow4(mL3)*pow4(Mu) + 9*pow2(Mu)*pow6(mL3) + 5*pow2(mL3)*pow6(Mu) - 2*
        pow8(mL3) + pow8(Mu)) + pow2(mE3)*(-8*pow4(Mu)*pow6(mL3) + 5*pow4(mL3)*
        pow6(Mu) + 3*pow2(Mu)*pow8(mL3) - 2*pow2(mL3)*pow8(Mu))) + log(mE3)*
        pow2(mE3)*pow2(mL3)*(4*pow2(mL3)*(pow2(mL3)*pow2(Mu) - pow4(mL3) +
        pow4(Mu))*pow6(mE3) + 18*pow6(mL3)*pow6(Mu) + 2*(-2*pow2(mL3)*pow2(Mu)
        + pow4(mL3))*pow8(mE3) - 3*pow4(Mu)*pow8(mL3) - 25*pow4(mL3)*pow8(Mu) -
        pow4(mE3)*(41*pow4(mL3)*pow4(Mu) - 28*pow2(Mu)*pow6(mL3) - 18*pow2(mL3)
        *pow6(Mu) + 4*pow8(mL3) + pow8(Mu)) + 4*pow2(mE3)*(-11*pow4(Mu)*pow6(
        mL3) + 15*pow4(mL3)*pow6(Mu) + 2*pow2(Mu)*pow8(mL3) - 7*pow2(mL3)*pow8(
        Mu)) + 12*pow2(mL3)*power10(Mu))))/(pow2(mE3)*pow2(mL3)*pow2(pow2(mE3)
        - pow2(Mu))*pow2(pow2(mL3) - pow2(Mu)))))/pow4(pow2(mE3) - pow2(mL3)))/
        pow2(cbeta));
}

double himalaya::ThresholdCalculator::getDeltaLambdaYt2Yb4(int omitLogs){
   using std::log;
   using std::sqrt;
   using gm2calc::dilog;
   
   const double MR2 = pow2(p.scale);
   const double lMR = omitLogs*log(MR2);
   const double mQ32 = p.mq2(2,2);
   const double mD32 = p.md2(2,2);
   const double mU32 = p.mu2(2,2);
   const double mQ3 = sqrt(mQ32);
   const double mU3 = sqrt(mU32);
   const double mD3 = sqrt(mD32);
   const double mA = p.MA;
   const double beta = atan(p.vu/p.vd);
   const double sbeta = sin(beta);
   const double cbeta = cos(beta);
   const double Xt = p.Au(2,2) - p.mu*p.vd/p.vu;
   const double Yt = p.Au(2,2) + p.mu*p.vu/p.vd;
   const double Xb = p.Ad(2,2) - p.mu*p.vu/p.vd;
   const double Yb = p.Ad(2,2) + p.mu*p.vd/p.vu;
   const double Mu = p.mu;
   
   return 2*(-3 + 18*lMR - (6*Xb*Yb*log(mD32)*(2 + 2*lMR - log(mQ32)))/(mD32 -
        mQ32) - 6*(1 + lMR)*log(mQ32) + (12*(1 + lMR)*Xb*Yb*log(mQ32))/(mD32 -
        mQ32) - 6*log(mD32)*(2 + 2*lMR - log(mU32)) + (6*Xb*Yb*(log(mD32) -
        log(mQ32))*log(pow2(mA)))/(mD32 - mQ32) - 6*(4*lMR - log(mD32) + log(
        mU32))*log(pow2(mA)) + 21*pow2(lMR) + (12*Xb*(2*dilog(1 - pow2(Mu)/
        mD32) - 2*dilog(1 - pow2(Mu)/mQ32) + (log(mD32) - log(mQ32))*(-2 - 2*
        lMR + log(mD32) + log(mQ32)))*Mu)/(cbeta*(mD32 - mQ32)*
        sbeta) + 4*pow2(Pi) - (6*Xb*Yb*(mQ32*phixyz(pow2(mA),mQ32,mD32)*(mD32 -
        mQ32 + pow2(mA)) + mD32*(mQ32*(log(mD32) - log(mQ32))*(-log(mQ32) +
        log(pow2(mA))) - phixyz(pow2(mA),mQ32,mQ32)*pow2(mA)))*pow2(sbeta))/(
        mD32*(mD32 - mQ32)*mQ32*pow2(cbeta)) - (24*pow2(Xb))/(mD32 - mQ32) - (
        12*lMR*pow2(Xb))/(mD32 - mQ32) + (36*dilog(1 - mD32/mQ32)*pow2(Xb))/(
        mD32 - mQ32) + (6*(mD32 - 2*mQ32 + mU32)*(log(mD32) - log(mQ32))*log(
        pow2(mA))*pow2(Xb))/pow2(mD32 - mQ32) - (6*log(mQ32)*((1 + 2*lMR)*mD32
        + 3*mQ32 + log(mU32)*(-mQ32 - mU32 + pow2(mA)))*pow2(Xb))/pow2(mD32 -
        mQ32) + (6*log(mD32)*(3*mD32 + 2*lMR*mD32 + mQ32 - log(mQ32)*(3*mD32 -
        3*mQ32 + pow2(mA)) + log(mU32)*(-mQ32 - mU32 + pow2(mA)))*pow2(Xb))/
        pow2(mD32 - mQ32) + (6*(-2*dilog(1 - pow2(Mu)/mD32) - 2*dilog(1 - pow2(
        Mu)/mQ32) + lMR*log(mD32) + lMR*log(mQ32) + log(mD32)*log(mQ32) - pow2(
        lMR) + (2*log(pow2(Mu))*(mD32 + mQ32 - 2*pow2(Mu))*pow2(Mu))/((mD32 -
        pow2(Mu))*(mQ32 - pow2(Mu))) + (2*log(mD32)*pow2(Mu))/(-mD32 + pow2(Mu)
        ) + (2*log(mQ32)*pow2(Mu))/(-mQ32 + pow2(Mu)) - pow2(log(mD32)) - pow2(
        log(mQ32))))/pow2(cbeta) + 3*pow2(log(mQ32)) - (6*Xb*Yb*pow2(log(mQ32))
        )/(mD32 - mQ32) + (6*(3*mD32 - 3*mQ32 + pow2(mA))*pow2(Xb)*pow2(log(
        mQ32)))/pow2(mD32 - mQ32) + (12*Xt*Mu*(2*lMR*mU32*log(mU32)
        *(mQ32 - pow2(Mu)) + 2*lMR*mQ32*log(mQ32)*(-mU32 + pow2(Mu)) - 2*dilog(
        1 - pow2(Mu)/mQ32)*(mQ32 + pow2(Mu))*(-mU32 + pow2(Mu)) - 2*dilog(1 -
        pow2(Mu)/mU32)*(mQ32 - pow2(Mu))*(mU32 + pow2(Mu)) - 2*log(pow2(Mu))*
        pow2(Mu)*(lMR*(mQ32 - mU32) + log(mQ32)*(mU32 - pow2(Mu)) + log(mU32)*(
        -mQ32 + pow2(Mu))) - (mQ32 + pow2(Mu))*(-mU32 + pow2(Mu))*pow2(log(
        mQ32)) - (mQ32 - pow2(Mu))*(mU32 + pow2(Mu))*pow2(log(mU32))))/(cbeta*(
        mQ32 - mU32)*sbeta*(mQ32 - pow2(Mu))*(-mU32 + pow2(Mu))) + 12*pow2(log(
        pow2(mA))) + (6*deltaxyz(pow2(mA),mQ32,mD32)*phixyz(pow2(mA),mQ32,mD32)
        *pow2(Xb))/pow2(-(mD3*mQ32) + pow3(mD3)) + (6*deltaxyz(pow2(mA),mU32,
        mQ32)*phixyz(pow2(mA),mU32,mQ32)*pow2(Xb))/pow2(-(mD32*mQ3) + pow3(mQ3)
        ) - (48*Yb*pow3(Xb))/pow2(mD32 - mQ32) - (24*lMR*Yb*pow3(Xb))/pow2(mD32
        - mQ32) - (12*((1 + lMR)*mD32 + (3 + lMR)*mQ32)*Yb*log(mQ32)*pow3(Xb))/
        pow3(mD32 - mQ32) - (6*Yb*(log(mD32) - log(mQ32))*log(pow2(mA))*(mD32 -
        mQ32 + 2*pow2(mA))*pow3(Xb))/pow3(mD32 - mQ32) + (6*Yb*log(mD32)*(2*(3
        + lMR)*mD32 + 2*(1 + lMR)*mQ32 + log(mQ32)*(-mD32 - 3*mQ32 + 2*pow2(mA)
        ))*pow3(Xb))/pow3(mD32 - mQ32) - (6*Yb*(-mD32 - 3*mQ32 + 2*pow2(mA))*
        pow2(log(mQ32))*pow3(Xb))/pow3(mD32 - mQ32) + (12*Mu*(-8*
        mD32 - 4*lMR*mD32 + 8*mQ32 + 4*lMR*mQ32 + 6*mD32*log(mD32) + 2*lMR*
        mD32*log(mD32) + 2*mQ32*log(mD32) + 2*lMR*mQ32*log(mD32) - 2*mD32*log(
        mQ32) - 2*lMR*mD32*log(mQ32) - 6*mQ32*log(mQ32) - 2*lMR*mQ32*log(mQ32)
        - 2*dilog(1 - pow2(Mu)/mD32)*(mD32 + mQ32 - 2*pow2(Mu)) + 2*dilog(1 -
        pow2(Mu)/mQ32)*(mD32 + mQ32 - 2*pow2(Mu)) - 4*log(mD32)*log(pow2(Mu))*
        pow2(Mu) + 4*log(mQ32)*log(pow2(Mu))*pow2(Mu) - mD32*pow2(log(mD32)) -
        mQ32*pow2(log(mD32)) + 2*pow2(Mu)*pow2(log(mD32)) + mD32*pow2(log(mQ32)
        ) + mQ32*pow2(log(mQ32)) - 2*pow2(Mu)*pow2(log(mQ32)))*pow3(Xb))/(
        cbeta*sbeta*pow3(mD32 - mQ32)) + (6*Xb*Xt*((mD32 - mQ32)*mQ32*deltaxyz(
        pow2(mA),mQ32,mD32)*phixyz(pow2(mA),mQ32,mD32) + mQ32*(-mD32 + mQ32)*
        deltaxyz(pow2(mA),mU32,mD32)*phixyz(pow2(mA),mU32,mD32) - mQ32*log(
        mQ32)*log(mU32)*(mQ32 + mU32 - pow2(mA))*(mD32*mQ32 - pow4(mD3)) -
        mQ32*((mQ32 - mU32)*log(mD32) + (mD32 - 2*mQ32 + mU32)*log(mQ32) + (-
        mD32 + mQ32)*log(mU32))*log(pow2(mA))*(-(mD32*mQ32) + pow4(mD3)) -
        deltaxyz(pow2(mA),mQ32,mQ32)*phixyz(pow2(mA),mQ32,mQ32)*(-(mD32*mQ32) +
        pow4(mD3)) + deltaxyz(pow2(mA),mU32,mQ32)*phixyz(pow2(mA),mU32,mQ32)*(-
        (mD32*mQ32) + pow4(mD3)) - mQ32*log(mD32)*(log(mU32)*(mD32 + mU32 -
        pow2(mA)) + log(mQ32)*(-mD32 - mQ32 + pow2(mA)))*(-(mD32*mQ32) + pow4(
        mD3)) + mQ32*(-2*mQ32 + pow2(mA))*pow2(log(mQ32))*(-(mD32*mQ32) + pow4(
        mD3))))/((mD32 - mQ32)*mQ32*(mQ32 - mU32)*(-(mD32*mQ32) + pow4(mD3))) +
        (6*pow2(Xb)*(2*(mD32 - mQ32)*(mD32 - pow2(Mu)) + lMR*(mD32 - mQ32)*(
        mD32 - pow2(Mu)) + 2*mQ32*dilog(1 - pow2(Mu)/mD32)*(mD32 - pow2(Mu)) -
        2*mQ32*dilog(1 - pow2(Mu)/mQ32)*(mD32 - pow2(Mu)) + (mD32 + (1 + lMR)*
        mQ32)*log(mQ32)*(mD32 - pow2(Mu)) + 2*(mD32 - mQ32)*log(pow2(Mu))*pow2(
        Mu) + mQ32*(mD32 - pow2(Mu))*pow2(log(mD32)) - log(mD32)*(lMR*mD32*mQ32
        + mQ32*log(mQ32)*(mD32 - pow2(Mu)) - (2 + lMR)*mQ32*pow2(Mu) + 2*pow4(
        mD3))))/(pow2(cbeta)*pow2(mD32 - mQ32)*(mD32 - pow2(Mu))) + (6*phixyz(
        pow2(mA),mQ32,mQ32)*((mD32 - 2*mQ32)*deltaxyz(pow2(mA),mQ32,mQ32) + (-
        mD32 + mQ32)*pow2(mA)*(-5*mQ32 + pow2(mA)))*pow2(Xb))/(pow2(mD32 -
        mQ32)*pow4(mQ3)) + (6*Yb*phixyz(pow2(mA),mQ32,mQ32)*(-((mD32 - 3*mQ32)*
        deltaxyz(pow2(mA),mQ32,mQ32)) + (mD32 - mQ32)*pow2(mA)*(-5*mQ32 + pow2(
        mA)))*pow3(Xb))/(pow3(mD32 - mQ32)*pow4(mQ3)) - (6*phixyz(pow2(mA),
        mQ32,mQ32)*(deltaxyz(pow2(mA),mQ32,mQ32) + 5*mQ32*pow2(mA) - pow4(mA)))
        /pow4(mQ3) + (6*Xb*Yb*phixyz(pow2(mA),mQ32,mQ32)*(deltaxyz(pow2(mA),
        mQ32,mQ32) + 5*mQ32*pow2(mA) - pow4(mA)))/((mD32 - mQ32)*pow4(mQ3)) + (
        6*pow2(Xb)*((lMR*(2*mQ32 + mU32 - 2*pow2(Mu)))/((mD32 - mQ32)*mQ32) + (
        log(mQ32)*(mD32 + 2*mQ32 + mU32 - mU32*log(mU32) + lMR*(2*mD32 + mU32 -
        2*pow2(Mu)) - 2*pow2(Mu)))/pow2(mD32 - mQ32) + (2*dilog(1 - pow2(Mu)/
        mD32)*(mD32 - pow2(Mu)))/pow2(mD32 - mQ32) - (2*dilog(1 - pow2(Mu)/
        mQ32)*(mD32 - pow2(Mu)))/pow2(mD32 - mQ32) - (log(mD32)*((3 + 2*lMR)*
        mD32 - mU32*log(mU32) - (1 + lMR)*(-mU32 + 2*pow2(Mu))))/pow2(mD32 -
        mQ32) + ((mD32 - pow2(Mu))*pow2(log(mD32)))/pow2(mD32 - mQ32) - ((mD32
        - pow2(Mu))*pow2(log(mQ32)))/pow2(mD32 - mQ32) + (3*mQ32 + mU32 - 2*
        pow2(Mu))/(mD32*mQ32 - pow4(mQ3)) + (2*log(pow2(Mu))*pow2(Mu))/(mD32*
        mQ32 - pow4(mQ3)) + (mU32*log(mU32))/(-(mD32*mQ32) + pow4(mQ3))))/pow2(
        sbeta) + (6*Yb*phixyz(pow2(mA),mQ32,mD32)*pow3(Xb)*((-3*mD32 + mQ32)*
        deltaxyz(pow2(mA),mQ32,mD32) + (mD32 - mQ32)*(-3*mD32*mQ32 - (3*mD32 +
        2*mQ32)*pow2(mA) + pow4(mA) + 2*pow4(mD3) + pow4(mQ3))))/(pow3(mD32 -
        mQ32)*pow4(mD3)) + (6*pow2(Xt)*(2*(mQ32 - mU32) + 2*mU32*dilog(1 -
        pow2(Mu)/mQ32) - 2*mU32*dilog(1 - pow2(Mu)/mU32) + 2*mU32*log(mU32) +
        lMR*(mQ32 - mU32 + mU32*log(mU32)) + log(mD32)*(mQ32 - mU32 - mU32*log(
        mQ32) + mU32*log(mU32)) + (2*(mQ32 - mU32)*log(pow2(Mu))*pow2(Mu))/(
        mQ32 - pow2(Mu)) + ((mQ32 - mU32)*pow2(Xb))/(mD32 - mQ32) + (mU32*log(
        mU32)*pow2(Xb))/(mD32 - mQ32) - (mQ32*log(mD32)*(mQ32 - mU32 - mU32*
        log(mQ32) + mU32*log(mU32))*pow2(Xb))/pow2(mD32 - mQ32) + mU32*pow2(
        log(mQ32)) - (mQ32*mU32*pow2(Xb)*pow2(log(mQ32)))/pow2(mD32 - mQ32) -
        mU32*pow2(log(mU32)) + (log(mQ32)*pow2(Xb)*(-(mD32*mU32) + mQ32*mU32*
        log(mU32) + pow4(mQ3)))/pow2(mD32 - mQ32) - (log(mQ32)*(lMR*mQ32*mU32 -
        (2 + lMR)*mU32*pow2(Mu) + 2*pow4(mQ3)))/(mQ32 - pow2(Mu))))/(pow2(
        cbeta)*pow2(mQ32 - mU32)) - (3*pow2(sbeta)*pow2(Yb)*(deltaxyz(pow2(mA),
        mQ32,mD32)*(mD32*(-log(mQ32) + log(pow2(mA))) + phixyz(pow2(mA),mQ32,
        mD32)*(mQ32 - pow2(mA))) + (mD32 - mQ32 + pow2(mA))*(-(mD32*(-2*mD32*
        log(mD32) + log(mQ32)*(mD32 + mQ32 - pow2(mA)) + log(pow2(mA))*(mD32 -
        mQ32 + pow2(mA)))) + phixyz(pow2(mA),mQ32,mD32)*(-(mD32*mQ32) - (mD32 +
        2*mQ32)*pow2(mA) + pow4(mA) + pow4(mQ3)))))/(deltaxyz(pow2(mA),mQ32,
        mD32)*pow2(cbeta)*pow4(mD3)) + (9*(2*lMR - log(mD32) - log(mQ32))*pow2(
        Xt)*(-2*mQ32*mU32*log(mQ32) + 2*mQ32*mU32*log(mU32) + pow4(mQ3) - pow4(
        mU3)))/pow3(mQ32 - mU32) + (6*phixyz(pow2(mA),mU32,mD32)*(-3*mD32*mU32
        - deltaxyz(pow2(mA),mU32,mD32) - (3*mD32 + 2*mU32)*pow2(mA) + pow4(mA)
        + 2*pow4(mD3) + pow4(mU3)))/pow4(mD3) - (3*pow2(Xt)*(-6*mQ32*mU32 - 4*
        lMR*mQ32*mU32 + 5*mQ32*mU32*log(mU32) + 2*lMR*mQ32*mU32*log(mU32) -
        mQ32*log(mQ32)*(3*mQ32 + mU32 + 2*lMR*mU32 + (6*mQ32 - 4*mU32)*log(
        mU32)) - 3*mQ32*mU32*pow2(log(mU32)) + 5*pow4(mQ3) + 3*lMR*pow4(mQ3) +
        3*pow2(log(mU32))*pow4(mQ3) + 6*dilog(1 - mQ32/mU32)*(-(mQ32*mU32) +
        pow4(mQ3)) + pow2(log(mQ32))*(-(mQ32*mU32) + 3*pow4(mQ3)) + pow4(mU3) +
        lMR*pow4(mU3) - log(mU32)*pow4(mU3)))/pow2(-(mQ3*mU32) + pow3(mQ3)) + (
        6*phixyz(pow2(mA),mU32,mD32)*pow2(Xb)*((-2*mD32 + mQ32)*deltaxyz(pow2(
        mA),mU32,mD32) + (mD32 - mQ32)*(-3*mD32*mU32 - (3*mD32 + 2*mU32)*pow2(
        mA) + pow4(mA) + 2*pow4(mD3) + pow4(mU3))))/(pow2(mD32 - mQ32)*pow4(
        mD3)) - (6*Xt*Yb*((log(mQ32) - log(mU32))*(-log(mD32) + log(pow2(mA)))*
        pow4(mD3) + phixyz(pow2(mA),mQ32,mD32)*(3*mD32*mQ32 + deltaxyz(pow2(mA)
        ,mQ32,mD32) + (3*mD32 + 2*mQ32)*pow2(mA) - pow4(mA) - 2*pow4(mD3) -
        pow4(mQ3)) + phixyz(pow2(mA),mU32,mD32)*(-3*mD32*mU32 - deltaxyz(pow2(
        mA),mU32,mD32) - (3*mD32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(mD3) +
        pow4(mU3))))/((mQ32 - mU32)*pow4(mD3)) + (6*Xb*Yt*(pow4(mD3)*(-((log(
        mD32) - log(mQ32))*(2 + 2*lMR - log(mU32) - log(pow2(mA)))*pow4(mQ3)) +
        phixyz(pow2(mA),mU32,mQ32)*(3*mQ32*mU32 + deltaxyz(pow2(mA),mU32,mQ32)
        + (3*mQ32 + 2*mU32)*pow2(mA) - pow4(mA) - 2*pow4(mQ3) - pow4(mU3))) +
        phixyz(pow2(mA),mU32,mD32)*pow4(mQ3)*(-3*mD32*mU32 - deltaxyz(pow2(mA),
        mU32,mD32) - (3*mD32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(mD3) +
        pow4(mU3))))/((mD32 - mQ32)*pow4(mD3)*pow4(mQ3)) - (3*(4*lMR*mQ32*mU32*
        log(mU32)*(mU32 - 2*pow2(Mu)) + 2*(mQ32 + 2*mU32 - 2*pow2(Mu))*pow2(Mu)
        *(-mU32 + pow2(Mu)) + mQ32*pow2(-mU32 + pow2(Mu)) - 4*mQ32*dilog(1 -
        pow2(Mu)/mQ32)*pow2(-mU32 + pow2(Mu)) + 2*(1 + 2*lMR)*mQ32*log(mQ32)*
        pow2(-mU32 + pow2(Mu)) - 4*mQ32*pow2(lMR)*pow2(-mU32 + pow2(Mu)) - 2*
        mQ32*pow2(log(mQ32))*pow2(-mU32 + pow2(Mu)) - 2*mU32*log(mU32)*(mQ32*(
        mU32 + 2*pow2(Mu)) + pow2(-mU32 + pow2(Mu))) + 2*log(pow2(Mu))*pow2(Mu)
        *(-2*mQ32*log(mU32)*pow2(Mu) + mQ32*(2*mU32 + pow2(Mu) + 2*lMR*pow2(Mu)
        ) + 2*pow2(-mU32 + pow2(Mu))) + 2*pow2(-(mU3*pow2(Mu)) + pow3(mU3)) +
        4*mQ32*dilog(1 - pow2(Mu)/mU32)*(2*mU32*pow2(Mu) + pow4(Mu) - pow4(mU3)
        ) + 2*mQ32*pow2(log(mU32))*(2*mU32*pow2(Mu) + pow4(Mu) - pow4(mU3)) -
        2*lMR*(-mU32 + pow2(Mu))*(2*mQ32*pow2(Mu) - 3*mU32*pow2(Mu) + 2*pow4(
        Mu) + pow4(mU3))))/(2.*mQ32*pow2(sbeta)*pow2(-mU32 + pow2(Mu))) + (6*
        Xt*Yb*pow2(Xb)*(mQ32*(mQ32 - mU32)*(log(mD32) - log(mQ32))*log(pow2(mA)
        )*pow4(mD3) + deltaxyz(pow2(mA),mQ32,mQ32)*phixyz(pow2(mA),mQ32,mQ32)*
        pow4(mD3) - deltaxyz(pow2(mA),mU32,mQ32)*phixyz(pow2(mA),mU32,mQ32)*
        pow4(mD3) - mQ32*log(mQ32)*log(mU32)*(mQ32 + mU32 - pow2(mA))*pow4(mD3)
        + mQ32*log(mD32)*(log(mU32)*(mQ32 + mU32 - pow2(mA)) + log(mQ32)*(-2*
        mQ32 + pow2(mA)))*pow4(mD3) + mQ32*(2*mQ32 - pow2(mA))*pow2(log(mQ32))*
        pow4(mD3) + mQ32*phixyz(pow2(mA),mQ32,mD32)*((-2*mD32 + mQ32)*deltaxyz(
        pow2(mA),mQ32,mD32) + (mD32 - mQ32)*(-3*mD32*mQ32 - (3*mD32 + 2*mQ32)*
        pow2(mA) + pow4(mA) + 2*pow4(mD3) + pow4(mQ3))) - mQ32*phixyz(pow2(mA),
        mU32,mD32)*((-2*mD32 + mQ32)*deltaxyz(pow2(mA),mU32,mD32) + (mD32 -
        mQ32)*(-3*mD32*mU32 - (3*mD32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(
        mD3) + pow4(mU3)))))/(mQ32*(mQ32 - mU32)*pow2(mD32 - mQ32)*pow4(mD3)) +
        (3*pow2(sbeta)*(-(phixyz(pow2(mA),mQ32,mQ32)*pow2(mA)*pow4(mD3)) +
        mQ32*((-2*log(mQ32) - 2*lMR*log(mQ32) + log(mD32)*(-2 - 2*lMR + log(
        mU32)) + (4 + log(mD32) - log(mU32))*log(pow2(mA)) + 2*pow2(lMR) +
        pow2(log(mQ32)))*pow4(mD3) + phixyz(pow2(mA),mU32,mD32)*(-3*mD32*mU32 -
        deltaxyz(pow2(mA),mU32,mD32) - (3*mD32 + 2*mU32)*pow2(mA) + pow4(mA) +
        2*pow4(mD3) + pow4(mU3)))))/(mQ32*pow2(cbeta)*pow4(mD3)) - (6*Yt*pow3(
        Xb)*(8*(mD32 - mQ32)*pow4(mD3)*pow4(mQ3) + 4*lMR*(mD32 - mQ32)*pow4(
        mD3)*pow4(mQ3) + (log(mD32) - log(mQ32))*log(pow2(mA))*(mD32 + mQ32 -
        2*mU32 + 2*pow2(mA))*pow4(mD3)*pow4(mQ3) - log(mD32)*(2*(3 + lMR)*mD32
        + 2*(1 + lMR)*mQ32 + log(mU32)*(-mD32 - mQ32 - 2*mU32 + 2*pow2(mA)))*
        pow4(mD3)*pow4(mQ3) + log(mQ32)*(2*(1 + lMR)*mD32 + 2*(3 + lMR)*mQ32 +
        log(mU32)*(-mD32 - mQ32 - 2*mU32 + 2*pow2(mA)))*pow4(mD3)*pow4(mQ3) -
        phixyz(pow2(mA),mU32,mD32)*pow4(mQ3)*((-3*mD32 + mQ32)*deltaxyz(pow2(
        mA),mU32,mD32) + (mD32 - mQ32)*(-3*mD32*mU32 - (3*mD32 + 2*mU32)*pow2(
        mA) + pow4(mA) + 2*pow4(mD3) + pow4(mU3))) - phixyz(pow2(mA),mU32,mQ32)
        *pow4(mD3)*(-((mD32 - 3*mQ32)*deltaxyz(pow2(mA),mU32,mQ32)) + (mD32 -
        mQ32)*(-3*mQ32*mU32 - (3*mQ32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(
        mQ3) + pow4(mU3)))))/(pow3(mD32 - mQ32)*pow4(mD3)*pow4(mQ3)) - (3*pow2(
        sbeta)*pow2(Xb)*(pow2(Yb)*(-(mD32*mQ32*log(pow2(mA))*(deltaxyz(pow2(mA)
        ,mQ32,mD32)*(-mD32 + mQ32 + mD32*log(mD32) - mD32*log(mQ32)) + (mD32 -
        mQ32)*pow2(mD32 - mQ32 + pow2(mA)))) - mQ32*phixyz(pow2(mA),mQ32,mD32)*
        (deltaxyz(pow2(mA),mQ32,mD32)*((2*mD32 - mQ32)*pow2(mA) + pow2(mD32 -
        mQ32)) + (mD32 - mQ32)*(mD32 - mQ32 + pow2(mA))*((mD32 - mQ32)*mQ32 + (
        mD32 + 2*mQ32)*pow2(mA) - pow4(mA))) + deltaxyz(pow2(mA),mQ32,mD32)*
        phixyz(pow2(mA),mQ32,mQ32)*pow2(mA)*pow4(mD3) + mQ32*log(mD32)*(
        deltaxyz(pow2(mA),mQ32,mD32)*log(mQ32) + 2*(mD32 - mQ32)*(mD32 - mQ32 +
        pow2(mA)))*pow4(mD3) - mQ32*deltaxyz(pow2(mA),mQ32,mD32)*pow2(log(mQ32)
        )*pow4(mD3) + mD32*(mD32 - mQ32)*mQ32*log(mQ32)*(-deltaxyz(pow2(mA),
        mQ32,mD32) - 2*mQ32*pow2(mA) + pow4(mA) - pow4(mD3) + pow4(mQ3))) +
        deltaxyz(pow2(mA),mQ32,mD32)*(4*(mD32 - mQ32)*mQ32*pow4(mD3) + 2*lMR*(
        mD32 - mQ32)*mQ32*pow4(mD3) - deltaxyz(pow2(mA),mU32,mQ32)*phixyz(pow2(
        mA),mU32,mQ32)*pow4(mD3) + mQ32*(log(mD32) - log(mQ32))*log(pow2(mA))*(
        mQ32 - mU32 + pow2(mA))*pow4(mD3) + mQ32*log(mQ32)*(2*(2 + lMR)*mQ32 +
        log(mU32)*(-mQ32 - mU32 + pow2(mA)))*pow4(mD3) - mQ32*log(mD32)*(2*(
        mD32 + (1 + lMR)*mQ32) + log(mU32)*(-mQ32 - mU32 + pow2(mA)))*pow4(mD3)
        - mQ32*phixyz(pow2(mA),mU32,mD32)*((-2*mD32 + mQ32)*deltaxyz(pow2(mA),
        mU32,mD32) + (mD32 - mQ32)*(-3*mD32*mU32 - (3*mD32 + 2*mU32)*pow2(mA) +
        pow4(mA) + 2*pow4(mD3) + pow4(mU3))))))/(mQ32*deltaxyz(pow2(mA),mQ32,
        mD32)*pow2(cbeta)*pow2(mD32 - mQ32)*pow4(mD3)) - (3*Xt*pow2(sbeta)*(-2*
        (mD32 - mQ32)*Xb*pow4(mD3)*((mD32 - mQ32)*mQ32*deltaxyz(pow2(mA),mQ32,
        mD32)*phixyz(pow2(mA),mQ32,mD32) + mQ32*(-mD32 + mQ32)*deltaxyz(pow2(
        mA),mU32,mD32)*phixyz(pow2(mA),mU32,mD32) - mQ32*log(mQ32)*log(mU32)*(
        mQ32 + mU32 - pow2(mA))*(mD32*mQ32 - pow4(mD3)) - mQ32*((mQ32 - mU32)*
        log(mD32) + (mD32 - 2*mQ32 + mU32)*log(mQ32) + (-mD32 + mQ32)*log(mU32)
        )*log(pow2(mA))*(-(mD32*mQ32) + pow4(mD3)) - deltaxyz(pow2(mA),mQ32,
        mQ32)*phixyz(pow2(mA),mQ32,mQ32)*(-(mD32*mQ32) + pow4(mD3)) + deltaxyz(
        pow2(mA),mU32,mQ32)*phixyz(pow2(mA),mU32,mQ32)*(-(mD32*mQ32) + pow4(
        mD3)) - mQ32*log(mD32)*(log(mU32)*(mD32 + mU32 - pow2(mA)) + log(mQ32)*
        (-mD32 - mQ32 + pow2(mA)))*(-(mD32*mQ32) + pow4(mD3)) + mQ32*(-2*mQ32 +
        pow2(mA))*pow2(log(mQ32))*(-(mD32*mQ32) + pow4(mD3))) + 2*mQ32*Yb*pow2(
        mD32 - mQ32)*(-(mD32*mQ32) + pow4(mD3))*((log(mQ32) - log(mU32))*(-log(
        mD32) + log(pow2(mA)))*pow4(mD3) + phixyz(pow2(mA),mQ32,mD32)*(3*mD32*
        mQ32 + deltaxyz(pow2(mA),mQ32,mD32) + (3*mD32 + 2*mQ32)*pow2(mA) -
        pow4(mA) - 2*pow4(mD3) - pow4(mQ3)) + phixyz(pow2(mA),mU32,mD32)*(-3*
        mD32*mU32 - deltaxyz(pow2(mA),mU32,mD32) - (3*mD32 + 2*mU32)*pow2(mA) +
        pow4(mA) + 2*pow4(mD3) + pow4(mU3))) - 2*Yb*pow2(Xb)*(-(mD32*mQ32) +
        pow4(mD3))*(mQ32*(mQ32 - mU32)*(log(mD32) - log(mQ32))*log(pow2(mA))*
        pow4(mD3) + deltaxyz(pow2(mA),mQ32,mQ32)*phixyz(pow2(mA),mQ32,mQ32)*
        pow4(mD3) - deltaxyz(pow2(mA),mU32,mQ32)*phixyz(pow2(mA),mU32,mQ32)*
        pow4(mD3) - mQ32*log(mQ32)*log(mU32)*(mQ32 + mU32 - pow2(mA))*pow4(mD3)
        + mQ32*log(mD32)*(log(mU32)*(mQ32 + mU32 - pow2(mA)) + log(mQ32)*(-2*
        mQ32 + pow2(mA)))*pow4(mD3) + mQ32*(2*mQ32 - pow2(mA))*pow2(log(mQ32))*
        pow4(mD3) + mQ32*phixyz(pow2(mA),mQ32,mD32)*((-2*mD32 + mQ32)*deltaxyz(
        pow2(mA),mQ32,mD32) + (mD32 - mQ32)*(-3*mD32*mQ32 - (3*mD32 + 2*mQ32)*
        pow2(mA) + pow4(mA) + 2*pow4(mD3) + pow4(mQ3))) - mQ32*phixyz(pow2(mA),
        mU32,mD32)*((-2*mD32 + mQ32)*deltaxyz(pow2(mA),mU32,mD32) + (mD32 -
        mQ32)*(-3*mD32*mU32 - (3*mD32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(
        mD3) + pow4(mU3))))))/(mQ32*(mQ32 - mU32)*pow2(cbeta)*pow2(mD32 - mQ32)
        *pow4(mD3)*(-(mD32*mQ32) + pow4(mD3))) - (3*pow2(sbeta)*pow2(Xt)*(2*
        mD32*(mD32 - mQ32)*Xb*Yb*deltaxyz(pow2(mA),mQ32,mD32)*(-(mD32*(mD32 -
        mQ32)*mQ32*(log(mQ32) - log(mU32))*log(pow2(mA))) - mQ32*deltaxyz(pow2(
        mA),mU32,mD32)*phixyz(pow2(mA),mU32,mD32) + mD32*deltaxyz(pow2(mA),
        mU32,mQ32)*phixyz(pow2(mA),mU32,mQ32) + mD32*mQ32*log(mD32)*(log(mQ32)
        - log(mU32))*(mD32 + mU32 - pow2(mA)) + mD32*mQ32*log(mQ32)*log(mU32)*(
        mQ32 + mU32 - pow2(mA)) - mD32*phixyz(pow2(mA),mQ32,mQ32)*(deltaxyz(
        pow2(mA),mQ32,mQ32) + (mQ32 - mU32)*pow2(mA)) + mQ32*phixyz(pow2(mA),
        mQ32,mD32)*(deltaxyz(pow2(mA),mQ32,mD32) + (mQ32 - mU32)*(mD32 - mQ32 +
        pow2(mA))) - mD32*mQ32*(mQ32 + mU32 - pow2(mA))*pow2(log(mQ32)))*(-(
        mD32*mQ32) + pow4(mD3)) + deltaxyz(pow2(mA),mQ32,mD32)*pow2(mD32 -
        mQ32)*(4*mQ32*(mQ32 - mU32) + 4*mQ32*mU32*log(mU32) + 2*lMR*mQ32*(mQ32
        - mU32 + mU32*log(mU32)) - deltaxyz(pow2(mA),mU32,mQ32)*phixyz(pow2(mA)
        ,mU32,mQ32) - mQ32*log(mQ32)*(2*(mQ32 + (1 + lMR)*mU32) + log(mU32)*(
        mQ32 + mU32 - pow2(mA))) + mQ32*(log(mQ32) - log(mU32))*log(pow2(mA))*(
        -mQ32 + mU32 + pow2(mA)) + phixyz(pow2(mA),mQ32,mQ32)*(deltaxyz(pow2(
        mA),mQ32,mQ32) + (mQ32 - mU32)*pow2(mA)) + mQ32*(mQ32 + mU32 - pow2(mA)
        )*pow2(log(mQ32)))*pow4(mD3)*(-(mD32*mQ32) + pow4(mD3)) + mQ32*pow2(
        mD32 - mQ32)*pow2(Yb)*(-(mD32*mQ32) + pow4(mD3))*(-(mD32*log(pow2(mA))*
        (mD32*deltaxyz(pow2(mA),mQ32,mD32)*log(mQ32) - mD32*deltaxyz(pow2(mA),
        mQ32,mD32)*log(mU32) + (mQ32 - mU32)*(-deltaxyz(pow2(mA),mQ32,mD32) +
        pow2(mD32 - mQ32 + pow2(mA))))) + log(mD32)*(deltaxyz(pow2(mA),mQ32,
        mD32)*(log(mQ32) - log(mU32)) + 2*(mQ32 - mU32)*(mD32 - mQ32 + pow2(mA)
        ))*pow4(mD3) + mD32*(mQ32 - mU32)*log(mQ32)*(-deltaxyz(pow2(mA),mQ32,
        mD32) - 2*mQ32*pow2(mA) + pow4(mA) - pow4(mD3) + pow4(mQ3)) - phixyz(
        pow2(mA),mQ32,mD32)*(deltaxyz(pow2(mA),mQ32,mD32)*(3*mD32*mQ32 + mQ32*
        mU32 + deltaxyz(pow2(mA),mQ32,mD32) + (3*mD32 + 3*mQ32 - mU32)*pow2(mA)
        - pow4(mA) - 2*pow4(mD3) - 2*pow4(mQ3)) - (mQ32 - mU32)*(mD32 - mQ32 +
        pow2(mA))*(-(mD32*mQ32) - (mD32 + 2*mQ32)*pow2(mA) + pow4(mA) + pow4(
        mQ3))) - deltaxyz(pow2(mA),mQ32,mD32)*phixyz(pow2(mA),mU32,mD32)*(-3*
        mD32*mU32 - deltaxyz(pow2(mA),mU32,mD32) - (3*mD32 + 2*mU32)*pow2(mA) +
        pow4(mA) + 2*pow4(mD3) + pow4(mU3))) - pow2(Xb)*pow2(Yb)*(mD32*(mD32 -
        mQ32)*mQ32*(mQ32 - mU32)*log(pow2(mA))*(-deltaxyz(pow2(mA),mQ32,mD32) +
        pow2(mD32 - mQ32 + pow2(mA)))*(-(mD32*mQ32) + pow4(mD3)) + deltaxyz(
        pow2(mA),mQ32,mD32)*deltaxyz(pow2(mA),mU32,mQ32)*phixyz(pow2(mA),mU32,
        mQ32)*pow4(mD3)*(-(mD32*mQ32) + pow4(mD3)) - deltaxyz(pow2(mA),mQ32,
        mD32)*phixyz(pow2(mA),mQ32,mQ32)*(deltaxyz(pow2(mA),mQ32,mQ32) + (mQ32
        - mU32)*pow2(mA))*pow4(mD3)*(-(mD32*mQ32) + pow4(mD3)) - mQ32*log(mD32)
        *(2*(mD32 - mQ32)*(mQ32 - mU32)*(mD32 - mQ32 + pow2(mA)) + deltaxyz(
        pow2(mA),mQ32,mD32)*log(mQ32)*(-mQ32 - mU32 + pow2(mA)) - deltaxyz(
        pow2(mA),mQ32,mD32)*log(mU32)*(-mQ32 - mU32 + pow2(mA)))*pow4(mD3)*(-(
        mD32*mQ32) + pow4(mD3)) - mQ32*deltaxyz(pow2(mA),mQ32,mD32)*(mQ32 +
        mU32 - pow2(mA))*pow2(log(mQ32))*pow4(mD3)*(-(mD32*mQ32) + pow4(mD3)) +
        mQ32*phixyz(pow2(mA),mQ32,mD32)*(-(mD32*mQ32) + pow4(mD3))*((mD32 -
        mQ32)*(mQ32 - mU32)*(mD32 - mQ32 + pow2(mA))*((mD32 - mQ32)*mQ32 + (
        mD32 + 2*mQ32)*pow2(mA) - pow4(mA)) + deltaxyz(pow2(mA),mQ32,mD32)*((2*
        mD32 - mQ32)*deltaxyz(pow2(mA),mQ32,mD32) - (2*mD32 - 2*mQ32 + mU32)*
        pow2(mD32 - mQ32) + (-mD32 + mQ32)*pow4(mA) + pow2(mA)*(mD32*(mQ32 - 2*
        mU32) + mQ32*mU32 + 3*pow4(mD3) - 3*pow4(mQ3)))) - mQ32*log(mQ32)*pow4(
        mD3)*(-(deltaxyz(pow2(mA),mQ32,mD32)*log(mU32)*(mQ32 + mU32 - pow2(mA))
        *(-(mD32*mQ32) + pow4(mD3))) + (mQ32 - mU32)*pow2(mD32 - mQ32)*(-
        deltaxyz(pow2(mA),mQ32,mD32) - 2*mQ32*pow2(mA) + pow4(mA) - pow4(mD3) +
        pow4(mQ3))) + mQ32*deltaxyz(pow2(mA),mQ32,mD32)*phixyz(pow2(mA),mU32,
        mD32)*(-(mD32*mQ32) + pow4(mD3))*((-2*mD32 + mQ32)*deltaxyz(pow2(mA),
        mU32,mD32) + (mD32 - mQ32)*(-3*mD32*mU32 - (3*mD32 + 2*mU32)*pow2(mA) +
        pow4(mA) + 2*pow4(mD3) + pow4(mU3))))))/(mQ32*deltaxyz(pow2(mA),mQ32,
        mD32)*pow2(cbeta)*pow2(mD32 - mQ32)*pow2(mQ32 - mU32)*pow4(mD3)*(-(
        mD32*mQ32) + pow4(mD3))) + (3*pow2(Xb)*pow2(Xt)*(6*(log(mD32) - log(
        mQ32))*pow2(mD32 - mQ32)*(2*mQ32*mU32*log(mQ32) - 2*mQ32*mU32*log(mU32)
        - pow4(mQ3) + pow4(mU3)) + ((mQ32 - mU32)*(2*(mQ32 - mU32)*mU32*log(
        mU32)*pow2(mD32 - mQ32) + 2*pow2(mD32 - mQ32)*pow2(mQ32 - mU32) + 2*
        lMR*pow2(mD32 - mQ32)*pow2(mQ32 - mU32) + 6*(-mD32 + mQ32)*(mD32 + mQ32
        - 2*mU32)*dilog(1 - mD32/mQ32)*(-(mD32*mQ32) + pow4(mQ3)) + 6*(mQ32 -
        mU32)*(-2*mD32 + mQ32 + mU32)*dilog(1 - mQ32/mU32)*(-(mD32*mQ32) +
        pow4(mQ3)) + 6*dilog(1 - mD32/mU32)*pow2(mD32 - mU32)*(-(mD32*mQ32) +
        pow4(mQ3)) + 3*pow2(mD32 - mQ32)*pow2(log(mU32))*(-(mD32*mQ32) + pow4(
        mQ3)) + pow2(log(mQ32))*(-(mD32*mQ32) + pow4(mQ3))*(-4*mQ32*mU32 +
        mD32*(-6*mQ32 + 8*mU32) - 3*pow4(mD3) + 5*pow4(mQ3)) - 2*(mD32 - mQ32)*
        mQ32*log(mD32)*((mQ32 - mU32)*(mD32 + lMR*mQ32 - mU32 - lMR*mU32) +
        log(mQ32)*(-4*mD32*mU32 + 2*mQ32*mU32 + 3*pow4(mD3) - pow4(mQ3)) - log(
        mU32)*(-4*mD32*mU32 + 3*pow4(mD3) + pow4(mU3))) + 2*(mD32 - mQ32)*mQ32*
        log(mQ32)*((1 + lMR)*pow2(mQ32 - mU32) - log(mU32)*(mD32*(6*mQ32 - 4*
        mU32) - 3*pow4(mQ3) + pow4(mU3)))))/mQ32))/(pow3(mD32 - mQ32)*pow3(mQ32
        - mU32)) + (6*Xt*Yt*(pow3(mD32 - mQ32)*pow4(mD3)*(phixyz(pow2(mA),mQ32,
        mQ32)*(-deltaxyz(pow2(mA),mQ32,mQ32) - 5*mQ32*pow2(mA) + pow4(mA)) - (
        log(mQ32) - log(mU32))*(-log(mQ32) + log(pow2(mA)))*pow4(mQ3) + phixyz(
        pow2(mA),mU32,mQ32)*(3*mQ32*mU32 + deltaxyz(pow2(mA),mU32,mQ32) + (3*
        mQ32 + 2*mU32)*pow2(mA) - pow4(mA) - 2*pow4(mQ3) - pow4(mU3))) - Xb*Yb*
        pow2(mD32 - mQ32)*(phixyz(pow2(mA),mQ32,mQ32)*(-deltaxyz(pow2(mA),mQ32,
        mQ32) - 5*mQ32*pow2(mA) + pow4(mA))*pow4(mD3) - log(mD32)*(log(mQ32) -
        log(mU32))*pow4(mD3)*pow4(mQ3) - log(mQ32)*log(mU32)*pow4(mD3)*pow4(
        mQ3) + pow2(log(mQ32))*pow4(mD3)*pow4(mQ3) - phixyz(pow2(mA),mQ32,mD32)
        *pow4(mQ3)*(-3*mD32*mQ32 - deltaxyz(pow2(mA),mQ32,mD32) - (3*mD32 + 2*
        mQ32)*pow2(mA) + pow4(mA) + 2*pow4(mD3) + pow4(mQ3)) + phixyz(pow2(mA),
        mU32,mD32)*pow4(mQ3)*(-3*mD32*mU32 - deltaxyz(pow2(mA),mU32,mD32) - (3*
        mD32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(mD3) + pow4(mU3)) - phixyz(
        pow2(mA),mU32,mQ32)*pow4(mD3)*(-3*mQ32*mU32 - deltaxyz(pow2(mA),mU32,
        mQ32) - (3*mQ32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(mQ3) + pow4(mU3)
        )) - mD32*(mD32 - mQ32)*pow2(Xb)*(-(mD32*phixyz(pow2(mA),mQ32,mQ32)*((
        mD32 - 2*mQ32)*deltaxyz(pow2(mA),mQ32,mQ32) + (-mD32 + mQ32)*pow2(mA)*(
        -5*mQ32 + pow2(mA)))) + mD32*(mQ32 - mU32)*(log(mD32) - log(mQ32))*log(
        pow2(mA))*pow4(mQ3) - deltaxyz(pow2(mA),mQ32,mD32)*phixyz(pow2(mA),
        mQ32,mD32)*pow4(mQ3) + deltaxyz(pow2(mA),mU32,mD32)*phixyz(pow2(mA),
        mU32,mD32)*pow4(mQ3) - mD32*log(mQ32)*log(mU32)*(mD32 + mU32 - pow2(mA)
        )*pow4(mQ3) + mD32*log(mD32)*(log(mU32)*(mD32 + mU32 - pow2(mA)) + log(
        mQ32)*(-mD32 - mQ32 + pow2(mA)))*pow4(mQ3) + mD32*(mD32 + mQ32 - pow2(
        mA))*pow2(log(mQ32))*pow4(mQ3) - mD32*phixyz(pow2(mA),mU32,mQ32)*(-((
        mD32 - 2*mQ32)*deltaxyz(pow2(mA),mU32,mQ32)) + (mD32 - mQ32)*(-3*mQ32*
        mU32 - (3*mQ32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(mQ3) + pow4(mU3))
        )) + Yb*pow3(Xb)*(phixyz(pow2(mA),mQ32,mQ32)*(-((mD32 - 3*mQ32)*
        deltaxyz(pow2(mA),mQ32,mQ32)) + (mD32 - mQ32)*pow2(mA)*(-5*mQ32 + pow2(
        mA)))*pow4(mD3) + 2*(mQ32 - mU32)*(log(mD32) - log(mQ32))*log(pow2(mA))
        *pow4(mD3)*pow4(mQ3) - log(mQ32)*log(mU32)*(mD32 + mQ32 + 2*mU32 - 2*
        pow2(mA))*pow4(mD3)*pow4(mQ3) + log(mD32)*(log(mU32)*(mD32 + mQ32 + 2*
        mU32 - 2*pow2(mA)) + log(mQ32)*(-mD32 - 3*mQ32 + 2*pow2(mA)))*pow4(mD3)
        *pow4(mQ3) + (mD32 + 3*mQ32 - 2*pow2(mA))*pow2(log(mQ32))*pow4(mD3)*
        pow4(mQ3) + phixyz(pow2(mA),mQ32,mD32)*pow4(mQ3)*((-3*mD32 + mQ32)*
        deltaxyz(pow2(mA),mQ32,mD32) + (mD32 - mQ32)*(-3*mD32*mQ32 - (3*mD32 +
        2*mQ32)*pow2(mA) + pow4(mA) + 2*pow4(mD3) + pow4(mQ3))) - phixyz(pow2(
        mA),mU32,mD32)*pow4(mQ3)*((-3*mD32 + mQ32)*deltaxyz(pow2(mA),mU32,mD32)
        + (mD32 - mQ32)*(-3*mD32*mU32 - (3*mD32 + 2*mU32)*pow2(mA) + pow4(mA) +
        2*pow4(mD3) + pow4(mU3))) + phixyz(pow2(mA),mU32,mQ32)*pow4(mD3)*((mD32
        - 3*mQ32)*deltaxyz(pow2(mA),mU32,mQ32) + (-mD32 + mQ32)*(-3*mQ32*mU32 -
        (3*mQ32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(mQ3) + pow4(mU3))))))/((
        mQ32 - mU32)*pow3(mD32 - mQ32)*pow4(mD3)*pow4(mQ3)) + (48*pow4(Xb))/
        pow2(mD32 - mQ32) + (24*lMR*pow4(Xb))/pow2(mD32 - mQ32) - (18*dilog(1 -
        mD32/mQ32)*pow4(Xb))/pow2(mD32 - mQ32) + (6*((3 + 2*lMR)*mD32 + (5 + 2*
        lMR)*mQ32)*log(mQ32)*pow4(Xb))/pow3(mD32 - mQ32) - (6*log(mD32)*((7 +
        2*lMR)*mD32 + (1 + 2*lMR)*mQ32 - 2*(mD32 + mQ32)*log(mQ32))*pow4(Xb))/
        pow3(mD32 - mQ32) - (12*(mD32 + mQ32)*pow2(log(mQ32))*pow4(Xb))/pow3(
        mD32 - mQ32) - (3*(-((mD32 - mQ32)*(mD32 + 5*mQ32)*mU32*log(mU32)) + (
        mD32 - mQ32)*(mQ32*(3*mQ32 + 5*mU32 - 16*pow2(Mu)) + mD32*(9*mQ32 +
        mU32 - 2*pow2(Mu))) + 2*mQ32*dilog(1 - pow2(Mu)/mD32)*(mD32 + 2*mQ32 -
        3*pow2(Mu))*(mD32 - pow2(Mu)) - 2*mQ32*dilog(1 - pow2(Mu)/mQ32)*(mD32 +
        2*mQ32 - 3*pow2(Mu))*(mD32 - pow2(Mu)) + mQ32*(mD32 + 2*mQ32 - 3*pow2(
        Mu))*(mD32 - pow2(Mu))*pow2(log(mD32)) - mQ32*(mD32 + 2*mQ32 - 3*pow2(
        Mu))*(mD32 - pow2(Mu))*pow2(log(mQ32)) + mQ32*log(mQ32)*(-2*(2*mD32 +
        mQ32)*mU32*log(mU32) + mQ32*(mQ32 + 2*(1 + lMR)*mU32 - 2*(5 + 2*lMR)*
        pow2(Mu)) + 2*mD32*((5 + 2*lMR)*mQ32 - 2*(1 + lMR)*(-mU32 + 2*pow2(Mu))
        ) + (1 + 2*lMR)*pow4(mD3)) + 2*log(pow2(Mu))*pow2(Mu)*(mD32*mQ32 + 3*
        mQ32*log(mQ32)*pow2(Mu) + pow4(mD3) - 2*pow4(mQ3)) + lMR*(mD32 - mQ32)*
        (5*mQ32*(mU32 - 2*pow2(Mu)) + mD32*(5*mQ32 + mU32 - 2*pow2(Mu)) + pow4(
        mQ3)) - 2*mQ32*log(mD32)*(-((2*mD32 + mQ32)*mU32*log(mU32)) + mD32*(3*
        mQ32 + 2*mU32 - 7*pow2(Mu)) + mQ32*(mU32 - 2*pow2(Mu)) + 3*pow4(mD3) +
        lMR*(mQ32*(mU32 - 2*pow2(Mu)) + 2*mD32*(mQ32 + mU32 - 2*pow2(Mu)) +
        pow4(mD3)) + 3*log(pow2(Mu))*pow4(Mu)))*pow4(Xb))/(mQ32*pow2(sbeta)*
        pow4(mD32 - mQ32)) + (6*Xb*Yb*phixyz(pow2(mA),mQ32,mD32)*(-3*mD32*mQ32
        - deltaxyz(pow2(mA),mQ32,mD32) - (3*mD32 + 2*mQ32)*pow2(mA) + pow4(mA)
        + 2*pow4(mD3) + pow4(mQ3)))/(-(mQ32*pow4(mD3)) + pow6(mD3)) - (3*pow2(
        Xt)*pow4(Xb)*(3*(mD32 - mQ32)*mQ32*(2*(mD32 - mQ32) - (mD32 + mQ32)*
        log(mD32) + (mD32 + mQ32)*log(mQ32))*(-2*mQ32*mU32*log(mQ32) + 2*mQ32*
        mU32*log(mU32) + pow4(mQ3) - pow4(mU3)) + (mQ32 - mU32)*((mQ32 - mU32)*
        mU32*log(mU32)*pow2(mD32 - mQ32) - 6*mQ32*(mQ32 - mU32)*dilog(1 - mD32/
        mU32)*pow2(mD32 - mU32) + 6*mQ32*(mQ32 - mU32)*dilog(1 - mQ32/mU32)*
        pow2(mD32 - mU32) + lMR*(mD32 - mQ32)*(mD32 + 5*mQ32)*pow2(mQ32 - mU32)
        + (mD32 - mQ32)*(mD32 + 11*mQ32)*pow2(mQ32 - mU32) + (mD32 - mQ32)*
        mQ32*(2*mQ32*(3*mD32 + mQ32) - 4*(mD32 + mQ32)*mU32)*pow2(log(mQ32)) +
        6*(mQ32 - mU32)*dilog(1 - mD32/mQ32)*pow2(-(mD32*mQ3) + pow3(mQ3)) -
        mQ32*log(mQ32)*(-2*(mD32 - mU32)*(mQ32*mU32 + mD32*(-3*mQ32 + 2*mU32))*
        log(mU32) + (mQ32 - mU32)*(2*(4 + lMR)*mQ32*mU32 + mD32*(-2*(5 + 2*lMR)
        *mQ32 + 4*(1 + lMR)*mU32) + 3*pow4(mD3) - (5 + 2*lMR)*pow4(mQ3))) -
        mQ32*log(mD32)*(2*(mD32 - mU32)*(mQ32*mU32 + mD32*(-3*mQ32 + 2*mU32))*
        log(mU32) - 2*(mQ32 - mU32)*(mD32*(-((7 + 2*lMR)*mQ32) + (5 + 2*lMR)*
        mU32) + mQ32*(mU32 + lMR*(-mQ32 + mU32)) + pow4(mD3)) + log(mQ32)*((6*
        mQ32 - 4*mU32)*pow4(mD3) - 4*mD32*pow4(mQ3) + 4*mU32*pow4(mQ3) - 2*
        pow6(mQ3))))))/(mQ32*pow3(mQ32 - mU32)*pow4(mD32 - mQ32)) + (pow2(
        cbeta)*(pow4(mQ3)*(3*mQ32 + 6*lMR*mQ32 - 6*mQ32*log(mD32)*(2 + 2*lMR -
        log(mU32)) + 12*mQ32*pow2(lMR) - 6*pow2(mA) - 6*lMR*pow2(mA) + 6*log(
        pow2(mA))*(mQ32 - 2*lMR*mQ32 + mQ32*log(mD32) - mQ32*log(mU32) + pow2(
        mA)) + 2*mQ32*pow2(Pi) + 6*mQ32*pow2(log(pow2(mA))) + (6*mQ32*phixyz(
        pow2(mA),mU32,mD32)*(-3*mD32*mU32 - deltaxyz(pow2(mA),mU32,mD32) - (3*
        mD32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(mD3) + pow4(mU3)))/pow4(
        mD3)) + (6*pow2(Xb)*pow4(mQ3)*(deltaxyz(pow2(mA),mU32,mQ32)*phixyz(
        pow2(mA),mU32,mQ32)*pow4(mD3) + 2*(mD32 - mQ32)*(-2*mQ32 + pow2(mA))*
        pow4(mD3) + 2*lMR*(mD32 - mQ32)*(-mQ32 + pow2(mA))*pow4(mD3) + mQ32*
        log(mQ32)*(-2*(2 + lMR)*mQ32 + log(mU32)*(mQ32 + mU32 - pow2(mA)) + 2*(
        1 + lMR)*pow2(mA))*pow4(mD3) - mQ32*log(mD32)*(-2*(mD32 + (1 + lMR)*
        mQ32) + log(mU32)*(mQ32 + mU32 - pow2(mA)) + 2*(1 + lMR)*pow2(mA))*
        pow4(mD3) - log(pow2(mA))*(2*(mD32 - mQ32)*pow2(mA) - mQ32*log(mD32)*(-
        mQ32 + mU32 + pow2(mA)) + mQ32*log(mQ32)*(-mQ32 + mU32 + pow2(mA)))*
        pow4(mD3) + mQ32*phixyz(pow2(mA),mU32,mD32)*((-2*mD32 + mQ32)*deltaxyz(
        pow2(mA),mU32,mD32) + (mD32 - mQ32)*(-3*mD32*mU32 - (3*mD32 + 2*mU32)*
        pow2(mA) + pow4(mA) + 2*pow4(mD3) + pow4(mU3)))))/(pow2(mD32 - mQ32)*
        pow4(mD3)) + (12*mQ32*Yt*(Xb*pow2(mD32 - mQ32)*(pow4(mD3)*(-((log(mD32)
        - log(mQ32))*(2 + 2*lMR - log(mU32) - log(pow2(mA)))*pow4(mQ3)) +
        phixyz(pow2(mA),mU32,mQ32)*(3*mQ32*mU32 + deltaxyz(pow2(mA),mU32,mQ32)
        + (3*mQ32 + 2*mU32)*pow2(mA) - pow4(mA) - 2*pow4(mQ3) - pow4(mU3))) +
        phixyz(pow2(mA),mU32,mD32)*pow4(mQ3)*(-3*mD32*mU32 - deltaxyz(pow2(mA),
        mU32,mD32) - (3*mD32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(mD3) +
        pow4(mU3))) - pow3(Xb)*(8*(mD32 - mQ32)*pow4(mD3)*pow4(mQ3) + 4*lMR*(
        mD32 - mQ32)*pow4(mD3)*pow4(mQ3) + (log(mD32) - log(mQ32))*log(pow2(mA)
        )*(mD32 + mQ32 - 2*mU32 + 2*pow2(mA))*pow4(mD3)*pow4(mQ3) - log(mD32)*(
        2*(3 + lMR)*mD32 + 2*(1 + lMR)*mQ32 + log(mU32)*(-mD32 - mQ32 - 2*mU32
        + 2*pow2(mA)))*pow4(mD3)*pow4(mQ3) + log(mQ32)*(2*(1 + lMR)*mD32 + 2*(3
        + lMR)*mQ32 + log(mU32)*(-mD32 - mQ32 - 2*mU32 + 2*pow2(mA)))*pow4(mD3)
        *pow4(mQ3) - phixyz(pow2(mA),mU32,mD32)*pow4(mQ3)*((-3*mD32 + mQ32)*
        deltaxyz(pow2(mA),mU32,mD32) + (mD32 - mQ32)*(-3*mD32*mU32 - (3*mD32 +
        2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(mD3) + pow4(mU3))) + phixyz(pow2(
        mA),mU32,mQ32)*pow4(mD3)*((mD32 - 3*mQ32)*deltaxyz(pow2(mA),mU32,mQ32)
        + (-mD32 + mQ32)*(-3*mQ32*mU32 - (3*mQ32 + 2*mU32)*pow2(mA) + pow4(mA)
        + 2*pow4(mQ3) + pow4(mU3))))))/(pow3(mD32 - mQ32)*pow4(mD3)) - (6*(1 +
        lMR - log(pow2(mA)))*pow2(mA)*pow4(mQ3)*(4*mD32*mQ32 + pow4(mD3) - 5*
        pow4(mQ3) - 2*log(mD32)*(2*mD32*mQ32 + pow4(mQ3)) + 2*log(mQ32)*(2*
        mD32*mQ32 + pow4(mQ3)))*pow4(Xb))/pow4(mD32 - mQ32) + (3*pow2(Yt)*(-2*
        deltaxyz(pow2(mA),mU32,mQ32)*pow4(mQ3) - 2*lMR*deltaxyz(pow2(mA),mU32,
        mQ32)*pow4(mQ3) + 2*log(mQ32)*pow4(mQ3)*(-3*mQ32*mU32 - deltaxyz(pow2(
        mA),mU32,mQ32) - (3*mQ32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(mQ3) +
        pow4(mU3)) + mQ32*log(mU32)*(deltaxyz(pow2(mA),mU32,mQ32)*(2*mQ32 +
        mU32 - pow2(mA)) + (-mQ32 - mU32 + pow2(mA))*(-3*mQ32*mU32 - (3*mQ32 +
        2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(mQ3) + pow4(mU3))) + mQ32*log(
        pow2(mA))*(deltaxyz(pow2(mA),mU32,mQ32)*(2*mQ32 - mU32 + pow2(mA)) - (
        mQ32 - mU32 + pow2(mA))*(-3*mQ32*mU32 - (3*mQ32 + 2*mU32)*pow2(mA) +
        pow4(mA) + 2*pow4(mQ3) + pow4(mU3))) + phixyz(pow2(mA),mU32,mQ32)*(
        deltaxyz(pow2(mA),mU32,mQ32)*(6*mQ32*mU32 + 2*deltaxyz(pow2(mA),mU32,
        mQ32) + 6*(mQ32 + mU32)*pow2(mA) - 3*pow4(mA) - 2*pow4(mQ3) - 3*pow4(
        mU3)) + (-(mQ32*mU32) - (mQ32 + 2*mU32)*pow2(mA) + pow4(mA) + pow4(mU3)
        )*(-3*mQ32*mU32 - (3*mQ32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(mQ3) +
        pow4(mU3))) + (2*pow2(Xb)*(2*(mD32 - mQ32)*deltaxyz(pow2(mA),mU32,mQ32)
        *pow4(mQ3) + 2*lMR*(mD32 - mQ32)*deltaxyz(pow2(mA),mU32,mQ32)*pow4(mQ3)
        + log(mQ32)*pow4(mQ3)*(deltaxyz(pow2(mA),mU32,mQ32)*(2*(mD32 + lMR*
        mQ32) - mQ32*log(mU32)) - 2*(mD32 - mQ32)*(-3*mQ32*mU32 - (3*mQ32 + 2*
        mU32)*pow2(mA) + pow4(mA) + 2*pow4(mQ3) + pow4(mU3))) + (mD32 - mQ32)*
        mQ32*log(mU32)*(deltaxyz(pow2(mA),mU32,mQ32)*(-2*mQ32 - mU32 + pow2(mA)
        ) + (mQ32 + mU32 - pow2(mA))*(-3*mQ32*mU32 - (3*mQ32 + 2*mU32)*pow2(mA)
        + pow4(mA) + 2*pow4(mQ3) + pow4(mU3))) + mQ32*log(pow2(mA))*(deltaxyz(
        pow2(mA),mU32,mQ32)*log(mD32)*pow4(mQ3) - deltaxyz(pow2(mA),mU32,mQ32)*
        log(mQ32)*pow4(mQ3) + (mD32 - mQ32)*(deltaxyz(pow2(mA),mU32,mQ32)*(-2*
        mQ32 + mU32 - pow2(mA)) + (mQ32 - mU32 + pow2(mA))*(-3*mQ32*mU32 - (3*
        mQ32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(mQ3) + pow4(mU3)))) +
        phixyz(pow2(mA),mU32,mQ32)*((mD32 - mQ32)*((mQ32 - mU32)*mU32 + (mQ32 +
        2*mU32)*pow2(mA) - pow4(mA))*(-3*mQ32*mU32 - (3*mQ32 + 2*mU32)*pow2(mA)
        + pow4(mA) + 2*pow4(mQ3) + pow4(mU3)) + deltaxyz(pow2(mA),mU32,mQ32)*(-
        6*mD32*mQ32*mU32 + (-2*mD32 + 3*mQ32)*deltaxyz(pow2(mA),mU32,mQ32) + (
        3*mD32 - 4*mQ32)*pow4(mA) + 2*mD32*pow4(mQ3) + 9*mU32*pow4(mQ3) + pow2(
        mA)*(8*mQ32*mU32 - 6*mD32*(mQ32 + mU32) + 9*pow4(mQ3)) + 3*mD32*pow4(
        mU3) - 4*mQ32*pow4(mU3) - 4*pow6(mQ3))) - deltaxyz(pow2(mA),mU32,mQ32)*
        log(mD32)*(2 + 2*lMR - log(mU32))*pow6(mQ3) + (deltaxyz(pow2(mA),mU32,
        mQ32)*phixyz(pow2(mA),mU32,mD32)*(-3*mD32*mU32 - deltaxyz(pow2(mA),
        mU32,mD32) - (3*mD32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(mD3) +
        pow4(mU3))*pow6(mQ3))/pow4(mD3)))/pow2(mD32 - mQ32) - (pow4(Xb)*(2*lMR*
        (mD32 - mQ32)*(mD32 + 5*mQ32)*deltaxyz(pow2(mA),mU32,mQ32)*pow4(mD3)*
        pow4(mQ3) + 2*(mD32 - mQ32)*(mD32 + 11*mQ32)*deltaxyz(pow2(mA),mU32,
        mQ32)*pow4(mD3)*pow4(mQ3) - mQ32*log(mU32)*pow2(mD32 - mQ32)*pow4(mD3)*
        (deltaxyz(pow2(mA),mU32,mQ32)*(2*mQ32 + mU32 - pow2(mA)) + (-mQ32 -
        mU32 + pow2(mA))*(-3*mQ32*mU32 - (3*mQ32 + 2*mU32)*pow2(mA) + pow4(mA)
        + 2*pow4(mQ3) + pow4(mU3))) - 2*log(mQ32)*pow4(mD3)*pow4(mQ3)*(-2*mD32*
        mQ32*deltaxyz(pow2(mA),mU32,mQ32) - 2*lMR*mQ32*(2*mD32 + mQ32)*
        deltaxyz(pow2(mA),mU32,mQ32) + mQ32*deltaxyz(pow2(mA),mU32,mQ32)*log(
        mU32)*(2*mD32 + mQ32 + 3*mU32 - 3*pow2(mA)) - deltaxyz(pow2(mA),mU32,
        mQ32)*pow4(mD3) - 9*deltaxyz(pow2(mA),mU32,mQ32)*pow4(mQ3) + pow2(mD32
        - mQ32)*(-3*mQ32*mU32 - (3*mQ32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*pow4(
        mQ3) + pow4(mU3))) - phixyz(pow2(mA),mU32,mQ32)*pow4(mD3)*(2*pow2(
        deltaxyz(pow2(mA),mU32,mQ32))*(-4*mD32*mQ32 + pow4(mD3) + 6*pow4(mQ3))
        + pow2(mD32 - mQ32)*(-(mQ32*mU32) - (mQ32 + 2*mU32)*pow2(mA) + pow4(mA)
        + pow4(mU3))*(-3*mQ32*mU32 - (3*mQ32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*
        pow4(mQ3) + pow4(mU3)) - (mD32 - mQ32)*deltaxyz(pow2(mA),mU32,mQ32)*(-
        6*(mD32 - 3*mQ32)*mQ32*mU32 + (3*mD32 - 7*mQ32)*pow4(mA) + 2*(mD32 - 5*
        mQ32)*pow4(mQ3) + 2*pow2(mA)*(7*mQ32*mU32 - 3*mD32*(mQ32 + mU32) + 9*
        pow4(mQ3)) + (3*mD32 - 7*mQ32)*pow4(mU3))) + mQ32*log(pow2(mA))*pow4(
        mD3)*(2*deltaxyz(pow2(mA),mU32,mQ32)*log(mD32)*(2*mD32 + mQ32 - 3*mU32
        + 3*pow2(mA))*pow4(mQ3) - 2*deltaxyz(pow2(mA),mU32,mQ32)*log(mQ32)*(2*
        mD32 + mQ32 - 3*mU32 + 3*pow2(mA))*pow4(mQ3) - pow2(mD32 - mQ32)*(
        deltaxyz(pow2(mA),mU32,mQ32)*(2*mQ32 - mU32 + pow2(mA)) - (mQ32 - mU32
        + pow2(mA))*(-3*mQ32*mU32 - (3*mQ32 + 2*mU32)*pow2(mA) + pow4(mA) + 2*
        pow4(mQ3) + pow4(mU3)))) - 2*deltaxyz(pow2(mA),mU32,mQ32)*log(mD32)*(2*
        (5 + 2*lMR)*mD32 + 2*(1 + lMR)*mQ32 + log(mU32)*(-2*mD32 - mQ32 - 3*
        mU32 + 3*pow2(mA)))*pow4(mD3)*pow6(mQ3) - 2*deltaxyz(pow2(mA),mU32,
        mQ32)*phixyz(pow2(mA),mU32,mD32)*((-4*mD32 + mQ32)*deltaxyz(pow2(mA),
        mU32,mD32) + (mD32 - mQ32)*(-3*mD32*mU32 - (3*mD32 + 2*mU32)*pow2(mA) +
        pow4(mA) + 2*pow4(mD3) + pow4(mU3)))*pow6(mQ3)))/pow4(-(mD3*mQ32) +
        pow3(mD3))))/deltaxyz(pow2(mA),mU32,mQ32)))/(2.*pow2(sbeta)*pow6(mQ3)));
}

double himalaya::ThresholdCalculator::getDeltaLambdaYt4Yb2(int omitLogs){
   using std::log;
   using std::sqrt;
   using gm2calc::dilog;
   
   const double MR2 = pow2(p.scale);
   const double lMR = omitLogs*log(MR2);
   const double mQ32 = p.mq2(2,2);
   const double mD32 = p.md2(2,2);
   const double mU32 = p.mu2(2,2);
   const double mQ3 = sqrt(mQ32);
   const double mU3 = sqrt(mU32);
   const double mD3 = sqrt(mD32);
   const double mA = p.MA;
   const double beta = atan(p.vu/p.vd);
   const double sbeta = sin(beta);
   const double cbeta = cos(beta);
   const double Xt = p.Au(2,2) - p.mu*p.vd/p.vu;
   const double Yt = p.Au(2,2) + p.mu*p.vu/p.vd;
   const double Xb = p.Ad(2,2) - p.mu*p.vu/p.vd;
   const double Yb = p.Ad(2,2) + p.mu*p.vd/p.vu;
   const double Mu = p.mu;
   
   return 2*(3*Xb*Yt*((2*log(mD32)*log(mU32))/(mD32 - mQ32) + (2*log(mQ32)*log(
        mU32))/(-mD32 + mQ32) + ((2*log(mD32))/(-mD32 + mQ32) + (2*log(mQ32))/(
        mD32 - mQ32))*log(pow2(mA)) - (2*phixyz(pow2(mA),mU32,mD32)*(mD32 -
        mU32 + pow2(mA)))/(mD32*(mD32 - mQ32)) + (2*phixyz(pow2(mA),mU32,mQ32)*
        (mQ32 - mU32 + pow2(mA)))/((mD32 - mQ32)*mQ32)) + 3*Xb*Yb*((2*log(mD32)
        *log(mQ32))/(mD32 - mQ32) + ((2*log(mD32))/(-mD32 + mQ32) + (2*log(
        mQ32))/(mD32 - mQ32))*log(pow2(mA)) + (2*phixyz(pow2(mA),mQ32,mQ32)*
        pow2(mA))/((mD32 - mQ32)*mQ32) - (2*phixyz(pow2(mA),mQ32,mD32)*(mD32 -
        mQ32 + pow2(mA)))/(mD32*(mD32 - mQ32)) + (2*pow2(log(mQ32)))/(-mD32 +
        mQ32)) + 3*pow2(Xb)*((mD32 - 5*mQ32)/(mQ32*(-mD32 + mQ32)) + (lMR*(mD32
        - 3*mQ32))/(mQ32*(-mD32 + mQ32)) + (6*dilog(1 - mD32/mQ32))/(-mD32 +
        mQ32) + log(mQ32)*((2*lMR*mD32)/pow2(mD32 - mQ32) + (mD32 + 3*mQ32)/
        pow2(mD32 - mQ32)) + log(mD32)*((-2*lMR*mD32)/pow2(mD32 - mQ32) + (
        mD32*(mD32 - 5*mQ32))/(mQ32*pow2(mD32 - mQ32)) + (2*mD32*log(mQ32))/
        pow2(mD32 - mQ32)) - (2*mD32*pow2(log(mQ32)))/pow2(mD32 - mQ32)) + 3*(-
        1 - 2*lMR + (2 + 2*lMR)*log(mQ32) + 2*log(mD32)*log(mU32) + (-2*log(
        mD32) - 4*log(mQ32) - 2*log(mU32))*log(pow2(mA)) - pow2(lMR) - (2*
        phixyz(pow2(mA),mQ32,mQ32)*pow2(mA))/mQ32 - (2*phixyz(pow2(mA),mU32,
        mD32)*(mD32 - mU32 + pow2(mA)))/mD32 + (4*pow2(Pi))/3. + pow2(log(mQ32)
        ) + 4*pow2(log(pow2(mA)))) + (3*Yb*(-16/pow2(mQ32 - mU32) + log(mQ32)*(
        (4*lMR*(mQ32 + mU32))/pow3(mQ32 - mU32) + (4*(3*mQ32 + mU32))/pow3(mQ32
        - mU32)) + lMR*(-8/pow2(mQ32 - mU32) - (4*(mQ32 + mU32)*log(mU32))/
        pow3(mQ32 - mU32)) + log(mD32)*((-2*log(mQ32)*(2*mD32 + mQ32 + mU32 -
        2*pow2(mA)))/pow3(mQ32 - mU32) + (2*log(mU32)*(2*mD32 + mQ32 + mU32 -
        2*pow2(mA)))/pow3(mQ32 - mU32)) + log(pow2(mA))*((-2*log(mQ32)*(-2*mD32
        + mQ32 + mU32 + 2*pow2(mA)))/pow3(mQ32 - mU32) + (2*log(mU32)*(-2*mD32
        + mQ32 + mU32 + 2*pow2(mA)))/pow3(mQ32 - mU32)) - (4*(mQ32 + 3*mU32)*
        log(mU32))/pow3(mQ32 - mU32) - (2*phixyz(pow2(mA),mQ32,mD32)*(2*
        deltaxyz(pow2(mA),mQ32,mD32) + (mQ32 - mU32)*(mD32 - mQ32 + pow2(mA))))
        /(mD32*pow3(mQ32 - mU32)) + (phixyz(pow2(mA),mU32,mD32)*(4*deltaxyz(
        pow2(mA),mU32,mD32) + 2*(-mQ32 + mU32)*(mD32 - mU32 + pow2(mA))))/(
        mD32*pow3(mQ32 - mU32))) + Yt*(3*(-16/pow2(mQ32 - mU32) + lMR*(-8/pow2(
        mQ32 - mU32) - (4*(mQ32 + mU32)*log(mU32))/pow3(mQ32 - mU32)) + log(
        mQ32)*((4*lMR*(mQ32 + mU32))/pow3(mQ32 - mU32) + (4*(3*mQ32 + mU32))/
        pow3(mQ32 - mU32) + (2*log(mU32)*(3*mQ32 + mU32 - 2*pow2(mA)))/pow3(
        mQ32 - mU32)) + log(pow2(mA))*((2*log(mQ32)*(mQ32 - mU32 - 2*pow2(mA)))
        /pow3(mQ32 - mU32) + (2*log(mU32)*(-mQ32 + mU32 + 2*pow2(mA)))/pow3(
        mQ32 - mU32)) - (4*(mQ32 + 3*mU32)*log(mU32))/pow3(mQ32 - mU32) - (2*
        phixyz(pow2(mA),mQ32,mQ32)*(2*deltaxyz(pow2(mA),mQ32,mQ32) + (mQ32 -
        mU32)*pow2(mA)))/(mQ32*pow3(mQ32 - mU32)) + (phixyz(pow2(mA),mU32,mQ32)
        *(4*deltaxyz(pow2(mA),mU32,mQ32) - 2*(mQ32 - mU32)*(mQ32 - mU32 + pow2(
        mA))))/(mQ32*pow3(mQ32 - mU32)) - (2*(3*mQ32 + mU32 - 2*pow2(mA))*pow2(
        log(mQ32)))/pow3(mQ32 - mU32)) + 3*Xb*Yb*(log(mD32)*((-2*log(mQ32)*(2*
        mD32 + mQ32 + mU32 - 2*pow2(mA)))/((mD32 - mQ32)*pow3(mQ32 - mU32)) + (
        2*log(mU32)*(2*mD32 + mQ32 + mU32 - 2*pow2(mA)))/((mD32 - mQ32)*pow3(
        mQ32 - mU32))) - (2*log(mQ32)*log(mU32)*(3*mQ32 + mU32 - 2*pow2(mA)))/(
        (mD32 - mQ32)*pow3(mQ32 - mU32)) + (2*phixyz(pow2(mA),mQ32,mQ32)*(2*
        deltaxyz(pow2(mA),mQ32,mQ32) + (mQ32 - mU32)*pow2(mA)))/((mD32 - mQ32)*
        mQ32*pow3(mQ32 - mU32)) - (2*phixyz(pow2(mA),mQ32,mD32)*(2*deltaxyz(
        pow2(mA),mQ32,mD32) + (mQ32 - mU32)*(mD32 - mQ32 + pow2(mA))))/(mD32*(
        mD32 - mQ32)*pow3(mQ32 - mU32)) + (phixyz(pow2(mA),mU32,mD32)*(4*
        deltaxyz(pow2(mA),mU32,mD32) + 2*(-mQ32 + mU32)*(mD32 - mU32 + pow2(mA)
        )))/(mD32*(mD32 - mQ32)*pow3(mQ32 - mU32)) + (2*phixyz(pow2(mA),mU32,
        mQ32)*(-2*deltaxyz(pow2(mA),mU32,mQ32) + (mQ32 - mU32)*(mQ32 - mU32 +
        pow2(mA))))/((mD32 - mQ32)*mQ32*pow3(mQ32 - mU32)) + (2*(3*mQ32 + mU32
        - 2*pow2(mA))*pow2(log(mQ32)))/((mD32 - mQ32)*pow3(mQ32 - mU32)) + log(
        pow2(mA))*((4*log(mQ32))/pow3(mQ32 - mU32) + (4*log(mU32))/pow3(-mQ32 +
        mU32)))))*pow3(Xt) + pow2(Xt)*(3*Xb*Yb*(log(pow2(mA))*((2*log(mQ32))/
        pow2(mQ32 - mU32) - (2*log(mU32))/pow2(mQ32 - mU32)) + log(mD32)*((-2*
        log(mQ32)*(mD32 + mU32 - pow2(mA)))/((mD32 - mQ32)*pow2(mQ32 - mU32)) +
        (2*log(mU32)*(mD32 + mU32 - pow2(mA)))/((mD32 - mQ32)*pow2(mQ32 - mU32)
        )) + (2*deltaxyz(pow2(mA),mU32,mD32)*phixyz(pow2(mA),mU32,mD32))/(mD32*
        (mD32 - mQ32)*pow2(mQ32 - mU32)) - (2*deltaxyz(pow2(mA),mU32,mQ32)*
        phixyz(pow2(mA),mU32,mQ32))/((mD32 - mQ32)*mQ32*pow2(mQ32 - mU32)) - (
        2*log(mQ32)*log(mU32)*(mQ32 + mU32 - pow2(mA)))/((mD32 - mQ32)*pow2(
        mQ32 - mU32)) + (2*phixyz(pow2(mA),mQ32,mQ32)*(deltaxyz(pow2(mA),mQ32,
        mQ32) + (mQ32 - mU32)*pow2(mA)))/((mD32 - mQ32)*mQ32*pow2(mQ32 - mU32))
        - (2*phixyz(pow2(mA),mQ32,mD32)*(deltaxyz(pow2(mA),mQ32,mD32) + (mQ32 -
        mU32)*(mD32 - mQ32 + pow2(mA))))/(mD32*(mD32 - mQ32)*pow2(mQ32 - mU32))
        + (2*(mQ32 + mU32 - pow2(mA))*pow2(log(mQ32)))/((mD32 - mQ32)*pow2(mQ32
        - mU32))) + 3*Xb*Yt*(log(pow2(mA))*((-2*log(mQ32))/pow2(mQ32 - mU32) +
        (2*log(mU32))/pow2(mQ32 - mU32)) + log(mD32)*((2*log(mQ32)*(mD32 + mQ32
        - pow2(mA)))/((mD32 - mQ32)*pow2(mQ32 - mU32)) + (2*log(mU32)*(-mD32 -
        mQ32 + pow2(mA)))/((mD32 - mQ32)*pow2(mQ32 - mU32))) + (2*deltaxyz(
        pow2(mA),mQ32,mD32)*phixyz(pow2(mA),mQ32,mD32))/(mD32*(mD32 - mQ32)*
        pow2(mQ32 - mU32)) - (2*deltaxyz(pow2(mA),mQ32,mQ32)*phixyz(pow2(mA),
        mQ32,mQ32))/((mD32 - mQ32)*mQ32*pow2(mQ32 - mU32)) + (2*log(mQ32)*log(
        mU32)*(-2*mQ32 + pow2(mA)))/((-mD32 + mQ32)*pow2(mQ32 - mU32)) + (2*
        phixyz(pow2(mA),mU32,mD32)*(-deltaxyz(pow2(mA),mU32,mD32) + (mQ32 -
        mU32)*(mD32 - mU32 + pow2(mA))))/(mD32*(mD32 - mQ32)*pow2(mQ32 - mU32))
        + (phixyz(pow2(mA),mU32,mQ32)*(2*deltaxyz(pow2(mA),mU32,mQ32) - 2*(mQ32
        - mU32)*(mQ32 - mU32 + pow2(mA))))/((mD32 - mQ32)*mQ32*pow2(mQ32 -
        mU32)) + (2*(-2*mQ32 + pow2(mA))*pow2(log(mQ32)))/((mD32 - mQ32)*pow2(
        mQ32 - mU32))) + 3*(8/(mQ32 - mU32) + (12*dilog(1 - mQ32/mU32))/(mQ32 -
        mU32) + log(pow2(mA))*((-2*(mD32 + 2*mQ32 - 3*mU32)*log(mQ32))/pow2(
        mQ32 - mU32) + (2*(mD32 + 2*mQ32 - 3*mU32)*log(mU32))/pow2(mQ32 - mU32)
        ) + lMR*(4/(mQ32 - mU32) + ((-8*mQ32 + 12*mU32)*log(mU32))/pow2(mQ32 -
        mU32)) + log(mD32)*((2*log(mQ32)*(mD32 + mQ32 - pow2(mA)))/pow2(mQ32 -
        mU32) + (2*log(mU32)*(-mD32 - mQ32 + pow2(mA)))/pow2(mQ32 - mU32)) +
        log(mQ32)*((2*(mQ32 - 5*mU32))/pow2(mQ32 - mU32) + (4*lMR*(2*mQ32 - 3*
        mU32))/pow2(mQ32 - mU32) - (2*log(mU32)*(3*mQ32 - 3*mU32 + pow2(mA)))/
        pow2(mQ32 - mU32)) + ((-6*mQ32 + 14*mU32)*log(mU32))/pow2(mQ32 - mU32)
        + (2*deltaxyz(pow2(mA),mQ32,mD32)*phixyz(pow2(mA),mQ32,mD32))/(mD32*
        pow2(mQ32 - mU32)) + (2*deltaxyz(pow2(mA),mU32,mQ32)*phixyz(pow2(mA),
        mU32,mQ32))/(mQ32*pow2(mQ32 - mU32)) - (2*phixyz(pow2(mA),mQ32,mQ32)*(
        deltaxyz(pow2(mA),mQ32,mQ32) + (mQ32 - mU32)*pow2(mA)))/(mQ32*pow2(mQ32
        - mU32)) + (2*phixyz(pow2(mA),mU32,mD32)*(-deltaxyz(pow2(mA),mU32,mD32)
        + (mQ32 - mU32)*(mD32 - mU32 + pow2(mA))))/(mD32*pow2(mQ32 - mU32)) + (
        2*pow2(mA)*pow2(log(mQ32)))/pow2(mQ32 - mU32) + (6*pow2(log(mU32)))/(
        mQ32 - mU32)) + 3*pow2(Xb)*((6*(-2*mD32 + mQ32 + mU32)*dilog(1 - mQ32/
        mU32))/((mQ32 - mU32)*pow2(mD32 - mQ32)) + log(mD32)*((2*mD32)/((mD32 -
        mQ32)*mQ32*(mQ32 - mU32)) + (4*mD32*(mD32 - mU32)*log(mQ32))/(pow2(mD32
        - mQ32)*pow2(mQ32 - mU32)) + (4*mD32*(-mD32 + mU32)*log(mU32))/(pow2(
        mD32 - mQ32)*pow2(mQ32 - mU32))) + (6*(mD32 + mQ32 - 2*mU32)*dilog(1 -
        mD32/mQ32))/((-mD32 + mQ32)*pow2(mQ32 - mU32)) - (2*(mD32 - mU32)*log(
        mU32))/((mD32 - mQ32)*pow2(mQ32 - mU32)) + (6*dilog(1 - mD32/mU32)*
        pow2(mD32 - mU32))/(pow2(mD32 - mQ32)*pow2(mQ32 - mU32)) + (3*pow2(log(
        mU32)))/pow2(mQ32 - mU32) + log(mQ32)*(2/pow2(mQ32 - mU32) + (2*lMR)/
        pow2(mQ32 - mU32) + (log(mU32)*(8*mD32*mQ32 - 4*mD32*mU32 - 4*pow4(mQ3)
        ))/(pow2(mD32 - mQ32)*pow2(mQ32 - mU32))) - 2/(-(mQ32*mU32) + pow4(mQ3)
        ) + (pow2(log(mQ32))*(-2*mD32*mQ32 + 4*mD32*mU32 - 3*pow4(mD3) + pow4(
        mQ3)))/(pow2(mD32 - mQ32)*pow2(mQ32 - mU32)) + lMR*((-2*log(mU32))/
        pow2(mQ32 - mU32) - 2/(-(mQ32*mU32) + pow4(mQ3))))) + Xt*(3*Yb*((4/(-
        mQ32 + mU32) + (4*lMR)/(-mQ32 + mU32))*log(mQ32) + (4*log(mU32))/(mQ32
        - mU32) + (4*lMR*log(mU32))/(mQ32 - mU32) + log(mD32)*((2*log(mQ32))/(
        mQ32 - mU32) + (2*log(mU32))/(-mQ32 + mU32)) + ((2*log(mQ32))/(mQ32 -
        mU32) + (2*log(mU32))/(-mQ32 + mU32))*log(pow2(mA)) + (2*phixyz(pow2(
        mA),mQ32,mD32)*(mD32 - mQ32 + pow2(mA)))/(mD32*(-mQ32 + mU32)) + (2*
        phixyz(pow2(mA),mU32,mD32)*(mD32 - mU32 + pow2(mA)))/(mD32*(mQ32 -
        mU32))) + 3*Xb*(((2*log(mD32))/(-mD32 + mQ32) + 2*(1/(mD32 - mQ32) + 1/
        (-mQ32 + mU32))*log(mQ32) + (2*log(mU32))/(mQ32 - mU32))*log(pow2(mA))
        - (2*deltaxyz(pow2(mA),mQ32,mQ32)*phixyz(pow2(mA),mQ32,mQ32))/((mD32 -
        mQ32)*mQ32*(mQ32 - mU32)) + (2*deltaxyz(pow2(mA),mU32,mQ32)*phixyz(
        pow2(mA),mU32,mQ32))/((mD32 - mQ32)*mQ32*(mQ32 - mU32)) + log(mD32)*((
        2*log(mQ32)*(mD32 + mQ32 - pow2(mA)))/((mD32 - mQ32)*(mQ32 - mU32)) - (
        2*log(mU32)*(mD32 + mU32 - pow2(mA)))/((mD32 - mQ32)*(mQ32 - mU32))) +
        (2*log(mQ32)*log(mU32)*(mQ32 + mU32 - pow2(mA)))/((mD32 - mQ32)*(mQ32 -
        mU32)) + (2*(-2*mQ32 + pow2(mA))*pow2(log(mQ32)))/((mD32 - mQ32)*(mQ32
        - mU32)) + (2*deltaxyz(pow2(mA),mQ32,mD32)*phixyz(pow2(mA),mQ32,mD32))/
        ((mQ32 - mU32)*(-(mD32*mQ32) + pow4(mD3))) - (2*deltaxyz(pow2(mA),mU32,
        mD32)*phixyz(pow2(mA),mU32,mD32))/((mQ32 - mU32)*(-(mD32*mQ32) + pow4(
        mD3)))) + Yt*(3*Xb*Yb*((2*log(mQ32)*log(mU32))/((mD32 - mQ32)*(mQ32 -
        mU32)) + log(mD32)*((2*log(mQ32))/((mD32 - mQ32)*(mQ32 - mU32)) + (2*
        log(mU32))/((mD32 - mQ32)*(-mQ32 + mU32))) + (2*phixyz(pow2(mA),mQ32,
        mQ32)*pow2(mA))/((mD32 - mQ32)*mQ32*(mQ32 - mU32)) + (2*phixyz(pow2(mA)
        ,mU32,mD32)*(mD32 - mU32 + pow2(mA)))/(mD32*(mD32 - mQ32)*(mQ32 - mU32)
        ) + (2*phixyz(pow2(mA),mU32,mQ32)*(mQ32 - mU32 + pow2(mA)))/(mQ32*(-
        mD32 + mQ32)*(mQ32 - mU32)) + (2*pow2(log(mQ32)))/((mD32 - mQ32)*(-mQ32
        + mU32)) - (2*phixyz(pow2(mA),mQ32,mD32)*(mD32 - mQ32 + pow2(mA)))/((
        mQ32 - mU32)*(-(mD32*mQ32) + pow4(mD3)))) + 3*((4*log(mU32))/(mQ32 -
        mU32) + (4*lMR*log(mU32))/(mQ32 - mU32) + log(mQ32)*(4/(-mQ32 + mU32) +
        (4*lMR)/(-mQ32 + mU32) + (2*log(mU32))/(-mQ32 + mU32)) + ((2*log(mQ32))
        /(mQ32 - mU32) + (2*log(mU32))/(-mQ32 + mU32))*log(pow2(mA)) + (2*
        phixyz(pow2(mA),mU32,mQ32)*(mQ32 - mU32 + pow2(mA)))/(mQ32*(mQ32 -
        mU32)) + (2*pow2(log(mQ32)))/(mQ32 - mU32) - (2*phixyz(pow2(mA),mQ32,
        mQ32)*pow2(mA))/(-(mQ32*mU32) + pow4(mQ3))))) + (3*(-4*dilog(1 - pow2(
        Mu)/mQ32) - 4*dilog(1 - pow2(Mu)/mU32) + 2*lMR*log(mU32) - 2*pow2(lMR)
        + 4*log(pow2(Mu))*(1/(mQ32 - pow2(Mu)) + 1/(mU32 - pow2(Mu)))*pow2(Mu)
        + (4*log(mU32)*pow2(Mu))/(-mU32 + pow2(Mu)) + log(mQ32)*(2*lMR + 2*log(
        mU32) + (4*pow2(Mu))/(-mQ32 + pow2(Mu))) - 2*pow2(log(mQ32)) - 2*pow2(
        log(mU32))) + 3*pow2(Xb)*(4/(-mD32 + mQ32) + (2*lMR)/(-mD32 + mQ32) + (
        2*log(mU32))/(-mD32 + mQ32) + log(mD32)*((4*mD32)/pow2(mD32 - mQ32) + (
        2*lMR*mD32)/pow2(mD32 - mQ32) + (2*mD32*log(mU32))/pow2(mD32 - mQ32)) -
        (4*mD32*dilog(1 - pow2(Mu)/mD32))/pow2(mD32 - mQ32) + (4*mD32*dilog(1 -
        pow2(Mu)/mQ32))/pow2(mD32 - mQ32) + (4*log(pow2(Mu))*pow2(Mu))/((mD32 -
        mQ32)*(-mQ32 + pow2(Mu))) - (2*mD32*pow2(log(mD32)))/pow2(mD32 - mQ32)
        + (2*mD32*pow2(log(mQ32)))/pow2(mD32 - mQ32) + log(mQ32)*((-2*lMR*mD32)
        /pow2(mD32 - mQ32) - (2*mD32*log(mU32))/pow2(mD32 - mQ32) + (4*(mD32*
        pow2(Mu) - pow4(mQ3)))/(pow2(mD32 - mQ32)*(mQ32 - pow2(Mu))))) + pow2(
        Xt)*(3*pow2(Xb)*(2/((mD32 - mQ32)*(mQ32 - mU32)) + log(mD32)*((2*mD32)/
        ((-mQ32 + mU32)*pow2(mD32 - mQ32)) + (2*mD32*mQ32*log(mQ32))/(pow2(mD32
        - mQ32)*pow2(mQ32 - mU32)) - (2*mD32*mQ32*log(mU32))/(pow2(mD32 - mQ32)
        *pow2(mQ32 - mU32))) + (2*mQ32*log(mU32))/((mD32 - mQ32)*pow2(mQ32 -
        mU32)) - (2*mD32*mQ32*pow2(log(mQ32)))/(pow2(mD32 - mQ32)*pow2(mQ32 -
        mU32)) + log(mQ32)*((2*mD32*mQ32*log(mU32))/(pow2(mD32 - mQ32)*pow2(
        mQ32 - mU32)) + (2*(-(mD32*mU32) + pow4(mQ3)))/(pow2(mD32 - mQ32)*pow2(
        mQ32 - mU32)))) + 3*(4/(-mQ32 + mU32) + (4*log(pow2(Mu))*pow2(Mu))/((
        mQ32 - mU32)*(-mU32 + pow2(Mu))) + lMR*(2/(-mQ32 + mU32) - (2*mQ32*log(
        mU32))/pow2(mQ32 - mU32)) + log(mQ32)*((2*lMR*mQ32)/pow2(mQ32 - mU32) +
        (2*(mQ32 + mU32))/pow2(mQ32 - mU32) - (2*mQ32*log(mU32))/pow2(mQ32 -
        mU32)) - (4*mQ32*dilog(1 - pow2(Mu)/mQ32))/pow2(mQ32 - mU32) + (4*mQ32*
        dilog(1 - pow2(Mu)/mU32))/pow2(mQ32 - mU32) + (2*mQ32*pow2(log(mU32)))/
        pow2(mQ32 - mU32) + (log(mU32)*(-4*mQ32*pow2(Mu) + 4*pow4(mU3)))/((-
        mU32 + pow2(Mu))*pow2(mQ32 - mU32)))) + pow2(cbeta)*(3*Xb*Yt*((2*log(
        mD32)*log(mU32))/(mD32 - mQ32) + (2*log(mQ32)*log(mU32))/(-mD32 + mQ32)
        + ((2*log(mD32))/(-mD32 + mQ32) + (2*log(mQ32))/(mD32 - mQ32))*log(
        pow2(mA)) - (2*phixyz(pow2(mA),mU32,mD32)*(mD32 - mU32 + pow2(mA)))/(
        mD32*(mD32 - mQ32)) + (2*phixyz(pow2(mA),mU32,mQ32)*(mQ32 - mU32 +
        pow2(mA)))/((mD32 - mQ32)*mQ32)) + 3*pow2(Xb)*(4/(mD32 - mQ32) + (2*
        lMR)/(mD32 - mQ32) + log(mQ32)*((2*lMR*mD32)/pow2(mD32 - mQ32) + (2*(
        mD32 + mQ32))/pow2(mD32 - mQ32)) + log(mD32)*((-4*mD32)/pow2(mD32 -
        mQ32) - (2*lMR*mD32)/pow2(mD32 - mQ32) + (log(mQ32)*(mD32 + mQ32 -
        pow2(mA)))/pow2(mD32 - mQ32)) + log(pow2(mA))*((log(mD32)*(mD32 - mQ32
        + pow2(mA)))/pow2(mD32 - mQ32) - (log(mQ32)*(mD32 - mQ32 + pow2(mA)))/
        pow2(mD32 - mQ32)) + (deltaxyz(pow2(mA),mQ32,mD32)*phixyz(pow2(mA),
        mQ32,mD32))/(mD32*pow2(mD32 - mQ32)) - ((mD32 + mQ32 - pow2(mA))*pow2(
        log(mQ32)))/pow2(mD32 - mQ32) + (phixyz(pow2(mA),mQ32,mQ32)*((mD32 - 2*
        mQ32)*deltaxyz(pow2(mA),mQ32,mQ32) + (-mD32 + mQ32)*pow2(mA)*(-5*mQ32 +
        pow2(mA))))/(pow2(mD32 - mQ32)*pow4(mQ3))) + 3*((-2 - 2*lMR)*log(mQ32)
        - 2*log(mU32) - 2*lMR*log(mU32) + log(mD32)*log(mU32) + (4 - log(mD32)
        + log(mU32))*log(pow2(mA)) + 2*pow2(lMR) - (phixyz(pow2(mA),mU32,mD32)*
        (mD32 - mU32 + pow2(mA)))/mD32 + pow2(log(mQ32)) + (phixyz(pow2(mA),
        mQ32,mQ32)*(-deltaxyz(pow2(mA),mQ32,mQ32) - 5*mQ32*pow2(mA) + pow4(mA))
        )/pow4(mQ3)) + pow2(Yt)*(3*((-2*log(mQ32)*(mQ32 - mU32 + pow2(mA)))/
        deltaxyz(pow2(mA),mU32,mQ32) + (log(pow2(mA))*(-deltaxyz(pow2(mA),mU32,
        mQ32) + pow2(mQ32 - mU32 + pow2(mA))))/(mQ32*deltaxyz(pow2(mA),mU32,
        mQ32)) + (phixyz(pow2(mA),mU32,mQ32)*(-mU32 + pow2(mA) + ((mQ32 - mU32
        + pow2(mA))*((mQ32 - mU32)*mU32 + (mQ32 + 2*mU32)*pow2(mA) - pow4(mA)))
        /deltaxyz(pow2(mA),mU32,mQ32)))/pow4(mQ3) + (log(mU32)*(deltaxyz(pow2(
        mA),mU32,mQ32) + 2*mU32*pow2(mA) - pow4(mA) + pow4(mQ3) - pow4(mU3)))/(
        mQ32*deltaxyz(pow2(mA),mU32,mQ32))) + 3*pow2(Xb)*(log(mQ32)*((2*(mQ32 -
        mU32 + pow2(mA)))/((mD32 - mQ32)*deltaxyz(pow2(mA),mU32,mQ32)) - log(
        mU32)/pow2(mD32 - mQ32)) + (log(mD32)*log(mU32))/pow2(mD32 - mQ32) - (
        phixyz(pow2(mA),mU32,mD32)*(mD32 - mU32 + pow2(mA)))/(mD32*pow2(mD32 -
        mQ32)) + log(pow2(mA))*(-(log(mD32)/pow2(mD32 - mQ32)) + log(mQ32)/
        pow2(mD32 - mQ32) + (deltaxyz(pow2(mA),mU32,mQ32) - pow2(mQ32 - mU32 +
        pow2(mA)))/((mD32 - mQ32)*mQ32*deltaxyz(pow2(mA),mU32,mQ32))) + (
        phixyz(pow2(mA),mU32,mQ32)*((mD32 - mQ32)*(mQ32 - mU32 + pow2(mA))*(
        mU32*(-mQ32 + mU32) - (mQ32 + 2*mU32)*pow2(mA) + pow4(mA)) + deltaxyz(
        pow2(mA),mU32,mQ32)*(mD32*mU32 - 2*mQ32*mU32 - mD32*pow2(mA) + 2*mQ32*
        pow2(mA) + pow4(mQ3))))/(deltaxyz(pow2(mA),mU32,mQ32)*pow2(mD32 - mQ32)
        *pow4(mQ3)) + (log(mU32)*(deltaxyz(pow2(mA),mU32,mQ32) + 2*mU32*pow2(
        mA) - pow4(mA) + pow4(mQ3) - pow4(mU3)))/(mQ32*(-mD32 + mQ32)*deltaxyz(
        pow2(mA),mU32,mQ32)))) + pow2(Xt)*(3*(4/(mQ32 - mU32) + log(mQ32)*((-4*
        mQ32)/pow2(mQ32 - mU32) - (2*lMR*mQ32)/pow2(mQ32 - mU32)) + lMR*(2/(
        mQ32 - mU32) + (2*mQ32*log(mU32))/pow2(mQ32 - mU32)) + log(mD32)*((log(
        mQ32)*(mD32 + mQ32 - pow2(mA)))/pow2(mQ32 - mU32) - (log(mU32)*(mD32 +
        mQ32 - pow2(mA)))/pow2(mQ32 - mU32)) + log(pow2(mA))*((log(mQ32)*(-mD32
        + mQ32 + pow2(mA)))/pow2(mQ32 - mU32) - (log(mU32)*(-mD32 + mQ32 +
        pow2(mA)))/pow2(mQ32 - mU32)) + (2*(mQ32 + mU32)*log(mU32))/pow2(mQ32 -
        mU32) + (deltaxyz(pow2(mA),mQ32,mD32)*phixyz(pow2(mA),mQ32,mD32))/(
        mD32*pow2(mQ32 - mU32)) + (phixyz(pow2(mA),mU32,mD32)*(-deltaxyz(pow2(
        mA),mU32,mD32) + (mQ32 - mU32)*(mD32 - mU32 + pow2(mA))))/(mD32*pow2(
        mQ32 - mU32))) + 3*Xb*Yt*(log(pow2(mA))*((-2*log(mQ32))/pow2(mQ32 -
        mU32) + (2*log(mU32))/pow2(mQ32 - mU32)) + log(mD32)*((2*log(mQ32)*(
        mD32 + mQ32 - pow2(mA)))/((mD32 - mQ32)*pow2(mQ32 - mU32)) + (2*log(
        mU32)*(-mD32 - mQ32 + pow2(mA)))/((mD32 - mQ32)*pow2(mQ32 - mU32))) + (
        2*deltaxyz(pow2(mA),mQ32,mD32)*phixyz(pow2(mA),mQ32,mD32))/(mD32*(mD32
        - mQ32)*pow2(mQ32 - mU32)) - (2*deltaxyz(pow2(mA),mQ32,mQ32)*phixyz(
        pow2(mA),mQ32,mQ32))/((mD32 - mQ32)*mQ32*pow2(mQ32 - mU32)) + (2*log(
        mQ32)*log(mU32)*(-2*mQ32 + pow2(mA)))/((-mD32 + mQ32)*pow2(mQ32 - mU32)
        ) + (2*phixyz(pow2(mA),mU32,mD32)*(-deltaxyz(pow2(mA),mU32,mD32) + (
        mQ32 - mU32)*(mD32 - mU32 + pow2(mA))))/(mD32*(mD32 - mQ32)*pow2(mQ32 -
        mU32)) + (phixyz(pow2(mA),mU32,mQ32)*(2*deltaxyz(pow2(mA),mU32,mQ32) -
        2*(mQ32 - mU32)*(mQ32 - mU32 + pow2(mA))))/((mD32 - mQ32)*mQ32*pow2(
        mQ32 - mU32)) + (2*(-2*mQ32 + pow2(mA))*pow2(log(mQ32)))/((mD32 - mQ32)
        *pow2(mQ32 - mU32))) + pow2(Yt)*(3*(log(mQ32)*((2*(mQ32 - mU32 + pow2(
        mA)))/((mQ32 - mU32)*deltaxyz(pow2(mA),mU32,mQ32)) - log(mU32)/pow2(
        mQ32 - mU32)) + pow2(log(mQ32))/pow2(mQ32 - mU32) + log(pow2(mA))*(-(
        log(mQ32)/pow2(mQ32 - mU32)) + log(mU32)/pow2(mQ32 - mU32) + (deltaxyz(
        pow2(mA),mU32,mQ32) - pow2(mQ32 - mU32 + pow2(mA)))/(mQ32*(mQ32 - mU32)
        *deltaxyz(pow2(mA),mU32,mQ32))) + (phixyz(pow2(mA),mQ32,mQ32)*(-
        deltaxyz(pow2(mA),mQ32,mQ32) + pow2(mA)*(-5*mQ32 + pow2(mA))))/(pow2(
        mQ32 - mU32)*pow4(mQ3)) + (phixyz(pow2(mA),mU32,mQ32)*(deltaxyz(pow2(
        mA),mU32,mQ32)*(deltaxyz(pow2(mA),mU32,mQ32) + (2*mQ32 + 3*mU32)*pow2(
        mA) - 2*pow2(mQ32 - mU32) - pow4(mA)) + (mQ32 - mU32)*(mQ32 - mU32 +
        pow2(mA))*(mU32*(-mQ32 + mU32) - (mQ32 + 2*mU32)*pow2(mA) + pow4(mA))))
        /(deltaxyz(pow2(mA),mU32,mQ32)*pow2(mQ32 - mU32)*pow4(mQ3)) + (log(
        mU32)*(-deltaxyz(pow2(mA),mU32,mQ32) - 2*mU32*pow2(mA) + pow4(mA) -
        pow4(mQ3) + pow4(mU3)))/(mQ32*(mQ32 - mU32)*deltaxyz(pow2(mA),mU32,
        mQ32))) + 3*pow2(Xb)*(log(mD32)*((log(mQ32)*(mD32 + mQ32 - pow2(mA)))/(
        pow2(mD32 - mQ32)*pow2(mQ32 - mU32)) - (log(mU32)*(mD32 + mQ32 - pow2(
        mA)))/(pow2(mD32 - mQ32)*pow2(mQ32 - mU32))) + log(mQ32)*((2*(mQ32 -
        mU32 + pow2(mA)))/((-mD32 + mQ32)*(mQ32 - mU32)*deltaxyz(pow2(mA),mU32,
        mQ32)) + (log(mU32)*(mD32 + mQ32 - pow2(mA)))/(pow2(mD32 - mQ32)*pow2(
        mQ32 - mU32))) + (deltaxyz(pow2(mA),mQ32,mD32)*phixyz(pow2(mA),mQ32,
        mD32))/(mD32*pow2(mD32 - mQ32)*pow2(mQ32 - mU32)) + (phixyz(pow2(mA),
        mU32,mD32)*(-deltaxyz(pow2(mA),mU32,mD32) + (mQ32 - mU32)*(mD32 - mU32
        + pow2(mA))))/(mD32*pow2(mD32 - mQ32)*pow2(mQ32 - mU32)) - ((mD32 +
        mQ32 - pow2(mA))*pow2(log(mQ32)))/(pow2(mD32 - mQ32)*pow2(mQ32 - mU32))
        + (log(pow2(mA))*(deltaxyz(pow2(mA),mU32,mQ32) - pow2(mQ32 - mU32 +
        pow2(mA))))/(mQ32*(-mD32 + mQ32)*(mQ32 - mU32)*deltaxyz(pow2(mA),mU32,
        mQ32)) + (phixyz(pow2(mA),mQ32,mQ32)*((mD32 - 2*mQ32)*deltaxyz(pow2(mA)
        ,mQ32,mQ32) + (-mD32 + mQ32)*pow2(mA)*(-5*mQ32 + pow2(mA))))/(pow2(mD32
        - mQ32)*pow2(mQ32 - mU32)*pow4(mQ3)) + (phixyz(pow2(mA),mU32,mQ32)*((
        mD32 - mQ32)*(mQ32 - mU32)*(mQ32 - mU32 + pow2(mA))*((mQ32 - mU32)*mU32
        + (mQ32 + 2*mU32)*pow2(mA) - pow4(mA)) + deltaxyz(pow2(mA),mU32,mQ32)*(
        -((mD32 - 2*mQ32)*deltaxyz(pow2(mA),mU32,mQ32)) + (2*mD32 - 3*mQ32)*
        pow2(mQ32 - mU32) + (mD32 - mQ32)*pow4(mA) + pow2(mA)*(-2*mD32*mQ32 -
        3*mD32*mU32 + 4*mQ32*mU32 + pow4(mQ3)))))/(deltaxyz(pow2(mA),mU32,mQ32)
        *pow2(mD32 - mQ32)*pow2(mQ32 - mU32)*pow4(mQ3)) + (log(mU32)*(-
        deltaxyz(pow2(mA),mU32,mQ32) - 2*mU32*pow2(mA) + pow4(mA) - pow4(mQ3) +
        pow4(mU3)))/(mQ32*(-mD32 + mQ32)*(mQ32 - mU32)*deltaxyz(pow2(mA),mU32,
        mQ32))))) + Xt*(3*Xb*(((2*log(mD32))/(-mD32 + mQ32) + 2*(1/(mD32 -
        mQ32) + 1/(-mQ32 + mU32))*log(mQ32) + (2*log(mU32))/(mQ32 - mU32))*log(
        pow2(mA)) - (2*deltaxyz(pow2(mA),mQ32,mQ32)*phixyz(pow2(mA),mQ32,mQ32))
        /((mD32 - mQ32)*mQ32*(mQ32 - mU32)) + (2*deltaxyz(pow2(mA),mU32,mQ32)*
        phixyz(pow2(mA),mU32,mQ32))/((mD32 - mQ32)*mQ32*(mQ32 - mU32)) + log(
        mD32)*((2*log(mQ32)*(mD32 + mQ32 - pow2(mA)))/((mD32 - mQ32)*(mQ32 -
        mU32)) - (2*log(mU32)*(mD32 + mU32 - pow2(mA)))/((mD32 - mQ32)*(mQ32 -
        mU32))) + (2*log(mQ32)*log(mU32)*(mQ32 + mU32 - pow2(mA)))/((mD32 -
        mQ32)*(mQ32 - mU32)) + (2*(-2*mQ32 + pow2(mA))*pow2(log(mQ32)))/((mD32
        - mQ32)*(mQ32 - mU32)) + (2*deltaxyz(pow2(mA),mQ32,mD32)*phixyz(pow2(
        mA),mQ32,mD32))/((mQ32 - mU32)*(-(mD32*mQ32) + pow4(mD3))) - (2*
        deltaxyz(pow2(mA),mU32,mD32)*phixyz(pow2(mA),mU32,mD32))/((mQ32 - mU32)
        *(-(mD32*mQ32) + pow4(mD3)))) + Yt*(3*((2*log(mQ32)*log(mU32))/(-mQ32 +
        mU32) + ((2*log(mQ32))/(-mQ32 + mU32) + (2*log(mU32))/(mQ32 - mU32))*
        log(pow2(mA)) + (2*pow2(log(mQ32)))/(mQ32 - mU32) - (2*phixyz(pow2(mA),
        mQ32,mQ32)*(deltaxyz(pow2(mA),mQ32,mQ32) - pow2(mA)*(-5*mQ32 + pow2(mA)
        )))/((mQ32 - mU32)*pow4(mQ3)) - (2*phixyz(pow2(mA),mU32,mQ32)*(-3*mQ32*
        mU32 - deltaxyz(pow2(mA),mU32,mQ32) - 3*mQ32*pow2(mA) - 2*mU32*pow2(mA)
        + pow4(mA) + 2*pow4(mQ3) + pow4(mU3)))/((mQ32 - mU32)*pow4(mQ3))) + 3*
        pow2(Xb)*(log(pow2(mA))*((-2*log(mD32))/pow2(mD32 - mQ32) + (2*log(
        mQ32))/pow2(mD32 - mQ32)) + log(mD32)*((2*log(mQ32)*(mD32 + mQ32 -
        pow2(mA)))/((mQ32 - mU32)*pow2(mD32 - mQ32)) - (2*log(mU32)*(mD32 +
        mU32 - pow2(mA)))/((mQ32 - mU32)*pow2(mD32 - mQ32))) + (2*deltaxyz(
        pow2(mA),mQ32,mD32)*phixyz(pow2(mA),mQ32,mD32))/(mD32*(mQ32 - mU32)*
        pow2(mD32 - mQ32)) - (2*deltaxyz(pow2(mA),mU32,mD32)*phixyz(pow2(mA),
        mU32,mD32))/(mD32*(mQ32 - mU32)*pow2(mD32 - mQ32)) + (2*log(mQ32)*log(
        mU32)*(mD32 + mU32 - pow2(mA)))/((mQ32 - mU32)*pow2(mD32 - mQ32)) + (2*
        (-mD32 - mQ32 + pow2(mA))*pow2(log(mQ32)))/((mQ32 - mU32)*pow2(mD32 -
        mQ32)) + (2*phixyz(pow2(mA),mQ32,mQ32)*((mD32 - 2*mQ32)*deltaxyz(pow2(
        mA),mQ32,mQ32) + (-mD32 + mQ32)*pow2(mA)*(-5*mQ32 + pow2(mA))))/((mQ32
        - mU32)*pow2(mD32 - mQ32)*pow4(mQ3)) + (2*phixyz(pow2(mA),mU32,mQ32)*(-
        ((mD32 - 2*mQ32)*deltaxyz(pow2(mA),mU32,mQ32)) + (mD32 - mQ32)*(-3*
        mQ32*mU32 - 3*mQ32*pow2(mA) - 2*mU32*pow2(mA) + pow4(mA) + 2*pow4(mQ3)
        + pow4(mU3))))/((mQ32 - mU32)*pow2(mD32 - mQ32)*pow4(mQ3)))))))/pow2(
        sbeta) + (3*(24/pow2(mQ32 - mU32) + (6*dilog(1 - mQ32/mU32))/pow2(mQ32
        - mU32) + (3*pow2(log(mU32)))/pow2(mQ32 - mU32) + log(mQ32)*((-8*lMR*(
        mQ32 + mU32))/pow3(mQ32 - mU32) - (2*(7*mQ32 + 5*mU32))/pow3(mQ32 -
        mU32) + (2*(-5*mQ32 + mU32)*log(mU32))/pow3(mQ32 - mU32)) + log(pow2(
        mA))*(-8/pow2(mQ32 - mU32) + (4*(mQ32 + mU32)*log(mQ32))/pow3(mQ32 -
        mU32) - (4*(mQ32 + mU32)*log(mU32))/pow3(mQ32 - mU32)) + lMR*(16/pow2(
        mQ32 - mU32) + (8*(mQ32 + mU32)*log(mU32))/pow3(mQ32 - mU32)) + (6*(
        mQ32 + 3*mU32)*log(mU32))/pow3(mQ32 - mU32) + ((7*mQ32 + mU32)*pow2(
        log(mQ32)))/pow3(mQ32 - mU32)) + 3*pow2(Xb)*((-6*dilog(1 - mQ32/mU32))/
        ((mD32 - mQ32)*pow2(mQ32 - mU32)) + (11*mQ32 + mU32)/(mQ32*pow3(mQ32 -
        mU32)) + lMR*((5*mQ32 + mU32)/(mQ32*pow3(mQ32 - mU32)) + (2*(mQ32 + 2*
        mU32)*log(mU32))/pow4(mQ32 - mU32)) + log(mD32)*(mD32/((mD32 - mQ32)*
        mQ32*pow2(mQ32 - mU32)) + (2*mD32*(mD32 - mU32)*(3*mD32 - 2*mQ32 -
        mU32)*log(mQ32))/(pow2(mD32 - mQ32)*pow4(mQ32 - mU32)) - (2*mD32*(mD32
        - mU32)*(3*mD32 - 2*mQ32 - mU32)*log(mU32))/(pow2(mD32 - mQ32)*pow4(
        mQ32 - mU32))) + log(mQ32)*((2*log(mU32)*(5*mD32*mQ32 - mD32*mU32 - 4*
        pow4(mQ3)))/(pow2(mD32 - mQ32)*pow3(mQ32 - mU32)) - (2*lMR*(mQ32 + 2*
        mU32))/pow4(mQ32 - mU32) + (10*mQ32*mU32 - 4*mD32*(2*mQ32 + mU32) + 5*
        pow4(mQ3) - 3*pow4(mU3))/((mD32 - mQ32)*pow4(mQ32 - mU32))) + (2*(mU32*
        (-7*mQ32 + mU32) + mD32*(mQ32 + 5*mU32))*log(mU32))/((mD32 - mQ32)*
        pow4(mQ32 - mU32)) - (6*dilog(1 - mD32/mQ32)*pow2(mD32 - mU32))/((mD32
        - mQ32)*pow4(mQ32 - mU32)) + (6*dilog(1 - mD32/mU32)*pow2(mD32 - mU32))
        /((mD32 - mQ32)*pow4(mQ32 - mU32)) + (3*(mD32 + mQ32 - 2*mU32)*pow2(
        log(mU32)))/pow4(mQ32 - mU32) + (pow2(log(mQ32))*(-3*(mD32 + mQ32 - 2*
        mU32) + (2*(mQ32 - mU32)*(mD32*(-5*mQ32 + mU32) + 4*pow4(mQ3)))/pow2(
        mD32 - mQ32)))/pow4(mQ32 - mU32)))*pow4(Xt) + (3*Xb*pow2(Xt)*(log(mD32)
        *((-8*mD32*Mu*log(mQ32))/((mD32 - mQ32)*(-mQ32 + mU32)*(mD32 - pow2(Mu)
        )) - (8*mD32*Mu*log(mU32))/((mD32 - mQ32)*(mQ32 - mU32)*(mD32 - pow2(
        Mu)))) - (8*mQ32*Mu*log(mQ32)*log(mU32))/((-mD32 + mQ32)*(-mQ32 + mU32)
        *(-mQ32 + pow2(Mu))) + log(pow2(Mu))*((-8*log(mQ32)*pow3(Mu)
        )/((mQ32 - mU32)*(mD32 - pow2(Mu))*(-mQ32 + pow2(Mu))) - (8*log(mU32)*
        pow3(Mu))/((mQ32 - mU32)*(-mD32 + pow2(Mu))*(-mQ32 + pow2(
        Mu)))) - (8*mQ32*Mu*pow2(log(mQ32)))/((-mD32 + mQ32)*(mQ32 - mU32)*(-
        mQ32 + pow2(Mu)))) + 3*Xb*((-8*Mu*dilog(1 - pow2(Mu)/mD32)*(mD32 +
        pow2(Mu)))/((mD32 - mQ32)*(mD32 - pow2(Mu))) - (4*mQ32*Mu*log(mQ32)*
        log(mU32))/((-mD32 + mQ32)*(-mQ32 + pow2(Mu))) + (8*Mu*dilog(1 - pow2(
        Mu)/mQ32)*(mQ32 + pow2(Mu)))/((-mD32 + mQ32)*(-mQ32 + pow2(Mu))) + log(
        mD32)*((-4*mD32*Mu*log(mQ32))/((-mD32 + mQ32)*(mD32 - pow2(Mu))) - (4*
        mD32*Mu*log(mU32))/((-mD32 + mQ32)*(mD32 - pow2(Mu))) - (8*log(pow2(Mu)
        )*pow3(Mu))/((mD32 - mQ32)*(-mD32 + pow2(Mu)))) + log(pow2(
        Mu))*((-4*log(mU32)*pow3(Mu))/((mD32 - pow2(Mu))*(-mQ32 +
        pow2(Mu))) + (4*log(mQ32)*(mD32 + mQ32 - 2*pow2(Mu))*pow3(Mu)
        )/((mD32 - mQ32)*(mD32 - pow2(Mu))*(-mQ32 + pow2(Mu)))) - (4*Mu*(mD32
        + pow2(Mu))*pow2(log(mD32)))/((mD32 - mQ32)*(mD32 - pow2(Mu))) - (4*
        pow3(Mu)*pow2(log(mQ32)))/((mD32 - mQ32)*(-mQ32 + pow2(Mu)))
        ) + 3*Xt*((-8*Mu*dilog(1 - pow2(Mu)/mQ32))/(-mQ32 + mU32) - (8*Mu*
        dilog(1 - pow2(Mu)/mU32))/(mQ32 - mU32) + ((-8*Mu)/(mQ32 - mU32) - (8*
        lMR*Mu)/(mQ32 - mU32))*log(mQ32) - (8*Mu*log(mU32))/(-mQ32 + mU32) - (
        8*lMR*Mu*log(mU32))/(-mQ32 + mU32) - (4*Mu*pow2(log(mQ32)))/(-mQ32 +
        mU32) - (4*Mu*pow2(log(mU32)))/(mQ32 - mU32)) + 3*((-32*Mu)/pow2(mQ32 -
        mU32) + log(mQ32)*((8*lMR*Mu*(mQ32 + mU32))/pow3(mQ32 - mU32) + (8*Mu*(
        3*mQ32 + mU32))/pow3(mQ32 - mU32)) + lMR*((-16*Mu)/pow2(mQ32 - mU32) -
        (8*Mu*(mQ32 + mU32)*log(mU32))/pow3(mQ32 - mU32)) - (8*Mu*(mQ32 + 3*
        mU32)*log(mU32))/pow3(mQ32 - mU32) - (8*Mu*dilog(1 - pow2(Mu)/mQ32)*(
        mQ32 + mU32 - 2*pow2(Mu)))/pow3(mQ32 - mU32) + (8*Mu*dilog(1 - pow2(Mu)
        /mU32)*(mQ32 + mU32 - 2*pow2(Mu)))/pow3(mQ32 - mU32) - (4*Mu*(mQ32 +
        mU32 - 2*pow2(Mu))*pow2(log(mQ32)))/pow3(mQ32 - mU32) + (4*Mu*(mQ32 +
        mU32 - 2*pow2(Mu))*pow2(log(mU32)))/pow3(mQ32 - mU32) + log(pow2(Mu))*(
        (-16*log(mQ32)*pow3(Mu))/pow3(mQ32 - mU32) - (16*log(mU32)*
        pow3(Mu))/pow3(-mQ32 + mU32)))*pow3(Xt) + 3*Xb*(log(mD32)*((
        8*mD32*Mu)/((mD32 - mQ32)*(mD32 - pow2(Mu))*pow2(mQ32 - mU32)) - (4*
        mD32*Mu*(mQ32 + mU32)*log(mQ32))/((mD32 - mQ32)*(mD32 - pow2(Mu))*pow3(
        mQ32 - mU32)) + (4*mD32*Mu*(mQ32 + mU32)*log(mU32))/((mD32 - mQ32)*(
        mD32 - pow2(Mu))*pow3(mQ32 - mU32))) + log(mQ32)*((-8*mQ32*Mu)/((-mD32
        + mQ32)*(-mQ32 + pow2(Mu))*pow2(mQ32 - mU32)) - (4*mQ32*Mu*(mQ32 +
        mU32)*log(mU32))/((-mD32 + mQ32)*(-mQ32 + pow2(Mu))*pow3(mQ32 - mU32)))
        + log(pow2(Mu))*((-8*pow3(Mu))/((mD32 - pow2(Mu))*(-mQ32 +
        pow2(Mu))*pow2(mQ32 - mU32)) + (4*(mQ32 + mU32)*log(mQ32)*pow3(Mu)
        )/((mD32 - pow2(Mu))*(-mQ32 + pow2(Mu))*pow3(mQ32 - mU32)) - (
        4*(mQ32 + mU32)*log(mU32)*pow3(Mu))/((mD32 - pow2(Mu))*(-
        mQ32 + pow2(Mu))*pow3(mQ32 - mU32))) + (4*mQ32*Mu*(mQ32 + mU32)*pow2(
        log(mQ32)))/((-mD32 + mQ32)*(-mQ32 + pow2(Mu))*pow3(mQ32 - mU32)))*
        pow4(Xt))/(cbeta*sbeta) + (pow2(sbeta)*(3*Xt*Yb*((4/(-mQ32 + mU32) + (
        4*lMR)/(-mQ32 + mU32))*log(mQ32) + (4*log(mU32))/(mQ32 - mU32) + (4*
        lMR*log(mU32))/(mQ32 - mU32) + log(mD32)*((2*log(mQ32))/(mQ32 - mU32) +
        (2*log(mU32))/(-mQ32 + mU32)) + ((2*log(mQ32))/(mQ32 - mU32) + (2*log(
        mU32))/(-mQ32 + mU32))*log(pow2(mA)) + (2*phixyz(pow2(mA),mQ32,mD32)*(
        mD32 - mQ32 + pow2(mA)))/(mD32*(-mQ32 + mU32)) + (2*phixyz(pow2(mA),
        mU32,mD32)*(mD32 - mU32 + pow2(mA)))/(mD32*(mQ32 - mU32))) + 3*(0.5 + (
        0.5 + lMR)*log(mQ32) - (3*log(mU32))/2. + log(mD32)*log(mU32) - pow2(
        mA)/mQ32 - (phixyz(pow2(mA),mU32,mD32)*(mD32 - mU32 + pow2(mA)))/mD32 +
        lMR*(-log(mU32) - pow2(mA)/mQ32) + log(pow2(mA))*(-log(mD32) - log(
        mQ32) + (mQ32 + pow2(mA))/mQ32) + pow2(Pi)/3. + pow2(log(pow2(mA)))) +
        3*Yb*(-16/pow2(mQ32 - mU32) + log(mQ32)*((4*lMR*(mQ32 + mU32))/pow3(
        mQ32 - mU32) + (4*(3*mQ32 + mU32))/pow3(mQ32 - mU32)) + lMR*(-8/pow2(
        mQ32 - mU32) - (4*(mQ32 + mU32)*log(mU32))/pow3(mQ32 - mU32)) + log(
        mD32)*((-2*log(mQ32)*(2*mD32 + mQ32 + mU32 - 2*pow2(mA)))/pow3(mQ32 -
        mU32) + (2*log(mU32)*(2*mD32 + mQ32 + mU32 - 2*pow2(mA)))/pow3(mQ32 -
        mU32)) + log(pow2(mA))*((-2*log(mQ32)*(-2*mD32 + mQ32 + mU32 + 2*pow2(
        mA)))/pow3(mQ32 - mU32) + (2*log(mU32)*(-2*mD32 + mQ32 + mU32 + 2*pow2(
        mA)))/pow3(mQ32 - mU32)) - (4*(mQ32 + 3*mU32)*log(mU32))/pow3(mQ32 -
        mU32) - (2*phixyz(pow2(mA),mQ32,mD32)*(2*deltaxyz(pow2(mA),mQ32,mD32) +
        (mQ32 - mU32)*(mD32 - mQ32 + pow2(mA))))/(mD32*pow3(mQ32 - mU32)) + (
        phixyz(pow2(mA),mU32,mD32)*(4*deltaxyz(pow2(mA),mU32,mD32) + 2*(-mQ32 +
        mU32)*(mD32 - mU32 + pow2(mA))))/(mD32*pow3(mQ32 - mU32)))*pow3(Xt) +
        3*pow2(Yb)*(-(1/mQ32) - lMR/mQ32 - (log(mQ32)*(mD32 - mQ32 + pow2(mA)))
        /deltaxyz(pow2(mA),mQ32,mD32) + (phixyz(pow2(mA),mQ32,mD32)*(deltaxyz(
        pow2(mA),mQ32,mD32) - pow2(mD32 - mQ32 + pow2(mA))))/(2.*mD32*deltaxyz(
        pow2(mA),mQ32,mD32)) + (log(pow2(mA))*(deltaxyz(pow2(mA),mQ32,mD32) -
        pow2(mD32 - mQ32) + pow4(mA)))/(2.*mQ32*deltaxyz(pow2(mA),mQ32,mD32)) +
        (log(mD32)*(deltaxyz(pow2(mA),mQ32,mD32) + 2*mQ32*pow2(mA) - pow4(mA) +
        pow4(mD3) - pow4(mQ3)))/(2.*mQ32*deltaxyz(pow2(mA),mQ32,mD32))) + pow2(
        Xt)*(3*pow2(Yb)*(log(mQ32)*((2*lMR)/pow2(mQ32 - mU32) + (2*(deltaxyz(
        pow2(mA),mQ32,mD32) + (-mQ32 + mU32)*(mD32 - mQ32 + pow2(mA))))/(
        deltaxyz(pow2(mA),mQ32,mD32)*pow2(mQ32 - mU32))) - (2*log(mU32))/pow2(
        mQ32 - mU32) - (phixyz(pow2(mA),mU32,mD32)*(mD32 - mU32 + pow2(mA)))/(
        mD32*pow2(mQ32 - mU32)) + (phixyz(pow2(mA),mQ32,mD32)*(deltaxyz(pow2(
        mA),mQ32,mD32)*(mD32 - mU32 + pow2(mA)) - (mQ32 - mU32)*pow2(mD32 -
        mQ32 + pow2(mA))))/(mD32*deltaxyz(pow2(mA),mQ32,mD32)*pow2(mQ32 - mU32)
        ) + log(pow2(mA))*(-(log(mQ32)/pow2(mQ32 - mU32)) + log(mU32)/pow2(mQ32
        - mU32) + (deltaxyz(pow2(mA),mQ32,mD32) - pow2(mD32 - mQ32) + pow4(mA))
        /(mQ32*(mQ32 - mU32)*deltaxyz(pow2(mA),mQ32,mD32))) + log(mD32)*(-(log(
        mQ32)/pow2(mQ32 - mU32)) + log(mU32)/pow2(mQ32 - mU32) + (deltaxyz(
        pow2(mA),mQ32,mD32) + 2*mQ32*pow2(mA) - pow4(mA) + pow4(mD3) - pow4(
        mQ3))/(mQ32*(mQ32 - mU32)*deltaxyz(pow2(mA),mQ32,mD32))) - 2/(-(mQ32*
        mU32) + pow4(mQ3)) + lMR*((-2*log(mU32))/pow2(mQ32 - mU32) - 2/(-(mQ32*
        mU32) + pow4(mQ3)))) + 3*((4*mQ32 - 2*pow2(mA))/(mQ32*(mQ32 - mU32)) +
        log(mD32)*((log(mQ32)*(mD32 + mQ32 - pow2(mA)))/pow2(mQ32 - mU32) - (
        log(mU32)*(mD32 + mQ32 - pow2(mA)))/pow2(mQ32 - mU32)) + log(mQ32)*(-((
        3*mQ32 + mU32 - 2*pow2(mA))/pow2(mQ32 - mU32)) + (2*lMR*(-mU32 + pow2(
        mA)))/pow2(mQ32 - mU32)) + lMR*((2*(mQ32 - pow2(mA)))/(mQ32*(mQ32 -
        mU32)) - (2*log(mU32)*(-mU32 + pow2(mA)))/pow2(mQ32 - mU32)) + (
        deltaxyz(pow2(mA),mQ32,mD32)*phixyz(pow2(mA),mQ32,mD32))/(mD32*pow2(
        mQ32 - mU32)) + (log(mU32)*(mQ32 + 3*mU32 - 2*pow2(mA)))/pow2(mQ32 -
        mU32) + (phixyz(pow2(mA),mU32,mD32)*(-deltaxyz(pow2(mA),mU32,mD32) + (
        mQ32 - mU32)*(mD32 - mU32 + pow2(mA))))/(mD32*pow2(mQ32 - mU32)) + log(
        pow2(mA))*(-((log(mQ32)*(mD32 + mQ32 - 2*mU32 + pow2(mA)))/pow2(mQ32 -
        mU32)) + (log(mU32)*(mD32 + mQ32 - 2*mU32 + pow2(mA)))/pow2(mQ32 -
        mU32) + (2*pow2(mA))/(-(mQ32*mU32) + pow4(mQ3))))) + (3*pow2(Yb)*((11*
        mQ32 + mU32)/(mQ32*pow3(mQ32 - mU32)) + log(mQ32)*(-((mD32 - mQ32 +
        pow2(mA))/(deltaxyz(pow2(mA),mQ32,mD32)*pow2(mQ32 - mU32))) - (4*(2*
        mQ32 + mU32))/pow4(mQ32 - mU32) - (2*lMR*(mQ32 + 2*mU32))/pow4(mQ32 -
        mU32)) + lMR*((5*mQ32 + mU32)/(mQ32*pow3(mQ32 - mU32)) + (2*(mQ32 + 2*
        mU32)*log(mU32))/pow4(mQ32 - mU32)) + log(mD32)*((deltaxyz(pow2(mA),
        mQ32,mD32) + 2*mQ32*pow2(mA) - pow4(mA) + pow4(mD3) - pow4(mQ3))/(2.*
        mQ32*deltaxyz(pow2(mA),mQ32,mD32)*pow2(mQ32 - mU32)) + (log(mQ32)*(3*
        mD32 + mQ32 + 2*mU32 - 3*pow2(mA)))/pow4(mQ32 - mU32) - (log(mU32)*(3*
        mD32 + mQ32 + 2*mU32 - 3*pow2(mA)))/pow4(mQ32 - mU32)) + log(pow2(mA))*
        ((deltaxyz(pow2(mA),mQ32,mD32) - pow2(mD32 - mQ32) + pow4(mA))/(2.*
        mQ32*deltaxyz(pow2(mA),mQ32,mD32)*pow2(mQ32 - mU32)) + (log(mQ32)*(-3*
        mD32 + mQ32 + 2*mU32 + 3*pow2(mA)))/pow4(mQ32 - mU32) - (log(mU32)*(-3*
        mD32 + mQ32 + 2*mU32 + 3*pow2(mA)))/pow4(mQ32 - mU32)) + (2*(mQ32 + 5*
        mU32)*log(mU32))/pow4(mQ32 - mU32) + (phixyz(pow2(mA),mU32,mD32)*(-3*
        deltaxyz(pow2(mA),mU32,mD32) + (mQ32 - mU32)*(mD32 - mU32 + pow2(mA))))
        /(mD32*pow4(mQ32 - mU32)) + (phixyz(pow2(mA),mQ32,mD32)*((mQ32 - mU32)*
        deltaxyz(pow2(mA),mQ32,mD32)*(4*mD32 - 3*mQ32 - mU32 + 4*pow2(mA)) + 6*
        pow2(deltaxyz(pow2(mA),mQ32,mD32)) - pow2(mQ32 - mU32)*pow2(mD32 - mQ32
        + pow2(mA))))/(2.*mD32*deltaxyz(pow2(mA),mQ32,mD32)*pow4(mQ32 - mU32)))
        + 3*((mQ32*(mQ32 - mU32) + (5*mQ32 + mU32)*pow2(mA))/(mQ32*pow3(mQ32 -
        mU32)) + lMR*((2*mQ32*(mQ32 - mU32) + (5*mQ32 + mU32)*pow2(mA))/(mQ32*
        pow3(mQ32 - mU32)) + (log(mU32)*(2*mQ32*pow2(mA) + 4*mU32*pow2(mA) +
        pow4(mQ3) - pow4(mU3)))/pow4(mQ32 - mU32)) + log(pow2(mA))*((2*mQ32*(-
        mQ32 + mU32) - (5*mQ32 + mU32)*pow2(mA))/(mQ32*pow3(mQ32 - mU32)) + (
        log(mQ32)*(2*mQ32*pow2(mA) + 4*mU32*pow2(mA) + pow4(mQ3) - pow4(mU3)))/
        pow4(mQ32 - mU32) + (log(mU32)*(-4*mU32*pow2(mA) - mQ32*(mQ32 + 2*pow2(
        mA)) + pow4(mU3)))/pow4(mQ32 - mU32)) + log(mQ32)*((lMR*(-4*mU32*pow2(
        mA) - mQ32*(mQ32 + 2*pow2(mA)) + pow4(mU3)))/pow4(mQ32 - mU32) + (-8*
        mU32*pow2(mA) - mQ32*(mQ32 + 4*pow2(mA)) + pow4(mU3))/(2.*pow4(mQ32 -
        mU32))) + (log(mU32)*(4*mQ32*pow2(mA) + 8*mU32*pow2(mA) + pow4(mQ3) -
        pow4(mU3)))/(2.*pow4(mQ32 - mU32))))*pow4(Xt)))/pow2(cbeta) + (3*pow2(
        Xt)*((2*(mD32 + 3*mQ32 - 2*pow2(Mu)))/(mQ32*(-mQ32 + mU32)) + lMR*((2*(
        mD32 + 2*mQ32 - 2*pow2(Mu)))/(mQ32*(-mQ32 + mU32)) - (2*log(mU32)*(mD32
        + mQ32 + mU32 - 2*pow2(Mu)))/pow2(mQ32 - mU32)) + log(mQ32)*((2*lMR*(
        mD32 + mQ32 + mU32 - 2*pow2(Mu)))/pow2(mQ32 - mU32) + (2*(mD32 + 3*mQ32
        - 2*pow2(Mu)) + ((-mQ32 + mU32)*(mD32 + pow2(Mu)))/(mD32 - pow2(Mu)))/
        pow2(mQ32 - mU32)) + (log(mU32)*(-2*(mD32 + 3*mQ32 - 2*pow2(Mu)) + ((
        mQ32 - mU32)*(5*mD32 - 3*pow2(Mu)))/(mD32 - pow2(Mu))))/pow2(mQ32 -
        mU32) + (4*dilog(1 - pow2(Mu)/mQ32)*(-mU32 + pow2(Mu)))/pow2(mQ32 -
        mU32) - (4*dilog(1 - pow2(Mu)/mU32)*(-mU32 + pow2(Mu)))/pow2(mQ32 -
        mU32) + (2*(-mU32 + pow2(Mu))*pow2(log(mQ32)))/pow2(mQ32 - mU32) - (2*(
        -mU32 + pow2(Mu))*pow2(log(mU32)))/pow2(mQ32 - mU32) + log(mD32)*((2*
        log(mU32)*(mD32 + (mD32*(mQ32 - mU32)*(mD32 - 2*pow2(Mu)))/pow2(mD32 -
        pow2(Mu))))/pow2(mQ32 - mU32) + (2*mD32*log(mQ32)*(-1 + ((-mQ32 + mU32)
        *(mD32 - 2*pow2(Mu)))/pow2(mD32 - pow2(Mu))))/pow2(mQ32 - mU32) + (2*
        mD32)/(-(mQ32*mU32) + pow4(mQ3))) + log(pow2(Mu))*((-4*pow2(Mu))/(-(
        mQ32*mU32) + pow4(mQ3)) + (2*log(mQ32)*pow4(Mu))/((-mQ32 + mU32)*pow2(
        mD32 - pow2(Mu))) + (2*log(mU32)*pow4(Mu))/((mQ32 - mU32)*pow2(mD32 -
        pow2(Mu))))) + 3*(-1.5 + 2*dilog(1 - pow2(Mu)/mQ32) + lMR*(log(mU32) -
        (mD32 + mQ32 - 2*pow2(Mu))/mQ32) - (mD32 - 2*pow2(Mu))/mQ32 + mD32/(
        mD32 - pow2(Mu)) + log(mU32)*(1.5 + mD32/(-mD32 + pow2(Mu))) + log(
        mQ32)*(0.5 - lMR + mD32/(-mD32 + pow2(Mu))) + pow2(log(mQ32)) + (2*
        dilog(1 - pow2(Mu)/mD32)*(-2*mD32*pow2(Mu) + pow4(mD3) - pow4(Mu)))/
        pow2(mD32 - pow2(Mu)) + (pow2(log(mD32))*(-2*mD32*pow2(Mu) + pow4(mD3)
        - pow4(Mu)))/pow2(mD32 - pow2(Mu)) + log(pow2(Mu))*((-2*pow2(Mu))/mQ32
        - (pow2(Mu)*(2*mD32 + pow2(Mu)))/pow2(mD32 - pow2(Mu)) - (log(mQ32)*
        pow4(Mu))/pow2(mD32 - pow2(Mu)) - (log(mU32)*pow4(Mu))/pow2(mD32 -
        pow2(Mu))) + log(mD32)*(mD32*(1/mQ32 + (mD32 + 2*pow2(Mu))/pow2(mD32 -
        pow2(Mu))) - (mD32*log(mQ32)*(mD32 - 2*pow2(Mu)))/pow2(mD32 - pow2(Mu))
        - (mD32*log(mU32)*(mD32 - 2*pow2(Mu)))/pow2(mD32 - pow2(Mu)) + (2*log(
        pow2(Mu))*pow4(Mu))/pow2(mD32 - pow2(Mu)))) + 3*((mD32*(4*mQ32*(mQ32 +
        2*mU32) - 3*(7*mQ32 + mU32)*pow2(Mu)) + 2*pow2(Mu)*(-3*mQ32*(mQ32 +
        mU32) + (8*mQ32 + mU32)*pow2(Mu)) + (5*mQ32 + mU32)*pow4(mD3))/(mQ32*(
        mD32 - pow2(Mu))*pow3(mQ32 - mU32)) + lMR*((3*mQ32*(mQ32 + mU32) +
        mD32*(5*mQ32 + mU32) - 2*(5*mQ32 + mU32)*pow2(Mu))/(mQ32*pow3(mQ32 -
        mU32)) + (log(mU32)*(mQ32*(2*mD32 + mQ32 - 4*pow2(Mu)) + 4*mU32*(mD32 +
        mQ32 - 2*pow2(Mu)) + pow4(mU3)))/pow4(mQ32 - mU32)) + log(pow2(Mu))*((
        2*pow2(Mu)*(-3 + ((mQ32 - mU32)*(mQ32*pow2(Mu) + pow2(mD32 - pow2(Mu)))
        )/(mQ32*pow2(mD32 - pow2(Mu)))))/pow3(-mQ32 + mU32) + (log(mQ32)*((mQ32
        - mU32)*(mQ32 + mU32) - 6*pow2(mD32 - pow2(Mu)))*pow4(Mu))/(pow2(mD32 -
        pow2(Mu))*pow4(mQ32 - mU32)) + (log(mU32)*pow4(Mu)*(6*pow2(mD32 - pow2(
        Mu)) - pow4(mQ3) + pow4(mU3)))/(pow2(mD32 - pow2(Mu))*pow4(mQ32 - mU32)
        )) + log(mD32)*((mD32*(6 - ((mQ32 - mU32)*(-2*mD32*(mQ32 + pow2(Mu)) +
        pow2(Mu)*(4*mQ32 + pow2(Mu)) + pow4(mD3)))/(mQ32*pow2(mD32 - pow2(Mu)))
        ))/pow3(-mQ32 + mU32) + (mD32*log(mQ32)*(4*mU32*pow2(mD32 - pow2(Mu)) +
        mQ32*(mQ32*(mD32 - 2*pow2(Mu)) + 2*pow2(mD32 - pow2(Mu))) - (mD32 - 2*
        pow2(Mu))*pow4(mU3)))/(pow2(mD32 - pow2(Mu))*pow4(mQ32 - mU32)) + (
        mD32*log(mU32)*(mQ32*(-(mQ32*(mD32 - 2*pow2(Mu))) - 2*pow2(mD32 - pow2(
        Mu))) - 4*mU32*pow2(mD32 - pow2(Mu)) + (mD32 - 2*pow2(Mu))*pow4(mU3)))/
        (pow2(mD32 - pow2(Mu))*pow4(mQ32 - mU32))) + log(mQ32)*(-((lMR*(mQ32*(
        2*mD32 + mQ32 - 4*pow2(Mu)) + 4*mU32*(mD32 + mQ32 - 2*pow2(Mu)) + pow4(
        mU3)))/pow4(mQ32 - mU32)) + (-4*mU32*(2*mD32 + 5*mQ32 - 4*pow2(Mu))*(
        mD32 - pow2(Mu)) + mQ32*(5*(mQ32 - 4*pow2(Mu))*pow2(Mu) + 3*mD32*(-mQ32
        + 8*pow2(Mu)) - 4*pow4(mD3)) - (mD32 + pow2(Mu))*pow4(mU3))/(2.*(mD32 -
        pow2(Mu))*pow4(mQ32 - mU32))) - (2*dilog(1 - pow2(Mu)/mU32)*(2*mQ32 +
        mU32 - 3*pow2(Mu))*(mU32 - pow2(Mu)))/pow4(mQ32 - mU32) + (2*dilog(1 -
        pow2(Mu)/mQ32)*(-mU32 + pow2(Mu))*(-2*mQ32 - mU32 + 3*pow2(Mu)))/pow4(
        mQ32 - mU32) + ((-mU32 + pow2(Mu))*(-2*mQ32 - mU32 + 3*pow2(Mu))*pow2(
        log(mQ32)))/pow4(mQ32 - mU32) + ((mU32 - pow2(Mu))*(-2*mQ32 - mU32 + 3*
        pow2(Mu))*pow2(log(mU32)))/pow4(mQ32 - mU32) + (log(mU32)*(mD32*((mQ32
        + mU32)*(mQ32 + 11*mU32) - 12*(mQ32 + 3*mU32)*pow2(Mu)) + pow2(Mu)*(-3*
        (mQ32 + mU32)*(mQ32 + 3*mU32) + 4*(2*mQ32 + 7*mU32)*pow2(Mu)) + 4*(mQ32
        + 2*mU32)*pow4(mD3)))/(2.*(mD32 - pow2(Mu))*pow4(mQ32 - mU32)))*pow4(
        Xt))/pow2(cbeta));
}

//	one-loop functions from arxiv:1407.4081. Checked.	//

double himalaya::ThresholdCalculator::F1(double x){
   if(x == 1.) return 1.;
   
   const double x2 = pow2(x);
   
   return x*log(x2)/(x2 - 1);
}

double himalaya::ThresholdCalculator::F2(double x){
   if(x == 1.) return 1.;
   
   const double x2 = pow2(x);
   
   return 6*x2*(2 - 2*x2 + (1 + x2)*log(x2))/pow3(x2 - 1);
}

double himalaya::ThresholdCalculator::F3(double x){
   if(x == 1.) return 1.;
   
   const double x2 = pow2(x);
   
   return 2*x*(5*(1-x2) + (1 + 4*x2)*log(x2))/(3*pow2(x2-1));
}

double himalaya::ThresholdCalculator::F4(double x){
   if(x == 1.) return 1.;
   
   const double x2 = pow2(x);
   
   return 2*x*(x2 - 1 - log(x2))/pow2(x2 -1);
}

double himalaya::ThresholdCalculator::F5(double x){
   if(x == 1.) return 1.;
   
   const double x2 = pow2(x);
   
   return 3*x*(1 - pow2(x2) + 2*x2*log(x2))/pow3(1 - x2);
}

double himalaya::ThresholdCalculator::F6(double x){
   if(x == 1.) return 0.;
   
   const double x2 = pow2(x);
   
   return (x2 - 3)/(4.*(1-x2)) + x2*(x2 - 2)*log(x2)/(2*pow2(1 - x2));
}

double himalaya::ThresholdCalculator::F7(double x){
   if(x == 1.) return 1.;
   
   const double x2 = pow2(x);
   
   return -3*(1 - 6*x2 + pow2(x2))/(2*pow2(x2 - 1)) + 3*pow2(x2)*(x2 - 3)
      *log(x2)/pow3(x2 - 1);
}

double himalaya::ThresholdCalculator::F8(double x1, double x2){
   if(x1 == 1. && x2 == 1.){
      return 1.;
   } else if(x1 == 1. || x2 == 1.){
      if (x1 == 1.) x1 = x2;
      
      const double x12 = pow2(x1);
      return 2*(x12 - pow2(x12) + pow2(x12)*log(x12))/pow2(x12 - 1);
   } else if(x1 == x2){
      const double x12 = pow2(x1);
      return 2*(-1 + x12 + x12*(x12 - 2)*log(x12))/pow2(x12 - 1);
   } else{
      const double x12 = pow2(x1);
      const double x22 = pow2(x2);
      return -2 + 2*(pow2(x12)*log(x12)/(x12 -1) - pow2(x22)*log(x22)/(x22 - 1))
	 /(x12 - x22);
   }
}

double himalaya::ThresholdCalculator::F9(double x1, double x2){
   if(x1 == 1. && x2 == 1.){
      return 1.;
   } else if(x1 == 1. || x2 == 1.){
      if (x1 == 1.) x1 = x2;
      
      const double x12 = pow2(x1);
      return 2*(1 - x12 + x12*log(x12))/pow2(x12 - 1);
   } else if(x1 == x2){
      const double x12 = pow2(x1);
      return 2*(-1 + x12 - log(x12))/pow2(x12 - 1);
   } else{
      const double x12 = pow2(x1);
      const double x22 = pow2(x2);
      return 2*(x12*log(x12)/(x12 - 1) - x22*log(x22)/(x22 - 1))/(x12 - x22);
   }
}

double himalaya::ThresholdCalculator::f1(double x){
   if(x == 1.) return 0.;
   
   const double x2 = pow2(x);
   
   return 6*x2*(3+x2)/(7*pow2(x2 - 1)) + 6*pow2(x2)*(x2 - 5)*log(x2)/(7
      *pow3(x2-1));
}

double himalaya::ThresholdCalculator::f2(double x){
   if(x == 1.) return 0.;
   
   const double x2 = pow2(x);
   
   return 2*x2*(11 + x2)/(9*pow2(x2 - 1)) + 2*pow2(x2)*(5*x2 - 17)*log(x2)/(9*
      pow3(x2 - 1));
}

double himalaya::ThresholdCalculator::f3(double x){
   if(x == 1.) return 0.;
   
   const double x2 = pow2(x);
   
   return 2*(2 + 9*x2 + pow2(x2))/(3*pow2(x2 - 1)) + 2*x2*(-6 - 7*x2 + pow2(x2))
      *log(x2)/(3*pow3(x2 - 1));
}

double himalaya::ThresholdCalculator::f4(double x){
   if(x == 1.) return 0.;
   
   const double x2 = pow2(x);
   
   return 2*(6 + 25*x2 + 5*pow2(x2))/(7*pow2(x2 - 1)) + 2*x2*(-18 - 19*x2 
      + pow2(x2))*log(x2)/(7*pow3(x2 - 1));
}

double himalaya::ThresholdCalculator::f5(double x1, double x2){
   if(x1 == 1. && x2 == 1.){
      return 1.;
   } else if(x1 == 1. || x2 == 1.){
      if (x1 == 1.) x1 = x2;
      
      const double x12 = pow2(x1);
      return 3*(-1 + x1 + 2*x12 - pow2(x12) - pow5(x1) + (pow3(x1) + pow5(x1))
	 *log(x12))/(4*pow3(x1 - 1)*pow2(1 + x1));
   } else if(x1 == x2){
      const double x12 = pow2(x1);
      return 3/4.*(-1 - 5*x12 + 5*pow2(x12) + pow3(x12) + x12*(-3 - 6*x12 + pow2(x12))
	 *log(x12))/pow3(x12 - 1);
   } else{
      const double x12 = pow2(x1);
      const double x22 = pow2(x2);
      return 3/4.*((1 - x12*x22 + pow2(x1 + x2))/((x12 - 1)*(x22 - 1))
	 + pow3(x1)*(1 + x12)*log(x12)/(pow2(x12 - 1)*(x1 - x2))
	 - pow3(x2)*(1 + x22)*log(x22)/(pow2(x22 - 1)*(x1 - x2)));
   }
}

double himalaya::ThresholdCalculator::f6(double x1, double x2){
   if(x1 == 1. && x2 == 1.){
      return 1.;
   } else if(x1 == 1. || x2 == 1.){
      if (x1 == 1.) x1 = x2;
      
      const double x12 = pow2(x1);
      return -3*(1 - 2*x12 - 2*pow3(x1) + pow2(x12) + 2*pow5(x1) - 2*pow5(x1)
	 *log(x12))/(7*pow3(x1 - 1)*pow2(x1 + 1));
   } else if(x1 == x2){
      const double x12 = pow2(x1);
      return 6/7.*x12*(-3 + 2*x12 + pow2(x12) + x12*(x12 - 5)*log(x12))/pow3(x12 - 1);
   } else{
      const double x12 = pow2(x1);
      const double x22 = pow2(x2);
      return 6/7.*((x12 + x1*x2 + x22 - x12*x22)/((x12 - 1)*(x22 - 1)) 
	 + pow5(x1)*log(x12)/(pow2(x12 - 1)*(x1 - x2)) 
	 - pow5(x2)*log(x22)/(pow2(x22 - 1)*(x1 - x2)));
   }
}

double himalaya::ThresholdCalculator::f7(double x1, double x2){
   if(x1 == 1. && x2 == 1.){
      return 1.;
   } else if(x1 == 1. || x2 == 1.){
      if (x1 == 1.) x1 = x2;
      
      const double x12 = pow2(x1);
      return -3*(1 - 2*x1 - 2*x12 + 2*pow3(x1) + pow2(x12) - 2*pow3(x1)*log(x12))
	 /(pow3(x1 - 1)*pow2(x1 + 1));
   } else if(x1 == x2){
      const double x12 = pow2(x1);
      return -6*(1 + 2*x12 -3*pow2(x12) + x12*(3 + x12)*log(x12))/pow3(x12 - 1);
   } else{
      const double x12 = pow2(x1);
      const double x22 = pow2(x2);
      return 6*((1 + x1*x2)/((x12 - 1)*(x22 - 1))
	 + pow3(x1)*log(x12)/(pow2(x12 - 1)*(x1 - x2))
	 - pow3(x2)*log(x22)/(pow2(x22 - 1)*(x1 - x2)));
   }
}

double himalaya::ThresholdCalculator::f8(double x1, double x2){
   if(x1 == 1. && x2 == 1.){
      return 1.;
   } else if(x1 == 1. || x2 == 1.){
      if (x1 == 1.) x1 = x2;
      
      const double x12 = pow2(x1);
      return 3*(-1 + 4*x12 - 3*pow2(x12) + 2*pow2(x12)*log(x12))
	 /(4*pow3(x1 - 1)*pow2(1 + x1));
   } else if(x1 == x2){
      const double x12 = pow2(x1);
      return 3*x1*(-1 + pow2(x12) - 2*x12*log(x12))/pow3(x12 - 1);
   } else{
      const double x12 = pow2(x1);
      const double x22 = pow2(x2);
      return 3/2.*((x1 + x2)/((x12 - 1)*(x22 - 1))
	 + pow2(x12)*log(x12)/(pow2(x12 - 1)*(x1 - x2))
	 - pow2(x22)*log(x22)/(pow2(x22 - 1)*(x1 - x2)));
   }
}

double himalaya::ThresholdCalculator::deltaxyz(double x, double y, double z){
   return pow2(x) + pow2(y) + pow2(z) - 2*(x*y+x*z+y*z);
}

/**
 * \f$\Phi(x,y,z)\f$ function.  The arguments x, y and z are
 * interpreted as squared masses. Taken from FlexibleSUSY.
 *
 * Davydychev and Tausk, Nucl. Phys. B397 (1993) 23
 *
 * @param x squared mass
 * @param y squared mass
 * @param z squared mass
 *
 * @return \f$\Phi(x,y,z)\f$
 */
double himalaya::ThresholdCalculator::phixyz(double x, double y, double z){
   const auto u = x/z, v = y/z;
   return phi_uv(u,v);
}

//	at*as^n threshold corrections with limits and DR' -> MS shifts		//
/**
 * 	Returns delta g3_as in the MSbar scheme for a given mass limit
 * 	@param omitLogs an intiger to omit all log mu terms
 * 	@return delta g3_as in the MSbar scheme for a given mass limit
 */
double himalaya::ThresholdCalculator::getDeltaG3Alphas(int omitLogs){

   using std::log;
   
   const double Mst12 = pow2(p.MSt(0));
   const double m32 = pow2(p.MG);
   const double MR2 = pow2(p.scale);
   const double mU32 = p.mu2(2,2);
   const double mQ32 = p.mq2(2,2);
   const double lmsqMR = omitLogs * log(Mst12 / MR2) + log(msq2 / Mst12);
   const double lm3MR = omitLogs * log(Mst12 / MR2) + log(m32 / Mst12);
   const double lmU3MR = omitLogs * log(Mst12 / MR2) + log(mU32 / Mst12);
   const double lmQ3MR = omitLogs * log(Mst12 / MR2) + log(mQ32 / Mst12);

   return 1 / 2. - lm3MR - (lmQ3MR + 10 * lmsqMR + lmU3MR) / 12.;
}

/**
 * 	Returns delta yt_as in the MSbar scheme for a given mass limit
 * 	@param limit an integer key for a mass limit
 * 	@param omitLogs an integer key to omit all mu terms
 * 	@return delta yt_as in the MSbar scheme for a given mass limit
 */
double himalaya::ThresholdCalculator::getDeltaYtAlphas(int limit, int omitLogs){

   using std::log;
   using std::sqrt;
   
   const double Mst12 = pow2(p.MSt(0));
   const double m32 = pow2(p.MG);
   const double MR2 = pow2(p.scale);
   const double mU32 = p.mu2(2,2);
   const double mQ32 = p.mq2(2,2);
   const double m3 = sqrt(m32);
   const double mU3 = sqrt(mU32);
   const double mQ3 = sqrt(mQ32);
   const double Xt = p.Au(2,2) - p.mu * p.vd / p.vu;
   const double lmQ3MR = omitLogs * log(Mst12 / MR2) + log(mQ32 / Mst12);
   
   if(limit == Limits::DEGENERATE) limit = Limits::MQ3_EQ_MU3_EQ_M3;

   switch (limit){
      case(Limits::GENERAL):{
	 return (2*(-2*lmQ3MR + (mQ32*mU32)/((-m32 + mQ32)*(m32 - mU32)) + pow4(m3)/((m32
        - mQ32)*(m32 - mU32)) - (mU32*log(mU32/pow2(mQ3))*(2*m32*(mQ32 - mU32)
        - mQ32*mU32 - 4*m3*mU32*Xt + 4*Xt*pow3(m3) + pow4(mU3)))/((-mQ32 +
        mU32)*pow2(m32 - mU32)) - (log(m32/pow2(mQ3))*pow3(m3)*(-4*mQ32*mU32*Xt
        + 4*m32*(mQ32 + mU32)*Xt - 2*(mQ32 + mU32)*pow3(m3) - 4*Xt*pow4(m3) +
        m3*(pow4(mQ3) + pow4(mU3)) + 2*pow5(m3)))/(pow2(m32 - mQ32)*pow2(m32 -
        mU32))))/3.;
      }
      case(Limits::MQ3_EQ_MU3):{
	 return (-2*((m32 - mQ32)*((-1 + 2*lmQ3MR)*m32 - (1 + 2*lmQ3MR)*mQ32 + 4*m3*Xt) +
        2*(m3 - 2*Xt)*log(m32/pow2(mQ3))*pow3(m3)))/(3.*pow2(m32 - mQ32));
      }
      case(Limits::MQ3_EQ_M3):{
	 return (-4*lmQ3MR - ((mQ32 - mU32)*(mQ32 - 3*mU32 - 8*mQ3*Xt) + 2*mU32*(-2*mQ32
        + mU32 - 4*mQ3*Xt)*log(mU32/pow2(mQ3)))/pow2(mQ32 - mU32))/3.;
      }
      case(Limits::MU3_EQ_M3):{
	 return (2*(-2*lmQ3MR + (-3*mQ32 + mU3*(mU3 - 8*Xt))/(2.*(mQ32 - mU32)) - (log(
        mU32/pow2(mQ3))*(-2*mQ32*mU3*(mU3 - 2*Xt) + pow4(mQ3) + 2*pow4(mU3)))/
        pow2(mQ32 - mU32)))/3.;
      }
      case(Limits::MQ3_EQ_MU3_EQ_M3):{
	 return (-4*(mQ3 + lmQ3MR*mQ3 - Xt))/(3.*mQ3);
      }
   };
   
   throw std::runtime_error("Mass limit not included!");
}

/**
  * 	Returns delta yt_as^2 in the MSbar scheme for a given mass limit
  * 	@param limit an integer key for a mass limit
  * 	@param omitLogs an integer key to omit all mu terms
  * 	@return delta yt_as^2 in the MSbar scheme for a given mass limit
  */
double himalaya::ThresholdCalculator::getDeltaYtAlphas2(int limit, int omitLogs){
   using std::log;
   using std::sqrt;
   using gm2calc::dilog;

   const double Mst12 = pow2(p.MSt(0));
   const double m32 = pow2(p.MG);
   const double MR2 = pow2(p.scale);
   const double mU32 = p.mu2(2,2);
   const double mQ32 = p.mq2(2,2);
   const double m3 = sqrt(m32);
   const double mU3 = sqrt(mU32);
   const double mQ3 = sqrt(mQ32);
   const double msq = sqrt(msq2);
   const double Xt = p.Au(2,2) - p.mu * p.vd / p.vu;
   const double lmQ3MR = omitLogs * log(Mst12 / MR2) + log(mQ32 / Mst12);
   
   switch (limit){
      case(Limits::GENERAL):{
	 return (4*pow2(lmQ3MR) - 60*pow2(log(msq2/mQ32)) - (16*Xt*(-3*m3*(mQ32 + 10*msq2
        + mU32) + 14*pow3(m3)))/((m32 - mQ32)*(m32 - mU32)) + (60*(mQ32 - msq2)
        *dilog(1 - msq2/mQ32)*(-3*m32*(mQ32 - msq2) + mQ32*(mQ32 + msq2) + (8*
        m3*(m32 - mQ32)*(mQ32 - msq2)*Xt)/(mQ32 - mU32) - 2*pow4(m3)))/pow3(m32
        - mQ32) + (60*(msq2 - mU32)*dilog(1 - msq2/mU32)*(-3*m32*(msq2 - mU32)
        - mU32*(msq2 + mU32) + (8*m3*(m32 - mU32)*(msq2 - mU32)*Xt)/(-mQ32 +
        mU32) + 2*pow4(m3)))/pow3(m32 - mU32) + 60*(m32 - msq2)*dilog(1 - msq2/
        m32)*(1/(-m32 + mQ32) + 1/(-m32 + mU32) + (3*mU32)/pow2(m32 - mU32) - (
        8*m3*(mQ32*(msq2 - 2*mU32) + msq2*mU32 + m32*(mQ32 - 2*msq2 + mU32))*
        Xt)/(pow2(m32 - mQ32)*pow2(m32 - mU32)) + (mQ32*(-3*m32 + 3*mQ32 + 4*
        msq2))/pow3(-m32 + mQ32) - msq2*(3*(1/pow2(m32 - mQ32) + 1/pow2(m32 -
        mU32)) + (4*mU32)/pow3(m32 - mU32)) + (4*pow4(mQ3))/pow3(m32 - mQ32) +
        (4*pow4(mU3))/pow3(m32 - mU32)) - (20*log(msq2/mQ32)*(-24*m3*mQ32*msq2*
        mU32*Xt + 24*msq2*(mQ32 + mU32)*Xt*pow3(m3) + 3*mQ32*msq2*pow4(mU3) +
        pow4(mQ3)*(3*msq2*mU32 + 10*pow4(mU3)) + pow4(m3)*(-15*msq2*mU32 +
        mQ32*(-15*msq2 + 44*mU32) + 11*pow4(mQ3) + 11*pow4(mU3)) + 3*m32*((3*
        msq2 - 7*mU32)*pow4(mQ3) + 3*msq2*pow4(mU3) - mQ32*(4*msq2*mU32 + 7*
        pow4(mU3))) - 24*msq2*Xt*pow5(m3) + (-23*mQ32 + 18*msq2 - 23*mU32)*
        pow6(m3) + 12*pow8(m3)))/(pow2(m32 - mQ32)*pow2(m32 - mU32)) + (-(mQ32*
        mU32*(6*mU32*(10*msq2 + mU32) + mQ32*(60*msq2 + 169*mU32) + 6*pow4(mQ3)
        )) + pow4(m3)*(4*mQ32*(75*msq2 - 28*mU32) + 300*msq2*mU32 + 93*pow4(
        mQ3) + 93*pow4(mU3)) - 6*(47*mQ32 + 60*msq2 + 47*mU32)*pow6(m3) - 2*
        m32*(2*(45*msq2 - 52*mU32)*pow4(mQ3) + 9*(10*msq2 + mU32)*pow4(mU3) -
        8*mQ32*(15*msq2*mU32 + 13*pow4(mU3)) + 9*pow6(mQ3)) + 291*pow8(m3))/(
        pow2(m32 - mQ32)*pow2(m32 - mU32)) - (2*(mQ32 - mU32)*dilog(1 - mU32/
        mQ32)*(-8*m3*mQ32*mU32*Xt*(-(mQ32*mU32) + 3*pow4(mQ3) + 3*pow4(mU3)) -
        16*Xt*(7*mQ32*mU32 + 4*pow4(mQ3) + 4*pow4(mU3))*pow5(m3) + (-76*mQ32*
        mU32 + 34*pow4(mQ3) + 34*pow4(mU3))*pow6(m3) + 7*pow4(mU3)*pow6(mQ3) +
        pow4(m3)*(65*mU32*pow4(mQ3) + 65*mQ32*pow4(mU3) - 29*pow6(mQ3) - 29*
        pow6(mU3)) + 7*pow4(mQ3)*pow6(mU3) + 8*Xt*pow3(m3)*(7*mU32*pow4(mQ3) +
        7*mQ32*pow4(mU3) + 3*pow6(mQ3) + 3*pow6(mU3)) + 80*(mQ32 + mU32)*Xt*
        pow7(m3) - 14*(mQ32 + mU32)*pow8(m3) + 3*mU32*pow8(mQ3) + 3*mQ32*pow8(
        mU3) + m32*(-34*pow4(mQ3)*pow4(mU3) - 26*mU32*pow6(mQ3) - 26*mQ32*pow6(
        mU3) + 9*pow8(mQ3) + 9*pow8(mU3)) - 40*Xt*pow9(m3) + 12*power10(m3)))/(
        pow3(m32 - mQ32)*pow3(m32 - mU32)) + (pow2(log(mU32/mQ32))*((128*m32*
        pow2(m32 - mU32)*pow2(Xt)*pow4(mU3))/pow2(mQ32 - mU32) + Xt*((8*m3*(
        mQ32 - mU32)*pow2(m32 - mU32)*(-(mQ32*mU32) - 5*m32*(mQ32 + mU32) + 5*
        pow4(m3) + 3*pow4(mQ3) + 3*pow4(mU3)))/pow2(m32 - mQ32) - (8*(m32 -
        mU32)*((69*mQ32*mU32 + pow4(mQ3) - 86*pow4(mU3))*pow5(m3) + m3*mU32*(
        10*mU32*pow4(mQ3) + 30*mU32*pow4(msq) - 60*msq2*pow4(mU3) + mQ32*(60*
        msq2*mU32 - 30*pow4(msq) + 4*pow4(mU3)) - 3*pow6(mQ3) - 27*pow6(mU3)) +
        pow3(m3)*(-11*mU32*pow4(mQ3) - 30*mU32*pow4(msq) + mQ32*(-60*msq2*mU32
        + 30*pow4(msq) - 59*pow4(mU3)) + 60*msq2*pow4(mU3) + 3*pow6(mQ3) + 99*
        pow6(mU3)) - 18*(mQ32 - mU32)*pow7(m3)))/pow2(mQ32 - mU32)) + ((mQ32 -
        mU32)*pow4(mU3)*(4*mQ32*mU32 + 3*pow4(mQ3) + 30*pow4(msq) + 21*pow4(
        mU3)) + 2*(mQ32*(30*msq2 - 34*mU32) - 30*msq2*mU32 + pow4(mQ3) - 351*
        pow4(mU3))*pow6(m3) - 2*m32*mU32*(18*mU32*pow4(mQ3) + 6*mU32*(-15*msq2*
        mU32 + 5*pow4(msq) + 4*pow4(mU3)) + mQ32*(90*msq2*mU32 - 30*pow4(msq) +
        25*pow4(mU3)) - 3*pow6(mQ3)) + pow4(m3)*(33*mU32*pow4(mQ3) + 90*mU32*
        pow4(msq) - 120*msq2*pow4(mU3) + mQ32*(120*msq2*mU32 - 90*pow4(msq) +
        141*pow4(mU3)) - 9*pow6(mQ3) + 347*pow6(mU3)) + (-44*mQ32 + 556*mU32)*
        pow8(m3) - 128*power10(m3))/(mQ32 - mU32) + ((m32 - mU32)*(-mQ32 +
        mU32)*((-76*mQ32*mU32 + 34*pow4(mQ3) + 34*pow4(mU3))*pow6(m3) + 7*pow4(
        mU3)*pow6(mQ3) + pow4(m3)*(65*mU32*pow4(mQ3) + 65*mQ32*pow4(mU3) - 29*
        pow6(mQ3) - 29*pow6(mU3)) + 7*pow4(mQ3)*pow6(mU3) - 14*(mQ32 + mU32)*
        pow8(m3) + 3*mU32*pow8(mQ3) + 3*mQ32*pow8(mU3) + m32*(-34*pow4(mQ3)*
        pow4(mU3) - 26*mU32*pow6(mQ3) - 26*mQ32*pow6(mU3) + 9*pow8(mQ3) + 9*
        pow8(mU3)) + 12*power10(m3)))/pow3(m32 - mQ32)))/pow4(m32 - mU32) + (2*
        log(mU32/mQ32)*(10*log(msq2/mQ32)*(-2*(3*msq2 + mU32)*pow4(m3) + 3*
        mU32*pow4(msq) + (4*m3*(m32 - mU32)*Xt*(m32*mU32 + 12*msq2*mU32 - 6*
        pow4(msq) - pow4(mU3)))/(-mQ32 + mU32) + 3*m32*(-6*msq2*mU32 + 3*pow4(
        msq) + pow4(mU3)) - pow6(mU3)) + (8*(m32 - mU32)*Xt*(-(mU32*(53*mQ32 +
        30*msq2 + 61*mU32)*pow3(m3)) + m3*mU32*(mQ32*(30*msq2 + 53*mU32) + 3*
        pow4(mQ3) + 3*pow4(mU3)) + (-16*mQ32 + 55*mU32)*pow5(m3) + 16*pow7(m3))
        )/((m32 - mQ32)*(mQ32 - mU32)) + (-(mQ32*(mQ32 - mU32)*(2*mQ32*(15*msq2
        - 64*mU32) + 3*pow4(mQ3) - 3*pow4(mU3))*pow4(mU3)) + mU32*pow4(m3)*(6*(
        30*msq2 + 157*mU32)*pow4(mQ3) - 5*mQ32*(42*msq2*mU32 - 5*pow4(mU3)) +
        30*msq2*pow4(mU3) + 163*pow6(mQ3) - 106*pow6(mU3)) + pow6(m3)*(-622*
        mU32*pow4(mQ3) - 5*mQ32*(18*msq2*mU32 + 197*pow4(mU3)) + 53*pow6(mQ3) +
        18*(5*msq2*pow4(mU3) + pow6(mU3))) + (764*mQ32*mU32 - 105*pow4(mQ3) +
        365*pow4(mU3))*pow8(m3) - m32*mU32*(-6*pow4(mQ3)*(25*msq2*mU32 - 41*
        pow4(mU3)) + (90*msq2 + 271*mU32)*pow6(mQ3) + 3*mQ32*(20*msq2*pow4(mU3)
        - 93*pow6(mU3)) + 9*pow8(mQ3) + 9*pow8(mU3)) + 4*(13*mQ32 - 77*mU32)*
        power10(m3))/((mQ32 - mU32)*pow2(m32 - mQ32))))/pow3(m32 - mU32) - 4*
        lmQ3MR*(20*log(msq2/mQ32) + (8*Xt*pow3(m3))/((m32 - mQ32)*(m32 - mU32))
        - (log(mU32/mQ32)*(8*mU32*(mQ32 + 15*mU32)*pow4(m3) - 200*Xt*pow3(m3)*
        pow4(mU3) + (34*mQ32 - 98*mU32)*pow6(m3) + 23*(mQ32 - mU32)*pow6(mU3) +
        136*m3*Xt*pow6(mU3) + m32*(-69*mQ32*pow4(mU3) + 5*pow6(mU3)) + 64*Xt*
        pow7(m3)))/((-mQ32 + mU32)*pow3(m32 - mU32)) + (-463*m32*mQ32*mU32*(
        mQ32 + mU32) + 202*pow4(mQ3)*pow4(mU3) + pow4(m3)*(1044*mQ32*mU32 +
        257*pow4(mQ3) + 257*pow4(mU3)) - 573*(mQ32 + mU32)*pow6(m3) + 312*pow8(
        m3))/(pow2(m32 - mQ32)*pow2(m32 - mU32)) - (log(m32/mQ32)*(-192*Xt*
        pow11(m3) + 154*pow12(m3) + 72*m32*(mQ32 + mU32)*pow4(mQ3)*pow4(mU3) -
        208*Xt*pow3(m3)*pow4(mQ3)*pow4(mU3) + 408*mQ32*mU32*(mQ32 + mU32)*Xt*
        pow5(m3) - 24*pow6(mQ3)*pow6(mU3) - pow6(m3)*(95*mU32*pow4(mQ3) + 95*
        mQ32*pow4(mU3) + 33*pow6(mQ3) + 33*pow6(mU3)) - pow4(m3)*(152*pow4(mQ3)
        *pow4(mU3) + 11*mU32*pow6(mQ3) + 11*mQ32*pow6(mU3)) - 200*Xt*(4*mQ32*
        mU32 + pow4(mQ3) + pow4(mU3))*pow7(m3) + (406*mQ32*mU32 + 163*pow4(mQ3)
        + 163*pow4(mU3))*pow8(m3) + 392*(mQ32 + mU32)*Xt*pow9(m3) - 288*(mQ32 +
        mU32)*power10(m3)))/(pow3(m32 - mQ32)*pow3(m32 - mU32))) + 2*pow2(log(
        m32/mQ32))*(-15*(m32 - msq2)*(1/(m32 - mQ32) + 1/(m32 - mU32) - (3*
        mU32)/pow2(m32 - mU32) + (mQ32*(-3*m32 + 3*mQ32 + 4*msq2))/pow3(m32 -
        mQ32) + msq2*(3*(1/pow2(m32 - mQ32) + 1/pow2(m32 - mU32)) + (4*mU32)/
        pow3(m32 - mU32)) + (4*pow4(mQ3))/pow3(-m32 + mQ32) + (4*pow4(mU3))/
        pow3(-m32 + mU32)) + (64*pow2(Xt)*pow6(m3))/(pow2(m32 - mQ32)*pow2(m32
        - mU32)) + (8*m3*Xt*(-15*(m32 - mQ32)*(m32 - msq2)*(m32 - mU32)*(mQ32*(
        msq2 - 2*mU32) + msq2*mU32 + m32*(mQ32 - 2*msq2 + mU32)) - m32*(-142*
        m32*mQ32*mU32*(mQ32 + mU32) + 87*pow4(mQ3)*pow4(mU3) + pow4(m3)*(220*
        mQ32*mU32 + 57*pow4(mQ3) + 57*pow4(mU3)) - 82*(mQ32 + mU32)*pow6(m3) +
        27*pow8(m3))))/(pow3(m32 - mQ32)*pow3(m32 - mU32)) + (-878*(mQ32 +
        mU32)*pow14(m3) + 262*pow16(m3) - pow4(m3)*pow4(mQ3)*pow4(mU3)*(452*
        mQ32*mU32 + pow4(mQ3) + pow4(mU3)) + pow12(m3)*(2972*mQ32*mU32 + 885*
        pow4(mQ3) + 885*pow4(mU3)) + 144*m32*(mQ32 + mU32)*pow6(mQ3)*pow6(mU3)
        - 36*pow8(mQ3)*pow8(mU3) + pow8(m3)*(2404*pow4(mQ3)*pow4(mU3) + 1004*
        mU32*pow6(mQ3) + 1004*mQ32*pow6(mU3) + 49*pow8(mQ3) + 49*pow8(mU3)) -
        2*pow6(m3)*(184*pow4(mU3)*pow6(mQ3) + 184*pow4(mQ3)*pow6(mU3) + 79*
        mU32*pow8(mQ3) + 79*mQ32*pow8(mU3)) - 4*(733*mU32*pow4(mQ3) + 733*mQ32*
        pow4(mU3) + 80*pow6(mQ3) + 80*pow6(mU3))*power10(m3))/(pow4(m32 - mQ32)
        *pow4(m32 - mU32))) + (2*dilog(1 - m32/mQ32)*(-8*m3*(m32 - mU32)*Xt*(
        m32*(mQ32 - 37*mU32) - 7*mQ32*mU32 + 18*pow4(m3) + 3*pow4(mQ3) + 22*
        pow4(mU3)) + (128*pow12(m3) - mU32*pow4(mQ3)*(mU32*pow4(mQ3) + 19*mQ32*
        pow4(mU3) + 3*pow6(mQ3) - 23*pow6(mU3)) + pow6(m3)*(-251*mU32*pow4(mQ3)
        - 1025*mQ32*pow4(mU3) + 9*pow6(mQ3) - 13*pow6(mU3)) + (902*mQ32*mU32 +
        98*pow4(mQ3) + 280*pow4(mU3))*pow8(m3) + pow4(m3)*(417*pow4(mQ3)*pow4(
        mU3) - 151*mU32*pow6(mQ3) + 391*mQ32*pow6(mU3) + 20*pow8(mQ3) - 37*
        pow8(mU3)) - 2*(147*mQ32 + 173*mU32)*power10(m3) + m32*(41*pow4(mU3)*
        pow6(mQ3) - 167*pow4(mQ3)*pow6(mU3) + 41*mU32*pow8(mQ3) - 34*mQ32*pow8(
        mU3) - 9*power10(mQ3)))/pow2(m32 - mQ32)))/((mQ32 - mU32)*pow3(m32 -
        mU32)) + (2*dilog(1 - m32/mU32)*(8*m3*(m32 - mQ32)*Xt*(-7*mQ32*mU32 +
        m32*(-37*mQ32 + mU32) + 18*pow4(m3) + 22*pow4(mQ3) + 3*pow4(mU3)) + (-
        128*pow12(m3) + pow6(m3)*(1025*mU32*pow4(mQ3) + 251*mQ32*pow4(mU3) +
        13*pow6(mQ3) - 9*pow6(mU3)) + 19*pow6(mQ3)*pow6(mU3) - 2*(451*mQ32*mU32
        + 140*pow4(mQ3) + 49*pow4(mU3))*pow8(m3) - 23*pow4(mU3)*pow8(mQ3) +
        pow4(m3)*(-417*pow4(mQ3)*pow4(mU3) - 391*mU32*pow6(mQ3) + 151*mQ32*
        pow6(mU3) + 37*pow8(mQ3) - 20*pow8(mU3)) + pow4(mQ3)*pow8(mU3) + m32*
        mU32*(-41*pow4(mQ3)*pow4(mU3) + 167*mU32*pow6(mQ3) - 41*mQ32*pow6(mU3)
        + 34*pow8(mQ3) + 9*pow8(mU3)) + (346*mQ32 + 294*mU32)*power10(m3) + 3*
        mQ32*power10(mU3))/pow2(m32 - mU32)))/((mQ32 - mU32)*pow3(m32 - mQ32))
        + (2*log(m32/mQ32)*(8*(m32 - mQ32)*Xt*pow2(m32 - mU32)*pow3(m3)*(3*
        mU32*(10*msq2 + mU32) - 15*m32*(5*mQ32 + 4*msq2 + 5*mU32) + mQ32*(30*
        msq2 + 59*mU32) + 85*pow4(m3) + 3*pow4(mQ3)) - (m32 - mU32)*(864*pow12(
        m3) + 48*pow6(mQ3)*pow6(mU3) + 3*m32*mQ32*mU32*(5*(2*msq2 - 23*mU32)*
        pow4(mQ3) - 115*mQ32*pow4(mU3) + 10*msq2*pow4(mU3) + pow6(mQ3) + pow6(
        mU3)) - 9*pow6(m3)*((30*msq2 + 317*mU32)*pow4(mQ3) + 30*msq2*pow4(mU3)
        + mQ32*(-20*msq2*mU32 + 317*pow4(mU3)) + 39*pow6(mQ3) + 39*pow6(mU3)) +
        4*(60*msq2*mU32 + 2*mQ32*(30*msq2 + 511*mU32) + 365*pow4(mQ3) + 365*
        pow4(mU3))*pow8(m3) + pow4(m3)*(pow4(mQ3)*(-90*msq2*mU32 + 1894*pow4(
        mU3)) + (90*msq2 + 572*mU32)*pow6(mQ3) + 9*(10*msq2 + mU32)*pow6(mU3) +
        mQ32*(-90*msq2*pow4(mU3) + 572*pow6(mU3)) + 9*pow8(mQ3)) - 2*(971*mQ32
        + 90*msq2 + 971*mU32)*power10(m3)) + 10*(m32 - mU32)*log(msq2/mQ32)*(-
        4*pow12(m3) + 42*mU32*pow4(mQ3)*pow6(m3) + 42*mQ32*pow4(mU3)*pow6(m3) -
        4*m3*(m32 - mQ32)*(m32 - mU32)*Xt*(-((mQ32 - 12*msq2 + mU32)*pow4(m3))
        + m32*(mQ32*mU32 - 12*pow4(msq)) + 6*mU32*pow4(msq) + 6*mQ32*(-2*msq2*
        mU32 + pow4(msq)) + pow6(m3)) - 14*mU32*pow4(m3)*pow6(mQ3) + 2*pow6(m3)
        *pow6(mQ3) - 14*mQ32*pow4(m3)*pow6(mU3) + 2*pow6(m3)*pow6(mU3) - 84*
        mQ32*mU32*pow8(m3) - 6*pow4(mQ3)*pow8(m3) - 6*pow4(mU3)*pow8(m3) + 20*
        mQ32*power10(m3) + 20*mU32*power10(m3) + 3*(m32 - msq2)*(-(mQ32*msq2*
        mU32*(pow4(mQ3) + pow4(mU3))) + (-8*msq2*mU32 + mQ32*(-8*msq2 + 30*
        mU32) + 3*pow4(mQ3) + 3*pow4(mU3))*pow6(m3) - pow4(m3)*((-9*msq2 + 15*
        mU32)*pow4(mQ3) - 9*msq2*pow4(mU3) + 3*mQ32*(2*msq2*mU32 + 5*pow4(mU3))
        + pow6(mQ3) + pow6(mU3)) + m32*(3*msq2*mU32*pow4(mQ3) + (-3*msq2 + 5*
        mU32)*pow6(mQ3) - 3*msq2*pow6(mU3) + mQ32*(3*msq2*pow4(mU3) + 5*pow6(
        mU3))) + (-8*mQ32 + 6*msq2 - 8*mU32)*pow8(m3) + 2*power10(m3))) + (2*
        log(mU32/mQ32)*(604*mQ32*mU32*pow12(m3) - 204*mQ32*pow14(m3) - 180*
        mU32*pow14(m3) + 64*pow16(m3) + 64*mU32*pow2(m32 - mQ32)*pow2(m32 -
        mU32)*pow2(Xt)*pow4(m3) + 234*pow12(m3)*pow4(mQ3) + 122*pow12(m3)*pow4(
        mU3) - 239*pow4(mU3)*pow6(m3)*pow6(mQ3) - 13*pow4(mQ3)*pow6(m3)*pow6(
        mU3) - 53*pow4(m3)*pow6(mQ3)*pow6(mU3) + 503*pow4(mQ3)*pow4(mU3)*pow8(
        m3) + 401*mU32*pow6(mQ3)*pow8(m3) + 13*mQ32*pow6(mU3)*pow8(m3) + 43*
        pow4(m3)*pow4(mU3)*pow8(mQ3) - 76*mU32*pow6(m3)*pow8(mQ3) + 24*m32*
        pow6(mU3)*pow8(mQ3) + 13*pow8(m3)*pow8(mQ3) + 61*pow4(m3)*pow4(mQ3)*
        pow8(mU3) - 49*mQ32*pow6(m3)*pow8(mU3) - 6*m32*pow6(mQ3)*pow8(mU3) +
        30*pow8(m3)*pow8(mU3) - 6*pow8(mQ3)*pow8(mU3) - 2*(m32 - mQ32)*(m32 -
        mU32)*Xt*(68*pow11(m3) + 12*m3*pow4(mQ3)*pow6(mU3) + pow5(m3)*(-184*
        mU32*pow4(mQ3) - 215*mQ32*pow4(mU3) + 3*pow6(mU3)) + pow3(m3)*(105*
        pow4(mQ3)*pow4(mU3) - 17*mQ32*pow6(mU3)) + (353*mQ32*mU32 + 75*pow4(
        mQ3) + 120*pow4(mU3))*pow7(m3) - (137*mQ32 + 183*mU32)*pow9(m3)) - 779*
        mU32*pow4(mQ3)*power10(m3) - 369*mQ32*pow4(mU3)*power10(m3) - 101*pow6(
        mQ3)*power10(m3) - 31*pow6(mU3)*power10(m3) + 13*mQ32*pow4(m3)*power10(
        mU3) - 18*m32*pow4(mQ3)*power10(mU3) - 7*pow6(m3)*power10(mU3) + 6*
        pow6(mQ3)*power10(mU3)))/(mQ32 - mU32)))/(pow3(m32 - mQ32)*pow4(m32 -
        mU32)))/18.;
      }
      case(Limits::MQ3_EQ_MU3):{
	 return (4*pow2(lmQ3MR) + (128*m3*Xt*(m32 - mQ32 + m3*Xt))/pow2(m32 - mQ32) - 60*
        pow2(log(msq2/mQ32)) + (480*m3*msq2*(-mQ32 + msq2)*Xt*log(msq2/mQ32))/
        pow2(-(m32*mQ3) + pow3(mQ3)) + (32*(8*m3 - 9*Xt)*log(m32/mQ32)*pow3(m3)
        )/((m32 - mQ32)*mQ32) - (32*Xt*(-3*m3*(mQ32 + 5*msq2) + 7*pow3(m3)))/
        pow2(m32 - mQ32) + (60*(mQ32 - msq2)*dilog(1 - msq2/mQ32)*(-3*m32*(mQ32
        - msq2) + mQ32*(mQ32 + msq2) + (2*m3*(m32 - mQ32)*(mQ32 - msq2)*Xt)/
        mQ32 - 2*pow4(m3)))/pow3(m32 - mQ32) - (120*(m32 - msq2)*dilog(1 -
        msq2/m32)*(mQ32*msq2 + m32*(-5*mQ32 + 3*msq2) + 8*m3*(mQ32 - msq2)*Xt +
        pow4(m3)))/pow3(m32 - mQ32) + (18*m32*(mQ32 - 20*msq2) - 120*mQ32*msq2
        + 291*pow4(m3) - 181*pow4(mQ3))/pow2(m32 - mQ32) - (40*log(msq2/mQ32)*(
        3*mQ32*msq2 + m32*(-11*mQ32 + 9*msq2) - 12*m3*msq2*Xt + 6*pow4(m3) + 5*
        pow4(mQ3)))/pow2(m32 - mQ32) + (60*(mQ32 - msq2)*dilog(1 - msq2/mQ32)*(
        2*m3*mQ32*(mQ32 - 9*msq2)*Xt + 2*(7*mQ32 + msq2)*Xt*pow3(m3) - 2*mQ32*
        pow4(m3) + (mQ32 + msq2)*pow4(mQ3) - 3*m32*(-(mQ32*msq2) + pow4(mQ3))))
        /(mQ32*pow3(m32 - mQ32)) - (16*m3*(55*m32*mQ32*Xt - 30*mQ32*msq2*Xt -
        32*mQ32*pow3(m3) + 16*Xt*pow4(m3) + 32*m3*pow4(mQ3) - 59*Xt*pow4(mQ3) -
        5*Xt*log(msq2/mQ32)*(m32*mQ32 + 12*mQ32*msq2 - pow4(mQ3) - 6*pow4(msq))
        ))/pow2(-(m32*mQ3) + pow3(mQ3)) + (4*dilog(1 - m32/mQ32)*(-40*mQ32*Xt*
        pow3(m3) + 13*mQ32*pow4(m3) - 36*m32*pow4(mQ3) + 22*m3*Xt*pow4(mQ3) +
        18*Xt*pow5(m3) - 16*pow6(m3) + 15*pow6(mQ3)))/pow2(-(m32*mQ3) + pow3(
        mQ3)) + (4*dilog(1 - m32/mQ32)*(32*mQ32*Xt*pow3(m3) - 51*mQ32*pow4(m3)
        - 4*m32*pow4(mQ3) - 14*m3*Xt*pow4(mQ3) - 18*Xt*pow5(m3) + 16*pow6(m3) +
        15*pow6(mQ3)))/pow2(-(m32*mQ3) + pow3(mQ3)) - (8*lmQ3MR*((m32 - mQ32)*(
        -10*log(msq2/mQ32)*pow2(-(m32*mQ3) + pow3(mQ3)) + 28*mQ32*Xt*pow3(m3) -
        188*mQ32*pow4(m3) + 293*m32*pow4(mQ3) - 68*m3*Xt*pow4(mQ3) + 32*Xt*
        pow5(m3) - 101*pow6(mQ3)) + mQ32*log(m32/mQ32)*(104*mQ32*Xt*pow3(m3) -
        57*mQ32*pow4(m3) - 36*m32*pow4(mQ3) - 96*Xt*pow5(m3) + 77*pow6(m3) +
        12*pow6(mQ3))))/(mQ32*pow3(-m32 + mQ32)) + (8*log(m32/mQ32)*(6*mQ32*(3*
        mQ32 + 20*msq2)*Xt*pow3(m3) + mQ32*(-291*mQ32 - 45*msq2 + 32*pow2(Xt))*
        pow4(m3) + 14*mQ32*Xt*pow5(m3) + 152*mQ32*pow6(m3) - 5*mQ32*log(msq2/
        mQ32)*(24*m3*msq2*(-mQ32 + msq2)*Xt + 2*(mQ32 - 12*msq2)*Xt*pow3(m3) -
        (mQ32 - 6*msq2)*pow4(m3) + 9*m32*(2*mQ32*msq2 - pow4(msq)) - 3*mQ32*
        pow4(msq) - 2*Xt*pow5(m3) + pow6(m3)) - 12*m3*Xt*pow6(mQ3) + 15*m32*(-(
        msq2*pow4(mQ3)) + 9*pow6(mQ3)) - 68*Xt*pow7(m3) + 32*pow8(m3) - 12*
        pow8(mQ3)))/(mQ32*pow3(-m32 + mQ32)) + (4*pow2(log(m32/mQ32))*(-120*m3*
        mQ32*(mQ32 - msq2)*msq2*Xt - 12*Xt*pow3(m3)*(19*pow4(mQ3) + 10*pow4(
        msq)) + pow4(m3)*(-60*mQ32*msq2 + 94*pow4(mQ3) + 45*pow4(msq)) + 40*(8*
        mQ32 + 3*msq2)*Xt*pow5(m3) + (-264*mQ32 - 30*msq2 + 32*pow2(Xt))*pow6(
        m3) + 6*m32*(15*msq2*pow4(mQ3) - 5*mQ32*pow4(msq) + 12*pow6(mQ3)) -
        108*Xt*pow7(m3) + 116*pow8(m3) - 3*(5*pow4(mQ3)*pow4(msq) + 6*pow8(mQ3)
        )))/pow4(m32 - mQ32))/18.;
      }
      case(Limits::MQ3_EQ_M3):{
	 return   (8064 + (384*(mQ32 + mU32 + 2*mQ3*Xt)*log(mQ32/mU32))/(mQ32 - mU32) +
        192*pow2(lmQ3MR) - 2880*pow2(log(msq2/mQ32)) - (288*(mQ3 - 2*Xt)*(-2*
        mQ32 + 2*mU32 + (mQ32 + mU32)*log(mQ32/mU32)))/(-(mQ3*mU32) + pow3(mQ3)
        ) - (2880*msq2*(-2*mQ32 + 2*msq2 + (-3*mQ32 + msq2)*log(msq2/mQ32))*(-(
        mQ3*mU32) - 2*mQ32*Xt + 2*msq2*Xt + pow3(mQ3)))/((mQ32 - msq2)*(mQ32 -
        mU32)*pow3(mQ3)) - (2880*(msq2 - mU32)*dilog(1 - msq2/mU32)*(3*mQ32*(
        msq2 - mU32) + mU32*(msq2 + mU32) + 8*mQ3*(msq2 - mU32)*Xt - 2*pow4(
        mQ3)))/pow3(mQ32 - mU32) + (180*(mQ32 - msq2)*(-3*mQ32 - msq2 - (8*mQ3*
        (mQ32 - msq2)*Xt)/(mQ32 - mU32))*dilog(1 - msq2/mQ32))/pow4(mQ3) - (
        192*Xt*(mQ32*(90*msq2 - 58*mU32) + 3*mU32*(10*msq2 + mU32) + 23*pow4(
        mQ3)))/(mQ3*pow2(mQ32 - mU32)) + (32*(mQ32*(-10*mQ32 + 30*msq2 + 3*mU32
        + 3*mQ32*log(mU32/mQ32)) + 30*log(msq2/mQ32)*(2*mQ32*msq2 - pow4(msq)))
        )/pow4(mQ3) + (96*(2*mQ32*mU32 - 3*pow4(mQ3) + log(mQ32/mU32)*(3*mQ32*
        mU32 + pow4(mQ3)) + pow4(mU3)))/(-(mQ32*mU32) + pow4(mQ3)) - (12*(240*
        msq2*mU32 + 10*mQ32*(72*msq2 + 61*mU32) - 837*pow4(mQ3) + 19*pow4(mU3))
        )/pow2(mQ32 - mU32) - (240*log(msq2/mQ32)*(72*mQ32*msq2*Xt + 24*msq2*
        mU32*Xt + 18*(2*msq2 - 5*mU32)*pow3(mQ3) + mQ3*(12*msq2*mU32 + 43*pow4(
        mU3)) + 47*pow5(mQ3)))/(mQ3*pow2(mQ32 - mU32)) + (24*(-(log(mU32/mQ32)*
        pow3(mQ3)*(-34*mQ32*mU32 + 8*mQ3*mU32*Xt - 24*Xt*pow3(mQ3) + 13*pow4(
        mQ3) + 17*pow4(mU3))) + mQ3*(mQ32 - mU32)*(-(mQ32*(120*msq2 + 137*mU32)
        ) + 24*mQ3*(10*msq2 + mU32)*Xt - 104*Xt*pow3(mQ3) + 129*pow4(mQ3) + 12*
        (10*msq2*mU32 + pow4(mU3))) - 10*(mQ32 - mU32)*log(msq2/mQ32)*(-12*mQ3*
        msq2*mU32 - 48*mQ32*msq2*Xt + (12*msq2 - mU32)*pow3(mQ3) + 24*Xt*pow4(
        msq) + pow5(mQ3))))/(pow2(mQ32 - mU32)*pow3(mQ3)) - (960*msq2*(-10*
        mQ32*msq2 + 7*pow4(mQ3) + 3*pow4(msq) + log(msq2/mQ32)*(-3*mQ32*msq2 +
        6*pow4(mQ3) + pow4(msq))))/(-(msq2*pow4(mQ3)) + pow6(mQ3)) + (48*(10*(
        3*mQ32 - 3*mU32 - 8*mQ3*Xt)*log(msq2/mQ32)*pow2(-(mQ3*mU32) + pow3(mQ3)
        ) - pow2(mQ32 - mU32)*(6*mU32*(10*msq2 + mU32) - 3*mQ32*(20*msq2 + 221*
        mU32) + 48*mQ3*(10*msq2 + mU32)*Xt - 1312*Xt*pow3(mQ3) + 1189*pow4(mQ3)
        ) + mQ32*log(mU32/mQ32)*(144*mU32*Xt*pow3(mQ3) - 33*mU32*pow4(mQ3) -
        88*mQ3*Xt*pow4(mU3) + mQ32*(256*mU32*pow2(Xt) + 53*pow4(mU3)) + 8*Xt*
        pow5(mQ3) + 11*pow6(mQ3) - 15*pow6(mU3))))/(mQ32*pow3(mQ32 - mU32)) + (
        6*dilog(1 - mU32/mQ32)*(80*mU32*Xt*pow3(mQ3) + 31*mU32*pow4(mQ3) - 19*
        mQ32*pow4(mU3) - 24*mQ3*Xt*pow4(mU3) + 456*Xt*pow5(mQ3) + 119*pow6(mQ3)
        - 3*pow6(mU3)))/(-(mU32*pow4(mQ3)) + pow6(mQ3)) - (32*lmQ3MR*(120*log(
        msq2/mQ32)*pow3(mQ32 - mU32) + (mQ32 - mU32)*(-2044*mQ32*mU32 - 1224*
        mQ3*mU32*Xt + 1176*Xt*pow3(mQ3) + 983*pow4(mQ3) + 1037*pow4(mU3)) + 6*
        log(mU32/mQ32)*(64*mU32*Xt*pow3(mQ3) - 56*mU32*pow4(mQ3) - 5*mQ32*pow4(
        mU3) - 136*mQ3*Xt*pow4(mU3) + 64*Xt*pow5(mQ3) + 34*pow6(mQ3) + 23*pow6(
        mU3))))/pow3(mQ32 - mU32) + (24*log(mU32/mQ32)*(-96*mQ32*mU32*(10*msq2
        + 19*mU32)*Xt + 1544*mU32*Xt*pow4(mQ3) - 3*pow3(mQ3)*(120*msq2*mU32 +
        59*pow4(mU3)) - 479*mU32*pow5(mQ3) + 512*Xt*pow6(mQ3) + 24*Xt*pow6(mU3)
        - 40*mQ3*log(msq2/mQ32)*(4*mU32*Xt*pow3(mQ3) + 2*(3*msq2 + mU32)*pow4(
        mQ3) - 3*mU32*pow4(msq) - 3*mQ32*(-6*msq2*mU32 + 3*pow4(msq) + pow4(
        mU3)) - 4*mQ3*Xt*(-12*msq2*mU32 + 6*pow4(msq) + pow4(mU3)) + pow6(mU3))
        + mQ3*(-120*msq2*pow4(mU3) + 527*pow6(mU3)) + 209*pow7(mQ3)))/(mQ3*
        pow3(mQ32 - mU32)) + (4*(-2880*msq2*(msq2 + 2*mU32)*Xt*pow3(mQ3) +
        1440*mQ32*mU32*pow4(msq) + 2880*mQ3*mU32*Xt*pow4(msq) - 720*pow4(msq)*
        pow4(mU3) + pow4(mQ3)*(-720*pow4(msq) + 391*pow4(mU3)) + 480*(12*msq2 -
        5*mU32)*Xt*pow5(mQ3) + (-782*mU32 + 1536*pow2(Xt))*pow6(mQ3) + 2784*Xt*
        pow7(mQ3) + 487*pow8(mQ3)))/(pow2(mQ32 - mU32)*pow4(mQ3)) + (6*dilog(1
        - mQ32/mU32)*(-104*Xt*pow3(mQ3)*pow4(mU3) + 318*pow4(mQ3)*pow4(mU3) -
        2168*mU32*Xt*pow5(mQ3) + 1544*mU32*pow6(mQ3) + 16*mQ32*pow6(mU3) + 24*
        mQ3*Xt*pow6(mU3) + 2248*Xt*pow7(mQ3) - 2649*pow8(mQ3) + 3*pow8(mU3)))/(
        pow2(mQ32 - mU32)*pow4(mQ3)) - (180*dilog(1 - msq2/mQ32)*(16*msq2*mU32*
        (msq2 + mU32)*Xt*pow3(mQ3) - 8*mQ3*Xt*pow4(msq)*pow4(mU3) - 8*Xt*(-28*
        msq2*mU32 + 17*pow4(msq) + 17*pow4(mU3))*pow5(mQ3) + (90*msq2*mU32 -
        47*pow4(msq) + 39*pow4(mU3))*pow6(mQ3) + pow4(mQ3)*(-19*mU32*pow4(msq)
        + 6*msq2*pow4(mU3) - 13*pow6(mU3)) - pow4(msq)*pow6(mU3) + mQ32*(3*
        pow4(msq)*pow4(mU3) - 2*msq2*pow6(mU3)) + 16*(msq2 + mU32)*Xt*pow7(mQ3)
        + 17*(2*msq2 - 7*mU32)*pow8(mQ3) - 8*Xt*pow9(mQ3) + 29*power10(mQ3)))/(
        pow3(mQ32 - mU32)*pow4(mQ3)) - (3*pow2(log(mU32/mQ32))*(-2248*Xt*pow11(
        mQ3) + 2745*pow12(mQ3) - 3*pow12(mU3) + 240*mU32*Xt*(32*msq2*mU32 - 16*
        pow4(msq) + 15*pow4(mU3))*pow5(mQ3) - 4*pow6(mQ3)*(240*mU32*pow4(msq) -
        720*msq2*pow4(mU3) + 512*pow2(Xt)*pow4(mU3) + 215*pow6(mU3)) + 16*Xt*(-
        480*msq2*mU32 + 240*pow4(msq) - 677*pow4(mU3))*pow7(mQ3) + (-1920*msq2*
        mU32 + 1440*pow4(msq) + 3691*pow4(mU3))*pow8(mQ3) + 152*Xt*pow3(mQ3)*
        pow8(mU3) - pow4(mQ3)*(480*pow4(msq)*pow4(mU3) + 257*pow8(mU3)) + 8840*
        mU32*Xt*pow9(mQ3) - 2*(480*msq2 + 2621*mU32)*power10(mQ3) - 10*mQ32*
        power10(mU3) - 24*mQ3*Xt*power10(mU3)))/pow4(-(mQ3*mU32) + pow3(mQ3)))/
        864.;
      }
      case(Limits::MU3_EQ_M3):{
	 return(32080 - (1536*(mQ32 + mU3*(mU3 + 2*Xt))*log(mU32/mQ32))/(mQ32 - mU32) +
        768*pow2(lmQ3MR) - 11520*pow2(log(msq2/mQ32)) - (1152*(mU3 - 2*Xt)*(2*(
        mQ32 - mU32) + (mQ32 + mU32)*log(mU32/mQ32)))/(-(mQ32*mU3) + pow3(mU3))
        - (11520*msq2*(2*(msq2 - mU32) + (msq2 - 3*mU32)*log(msq2/mU32))*(-(
        mQ32*mU3) + 2*msq2*Xt - 2*mU32*Xt + pow3(mU3)))/((-mQ32 + mU32)*(-msq2
        + mU32)*pow3(mU3)) - (48*(9*(80*msq2 - 93*mU32)*mU32 + 10*mQ32*(24*msq2
        + 61*mU32) + 19*pow4(mQ3)))/pow2(mQ32 - mU32) - (11520*(mQ32 - msq2)*
        dilog(1 - msq2/mQ32)*(msq2*mU3*(3*mU3 + 8*Xt) + mQ32*(msq2 - mU3*(3*mU3
        + 8*Xt)) + pow4(mQ3) - 2*pow4(mU3)))/pow3(mQ32 - mU32) + (720*(msq2 -
        mU32)*((mQ32 - mU32)*(msq2 + 3*mU32) + 8*mU3*(msq2 - mU32)*Xt)*dilog(1
        - msq2/mU32))/((mQ32 - mU32)*pow4(mU3)) - (768*Xt*(mQ32*(30*msq2 - 58*
        mU32) + 90*msq2*mU32 + 3*pow4(mQ3) + 23*pow4(mU3)))/(mU3*pow2(mQ32 -
        mU32)) + (384*(2*mQ32*mU32 + pow4(mQ3) - 3*pow4(mU3) + log(mU32/mQ32)*(
        3*mQ32*mU32 + pow4(mU3))))/(-(mQ32*mU32) + pow4(mU3)) - (48*(2*mU3*(
        mQ32 - mU32)*(-120*msq2*mU3*(mU3 - 2*Xt) + mQ32*(120*msq2 + mU3*(-137*
        mU3 + 24*Xt)) + (129*mU3 - 104*Xt)*pow3(mU3) + 12*pow4(mQ3)) + log(
        mU32/mQ32)*pow3(mU3)*(10*mQ32*mU3*(-47*mU3 + 32*Xt) + mU32*(227*mU32 -
        352*mU3*Xt - 256*pow2(Xt)) + 235*pow4(mQ3)) + 20*(mQ32 - mU32)*log(
        msq2/mQ32)*(-12*msq2*mU32*(mU3 - 4*Xt) + mQ32*(12*msq2*mU3 + pow3(mU3))
        - 24*Xt*pow4(msq) - pow5(mU3))))/(pow2(mQ32 - mU32)*pow3(mU3)) - (960*
        log(msq2/mQ32)*(36*msq2*mU32*(mU3 + 2*Xt) + 6*mQ32*(2*msq2*(mU3 + 2*Xt)
        - 15*pow3(mU3)) + 43*mU3*pow4(mQ3) + 47*pow5(mU3)))/(mU3*pow2(mQ32 -
        mU32)) - (128*(3 - log(mU32/mQ32))*((mQ32 - mU32)*(-60*msq2*mU32 + 30*
        pow4(msq) - 37*pow4(mU3)) + 8*Xt*pow5(mU3)))/((mQ32 - mU32)*pow4(mU3))
        + (48*(-2 + log(mU32/mQ32))*(3*(80*msq2*mU3 + 27*pow3(mU3))*pow4(mQ3) +
        480*mU32*Xt*pow4(msq) + 240*msq2*(mU3 - 4*Xt)*pow4(mU3) - 2*mQ32*(240*
        msq2*mU32*(mU3 - 2*Xt) + 240*Xt*pow4(msq) + (81*mU3 - 184*Xt)*pow4(mU3)
        ) + (65*mU32 - 432*mU3*Xt - 256*pow2(Xt))*pow5(mU3)))/(pow2(mQ32 -
        mU32)*pow3(mU3)) - (96*(20*mU32*(3*mQ32 + mU3*(-3*mU3 + 8*Xt))*log(
        msq2/mQ32)*pow2(mQ32 - mU32) + 2*pow2(mQ32 - mU32)*(-60*msq2*mU3*(mU3 -
        8*Xt) + mQ32*(60*msq2 - 663*mU32 + 48*mU3*Xt) + 41*(29*mU3 - 32*Xt)*
        pow3(mU3) + 6*pow4(mQ3)) + mU32*log(mU32/mQ32)*(mQ32*mU32*(-1177*mU32 +
        864*mU3*Xt - 768*pow2(Xt)) + mU3*(625*mU3 - 656*Xt)*pow4(mQ3) + (563*
        mU32 - 336*mU3*Xt + 256*pow2(Xt))*pow4(mU3) - 43*pow6(mQ3))))/(mU32*
        pow3(-mQ32 + mU32)) + (96*log(mU32/mQ32)*(mQ32*mU32*(3971*mU32 - 3328*
        mU3*Xt + 1280*pow2(Xt)) + mU3*(-2867*mU3 + 1856*Xt)*pow4(mQ3) + (-1665*
        mU32 + 1728*mU3*Xt - 256*pow2(Xt))*pow4(mU3) + 625*pow6(mQ3)))/pow3(
        mQ32 - mU32) - (128*lmQ3MR*(120*log(msq2/mQ32)*pow3(mQ32 - mU32) + (
        mQ32 - mU32)*(-4*mQ32*mU3*(511*mU3 + 306*Xt) + (983*mU3 + 1176*Xt)*
        pow3(mU3) + 1037*pow4(mQ3)) + 6*log(mU32/mQ32)*(2*mQ32*(-61*mU3 + 32*
        Xt)*pow3(mU3) + mU3*(61*mU3 - 136*Xt)*pow4(mQ3) + 8*(7*mU3 + 8*Xt)*
        pow5(mU3) + pow6(mQ3))))/pow3(mQ32 - mU32) + (128*(-((3*mQ32 + 30*msq2
        - 10*mU32)*(mQ32 - mU32)*mU32) + 30*msq2*(msq2 - 2*mU32)*(mQ32 - mU32)*
        log(msq2/mQ32) + 8*(-4*mQ32 + mU3*(4*mU3 + Xt))*log(mU32/mQ32)*pow4(
        mU3)))/(-(mQ32*pow4(mU3)) + pow6(mU3)) + (24*dilog(1 - mU32/mQ32)*(-(
        mQ32*(31*mU3 + 80*Xt)*pow3(mU3)) + mU3*(19*mU3 + 24*Xt)*pow4(mQ3) - (
        119*mU3 + 456*Xt)*pow5(mU3) + 3*pow6(mQ3)))/(-(mQ32*pow4(mU3)) + pow6(
        mU3)) - (3840*msq2*(-10*msq2*mU32 + 3*pow4(msq) + 7*pow4(mU3) + log(
        msq2/mU32)*(-3*msq2*mU32 + pow4(msq) + 6*pow4(mU3))))/(-(msq2*pow4(mU3)
        ) + pow6(mU3)) + (3*pow2(log(mU32/mQ32))*(120*(mU3 - 8*Xt)*pow3(mU3)*
        pow4(msq) + pow4(mQ3)*(240*msq2*mU32 + 120*pow4(msq) - 1267*pow4(mU3))
        + 240*msq2*(mU3 + 8*Xt)*pow5(mU3) - 2*mQ32*(240*msq2*(mU3 + 4*Xt)*pow3(
        mU3) + 120*mU3*(mU3 - 4*Xt)*pow4(msq) + (2573*mU3 - 4832*Xt)*pow5(mU3))
        + (6413*mU32 - 7616*mU3*Xt - 512*pow2(Xt))*pow6(mU3)))/(pow2(mQ32 -
        mU32)*pow4(mU3)) + (24*dilog(1 - mU32/mQ32)*(2*(159*mU3 - 52*Xt)*pow3(
        mU3)*pow4(mQ3) + 8*mQ32*(193*mU3 - 271*Xt)*pow5(mU3) + 8*mU3*(2*mU3 +
        3*Xt)*pow6(mQ3) + (-2649*mU3 + 2248*Xt)*pow7(mU3) + 3*pow8(mQ3)))/(
        pow2(mQ32 - mU32)*pow4(mU3)) + (24*log(mU32/mQ32)*(-10*log(msq2/mQ32)*
        pow2(mQ32 - mU32)*((mQ32 - mU32)*(6*msq2*mU32 + 3*pow4(msq) - 5*pow4(
        mU3)) + 8*mU3*Xt*(-6*msq2*mU32 + 3*pow4(msq) + 2*pow4(mU3))) + 8*Xt*
        pow3(mU3)*(5*(6*msq2 + 113*mU32)*pow4(mQ3) + 30*msq2*pow4(mU3) - 3*
        mQ32*(20*msq2*mU32 + 409*pow4(mU3)) + 3*pow6(mQ3) + 627*pow6(mU3)) +
        mU32*(-10*pow4(mQ3)*(9*msq2*mU32 + 325*pow4(mU3)) + 6*(5*msq2 + 2*mU32)
        *pow6(mQ3) - 3*(10*msq2 + 971*mU32)*pow6(mU3) + 18*mQ32*(5*msq2*pow4(
        mU3) + 338*pow6(mU3)) + 3*pow8(mQ3))))/(pow3(mQ32 - mU32)*pow4(mU3)) +
        (3*pow2(log(mU32/mQ32))*(4*(240*msq2*(mU3 + 2*Xt)*pow3(mU3) + 120*mU3*(
        mU3 - 2*Xt)*pow4(msq) + (-8781*mU3 + 12800*Xt)*pow5(mU3))*pow6(mQ3) +
        pow4(mQ3)*(-240*(11*mU3 - 12*Xt)*pow3(mU3)*pow4(msq) + 480*msq2*(21*mU3
        + 52*Xt)*pow5(mU3) + 2*(49527*mU32 - 43840*mU3*Xt + 9984*pow2(Xt))*
        pow6(mU3)) + (-240*msq2*mU32*(17*mU3 + 8*Xt) + (23053*mU32 - 7808*mU3*
        Xt - 512*pow2(Xt))*pow3(mU3) + 120*(47*mU3 + 136*Xt)*pow4(msq))*pow7(
        mU3) + (-240*msq2*mU32 - 120*pow4(msq) + 1837*pow4(mU3))*pow8(mQ3) -
        20*mQ32*(24*(7*mU3 + 38*Xt)*pow4(msq)*pow5(mU3) + 48*msq2*(7*mU3 + 26*
        Xt)*pow7(mU3) + (4377*mU32 - 2624*mU3*Xt - 256*pow2(Xt))*pow8(mU3))))/
        pow4(-(mQ32*mU3) + pow3(mU3)) + (720*dilog(1 - msq2/mU32)*((47*mU3 +
        136*Xt)*pow4(msq)*pow5(mU3) - pow4(mQ3)*(2*msq2*(3*mU3 + 8*Xt)*pow3(
        mU3) + mU3*(3*mU3 - 8*Xt)*pow4(msq) + (39*mU3 - 136*Xt)*pow5(mU3)) + (
        2*msq2*mU32 + pow4(msq) + 13*pow4(mU3))*pow6(mQ3) - 2*msq2*(17*mU3 + 8*
        Xt)*pow7(mU3) + mQ32*((19*mU3 - 16*Xt)*pow3(mU3)*pow4(msq) - 2*msq2*(
        45*mU3 + 112*Xt)*pow5(mU3) + (119*mU3 - 16*Xt)*pow7(mU3)) + (-29*mU3 +
        8*Xt)*pow9(mU3)))/(pow3(-mQ32 + mU32)*pow4(mU3)) - (6*log(mU32/mQ32)*(
        4*(mQ32 - mU32)*mU32*(-2*(45*msq2*mU3*(mU3 + 8*Xt) + (11243*mU3 - 5748*
        Xt)*pow3(mU3))*pow4(mQ3) + 30*msq2*(47*mU3 + 104*Xt)*pow5(mU3) + 2*
        mQ32*(15*msq2*(19*mU3 + 48*Xt)*pow3(mU3) + 4*(3693*mU3 - 1951*Xt)*pow5(
        mU3)) + (30*msq2 + 8*mU3*(497*mU3 - 9*Xt))*pow6(mQ3) + 3*(-3807*mU3 +
        968*Xt)*pow7(mU3) + 3*pow8(mQ3)) + log(mU32/mQ32)*pow4(mU3)*(2*mU32*(
        25391*mU32 - 28144*mU3*Xt + 2816*pow2(Xt))*pow4(mQ3) + 4*mQ32*(-12005*
        mU32 + 14088*mU3*Xt + 768*pow2(Xt))*pow4(mU3) - 4*mU3*(4621*mU3 - 5624*
        Xt)*pow6(mQ3) + (15341*mU32 - 19488*mU3*Xt - 512*pow2(Xt))*pow6(mU3) +
        1149*pow8(mQ3)) - 40*(mQ32 - mU32)*log(msq2/mQ32)*(3*(47*mU3 + 136*Xt)*
        pow4(msq)*pow5(mU3) - pow4(mQ3)*(6*msq2*(3*mU3 + 8*Xt)*pow3(mU3) + 3*
        mU3*(3*mU3 - 8*Xt)*pow4(msq) + (33*mU3 - 80*Xt)*pow5(mU3)) + (6*msq2*
        mU32 + 3*pow4(msq) + 11*pow4(mU3))*pow6(mQ3) - 6*msq2*(17*mU3 + 8*Xt)*
        pow7(mU3) + mQ32*((57*mU3 - 48*Xt)*pow3(mU3)*pow4(msq) - 6*msq2*(45*mU3
        + 112*Xt)*pow5(mU3) + (49*mU3 - 96*Xt)*pow7(mU3)) + (-27*mU3 + 16*Xt)*
        pow9(mU3))))/pow4(-(mQ32*mU3) + pow3(mU3)))/3456.;
      }
      case(Limits::MQ3_EQ_MU3_EQ_M3):{
	 return (1835*mQ32*msq2 - 232*mQ3*msq2*Xt + 780*mQ32*msq2*log(msq2/mQ32) - 360*
        mQ3*msq2*Xt*log(msq2/mQ32) - 4*lmQ3MR*mQ3*(mQ32 - msq2)*(335*mQ3 + 104*
        Xt + 60*mQ3*log(msq2/mQ32)) + 96*mQ32*pow2(Xt) - 96*msq2*pow2(Xt) +
        180*mQ32*msq2*pow2(log(msq2/mQ32)) + 232*Xt*pow3(mQ3) - 120*Xt*log(
        msq2/mQ32)*pow3(mQ3) - 1835*pow4(mQ3) - 540*log(msq2/mQ32)*pow4(mQ3) -
        180*pow2(log(msq2/mQ32))*pow4(mQ3) - 360*dilog(1 - msq2/mQ32)*(-(mQ32*
        msq2) + pow4(mQ3)) + 12*pow2(lmQ3MR)*(-(mQ32*msq2) + pow4(mQ3)))/(54.*
        mQ32*(mQ32 - msq2));
      }
      case(Limits::DEGENERATE):{
	 return (-2075*mQ32 + 712*mQ3*Xt - 4*lmQ3MR*mQ3*(335*mQ3 + 104*Xt) + 12*mQ32*
        pow2(lmQ3MR) + 96*pow2(Xt))/(54.*mQ32);
      }
   };
   
   throw std::runtime_error("Mass limit not included!");
}

/**
 * 	Returns delta lambda_at in the MSbar scheme for a given mass limit
 * 	@param limit an integer key for a mass limit
 * 	@param omitLogs an integer key to omit all mu terms
 * 	@return delta lambda_at in the MSbar scheme for a given mass limit
 */
double himalaya::ThresholdCalculator::getDeltaLambdaAlphat(int limit, int omitLogs){

   using std::log;
   using std::sqrt;
   
   const double Mst12 = pow2(p.MSt(0));
   const double MR2 = pow2(p.scale);
   const double mU32 = p.mu2(2,2);
   const double mQ32 = p.mq2(2,2);
   const double Xt = p.Au(2,2) - p.mu * p.vd / p.vu;
   const double lmQ3MR = omitLogs * log(Mst12 / MR2) + log(mQ32 / Mst12);
   
   // Translate limits to occurring ones
   if(limit == Limits::DEGENERATE) limit = Limits::MQ3_EQ_MU3_EQ_M3;
   if(limit == Limits::MQ3_EQ_M3 || limit == Limits::MU3_EQ_M3) limit = Limits::GENERAL;
   if(limit == Limits::MQ3_EQ_MU3_EQ_M3) limit = Limits::MQ3_EQ_MU3;
   
   switch (limit){
      case(Limits::GENERAL):{
	 return 12*pow4(Xt)/pow2(mQ32 - mU32) + 12*lmQ3MR + (6 - 12*pow2(Xt)/(mQ32 - mU32)
	    + (6*(mQ32 + mU32)/pow3(mQ32 - mU32))*pow4(Xt))*log(mU32/mQ32);
      }
      case(Limits::MQ3_EQ_MU3):{
	 return 12*lmQ3MR + 12*pow2(Xt)/mQ32 - pow4(Xt)/pow2(mQ32);
      }
   };
   
   throw std::runtime_error("Mass limit not included!");
}

/**
 * 	Returns delta lambda_atas in the MSbar scheme for a given mass limit
 * 	@param limit an integer key for a mass limit
 * 	@param omitLogs an integer key to omit all mu terms
 * 	@return delta lambda_atas in the MSbar scheme for a given mass limit
 */
double himalaya::ThresholdCalculator::getDeltaLambdaAlphatAlphas(int limit, int omitLogs){
   using std::log;
   using std::sqrt;
   using gm2calc::dilog;
   
   const double Mst12 = pow2(p.MSt(0));
   const double m32 = pow2(p.MG);
   const double MR2 = pow2(p.scale);
   const double mU32 = p.mu2(2,2);
   const double mQ32 = p.mq2(2,2);
   const double m3 = sqrt(m32);
   const double mU3 = sqrt(mU32);
   const double mQ3 = sqrt(mQ32);
   const double Xt = p.Au(2,2) - p.mu * p.vd / p.vu;
   const double lmQ3MR = omitLogs * log(Mst12 / MR2) + log(mQ32 / Mst12);

   if(limit == Limits::DEGENERATE) limit = Limits::MQ3_EQ_MU3_EQ_M3;
   
   switch (limit){
      case(Limits::GENERAL):{
	 return 16*(-6*pow2(lmQ3MR) - (4*m32*pow2(Xt))/(pow2(mQ3)*pow2(mU3)) - (32*m3*
        pow3(Xt))/pow2(mQ32 - mU32) - (2*dilog(1 - m32/pow2(mQ3))*(-8*(m32 -
        mQ32)*Xt*pow2(mQ32 - mU32)*pow3(m3) - 4*m3*(2*m32 - mQ32 - mU32)*pow2(
        m32 - mQ32)*pow3(Xt) + 2*pow3(mQ32 - mU32)*pow4(m3) - (2*m32 - mQ32 -
        mU32)*pow2(m32 - mQ32)*pow4(Xt)))/(pow2(m32 - mQ32)*pow3(mQ32 - mU32))
        - (2*dilog(1 - m32/pow2(mU3))*(8*(m32 - mU32)*Xt*pow2(mQ32 - mU32)*
        pow3(m3) + 4*m3*(2*m32 - mQ32 - mU32)*pow2(m32 - mU32)*pow3(Xt) + 2*
        pow3(mQ32 - mU32)*pow4(m3) + (2*m32 - mQ32 - mU32)*pow2(m32 - mU32)*
        pow4(Xt)))/(pow2(m32 - mU32)*pow3(mQ32 - mU32)) + (-6*m32*mQ32*mU32*(
        mQ32 + mU32) + 3*pow4(mQ3)*pow4(mU3) + pow4(m3)*(9*mQ32*mU32 + 2*pow4(
        mQ3) + 2*pow4(mU3)) - 2*(mQ32 + mU32)*pow6(m3))/((-m32 + mQ32)*(m32 -
        mU32)*pow2(mQ3)*pow2(mU3)) + (2*pow4(Xt)*(-5*m32*mQ32*mU32*(mQ32 +
        mU32) + 5*pow4(mQ3)*pow4(mU3) - pow4(m3)*(-5*mQ32*mU32 + pow4(mQ3) +
        pow4(mU3)) + (mQ32 + mU32)*pow6(m3)))/((m32 - mQ32)*(m32 - mU32)*pow2(
        mQ3)*pow2(mU3)*pow2(mQ32 - mU32)) + (2*lmQ3MR*(((mQ32 - mU32)*(8*m3*
        mQ32*mU32*pow3(Xt) + 2*mQ32*mU32*(-2*mQ32*mU32 + pow4(mQ3) + pow4(mU3)
        - 3*pow4(Xt)) - m32*(pow2(-(mU3*pow2(Xt)) + pow3(mU3)) - (mU32 + 2*
        pow2(Xt))*pow4(mQ3) + mQ32*(4*mU32*pow2(Xt) - pow4(mU3) + pow4(Xt)) +
        pow6(mQ3))))/(pow2(mQ3)*pow2(mU3)) - log(mU32/pow2(mQ3))*(mU32*(-4*m3 +
        3*Xt)*pow3(Xt) + (-9*mU32 + 4*m3*Xt - 6*pow2(Xt))*pow4(mQ3) + 2*(2*m3 -
        3*Xt)*Xt*pow4(mU3) + mQ32*(4*mU32*Xt*(-2*m3 + 3*Xt) + (-4*m3 + 3*Xt)*
        pow3(Xt) + 9*pow4(mU3)) + 2*m32*pow4(Xt) + 3*pow6(mQ3) - 3*pow6(mU3))))
        /pow3(mQ32 - mU32) - (pow2(log(mU32/pow2(mQ3)))*(4*(m32 - mU32)*Xt*(-(
        m3*mU32) + 2*pow3(m3))*pow3(mQ32 - mU32) + 4*m3*(m32 - mU32)*(mQ32 -
        mU32)*pow3(Xt)*(3*mQ32*mU32 - m32*(mQ32 + 3*mU32) + 2*pow4(m3) - pow4(
        mU3)) - 2*pow2(mQ32 - mU32)*pow2(Xt)*((mQ32 - 3*mU32)*pow4(m3) + 2*(
        mQ32 - 2*mU32)*pow4(mU3) + m32*(-4*mQ32*mU32 + 8*pow4(mU3))) + (-4*m32*
        mU32 + 3*pow4(m3) + 2*pow4(mU3))*pow4(mQ32 - mU32) - 4*m3*(m32 - mU32)*
        mU32*(mQ32 + mU32)*pow5(Xt) + pow4(Xt)*(-8*mQ32*mU32*pow4(m3) + (-4*
        mQ32*mU32 + pow4(mQ3) - 5*pow4(mU3))*pow4(mU3) + 2*(mQ32 - mU32)*pow6(
        m3) + m32*(-2*mU32*pow4(mQ3) + 10*mQ32*pow4(mU3) + 8*pow6(mU3)))))/(
        pow2(m32 - mU32)*pow4(mQ32 - mU32)) + (log(mU32/pow2(mQ3))*(8*m3*(m32 -
        mQ32)*Xt*pow2(m32 - mU32)*pow2(mQ32 - mU32) - 8*m3*(m32 - mQ32)*(mQ32 +
        3*mU32)*pow2(m32 - mU32)*pow3(Xt) - 2*(m32 - mU32)*pow2(mQ32 - mU32)*
        pow2(Xt)*(3*mQ32*mU32 - 2*m32*(3*mQ32 + 2*mU32) + 7*pow4(m3)) + 8*m3*(
        m32 - mQ32)*(m32 - mU32)*mU32*pow5(Xt) + pow3(mQ32 - mU32)*(-((6*mQ32 +
        7*mU32)*pow4(m3)) - 2*mQ32*pow4(mU3) + m32*(5*mQ32*mU32 + 3*pow4(mU3))
        + 7*pow6(m3)) + pow4(Xt)*(-3*mQ32*(mQ32 + 5*mU32)*pow4(mU3) - pow4(m3)*
        (15*mQ32*mU32 + 6*pow4(mQ3) + 29*pow4(mU3)) + (3*mQ32 + 7*mU32)*pow6(
        m3) + m32*(7*mU32*pow4(mQ3) + 31*mQ32*pow4(mU3) + 16*pow6(mU3)) + 4*
        pow8(m3))))/((m32 - mQ32)*pow2(m32 - mU32)*pow3(mQ32 - mU32)) - (m32*
        log(m32/pow2(mQ3))*(-4*m32*(m32 - mQ32)*(m32 - mU32)*(m32 - mQ32 -
        mU32)*pow2(Xt)*pow3(mQ32 - mU32) - 8*m3*(m32 - mQ32)*mQ32*(m32 - mU32)*
        (mQ32 - mU32)*mU32*pow5(Xt) + m3*mQ32*mU32*log(mU32/pow2(mQ3))*(-4*(m32
        - mQ32)*(m32 - mU32)*(2*m32 - mQ32 - mU32)*Xt*pow2(mQ32 - mU32) - 8*(
        m32 - mQ32)*(m32 - mU32)*pow3(Xt)*(4*mQ32*mU32 - 2*m32*(mQ32 + mU32) +
        2*pow4(m3) - pow4(mQ3) - pow4(mU3)) - 2*m3*pow2(mQ32 - mU32)*pow2(Xt)*(
        -2*m32*(mQ32 + mU32) + 2*pow4(m3) + pow4(mQ3) + pow4(mU3)) + m3*(2*m32
        - mQ32 - mU32)*pow4(mQ32 - mU32) + m3*(mQ32 + mU32)*(-2*m32*(mQ32 +
        mU32) + 2*pow4(m3) + pow4(mQ3) + pow4(mU3))*pow4(Xt) - 4*(m32 - mQ32)*(
        m32 - mU32)*(mQ32 + mU32)*pow5(Xt)) + m32*pow3(mQ32 - mU32)*(2*m32*(
        mQ32 + mU32)*pow2(mQ32 - mU32) + pow4(m3)*(2*mQ32*mU32 - 4*pow4(mQ3) -
        4*pow4(mU3)) + mQ32*mU32*(pow4(mQ3) + pow4(mU3)) + 2*(mQ32 + mU32)*
        pow6(m3)) + 2*(mQ32 - mU32)*pow4(Xt)*(pow3(mQ32 + mU32)*pow4(m3) + (
        mQ32 + mU32)*pow4(mQ3)*pow4(mU3) - m32*mQ32*mU32*(4*mQ32*mU32 + pow4(
        mQ3) + pow4(mU3)) - 2*(mQ32*mU32 + pow4(mQ3) + pow4(mU3))*pow6(m3) + (
        mQ32 + mU32)*pow8(m3))))/(pow2(mQ3)*pow2(m32 - mQ32)*pow2(mU3)*pow2(m32
        - mU32)*pow3(mQ32 - mU32)));
      }
      case(Limits::MQ3_EQ_MU3):{
	 return (-8*(48*(m3 - 2*Xt)*dilog(1 - m32/pow2(mQ3))*pow3(m3)*pow6(mQ3) + 4*log(
        m32/pow2(mQ3))*pow3(m3)*(2*m32*mQ32*Xt*(-6*mQ32 + pow2(Xt)) - 16*pow3(
        Xt)*pow4(mQ3) + pow3(m3)*(-6*mQ32*pow2(Xt) + 6*pow4(mQ3) + pow4(Xt)) +
        mQ32*pow5(Xt) - 12*Xt*pow6(mQ3) + m3*(18*pow2(Xt)*pow4(mQ3) - 2*mQ32*
        pow4(Xt) + 3*pow6(mQ3))) + (m32 - mQ32)*(-8*(-1 + lmQ3MR)*mQ32*Xt*(6*
        mQ32 - pow2(Xt))*pow3(m3) + 4*(-1 + lmQ3MR)*pow4(m3)*(-6*mQ32*pow2(Xt)
        + 6*pow4(mQ3) + pow4(Xt)) - pow4(mQ3)*(12*(-1 + 6*lmQ3MR)*mQ32*pow2(Xt)
        + 6*(3 - 4*lmQ3MR + 6*pow2(lmQ3MR))*pow4(mQ3) + (1 - 6*lmQ3MR)*pow4(Xt)
        ) + m3*(-8*(-8 + lmQ3MR)*pow3(Xt)*pow4(mQ3) - 4*mQ32*pow5(Xt) + 48*(-1
        + lmQ3MR)*Xt*pow6(mQ3)) + m32*(12*(-7 + 8*lmQ3MR)*pow2(Xt)*pow4(mQ3) +
        (9 - 10*lmQ3MR)*mQ32*pow4(Xt) + 6*(9 - 8*lmQ3MR + 6*pow2(lmQ3MR))*pow6(
        mQ3)))))/(3.*pow2(m32 - mQ32)*pow6(mQ3));
      }
      case(Limits::MQ3_EQ_M3):{
	 return 4*(-23 + (8*(3*mQ32 - 3*mU32 - 8*mQ3*Xt))/(mQ32 - mU32) - 24*pow2(lmQ3MR)
        - (16*pow2(Xt))/(mQ32 - mU32) - (16*pow2(Xt))/pow2(mU3) - (128*mQ3*
        pow3(Xt))/pow2(mQ32 - mU32) + (-19*mQ32*mU32 + 8*pow4(mQ3) + 7*pow4(
        mU3))/(mQ32*mU32 - pow4(mU3)) - (10*pow4(Xt))/pow2(mQ32 - mU32) + (2*(
        27*mQ32*mU32 + 4*pow4(mQ3) - 27*pow4(mU3))*pow4(Xt))/(pow2(mU3)*pow3(
        mQ32 - mU32)) - (8*dilog(1 - mQ32/pow2(mU3))*(8*Xt*pow3(mQ3) + 4*mQ3*
        pow3(Xt) + 2*pow4(mQ3) + pow4(Xt)))/pow2(mQ32 - mU32) + (log(mU32/pow2(
        mQ3))*(-2*pow2(mQ32 - mU32)*pow2(Xt) + pow3(mQ32 - mU32) + (mQ32 +
        mU32)*pow4(Xt)))/pow3(mQ32 - mU32) + 8*lmQ3MR*(1 - mQ32/pow2(mU3) + (2*
        pow2(Xt))/pow2(mU3) + (8*mQ3*pow3(Xt))/pow2(mQ32 - mU32) - ((mQ32 + 7*
        mU32)*pow4(Xt))/pow2(-(mQ32*mU3) + pow3(mU3)) - (log(mU32/pow2(mQ3))*(
        4*mQ3*Xt*pow2(mQ32 - mU32) - 6*pow2(mQ32 - mU32)*pow2(Xt) + 3*pow3(mQ32
        - mU32) - 4*mQ3*mU32*pow3(Xt) - 4*pow3(mQ3)*pow3(Xt) + 5*mQ32*pow4(Xt)
        + 3*mU32*pow4(Xt)))/pow3(mQ32 - mU32)) + (32*mQ3*pow5(Xt))/pow3(mQ32 -
        mU32) + (log(mU32/pow2(mQ3))*(-6*(9*mQ32 - 5*mU32)*pow2(mQ32 - mU32)*
        pow2(Xt) + 32*mQ3*Xt*pow3(mQ32 - mU32) - 32*mQ3*(mQ32 - mU32)*(mQ32 +
        3*mU32)*pow3(Xt) + pow2(mQ32 - mU32)*(-26*mQ32*mU32 + 27*pow4(mQ3) +
        11*pow4(mU3)) + (36*mQ32*mU32 + 43*pow4(mQ3) - 63*pow4(mU3))*pow4(Xt) +
        32*mQ3*mU32*pow5(Xt)))/pow4(mQ32 - mU32) - (2*log(mU32/pow2(mQ3))*(-8*
        mQ3*Xt*pow3(mQ32 - mU32) - 6*pow2(Xt)*pow3(mQ32 - mU32) + 16*mQ3*pow2(
        mQ32 - mU32)*pow3(Xt) + 3*pow4(mQ32 - mU32) + 3*(pow4(mQ3) - pow4(mU3))
        *pow4(Xt) - 8*mQ3*(mQ32 + mU32)*pow5(Xt)))/pow4(mQ32 - mU32) - (4*pow2(
        log(mU32/pow2(mQ3)))*(4*Xt*(-(mQ3*mU32) + 2*pow3(mQ3))*pow3(mQ32 -
        mU32) + 4*mQ3*(mQ32 + mU32)*pow2(mQ32 - mU32)*pow3(Xt) + pow3(mQ32 -
        mU32)*(-4*mQ32*mU32 + 3*pow4(mQ3) + 2*pow4(mU3)) - 2*pow2(mQ32 - mU32)*
        pow2(Xt)*(-6*mQ32*mU32 + pow4(mQ3) + 4*pow4(mU3)) - 4*mQ3*mU32*(mQ32 +
        mU32)*pow5(Xt) + pow4(Xt)*(-10*mU32*pow4(mQ3) + mQ32*pow4(mU3) + 2*
        pow6(mQ3) + 5*pow6(mU3))))/pow5(mQ32 - mU32));
      }
      case(Limits::MU3_EQ_M3):{
	 return (-8*(4*mQ32*dilog(1 - mU32/pow2(mQ3))*pow3(mQ32 - mU32)*(8*Xt*pow3(mU3) +
        4*mU3*pow3(Xt) + 2*pow4(mU3) + pow4(Xt)) + pow2(mQ32 - mU32)*(-4*(-1 +
        lmQ3MR)*pow2(mU32 - pow2(Xt))*pow4(mU3) + pow4(mQ3)*(8*(-1 + 3*lmQ3MR)*
        mU32*pow2(Xt) + 64*Xt*pow3(mU3) - 32*(-2 + lmQ3MR)*mU3*pow3(Xt) + (25 -
        24*lmQ3MR + 36*pow2(lmQ3MR))*pow4(mU3) + 2*(-11 + 14*lmQ3MR)*pow4(Xt))
        + mQ32*mU3*(-8*(-2 + 3*lmQ3MR)*pow2(Xt)*pow3(mU3) + 32*(-2 + lmQ3MR)*
        mU32*pow3(Xt) - 32*Xt*pow4(mU3) + 2*(11 - 12*lmQ3MR)*mU3*pow4(Xt) + (-
        17 + 16*lmQ3MR - 12*pow2(lmQ3MR))*pow5(mU3) + 16*pow5(Xt)) - (32*mU3*Xt
        + mU32*(15 - 16*lmQ3MR + 36*pow2(lmQ3MR)) + 8*lmQ3MR*pow2(Xt))*pow6(
        mQ3) + (3 - 4*lmQ3MR + 12*pow2(lmQ3MR))*pow8(mQ3)) + 2*mQ32*pow2(log(
        mU32/pow2(mQ3)))*(mU3*pow4(mQ3)*(-38*pow2(Xt)*pow3(mU3) + 12*mU32*pow3(
        Xt) + 12*Xt*pow4(mU3) - 3*mU3*pow4(Xt) - 23*pow5(mU3) + 4*pow5(Xt)) +
        mQ32*pow3(mU3)*(32*pow2(Xt)*pow3(mU3) + 12*mU32*pow3(Xt) - 4*Xt*pow4(
        mU3) - 4*mU3*pow4(Xt) + 13*pow5(mU3) + 4*pow5(Xt)) + (20*mU32*pow2(Xt)
        - 12*Xt*pow3(mU3) - 12*mU3*pow3(Xt) + 21*pow4(mU3) + pow4(Xt))*pow6(
        mQ3) - (10*mU32*pow2(Xt) + 12*mU3*pow3(Xt) + 3*pow4(mU3) - 8*pow4(Xt))*
        pow6(mU3) - 2*(5*mU32 - 2*mU3*Xt + 2*pow2(Xt))*pow8(mQ3) + 2*power10(
        mQ3)) + (mQ32 - mU32)*log(mU32/pow2(mQ3))*(mQ32*pow3(mU3)*(2*(5 + 12*
        lmQ3MR)*pow2(Xt)*pow3(mU3) + 16*(-4 + lmQ3MR)*mU32*pow3(Xt) - 8*(-3 +
        2*lmQ3MR)*Xt*pow4(mU3) + (39 - 20*lmQ3MR)*mU3*pow4(Xt) + 3*(-3 + 4*
        lmQ3MR)*pow5(mU3) + 8*pow5(Xt)) + 2*mU3*pow4(mQ3)*(3*(1 - 12*lmQ3MR)*
        pow2(Xt)*pow3(mU3) + 32*mU32*pow3(Xt) + 12*(-3 + 2*lmQ3MR)*Xt*pow4(mU3)
        + (-17 + 4*lmQ3MR)*mU3*pow4(Xt) + (5 - 24*lmQ3MR)*pow5(mU3) + 12*pow5(
        Xt)) + (2*(-5 + 36*lmQ3MR)*mU32*pow2(Xt) + 24*(3 - 2*lmQ3MR)*Xt*pow3(
        mU3) - 16*lmQ3MR*mU3*pow3(Xt) + 2*(-5 + 36*lmQ3MR)*pow4(mU3) + (-1 +
        12*lmQ3MR)*pow4(Xt))*pow6(mQ3) + 4*pow2(mU32 - pow2(Xt))*pow6(mU3) + 2*
        ((3 - 24*lmQ3MR)*mU32 + 4*(-3 + 2*lmQ3MR)*mU3*Xt + (1 - 12*lmQ3MR)*
        pow2(Xt))*pow8(mQ3) + (-1 + 12*lmQ3MR)*power10(mQ3))))/(pow2(mQ3)*pow5(
        mQ32 - mU32));
      }
      case(Limits::MQ3_EQ_MU3_EQ_M3):{
	 return (-8*(-2*lmQ3MR*mQ3*Xt*(-24*mQ32*Xt - 4*mQ3*pow2(Xt) + 24*pow3(mQ3) +
        pow3(Xt)) + Xt*(-28*mQ32*pow2(Xt) + 12*Xt*pow3(mQ3) - mQ3*pow3(Xt) +
        24*pow4(mQ3) + 2*pow4(Xt)) + 36*pow2(lmQ3MR)*pow5(mQ3)))/(3.*pow5(mQ3));
      }
   };
   
   throw std::runtime_error("Mass limit not included!");
}

/**
 * 	Returns delta lambda_atas2 in the MSbar scheme for a given mass limit
 * 	@param limit an integer key for a mass limit
 * 	@param omitLogs an integer key to omit all mu terms
 * 	@return delta lambda_atas2 in the MSbar scheme for a given mass limit
 */
double himalaya::ThresholdCalculator::getDeltaLambdaAlphatAlphas2(int limit, int omitLogs){

   using std::log;
   using std::sqrt;
   using gm2calc::dilog;
   
   const double Mst12 = pow2(p.MSt(0));
   const double m32 = pow2(p.MG);
   const double MR2 = pow2(p.scale);
   const double mU32 = p.mu2(2,2);
   const double mQ32 = p.mq2(2,2);
   const double m3 = sqrt(m32);
   const double mU3 = sqrt(mU32);
   const double mQ3 = sqrt(mQ32);
   const double Xt = p.Au(2,2) - p.mu * p.vd / p.vu;
   const double lmQ3MR = omitLogs * log(Mst12 / MR2) + log(mQ32 / Mst12);
   const double dlatas2Const = 0.;
   const double z3 = 1.202056903159594;
   
   if(limit == Limits::DEGENERATE) limit = Limits::MQ3_EQ_MU3_EQ_M3;
   
   switch (limit){
      case(Limits::GENERAL):{
      return dlatas2Const + 736*pow3(lmQ3MR) + (16*pow2(lmQ3MR)*(-((mQ32 - mU32)*(16*
        pow2(mQ32 - mU32)*pow4(m3)*(-2*mQ32*pow2(Xt) + pow2(mU32 - pow2(Xt)) +
        pow4(mQ3)) + 1104*m3*pow3(Xt)*pow4(mQ3)*pow4(mU3) - 128*mQ32*mU32*Xt*
        pow3(m3)*(-(mU32*pow2(Xt)) - mQ32*(2*mU32 + pow2(Xt)) + pow4(mQ3) +
        pow4(mU3)) + 3*pow4(mQ3)*pow4(mU3)*(-254*mQ32*mU32 + 127*pow4(mQ3) +
        127*pow4(mU3) - 138*pow4(Xt)) - 3*m32*mQ32*mU32*(49*pow2(-(mU3*pow2(Xt)
        ) + pow3(mU3)) - 49*(mU32 + 2*pow2(Xt))*pow4(mQ3) + mQ32*(324*mU32*
        pow2(Xt) - 49*pow4(mU3) + 49*pow4(Xt)) + 49*pow6(mQ3)))) + log(mU32/
        pow2(mQ3))*pow4(mQ3)*pow4(mU3)*(-256*pow3(m3)*pow3(Xt) + 552*m3*Xt*(-(
        mU32*pow2(Xt)) - mQ32*(2*mU32 + pow2(Xt)) + pow4(mQ3) + pow4(mU3)) +
        m32*(192*mU32*pow2(Xt) + 64*mQ32*(2*mU32 + 3*pow2(Xt)) - 64*pow4(mQ3) -
        64*pow4(mU3) + 294*pow4(Xt)) + 207*(-((3*mU32 + 2*pow2(Xt))*pow4(mQ3))
        - 2*pow2(Xt)*pow4(mU3) + mU32*pow4(Xt) + mQ32*(4*mU32*pow2(Xt) + 3*
        pow4(mU3) + pow4(Xt)) + pow6(mQ3) - pow6(mU3)))))/(3.*pow3(mQ32 - mU32)
        *pow4(mQ3)*pow4(mU3)) + (8*lmQ3MR*(-512*mQ32*mU32*Xt*pow3(m3)*pow3(m32
        - mQ32)*pow3(m32 - mU32)*pow4(mQ32 - mU32) + 48*z3*pow3(-m32 + mQ32)*
        pow3(m32 - mU32)*pow4(mQ3)*pow4(mU3)*pow4(mQ32 - mU32) + 4*(mQ32 -
        mU32)*dilog(1 - m32/pow2(mQ3))*pow3(m32 - mU32)*pow4(mQ3)*pow4(mU3)*(
        192*m32*(2*m32 - mQ32 - mU32)*pow2(Xt)*pow3(m32 - mQ32) - 8*(-31*m3*(
        mQ32 + mU32) + 64*pow3(m3))*pow3(m32 - mQ32)*pow3(Xt) - 2*pow2(mQ32 -
        mU32)*pow4(m3)*(-105*mQ32*mU32 + m32*(27*mQ32 + 101*mU32) - 64*pow4(m3)
        + 41*pow4(mQ3)) - (142*m32 - 69*(mQ32 + mU32))*pow3(m32 - mQ32)*pow4(
        Xt) - 16*(m32 - mQ32)*Xt*pow2(mQ32 - mU32)*(-48*mQ32*pow3(m3) + 47*
        pow5(m3))) - 4*(mQ32 - mU32)*dilog(1 - m32/pow2(mU3))*pow3(m32 - mQ32)*
        pow4(mQ3)*pow4(mU3)*(192*m32*(2*m32 - mQ32 - mU32)*pow2(Xt)*pow3(m32 -
        mU32) - 8*(-31*m3*(mQ32 + mU32) + 64*pow3(m3))*pow3(m32 - mU32)*pow3(
        Xt) + 2*pow2(mQ32 - mU32)*pow4(m3)*(105*mQ32*mU32 - m32*(101*mQ32 + 27*
        mU32) + 64*pow4(m3) - 41*pow4(mU3)) - (142*m32 - 69*(mQ32 + mU32))*
        pow3(m32 - mU32)*pow4(Xt) - 16*(m32 - mU32)*Xt*pow2(mQ32 - mU32)*(-48*
        mU32*pow3(m3) + 47*pow5(m3))) - 32*pow2(m32 - mQ32)*pow2(m32 - mU32)*
        pow2(mQ32 - mU32)*pow3(m3)*pow4(mQ3)*pow4(mU3)*pow5(Xt) + 20*m3*mQ32*
        mU32*log(msq2/pow2(mQ3))*pow2(mQ32 - mU32)*pow3(m32 - mQ32)*pow3(m32 -
        mU32)*(-8*mQ32*mU32*pow3(Xt) + m3*(pow2(-(mU3*pow2(Xt)) + pow3(mU3)) -
        (mU32 + 2*pow2(Xt))*pow4(mQ3) + mQ32*(4*mU32*pow2(Xt) - pow4(mU3) +
        pow4(Xt)) + pow6(mQ3))) + 32*mQ32*mU32*pow2(m32 - mQ32)*pow2(m32 -
        mU32)*pow2(mQ32 - mU32)*pow3(Xt)*(-193*mQ32*mU32*(mQ32 + mU32)*pow3(m3)
        + 201*m3*pow4(mQ3)*pow4(mU3) + (185*mQ32*mU32 - 16*pow4(mQ3) - 16*pow4(
        mU3))*pow5(m3) + 16*(mQ32 + mU32)*pow7(m3)) - 4*pow2(m32 - mQ32)*pow2(
        m32 - mU32)*pow2(mQ32 - mU32)*pow2(Xt)*(3*(mQ32 + 10*msq2 + mU32)*pow2(
        mQ32 - mU32)*pow4(mQ3)*pow4(mU3) + mQ32*mU32*pow4(m3)*((30*msq2 - 1067*
        mU32)*pow4(mQ3) + 30*msq2*pow4(mU3) - mQ32*(60*msq2*mU32 + 1067*pow4(
        mU3)) + 299*pow6(mQ3) + 299*pow6(mU3)) + 32*(mQ32 + mU32)*pow2(mQ32 -
        mU32)*pow8(m3) - 3*m32*mQ32*mU32*(-2*pow4(mQ3)*(5*msq2*mU32 + 213*pow4(
        mU3)) + 2*(5*msq2 + 42*mU32)*pow6(mQ3) + 10*msq2*pow6(mU3) + mQ32*(-10*
        msq2*pow4(mU3) + 84*pow6(mU3)) + pow8(mQ3) + pow8(mU3)) - 8*pow6(m3)*(-
        174*pow4(mQ3)*pow4(mU3) + 35*mU32*pow6(mQ3) + 35*mQ32*pow6(mU3) + 4*
        pow8(mQ3) + 4*pow8(mU3))) - 2*(m32 - mQ32)*(m32 - mU32)*pow2(mQ32 -
        mU32)*pow4(Xt)*(-32*pow12(m3)*pow2(mQ32 - mU32) + 6*m32*(mQ32 + mU32)*
        pow4(mQ3)*pow4(mU3)*(mQ32*(10*msq2 - 383*mU32) + 10*msq2*mU32 + pow4(
        mQ3) + pow4(mU3)) + 2*mQ32*mU32*(mQ32 + mU32)*(mQ32*(30*msq2 - 1007*
        mU32) + 30*msq2*mU32 + 167*pow4(mQ3) + 167*pow4(mU3))*pow6(m3) - 3*(10*
        mQ32*(msq2 - 39*mU32) + 10*msq2*mU32 + pow4(mQ3) + pow4(mU3))*pow6(mQ3)
        *pow6(mU3) - mQ32*mU32*pow4(m3)*(2*pow4(mQ3)*(75*msq2*mU32 - 2269*pow4(
        mU3)) + (30*msq2 - 812*mU32)*pow6(mQ3) + 2*mQ32*(75*msq2*pow4(mU3) -
        406*pow6(mU3)) + 3*(10*msq2 + mU32)*pow6(mU3) + 3*pow8(mQ3)) - pow8(m3)
        *(10*pow4(mQ3)*(3*msq2*mU32 - 67*pow4(mU3)) + 611*mU32*pow6(mQ3) +
        mQ32*(30*msq2*pow4(mU3) + 611*pow6(mU3)) + 32*pow8(mQ3) + 32*pow8(mU3))
        + 8*(27*mU32*pow4(mQ3) + 27*mQ32*pow4(mU3) + 8*pow6(mQ3) + 8*pow6(mU3))
        *power10(m3)) + (m32 - mQ32)*(m32 - mU32)*pow4(mQ32 - mU32)*(64*pow12(
        m3)*(pow4(mQ3) + pow4(mU3)) - 12*m32*(mQ32 + mU32)*pow4(mQ3)*pow4(mU3)*
        (10*msq2*mU32 + mQ32*(10*msq2 + 147*mU32) + pow4(mQ3) + pow4(mU3)) - 4*
        mQ32*mU32*(mQ32 + mU32)*(30*msq2*mU32 + 6*mQ32*(5*msq2 + 154*mU32) +
        167*pow4(mQ3) + 167*pow4(mU3))*pow6(m3) + (6*mU32*(10*msq2 + mU32) +
        mQ32*(60*msq2 + 559*mU32) + 6*pow4(mQ3))*pow6(mQ3)*pow6(mU3) + mQ32*
        mU32*pow4(m3)*(12*pow4(mQ3)*(25*msq2*mU32 + 408*pow4(mU3)) + (60*msq2 +
        1829*mU32)*pow6(mQ3) + 6*(10*msq2 + mU32)*pow6(mU3) + mQ32*(300*msq2*
        pow4(mU3) + 1829*pow6(mU3)) + 6*pow8(mQ3)) + pow8(m3)*(pow4(mQ3)*(60*
        msq2*mU32 + 3179*pow4(mU3)) + 1350*mU32*pow6(mQ3) + 30*mQ32*(2*msq2*
        pow4(mU3) + 45*pow6(mU3)) + 64*pow8(mQ3) + 64*pow8(mU3)) - 16*(43*mU32*
        pow4(mQ3) + 43*mQ32*pow4(mU3) + 8*pow6(mQ3) + 8*pow6(mU3))*power10(m3))
        - 2*pow2(log(mU32/pow2(mQ3)))*pow3(m32 - mQ32)*pow4(mQ3)*pow4(mU3)*(8*(
        m32 - mU32)*(-39*m3*(mQ32 + mU32)*pow4(mU3) + 2*pow3(m3)*(15*mQ32*mU32
        + 7*pow4(mU3)) + 8*(mQ32 + 3*mU32)*pow5(m3))*pow5(Xt) + 4*m3*(m32 -
        mU32)*Xt*pow2(mQ32 - mU32)*((-189*mQ32 + 221*mU32)*pow4(m3) + m32*(318*
        mQ32*mU32 - 382*pow4(mU3)) - 111*mQ32*pow4(mU3) + 143*pow6(mU3)) +
        pow3(mQ32 - mU32)*(pow4(m3)*(519*mQ32*mU32 - 263*pow4(mU3)) - (239*mQ32
        + 81*mU32)*pow6(m3) + 138*(mQ32 - mU32)*pow6(mU3) + m32*(-414*mQ32*
        pow4(mU3) + 350*pow6(mU3)) + 128*pow8(m3)) - pow4(Xt)*(-4*(89*mQ32*mU32
        + 8*pow4(mQ3) - 311*pow4(mU3))*pow6(m3) + 69*(4*mQ32*mU32 - pow4(mQ3) +
        5*pow4(mU3))*pow6(mU3) - 2*pow4(m3)*(51*mU32*pow4(mQ3) - 310*mQ32*pow4(
        mU3) + 455*pow6(mU3)) + 16*(7*mQ32 - 23*mU32)*pow8(m3) + m32*(207*pow4(
        mQ3)*pow4(mU3) - 652*mQ32*pow6(mU3) - 315*pow8(mU3))) + 4*m3*(m32 -
        mU32)*pow3(Xt)*(pow4(m3)*(256*mQ32*mU32 + 47*pow4(mQ3) - 431*pow4(mU3))
        + 251*pow4(mQ3)*pow4(mU3) - 128*(mQ32 - mU32)*pow6(m3) - 376*mQ32*pow6(
        mU3) + m32*(-310*mU32*pow4(mQ3) + 240*mQ32*pow4(mU3) + 326*pow6(mU3)) -
        3*pow8(mU3)) + 2*(mQ32 - mU32)*pow2(Xt)*((364*mQ32*mU32 + 37*pow4(mQ3)
        + 751*pow4(mU3))*pow6(m3) - 138*(-3*mQ32*mU32 + pow4(mQ3) + 2*pow4(mU3)
        )*pow6(mU3) - 3*pow4(m3)*(103*mU32*pow4(mQ3) - 88*mQ32*pow4(mU3) + 241*
        pow6(mU3)) - 128*(mQ32 + 5*mU32)*pow8(m3) + m32*(414*pow4(mQ3)*pow4(
        mU3) - 922*mQ32*pow6(mU3) + 700*pow8(mU3)) + 192*power10(m3))) + 2*(m32
        - mQ32)*mQ32*(mQ32 - mU32)*mU32*log(mU32/pow2(mQ3))*(20*m3*mQ32*mU32*
        Xt*log(msq2/pow2(mQ3))*pow2(m32 - mQ32)*pow3(m32 - mU32)*(2*pow2(mQ32 -
        mU32) - 2*(mQ32 + mU32)*pow2(Xt) + m3*pow3(Xt)) - 8*(m32 - mQ32)*(m32 -
        mU32)*mU32*pow5(Xt)*(78*m3*pow4(mQ3)*pow4(mU3) - pow3(m3)*(53*mU32*
        pow4(mQ3) + 71*mQ32*pow4(mU3)) + (45*mQ32*mU32 - 23*pow4(mQ3) - 8*pow4(
        mU3))*pow5(m3) + 8*(3*mQ32 + mU32)*pow7(m3)) - 8*(m32 - mQ32)*(m32 -
        mU32)*mU32*Xt*pow2(mQ32 - mU32)*(-107*m3*pow4(mQ3)*pow4(mU3) + pow3(m3)
        *(263*mU32*pow4(mQ3) + 124*mQ32*pow4(mU3)) - (289*mQ32*mU32 + 152*pow4(
        mQ3) + 8*pow4(mU3))*pow5(m3) + (161*mQ32 + 8*mU32)*pow7(m3)) + 16*m3*(
        m32 - mQ32)*(m32 - mU32)*mU32*pow3(Xt)*((101*mQ32*mU32 + 44*pow4(mQ3) +
        8*pow4(mU3))*pow6(m3) - 53*pow4(mU3)*pow6(mQ3) - 180*pow4(mQ3)*pow6(
        mU3) - pow4(m3)*(224*mU32*pow4(mQ3) + 332*mQ32*pow4(mU3) + 71*pow6(mQ3)
        + 8*pow6(mU3)) + m32*(376*pow4(mQ3)*pow4(mU3) + 116*mU32*pow6(mQ3) +
        191*mQ32*pow6(mU3)) + 32*mQ32*pow8(m3)) + pow4(Xt)*(pow12(m3)*(-784*
        mQ32*mU32 + 33*pow4(mQ3) - 65*pow4(mU3)) - 2*m32*pow4(mQ3)*(-60*msq2*
        mU32 + mQ32*(-90*msq2 + 1773*mU32) + 744*pow4(mQ3) + 1283*pow4(mU3))*
        pow6(mU3) + mQ32*pow4(m3)*pow4(mU3)*((-180*msq2 + 3613*mU32)*pow4(mQ3)
        - 60*msq2*pow4(mU3) + mQ32*(-360*msq2*mU32 + 5687*pow4(mU3)) + 1899*
        pow6(mQ3) + 1209*pow6(mU3)) + pow8(m3)*(-60*pow4(mQ3)*(2*msq2*mU32 +
        21*pow4(mU3)) + 1111*mU32*pow6(mQ3) + mQ32*(-180*msq2*pow4(mU3) + 75*
        pow6(mU3)) + 33*pow8(mQ3) - 243*pow8(mU3)) + 12*(37*mQ32 - 5*msq2 +
        106*mU32)*pow6(mQ3)*pow8(mU3) + mU32*pow6(m3)*(3*pow4(mQ3)*(120*msq2*
        mU32 - 809*pow4(mU3)) + 4*(15*msq2 - 602*mU32)*pow6(mQ3) + 6*mQ32*(30*
        msq2*pow4(mU3) - 389*pow6(mU3)) - 880*pow8(mQ3) + 81*pow8(mU3)) + (557*
        mU32*pow4(mQ3) + 2*mQ32*(30*msq2*mU32 + 913*pow4(mU3)) - 66*pow6(mQ3) +
        227*pow6(mU3))*power10(m3)) + 2*(m32 - mU32)*pow2(Xt)*(450*pow2(mQ32 -
        mU32)*pow6(mQ3)*pow6(mU3) - 2*m32*pow4(mQ3)*pow4(mU3)*(-653*mU32*pow4(
        mQ3) - 53*mQ32*pow4(mU3) + 613*pow6(mQ3) + 477*pow6(mU3)) + 2*pow8(m3)*
        (-98*pow4(mQ3)*pow4(mU3) + 551*mU32*pow6(mQ3) + 1099*mQ32*pow6(mU3) +
        33*pow8(mQ3) - 49*pow8(mU3)) + mQ32*mU32*pow4(m3)*(-2316*pow4(mQ3)*
        pow4(mU3) + 1310*mU32*pow6(mQ3) + 2810*mQ32*pow6(mU3) + 817*pow8(mQ3) +
        451*pow8(mU3)) - 3*(31*mU32*pow4(mQ3) + 225*mQ32*pow4(mU3) + 11*pow6(
        mQ3) - 11*pow6(mU3))*power10(m3) - pow6(m3)*(-803*pow4(mU3)*pow6(mQ3) +
        1695*pow4(mQ3)*pow6(mU3) + 1830*mU32*pow8(mQ3) + 1902*mQ32*pow8(mU3) +
        33*power10(mQ3) - 49*power10(mU3))) + pow2(mQ32 - mU32)*(pow12(m3)*(96*
        mQ32*mU32 + 33*pow4(mQ3) - pow4(mU3)) + mQ32*pow4(m3)*pow4(mU3)*(1491*
        mU32*pow4(mQ3) - 2180*mQ32*pow4(mU3) + 1843*pow6(mQ3) - 514*pow6(mU3))
        + pow8(m3)*(990*pow4(mQ3)*pow4(mU3) + 2083*mU32*pow6(mQ3) - 1775*mQ32*
        pow6(mU3) + 33*pow8(mQ3) - 51*pow8(mU3)) + 381*(mQ32 - mU32)*pow6(mQ3)*
        pow8(mU3) + (-1233*mU32*pow4(mQ3) + 624*mQ32*pow4(mU3) - 66*pow6(mQ3) +
        35*pow6(mU3))*power10(m3) + pow6(m3)*(-3472*pow4(mU3)*pow6(mQ3) + 1568*
        pow4(mQ3)*pow6(mU3) - 942*mU32*pow8(mQ3) + 1549*mQ32*pow8(mU3) + 17*
        power10(mU3)) + m32*(-1295*pow6(mU3)*pow8(mQ3) + 285*pow6(mQ3)*pow8(
        mU3) + 882*pow4(mQ3)*power10(mU3)))) - 2*m3*(mQ32 - mU32)*log(m32/pow2(
        mQ3))*(-32*(m32 - mQ32)*mQ32*(m32 - mU32)*mU32*Xt*pow3(mQ32 - mU32)*
        pow4(m3)*(-11*mQ32*mU32*(mQ32 + mU32) - 18*(mQ32 + mU32)*pow4(m3) + 10*
        m32*(3*mQ32*mU32 + pow4(mQ3) + pow4(mU3)) + 8*pow6(m3)) + 16*m32*(m32 -
        mQ32)*mQ32*(m32 - mU32)*(mQ32 - mU32)*mU32*pow5(Xt)*(-43*m32*mQ32*mU32*
        (mQ32 + mU32) + pow4(m3)*(38*mQ32*mU32 - 4*pow4(mQ3) - 4*pow4(mU3)) +
        48*pow4(mQ3)*pow4(mU3) + 4*(mQ32 + mU32)*pow6(m3)) + 16*(m32 - mQ32)*
        mQ32*(m32 - mU32)*(mQ32 - mU32)*mU32*pow3(Xt)*(5*m32*(mQ32 + mU32)*
        pow4(mQ3)*pow4(mU3) + 6*pow6(mQ3)*pow6(mU3) + pow6(m3)*(47*mU32*pow4(
        mQ3) + 47*mQ32*pow4(mU3) + 24*pow6(mQ3) + 24*pow6(mU3)) - pow4(m3)*(28*
        pow4(mQ3)*pow4(mU3) + 27*mU32*pow6(mQ3) + 27*mQ32*pow6(mU3)) - 4*(7*
        mQ32*mU32 + 10*pow4(mQ3) + 10*pow4(mU3))*pow8(m3) + 16*(mQ32 + mU32)*
        power10(m3)) - 4*m3*(m32 - mQ32)*(m32 - mU32)*pow2(Xt)*pow3(mQ32 -
        mU32)*(117*m32*(mQ32 + mU32)*pow4(mQ3)*pow4(mU3) - 6*pow6(mQ3)*pow6(
        mU3) + 2*pow6(m3)*(141*mU32*pow4(mQ3) + 141*mQ32*pow4(mU3) + 8*pow6(
        mQ3) + 8*pow6(mU3)) - pow4(m3)*(353*pow4(mQ3)*pow4(mU3) + 133*mU32*
        pow6(mQ3) + 133*mQ32*pow6(mU3)) - (173*mQ32*mU32 + 32*pow4(mQ3) + 32*
        pow4(mU3))*pow8(m3) + 16*(mQ32 + mU32)*power10(m3)) + 2*m3*(mQ32 -
        mU32)*pow4(Xt)*(16*pow14(m3)*pow2(mQ32 - mU32) + pow4(m3)*pow4(mQ3)*
        pow4(mU3)*(89*mU32*pow4(mQ3) + 89*mQ32*pow4(mU3) - 184*pow6(mQ3) - 184*
        pow6(mU3)) + 2*m32*(49*mQ32*mU32 + 69*pow4(mQ3) + 69*pow4(mU3))*pow6(
        mQ3)*pow6(mU3) - 3*pow12(m3)*(15*mU32*pow4(mQ3) + 15*mQ32*pow4(mU3) +
        16*pow6(mQ3) + 16*pow6(mU3)) - 75*(mQ32 + mU32)*pow8(mQ3)*pow8(mU3) +
        mQ32*mU32*pow6(m3)*(-692*pow4(mQ3)*pow4(mU3) + 219*mU32*pow6(mQ3) +
        219*mQ32*pow6(mU3) + 133*pow8(mQ3) + 133*pow8(mU3)) + (-192*pow4(mQ3)*
        pow4(mU3) + 311*mU32*pow6(mQ3) + 311*mQ32*pow6(mU3) + 48*pow8(mQ3) +
        48*pow8(mU3))*power10(m3) - pow8(m3)*(-190*pow4(mU3)*pow6(mQ3) - 190*
        pow4(mQ3)*pow6(mU3) + 367*mU32*pow8(mQ3) + 367*mQ32*pow8(mU3) + 16*
        power10(mQ3) + 16*power10(mU3))) - m3*pow3(mQ32 - mU32)*(-32*pow14(m3)*
        (pow4(mQ3) + pow4(mU3)) + 3*m32*(-24*mQ32*mU32 + 23*pow4(mQ3) + 23*
        pow4(mU3))*pow6(mQ3)*pow6(mU3) + pow12(m3)*(346*mU32*pow4(mQ3) + 346*
        mQ32*pow4(mU3) + 96*pow6(mQ3) + 96*pow6(mU3)) + pow4(m3)*pow4(mQ3)*
        pow4(mU3)*(-155*mU32*pow4(mQ3) - 155*mQ32*pow4(mU3) + 237*pow6(mQ3) +
        237*pow6(mU3)) + 12*(mQ32 + mU32)*pow8(mQ3)*pow8(mU3) - mQ32*mU32*pow6(
        m3)*(-318*pow4(mQ3)*pow4(mU3) + 739*mU32*pow6(mQ3) + 739*mQ32*pow6(mU3)
        + 330*pow8(mQ3) + 330*pow8(mU3)) - 2*(285*pow4(mQ3)*pow4(mU3) + 503*
        mU32*pow6(mQ3) + 503*mQ32*pow6(mU3) + 48*pow8(mQ3) + 48*pow8(mU3))*
        power10(m3) + pow8(m3)*(738*pow4(mU3)*pow6(mQ3) + 738*pow4(mQ3)*pow6(
        mU3) + 990*mU32*pow8(mQ3) + 990*mQ32*pow8(mU3) + 32*power10(mQ3) + 32*
        power10(mU3))) - log(mU32/pow2(mQ3))*pow4(mQ3)*pow4(mU3)*(-8*m32*(m32 -
        mQ32)*(m32 - mU32)*pow5(Xt)*(48*mQ32*mU32*(mQ32 + mU32) + 30*(mQ32 +
        mU32)*pow4(m3) - m32*(78*mQ32*mU32 + 47*pow4(mQ3) + 47*pow4(mU3)) + 16*
        pow6(m3)) - 24*(m32 - mQ32)*(m32 - mU32)*Xt*pow2(mQ32 - mU32)*(-12*m32*
        mQ32*mU32*(mQ32 + mU32) - 2*pow4(mQ3)*pow4(mU3) + pow4(m3)*(56*mQ32*
        mU32 + 11*pow4(mQ3) + 11*pow4(mU3)) - 38*(mQ32 + mU32)*pow6(m3) + 24*
        pow8(m3)) + pow4(Xt)*(322*(mQ32 + mU32)*pow11(m3) + 88*pow13(m3) + 24*
        m3*pow6(mQ3)*pow6(mU3) + (2221*mU32*pow4(mQ3) + 2221*mQ32*pow4(mU3) +
        567*pow6(mQ3) + 567*pow6(mU3))*pow7(m3) - pow5(m3)*(1694*pow4(mQ3)*
        pow4(mU3) + 952*mU32*pow6(mQ3) + 952*mQ32*pow6(mU3) + 101*pow8(mQ3) +
        101*pow8(mU3)) + pow3(m3)*(353*pow4(mU3)*pow6(mQ3) + 353*pow4(mQ3)*
        pow6(mU3) + 105*mU32*pow8(mQ3) + 105*mQ32*pow8(mU3)) - 8*(211*mQ32*mU32
        + 110*pow4(mQ3) + 110*pow4(mU3))*pow9(m3)) - 16*(m32 - mQ32)*(m32 -
        mU32)*pow3(Xt)*(3*(mQ32 + mU32)*pow4(mQ3)*pow4(mU3) + 4*(112*mQ32*mU32
        + 9*pow4(mQ3) + 9*pow4(mU3))*pow6(m3) + pow4(m3)*(-220*mU32*pow4(mQ3) -
        220*mQ32*pow4(mU3) + 54*pow6(mQ3) + 54*pow6(mU3)) - 18*m32*(-10*pow4(
        mQ3)*pow4(mU3) + 3*mU32*pow6(mQ3) + 3*mQ32*pow6(mU3)) - 181*(mQ32 +
        mU32)*pow8(m3) + 96*power10(m3)) + pow2(mQ32 - mU32)*pow3(m3)*((372*
        mQ32*mU32 + 454*pow4(mQ3) + 454*pow4(mU3))*pow6(m3) + 41*pow4(mU3)*
        pow6(mQ3) + 41*pow4(mQ3)*pow6(mU3) - pow4(m3)*(273*mU32*pow4(mQ3) +
        273*mQ32*pow4(mU3) + 367*pow6(mQ3) + 367*pow6(mU3)) - 320*(mQ32 + mU32)
        *pow8(m3) - 105*mU32*pow8(mQ3) - 105*mQ32*pow8(mU3) + m32*(-246*pow4(
        mQ3)*pow4(mU3) + 342*mU32*pow6(mQ3) + 342*mQ32*pow6(mU3) + 101*pow8(
        mQ3) + 101*pow8(mU3)) + 128*power10(m3)) + 2*pow2(Xt)*pow3(m3)*(384*
        pow12(m3) + 24*pow6(m3)*(-177*mU32*pow4(mQ3) - 177*mQ32*pow4(mU3) + 17*
        pow6(mQ3) + 17*pow6(mU3)) + (4244*mQ32*mU32 + 758*pow4(mQ3) + 758*pow4(
        mU3))*pow8(m3) + pow4(m3)*(5262*pow4(mQ3)*pow4(mU3) + 744*mU32*pow6(
        mQ3) + 744*mQ32*pow6(mU3) - 495*pow8(mQ3) - 495*pow8(mU3)) + 3*mQ32*
        mU32*(186*pow4(mQ3)*pow4(mU3) + 6*mU32*pow6(mQ3) + 6*mQ32*pow6(mU3) -
        35*pow8(mQ3) - 35*pow8(mU3)) - 1152*(mQ32 + mU32)*power10(m3) + m32*(-
        1750*pow4(mU3)*pow6(mQ3) - 1750*pow4(mQ3)*pow6(mU3) + 497*mU32*pow8(
        mQ3) + 497*mQ32*pow8(mU3) + 101*power10(mQ3) + 101*power10(mU3)))))))/(
        3.*pow3(m32 - mQ32)*pow3(m32 - mU32)*pow4(mQ3)*pow4(mU3)*pow4(mQ32 -
        mU32));
      };
      case(Limits::MQ3_EQ_MU3):{
	 return (-384*lmQ3MR*dilog(1 - m32/pow2(mQ3))*pow3(m3)*(-137*m3*mQ32 - 184*m32*Xt
        + 192*mQ32*Xt + 133*pow3(m3))*pow8(mQ3) - 6624*pow3(lmQ3MR)*pow3(m32 -
        mQ32)*pow8(mQ3) + 9*dlatas2Const*pow3(-m32 + mQ32)*pow8(mQ3) + 8*pow2(
        lmQ3MR)*pow3(m32 - mQ32)*(-256*pow3(m3)*(-(mQ32*pow3(Xt)) + 3*Xt*pow4(
        mQ3)) + 96*pow4(m3)*(-4*mQ32*pow2(Xt) + 2*pow4(mQ3) + pow4(Xt)) - 6*
        m32*(-326*pow2(Xt)*pow4(mQ3) + 49*mQ32*pow4(Xt) + 358*pow6(mQ3)) + 552*
        m3*(-(pow3(Xt)*pow4(mQ3)) + 6*Xt*pow6(mQ3)) + 9*(23*pow4(mQ3)*pow4(Xt)
        - 276*pow2(Xt)*pow6(mQ3) + 254*pow8(mQ3))) + 8*lmQ3MR*(-((m32 - mQ32)*(
        20*m3*mQ32*log(msq2/pow2(mQ3))*pow2(m32 - mQ32)*(2*mQ32*Xt*(-6*mQ32 +
        pow2(Xt)) + m3*(-6*mQ32*pow2(Xt) + 6*pow4(mQ3) + pow4(Xt))) + 104*m3*
        Xt*(-49*mQ32*pow2(Xt) + 42*pow4(mQ3) + 3*pow4(Xt))*pow6(mQ3) - 16*pow6(
        m3)*(-354*pow2(Xt)*pow4(mQ3) + 61*mQ32*pow4(Xt) + 306*pow6(mQ3)) + 8*
        pow5(m3)*(-249*pow3(Xt)*pow4(mQ3) + 8*mQ32*pow5(Xt) + 1302*Xt*pow6(mQ3)
        ) + 3*pow6(mQ3)*(24*(5*msq2 - 53*pow2(Xt))*pow4(mQ3) - 12*mQ32*(10*
        msq2*pow2(Xt) - 9*pow4(Xt)) + 20*msq2*pow4(Xt) + (571 - 48*z3)*pow6(
        mQ3)) + 2*m32*pow4(mQ3)*((-360*msq2 + 9336*pow2(Xt))*pow4(mQ3) + mQ32*(
        360*msq2*pow2(Xt) - 926*pow4(Xt)) - 60*msq2*pow4(Xt) + 3*(-1345 + 48*
        z3)*pow6(mQ3)) - 512*(-(mQ32*pow3(Xt)) + 3*Xt*pow4(mQ3))*pow7(m3) +
        192*(-4*mQ32*pow2(Xt) + 2*pow4(mQ3) + pow4(Xt))*pow8(m3) - 8*pow3(m3)*(
        45*pow4(mQ3)*pow5(Xt) - 794*pow3(Xt)*pow6(mQ3) + 1680*Xt*pow8(mQ3)) +
        pow4(m3)*(-8*pow4(mQ3)*(45*msq2*pow2(Xt) - 287*pow4(Xt)) + 60*mQ32*
        msq2*pow4(Xt) + 120*(3*msq2 - 163*pow2(Xt))*pow6(mQ3) - 9*(-1213 + 16*
        z3)*pow8(mQ3)))) + 4*m3*log(m32/pow2(mQ3))*(pow3(m3)*(2766*mQ32*pow2(
        Xt) + 399*pow4(mQ3) - 308*pow4(Xt))*pow6(mQ3) - 12*m32*Xt*(131*mQ32*
        pow2(Xt) + 78*pow4(mQ3) - 8*pow4(Xt))*pow6(mQ3) + 8*pow6(m3)*(-93*pow3(
        Xt)*pow4(mQ3) + 4*mQ32*pow5(Xt) + 342*Xt*pow6(mQ3)) - 3*(-442*pow2(Xt)*
        pow4(mQ3) + 79*mQ32*pow4(Xt) + 410*pow6(mQ3))*pow7(m3) - 128*(-(mQ32*
        pow3(Xt)) + 3*Xt*pow4(mQ3))*pow8(m3) + 6*m3*(-6*mQ32*pow2(Xt) + 6*pow4(
        mQ3) + pow4(Xt))*pow8(mQ3) + pow5(m3)*(487*pow4(mQ3)*pow4(Xt) - 3816*
        pow2(Xt)*pow6(mQ3) + 759*pow8(mQ3)) - 4*pow4(m3)*(31*pow4(mQ3)*pow5(Xt)
        - 530*pow3(Xt)*pow6(mQ3) + 372*Xt*pow8(mQ3)) + 48*(-4*mQ32*pow2(Xt) +
        2*pow4(mQ3) + pow4(Xt))*pow9(m3) + 12*Xt*(-6*mQ32 + pow2(Xt))*power10(
        mQ3))))/(9.*pow3(-m32 + mQ32)*pow8(mQ3));
      }
      case(Limits::MQ3_EQ_M3):{
	 return (27*dlatas2Const - (32*lmQ3MR*(3853*mQ32 - 2701*mU32 - 6624*mQ3*Xt))/(
        mQ32 - mU32) - 3456*lmQ3MR*z3 - (36864*lmQ3MR*mQ3*Xt)/pow2(mU3) +
        19872*pow3(lmQ3MR) - (2304*lmQ3MR*mQ3*pow3(Xt)*(219*mQ32*mU32 + 16*
        pow4(mQ3) - 227*pow4(mU3)))/(pow2(mU3)*pow3(-mQ32 + mU32)) - (576*
        lmQ3MR*(-5*mQ3*mU32 + pow3(mQ3))*pow5(Xt))/pow4(mQ32 - mU32) + (1440*
        lmQ3MR*log(msq2/pow2(mQ3))*(pow2(-(mU3*pow2(Xt)) + pow3(mU3)) - 8*mQ3*
        mU32*pow3(Xt) - (mU32 + 2*pow2(Xt))*pow4(mQ3) + mQ32*(4*mU32*pow2(Xt) -
        pow4(mU3) + pow4(Xt)) + pow6(mQ3)))/pow2(-(mQ32*mU3) + pow3(mU3)) + (
        288*lmQ3MR*dilog(1 - mQ32/pow2(mU3))*(-248*mQ3*mU32*pow3(Xt) + pow3(
        mQ3)*(-768*mU32*Xt + 264*pow3(Xt)) - 2*(41*mU32 + 96*pow2(Xt))*pow4(
        mQ3) - 69*mU32*pow4(Xt) + mQ32*(192*mU32*pow2(Xt) + 73*pow4(Xt)) + 752*
        Xt*pow5(mQ3) + 74*pow6(mQ3)))/pow3(mQ32 - mU32) - (288*lmQ3MR*pow2(Xt)*
        (6*pow4(mQ3)*(5*msq2*mU32 + 217*pow4(mU3)) - 321*mU32*pow6(mQ3) + 3*(
        10*msq2 + mU32)*pow6(mU3) - 4*mQ32*(15*msq2*pow4(mU3) + 61*pow6(mU3)) +
        32*pow8(mQ3)))/(pow2(-(mQ3*mU32) + pow3(mQ3))*pow4(mU3)) + (144*lmQ3MR*
        pow2(log(mU32/pow2(mQ3)))*(4*Xt*pow3(mQ32 - mU32)*(-350*mU32*pow3(mQ3)
        + 143*mQ3*pow4(mU3) + 189*pow5(mQ3)) - 2*pow2(mQ32 - mU32)*pow2(Xt)*(-
        383*mU32*pow4(mQ3) + 562*mQ32*pow4(mU3) + 101*pow6(mQ3) - 276*pow6(mU3)
        ) + pow3(mQ32 - mU32)*(-327*mU32*pow4(mQ3) + 350*mQ32*pow4(mU3) + 111*
        pow6(mQ3) - 138*pow6(mU3)) - 8*pow5(Xt)*(-25*pow3(mQ3)*pow4(mU3) + 54*
        mU32*pow5(mQ3) - 39*mQ3*pow6(mU3) + 8*pow7(mQ3)) + 4*(mQ32 - mU32)*
        pow3(Xt)*(-53*pow3(mQ3)*pow4(mU3) + 7*mU32*pow5(mQ3) - 3*mQ3*pow6(mU3)
        + 81*pow7(mQ3)) + pow4(Xt)*(1325*pow4(mQ3)*pow4(mU3) - 746*mU32*pow6(
        mQ3) - 306*mQ32*pow6(mU3) + 80*pow8(mQ3) - 345*pow8(mU3))))/pow6(mQ32 -
        mU32) + (144*pow2(lmQ3MR)*(log(mU32/pow2(mQ3))*pow4(mU3)*(-552*mQ3*
        mU32*Xt*(mU32 - pow2(Xt)) + 8*pow3(mQ3)*(138*mU32*Xt + 101*pow3(Xt)) +
        (493*mU32 + 222*pow2(Xt))*pow4(mQ3) - mQ32*(1020*mU32*pow2(Xt) + 557*
        pow4(mU3) + 501*pow4(Xt)) - 552*Xt*pow5(mQ3) - 143*pow6(mQ3) + 207*(2*
        pow2(Xt)*pow4(mU3) - mU32*pow4(Xt) + pow6(mU3))) + (mQ32 - mU32)*(128*
        mU32*Xt*(2*mU32 + pow2(Xt))*pow3(mQ3) - 545*pow4(mU3)*pow4(Xt) + 2*
        pow4(mQ3)*(163*mU32*pow2(Xt) + 280*pow4(mU3) + 8*pow4(Xt)) - 128*mU32*
        Xt*pow5(mQ3) - (179*mU32 + 32*pow2(Xt))*pow6(mQ3) + 262*pow2(Xt)*pow6(
        mU3) - mQ32*(940*pow2(Xt)*pow4(mU3) + 179*mU32*pow4(Xt) + 647*pow6(mU3)
        ) - 16*mQ3*(-77*pow3(Xt)*pow4(mU3) + 8*Xt*pow6(mU3)) + 16*pow8(mQ3) +
        250*pow8(mU3))))/(pow3(-mQ32 + mU32)*pow4(mU3)) + (72*lmQ3MR*log(mU32/
        pow2(mQ3))*(40*mQ3*Xt*log(msq2/pow2(mQ3))*pow2(-(mQ32*mU3) + pow3(mU3))
        *(2*mU32*(mU32 - pow2(Xt)) - 2*mQ32*(2*mU32 + pow2(Xt)) + mQ3*pow3(Xt)
        + 2*pow4(mQ3)) + 8*mQ3*(mQ32 - mU32)*mU32*pow3(Xt)*(330*mQ32*mU32 +
        437*pow4(mQ3) - 703*pow4(mU3)) - 4*mQ3*mU32*Xt*pow2(mQ32 - mU32)*(-
        1110*mQ32*mU32 + 653*pow4(mQ3) + 441*pow4(mU3)) - 4*mQ3*mU32*(236*mQ32*
        mU32 + 97*pow4(mQ3) - 349*pow4(mU3))*pow5(Xt) - 2*pow2(mQ32 - mU32)*
        pow2(Xt)*(-1465*mU32*pow4(mQ3) + 4098*mQ32*pow4(mU3) + 66*pow6(mQ3) -
        1147*pow6(mU3)) + pow4(Xt)*(pow4(mQ3)*(120*msq2*mU32 + 4241*pow4(mU3))
        - 3459*mU32*pow6(mQ3) + 5*(24*msq2 - 571*mU32)*pow6(mU3) + mQ32*(-240*
        msq2*pow4(mU3) + 2039*pow6(mU3)) + 66*pow8(mQ3)) + pow2(mQ32 - mU32)*(
        3551*pow4(mQ3)*pow4(mU3) - 1775*mU32*pow6(mQ3) - 2679*mQ32*pow6(mU3) +
        66*pow8(mQ3) + 877*pow8(mU3))))/(pow2(mU3)*pow5(mQ32 - mU32)) + (24*
        lmQ3MR*((mQ32 - mU32)*(-1748*pow4(mU3)*pow4(Xt) + pow4(mQ3)*(3324*mU32*
        pow2(Xt) - 10923*pow4(mU3) + 216*pow4(Xt)) + (-816*mU32*Xt + 768*pow3(
        Xt))*pow5(mQ3) + 96*pow3(mQ3)*(-29*mU32*pow3(Xt) + 29*Xt*pow4(mU3) - 4*
        pow5(Xt)) + (3209*mU32 - 432*pow2(Xt))*pow6(mQ3) + 2460*pow2(Xt)*pow6(
        mU3) + mQ32*(-5352*pow2(Xt)*pow4(mU3) + 5372*mU32*pow4(Xt) + 11355*
        pow6(mU3)) - 72*mQ3*(-28*pow3(Xt)*pow4(mU3) + 67*mU32*pow5(Xt) + 22*Xt*
        pow6(mU3)) - 384*Xt*pow7(mQ3) + 216*pow8(mQ3) - 3857*pow8(mU3)) + 2*
        mU32*log(mU32/pow2(mQ3))*(pow4(mQ3)*(4566*mU32*pow2(Xt) + 2838*pow4(
        mU3) + 1553*pow4(Xt)) + 6*(387*mU32*Xt + 314*pow3(Xt))*pow5(mQ3) - 6*
        pow3(mQ3)*(740*mU32*pow3(Xt) + 387*Xt*pow4(mU3) + 249*pow5(Xt)) - 2*(
        1042*mU32 + 953*pow2(Xt))*pow6(mQ3) - 2*mQ32*(1707*pow2(Xt)*pow4(mU3) -
        372*mU32*pow4(Xt) + 850*pow6(mU3)) + 6*mQ3*(426*pow3(Xt)*pow4(mU3) -
        185*mU32*pow5(Xt) + 129*Xt*pow6(mU3)) - 774*Xt*pow7(mQ3) + 569*pow8(
        mQ3) + 377*(-(pow4(mU3)*pow4(Xt)) + 2*pow2(Xt)*pow6(mU3) + pow8(mU3))))
        )/(pow2(mU3)*pow4(mQ32 - mU32)) + (144*lmQ3MR*pow4(Xt)*(5*(6*msq2*mU32
        - 211*pow4(mU3))*pow6(mQ3) + pow4(mQ3)*(-30*msq2*pow4(mU3) + 3014*pow6(
        mU3)) - 417*mU32*pow8(mQ3) + 3*(10*msq2 + mU32)*pow8(mU3) - 3*mQ32*(10*
        msq2*pow6(mU3) + 523*pow8(mU3)) + 32*power10(mQ3)))/(pow2(mQ3)*pow4(
        mU3)*pow4(mQ32 - mU32)) + (72*lmQ3MR*(3*(20*msq2*mU32 + 501*pow4(mU3))*
        pow6(mQ3) - 4*pow4(mQ3)*(15*msq2*pow4(mU3) + 259*pow6(mU3)) - 706*mU32*
        pow8(mQ3) + 6*(10*msq2 + mU32)*pow8(mU3) + mQ32*(-60*msq2*pow6(mU3) +
        177*pow8(mU3)) + 64*power10(mQ3)))/(pow2(-(mQ3*mU32) + pow3(mQ3))*pow4(
        mU3)))/27.;
      }
      case(Limits::MU3_EQ_M3):{
	 return (27*dlatas2Const - (32*lmQ3MR*(2701*mQ32 + mU3*(-3853*mU3 + 6624*Xt)))/(
        mQ32 - mU32) - 3456*lmQ3MR*z3 - (36864*lmQ3MR*mU3*Xt)/pow2(mQ3) +
        19872*pow3(lmQ3MR) - (2304*lmQ3MR*mU3*pow3(Xt)*(219*mQ32*mU32 - 227*
        pow4(mQ3) + 16*pow4(mU3)))/(pow2(mQ3)*pow3(mQ32 - mU32)) + (288*lmQ3MR*
        dilog(1 - mU32/pow2(mQ3))*(mQ32*(-192*mU32*pow2(Xt) + 768*Xt*pow3(mU3)
        + 248*mU3*pow3(Xt) + 82*pow4(mU3) + 69*pow4(Xt)) - mU32*(-192*mU32*
        pow2(Xt) + 752*Xt*pow3(mU3) + 264*mU3*pow3(Xt) + 74*pow4(mU3) + 73*
        pow4(Xt))))/pow3(mQ32 - mU32) + (576*lmQ3MR*(5*mQ32*mU3 - pow3(mU3))*
        pow5(Xt))/pow4(mQ32 - mU32) + (9*lmQ3MR*pow2(log(mU32/pow2(mQ3)))*(8*
        mU3*(1641*mQ32 - 1897*mU32)*Xt*pow2(mQ32 - mU32) - 2*(1095*mQ32 - 3047*
        mU32)*pow2(mQ32 - mU32)*pow2(Xt) + (3321*mQ32 - 6137*mU32)*pow3(mQ32 -
        mU32) + 16*mU3*pow3(Xt)*(142*mQ32*mU32 + 25*pow4(mQ3) + 345*pow4(mU3))
        - (-480*mQ32*mU32 + 9*pow4(mQ3) + 5719*pow4(mU3))*pow4(Xt) - 8*mU3*(
        313*mQ32 + 633*mU32)*pow5(Xt)))/pow4(mQ32 - mU32) + (1440*lmQ3MR*log(
        msq2/pow2(mQ3))*(pow2(-(mU3*pow2(Xt)) + pow3(mU3)) - (mU32 + 2*pow2(Xt)
        )*pow4(mQ3) + mQ32*(4*mU32*pow2(Xt) - 8*mU3*pow3(Xt) - pow4(mU3) +
        pow4(Xt)) + pow6(mQ3)))/pow2(-(mQ3*mU32) + pow3(mQ3)) + (144*pow2(
        lmQ3MR)*(log(mU32/pow2(mQ3))*pow4(mQ3)*((-685*mU32 + 552*mU3*Xt - 414*
        pow2(Xt))*pow4(mQ3) + mQ32*(1020*mU32*pow2(Xt) - 1104*Xt*pow3(mU3) -
        552*mU3*pow3(Xt) + 749*pow4(mU3) + 207*pow4(Xt)) + mU32*(-222*mU32*
        pow2(Xt) + 552*Xt*pow3(mU3) - 808*mU3*pow3(Xt) - 271*pow4(mU3) + 501*
        pow4(Xt)) + 207*pow6(mQ3)) - (mQ32 - mU32)*(16*pow2(mU32 - pow2(Xt))*
        pow4(mU3) + pow4(mQ3)*(-940*mU32*pow2(Xt) + 256*Xt*pow3(mU3) + 1232*
        mU3*pow3(Xt) + 560*pow4(mU3) - 545*pow4(Xt)) + mQ32*mU32*(326*mU32*
        pow2(Xt) - 128*Xt*pow3(mU3) + 128*mU3*pow3(Xt) - 179*pow4(mU3) - 179*
        pow4(Xt)) + (-647*mU32 - 128*mU3*Xt + 262*pow2(Xt))*pow6(mQ3) + 250*
        pow8(mQ3))))/(pow3(mQ32 - mU32)*pow4(mQ3)) - (288*lmQ3MR*pow2(Xt)*(
        pow4(mQ3)*(-60*msq2*mU32 + 1302*pow4(mU3)) + (30*msq2 - 244*mU32)*pow6(
        mQ3) + mQ32*(30*msq2*pow4(mU3) - 321*pow6(mU3)) + 3*pow8(mQ3) + 32*
        pow8(mU3)))/(pow2(mU3)*pow2(mQ32 - mU32)*pow4(mQ3)) - (9*lmQ3MR*log(
        mU32/pow2(mQ3))*(-320*mU3*Xt*log(msq2/pow2(mQ3))*pow2(-(mQ3*mU32) +
        pow3(mQ3))*(-2*mQ32*(2*mU32 + pow2(Xt)) + mU3*(-2*mU3*pow2(Xt) + 2*
        pow3(mU3) + pow3(Xt)) + 2*pow4(mQ3)) + 64*mU3*Xt*pow3(-mQ32 + mU32)*(
        323*mQ32*mU32 - 325*pow4(mQ3) + 20*pow4(mU3)) - 256*mU3*(-mQ32 + mU32)*
        pow3(Xt)*(-106*mU32*pow4(mQ3) + 181*mQ32*pow4(mU3) - 77*pow6(mQ3) + 10*
        pow6(mU3)) - 8*pow2(mQ32 - mU32)*pow2(Xt)*(-6278*mU32*pow4(mQ3) + 581*
        mQ32*pow4(mU3) + 2569*pow6(mQ3) + 40*pow6(mU3)) + 16*mU3*pow5(Xt)*(-
        714*mU32*pow4(mQ3) + 235*mQ32*pow4(mU3) + 383*pow6(mQ3) + 80*pow6(mU3))
        + pow3(mQ32 - mU32)*(-24902*mU32*pow4(mQ3) + 13707*mQ32*pow4(mU3) +
        10779*pow6(mQ3) + 352*pow6(mU3)) + 2*pow4(Xt)*(3*pow4(mQ3)*(320*msq2*
        mU32 - 6251*pow4(mU3)) + (-480*msq2 + 769*mU32)*pow6(mQ3) + mQ32*(-480*
        msq2*pow4(mU3) + 12379*pow6(mU3)) + 5205*pow8(mQ3) + 336*pow8(mU3))))/(
        pow2(mQ3)*pow5(mQ32 - mU32)) + (144*lmQ3MR*pow4(Xt)*((-30*msq2*mU32 +
        3014*pow4(mU3))*pow6(mQ3) - 5*pow4(mQ3)*(6*msq2*pow4(mU3) + 211*pow6(
        mU3)) + 3*(10*msq2 - 523*mU32)*pow8(mQ3) + mQ32*(30*msq2*pow6(mU3) -
        417*pow8(mU3)) + 3*power10(mQ3) + 32*power10(mU3)))/(pow2(mU3)*pow4(-(
        mQ3*mU32) + pow3(mQ3))) + (72*lmQ3MR*(-4*(15*msq2*mU32 + 259*pow4(mU3))
        *pow6(mQ3) + pow4(mQ3)*(-60*msq2*pow4(mU3) + 1503*pow6(mU3)) + 3*(20*
        msq2 + 59*mU32)*pow8(mQ3) + mQ32*(60*msq2*pow6(mU3) - 706*pow8(mU3)) +
        6*power10(mQ3) + 64*power10(mU3)))/(pow2(mU3)*pow2(mQ32 - mU32)*pow4(
        mQ3)) - (3*lmQ3MR*(-8*mQ32*pow3(mQ32 - mU32)*(-24*(9*mU3 - 16*Xt)*pow2(
        mU32 - pow2(Xt))*pow3(mU3) + pow4(mQ3)*(5352*mU32*pow2(Xt) - 2784*Xt*
        pow3(mU3) - 2016*mU3*pow3(Xt) + 10923*pow4(mU3) + 1748*pow4(Xt)) +
        mQ32*mU3*(-3324*pow2(Xt)*pow3(mU3) + 2784*mU32*pow3(Xt) + 816*Xt*pow4(
        mU3) - 5372*mU3*pow4(Xt) - 3209*pow5(mU3) + 4824*pow5(Xt)) - 3*(3785*
        mU32 - 528*mU3*Xt + 820*pow2(Xt))*pow6(mQ3) + 3857*pow8(mQ3)) + 3*pow2(
        log(mU32/pow2(mQ3)))*pow4(mQ3)*(1113*pow12(mQ3) + pow3(mU3)*pow4(mQ3)*(
        -41428*pow2(Xt)*pow3(mU3) - 19360*mU32*pow3(Xt) - 85968*Xt*pow4(mU3) -
        624*mU3*pow4(Xt) + 30871*pow5(mU3) - 3256*pow5(Xt)) + 2*mQ32*pow5(mU3)*
        (4333*pow2(Xt)*pow3(mU3) + 27296*mU32*pow3(Xt) + 23700*Xt*pow4(mU3) -
        8329*mU3*pow4(Xt) - 6603*pow5(mU3) + 7268*pow5(Xt)) + pow6(mQ3)*(-
        27584*pow3(mU3)*pow3(Xt) + 50196*pow2(Xt)*pow4(mU3) + 9554*mU32*pow4(
        Xt) + 75984*Xt*pow5(mU3) - 7496*mU3*pow5(Xt) - 37620*pow6(mU3)) + (
        2222*pow2(Xt)*pow3(mU3) - 24880*mU32*pow3(Xt) - 10248*Xt*pow4(mU3) +
        8713*mU3*pow4(Xt) + 2313*pow5(mU3) - 4040*pow5(Xt))*pow7(mU3) + 3*(-
        7294*mU32*pow2(Xt) - 10808*Xt*pow3(mU3) + 5744*mU3*pow3(Xt) + 8333*
        pow4(mU3) - 371*pow4(Xt))*pow8(mQ3) + (-8470*mU32 + 5256*mU3*Xt + 2226*
        pow2(Xt))*power10(mQ3)) - (mQ32 - mU32)*log(mU32/pow2(mQ3))*(22873*
        pow14(mQ3) + pow12(mQ3)*(-108845*mU32 + 19968*mU3*Xt - 42856*pow2(Xt))
        + pow4(mQ3)*pow5(mU3)*(-154408*pow2(Xt)*pow3(mU3) + 60288*mU32*pow3(Xt)
        + 36864*Xt*pow4(mU3) + 18986*mU3*pow4(Xt) + 33383*pow5(mU3) - 21936*
        pow5(Xt)) + pow3(mU3)*(323872*pow2(Xt)*pow3(mU3) - 207744*mU32*pow3(Xt)
        - 78336*Xt*pow4(mU3) + 80194*mU3*pow4(Xt) + 50381*pow5(mU3) - 50784*
        pow5(Xt))*pow6(mQ3) - 48*mQ32*(-738*pow2(Xt)*pow3(mU3) - 96*mU32*pow3(
        Xt) + 176*Xt*pow4(mU3) + 369*mU3*pow4(Xt) + 433*pow5(mU3) - 80*pow5(Xt)
        )*pow7(mU3) - 2*mU3*(176856*pow2(Xt)*pow3(mU3) - 111168*mU32*pow3(Xt) -
        49920*Xt*pow4(mU3) + 51601*mU3*pow4(Xt) + 94277*pow5(mU3) - 34824*pow5(
        Xt))*pow8(mQ3) + 2*(97376*mU32*pow2(Xt) - 34944*Xt*pow3(mU3) - 39744*
        mU3*pow3(Xt) + 105005*pow4(mU3) + 10291*pow4(Xt))*power10(mQ3) + 1536*
        pow2(mU32 - pow2(Xt))*power10(mU3))))/(pow4(mQ3)*pow6(mQ32 - mU32)))/
        27.;
      }
      case(Limits::MQ3_EQ_MU3_EQ_M3):{
	 return dlatas2Const + 736*pow3(lmQ3MR) - (8*pow2(lmQ3MR)*(-912*mQ32*pow2(Xt) +
        2544*Xt*pow3(mQ3) - 296*mQ3*pow3(Xt) + 330*pow4(mQ3) + 9*pow4(Xt)))/(9.
        *pow4(mQ3)) + (8*lmQ3MR*(-3328*pow3(mQ3)*pow3(Xt) + 24*(45*msq2 + 16*
        pow2(Xt))*pow4(mQ3) + 180*msq2*pow4(Xt) + 60*mQ32*log(msq2/pow2(mQ3))*(
        -6*mQ32*pow2(Xt) - 12*Xt*pow3(mQ3) + 2*mQ3*pow3(Xt) + 6*pow4(mQ3) +
        pow4(Xt)) - 2*mQ32*(540*msq2*pow2(Xt) + 49*pow4(Xt)) + 1600*Xt*pow5(
        mQ3) + 176*mQ3*pow5(Xt) - (851 + 432*z3)*pow6(mQ3)))/(27.*pow6(mQ3));
      }
   }
   
   throw std::runtime_error("Mass limit not included!");
}

/**
 * 	Returns the shift needed to convert the 3L threshold correction of lambda to the MSbar scheme
 * 	@param xtOrder an integer key to omit the Xt contributions starting at xtOrder + 1
 * 	@param omitLogs an integer key to omit all log mu terms
 * 	@param omitXtLogs an integer key to omit all Xt^4*Log[mu] and Xt^5*Log[mu] terms
 */
double himalaya::ThresholdCalculator::getDRbarPrimeToMSbarShift(int xtOrder, int omitLogs, int omitXtLogs){
   double xtTerms = 0.;
   if(xtOrder <= 3){
      xtTerms = getDRbarPrimeToMSbarXtTerms(p.massLimit3LThreshold, 4, omitXtLogs)
	 + getDRbarPrimeToMSbarXtTerms(p.massLimit3LThreshold, 5, omitXtLogs)
	 + getDRbarPrimeToMSbarXtTerms(p.massLimit3LThreshold, 6, omitXtLogs);
   } else{
      xtTerms = getDRbarPrimeToMSbarXtTerms(p.massLimit3LThreshold, 5, omitXtLogs)
	 + getDRbarPrimeToMSbarXtTerms(p.massLimit3LThreshold, 6, omitXtLogs);
   }

   const double g3as = getDeltaG3Alphas(omitLogs);
   const double ytas = getDeltaYtAlphas(p.massLimit3LThreshold, omitLogs);
   const double ytas2 = getDeltaYtAlphas2(p.massLimit3LThreshold, omitLogs);
   const double lambdaat = getDeltaLambdaAlphat(p.massLimit3LThreshold, omitLogs);
   const double lambdaatas = getDeltaLambdaAlphatAlphas(p.massLimit3LThreshold, omitLogs);

   return -(-2.*(lambdaat*(3*pow2(ytas) + 2*ytas2) + (lambdaatas - 4*ytas*lambdaat)
      *(g3as + 2*ytas)) - xtTerms);
}

/**
 * 	Returns the DRbarPrime to MSbar shift of delta lambda 3L at a given xtOrder
 * 	@param limit an integer key for a mass limit
 * 	@param xtOrder an integer key to omit the Xt contributions starting at xtOrder + 1
 * 	@param omitLogs an integer key to omit all log mu terms
 */
double himalaya::ThresholdCalculator::getDRbarPrimeToMSbarXtTerms(int limit, int xtOrder, int omitLogs){
   
   using std::log;
   using std::sqrt;
   using gm2calc::dilog;
   
   const int omitXt4 = xtOrder != 4 ? 0 : 1;
   const int omitXt5 = xtOrder != 5 ? 0 : 1;
   const int omitXt6 = xtOrder != 6 ? 0 : 1;
   
   const double Mst12 = pow2(p.MSt(0));
   const double m32 = pow2(p.MG);
   const double MR2 = pow2(p.scale);
   const double mU32 = p.mu2(2,2);
   const double mQ32 = p.mq2(2,2);
   const double m3 = sqrt(m32);
   const double mU3 = sqrt(mU32);
   const double mQ3 = sqrt(mQ32);
   const double msq = sqrt(msq2);
   const double Xt = p.Au(2,2) - p.mu * p.vd / p.vu;
   const double Xt4 = omitXt4*pow4(Xt);
   const double Xt5 = omitXt5*pow5(Xt);
   const double Xt6 = omitXt6*pow6(Xt);
   const double lmQ3MR = omitLogs * log(Mst12 / MR2) + log(mQ32 / Mst12);
   
   switch (limit){
      case(Limits::GENERAL):{
	 return (-4*Xt4*(640*mQ32*(mQ32 - mU32)*mU32*log(mQ32/mU32)*pow2(m32 - mQ32)*
        pow2(m32 - mU32)*pow2(m3*(m32 - mQ32)*mU32*log(mQ32/mU32) + (mQ32 -
        mU32)*log(mQ32/m32)*pow3(m3)) - 512*m3*mQ32*mU32*(-8*mQ32 + 4*lmQ3MR*
        mQ32 + 8*mU32 - 4*lmQ3MR*mU32 + (4*m32 - 2*(mQ32 + mU32))*dilog(1 -
        m32/mQ32) + 2*(-2*m32 + mQ32 + mU32)*dilog(1 - m32/mU32) + 2*mQ32*log(
        mQ32/mU32) - 2*lmQ3MR*mQ32*log(mQ32/mU32) + 6*mU32*log(mQ32/mU32) - 2*
        lmQ3MR*mU32*log(mQ32/mU32) + 4*m32*log(mQ32/m32)*log(mQ32/mU32) - 2*
        m32*pow2(log(mQ32/mU32)) + mQ32*pow2(log(mQ32/mU32)) + mU32*pow2(log(
        mQ32/mU32)))*(m3*(m32 - mQ32)*mU32*log(mQ32/mU32) + (mQ32 - mU32)*log(
        mQ32/m32)*pow3(m3))*pow3(m32 - mQ32)*pow3(m32 - mU32) - 4*pow2(m32 -
        mQ32)*pow2(m32 - mU32)*(6*pow2(m32 - mQ32)*pow2(m32 - mU32) - 12*
        lmQ3MR*pow2(m32 - mQ32)*pow2(m32 - mU32) + 12*log(mQ32/m32)*pow2(m32 -
        mQ32)*pow2(m32 - mU32) - (12*lmQ3MR - 10*log(mQ32/msq2) - log(mQ32/
        mU32))*pow2(m32 - mQ32)*pow2(m32 - mU32) + 16*(mQ32*(-m32 + mQ32)*(m32
        - mU32)*mU32 + (m32 - mQ32)*(m32 - mU32)*pow4(m3) + lmQ3MR*pow2(m32 -
        mU32)*(2*m32*mQ32 - pow4(mQ3)) + (lmQ3MR - log(mQ32/mU32))*pow2(m32 -
        mQ32)*(2*m32*mU32 - pow4(mU3)) - (lmQ3MR - log(mQ32/m32))*pow4(m3)*(-2*
        m32*(mQ32 + mU32) + 2*pow4(m3) + pow4(mQ3) + pow4(mU3))))*(mQ32*(mQ32 -
        mU32)*mU32*(-2*m32 + mQ32 + mU32)*dilog(1 - m32/mQ32) + m32*mU32*pow4(
        mQ3) - lmQ3MR*m32*mU32*pow4(mQ3) + m32*mU32*log(mQ32/m32)*pow4(mQ3) +
        2*m32*mU32*log(mQ32/mU32)*pow4(mQ3) - 2*lmQ3MR*m32*mU32*log(mQ32/mU32)*
        pow4(mQ3) + m32*mU32*pow2(log(mQ32/mU32))*pow4(mQ3) + m32*mQ32*pow4(
        mU3) - lmQ3MR*m32*mQ32*pow4(mU3) + m32*mQ32*log(mQ32/m32)*pow4(mU3) -
        2*m32*mQ32*log(mQ32/mU32)*pow4(mU3) + 2*lmQ3MR*m32*mQ32*log(mQ32/mU32)*
        pow4(mU3) - m32*mQ32*pow2(log(mQ32/mU32))*pow4(mU3) + 12*pow4(mQ3)*
        pow4(mU3) - 8*lmQ3MR*pow4(mQ3)*pow4(mU3) + 4*log(mQ32/mU32)*pow4(mQ3)*
        pow4(mU3) - 2*pow2(log(mQ32/mU32))*pow4(mQ3)*pow4(mU3) + mQ32*mU32*
        dilog(1 - m32/mU32)*(2*m32*(mQ32 - mU32) - pow4(mQ3) + pow4(mU3)) -
        m32*pow6(mQ3) + lmQ3MR*m32*pow6(mQ3) - 6*mU32*pow6(mQ3) + 4*lmQ3MR*
        mU32*pow6(mQ3) - m32*log(mQ32/m32)*pow6(mQ3) + 3*mU32*log(mQ32/mU32)*
        pow6(mQ3) - 2*lmQ3MR*mU32*log(mQ32/mU32)*pow6(mQ3) - m32*pow6(mU3) +
        lmQ3MR*m32*pow6(mU3) - 6*mQ32*pow6(mU3) + 4*lmQ3MR*mQ32*pow6(mU3) -
        m32*log(mQ32/m32)*pow6(mU3) - 7*mQ32*log(mQ32/mU32)*pow6(mU3) + 2*
        lmQ3MR*mQ32*log(mQ32/mU32)*pow6(mU3) - 2*mQ32*pow2(log(mQ32/mU32))*
        pow6(mU3)) + mQ32*mU32*(2*(mQ32 - mU32) - (mQ32 + mU32)*log(mQ32/mU32))
        *(12*(mQ32 - mU32)*pow2(-((m32 - mQ32)*((m32 - mQ32)*(2*m32 - mU32)*
        mU32*log(mQ32/mU32) + (m32 - mU32)*((1 + 2*lmQ3MR)*mQ32*mU32 - 2*
        lmQ3MR*m32*(mQ32 + mU32) + (-1 + 2*lmQ3MR)*pow4(m3)))) + log(mQ32/m32)*
        pow4(m3)*(-2*m32*(mQ32 + mU32) + 2*pow4(m3) + pow4(mQ3) + pow4(mU3))) -
        15*(m32 - mQ32)*(m32 - mU32)*(mQ32 - mU32)*pow2(lmQ3MR - log(mQ32/msq2)
        )*(-2*(mQ32 - msq2)*pow3(m32 - mU32)*(-3*m32*(mQ32 - msq2) + mQ32*(mQ32
        + msq2) - 2*pow4(m3)) - 2*(msq2 - mU32)*pow3(m32 - mQ32)*(-3*m32*(msq2
        - mU32) - mU32*(msq2 + mU32) + 2*pow4(m3))) + 60*(m32 - mU32)*(mQ32 -
        mU32)*(msq2 - mU32)*dilog(1 - msq2/mU32)*(-3*m32*(msq2 - mU32) - mU32*(
        msq2 + mU32) + 2*pow4(m3))*pow4(m32 - mQ32) - 60*(m32 - mQ32)*(mQ32 -
        msq2)*(mQ32 - mU32)*dilog(1 - msq2/mQ32)*(3*m32*(mQ32 - msq2) - mQ32*(
        mQ32 + msq2) + 2*pow4(m3))*pow4(m32 - mU32) - 20*(mQ32 - mU32)*(lmQ3MR
        - log(mQ32/msq2))*pow2(m32 - mQ32)*pow2(m32 - mU32)*(3*mQ32*msq2*pow4(
        mU3) + pow4(mQ3)*(3*msq2*mU32 + 10*pow4(mU3)) + pow4(m3)*(-15*msq2*mU32
        + mQ32*(-15*msq2 + 44*mU32) + 11*pow4(mQ3) + 11*pow4(mU3)) + 3*m32*((3*
        msq2 - 7*mU32)*pow4(mQ3) + 3*msq2*pow4(mU3) - mQ32*(4*msq2*mU32 + 7*
        pow4(mU3))) + (-23*mQ32 + 18*msq2 - 23*mU32)*pow6(m3) + 12*pow8(m3)) +
        (mQ32 - mU32)*pow2(m32 - mQ32)*pow2(m32 - mU32)*(-(mQ32*mU32*(6*mU32*(
        10*msq2 + mU32) + mQ32*(60*msq2 + 169*mU32) + 6*pow4(mQ3))) + pow4(m3)*
        (4*mQ32*(75*msq2 - 28*mU32) + 300*msq2*mU32 + 93*pow4(mQ3) + 93*pow4(
        mU3)) - 6*(47*mQ32 + 60*msq2 + 47*mU32)*pow6(m3) - 2*m32*(2*(45*msq2 -
        52*mU32)*pow4(mQ3) + 9*(10*msq2 + mU32)*pow4(mU3) - 8*mQ32*(15*msq2*
        mU32 + 13*pow4(mU3)) + 9*pow6(mQ3)) + 291*pow8(m3)) + 60*(m32 - mQ32)*(
        m32 - msq2)*(m32 - mU32)*(mQ32 - mU32)*dilog(1 - m32/msq2)*(-(mQ32*
        msq2*mU32*(pow4(mQ3) + pow4(mU3))) + (-8*msq2*mU32 + mQ32*(-8*msq2 +
        30*mU32) + 3*pow4(mQ3) + 3*pow4(mU3))*pow6(m3) - pow4(m3)*((-9*msq2 +
        15*mU32)*pow4(mQ3) - 9*msq2*pow4(mU3) + 3*mQ32*(2*msq2*mU32 + 5*pow4(
        mU3)) + pow6(mQ3) + pow6(mU3)) + m32*(3*msq2*mU32*pow4(mQ3) + (-3*msq2
        + 5*mU32)*pow6(mQ3) - 3*msq2*pow6(mU3) + mQ32*(3*msq2*pow4(mU3) + 5*
        pow6(mU3))) + (-8*mQ32 + 6*msq2 - 8*mU32)*pow8(m3) + 2*power10(m3)) +
        2*(m32 - mQ32)*(m32 - mU32)*dilog(1 - mQ32/mU32)*pow2(mQ32 - mU32)*((-
        76*mQ32*mU32 + 34*pow4(mQ3) + 34*pow4(mU3))*pow6(m3) + 7*pow4(mU3)*
        pow6(mQ3) + pow4(m3)*(65*mU32*pow4(mQ3) + 65*mQ32*pow4(mU3) - 29*pow6(
        mQ3) - 29*pow6(mU3)) + 7*pow4(mQ3)*pow6(mU3) - 14*(mQ32 + mU32)*pow8(
        m3) + 3*mU32*pow8(mQ3) + 3*mQ32*pow8(mU3) + m32*(-34*pow4(mQ3)*pow4(
        mU3) - 26*mU32*pow6(mQ3) - 26*mQ32*pow6(mU3) + 9*pow8(mQ3) + 9*pow8(
        mU3)) + 12*power10(m3)) - pow2(lmQ3MR - log(mQ32/mU32))*pow4(m32 -
        mQ32)*((-mQ32 + mU32)*pow4(mU3)*(4*mQ32*mU32 + 3*pow4(mQ3) + 30*pow4(
        msq) + 21*pow4(mU3)) + (60*msq2*mU32 + mQ32*(-60*msq2 + 68*mU32) - 2*
        pow4(mQ3) + 702*pow4(mU3))*pow6(m3) + 2*m32*mU32*(18*mU32*pow4(mQ3) +
        6*mU32*(-15*msq2*mU32 + 5*pow4(msq) + 4*pow4(mU3)) + mQ32*(90*msq2*mU32
        - 30*pow4(msq) + 25*pow4(mU3)) - 3*pow6(mQ3)) + pow4(m3)*(-33*mU32*
        pow4(mQ3) - 90*mU32*pow4(msq) + 3*mQ32*(-40*msq2*mU32 + 30*pow4(msq) -
        47*pow4(mU3)) + 120*msq2*pow4(mU3) + 9*pow6(mQ3) - 347*pow6(mU3)) + (
        44*mQ32 - 556*mU32)*pow8(m3) + 128*power10(m3)) - 2*(mQ32 - mU32)*pow2(
        lmQ3MR - log(mQ32/m32))*(878*(mQ32 + mU32)*pow14(m3) - 262*pow16(m3) +
        pow4(m3)*pow4(mQ3)*pow4(mU3)*(452*mQ32*mU32 + pow4(mQ3) + pow4(mU3)) -
        pow12(m3)*(2972*mQ32*mU32 + 885*pow4(mQ3) + 885*pow4(mU3)) - 144*m32*(
        mQ32 + mU32)*pow6(mQ3)*pow6(mU3) + 36*pow8(mQ3)*pow8(mU3) - pow8(m3)*(
        2404*pow4(mQ3)*pow4(mU3) + 1004*mU32*pow6(mQ3) + 1004*mQ32*pow6(mU3) +
        49*pow8(mQ3) + 49*pow8(mU3)) + 2*pow6(m3)*(184*pow4(mU3)*pow6(mQ3) +
        184*pow4(mQ3)*pow6(mU3) + 79*mU32*pow8(mQ3) + 79*mQ32*pow8(mU3)) + 4*(
        733*mU32*pow4(mQ3) + 733*mQ32*pow4(mU3) + 80*pow6(mQ3) + 80*pow6(mU3))*
        power10(m3)) + 2*lmQ3MR*(m32 - mQ32)*(m32 - mU32)*(-10*(mQ32 - mU32)*(
        lmQ3MR - log(mQ32/msq2))*pow3(m32 - mU32)*(2*(mQ32 + 3*msq2)*pow4(m3) -
        3*mQ32*pow4(msq) - 3*m32*(-6*mQ32*msq2 + pow4(mQ3) + 3*pow4(msq)) +
        pow6(mQ3)) + (m32 - mU32)*((mQ32 - mU32)*mU32*pow4(mQ3)*(128*mQ32*mU32
        + 3*pow4(mQ3) - 3*(10*msq2*mU32 + pow4(mU3))) + mQ32*pow4(m3)*(-5*(6*
        msq2 + 5*mU32)*pow4(mQ3) + 6*mQ32*(35*msq2*mU32 - 157*pow4(mU3)) - 180*
        msq2*pow4(mU3) + 106*pow6(mQ3) - 163*pow6(mU3)) + pow6(m3)*((-90*msq2 +
        985*mU32)*pow4(mQ3) + mQ32*(90*msq2*mU32 + 622*pow4(mU3)) - 18*pow6(
        mQ3) - 53*pow6(mU3)) + (-764*mQ32*mU32 - 365*pow4(mQ3) + 105*pow4(mU3))
        *pow8(m3) + m32*mQ32*(6*pow4(mQ3)*(10*msq2*mU32 + 41*pow4(mU3)) - 279*
        mU32*pow6(mQ3) + 9*(10*msq2 + mU32)*pow6(mU3) + mQ32*(-150*msq2*pow4(
        mU3) + 271*pow6(mU3)) + 9*pow8(mQ3)) + 4*(77*mQ32 - 13*mU32)*power10(
        m3)) - (mQ32 - mU32)*(lmQ3MR - log(mQ32/mU32))*(mU32*(4*mQ32*mU32 + 3*
        pow4(mQ3) - 3*pow4(mU3))*pow6(mQ3) + pow6(m3)*(-53*mU32*pow4(mQ3) + 57*
        mQ32*pow4(mU3) + 35*pow6(mQ3) + pow6(mU3)) + m32*pow4(mQ3)*(-35*mU32*
        pow4(mQ3) + mQ32*pow4(mU3) + 9*pow6(mQ3) + 9*pow6(mU3)) - (28*mQ32*mU32
        + 17*pow4(mQ3) + 3*pow4(mU3))*pow8(m3) - pow4(m3)*(23*pow4(mQ3)*pow4(
        mU3) - 75*mU32*pow6(mQ3) + 19*mQ32*pow6(mU3) + 29*pow8(mQ3)) + 2*(7*
        mQ32 + mU32)*power10(m3))) - 2*(lmQ3MR - log(mQ32/m32))*(20*(m32 -
        mQ32)*(m32 - mU32)*(mQ32 - mU32)*(lmQ3MR - log(mQ32/msq2))*pow4(m3)*(7*
        mQ32*mU32*(pow4(mQ3) + pow4(mU3)) + 3*pow4(m3)*(14*mQ32*mU32 + pow4(
        mQ3) + pow4(mU3)) - 10*(mQ32 + mU32)*pow6(m3) - m32*(21*mU32*pow4(mQ3)
        + 21*mQ32*pow4(mU3) + pow6(mQ3) + pow6(mU3)) + 2*pow8(m3)) + (m32 -
        mQ32)*(m32 - mU32)*(mQ32 - mU32)*(864*pow12(m3) + 48*pow6(mQ3)*pow6(
        mU3) + 3*m32*mQ32*mU32*(5*(2*msq2 - 23*mU32)*pow4(mQ3) - 115*mQ32*pow4(
        mU3) + 10*msq2*pow4(mU3) + pow6(mQ3) + pow6(mU3)) - 9*pow6(m3)*((30*
        msq2 + 317*mU32)*pow4(mQ3) + 30*msq2*pow4(mU3) + mQ32*(-20*msq2*mU32 +
        317*pow4(mU3)) + 39*pow6(mQ3) + 39*pow6(mU3)) + 4*(60*msq2*mU32 + 2*
        mQ32*(30*msq2 + 511*mU32) + 365*pow4(mQ3) + 365*pow4(mU3))*pow8(m3) +
        pow4(m3)*(pow4(mQ3)*(-90*msq2*mU32 + 1894*pow4(mU3)) + (90*msq2 + 572*
        mU32)*pow6(mQ3) + 9*(10*msq2 + mU32)*pow6(mU3) + mQ32*(-90*msq2*pow4(
        mU3) + 572*pow6(mU3)) + 9*pow8(mQ3)) - 2*(971*mQ32 + 90*msq2 + 971*
        mU32)*power10(m3)) - 2*(m32 - mQ32)*(lmQ3MR - log(mQ32/mU32))*(-12*(17*
        mQ32 + 15*mU32)*pow14(m3) + 64*pow16(m3) + 2*pow12(m3)*(302*mQ32*mU32 +
        117*pow4(mQ3) + 61*pow4(mU3)) + 6*m32*pow4(mQ3)*(-(mQ32*mU32) + 4*pow4(
        mQ3) - 3*pow4(mU3))*pow6(mU3) + mQ32*pow4(m3)*pow4(mU3)*(-53*mU32*pow4(
        mQ3) + 61*mQ32*pow4(mU3) + 43*pow6(mQ3) + 13*pow6(mU3)) + 6*(-mQ32 +
        mU32)*pow6(mQ3)*pow8(mU3) - mU32*pow6(m3)*(13*pow4(mQ3)*pow4(mU3) +
        239*mU32*pow6(mQ3) + 49*mQ32*pow6(mU3) + 76*pow8(mQ3) + 7*pow8(mU3)) +
        pow8(m3)*(503*pow4(mQ3)*pow4(mU3) + 401*mU32*pow6(mQ3) + 13*mQ32*pow6(
        mU3) + 13*pow8(mQ3) + 30*pow8(mU3)) - (779*mU32*pow4(mQ3) + 369*mQ32*
        pow4(mU3) + 101*pow6(mQ3) + 31*pow6(mU3))*power10(m3)) + 2*lmQ3MR*(m32
        - mU32)*(-12*(15*mQ32 + 17*mU32)*pow14(m3) + 64*pow16(m3) + 2*pow12(m3)
        *(302*mQ32*mU32 + 61*pow4(mQ3) + 117*pow4(mU3)) - 6*m32*(mQ32*mU32 + 3*
        pow4(mQ3) - 4*pow4(mU3))*pow4(mU3)*pow6(mQ3) + mU32*pow4(m3)*pow4(mQ3)*
        (61*mU32*pow4(mQ3) - 53*mQ32*pow4(mU3) + 13*pow6(mQ3) + 43*pow6(mU3)) +
        6*(mQ32 - mU32)*pow6(mU3)*pow8(mQ3) + pow8(m3)*(503*pow4(mQ3)*pow4(mU3)
        + 13*mU32*pow6(mQ3) + 401*mQ32*pow6(mU3) + 30*pow8(mQ3) + 13*pow8(mU3))
        - mQ32*pow6(m3)*(13*pow4(mQ3)*pow4(mU3) + 49*mU32*pow6(mQ3) + 239*mQ32*
        pow6(mU3) + 7*pow8(mQ3) + 76*pow8(mU3)) - (369*mU32*pow4(mQ3) + 779*
        mQ32*pow4(mU3) + 31*pow6(mQ3) + 101*pow6(mU3))*power10(m3))) + 2*(m32 -
        mU32)*dilog(1 - m32/mQ32)*pow2(m32 - mQ32)*(128*pow12(m3) - mU32*pow4(
        mQ3)*(mU32*pow4(mQ3) + 19*mQ32*pow4(mU3) + 3*pow6(mQ3) - 23*pow6(mU3))
        + pow6(m3)*(-251*mU32*pow4(mQ3) - 1025*mQ32*pow4(mU3) + 9*pow6(mQ3) -
        13*pow6(mU3)) + (902*mQ32*mU32 + 98*pow4(mQ3) + 280*pow4(mU3))*pow8(m3)
        + pow4(m3)*(417*pow4(mQ3)*pow4(mU3) - 151*mU32*pow6(mQ3) + 391*mQ32*
        pow6(mU3) + 20*pow8(mQ3) - 37*pow8(mU3)) - 2*(147*mQ32 + 173*mU32)*
        power10(m3) + m32*(41*pow4(mU3)*pow6(mQ3) - 167*pow4(mQ3)*pow6(mU3) +
        41*mU32*pow8(mQ3) - 34*mQ32*pow8(mU3) - 9*power10(mQ3))) + (m32 - mU32)
        *pow2(lmQ3MR)*(-4*(139*mQ32 + 85*mU32)*pow14(m3) + 128*pow16(m3) + 2*
        pow12(m3)*(-30*msq2*mU32 + mQ32*(30*msq2 + 856*mU32) + 357*pow4(mQ3) +
        131*pow4(mU3)) - (mQ32 - mU32)*mU32*pow4(mQ3)*(21*pow4(mQ3)*pow4(mU3) +
        30*pow4(msq)*pow4(mU3) + 4*mU32*pow6(mQ3) + 3*pow8(mQ3)) + pow8(m3)*(-(
        pow4(mU3)*(180*msq2*mU32 + 270*pow4(msq) + 43*pow4(mU3))) + pow4(mQ3)*(
        -180*msq2*mU32 + 60*pow4(msq) + 2771*pow4(mU3)) + (-180*msq2 + 933*
        mU32)*pow6(mQ3) + 3*mQ32*(70*mU32*pow4(msq) + 180*msq2*pow4(mU3) + 241*
        pow6(mU3)) + 96*pow8(mQ3)) + ((120*msq2 - 2209*mU32)*pow4(mQ3) - mQ32*(
        300*msq2*mU32 + 90*pow4(msq) + 1903*pow4(mU3)) - 373*pow6(mQ3) + 5*(18*
        mU32*pow4(msq) + 36*msq2*pow4(mU3) + pow6(mU3)))*power10(m3) - 2*pow6(
        m3)*(-3*(90*msq2*mU32 + 5*pow4(msq) - 251*pow4(mU3))*pow6(mQ3) + pow4(
        mQ3)*(105*mU32*pow4(msq) + 90*msq2*pow4(mU3) + 593*pow6(mU3)) - 53*
        mU32*pow8(mQ3) + 15*mQ32*(3*pow4(msq)*pow4(mU3) + 14*msq2*pow6(mU3) +
        2*pow8(mU3)) - 15*(9*pow4(msq)*pow6(mU3) + 2*msq2*pow8(mU3)) + 21*
        power10(mQ3)) + 2*pow4(m3)*(19*pow12(mQ3) - 5*pow6(mQ3)*(9*mU32*pow4(
        msq) + 54*msq2*pow4(mU3) - 61*pow6(mU3)) + 158*pow4(mU3)*pow8(mQ3) -
        45*pow4(msq)*pow8(mU3) + 3*pow4(mQ3)*(45*pow4(msq)*pow4(mU3) + 70*msq2*
        pow6(mU3) + 27*pow8(mU3)) + mQ32*(-45*pow4(msq)*pow6(mU3) + 60*msq2*
        pow8(mU3)) - 115*mU32*power10(mQ3)) + m32*(47*mU32*pow12(mQ3) - 9*
        pow14(mQ3) - 119*pow6(mU3)*pow8(mQ3) + 6*pow6(mQ3)*(15*pow4(msq)*pow4(
        mU3) + 30*msq2*pow6(mU3) - 14*pow8(mU3)) + 60*mQ32*pow4(msq)*pow8(mU3)
        - 30*pow4(mQ3)*(5*pow4(msq)*pow6(mU3) + 6*msq2*pow8(mU3)) + 37*pow4(
        mU3)*power10(mQ3))) - 2*(m32 - mQ32)*dilog(1 - m32/mU32)*pow2(m32 -
        mU32)*(128*pow12(m3) - pow6(m3)*(1025*mU32*pow4(mQ3) + 251*mQ32*pow4(
        mU3) + 13*pow6(mQ3) - 9*pow6(mU3)) + mQ32*pow4(mU3)*(-19*mU32*pow4(mQ3)
        - mQ32*pow4(mU3) + 23*pow6(mQ3) - 3*pow6(mU3)) + (902*mQ32*mU32 + 280*
        pow4(mQ3) + 98*pow4(mU3))*pow8(m3) + pow4(m3)*(417*pow4(mQ3)*pow4(mU3)
        + 391*mU32*pow6(mQ3) - 151*mQ32*pow6(mU3) - 37*pow8(mQ3) + 20*pow8(mU3)
        ) - 2*(173*mQ32 + 147*mU32)*power10(m3) + m32*(-167*pow4(mU3)*pow6(mQ3)
        + 41*pow4(mQ3)*pow6(mU3) - 34*mU32*pow8(mQ3) + 41*mQ32*pow8(mU3) - 9*
        power10(mU3))) + 2*(m32 - mU32)*(lmQ3MR - log(mQ32/mU32))*pow2(m32 -
        mQ32)*(180*msq2*mU32*pow4(m3)*pow4(mQ3) - 210*mQ32*msq2*pow4(m3)*pow4(
        mU3) + 150*m32*msq2*pow4(mQ3)*pow4(mU3) + 942*pow4(m3)*pow4(mQ3)*pow4(
        mU3) - 90*mQ32*msq2*mU32*pow6(m3) - 622*mU32*pow4(mQ3)*pow6(m3) - 985*
        mQ32*pow4(mU3)*pow6(m3) + 90*msq2*pow4(mU3)*pow6(m3) - 90*m32*msq2*
        mU32*pow6(mQ3) + 163*mU32*pow4(m3)*pow6(mQ3) - 271*m32*pow4(mU3)*pow6(
        mQ3) - 30*msq2*pow4(mU3)*pow6(mQ3) + 53*pow6(m3)*pow6(mQ3) - 60*m32*
        mQ32*msq2*pow6(mU3) + 25*mQ32*pow4(m3)*pow6(mU3) + 30*msq2*pow4(m3)*
        pow6(mU3) - 246*m32*pow4(mQ3)*pow6(mU3) + 30*msq2*pow4(mQ3)*pow6(mU3) +
        18*pow6(m3)*pow6(mU3) + 131*pow6(mQ3)*pow6(mU3) - 10*(mQ32 - mU32)*(
        lmQ3MR - log(mQ32/msq2))*pow2(m32 - mQ32)*(2*(3*msq2 + mU32)*pow4(m3) -
        3*mU32*pow4(msq) - 3*m32*(-6*msq2*mU32 + 3*pow4(msq) + pow4(mU3)) +
        pow6(mU3)) + 764*mQ32*mU32*pow8(m3) - 105*pow4(mQ3)*pow8(m3) + 365*
        pow4(mU3)*pow8(m3) - 9*m32*mU32*pow8(mQ3) - 3*pow4(mU3)*pow8(mQ3) +
        279*m32*mQ32*pow8(mU3) - 106*pow4(m3)*pow8(mU3) - 125*pow4(mQ3)*pow8(
        mU3) + 52*mQ32*power10(m3) - 308*mU32*power10(m3) - 9*m32*power10(mU3)
        - 3*mQ32*power10(mU3)))))/(3.*mQ32*mU32*pow4(m32 - mQ32)*pow4(m32 -
        mU32)*pow4(mQ32 - mU32))+
	(-32*Xt5*(32*pow2(m32 - mQ32)*pow2(m32 - mU32)*(m3*(m32 - mQ32)*mU32*log(
        mQ32/mU32) + (mQ32 - mU32)*log(mQ32/m32)*pow3(m3))*(mQ32*(mQ32 - mU32)*
        mU32*(-2*m32 + mQ32 + mU32)*dilog(1 - m32/mQ32) + m32*mU32*pow4(mQ3) -
        lmQ3MR*m32*mU32*pow4(mQ3) + m32*mU32*log(mQ32/m32)*pow4(mQ3) + 2*m32*
        mU32*log(mQ32/mU32)*pow4(mQ3) - 2*lmQ3MR*m32*mU32*log(mQ32/mU32)*pow4(
        mQ3) + m32*mU32*pow2(log(mQ32/mU32))*pow4(mQ3) + m32*mQ32*pow4(mU3) -
        lmQ3MR*m32*mQ32*pow4(mU3) + m32*mQ32*log(mQ32/m32)*pow4(mU3) - 2*m32*
        mQ32*log(mQ32/mU32)*pow4(mU3) + 2*lmQ3MR*m32*mQ32*log(mQ32/mU32)*pow4(
        mU3) - m32*mQ32*pow2(log(mQ32/mU32))*pow4(mU3) + 12*pow4(mQ3)*pow4(mU3)
        - 8*lmQ3MR*pow4(mQ3)*pow4(mU3) + 4*log(mQ32/mU32)*pow4(mQ3)*pow4(mU3) -
        2*pow2(log(mQ32/mU32))*pow4(mQ3)*pow4(mU3) + mQ32*mU32*dilog(1 - m32/
        mU32)*(2*m32*(mQ32 - mU32) - pow4(mQ3) + pow4(mU3)) - m32*pow6(mQ3) +
        lmQ3MR*m32*pow6(mQ3) - 6*mU32*pow6(mQ3) + 4*lmQ3MR*mU32*pow6(mQ3) -
        m32*log(mQ32/m32)*pow6(mQ3) + 3*mU32*log(mQ32/mU32)*pow6(mQ3) - 2*
        lmQ3MR*mU32*log(mQ32/mU32)*pow6(mQ3) - m32*pow6(mU3) + lmQ3MR*m32*pow6(
        mU3) - 6*mQ32*pow6(mU3) + 4*lmQ3MR*mQ32*pow6(mU3) - m32*log(mQ32/m32)*
        pow6(mU3) - 7*mQ32*log(mQ32/mU32)*pow6(mU3) + 2*lmQ3MR*mQ32*log(mQ32/
        mU32)*pow6(mU3) - 2*mQ32*pow2(log(mQ32/mU32))*pow6(mU3)) - mQ32*mU32*(
        2*(mQ32 - mU32) - (mQ32 + mU32)*log(mQ32/mU32))*(-60*m3*(m32 - mQ32)*(
        m32 - msq2)*(m32 - mU32)*(mQ32*(msq2 - 2*mU32) + msq2*mU32 + m32*(mQ32
        - 2*msq2 + mU32))*dilog(1 - m32/msq2)*pow2(mQ32 - mU32) - 60*m3*msq2*(
        lmQ3MR - log(mQ32/msq2))*pow2(m32 - mQ32)*pow2(m32 - mU32)*pow2(mQ32 -
        mU32) - 30*m3*(m32 - mQ32)*(m32 - mU32)*(mQ32 - mU32)*(pow2(mQ32 -
        msq2)*pow2(m32 - mU32) - pow2(m32 - mQ32)*pow2(msq2 - mU32))*pow2(
        lmQ3MR - log(mQ32/msq2)) + 2*pow2(m32 - mQ32)*pow2(m32 - mU32)*pow2(
        mQ32 - mU32)*(-3*m3*(mQ32 + 10*msq2 + mU32) + 14*pow3(m3)) + 60*m3*(m32
        - mU32)*(mQ32 - mU32)*dilog(1 - msq2/mU32)*pow2(msq2 - mU32)*pow3(m32 -
        mQ32) - 60*m3*(m32 - mQ32)*(mQ32 - mU32)*dilog(1 - msq2/mQ32)*pow2(mQ32
        - msq2)*pow3(m32 - mU32) + 2*m3*(m32 - mQ32)*(m32 - mU32)*dilog(1 -
        mQ32/mU32)*pow3(mQ32 - mU32)*(-(mQ32*mU32) - 5*m32*(mQ32 + mU32) + 5*
        pow4(m3) + 3*pow4(mQ3) + 3*pow4(mU3)) - 2*m3*(m32 - mQ32)*(mQ32 - mU32)
        *dilog(1 - m32/mU32)*pow3(m32 - mU32)*(-7*mQ32*mU32 + m32*(-37*mQ32 +
        mU32) + 18*pow4(m3) + 22*pow4(mQ3) + 3*pow4(mU3)) + 2*m3*(m32 - mU32)*(
        mQ32 - mU32)*dilog(1 - m32/mQ32)*pow3(m32 - mQ32)*(m32*(mQ32 - 37*mU32)
        - 7*mQ32*mU32 + 18*pow4(m3) + 3*pow4(mQ3) + 22*pow4(mU3)) + 12*m3*(mQ32
        - mU32)*(m32*(mQ32 - mU32)*log(mQ32/m32) + (m32 - mQ32)*mU32*log(mQ32/
        mU32))*(-((m32 - mQ32)*((m32 - mQ32)*(2*m32 - mU32)*mU32*log(mQ32/mU32)
        + (m32 - mU32)*((1 + 2*lmQ3MR)*mQ32*mU32 - 2*lmQ3MR*m32*(mQ32 + mU32) +
        (-1 + 2*lmQ3MR)*pow4(m3)))) + log(mQ32/m32)*pow4(m3)*(-2*m32*(mQ32 +
        mU32) + 2*pow4(m3) + pow4(mQ3) + pow4(mU3))) - 2*m3*(m32 - mU32)*(mQ32
        - mU32)*(lmQ3MR - log(mQ32/mU32))*pow2(m32 - mQ32)*(-53*m32*mQ32*mU32 -
        30*m32*msq2*mU32 + 30*mQ32*msq2*mU32 - 16*mQ32*pow4(m3) + 55*mU32*pow4(
        m3) + 3*mU32*pow4(mQ3) - 61*m32*pow4(mU3) + 53*mQ32*pow4(mU3) + 5*(m32
        - mQ32)*(lmQ3MR - log(mQ32/msq2))*(-(m32*mU32) - 12*msq2*mU32 + 6*pow4(
        msq) + pow4(mU3)) + 16*pow6(m3) + 3*pow6(mU3)) - pow2(lmQ3MR - log(
        mQ32/mU32))*pow3(m32 - mQ32)*(-((69*mQ32*mU32 + pow4(mQ3) - 86*pow4(
        mU3))*pow5(m3)) + pow3(m3)*(11*mU32*pow4(mQ3) + 30*mU32*pow4(msq) - 60*
        msq2*pow4(mU3) + mQ32*(60*msq2*mU32 - 30*pow4(msq) + 59*pow4(mU3)) - 3*
        pow6(mQ3) - 99*pow6(mU3)) + m3*mU32*(-10*mU32*pow4(mQ3) - 30*mU32*pow4(
        msq) + mQ32*(-60*msq2*mU32 + 30*pow4(msq) - 4*pow4(mU3)) + 60*msq2*
        pow4(mU3) + 3*pow6(mQ3) + 27*pow6(mU3)) + 18*(mQ32 - mU32)*pow7(m3)) +
        2*pow2(mQ32 - mU32)*pow2(lmQ3MR - log(mQ32/m32))*pow3(m3)*(-142*m32*
        mQ32*mU32*(mQ32 + mU32) + 87*pow4(mQ3)*pow4(mU3) + pow4(m3)*(220*mQ32*
        mU32 + 57*pow4(mQ3) + 57*pow4(mU3)) - 82*(mQ32 + mU32)*pow6(m3) + 27*
        pow8(m3)) - (mQ32 - mU32)*(lmQ3MR - log(mQ32/m32))*(2*(m32 - mQ32)*(m32
        - mU32)*(mQ32 - mU32)*pow3(m3)*(3*mU32*(10*msq2 + mU32) - 15*m32*(5*
        mQ32 + 4*msq2 + 5*mU32) + mQ32*(30*msq2 + 59*mU32) + 85*pow4(m3) + 3*
        pow4(mQ3)) - 10*(m32 - mQ32)*(m32 - mU32)*(mQ32 - mU32)*(lmQ3MR - log(
        mQ32/msq2))*(-11*mQ32*mU32*pow3(m3) + 5*(mQ32 + mU32)*pow5(m3) + pow7(
        m3)) + lmQ3MR*(m32 - mU32)*(68*pow11(m3) + 12*m3*pow4(mU3)*pow6(mQ3) +
        pow5(m3)*(-215*mU32*pow4(mQ3) - 184*mQ32*pow4(mU3) + 3*pow6(mQ3)) +
        pow3(m3)*(105*pow4(mQ3)*pow4(mU3) - 17*mU32*pow6(mQ3)) + (353*mQ32*mU32
        + 120*pow4(mQ3) + 75*pow4(mU3))*pow7(m3) - (183*mQ32 + 137*mU32)*pow9(
        m3)) - (m32 - mQ32)*(lmQ3MR - log(mQ32/mU32))*(68*pow11(m3) + 12*m3*
        pow4(mQ3)*pow6(mU3) + pow5(m3)*(-184*mU32*pow4(mQ3) - 215*mQ32*pow4(
        mU3) + 3*pow6(mU3)) + pow3(m3)*(105*pow4(mQ3)*pow4(mU3) - 17*mQ32*pow6(
        mU3)) + (353*mQ32*mU32 + 75*pow4(mQ3) + 120*pow4(mU3))*pow7(m3) - (137*
        mQ32 + 183*mU32)*pow9(m3))) - (m32 - mU32)*pow2(lmQ3MR)*(-18*(mQ32 -
        mU32)*pow11(m3) - 2*((30*msq2 + 49*mU32)*pow4(mQ3) + 15*mU32*pow4(msq)
        - mQ32*(30*msq2*mU32 + 15*pow4(msq) + 58*pow4(mU3)) + 52*pow6(mQ3) -
        11*pow6(mU3))*pow7(m3) + pow5(m3)*(60*pow4(msq)*pow4(mU3) - 3*pow4(mQ3)
        *(-20*msq2*mU32 + 10*pow4(msq) + 9*pow4(mU3)) + (60*msq2 + 169*mU32)*
        pow6(mQ3) - mQ32*(30*mU32*pow4(msq) + 120*msq2*pow4(mU3) + 83*pow6(mU3)
        ) + 37*pow8(mQ3)) + (-33*mQ32*mU32 + 86*pow4(mQ3) - 37*pow4(mU3))*pow9(
        m3) + m3*mQ32*(-30*mQ32*msq2*(msq2 + 2*mU32)*pow4(mU3) + 42*pow4(mU3)*
        pow6(mQ3) + pow4(mQ3)*(60*msq2*pow4(mU3) - 19*pow6(mU3)) + 30*pow4(msq)
        *pow6(mU3) - 10*mU32*pow8(mQ3) + 3*power10(mQ3)) - 2*pow3(m3)*(15*mQ32*
        msq2*(msq2 - 2*mU32)*pow4(mU3) + (60*msq2*mU32 + 53*pow4(mU3))*pow6(
        mQ3) + 15*pow4(msq)*pow6(mU3) - 6*pow4(mQ3)*(5*mU32*pow4(msq) + 5*msq2*
        pow4(mU3) + 7*pow6(mU3)) + 17*mU32*pow8(mQ3) + 4*power10(mQ3))) +
        lmQ3MR*(m32 - mQ32)*(m32 - mU32)*(-10*m3*(mQ32 - mU32)*(lmQ3MR - log(
        mQ32/msq2))*pow2(m32 - mU32)*(m32*mQ32 + 12*mQ32*msq2 - pow4(mQ3) - 6*
        pow4(msq)) + 2*(m32 - mU32)*(mQ32 - mU32)*(-(mQ32*(61*mQ32 + 30*msq2 +
        53*mU32)*pow3(m3)) + m3*mQ32*(53*mQ32*mU32 + 30*msq2*mU32 + 3*pow4(mQ3)
        + 3*pow4(mU3)) + (55*mQ32 - 16*mU32)*pow5(m3) + 16*pow7(m3)) - m3*(
        lmQ3MR - log(mQ32/mU32))*((-34*mQ32*mU32 + pow4(mQ3) + pow4(mU3))*pow6(
        m3) + 38*pow4(mU3)*pow6(mQ3) + pow4(m3)*(29*mU32*pow4(mQ3) + 59*mQ32*
        pow4(mU3) + 9*pow6(mQ3) - pow6(mU3)) + 8*pow4(mQ3)*pow6(mU3) - 20*mU32*
        pow8(mQ3) - m32*(88*pow4(mQ3)*pow4(mU3) - 11*mU32*pow6(mQ3) + 9*mQ32*
        pow6(mU3) + 10*pow8(mQ3)) + 6*power10(mQ3))))))/(3.*mQ32*mU32*pow3(m32
        - mQ32)*pow3(m32 - mU32)*pow5(mQ32 - mU32))+
	(1280*Xt6*(-2*mQ32 + 2*mU32 + (mQ32 + mU32)*log(mQ32/mU32))*pow2(m3*(m32
        - mQ32)*mU32*log(mQ32/mU32) + (mQ32 - mU32)*log(mQ32/m32)*pow3(m3)))/(
        3.*pow2(m32 - mQ32)*pow2(m32 - mU32)*pow5(mQ32 - mU32));
      }
      case(Limits::MQ3_EQ_MU3):{
	 return (2*Xt4*(120*(m32 - mQ32)*mQ32*(m32 - msq2)*dilog(1 - m32/msq2)*(mQ32*msq2
        + m32*(-5*mQ32 + 3*msq2) + pow4(m3)) + 600*msq2*pow4(m3)*pow4(mQ3) +
        480*msq2*dilog(1 - msq2/mQ32)*pow4(m3)*pow4(mQ3) + 240*msq2*log(mQ32/
        m32)*pow4(m3)*pow4(mQ3) - 600*msq2*log(mQ32/msq2)*pow4(m3)*pow4(mQ3) +
        240*msq2*pow2(log(mQ32/msq2))*pow4(m3)*pow4(mQ3) + 8*dilog(1 - m32/
        mQ32)*pow2(-(m32*mQ3) + pow3(mQ3))*(-20*m32*mQ32 - 19*pow4(m3) + 15*
        pow4(mQ3)) - 360*mQ32*dilog(1 - msq2/mQ32)*pow4(m3)*pow4(msq) - 180*
        mQ32*pow2(log(mQ32/msq2))*pow4(m3)*pow4(msq) + 240*m32*dilog(1 - msq2/
        mQ32)*pow4(mQ3)*pow4(msq) + 120*m32*pow2(log(mQ32/msq2))*pow4(mQ3)*
        pow4(msq) - 360*mQ32*msq2*pow6(m3) + 240*mQ32*msq2*dilog(1 - msq2/mQ32)
        *pow6(m3) - 360*mQ32*msq2*log(mQ32/m32)*pow6(m3) + 360*mQ32*msq2*log(
        mQ32/msq2)*pow6(m3) + 120*mQ32*msq2*pow2(log(mQ32/msq2))*pow6(m3) -
        10684*pow4(mQ3)*pow6(m3) + 13512*lmQ3MR*pow4(mQ3)*pow6(m3) - 240*dilog(
        1 - msq2/mQ32)*pow4(mQ3)*pow6(m3) + 19228*log(m32/mQ32)*pow4(mQ3)*pow6(
        m3) + 11312*lmQ3MR*log(m32/mQ32)*pow4(mQ3)*pow6(m3) - 524*log(mQ32/m32)
        *pow4(mQ3)*pow6(m3) + 18144*lmQ3MR*log(mQ32/m32)*pow4(mQ3)*pow6(m3) +
        24360*log(m32/mQ32)*log(mQ32/m32)*pow4(mQ3)*pow6(m3) - 1680*log(mQ32/
        msq2)*pow4(mQ3)*pow6(m3) + 480*lmQ3MR*log(mQ32/msq2)*pow4(mQ3)*pow6(m3)
        - 2020*log(m32/mQ32)*log(mQ32/msq2)*pow4(mQ3)*pow6(m3) - 1700*log(mQ32/
        m32)*log(mQ32/msq2)*pow4(mQ3)*pow6(m3) - 4688*pow2(lmQ3MR)*pow4(mQ3)*
        pow6(m3) + 16672*pow2(log(mQ32/m32))*pow4(mQ3)*pow6(m3) - 120*pow2(log(
        mQ32/msq2))*pow4(mQ3)*pow6(m3) - 120*m32*msq2*pow6(mQ3) - 720*m32*msq2*
        dilog(1 - msq2/mQ32)*pow6(mQ3) + 120*m32*msq2*log(mQ32/m32)*pow6(mQ3) +
        120*m32*msq2*log(mQ32/msq2)*pow6(mQ3) - 360*m32*msq2*pow2(log(mQ32/
        msq2))*pow6(mQ3) + 14474*pow4(m3)*pow6(mQ3) - 14104*lmQ3MR*pow4(m3)*
        pow6(mQ3) - 120*dilog(1 - msq2/mQ32)*pow4(m3)*pow6(mQ3) - 13712*log(
        m32/mQ32)*pow4(m3)*pow6(mQ3) - 8064*lmQ3MR*log(m32/mQ32)*pow4(m3)*pow6(
        mQ3) + 2504*log(mQ32/m32)*pow4(m3)*pow6(mQ3) - 11240*lmQ3MR*log(mQ32/
        m32)*pow4(m3)*pow6(mQ3) + 1728*log(m32/mQ32)*log(mQ32/m32)*pow4(m3)*
        pow6(mQ3) + 2000*log(mQ32/msq2)*pow4(m3)*pow6(mQ3) - 320*lmQ3MR*log(
        mQ32/msq2)*pow4(m3)*pow6(mQ3) + 1440*log(m32/mQ32)*log(mQ32/msq2)*pow4(
        m3)*pow6(mQ3) + 1000*log(mQ32/m32)*log(mQ32/msq2)*pow4(m3)*pow6(mQ3) +
        4792*pow2(lmQ3MR)*pow4(m3)*pow6(mQ3) + 2548*pow2(log(mQ32/m32))*pow4(
        m3)*pow6(mQ3) - 60*pow2(log(mQ32/msq2))*pow4(m3)*pow6(mQ3) + 120*dilog(
        1 - msq2/mQ32)*pow4(msq)*pow6(mQ3) + 60*pow2(log(mQ32/msq2))*pow4(msq)*
        pow6(mQ3) + 2463*mQ32*pow8(m3) - 5440*lmQ3MR*mQ32*pow8(m3) - 5680*mQ32*
        log(m32/mQ32)*pow8(m3) - 7168*lmQ3MR*mQ32*log(m32/mQ32)*pow8(m3) - 248*
        mQ32*log(mQ32/m32)*pow8(m3) - 11720*lmQ3MR*mQ32*log(mQ32/m32)*pow8(m3)
        - 21312*mQ32*log(m32/mQ32)*log(mQ32/m32)*pow8(m3) + 640*mQ32*log(mQ32/
        msq2)*pow8(m3) - 320*lmQ3MR*mQ32*log(mQ32/msq2)*pow8(m3) + 1280*mQ32*
        log(m32/mQ32)*log(mQ32/msq2)*pow8(m3) + 1480*mQ32*log(mQ32/m32)*log(
        mQ32/msq2)*pow8(m3) + 2292*mQ32*pow2(lmQ3MR)*pow8(m3) - 18996*mQ32*
        pow2(log(mQ32/m32))*pow8(m3) - 5868*m32*pow8(mQ3) + 6312*lmQ3MR*m32*
        pow8(mQ3) - 120*msq2*pow8(mQ3) + 480*m32*dilog(1 - msq2/mQ32)*pow8(mQ3)
        - 630*m32*log(m32/mQ32)*pow8(mQ3) + 2184*lmQ3MR*m32*log(m32/mQ32)*pow8(
        mQ3) - 2142*m32*log(mQ32/m32)*pow8(mQ3) + 2280*lmQ3MR*m32*log(mQ32/m32)
        *pow8(mQ3) - 468*m32*log(m32/mQ32)*log(mQ32/m32)*pow8(mQ3) - 1120*m32*
        log(mQ32/msq2)*pow8(mQ3) + 80*lmQ3MR*m32*log(mQ32/msq2)*pow8(mQ3) +
        120*msq2*log(mQ32/msq2)*pow8(mQ3) - 390*m32*log(m32/mQ32)*log(mQ32/
        msq2)*pow8(mQ3) - 390*m32*log(mQ32/m32)*log(mQ32/msq2)*pow8(mQ3) -
        2448*m32*pow2(lmQ3MR)*pow8(mQ3) - 180*m32*pow2(log(mQ32/m32))*pow8(mQ3)
        + 240*m32*pow2(log(mQ32/msq2))*pow8(mQ3) - 176*power10(m3) + 624*
        lmQ3MR*power10(m3) + 794*log(m32/mQ32)*power10(m3) + 1736*lmQ3MR*log(
        m32/mQ32)*power10(m3) + 266*log(mQ32/m32)*power10(m3) + 2536*lmQ3MR*
        log(mQ32/m32)*power10(m3) + 6572*log(m32/mQ32)*log(mQ32/m32)*power10(
        m3) - 80*log(mQ32/msq2)*power10(m3) + 80*lmQ3MR*log(mQ32/msq2)*power10(
        m3) - 310*log(m32/mQ32)*log(mQ32/msq2)*power10(m3) - 390*log(mQ32/m32)*
        log(mQ32/msq2)*power10(m3) - 448*pow2(lmQ3MR)*power10(m3) + 6220*pow2(
        log(mQ32/m32))*power10(m3) - 209*power10(mQ3) - 904*lmQ3MR*power10(mQ3)
        - 120*dilog(1 - msq2/mQ32)*power10(mQ3) + 144*log(mQ32/m32)*power10(
        mQ3) + 240*log(mQ32/msq2)*power10(mQ3) + 500*pow2(lmQ3MR)*power10(mQ3)
        - 72*pow2(log(mQ32/m32))*power10(mQ3) - 60*pow2(log(mQ32/msq2))*
        power10(mQ3)))/(9.*pow4(m32 - mQ32)*pow6(mQ3))+
	(-32*m3*Xt5*(60*mQ32*(-m32 + mQ32)*(m32 - msq2)*(mQ32 - msq2)*dilog(1 -
        m32/msq2) + 2*mQ32*dilog(1 - m32/mQ32)*pow3(m32 - mQ32) - 60*mQ32*msq2*
        pow4(m3) + 60*mQ32*msq2*dilog(1 - msq2/mQ32)*pow4(m3) - 60*mQ32*msq2*
        log(mQ32/m32)*pow4(m3) + 90*mQ32*msq2*log(mQ32/msq2)*pow4(m3) + 30*
        mQ32*msq2*log(msq2/mQ32)*pow4(m3) + 30*mQ32*msq2*pow2(log(mQ32/msq2))*
        pow4(m3) + 120*m32*msq2*pow4(mQ3) + 60*m32*msq2*log(mQ32/m32)*pow4(mQ3)
        - 180*m32*msq2*log(mQ32/msq2)*pow4(mQ3) - 60*m32*msq2*log(msq2/mQ32)*
        pow4(mQ3) - 345*pow4(m3)*pow4(mQ3) + 274*lmQ3MR*pow4(m3)*pow4(mQ3) -
        60*dilog(1 - msq2/mQ32)*pow4(m3)*pow4(mQ3) - 342*log(m32/mQ32)*pow4(m3)
        *pow4(mQ3) - 538*log(mQ32/m32)*pow4(m3)*pow4(mQ3) + 220*lmQ3MR*log(
        mQ32/m32)*pow4(m3)*pow4(mQ3) - 492*log(m32/mQ32)*log(mQ32/m32)*pow4(m3)
        *pow4(mQ3) - 15*log(mQ32/msq2)*pow4(m3)*pow4(mQ3) + 50*log(mQ32/m32)*
        log(mQ32/msq2)*pow4(m3)*pow4(mQ3) - 602*pow2(log(mQ32/m32))*pow4(m3)*
        pow4(mQ3) - 30*pow2(log(mQ32/msq2))*pow4(m3)*pow4(mQ3) - 60*m32*mQ32*
        dilog(1 - msq2/mQ32)*pow4(msq) + 60*m32*mQ32*log(mQ32/msq2)*pow4(msq) +
        60*m32*mQ32*log(msq2/mQ32)*pow4(msq) - 30*m32*mQ32*pow2(log(mQ32/msq2))
        *pow4(msq) - 30*log(mQ32/msq2)*pow4(m3)*pow4(msq) - 30*log(msq2/mQ32)*
        pow4(m3)*pow4(msq) + 60*dilog(1 - msq2/mQ32)*pow4(mQ3)*pow4(msq) - 30*
        log(mQ32/msq2)*pow4(mQ3)*pow4(msq) - 30*log(msq2/mQ32)*pow4(mQ3)*pow4(
        msq) + 30*pow2(log(mQ32/msq2))*pow4(mQ3)*pow4(msq) + 163*mQ32*pow6(m3)
        - 122*lmQ3MR*mQ32*pow6(m3) + 70*mQ32*log(m32/mQ32)*pow6(m3) + 277*mQ32*
        log(mQ32/m32)*pow6(m3) - 156*lmQ3MR*mQ32*log(mQ32/m32)*pow6(m3) + 408*
        mQ32*log(m32/mQ32)*log(mQ32/m32)*pow6(m3) + 5*mQ32*log(mQ32/msq2)*pow6(
        m3) + 5*mQ32*log(mQ32/m32)*log(mQ32/msq2)*pow6(m3) + 495*mQ32*pow2(log(
        mQ32/m32))*pow6(m3) + 265*m32*pow6(mQ3) - 246*lmQ3MR*m32*pow6(mQ3) -
        60*msq2*pow6(mQ3) + 60*m32*dilog(1 - msq2/mQ32)*pow6(mQ3) - 60*msq2*
        dilog(1 - msq2/mQ32)*pow6(mQ3) + 274*m32*log(m32/mQ32)*pow6(mQ3) + 317*
        m32*log(mQ32/m32)*pow6(mQ3) - 96*lmQ3MR*m32*log(mQ32/m32)*pow6(mQ3) +
        15*m32*log(mQ32/msq2)*pow6(mQ3) + 90*msq2*log(mQ32/msq2)*pow6(mQ3) -
        55*m32*log(mQ32/m32)*log(mQ32/msq2)*pow6(mQ3) + 30*msq2*log(msq2/mQ32)*
        pow6(mQ3) + 87*m32*pow2(log(mQ32/m32))*pow6(mQ3) + 30*m32*pow2(log(
        mQ32/msq2))*pow6(mQ3) - 30*msq2*pow2(log(mQ32/msq2))*pow6(mQ3) - 16*
        pow8(m3) + 16*lmQ3MR*pow8(m3) - 2*log(m32/mQ32)*pow8(m3) - 50*log(mQ32/
        m32)*pow8(m3) + 32*lmQ3MR*log(mQ32/m32)*pow8(m3) - 124*log(m32/mQ32)*
        log(mQ32/m32)*pow8(m3) - 156*pow2(log(mQ32/m32))*pow8(m3) - 67*pow8(
        mQ3) + 78*lmQ3MR*pow8(mQ3) - 6*log(mQ32/m32)*pow8(mQ3) - 5*log(mQ32/
        msq2)*pow8(mQ3)))/(9.*pow4(m32 - mQ32)*pow6(mQ3))+
	(640*Xt6*pow2(-(m3*mQ32) + pow3(m3) + log(mQ32/m32)*pow3(m3)))/(9.*pow4(-
        (m32*mQ3) + pow3(mQ3)));
      }
      case(Limits::MQ3_EQ_M3):{
	 return -(Xt4*(122320*msq2*mU32*pow14(mQ3) - 6560*lmQ3MR*msq2*mU32*pow14(mQ3) +
        40992*msq2*mU32*dilog(1 - mQ32/mU32)*pow14(mQ3) - 5376*lmQ3MR*msq2*
        mU32*dilog(1 - mQ32/mU32)*pow14(mQ3) + 5760*msq2*mU32*dilog(1 - msq2/
        mU32)*pow14(mQ3) - 5280*msq2*mU32*log(mQ32/msq2)*pow14(mQ3) - 3840*
        lmQ3MR*msq2*mU32*log(mQ32/msq2)*pow14(mQ3) + 960*msq2*mU32*dilog(1 -
        mQ32/mU32)*log(mQ32/msq2)*pow14(mQ3) - 45464*msq2*mU32*log(mQ32/mU32)*
        pow14(mQ3) - 26288*lmQ3MR*msq2*mU32*log(mQ32/mU32)*pow14(mQ3) - 8208*
        msq2*mU32*dilog(1 - mQ32/mU32)*log(mQ32/mU32)*pow14(mQ3) - 2880*msq2*
        mU32*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow14(mQ3) + 720*msq2*mU32*
        log(mQ32/msq2)*log(mQ32/mU32)*pow14(mQ3) - 1920*lmQ3MR*msq2*mU32*log(
        mQ32/msq2)*log(mQ32/mU32)*pow14(mQ3) + 192*msq2*pow16(mQ3) + 5184*
        lmQ3MR*msq2*pow16(mQ3) - 130960*mU32*pow16(mQ3) + 6560*lmQ3MR*mU32*
        pow16(mQ3) - 40992*mU32*dilog(1 - mQ32/mU32)*pow16(mQ3) + 5376*lmQ3MR*
        mU32*dilog(1 - mQ32/mU32)*pow16(mQ3) - 960*msq2*log(mQ32/msq2)*pow16(
        mQ3) + 960*lmQ3MR*msq2*log(mQ32/msq2)*pow16(mQ3) + 12000*mU32*log(mQ32/
        msq2)*pow16(mQ3) + 3840*lmQ3MR*mU32*log(mQ32/msq2)*pow16(mQ3) - 960*
        mU32*dilog(1 - mQ32/mU32)*log(mQ32/msq2)*pow16(mQ3) - 96*msq2*log(mQ32/
        mU32)*pow16(mQ3) + 96*lmQ3MR*msq2*log(mQ32/mU32)*pow16(mQ3) + 49784*
        mU32*log(mQ32/mU32)*pow16(mQ3) + 26288*lmQ3MR*mU32*log(mQ32/mU32)*
        pow16(mQ3) + 8208*mU32*dilog(1 - mQ32/mU32)*log(mQ32/mU32)*pow16(mQ3) -
        9840*mU32*log(mQ32/msq2)*log(mQ32/mU32)*pow16(mQ3) + 1920*lmQ3MR*mU32*
        log(mQ32/msq2)*log(mQ32/mU32)*pow16(mQ3) - 192*pow18(mQ3) - 5184*
        lmQ3MR*pow18(mQ3) + 960*log(mQ32/msq2)*pow18(mQ3) - 960*lmQ3MR*log(
        mQ32/msq2)*pow18(mQ3) + 96*log(mQ32/mU32)*pow18(mQ3) - 96*lmQ3MR*log(
        mQ32/mU32)*pow18(mQ3) - 2496*msq2*mU32*pow14(mQ3)*pow2(lmQ3MR) + 22752*
        msq2*mU32*log(mQ32/mU32)*pow14(mQ3)*pow2(lmQ3MR) - 5376*msq2*pow16(mQ3)
        *pow2(lmQ3MR) + 2496*mU32*pow16(mQ3)*pow2(lmQ3MR) - 22752*mU32*log(
        mQ32/mU32)*pow16(mQ3)*pow2(lmQ3MR) + 5376*pow18(mQ3)*pow2(lmQ3MR) +
        3330*msq2*mU32*pow14(mQ3)*pow2(log(mQ32/msq2)) - 1665*msq2*mU32*log(
        mQ32/mU32)*pow14(mQ3)*pow2(log(mQ32/msq2)) - 270*mU32*pow16(mQ3)*pow2(
        log(mQ32/msq2)) + 135*mU32*log(mQ32/mU32)*pow16(mQ3)*pow2(log(mQ32/
        msq2)) + 21480*msq2*mU32*pow14(mQ3)*pow2(log(mQ32/mU32)) - 2496*lmQ3MR*
        msq2*mU32*pow14(mQ3)*pow2(log(mQ32/mU32)) + 3840*msq2*mU32*log(mQ32/
        msq2)*pow14(mQ3)*pow2(log(mQ32/mU32)) - 18600*mU32*pow16(mQ3)*pow2(log(
        mQ32/mU32)) + 2496*lmQ3MR*mU32*pow16(mQ3)*pow2(log(mQ32/mU32)) - 960*
        mU32*log(mQ32/msq2)*pow16(mQ3)*pow2(log(mQ32/mU32)) - 5640*msq2*mU32*
        pow14(mQ3)*pow3(log(mQ32/mU32)) + 4200*mU32*pow16(mQ3)*pow3(log(mQ32/
        mU32)) - 29968*msq2*pow12(mU3)*pow4(mQ3) - 58720*lmQ3MR*msq2*pow12(mU3)
        *pow4(mQ3) + 3936*msq2*dilog(1 - mQ32/mU32)*pow12(mU3)*pow4(mQ3) +
        5376*lmQ3MR*msq2*dilog(1 - mQ32/mU32)*pow12(mU3)*pow4(mQ3) - 2880*msq2*
        dilog(1 - msq2/mU32)*pow12(mU3)*pow4(mQ3) + 17760*msq2*log(mQ32/msq2)*
        pow12(mU3)*pow4(mQ3) - 960*lmQ3MR*msq2*log(mQ32/msq2)*pow12(mU3)*pow4(
        mQ3) - 960*msq2*dilog(1 - mQ32/mU32)*log(mQ32/msq2)*pow12(mU3)*pow4(
        mQ3) + 520*msq2*log(mQ32/mU32)*pow12(mU3)*pow4(mQ3) - 63920*lmQ3MR*
        msq2*log(mQ32/mU32)*pow12(mU3)*pow4(mQ3) - 528*msq2*dilog(1 - mQ32/
        mU32)*log(mQ32/mU32)*pow12(mU3)*pow4(mQ3) - 1440*msq2*dilog(1 - msq2/
        mU32)*log(mQ32/mU32)*pow12(mU3)*pow4(mQ3) + 13200*msq2*log(mQ32/msq2)*
        log(mQ32/mU32)*pow12(mU3)*pow4(mQ3) + 29376*msq2*pow12(mU3)*pow2(
        lmQ3MR)*pow4(mQ3) + 12000*msq2*log(mQ32/mU32)*pow12(mU3)*pow2(lmQ3MR)*
        pow4(mQ3) - 1890*msq2*pow12(mU3)*pow2(log(mQ32/msq2))*pow4(mQ3) - 945*
        msq2*log(mQ32/mU32)*pow12(mU3)*pow2(log(mQ32/msq2))*pow4(mQ3) + 14616*
        msq2*pow12(mU3)*pow2(log(mQ32/mU32))*pow4(mQ3) - 17376*lmQ3MR*msq2*
        pow12(mU3)*pow2(log(mQ32/mU32))*pow4(mQ3) + 2400*msq2*log(mQ32/msq2)*
        pow12(mU3)*pow2(log(mQ32/mU32))*pow4(mQ3) + 4056*msq2*pow12(mU3)*pow3(
        log(mQ32/mU32))*pow4(mQ3) + 8640*mU32*pow12(mQ3)*pow4(msq) - 14400*
        mU32*dilog(1 - msq2/mU32)*pow12(mQ3)*pow4(msq) - 8640*mU32*log(mQ32/
        msq2)*pow12(mQ3)*pow4(msq) - 4320*mU32*log(mQ32/mU32)*pow12(mQ3)*pow4(
        msq) + 7200*mU32*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow12(mQ3)*pow4(
        msq) + 18720*mU32*log(mQ32/msq2)*log(mQ32/mU32)*pow12(mQ3)*pow4(msq) -
        7290*mU32*pow12(mQ3)*pow2(log(mQ32/msq2))*pow4(msq) + 3645*mU32*log(
        mQ32/mU32)*pow12(mQ3)*pow2(log(mQ32/msq2))*pow4(msq) + 90*mQ32*pow12(
        mU3)*pow2(log(mQ32/msq2))*pow4(msq) + 45*mQ32*log(mQ32/mU32)*pow12(mU3)
        *pow2(log(mQ32/msq2))*pow4(msq) - 7200*mU32*pow12(mQ3)*pow2(log(mQ32/
        mU32))*pow4(msq) - 7200*mU32*log(mQ32/msq2)*pow12(mQ3)*pow2(log(mQ32/
        mU32))*pow4(msq) + 3600*mU32*pow12(mQ3)*pow3(log(mQ32/mU32))*pow4(msq)
        - 536144*msq2*pow12(mQ3)*pow4(mU3) - 85088*lmQ3MR*msq2*pow12(mQ3)*pow4(
        mU3) - 155424*msq2*dilog(1 - mQ32/mU32)*pow12(mQ3)*pow4(mU3) + 26880*
        lmQ3MR*msq2*dilog(1 - mQ32/mU32)*pow12(mQ3)*pow4(mU3) + 11520*msq2*
        dilog(1 - msq2/mU32)*pow12(mQ3)*pow4(mU3) + 60000*msq2*log(mQ32/msq2)*
        pow12(mQ3)*pow4(mU3) + 4800*lmQ3MR*msq2*log(mQ32/msq2)*pow12(mQ3)*pow4(
        mU3) - 4800*msq2*dilog(1 - mQ32/mU32)*log(mQ32/msq2)*pow12(mQ3)*pow4(
        mU3) + 13032*msq2*log(mQ32/mU32)*pow12(mQ3)*pow4(mU3) + 119760*lmQ3MR*
        msq2*log(mQ32/mU32)*pow12(mQ3)*pow4(mU3) - 14928*msq2*dilog(1 - mQ32/
        mU32)*log(mQ32/mU32)*pow12(mQ3)*pow4(mU3) - 11520*msq2*dilog(1 - msq2/
        mU32)*log(mQ32/mU32)*pow12(mQ3)*pow4(mU3) - 30960*msq2*log(mQ32/msq2)*
        log(mQ32/mU32)*pow12(mQ3)*pow4(mU3) + 7680*lmQ3MR*msq2*log(mQ32/msq2)*
        log(mQ32/mU32)*pow12(mQ3)*pow4(mU3) + 559184*pow14(mQ3)*pow4(mU3) +
        85088*lmQ3MR*pow14(mQ3)*pow4(mU3) + 155424*dilog(1 - mQ32/mU32)*pow14(
        mQ3)*pow4(mU3) - 26880*lmQ3MR*dilog(1 - mQ32/mU32)*pow14(mQ3)*pow4(mU3)
        - 5760*dilog(1 - msq2/mU32)*pow14(mQ3)*pow4(mU3) - 73440*log(mQ32/msq2)
        *pow14(mQ3)*pow4(mU3) - 4800*lmQ3MR*log(mQ32/msq2)*pow14(mQ3)*pow4(mU3)
        + 4800*dilog(1 - mQ32/mU32)*log(mQ32/msq2)*pow14(mQ3)*pow4(mU3) - 7272*
        log(mQ32/mU32)*pow14(mQ3)*pow4(mU3) - 119760*lmQ3MR*log(mQ32/mU32)*
        pow14(mQ3)*pow4(mU3) + 14928*dilog(1 - mQ32/mU32)*log(mQ32/mU32)*pow14(
        mQ3)*pow4(mU3) + 2880*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow14(mQ3)*
        pow4(mU3) + 25200*log(mQ32/msq2)*log(mQ32/mU32)*pow14(mQ3)*pow4(mU3) -
        7680*lmQ3MR*log(mQ32/msq2)*log(mQ32/mU32)*pow14(mQ3)*pow4(mU3) + 93120*
        msq2*pow12(mQ3)*pow2(lmQ3MR)*pow4(mU3) - 79008*msq2*log(mQ32/mU32)*
        pow12(mQ3)*pow2(lmQ3MR)*pow4(mU3) - 93120*pow14(mQ3)*pow2(lmQ3MR)*pow4(
        mU3) + 79008*log(mQ32/mU32)*pow14(mQ3)*pow2(lmQ3MR)*pow4(mU3) + 3510*
        msq2*pow12(mQ3)*pow2(log(mQ32/msq2))*pow4(mU3) - 5085*msq2*log(mQ32/
        mU32)*pow12(mQ3)*pow2(log(mQ32/msq2))*pow4(mU3) - 1530*pow14(mQ3)*pow2(
        log(mQ32/msq2))*pow4(mU3) + 1035*log(mQ32/mU32)*pow14(mQ3)*pow2(log(
        mQ32/msq2))*pow4(mU3) - 27816*msq2*pow12(mQ3)*pow2(log(mQ32/mU32))*
        pow4(mU3) + 18240*lmQ3MR*msq2*pow12(mQ3)*pow2(log(mQ32/mU32))*pow4(mU3)
        + 1920*msq2*log(mQ32/msq2)*pow12(mQ3)*pow2(log(mQ32/mU32))*pow4(mU3) +
        26376*pow14(mQ3)*pow2(log(mQ32/mU32))*pow4(mU3) - 18240*lmQ3MR*pow14(
        mQ3)*pow2(log(mQ32/mU32))*pow4(mU3) + 6720*log(mQ32/msq2)*pow14(mQ3)*
        pow2(log(mQ32/mU32))*pow4(mU3) - 16200*msq2*pow12(mQ3)*pow3(log(mQ32/
        mU32))*pow4(mU3) + 11880*pow14(mQ3)*pow3(log(mQ32/mU32))*pow4(mU3) +
        90*(3*mQ32 + msq2)*mU32*dilog(1 - msq2/mQ32)*(-2*mQ32 + 2*mU32 + (mQ32
        + mU32)*log(mQ32/mU32))*pow2(mQ32 - msq2)*pow4(mQ32 - mU32) + 29968*
        pow12(mU3)*pow6(mQ3) + 58720*lmQ3MR*pow12(mU3)*pow6(mQ3) - 3936*dilog(1
        - mQ32/mU32)*pow12(mU3)*pow6(mQ3) - 5376*lmQ3MR*dilog(1 - mQ32/mU32)*
        pow12(mU3)*pow6(mQ3) + 2880*dilog(1 - msq2/mU32)*pow12(mU3)*pow6(mQ3) -
        15840*log(mQ32/msq2)*pow12(mU3)*pow6(mQ3) + 960*lmQ3MR*log(mQ32/msq2)*
        pow12(mU3)*pow6(mQ3) + 960*dilog(1 - mQ32/mU32)*log(mQ32/msq2)*pow12(
        mU3)*pow6(mQ3) - 520*log(mQ32/mU32)*pow12(mU3)*pow6(mQ3) + 63920*
        lmQ3MR*log(mQ32/mU32)*pow12(mU3)*pow6(mQ3) + 528*dilog(1 - mQ32/mU32)*
        log(mQ32/mU32)*pow12(mU3)*pow6(mQ3) + 1440*dilog(1 - msq2/mU32)*log(
        mQ32/mU32)*pow12(mU3)*pow6(mQ3) - 12240*log(mQ32/msq2)*log(mQ32/mU32)*
        pow12(mU3)*pow6(mQ3) - 29376*pow12(mU3)*pow2(lmQ3MR)*pow6(mQ3) - 12000*
        log(mQ32/mU32)*pow12(mU3)*pow2(lmQ3MR)*pow6(mQ3) + 1710*pow12(mU3)*
        pow2(log(mQ32/msq2))*pow6(mQ3) + 855*log(mQ32/mU32)*pow12(mU3)*pow2(
        log(mQ32/msq2))*pow6(mQ3) - 14616*pow12(mU3)*pow2(log(mQ32/mU32))*pow6(
        mQ3) + 17376*lmQ3MR*pow12(mU3)*pow2(log(mQ32/mU32))*pow6(mQ3) - 2400*
        log(mQ32/msq2)*pow12(mU3)*pow2(log(mQ32/mU32))*pow6(mQ3) - 4056*pow12(
        mU3)*pow3(log(mQ32/mU32))*pow6(mQ3) + 90*pow12(mU3)*pow2(log(mQ32/msq2)
        )*pow6(msq) + 45*log(mQ32/mU32)*pow12(mU3)*pow2(log(mQ32/msq2))*pow6(
        msq) - 919840*pow12(mQ3)*pow6(mU3) - 301504*lmQ3MR*pow12(mQ3)*pow6(mU3)
        - 216384*dilog(1 - mQ32/mU32)*pow12(mQ3)*pow6(mU3) + 53760*lmQ3MR*
        dilog(1 - mQ32/mU32)*pow12(mQ3)*pow6(mU3) + 2880*dilog(1 - msq2/mU32)*
        pow12(mQ3)*pow6(mU3) + 154560*log(mQ32/msq2)*pow12(mQ3)*pow6(mU3) -
        9600*dilog(1 - mQ32/mU32)*log(mQ32/msq2)*pow12(mQ3)*pow6(mU3) - 284624*
        log(mQ32/mU32)*pow12(mQ3)*pow6(mU3) + 137056*lmQ3MR*log(mQ32/mU32)*
        pow12(mQ3)*pow6(mU3) - 89952*dilog(1 - mQ32/mU32)*log(mQ32/mU32)*pow12(
        mQ3)*pow6(mU3) + 4320*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow12(mQ3)*
        pow6(mU3) - 4320*log(mQ32/msq2)*log(mQ32/mU32)*pow12(mQ3)*pow6(mU3) +
        11520*lmQ3MR*log(mQ32/msq2)*log(mQ32/mU32)*pow12(mQ3)*pow6(mU3) +
        240000*pow12(mQ3)*pow2(lmQ3MR)*pow6(mU3) - 88512*log(mQ32/mU32)*pow12(
        mQ3)*pow2(lmQ3MR)*pow6(mU3) - 1260*pow12(mQ3)*pow2(log(mQ32/msq2))*
        pow6(mU3) + 2430*log(mQ32/mU32)*pow12(mQ3)*pow2(log(mQ32/msq2))*pow6(
        mU3) - 41424*pow12(mQ3)*pow2(log(mQ32/mU32))*pow6(mU3) + 62496*lmQ3MR*
        pow12(mQ3)*pow2(log(mQ32/mU32))*pow6(mU3) - 11040*log(mQ32/msq2)*pow12(
        mQ3)*pow2(log(mQ32/mU32))*pow6(mU3) - 39888*pow12(mQ3)*pow3(log(mQ32/
        mU32))*pow6(mU3) + 2880*dilog(1 - msq2/mU32)*pow6(mQ3)*pow6(msq)*pow6(
        mU3) + 4320*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow6(mQ3)*pow6(msq)*
        pow6(mU3) - 2880*log(mQ32/msq2)*log(mQ32/mU32)*pow6(mQ3)*pow6(msq)*
        pow6(mU3) + 540*pow2(log(mQ32/msq2))*pow6(mQ3)*pow6(msq)*pow6(mU3) +
        2250*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow6(mQ3)*pow6(msq)*pow6(mU3)
        + 1440*pow2(log(mQ32/mU32))*pow6(mQ3)*pow6(msq)*pow6(mU3) - 4320*log(
        mQ32/msq2)*pow2(log(mQ32/mU32))*pow6(mQ3)*pow6(msq)*pow6(mU3) + 2160*
        pow3(log(mQ32/mU32))*pow6(mQ3)*pow6(msq)*pow6(mU3) - 14400*dilog(1 -
        msq2/mU32)*pow4(mU3)*pow6(msq)*pow8(mQ3) - 1440*dilog(1 - msq2/mU32)*
        log(mQ32/mU32)*pow4(mU3)*pow6(msq)*pow8(mQ3) + 14400*log(mQ32/msq2)*
        log(mQ32/mU32)*pow4(mU3)*pow6(msq)*pow8(mQ3) - 6750*pow2(log(mQ32/msq2)
        )*pow4(mU3)*pow6(msq)*pow8(mQ3) - 855*log(mQ32/mU32)*pow2(log(mQ32/
        msq2))*pow4(mU3)*pow6(msq)*pow8(mQ3) - 7200*pow2(log(mQ32/mU32))*pow4(
        mU3)*pow6(msq)*pow8(mQ3) + 1440*log(mQ32/msq2)*pow2(log(mQ32/mU32))*
        pow4(mU3)*pow6(msq)*pow8(mQ3) - 720*pow3(log(mQ32/mU32))*pow4(mU3)*
        pow6(msq)*pow8(mQ3) + 17280*pow4(msq)*pow6(mU3)*pow8(mQ3) + 25920*
        dilog(1 - msq2/mU32)*pow4(msq)*pow6(mU3)*pow8(mQ3) - 17280*log(mQ32/
        msq2)*pow4(msq)*pow6(mU3)*pow8(mQ3) + 20160*log(mQ32/mU32)*pow4(msq)*
        pow6(mU3)*pow8(mQ3) - 7200*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow4(
        msq)*pow6(mU3)*pow8(mQ3) - 31680*log(mQ32/msq2)*log(mQ32/mU32)*pow4(
        msq)*pow6(mU3)*pow8(mQ3) + 12060*pow2(log(mQ32/msq2))*pow4(msq)*pow6(
        mU3)*pow8(mQ3) - 3510*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow4(msq)*
        pow6(mU3)*pow8(mQ3) + 14400*pow2(log(mQ32/mU32))*pow4(msq)*pow6(mU3)*
        pow8(mQ3) + 7200*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(msq)*pow6(
        mU3)*pow8(mQ3) - 3600*pow3(log(mQ32/mU32))*pow4(msq)*pow6(mU3)*pow8(
        mQ3) - 20160*dilog(1 - msq2/mU32)*pow4(msq)*pow6(mQ3)*pow8(mU3) - 5760*
        log(mQ32/mU32)*pow4(msq)*pow6(mQ3)*pow8(mU3) - 10080*dilog(1 - msq2/
        mU32)*log(mQ32/mU32)*pow4(msq)*pow6(mQ3)*pow8(mU3) + 23040*log(mQ32/
        msq2)*log(mQ32/mU32)*pow4(msq)*pow6(mQ3)*pow8(mU3) - 9180*pow2(log(
        mQ32/msq2))*pow4(msq)*pow6(mQ3)*pow8(mU3) - 4950*log(mQ32/mU32)*pow2(
        log(mQ32/msq2))*pow4(msq)*pow6(mQ3)*pow8(mU3) - 14400*pow2(log(mQ32/
        mU32))*pow4(msq)*pow6(mQ3)*pow8(mU3) + 10080*log(mQ32/msq2)*pow2(log(
        mQ32/mU32))*pow4(msq)*pow6(mQ3)*pow8(mU3) - 5040*pow3(log(mQ32/mU32))*
        pow4(msq)*pow6(mQ3)*pow8(mU3) + 2880*dilog(1 - msq2/mU32)*pow4(mQ3)*
        pow6(msq)*pow8(mU3) + 1440*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow4(
        mQ3)*pow6(msq)*pow8(mU3) - 2880*log(mQ32/msq2)*log(mQ32/mU32)*pow4(mQ3)
        *pow6(msq)*pow8(mU3) + 2340*pow2(log(mQ32/msq2))*pow4(mQ3)*pow6(msq)*
        pow8(mU3) + 810*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow4(mQ3)*pow6(msq)
        *pow8(mU3) + 1440*pow2(log(mQ32/mU32))*pow4(mQ3)*pow6(msq)*pow8(mU3) -
        1440*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(mQ3)*pow6(msq)*pow8(mU3)
        + 720*pow3(log(mQ32/mU32))*pow4(mQ3)*pow6(msq)*pow8(mU3) - 716896*msq2*
        pow8(mQ3)*pow8(mU3) - 406144*lmQ3MR*msq2*pow8(mQ3)*pow8(mU3) - 126528*
        msq2*dilog(1 - mQ32/mU32)*pow8(mQ3)*pow8(mU3) + 53760*lmQ3MR*msq2*
        dilog(1 - mQ32/mU32)*pow8(mQ3)*pow8(mU3) + 2880*msq2*dilog(1 - msq2/
        mU32)*pow8(mQ3)*pow8(mU3) + 176640*msq2*log(mQ32/msq2)*pow8(mQ3)*pow8(
        mU3) - 4800*lmQ3MR*msq2*log(mQ32/msq2)*pow8(mQ3)*pow8(mU3) - 9600*msq2*
        dilog(1 - mQ32/mU32)*log(mQ32/msq2)*pow8(mQ3)*pow8(mU3) - 385168*msq2*
        log(mQ32/mU32)*pow8(mQ3)*pow8(mU3) - 44416*lmQ3MR*msq2*log(mQ32/mU32)*
        pow8(mQ3)*pow8(mU3) - 102816*msq2*dilog(1 - mQ32/mU32)*log(mQ32/mU32)*
        pow8(mQ3)*pow8(mU3) + 12960*msq2*dilog(1 - msq2/mU32)*log(mQ32/mU32)*
        pow8(mQ3)*pow8(mU3) + 21600*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow8(
        mQ3)*pow8(mU3) + 7680*lmQ3MR*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow8(
        mQ3)*pow8(mU3) + 266880*msq2*pow2(lmQ3MR)*pow8(mQ3)*pow8(mU3) - 19008*
        msq2*log(mQ32/mU32)*pow2(lmQ3MR)*pow8(mQ3)*pow8(mU3) - 3060*msq2*pow2(
        log(mQ32/msq2))*pow8(mQ3)*pow8(mU3) + 6030*msq2*log(mQ32/mU32)*pow2(
        log(mQ32/msq2))*pow8(mQ3)*pow8(mU3) - 54768*msq2*pow2(log(mQ32/mU32))*
        pow8(mQ3)*pow8(mU3) + 62112*lmQ3MR*msq2*pow2(log(mQ32/mU32))*pow8(mQ3)*
        pow8(mU3) - 12000*msq2*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow8(mQ3)*
        pow8(mU3) - 16080*msq2*pow3(log(mQ32/mU32))*pow8(mQ3)*pow8(mU3) -
        23040*pow4(msq)*pow4(mU3)*power10(mQ3) + 8640*dilog(1 - msq2/mU32)*
        pow4(msq)*pow4(mU3)*power10(mQ3) + 23040*log(mQ32/msq2)*pow4(msq)*pow4(
        mU3)*power10(mQ3) - 5760*log(mQ32/mU32)*pow4(msq)*pow4(mU3)*power10(
        mQ3) + 10080*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow4(msq)*pow4(mU3)*
        power10(mQ3) - 11520*log(mQ32/msq2)*log(mQ32/mU32)*pow4(msq)*pow4(mU3)*
        power10(mQ3) + 4770*pow2(log(mQ32/msq2))*pow4(msq)*pow4(mU3)*power10(
        mQ3) + 4905*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow4(msq)*pow4(mU3)*
        power10(mQ3) + 8640*pow2(log(mQ32/mU32))*pow4(msq)*pow4(mU3)*power10(
        mQ3) - 10080*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(msq)*pow4(mU3)*
        power10(mQ3) + 5040*pow3(log(mQ32/mU32))*pow4(msq)*pow4(mU3)*power10(
        mQ3) + 8640*mU32*dilog(1 - msq2/mU32)*pow6(msq)*power10(mQ3) - 4320*
        mU32*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow6(msq)*power10(mQ3) - 8640*
        mU32*log(mQ32/msq2)*log(mQ32/mU32)*pow6(msq)*power10(mQ3) + 4230*mU32*
        pow2(log(mQ32/msq2))*pow6(msq)*power10(mQ3) - 2115*mU32*log(mQ32/mU32)*
        pow2(log(mQ32/msq2))*pow6(msq)*power10(mQ3) + 4320*mU32*pow2(log(mQ32/
        mU32))*pow6(msq)*power10(mQ3) + 4320*mU32*log(mQ32/msq2)*pow2(log(mQ32/
        mU32))*pow6(msq)*power10(mQ3) - 2160*mU32*pow3(log(mQ32/mU32))*pow6(
        msq)*power10(mQ3) + 902560*msq2*pow6(mU3)*power10(mQ3) + 301504*lmQ3MR*
        msq2*pow6(mU3)*power10(mQ3) + 216384*msq2*dilog(1 - mQ32/mU32)*pow6(
        mU3)*power10(mQ3) - 53760*lmQ3MR*msq2*dilog(1 - mQ32/mU32)*pow6(mU3)*
        power10(mQ3) - 31680*msq2*dilog(1 - msq2/mU32)*pow6(mU3)*power10(mQ3) -
        156480*msq2*log(mQ32/msq2)*pow6(mU3)*power10(mQ3) + 9600*msq2*dilog(1 -
        mQ32/mU32)*log(mQ32/msq2)*pow6(mU3)*power10(mQ3) + 264464*msq2*log(
        mQ32/mU32)*pow6(mU3)*power10(mQ3) - 137056*lmQ3MR*msq2*log(mQ32/mU32)*
        pow6(mU3)*power10(mQ3) + 89952*msq2*dilog(1 - mQ32/mU32)*log(mQ32/mU32)
        *pow6(mU3)*power10(mQ3) - 1440*msq2*dilog(1 - msq2/mU32)*log(mQ32/mU32)
        *pow6(mU3)*power10(mQ3) + 40800*msq2*log(mQ32/msq2)*log(mQ32/mU32)*
        pow6(mU3)*power10(mQ3) - 11520*lmQ3MR*msq2*log(mQ32/msq2)*log(mQ32/
        mU32)*pow6(mU3)*power10(mQ3) - 240000*msq2*pow2(lmQ3MR)*pow6(mU3)*
        power10(mQ3) + 88512*msq2*log(mQ32/mU32)*pow2(lmQ3MR)*pow6(mU3)*
        power10(mQ3) - 11340*msq2*pow2(log(mQ32/msq2))*pow6(mU3)*power10(mQ3) -
        1170*msq2*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow6(mU3)*power10(mQ3) +
        25584*msq2*pow2(log(mQ32/mU32))*pow6(mU3)*power10(mQ3) - 62496*lmQ3MR*
        msq2*pow2(log(mQ32/mU32))*pow6(mU3)*power10(mQ3) + 8160*msq2*log(mQ32/
        msq2)*pow2(log(mQ32/mU32))*pow6(mU3)*power10(mQ3) + 41328*msq2*pow3(
        log(mQ32/mU32))*pow6(mU3)*power10(mQ3) + 716896*pow8(mU3)*power10(mQ3)
        + 406144*lmQ3MR*pow8(mU3)*power10(mQ3) + 126528*dilog(1 - mQ32/mU32)*
        pow8(mU3)*power10(mQ3) - 53760*lmQ3MR*dilog(1 - mQ32/mU32)*pow8(mU3)*
        power10(mQ3) + 14400*dilog(1 - msq2/mU32)*pow8(mU3)*power10(mQ3) -
        157440*log(mQ32/msq2)*pow8(mU3)*power10(mQ3) + 4800*lmQ3MR*log(mQ32/
        msq2)*pow8(mU3)*power10(mQ3) + 9600*dilog(1 - mQ32/mU32)*log(mQ32/msq2)
        *pow8(mU3)*power10(mQ3) + 390928*log(mQ32/mU32)*pow8(mU3)*power10(mQ3)
        + 44416*lmQ3MR*log(mQ32/mU32)*pow8(mU3)*power10(mQ3) + 102816*dilog(1 -
        mQ32/mU32)*log(mQ32/mU32)*pow8(mU3)*power10(mQ3) - 4320*dilog(1 - msq2/
        mU32)*log(mQ32/mU32)*pow8(mU3)*power10(mQ3) - 39840*log(mQ32/msq2)*log(
        mQ32/mU32)*pow8(mU3)*power10(mQ3) - 7680*lmQ3MR*log(mQ32/msq2)*log(
        mQ32/mU32)*pow8(mU3)*power10(mQ3) - 266880*pow2(lmQ3MR)*pow8(mU3)*
        power10(mQ3) + 19008*log(mQ32/mU32)*pow2(lmQ3MR)*pow8(mU3)*power10(mQ3)
        + 9900*pow2(log(mQ32/msq2))*pow8(mU3)*power10(mQ3) - 1890*log(mQ32/
        mU32)*pow2(log(mQ32/msq2))*pow8(mU3)*power10(mQ3) + 67728*pow2(log(
        mQ32/mU32))*pow8(mU3)*power10(mQ3) - 62112*lmQ3MR*pow2(log(mQ32/mU32))*
        pow8(mU3)*power10(mQ3) + 3360*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow8(
        mU3)*power10(mQ3) + 20400*pow3(log(mQ32/mU32))*pow8(mU3)*power10(mQ3) -
        90*(mQ32 - mU32)*mU32*dilog(1 - mQ32/msq2)*(-2*mQ32 + 2*mU32 + (mQ32 +
        mU32)*log(mQ32/mU32))*(29*pow12(mQ3) + pow6(mQ3)*(-109*mU32*pow4(msq) -
        33*msq2*pow4(mU3) + 47*pow6(msq) - 13*pow6(mU3)) + pow6(msq)*pow6(mU3)
        + pow4(mQ3)*(-3*pow4(msq)*pow4(mU3) + 19*mU32*pow6(msq) + 11*msq2*pow6(
        mU3)) + mQ32*(-3*pow4(mU3)*pow6(msq) + pow4(msq)*pow6(mU3)) + (209*
        msq2*mU32 - 81*pow4(msq) + 39*pow4(mU3))*pow8(mQ3) + (5*msq2 - 119*
        mU32)*power10(mQ3)) - 2880*pow4(mQ3)*pow4(msq)*power10(mU3) + 2880*log(
        mQ32/msq2)*pow4(mQ3)*pow4(msq)*power10(mU3) - 4320*log(mQ32/mU32)*pow4(
        mQ3)*pow4(msq)*power10(mU3) + 1440*log(mQ32/msq2)*log(mQ32/mU32)*pow4(
        mQ3)*pow4(msq)*power10(mU3) - 450*pow2(log(mQ32/msq2))*pow4(mQ3)*pow4(
        msq)*power10(mU3) - 135*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow4(mQ3)*
        pow4(msq)*power10(mU3) - 1440*pow2(log(mQ32/mU32))*pow4(mQ3)*pow4(msq)*
        power10(mU3) + 257936*msq2*pow6(mQ3)*power10(mU3) + 249824*lmQ3MR*msq2*
        pow6(mQ3)*power10(mU3) + 20640*msq2*dilog(1 - mQ32/mU32)*pow6(mQ3)*
        power10(mU3) - 26880*lmQ3MR*msq2*dilog(1 - mQ32/mU32)*pow6(mQ3)*
        power10(mU3) + 14400*msq2*dilog(1 - msq2/mU32)*pow6(mQ3)*power10(mU3) -
        91680*msq2*log(mQ32/msq2)*pow6(mQ3)*power10(mU3) + 3840*lmQ3MR*msq2*
        log(mQ32/msq2)*pow6(mQ3)*power10(mU3) + 4800*msq2*dilog(1 - mQ32/mU32)*
        log(mQ32/msq2)*pow6(mQ3)*power10(mU3) + 152712*msq2*log(mQ32/mU32)*
        pow6(mQ3)*power10(mU3) + 151824*lmQ3MR*msq2*log(mQ32/mU32)*pow6(mQ3)*
        power10(mU3) + 36528*msq2*dilog(1 - mQ32/mU32)*log(mQ32/mU32)*pow6(mQ3)
        *power10(mU3) + 4320*msq2*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow6(mQ3)
        *power10(mU3) - 45360*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow6(mQ3)*
        power10(mU3) - 1920*lmQ3MR*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow6(mQ3)
        *power10(mU3) - 141504*msq2*pow2(lmQ3MR)*pow6(mQ3)*power10(mU3) -
        25248*msq2*log(mQ32/mU32)*pow2(lmQ3MR)*pow6(mQ3)*power10(mU3) + 9450*
        msq2*pow2(log(mQ32/msq2))*pow6(mQ3)*power10(mU3) + 2835*msq2*log(mQ32/
        mU32)*pow2(log(mQ32/msq2))*pow6(mQ3)*power10(mU3) + 20904*msq2*pow2(
        log(mQ32/mU32))*pow6(mQ3)*power10(mU3) + 2016*lmQ3MR*msq2*pow2(log(
        mQ32/mU32))*pow6(mQ3)*power10(mU3) - 4320*msq2*log(mQ32/msq2)*pow2(log(
        mQ32/mU32))*pow6(mQ3)*power10(mU3) - 7080*msq2*pow3(log(mQ32/mU32))*
        pow6(mQ3)*power10(mU3) - 450*mQ32*pow2(log(mQ32/msq2))*pow6(msq)*
        power10(mU3) - 135*mQ32*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow6(msq)*
        power10(mU3) - 255056*pow8(mQ3)*power10(mU3) - 249824*lmQ3MR*pow8(mQ3)*
        power10(mU3) - 20640*dilog(1 - mQ32/mU32)*pow8(mQ3)*power10(mU3) +
        26880*lmQ3MR*dilog(1 - mQ32/mU32)*pow8(mQ3)*power10(mU3) - 14400*dilog(
        1 - msq2/mU32)*pow8(mQ3)*power10(mU3) + 79200*log(mQ32/msq2)*pow8(mQ3)*
        power10(mU3) - 3840*lmQ3MR*log(mQ32/msq2)*pow8(mQ3)*power10(mU3) -
        4800*dilog(1 - mQ32/mU32)*log(mQ32/msq2)*pow8(mQ3)*power10(mU3) -
        148392*log(mQ32/mU32)*pow8(mQ3)*power10(mU3) - 151824*lmQ3MR*log(mQ32/
        mU32)*pow8(mQ3)*power10(mU3) - 36528*dilog(1 - mQ32/mU32)*log(mQ32/
        mU32)*pow8(mQ3)*power10(mU3) - 4320*dilog(1 - msq2/mU32)*log(mQ32/mU32)
        *pow8(mQ3)*power10(mU3) + 41040*log(mQ32/msq2)*log(mQ32/mU32)*pow8(mQ3)
        *power10(mU3) + 1920*lmQ3MR*log(mQ32/msq2)*log(mQ32/mU32)*pow8(mQ3)*
        power10(mU3) + 141504*pow2(lmQ3MR)*pow8(mQ3)*power10(mU3) + 25248*log(
        mQ32/mU32)*pow2(lmQ3MR)*pow8(mQ3)*power10(mU3) - 8550*pow2(log(mQ32/
        msq2))*pow8(mQ3)*power10(mU3) - 2565*log(mQ32/mU32)*pow2(log(mQ32/msq2)
        )*pow8(mQ3)*power10(mU3) - 19464*pow2(log(mQ32/mU32))*pow8(mQ3)*
        power10(mU3) - 2016*lmQ3MR*pow2(log(mQ32/mU32))*pow8(mQ3)*power10(mU3)
        + 4320*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow8(mQ3)*power10(mU3) +
        7080*pow3(log(mQ32/mU32))*pow8(mQ3)*power10(mU3)))/(18.*(mQ32 - msq2)*
        mU32*pow4(mQ3)*pow7(mQ32 - mU32))+
	(4*Xt5*(-3424*mU32*pow12(mQ3) + 2208*lmQ3MR*mU32*pow12(mQ3) - 192*mU32*
        dilog(1 - mQ32/mU32)*pow12(mQ3) - 160*mU32*log(mQ32/msq2)*pow12(mQ3) +
        3360*mU32*log(mQ32/mU32)*pow12(mQ3) - 2768*lmQ3MR*mU32*log(mQ32/mU32)*
        pow12(mQ3) + 224*mU32*dilog(1 - mQ32/mU32)*log(mQ32/mU32)*pow12(mQ3) +
        80*mU32*log(mQ32/msq2)*log(mQ32/mU32)*pow12(mQ3) - 256*pow14(mQ3) +
        256*lmQ3MR*pow14(mQ3) + 30*mU32*pow12(mQ3)*pow2(log(mQ32/msq2)) - 15*
        mU32*log(mQ32/mU32)*pow12(mQ3)*pow2(log(mQ32/msq2)) - 216*mU32*pow12(
        mQ3)*pow2(log(mQ32/mU32)) + 256*lmQ3MR*mU32*pow12(mQ3)*pow2(log(mQ32/
        mU32)) - 30*mU32*dilog(1 - msq2/mQ32)*(-2*mQ32 + 2*mU32 + (mQ32 + mU32)
        *log(mQ32/mU32))*pow2(mQ32 - msq2)*pow3(mQ32 - mU32) + 112*mU32*pow12(
        mQ3)*pow3(log(mQ32/mU32)) - 1920*dilog(1 - msq2/mU32)*pow4(msq)*pow4(
        mU3)*pow6(mQ3) + 1920*log(mQ32/msq2)*log(mQ32/mU32)*pow4(msq)*pow4(mU3)
        *pow6(mQ3) - 1080*pow2(log(mQ32/msq2))*pow4(msq)*pow4(mU3)*pow6(mQ3) +
        30*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow4(msq)*pow4(mU3)*pow6(mQ3) -
        960*pow2(log(mQ32/mU32))*pow4(msq)*pow4(mU3)*pow6(mQ3) + 960*dilog(1 -
        msq2/mU32)*pow4(mQ3)*pow4(msq)*pow6(mU3) + 480*dilog(1 - msq2/mU32)*
        log(mQ32/mU32)*pow4(mQ3)*pow4(msq)*pow6(mU3) - 960*log(mQ32/msq2)*log(
        mQ32/mU32)*pow4(mQ3)*pow4(msq)*pow6(mU3) + 660*pow2(log(mQ32/msq2))*
        pow4(mQ3)*pow4(msq)*pow6(mU3) + 240*log(mQ32/mU32)*pow2(log(mQ32/msq2))
        *pow4(mQ3)*pow4(msq)*pow6(mU3) + 480*pow2(log(mQ32/mU32))*pow4(mQ3)*
        pow4(msq)*pow6(mU3) - 480*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(mQ3)
        *pow4(msq)*pow6(mU3) + 240*pow3(log(mQ32/mU32))*pow4(mQ3)*pow4(msq)*
        pow6(mU3) + 2880*msq2*pow6(mQ3)*pow6(mU3) + 3840*msq2*dilog(1 - msq2/
        mU32)*pow6(mQ3)*pow6(mU3) - 2880*msq2*log(mQ32/msq2)*pow6(mQ3)*pow6(
        mU3) + 2400*msq2*log(mQ32/mU32)*pow6(mQ3)*pow6(mU3) - 4320*msq2*log(
        mQ32/msq2)*log(mQ32/mU32)*pow6(mQ3)*pow6(mU3) + 1560*msq2*pow2(log(
        mQ32/msq2))*pow6(mQ3)*pow6(mU3) + 1920*msq2*pow2(log(mQ32/mU32))*pow6(
        mQ3)*pow6(mU3) + 960*mU32*dilog(1 - msq2/mU32)*pow4(msq)*pow8(mQ3) -
        480*mU32*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow4(msq)*pow8(mQ3) - 960*
        mU32*log(mQ32/msq2)*log(mQ32/mU32)*pow4(msq)*pow8(mQ3) + 510*mU32*pow2(
        log(mQ32/msq2))*pow4(msq)*pow8(mQ3) - 255*mU32*log(mQ32/mU32)*pow2(log(
        mQ32/msq2))*pow4(msq)*pow8(mQ3) + 480*mU32*pow2(log(mQ32/mU32))*pow4(
        msq)*pow8(mQ3) + 480*mU32*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(msq)
        *pow8(mQ3) - 240*mU32*pow3(log(mQ32/mU32))*pow4(msq)*pow8(mQ3) - 2880*
        msq2*pow4(mU3)*pow8(mQ3) - 1920*msq2*dilog(1 - msq2/mU32)*pow4(mU3)*
        pow8(mQ3) + 2880*msq2*log(mQ32/msq2)*pow4(mU3)*pow8(mQ3) - 480*msq2*
        log(mQ32/mU32)*pow4(mU3)*pow8(mQ3) + 960*msq2*dilog(1 - msq2/mU32)*log(
        mQ32/mU32)*pow4(mU3)*pow8(mQ3) + 1440*msq2*log(mQ32/msq2)*log(mQ32/
        mU32)*pow4(mU3)*pow8(mQ3) - 720*msq2*pow2(log(mQ32/msq2))*pow4(mU3)*
        pow8(mQ3) + 420*msq2*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow4(mU3)*
        pow8(mQ3) - 480*msq2*pow2(log(mQ32/mU32))*pow4(mU3)*pow8(mQ3) - 960*
        msq2*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(mU3)*pow8(mQ3) + 480*
        msq2*pow3(log(mQ32/mU32))*pow4(mU3)*pow8(mQ3) - 24128*pow6(mU3)*pow8(
        mQ3) + 18560*lmQ3MR*pow6(mU3)*pow8(mQ3) - 1152*dilog(1 - mQ32/mU32)*
        pow6(mU3)*pow8(mQ3) + 960*dilog(1 - msq2/mU32)*pow6(mU3)*pow8(mQ3) -
        960*log(mQ32/msq2)*pow6(mU3)*pow8(mQ3) - 12160*log(mQ32/mU32)*pow6(mU3)
        *pow8(mQ3) + 4256*lmQ3MR*log(mQ32/mU32)*pow6(mU3)*pow8(mQ3) + 768*
        dilog(1 - mQ32/mU32)*log(mQ32/mU32)*pow6(mU3)*pow8(mQ3) - 480*dilog(1 -
        msq2/mU32)*log(mQ32/mU32)*pow6(mU3)*pow8(mQ3) - 480*log(mQ32/msq2)*log(
        mQ32/mU32)*pow6(mU3)*pow8(mQ3) + 660*pow2(log(mQ32/msq2))*pow6(mU3)*
        pow8(mQ3) - 240*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow6(mU3)*pow8(mQ3)
        + 704*pow2(log(mQ32/mU32))*pow6(mU3)*pow8(mQ3) - 2528*lmQ3MR*pow2(log(
        mQ32/mU32))*pow6(mU3)*pow8(mQ3) + 80*log(mQ32/msq2)*pow2(log(mQ32/mU32)
        )*pow6(mU3)*pow8(mQ3) + 1352*pow3(log(mQ32/mU32))*pow6(mU3)*pow8(mQ3) -
        30*(mQ32 - mU32)*mU32*dilog(1 - mQ32/msq2)*(-2*mQ32 + 2*mU32 + (mQ32 +
        mU32)*log(mQ32/mU32))*(-2*mQ32*msq2*mU32*(msq2 + mU32) + pow4(msq)*
        pow4(mU3) + pow4(mQ3)*(-28*msq2*mU32 + 17*pow4(msq) + 17*pow4(mU3)) -
        2*(msq2 + mU32)*pow6(mQ3) + pow8(mQ3)) - 960*msq2*pow4(mQ3)*pow8(mU3) -
        1920*msq2*dilog(1 - msq2/mU32)*pow4(mQ3)*pow8(mU3) + 960*msq2*log(mQ32/
        msq2)*pow4(mQ3)*pow8(mU3) - 1440*msq2*log(mQ32/mU32)*pow4(mQ3)*pow8(
        mU3) - 960*msq2*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow4(mQ3)*pow8(mU3)
        + 2400*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow4(mQ3)*pow8(mU3) - 720*
        msq2*pow2(log(mQ32/msq2))*pow4(mQ3)*pow8(mU3) - 420*msq2*log(mQ32/mU32)
        *pow2(log(mQ32/msq2))*pow4(mQ3)*pow8(mU3) - 1440*msq2*pow2(log(mQ32/
        mU32))*pow4(mQ3)*pow8(mU3) + 960*msq2*log(mQ32/msq2)*pow2(log(mQ32/
        mU32))*pow4(mQ3)*pow8(mU3) - 480*msq2*pow3(log(mQ32/mU32))*pow4(mQ3)*
        pow8(mU3) - 120*mQ32*pow2(log(mQ32/msq2))*pow4(msq)*pow8(mU3) - 30*
        mQ32*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow4(msq)*pow8(mU3) + 16000*
        pow6(mQ3)*pow8(mU3) - 12864*lmQ3MR*pow6(mQ3)*pow8(mU3) + 768*dilog(1 -
        mQ32/mU32)*pow6(mQ3)*pow8(mU3) - 1920*dilog(1 - msq2/mU32)*pow6(mQ3)*
        pow8(mU3) + 640*log(mQ32/msq2)*pow6(mQ3)*pow8(mU3) + 17600*log(mQ32/
        mU32)*pow6(mQ3)*pow8(mU3) - 9920*lmQ3MR*log(mQ32/mU32)*pow6(mQ3)*pow8(
        mU3) - 320*dilog(1 - mQ32/mU32)*log(mQ32/mU32)*pow6(mQ3)*pow8(mU3) +
        640*log(mQ32/msq2)*log(mQ32/mU32)*pow6(mQ3)*pow8(mU3) - 1080*pow2(log(
        mQ32/msq2))*pow6(mQ3)*pow8(mU3) - 30*log(mQ32/mU32)*pow2(log(mQ32/msq2)
        )*pow6(mQ3)*pow8(mU3) + 5872*pow2(log(mQ32/mU32))*pow6(mQ3)*pow8(mU3) -
        448*lmQ3MR*pow2(log(mQ32/mU32))*pow6(mQ3)*pow8(mU3) + 80*log(mQ32/msq2)
        *pow2(log(mQ32/mU32))*pow6(mQ3)*pow8(mU3) + 232*pow3(log(mQ32/mU32))*
        pow6(mQ3)*pow8(mU3) + 960*msq2*mU32*power10(mQ3) - 960*msq2*mU32*log(
        mQ32/msq2)*power10(mQ3) - 480*msq2*mU32*log(mQ32/mU32)*power10(mQ3) +
        480*msq2*mU32*log(mQ32/msq2)*log(mQ32/mU32)*power10(mQ3) - 60*msq2*
        mU32*pow2(log(mQ32/msq2))*power10(mQ3) + 30*msq2*mU32*log(mQ32/mU32)*
        pow2(log(mQ32/msq2))*power10(mQ3) + 15744*pow4(mU3)*power10(mQ3) -
        11456*lmQ3MR*pow4(mU3)*power10(mQ3) + 768*dilog(1 - mQ32/mU32)*pow4(
        mU3)*power10(mQ3) + 640*log(mQ32/msq2)*pow4(mU3)*power10(mQ3) - 2240*
        log(mQ32/mU32)*pow4(mU3)*power10(mQ3) + 4160*lmQ3MR*log(mQ32/mU32)*
        pow4(mU3)*power10(mQ3) - 704*dilog(1 - mQ32/mU32)*log(mQ32/mU32)*pow4(
        mU3)*power10(mQ3) - 120*pow2(log(mQ32/msq2))*pow4(mU3)*power10(mQ3) +
        30*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow4(mU3)*power10(mQ3) - 2480*
        pow2(log(mQ32/mU32))*pow4(mU3)*power10(mQ3) + 1472*lmQ3MR*pow2(log(
        mQ32/mU32))*pow4(mU3)*power10(mQ3) - 80*log(mQ32/msq2)*pow2(log(mQ32/
        mU32))*pow4(mU3)*power10(mQ3) - 616*pow3(log(mQ32/mU32))*pow4(mU3)*
        power10(mQ3) - 60*mQ32*msq2*pow2(log(mQ32/msq2))*power10(mU3) - 30*
        mQ32*msq2*log(mQ32/mU32)*pow2(log(mQ32/msq2))*power10(mU3) - 3936*pow4(
        mQ3)*power10(mU3) + 3296*lmQ3MR*pow4(mQ3)*power10(mU3) - 192*dilog(1 -
        mQ32/mU32)*pow4(mQ3)*power10(mU3) + 960*dilog(1 - msq2/mU32)*pow4(mQ3)*
        power10(mU3) - 160*log(mQ32/msq2)*pow4(mQ3)*power10(mU3) - 6560*log(
        mQ32/mU32)*pow4(mQ3)*power10(mU3) + 4272*lmQ3MR*log(mQ32/mU32)*pow4(
        mQ3)*power10(mU3) + 32*dilog(1 - mQ32/mU32)*log(mQ32/mU32)*pow4(mQ3)*
        power10(mU3) + 480*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow4(mQ3)*
        power10(mU3) - 240*log(mQ32/msq2)*log(mQ32/mU32)*pow4(mQ3)*power10(mU3)
        + 510*pow2(log(mQ32/msq2))*pow4(mQ3)*power10(mU3) + 255*log(mQ32/mU32)*
        pow2(log(mQ32/msq2))*pow4(mQ3)*power10(mU3) - 3880*pow2(log(mQ32/mU32))
        *pow4(mQ3)*power10(mU3) + 1248*lmQ3MR*pow2(log(mQ32/mU32))*pow4(mQ3)*
        power10(mU3) - 80*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(mQ3)*
        power10(mU3) - 824*pow3(log(mQ32/mU32))*pow4(mQ3)*power10(mU3) + 30*
        pow2(log(mQ32/msq2))*pow4(msq)*power10(mU3) + 15*log(mQ32/mU32)*pow2(
        log(mQ32/msq2))*pow4(msq)*power10(mU3)))/(3.*mU32*pow3(mQ3)*pow7(mQ32 -
        mU32))+
	(1280*mQ32*Xt6*(-2*mQ32 + 2*mU32 + (mQ32 + mU32)*log(mQ32/mU32))*pow2(-
        mQ32 + mU32 + mU32*log(mQ32/mU32)))/(3.*pow7(mQ32 - mU32));
      }
      case(Limits::MU3_EQ_M3):{
	 return (Xt4*(84*msq2*mU32*dilog(1 - mU32/mQ32)*pow14(mQ3) - 78*msq2*mU32*dilog(1
        - mU32/mQ32)*log(mQ32/mU32)*pow14(mQ3) + 244640*mQ32*msq2*pow14(mU3) -
        13120*lmQ3MR*mQ32*msq2*pow14(mU3) + 1800*mQ32*msq2*dilog(1 - msq2/mU32)
        *pow14(mU3) + 80556*mQ32*msq2*dilog(1 - mU32/mQ32)*pow14(mU3) - 10752*
        lmQ3MR*mQ32*msq2*dilog(1 - mU32/mQ32)*pow14(mU3) + 1800*mQ32*msq2*
        dilog(1 - mU32/msq2)*pow14(mU3) - 10560*mQ32*msq2*log(mQ32/msq2)*pow14(
        mU3) - 7680*lmQ3MR*mQ32*msq2*log(mQ32/msq2)*pow14(mU3) + 1920*mQ32*
        msq2*dilog(1 - mU32/mQ32)*log(mQ32/msq2)*pow14(mU3) + 114224*mQ32*msq2*
        log(mQ32/mU32)*pow14(mU3) + 70240*lmQ3MR*mQ32*msq2*log(mQ32/mU32)*
        pow14(mU3) + 900*mQ32*msq2*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow14(
        mU3) + 24534*mQ32*msq2*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow14(mU3) +
        900*mQ32*msq2*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow14(mU3) - 7080*
        mQ32*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow14(mU3) + 3840*lmQ3MR*mQ32*
        msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow14(mU3) - 384*mQ32*msq2*log(mU32/
        mQ32)*pow14(mU3) - 192*mQ32*msq2*log(mQ32/mU32)*log(mU32/mQ32)*pow14(
        mU3) + 36*msq2*dilog(1 - mU32/mQ32)*pow16(mQ3) - 36*mU32*dilog(1 -
        mU32/mQ32)*pow16(mQ3) - 18*msq2*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*
        pow16(mQ3) + 18*mU32*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow16(mQ3) -
        261920*mQ32*pow16(mU3) + 13120*lmQ3MR*mQ32*pow16(mU3) + 384*msq2*pow16(
        mU3) + 10368*lmQ3MR*msq2*pow16(mU3) - 1080*mQ32*dilog(1 - msq2/mU32)*
        pow16(mU3) - 80556*mQ32*dilog(1 - mU32/mQ32)*pow16(mU3) + 10752*lmQ3MR*
        mQ32*dilog(1 - mU32/mQ32)*pow16(mU3) + 10440*mQ32*dilog(1 - mU32/msq2)*
        pow16(mU3) + 27840*mQ32*log(mQ32/msq2)*pow16(mU3) + 7680*lmQ3MR*mQ32*
        log(mQ32/msq2)*pow16(mU3) - 1920*msq2*log(mQ32/msq2)*pow16(mU3) + 1920*
        lmQ3MR*msq2*log(mQ32/msq2)*pow16(mU3) - 1920*mQ32*dilog(1 - mU32/mQ32)*
        log(mQ32/msq2)*pow16(mU3) - 140144*mQ32*log(mQ32/mU32)*pow16(mU3) -
        70240*lmQ3MR*mQ32*log(mQ32/mU32)*pow16(mU3) - 8256*msq2*log(mQ32/mU32)*
        pow16(mU3) + 19392*lmQ3MR*msq2*log(mQ32/mU32)*pow16(mU3) - 540*mQ32*
        dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow16(mU3) - 24534*mQ32*dilog(1 -
        mU32/mQ32)*log(mQ32/mU32)*pow16(mU3) + 5220*mQ32*dilog(1 - mU32/msq2)*
        log(mQ32/mU32)*pow16(mU3) + 15000*mQ32*log(mQ32/msq2)*log(mQ32/mU32)*
        pow16(mU3) - 3840*lmQ3MR*mQ32*log(mQ32/msq2)*log(mQ32/mU32)*pow16(mU3)
        - 1920*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow16(mU3) + 384*mQ32*log(
        mU32/mQ32)*pow16(mU3) + 192*mQ32*log(mQ32/mU32)*log(mU32/mQ32)*pow16(
        mU3) - 3840*mQ32*log(mU32/msq2)*pow16(mU3) - 1920*mQ32*log(mQ32/mU32)*
        log(mU32/msq2)*pow16(mU3) - 384*pow18(mU3) - 10368*lmQ3MR*pow18(mU3) +
        1920*log(mQ32/msq2)*pow18(mU3) - 1920*lmQ3MR*log(mQ32/msq2)*pow18(mU3)
        + 8256*log(mQ32/mU32)*pow18(mU3) - 19392*lmQ3MR*log(mQ32/mU32)*pow18(
        mU3) + 1920*log(mQ32/msq2)*log(mQ32/mU32)*pow18(mU3) - 4992*mQ32*msq2*
        pow14(mU3)*pow2(lmQ3MR) - 45504*mQ32*msq2*log(mQ32/mU32)*pow14(mU3)*
        pow2(lmQ3MR) + 4992*mQ32*pow16(mU3)*pow2(lmQ3MR) - 10752*msq2*pow16(
        mU3)*pow2(lmQ3MR) + 45504*mQ32*log(mQ32/mU32)*pow16(mU3)*pow2(lmQ3MR) +
        10752*pow18(mU3)*pow2(lmQ3MR) + 6660*mQ32*msq2*pow14(mU3)*pow2(log(
        mQ32/msq2)) + 3330*mQ32*msq2*log(mQ32/mU32)*pow14(mU3)*pow2(log(mQ32/
        msq2)) - 540*mQ32*pow16(mU3)*pow2(log(mQ32/msq2)) - 270*mQ32*log(mQ32/
        mU32)*pow16(mU3)*pow2(log(mQ32/msq2)) + 42*msq2*mU32*pow14(mQ3)*pow2(
        log(mQ32/mU32)) - 15094*mQ32*msq2*pow14(mU3)*pow2(log(mQ32/mU32)) +
        82176*lmQ3MR*mQ32*msq2*pow14(mU3)*pow2(log(mQ32/mU32)) - 2820*mQ32*
        msq2*log(mQ32/msq2)*pow14(mU3)*pow2(log(mQ32/mU32)) + 18*msq2*pow16(
        mQ3)*pow2(log(mQ32/mU32)) - 18*mU32*pow16(mQ3)*pow2(log(mQ32/mU32)) +
        6814*mQ32*pow16(mU3)*pow2(log(mQ32/mU32)) - 82176*lmQ3MR*mQ32*pow16(
        mU3)*pow2(log(mQ32/mU32)) - 8640*msq2*pow16(mU3)*pow2(log(mQ32/mU32)) +
        2460*mQ32*log(mQ32/msq2)*pow16(mU3)*pow2(log(mQ32/mU32)) + 8640*pow18(
        mU3)*pow2(log(mQ32/mU32)) - 39*msq2*mU32*pow14(mQ3)*pow3(log(mQ32/mU32)
        ) - 30099*mQ32*msq2*pow14(mU3)*pow3(log(mQ32/mU32)) - 9*msq2*pow16(mQ3)
        *pow3(log(mQ32/mU32)) + 9*mU32*pow16(mQ3)*pow3(log(mQ32/mU32)) + 30279*
        mQ32*pow16(mU3)*pow3(log(mQ32/mU32)) - 1072288*msq2*pow12(mU3)*pow4(
        mQ3) - 170176*lmQ3MR*msq2*pow12(mU3)*pow4(mQ3) - 9000*msq2*dilog(1 -
        msq2/mU32)*pow12(mU3)*pow4(mQ3) - 305508*msq2*dilog(1 - mU32/mQ32)*
        pow12(mU3)*pow4(mQ3) + 53760*lmQ3MR*msq2*dilog(1 - mU32/mQ32)*pow12(
        mU3)*pow4(mQ3) + 71640*msq2*dilog(1 - mU32/msq2)*pow12(mU3)*pow4(mQ3) +
        120000*msq2*log(mQ32/msq2)*pow12(mU3)*pow4(mQ3) + 9600*lmQ3MR*msq2*log(
        mQ32/msq2)*pow12(mU3)*pow4(mQ3) - 9600*msq2*dilog(1 - mU32/mQ32)*log(
        mQ32/msq2)*pow12(mU3)*pow4(mQ3) + 24880*msq2*log(mQ32/mU32)*pow12(mU3)*
        pow4(mQ3) - 621600*lmQ3MR*msq2*log(mQ32/mU32)*pow12(mU3)*pow4(mQ3) -
        2700*msq2*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow12(mU3)*pow4(mQ3) -
        13062*msq2*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow12(mU3)*pow4(mQ3) +
        37620*msq2*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow12(mU3)*pow4(mQ3) +
        38280*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow12(mU3)*pow4(mQ3) - 15360*
        lmQ3MR*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow12(mU3)*pow4(mQ3) + 768*
        msq2*log(mU32/mQ32)*pow12(mU3)*pow4(mQ3) + 1118368*pow14(mU3)*pow4(mQ3)
        + 170176*lmQ3MR*pow14(mU3)*pow4(mQ3) + 5400*dilog(1 - msq2/mU32)*pow14(
        mU3)*pow4(mQ3) + 305508*dilog(1 - mU32/mQ32)*pow14(mU3)*pow4(mQ3) -
        53760*lmQ3MR*dilog(1 - mU32/mQ32)*pow14(mU3)*pow4(mQ3) - 63720*dilog(1
        - mU32/msq2)*pow14(mU3)*pow4(mQ3) - 166080*log(mQ32/msq2)*pow14(mU3)*
        pow4(mQ3) - 9600*lmQ3MR*log(mQ32/msq2)*pow14(mU3)*pow4(mQ3) + 9600*
        dilog(1 - mU32/mQ32)*log(mQ32/msq2)*pow14(mU3)*pow4(mQ3) + 9680*log(
        mQ32/mU32)*pow14(mU3)*pow4(mQ3) + 621600*lmQ3MR*log(mQ32/mU32)*pow14(
        mU3)*pow4(mQ3) + 1620*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow14(mU3)*
        pow4(mQ3) + 13062*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow14(mU3)*pow4(
        mQ3) - 21420*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow14(mU3)*pow4(mQ3) -
        40440*log(mQ32/msq2)*log(mQ32/mU32)*pow14(mU3)*pow4(mQ3) + 15360*
        lmQ3MR*log(mQ32/msq2)*log(mQ32/mU32)*pow14(mU3)*pow4(mQ3) - 768*log(
        mU32/mQ32)*pow14(mU3)*pow4(mQ3) + 19200*log(mU32/msq2)*pow14(mU3)*pow4(
        mQ3) + 5760*log(mQ32/mU32)*log(mU32/msq2)*pow14(mU3)*pow4(mQ3) +
        186240*msq2*pow12(mU3)*pow2(lmQ3MR)*pow4(mQ3) + 158016*msq2*log(mQ32/
        mU32)*pow12(mU3)*pow2(lmQ3MR)*pow4(mQ3) - 186240*pow14(mU3)*pow2(
        lmQ3MR)*pow4(mQ3) - 158016*log(mQ32/mU32)*pow14(mU3)*pow2(lmQ3MR)*pow4(
        mQ3) + 7020*msq2*pow12(mU3)*pow2(log(mQ32/msq2))*pow4(mQ3) + 10170*
        msq2*log(mQ32/mU32)*pow12(mU3)*pow2(log(mQ32/msq2))*pow4(mQ3) - 3060*
        pow14(mU3)*pow2(log(mQ32/msq2))*pow4(mQ3) - 2070*log(mQ32/mU32)*pow14(
        mU3)*pow2(log(mQ32/msq2))*pow4(mQ3) + 327498*msq2*pow12(mU3)*pow2(log(
        mQ32/mU32))*pow4(mQ3) - 264192*lmQ3MR*msq2*pow12(mU3)*pow2(log(mQ32/
        mU32))*pow4(mQ3) - 1140*msq2*log(mQ32/msq2)*pow12(mU3)*pow2(log(mQ32/
        mU32))*pow4(mQ3) - 332178*pow14(mU3)*pow2(log(mQ32/mU32))*pow4(mQ3) +
        264192*lmQ3MR*pow14(mU3)*pow2(log(mQ32/mU32))*pow4(mQ3) + 2220*log(
        mQ32/msq2)*pow14(mU3)*pow2(log(mQ32/mU32))*pow4(mQ3) + 145527*msq2*
        pow12(mU3)*pow3(log(mQ32/mU32))*pow4(mQ3) - 146067*pow14(mU3)*pow3(log(
        mQ32/mU32))*pow4(mQ3) + 360*mU32*dilog(1 - msq2/mU32)*pow12(mQ3)*pow4(
        msq) + 360*mU32*dilog(1 - mU32/msq2)*pow12(mQ3)*pow4(msq) - 180*mU32*
        dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow12(mQ3)*pow4(msq) - 180*mU32*
        dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow12(mQ3)*pow4(msq) - 360*mU32*
        log(mQ32/msq2)*log(mQ32/mU32)*pow12(mQ3)*pow4(msq) + 17280*mQ32*pow12(
        mU3)*pow4(msq) - 360*mQ32*dilog(1 - msq2/mU32)*pow12(mU3)*pow4(msq) -
        29160*mQ32*dilog(1 - mU32/msq2)*pow12(mU3)*pow4(msq) - 17280*mQ32*log(
        mQ32/msq2)*pow12(mU3)*pow4(msq) + 25920*mQ32*log(mQ32/mU32)*pow12(mU3)*
        pow4(msq) - 180*mQ32*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow12(mU3)*
        pow4(msq) - 14580*mQ32*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow12(mU3)*
        pow4(msq) - 8280*mQ32*log(mQ32/msq2)*log(mQ32/mU32)*pow12(mU3)*pow4(
        msq) + 180*mU32*pow12(mQ3)*pow2(log(mQ32/msq2))*pow4(msq) - 90*mU32*
        log(mQ32/mU32)*pow12(mQ3)*pow2(log(mQ32/msq2))*pow4(msq) - 14580*mQ32*
        pow12(mU3)*pow2(log(mQ32/msq2))*pow4(msq) - 7290*mQ32*log(mQ32/mU32)*
        pow12(mU3)*pow2(log(mQ32/msq2))*pow4(msq) + 180*mU32*pow12(mQ3)*pow2(
        log(mQ32/mU32))*pow4(msq) + 180*mU32*log(mQ32/msq2)*pow12(mQ3)*pow2(
        log(mQ32/mU32))*pow4(msq) + 8460*mQ32*pow12(mU3)*pow2(log(mQ32/mU32))*
        pow4(msq) + 180*mQ32*log(mQ32/msq2)*pow12(mU3)*pow2(log(mQ32/mU32))*
        pow4(msq) - 90*mU32*pow12(mQ3)*pow3(log(mQ32/mU32))*pow4(msq) - 90*
        mQ32*pow12(mU3)*pow3(log(mQ32/mU32))*pow4(msq) - 59936*msq2*pow12(mQ3)*
        pow4(mU3) - 117440*lmQ3MR*msq2*pow12(mQ3)*pow4(mU3) - 1800*msq2*dilog(1
        - msq2/mU32)*pow12(mQ3)*pow4(mU3) + 6804*msq2*dilog(1 - mU32/mQ32)*
        pow12(mQ3)*pow4(mU3) + 10752*lmQ3MR*msq2*dilog(1 - mU32/mQ32)*pow12(
        mQ3)*pow4(mU3) + 3960*msq2*dilog(1 - mU32/msq2)*pow12(mQ3)*pow4(mU3) +
        35520*msq2*log(mQ32/msq2)*pow12(mQ3)*pow4(mU3) - 1920*lmQ3MR*msq2*log(
        mQ32/msq2)*pow12(mQ3)*pow4(mU3) - 1920*msq2*dilog(1 - mU32/mQ32)*log(
        mQ32/msq2)*pow12(mQ3)*pow4(mU3) + 80112*msq2*log(mQ32/mU32)*pow12(mQ3)*
        pow4(mU3) + 12256*lmQ3MR*msq2*log(mQ32/mU32)*pow12(mQ3)*pow4(mU3) +
        900*msq2*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow12(mQ3)*pow4(mU3) -
        7362*msq2*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow12(mQ3)*pow4(mU3) -
        1980*msq2*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow12(mQ3)*pow4(mU3) -
        16920*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow12(mQ3)*pow4(mU3) - 768*
        msq2*log(mU32/mQ32)*pow12(mQ3)*pow4(mU3) + 384*msq2*log(mQ32/mU32)*log(
        mU32/mQ32)*pow12(mQ3)*pow4(mU3) - 84*dilog(1 - mU32/mQ32)*pow14(mQ3)*
        pow4(mU3) + 78*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow14(mQ3)*pow4(mU3)
        + 58752*msq2*pow12(mQ3)*pow2(lmQ3MR)*pow4(mU3) - 24000*msq2*log(mQ32/
        mU32)*pow12(mQ3)*pow2(lmQ3MR)*pow4(mU3) - 3780*msq2*pow12(mQ3)*pow2(
        log(mQ32/msq2))*pow4(mU3) + 1890*msq2*log(mQ32/mU32)*pow12(mQ3)*pow2(
        log(mQ32/msq2))*pow4(mU3) - 19306*msq2*pow12(mQ3)*pow2(log(mQ32/mU32))*
        pow4(mU3) + 13248*lmQ3MR*msq2*pow12(mQ3)*pow2(log(mQ32/mU32))*pow4(mU3)
        + 1020*msq2*log(mQ32/msq2)*pow12(mQ3)*pow2(log(mQ32/mU32))*pow4(mU3) -
        42*pow14(mQ3)*pow2(log(mQ32/mU32))*pow4(mU3) - 63*msq2*pow12(mQ3)*pow3(
        log(mQ32/mU32))*pow4(mU3) + 39*pow14(mQ3)*pow3(log(mQ32/mU32))*pow4(
        mU3) + 2880*mQ32*(mQ32 - msq2)*(mQ32 - mU32)*(msq2 - mU32)*dilog(1 -
        msq2/mQ32)*(-2*mQ32 + 2*mU32 + (mQ32 + mU32)*log(mQ32/mU32))*(mQ32*(
        msq2 - 3*mU32) + 3*msq2*mU32 + pow4(mQ3) - 2*pow4(mU3))*pow4(mU3) -
        1839680*pow12(mU3)*pow6(mQ3) - 603008*lmQ3MR*pow12(mU3)*pow6(mQ3) -
        10800*dilog(1 - msq2/mU32)*pow12(mU3)*pow6(mQ3) - 425916*dilog(1 -
        mU32/mQ32)*pow12(mU3)*pow6(mQ3) + 107520*lmQ3MR*dilog(1 - mU32/mQ32)*
        pow12(mU3)*pow6(mQ3) + 110160*dilog(1 - mU32/msq2)*pow12(mU3)*pow6(mQ3)
        + 347520*log(mQ32/msq2)*pow12(mU3)*pow6(mQ3) - 19200*dilog(1 - mU32/
        mQ32)*log(mQ32/msq2)*pow12(mU3)*pow6(mQ3) + 823968*log(mQ32/mU32)*
        pow12(mU3)*pow6(mQ3) - 1234112*lmQ3MR*log(mQ32/mU32)*pow12(mU3)*pow6(
        mQ3) - 1080*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow12(mU3)*pow6(mQ3) +
        91098*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow12(mU3)*pow6(mQ3) + 1800*
        dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow12(mU3)*pow6(mQ3) + 17520*log(
        mQ32/msq2)*log(mQ32/mU32)*pow12(mU3)*pow6(mQ3) - 23040*lmQ3MR*log(mQ32/
        msq2)*log(mQ32/mU32)*pow12(mU3)*pow6(mQ3) - 768*log(mU32/mQ32)*pow12(
        mU3)*pow6(mQ3) - 768*log(mQ32/mU32)*log(mU32/mQ32)*pow12(mU3)*pow6(mQ3)
        - 38400*log(mU32/msq2)*pow12(mU3)*pow6(mQ3) - 3840*log(mQ32/mU32)*log(
        mU32/msq2)*pow12(mU3)*pow6(mQ3) + 480000*pow12(mU3)*pow2(lmQ3MR)*pow6(
        mQ3) + 177024*log(mQ32/mU32)*pow12(mU3)*pow2(lmQ3MR)*pow6(mQ3) - 2520*
        pow12(mU3)*pow2(log(mQ32/msq2))*pow6(mQ3) - 4860*log(mQ32/mU32)*pow12(
        mU3)*pow2(log(mQ32/msq2))*pow6(mQ3) + 658922*pow12(mU3)*pow2(log(mQ32/
        mU32))*pow6(mQ3) - 206016*lmQ3MR*pow12(mU3)*pow2(log(mQ32/mU32))*pow6(
        mQ3) + 10680*log(mQ32/msq2)*pow12(mU3)*pow2(log(mQ32/mU32))*pow6(mQ3) +
        125745*pow12(mU3)*pow3(log(mQ32/mU32))*pow6(mQ3) + 360*dilog(1 - msq2/
        mU32)*pow12(mQ3)*pow6(msq) + 360*dilog(1 - mU32/msq2)*pow12(mQ3)*pow6(
        msq) - 180*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow12(mQ3)*pow6(msq) -
        180*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow12(mQ3)*pow6(msq) - 360*log(
        mQ32/msq2)*log(mQ32/mU32)*pow12(mQ3)*pow6(msq) + 180*pow12(mQ3)*pow2(
        log(mQ32/msq2))*pow6(msq) - 90*log(mQ32/mU32)*pow12(mQ3)*pow2(log(mQ32/
        msq2))*pow6(msq) + 180*pow12(mQ3)*pow2(log(mQ32/mU32))*pow6(msq) + 180*
        log(mQ32/msq2)*pow12(mQ3)*pow2(log(mQ32/mU32))*pow6(msq) - 90*pow12(
        mQ3)*pow3(log(mQ32/mU32))*pow6(msq) + 6*mQ32*(-msq2 + mU32)*dilog(1 -
        mQ32/mU32)*(-2*mQ32 + 2*mU32 + (mQ32 + mU32)*log(mQ32/mU32))*pow3(mQ32
        - mU32)*(19*mU32*pow4(mQ3) - 31*mQ32*pow4(mU3) + 3*pow6(mQ3) - 119*
        pow6(mU3)) + 59936*pow12(mQ3)*pow6(mU3) + 117440*lmQ3MR*pow12(mQ3)*
        pow6(mU3) + 1080*dilog(1 - msq2/mU32)*pow12(mQ3)*pow6(mU3) - 6804*
        dilog(1 - mU32/mQ32)*pow12(mQ3)*pow6(mU3) - 10752*lmQ3MR*dilog(1 -
        mU32/mQ32)*pow12(mQ3)*pow6(mU3) - 4680*dilog(1 - mU32/msq2)*pow12(mQ3)*
        pow6(mU3) - 35520*log(mQ32/msq2)*pow12(mQ3)*pow6(mU3) + 1920*lmQ3MR*
        log(mQ32/msq2)*pow12(mQ3)*pow6(mU3) + 1920*dilog(1 - mU32/mQ32)*log(
        mQ32/msq2)*pow12(mQ3)*pow6(mU3) - 80112*log(mQ32/mU32)*pow12(mQ3)*pow6(
        mU3) - 12256*lmQ3MR*log(mQ32/mU32)*pow12(mQ3)*pow6(mU3) - 540*dilog(1 -
        msq2/mU32)*log(mQ32/mU32)*pow12(mQ3)*pow6(mU3) + 7362*dilog(1 - mU32/
        mQ32)*log(mQ32/mU32)*pow12(mQ3)*pow6(mU3) + 2340*dilog(1 - mU32/msq2)*
        log(mQ32/mU32)*pow12(mQ3)*pow6(mU3) + 17640*log(mQ32/msq2)*log(mQ32/
        mU32)*pow12(mQ3)*pow6(mU3) + 768*log(mU32/mQ32)*pow12(mQ3)*pow6(mU3) -
        384*log(mQ32/mU32)*log(mU32/mQ32)*pow12(mQ3)*pow6(mU3) + 3840*log(mU32/
        msq2)*pow12(mQ3)*pow6(mU3) - 1920*log(mQ32/mU32)*log(mU32/msq2)*pow12(
        mQ3)*pow6(mU3) - 58752*pow12(mQ3)*pow2(lmQ3MR)*pow6(mU3) + 24000*log(
        mQ32/mU32)*pow12(mQ3)*pow2(lmQ3MR)*pow6(mU3) + 3420*pow12(mQ3)*pow2(
        log(mQ32/msq2))*pow6(mU3) - 1710*log(mQ32/mU32)*pow12(mQ3)*pow2(log(
        mQ32/msq2))*pow6(mU3) + 18946*pow12(mQ3)*pow2(log(mQ32/mU32))*pow6(mU3)
        - 13248*lmQ3MR*pow12(mQ3)*pow2(log(mQ32/mU32))*pow6(mU3) - 1380*log(
        mQ32/msq2)*pow12(mQ3)*pow2(log(mQ32/mU32))*pow6(mU3) + 243*pow12(mQ3)*
        pow3(log(mQ32/mU32))*pow6(mU3) - 3600*dilog(1 - msq2/mU32)*pow6(mQ3)*
        pow6(msq)*pow6(mU3) + 2160*dilog(1 - mU32/msq2)*pow6(mQ3)*pow6(msq)*
        pow6(mU3) - 360*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow6(mQ3)*pow6(msq)
        *pow6(mU3) - 9000*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow6(mQ3)*pow6(
        msq)*pow6(mU3) + 3600*log(mQ32/msq2)*log(mQ32/mU32)*pow6(mQ3)*pow6(msq)
        *pow6(mU3) + 1080*pow2(log(mQ32/msq2))*pow6(mQ3)*pow6(msq)*pow6(mU3) -
        4500*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow6(mQ3)*pow6(msq)*pow6(mU3)
        - 1800*pow2(log(mQ32/mU32))*pow6(mQ3)*pow6(msq)*pow6(mU3) + 360*log(
        mQ32/msq2)*pow2(log(mQ32/mU32))*pow6(mQ3)*pow6(msq)*pow6(mU3) - 180*
        pow3(log(mQ32/mU32))*pow6(mQ3)*pow6(msq)*pow6(mU3) + 3600*dilog(1 -
        msq2/mU32)*pow4(mU3)*pow6(msq)*pow8(mQ3) + 9360*dilog(1 - mU32/msq2)*
        pow4(mU3)*pow6(msq)*pow8(mQ3) - 360*dilog(1 - msq2/mU32)*log(mQ32/mU32)
        *pow4(mU3)*pow6(msq)*pow8(mQ3) - 3240*dilog(1 - mU32/msq2)*log(mQ32/
        mU32)*pow4(mU3)*pow6(msq)*pow8(mQ3) - 3600*log(mQ32/msq2)*log(mQ32/
        mU32)*pow4(mU3)*pow6(msq)*pow8(mQ3) + 4680*pow2(log(mQ32/msq2))*pow4(
        mU3)*pow6(msq)*pow8(mQ3) - 1620*log(mQ32/mU32)*pow2(log(mQ32/msq2))*
        pow4(mU3)*pow6(msq)*pow8(mQ3) + 1800*pow2(log(mQ32/mU32))*pow4(mU3)*
        pow6(msq)*pow8(mQ3) + 360*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(mU3)
        *pow6(msq)*pow8(mQ3) - 180*pow3(log(mQ32/mU32))*pow4(mU3)*pow6(msq)*
        pow8(mQ3) + 3600*dilog(1 - msq2/mU32)*pow4(msq)*pow6(mU3)*pow8(mQ3) -
        36720*dilog(1 - mU32/msq2)*pow4(msq)*pow6(mU3)*pow8(mQ3) + 11520*log(
        mQ32/mU32)*pow4(msq)*pow6(mU3)*pow8(mQ3) - 360*dilog(1 - msq2/mU32)*
        log(mQ32/mU32)*pow4(msq)*pow6(mU3)*pow8(mQ3) + 19800*dilog(1 - mU32/
        msq2)*log(mQ32/mU32)*pow4(msq)*pow6(mU3)*pow8(mQ3) - 9360*log(mQ32/
        msq2)*log(mQ32/mU32)*pow4(msq)*pow6(mU3)*pow8(mQ3) - 18360*pow2(log(
        mQ32/msq2))*pow4(msq)*pow6(mU3)*pow8(mQ3) + 9900*log(mQ32/mU32)*pow2(
        log(mQ32/msq2))*pow4(msq)*pow6(mU3)*pow8(mQ3) - 1080*pow2(log(mQ32/
        mU32))*pow4(msq)*pow6(mU3)*pow8(mQ3) + 360*log(mQ32/msq2)*pow2(log(
        mQ32/mU32))*pow4(msq)*pow6(mU3)*pow8(mQ3) - 180*pow3(log(mQ32/mU32))*
        pow4(msq)*pow6(mU3)*pow8(mQ3) + 34560*pow4(msq)*pow6(mQ3)*pow8(mU3) -
        3600*dilog(1 - msq2/mU32)*pow4(msq)*pow6(mQ3)*pow8(mU3) + 48240*dilog(1
        - mU32/msq2)*pow4(msq)*pow6(mQ3)*pow8(mU3) - 34560*log(mQ32/msq2)*pow4(
        msq)*pow6(mQ3)*pow8(mU3) - 5760*log(mQ32/mU32)*pow4(msq)*pow6(mQ3)*
        pow8(mU3) - 360*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow4(msq)*pow6(mQ3)
        *pow8(mU3) + 14040*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow4(msq)*pow6(
        mQ3)*pow8(mU3) + 15120*log(mQ32/msq2)*log(mQ32/mU32)*pow4(msq)*pow6(
        mQ3)*pow8(mU3) + 24120*pow2(log(mQ32/msq2))*pow4(msq)*pow6(mQ3)*pow8(
        mU3) + 7020*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow4(msq)*pow6(mQ3)*
        pow8(mU3) - 10440*pow2(log(mQ32/mU32))*pow4(msq)*pow6(mQ3)*pow8(mU3) +
        360*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(msq)*pow6(mQ3)*pow8(mU3) -
        180*pow3(log(mQ32/mU32))*pow4(msq)*pow6(mQ3)*pow8(mU3) + 1800*dilog(1 -
        msq2/mU32)*pow4(mQ3)*pow6(msq)*pow8(mU3) - 27000*dilog(1 - mU32/msq2)*
        pow4(mQ3)*pow6(msq)*pow8(mU3) + 540*dilog(1 - msq2/mU32)*log(mQ32/mU32)
        *pow4(mQ3)*pow6(msq)*pow8(mU3) + 3420*dilog(1 - mU32/msq2)*log(mQ32/
        mU32)*pow4(mQ3)*pow6(msq)*pow8(mU3) - 1800*log(mQ32/msq2)*log(mQ32/
        mU32)*pow4(mQ3)*pow6(msq)*pow8(mU3) - 13500*pow2(log(mQ32/msq2))*pow4(
        mQ3)*pow6(msq)*pow8(mU3) + 1710*log(mQ32/mU32)*pow2(log(mQ32/msq2))*
        pow4(mQ3)*pow6(msq)*pow8(mU3) + 900*pow2(log(mQ32/mU32))*pow4(mQ3)*
        pow6(msq)*pow8(mU3) - 540*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(mQ3)
        *pow6(msq)*pow8(mU3) + 270*pow3(log(mQ32/mU32))*pow4(mQ3)*pow6(msq)*
        pow8(mU3) - 1433792*msq2*pow8(mQ3)*pow8(mU3) - 812288*lmQ3MR*msq2*pow8(
        mQ3)*pow8(mU3) - 18000*msq2*dilog(1 - msq2/mU32)*pow8(mQ3)*pow8(mU3) -
        250452*msq2*dilog(1 - mU32/mQ32)*pow8(mQ3)*pow8(mU3) + 107520*lmQ3MR*
        msq2*dilog(1 - mU32/mQ32)*pow8(mQ3)*pow8(mU3) + 102960*msq2*dilog(1 -
        mU32/msq2)*pow8(mQ3)*pow8(mU3) + 353280*msq2*log(mQ32/msq2)*pow8(mQ3)*
        pow8(mU3) - 9600*lmQ3MR*msq2*log(mQ32/msq2)*pow8(mQ3)*pow8(mU3) -
        19200*msq2*dilog(1 - mU32/mQ32)*log(mQ32/msq2)*pow8(mQ3)*pow8(mU3) +
        1226272*msq2*log(mQ32/mU32)*pow8(mQ3)*pow8(mU3) - 969088*lmQ3MR*msq2*
        log(mQ32/mU32)*pow8(mQ3)*pow8(mU3) + 1800*msq2*dilog(1 - msq2/mU32)*
        log(mQ32/mU32)*pow8(mQ3)*pow8(mU3) + 115674*msq2*dilog(1 - mU32/mQ32)*
        log(mQ32/mU32)*pow8(mQ3)*pow8(mU3) - 35640*msq2*dilog(1 - mU32/msq2)*
        log(mQ32/mU32)*pow8(mQ3)*pow8(mU3) - 21360*msq2*log(mQ32/msq2)*log(
        mQ32/mU32)*pow8(mQ3)*pow8(mU3) - 15360*lmQ3MR*msq2*log(mQ32/msq2)*log(
        mQ32/mU32)*pow8(mQ3)*pow8(mU3) - 3072*msq2*log(mU32/mQ32)*pow8(mQ3)*
        pow8(mU3) - 384*msq2*log(mQ32/mU32)*log(mU32/mQ32)*pow8(mQ3)*pow8(mU3)
        + 533760*msq2*pow2(lmQ3MR)*pow8(mQ3)*pow8(mU3) + 38016*msq2*log(mQ32/
        mU32)*pow2(lmQ3MR)*pow8(mQ3)*pow8(mU3) - 6120*msq2*pow2(log(mQ32/msq2))
        *pow8(mQ3)*pow8(mU3) - 12060*msq2*log(mQ32/mU32)*pow2(log(mQ32/msq2))*
        pow8(mQ3)*pow8(mU3) + 363790*msq2*pow2(log(mQ32/mU32))*pow8(mQ3)*pow8(
        mU3) + 63552*lmQ3MR*msq2*pow2(log(mQ32/mU32))*pow8(mQ3)*pow8(mU3) +
        15480*msq2*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow8(mQ3)*pow8(mU3) -
        58287*msq2*pow3(log(mQ32/mU32))*pow8(mQ3)*pow8(mU3) - 5760*pow4(msq)*
        pow4(mU3)*power10(mQ3) - 1800*dilog(1 - msq2/mU32)*pow4(msq)*pow4(mU3)*
        power10(mQ3) - 1800*dilog(1 - mU32/msq2)*pow4(msq)*pow4(mU3)*power10(
        mQ3) + 5760*log(mQ32/msq2)*pow4(msq)*pow4(mU3)*power10(mQ3) + 2880*log(
        mQ32/mU32)*pow4(msq)*pow4(mU3)*power10(mQ3) + 540*dilog(1 - msq2/mU32)*
        log(mQ32/mU32)*pow4(msq)*pow4(mU3)*power10(mQ3) + 540*dilog(1 - mU32/
        msq2)*log(mQ32/mU32)*pow4(msq)*pow4(mU3)*power10(mQ3) - 1080*log(mQ32/
        msq2)*log(mQ32/mU32)*pow4(msq)*pow4(mU3)*power10(mQ3) - 900*pow2(log(
        mQ32/msq2))*pow4(msq)*pow4(mU3)*power10(mQ3) + 270*log(mQ32/mU32)*pow2(
        log(mQ32/msq2))*pow4(msq)*pow4(mU3)*power10(mQ3) - 900*pow2(log(mQ32/
        mU32))*pow4(msq)*pow4(mU3)*power10(mQ3) - 540*log(mQ32/msq2)*pow2(log(
        mQ32/mU32))*pow4(msq)*pow4(mU3)*power10(mQ3) + 270*pow3(log(mQ32/mU32))
        *pow4(msq)*pow4(mU3)*power10(mQ3) - 1800*mU32*dilog(1 - msq2/mU32)*
        pow6(msq)*power10(mQ3) - 1800*mU32*dilog(1 - mU32/msq2)*pow6(msq)*
        power10(mQ3) + 540*mU32*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow6(msq)*
        power10(mQ3) + 540*mU32*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow6(msq)*
        power10(mQ3) + 1800*mU32*log(mQ32/msq2)*log(mQ32/mU32)*pow6(msq)*
        power10(mQ3) - 900*mU32*pow2(log(mQ32/msq2))*pow6(msq)*power10(mQ3) +
        270*mU32*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow6(msq)*power10(mQ3) -
        900*mU32*pow2(log(mQ32/mU32))*pow6(msq)*power10(mQ3) - 540*mU32*log(
        mQ32/msq2)*pow2(log(mQ32/mU32))*pow6(msq)*power10(mQ3) + 270*mU32*pow3(
        log(mQ32/mU32))*pow6(msq)*power10(mQ3) + 515872*msq2*pow6(mU3)*power10(
        mQ3) + 499648*lmQ3MR*msq2*pow6(mU3)*power10(mQ3) + 9000*msq2*dilog(1 -
        msq2/mU32)*pow6(mU3)*power10(mQ3) + 42564*msq2*dilog(1 - mU32/mQ32)*
        pow6(mU3)*power10(mQ3) - 53760*lmQ3MR*msq2*dilog(1 - mU32/mQ32)*pow6(
        mU3)*power10(mQ3) - 19800*msq2*dilog(1 - mU32/msq2)*pow6(mU3)*power10(
        mQ3) - 183360*msq2*log(mQ32/msq2)*pow6(mU3)*power10(mQ3) + 7680*lmQ3MR*
        msq2*log(mQ32/msq2)*pow6(mU3)*power10(mQ3) + 9600*msq2*dilog(1 - mU32/
        mQ32)*log(mQ32/msq2)*pow6(mU3)*power10(mQ3) - 619024*msq2*log(mQ32/
        mU32)*pow6(mU3)*power10(mQ3) + 254688*lmQ3MR*msq2*log(mQ32/mU32)*pow6(
        mU3)*power10(mQ3) - 2700*msq2*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow6(
        mU3)*power10(mQ3) - 28590*msq2*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*
        pow6(mU3)*power10(mQ3) + 5940*msq2*dilog(1 - mU32/msq2)*log(mQ32/mU32)*
        pow6(mU3)*power10(mQ3) + 45240*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow6(
        mU3)*power10(mQ3) + 3840*lmQ3MR*msq2*log(mQ32/msq2)*log(mQ32/mU32)*
        pow6(mU3)*power10(mQ3) + 2688*msq2*log(mU32/mQ32)*pow6(mU3)*power10(
        mQ3) - 576*msq2*log(mQ32/mU32)*log(mU32/mQ32)*pow6(mU3)*power10(mQ3) -
        283008*msq2*pow2(lmQ3MR)*pow6(mU3)*power10(mQ3) + 50496*msq2*log(mQ32/
        mU32)*pow2(lmQ3MR)*pow6(mU3)*power10(mQ3) + 18900*msq2*pow2(log(mQ32/
        msq2))*pow6(mU3)*power10(mQ3) - 5670*msq2*log(mQ32/mU32)*pow2(log(mQ32/
        msq2))*pow6(mU3)*power10(mQ3) - 1626*msq2*pow2(log(mQ32/mU32))*pow6(
        mU3)*power10(mQ3) - 100800*lmQ3MR*msq2*pow2(log(mQ32/mU32))*pow6(mU3)*
        power10(mQ3) - 1140*msq2*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow6(mU3)*
        power10(mQ3) + 67587*msq2*pow3(log(mQ32/mU32))*pow6(mU3)*power10(mQ3) -
        510112*pow8(mU3)*power10(mQ3) - 499648*lmQ3MR*pow8(mU3)*power10(mQ3) -
        5400*dilog(1 - msq2/mU32)*pow8(mU3)*power10(mQ3) - 42564*dilog(1 -
        mU32/mQ32)*pow8(mU3)*power10(mQ3) + 53760*lmQ3MR*dilog(1 - mU32/mQ32)*
        pow8(mU3)*power10(mQ3) + 23400*dilog(1 - mU32/msq2)*pow8(mU3)*power10(
        mQ3) + 177600*log(mQ32/msq2)*pow8(mU3)*power10(mQ3) - 7680*lmQ3MR*log(
        mQ32/msq2)*pow8(mU3)*power10(mQ3) - 9600*dilog(1 - mU32/mQ32)*log(mQ32/
        msq2)*pow8(mU3)*power10(mQ3) + 616144*log(mQ32/mU32)*pow8(mU3)*power10(
        mQ3) - 254688*lmQ3MR*log(mQ32/mU32)*pow8(mU3)*power10(mQ3) + 1620*
        dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow8(mU3)*power10(mQ3) + 28590*
        dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow8(mU3)*power10(mQ3) - 7020*
        dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow8(mU3)*power10(mQ3) - 45960*log(
        mQ32/msq2)*log(mQ32/mU32)*pow8(mU3)*power10(mQ3) - 3840*lmQ3MR*log(
        mQ32/msq2)*log(mQ32/mU32)*pow8(mU3)*power10(mQ3) - 2688*log(mU32/mQ32)*
        pow8(mU3)*power10(mQ3) + 576*log(mQ32/mU32)*log(mU32/mQ32)*pow8(mU3)*
        power10(mQ3) - 19200*log(mU32/msq2)*pow8(mU3)*power10(mQ3) + 5760*log(
        mQ32/mU32)*log(mU32/msq2)*pow8(mU3)*power10(mQ3) + 283008*pow2(lmQ3MR)*
        pow8(mU3)*power10(mQ3) - 50496*log(mQ32/mU32)*pow2(lmQ3MR)*pow8(mU3)*
        power10(mQ3) - 17100*pow2(log(mQ32/msq2))*pow8(mU3)*power10(mQ3) +
        5130*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow8(mU3)*power10(mQ3) + 3426*
        pow2(log(mQ32/mU32))*pow8(mU3)*power10(mQ3) + 100800*lmQ3MR*pow2(log(
        mQ32/mU32))*pow8(mU3)*power10(mQ3) + 2220*log(mQ32/msq2)*pow2(log(mQ32/
        mU32))*pow8(mU3)*power10(mQ3) - 68127*pow3(log(mQ32/mU32))*pow8(mU3)*
        power10(mQ3) - 46080*pow4(mQ3)*pow4(msq)*power10(mU3) + 1800*dilog(1 -
        msq2/mU32)*pow4(mQ3)*pow4(msq)*power10(mU3) + 19080*dilog(1 - mU32/
        msq2)*pow4(mQ3)*pow4(msq)*power10(mU3) + 46080*log(mQ32/msq2)*pow4(mQ3)
        *pow4(msq)*power10(mU3) - 34560*log(mQ32/mU32)*pow4(mQ3)*pow4(msq)*
        power10(mU3) + 540*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow4(mQ3)*pow4(
        msq)*power10(mU3) - 19620*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow4(mQ3)
        *pow4(msq)*power10(mU3) + 3960*log(mQ32/msq2)*log(mQ32/mU32)*pow4(mQ3)*
        pow4(msq)*power10(mU3) + 9540*pow2(log(mQ32/msq2))*pow4(mQ3)*pow4(msq)*
        power10(mU3) - 9810*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow4(mQ3)*pow4(
        msq)*power10(mU3) + 3780*pow2(log(mQ32/mU32))*pow4(mQ3)*pow4(msq)*
        power10(mU3) - 540*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(mQ3)*pow4(
        msq)*power10(mU3) + 270*pow3(log(mQ32/mU32))*pow4(mQ3)*pow4(msq)*
        power10(mU3) + 1805120*msq2*pow6(mQ3)*power10(mU3) + 603008*lmQ3MR*
        msq2*pow6(mQ3)*power10(mU3) + 18000*msq2*dilog(1 - msq2/mU32)*pow6(mQ3)
        *power10(mU3) + 425916*msq2*dilog(1 - mU32/mQ32)*pow6(mQ3)*power10(mU3)
        - 107520*lmQ3MR*msq2*dilog(1 - mU32/mQ32)*pow6(mQ3)*power10(mU3) -
        160560*msq2*dilog(1 - mU32/msq2)*pow6(mQ3)*power10(mU3) - 312960*msq2*
        log(mQ32/msq2)*pow6(mQ3)*power10(mU3) + 19200*msq2*dilog(1 - mU32/mQ32)
        *log(mQ32/msq2)*pow6(mQ3)*power10(mU3) - 818208*msq2*log(mQ32/mU32)*
        pow6(mQ3)*power10(mU3) + 1234112*lmQ3MR*msq2*log(mQ32/mU32)*pow6(mQ3)*
        power10(mU3) + 1800*msq2*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow6(mQ3)*
        power10(mU3) - 91098*msq2*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow6(mQ3)
        *power10(mU3) - 6840*msq2*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow6(mQ3)
        *power10(mU3) - 36240*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow6(mQ3)*
        power10(mU3) + 23040*lmQ3MR*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow6(
        mQ3)*power10(mU3) + 768*msq2*log(mU32/mQ32)*pow6(mQ3)*power10(mU3) +
        768*msq2*log(mQ32/mU32)*log(mU32/mQ32)*pow6(mQ3)*power10(mU3) - 480000*
        msq2*pow2(lmQ3MR)*pow6(mQ3)*power10(mU3) - 177024*msq2*log(mQ32/mU32)*
        pow2(lmQ3MR)*pow6(mQ3)*power10(mU3) - 22680*msq2*pow2(log(mQ32/msq2))*
        pow6(mQ3)*power10(mU3) + 2340*msq2*log(mQ32/mU32)*pow2(log(mQ32/msq2))*
        pow6(mQ3)*power10(mU3) - 646682*msq2*pow2(log(mQ32/mU32))*pow6(mQ3)*
        power10(mU3) + 206016*lmQ3MR*msq2*pow2(log(mQ32/mU32))*pow6(mQ3)*
        power10(mU3) - 11400*msq2*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow6(mQ3)
        *power10(mU3) - 125385*msq2*pow3(log(mQ32/mU32))*pow6(mQ3)*power10(mU3)
        - 360*mQ32*dilog(1 - msq2/mU32)*pow6(msq)*power10(mU3) + 16920*mQ32*
        dilog(1 - mU32/msq2)*pow6(msq)*power10(mU3) - 180*mQ32*dilog(1 - msq2/
        mU32)*log(mQ32/mU32)*pow6(msq)*power10(mU3) + 8460*mQ32*dilog(1 - mU32/
        msq2)*log(mQ32/mU32)*pow6(msq)*power10(mU3) + 360*mQ32*log(mQ32/msq2)*
        log(mQ32/mU32)*pow6(msq)*power10(mU3) + 8460*mQ32*pow2(log(mQ32/msq2))*
        pow6(msq)*power10(mU3) + 4230*mQ32*log(mQ32/mU32)*pow2(log(mQ32/msq2))*
        pow6(msq)*power10(mU3) - 180*mQ32*pow2(log(mQ32/mU32))*pow6(msq)*
        power10(mU3) + 180*mQ32*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow6(msq)*
        power10(mU3) - 90*mQ32*pow3(log(mQ32/mU32))*pow6(msq)*power10(mU3) +
        1433792*pow8(mQ3)*power10(mU3) + 812288*lmQ3MR*pow8(mQ3)*power10(mU3) +
        10800*dilog(1 - msq2/mU32)*pow8(mQ3)*power10(mU3) + 250452*dilog(1 -
        mU32/mQ32)*pow8(mQ3)*power10(mU3) - 107520*lmQ3MR*dilog(1 - mU32/mQ32)*
        pow8(mQ3)*power10(mU3) - 75600*dilog(1 - mU32/msq2)*pow8(mQ3)*power10(
        mU3) - 353280*log(mQ32/msq2)*pow8(mQ3)*power10(mU3) + 9600*lmQ3MR*log(
        mQ32/msq2)*pow8(mQ3)*power10(mU3) + 19200*dilog(1 - mU32/mQ32)*log(
        mQ32/msq2)*pow8(mQ3)*power10(mU3) - 1237792*log(mQ32/mU32)*pow8(mQ3)*
        power10(mU3) + 969088*lmQ3MR*log(mQ32/mU32)*pow8(mQ3)*power10(mU3) -
        1080*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow8(mQ3)*power10(mU3) -
        115674*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow8(mQ3)*power10(mU3) +
        19080*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow8(mQ3)*power10(mU3) +
        34320*log(mQ32/msq2)*log(mQ32/mU32)*pow8(mQ3)*power10(mU3) + 15360*
        lmQ3MR*log(mQ32/msq2)*log(mQ32/mU32)*pow8(mQ3)*power10(mU3) + 3072*log(
        mU32/mQ32)*pow8(mQ3)*power10(mU3) + 384*log(mQ32/mU32)*log(mU32/mQ32)*
        pow8(mQ3)*power10(mU3) + 38400*log(mU32/msq2)*pow8(mQ3)*power10(mU3) -
        3840*log(mQ32/mU32)*log(mU32/msq2)*pow8(mQ3)*power10(mU3) - 533760*
        pow2(lmQ3MR)*pow8(mQ3)*power10(mU3) - 38016*log(mQ32/mU32)*pow2(lmQ3MR)
        *pow8(mQ3)*power10(mU3) + 19800*pow2(log(mQ32/msq2))*pow8(mQ3)*power10(
        mU3) + 3780*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow8(mQ3)*power10(mU3)
        - 364510*pow2(log(mQ32/mU32))*pow8(mQ3)*power10(mU3) - 63552*lmQ3MR*
        pow2(log(mQ32/mU32))*pow8(mQ3)*power10(mU3) - 16200*log(mQ32/msq2)*
        pow2(log(mQ32/mU32))*pow8(mQ3)*power10(mU3) + 58647*pow3(log(mQ32/mU32)
        )*pow8(mQ3)*power10(mU3)))/(36.*mQ32*(-msq2 + mU32)*pow4(mU3)*pow7(mQ32
        - mU32))+
	(2*Xt5*(88*mU32*dilog(1 - mU32/mQ32)*pow12(mQ3) + 48*mU32*log(mQ32/mU32)*
        pow12(mQ3) - 32*mU32*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow12(mQ3) +
        48*mU32*log(mU32/mQ32)*pow12(mQ3) - 24*mU32*log(mQ32/mU32)*log(mU32/
        mQ32)*pow12(mQ3) + 6848*mQ32*pow12(mU3) - 4416*lmQ3MR*mQ32*pow12(mU3) -
        120*mQ32*dilog(1 - msq2/mU32)*pow12(mU3) + 612*mQ32*dilog(1 - mU32/
        mQ32)*pow12(mU3) - 120*mQ32*dilog(1 - mU32/msq2)*pow12(mU3) - 160*mQ32*
        log(mQ32/msq2)*pow12(mU3) + 11408*mQ32*log(mQ32/mU32)*pow12(mU3) -
        5536*lmQ3MR*mQ32*log(mQ32/mU32)*pow12(mU3) - 60*mQ32*dilog(1 - msq2/
        mU32)*log(mQ32/mU32)*pow12(mU3) + 562*mQ32*dilog(1 - mU32/mQ32)*log(
        mQ32/mU32)*pow12(mU3) - 60*mQ32*dilog(1 - mU32/msq2)*log(mQ32/mU32)*
        pow12(mU3) + 40*mQ32*log(mQ32/msq2)*log(mQ32/mU32)*pow12(mU3) + 112*
        mQ32*log(mU32/mQ32)*pow12(mU3) + 56*mQ32*log(mQ32/mU32)*log(mU32/mQ32)*
        pow12(mU3) + 480*mQ32*log(mU32/msq2)*pow12(mU3) + 240*mQ32*log(mQ32/
        mU32)*log(mU32/msq2)*pow12(mU3) - 12*dilog(1 - mU32/mQ32)*pow14(mQ3) +
        6*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow14(mQ3) + 512*pow14(mU3) -
        512*lmQ3MR*pow14(mU3) + 512*log(mQ32/mU32)*pow14(mU3) - 60*mQ32*pow12(
        mU3)*pow2(log(mQ32/msq2)) - 30*mQ32*log(mQ32/mU32)*pow12(mU3)*pow2(log(
        mQ32/msq2)) + 20*mU32*pow12(mQ3)*pow2(log(mQ32/mU32)) + 6158*mQ32*
        pow12(mU3)*pow2(log(mQ32/mU32)) - 512*lmQ3MR*mQ32*pow12(mU3)*pow2(log(
        mQ32/mU32)) + 60*mQ32*log(mQ32/msq2)*pow12(mU3)*pow2(log(mQ32/mU32)) -
        6*pow14(mQ3)*pow2(log(mQ32/mU32)) - 16*mU32*pow12(mQ3)*pow3(log(mQ32/
        mU32)) + 763*mQ32*pow12(mU3)*pow3(log(mQ32/mU32)) + 3*pow14(mQ3)*pow3(
        log(mQ32/mU32)) + 2*mQ32*dilog(1 - mQ32/mU32)*(-2*mQ32 + 2*mU32 + (mQ32
        + mU32)*log(mQ32/mU32))*pow3(mQ32 - mU32)*(-10*mQ32*mU32 + 3*pow4(mQ3)
        - 57*pow4(mU3)) + 960*(mQ32 - mU32)*dilog(1 - msq2/mQ32)*(-2*mQ32 + 2*
        mU32 + (mQ32 + mU32)*log(mQ32/mU32))*pow2(-(mQ3*msq2) + pow3(mQ3))*
        pow4(mU3) - 720*dilog(1 - msq2/mU32)*pow4(msq)*pow4(mU3)*pow6(mQ3) -
        2640*dilog(1 - mU32/msq2)*pow4(msq)*pow4(mU3)*pow6(mQ3) + 960*dilog(1 -
        mU32/msq2)*log(mQ32/mU32)*pow4(msq)*pow4(mU3)*pow6(mQ3) + 720*log(mQ32/
        msq2)*log(mQ32/mU32)*pow4(msq)*pow4(mU3)*pow6(mQ3) - 1320*pow2(log(
        mQ32/msq2))*pow4(msq)*pow4(mU3)*pow6(mQ3) + 480*log(mQ32/mU32)*pow2(
        log(mQ32/msq2))*pow4(msq)*pow4(mU3)*pow6(mQ3) - 360*pow2(log(mQ32/mU32)
        )*pow4(msq)*pow4(mU3)*pow6(mQ3) + 480*dilog(1 - msq2/mU32)*pow4(mQ3)*
        pow4(msq)*pow6(mU3) + 4320*dilog(1 - mU32/msq2)*pow4(mQ3)*pow4(msq)*
        pow6(mU3) + 120*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow4(mQ3)*pow4(msq)
        *pow6(mU3) + 120*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow4(mQ3)*pow4(
        msq)*pow6(mU3) - 480*log(mQ32/msq2)*log(mQ32/mU32)*pow4(mQ3)*pow4(msq)*
        pow6(mU3) + 2160*pow2(log(mQ32/msq2))*pow4(mQ3)*pow4(msq)*pow6(mU3) +
        60*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow4(mQ3)*pow4(msq)*pow6(mU3) +
        240*pow2(log(mQ32/mU32))*pow4(mQ3)*pow4(msq)*pow6(mU3) - 120*log(mQ32/
        msq2)*pow2(log(mQ32/mU32))*pow4(mQ3)*pow4(msq)*pow6(mU3) + 60*pow3(log(
        mQ32/mU32))*pow4(mQ3)*pow4(msq)*pow6(mU3) - 5760*msq2*pow6(mQ3)*pow6(
        mU3) + 1440*msq2*dilog(1 - msq2/mU32)*pow6(mQ3)*pow6(mU3) - 6240*msq2*
        dilog(1 - mU32/msq2)*pow6(mQ3)*pow6(mU3) + 2880*msq2*log(mQ32/msq2)*
        pow6(mQ3)*pow6(mU3) + 1920*msq2*log(mQ32/mU32)*pow6(mQ3)*pow6(mU3) -
        2400*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow6(mQ3)*pow6(mU3) + 2880*
        msq2*log(mU32/msq2)*pow6(mQ3)*pow6(mU3) - 3120*msq2*pow2(log(mQ32/msq2)
        )*pow6(mQ3)*pow6(mU3) + 1680*msq2*pow2(log(mQ32/mU32))*pow6(mQ3)*pow6(
        mU3) + 480*mU32*dilog(1 - msq2/mU32)*pow4(msq)*pow8(mQ3) + 480*mU32*
        dilog(1 - mU32/msq2)*pow4(msq)*pow8(mQ3) - 120*mU32*dilog(1 - msq2/
        mU32)*log(mQ32/mU32)*pow4(msq)*pow8(mQ3) - 120*mU32*dilog(1 - mU32/
        msq2)*log(mQ32/mU32)*pow4(msq)*pow8(mQ3) - 480*mU32*log(mQ32/msq2)*log(
        mQ32/mU32)*pow4(msq)*pow8(mQ3) + 240*mU32*pow2(log(mQ32/msq2))*pow4(
        msq)*pow8(mQ3) - 60*mU32*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow4(msq)*
        pow8(mQ3) + 240*mU32*pow2(log(mQ32/mU32))*pow4(msq)*pow8(mQ3) + 120*
        mU32*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(msq)*pow8(mQ3) - 60*mU32*
        pow3(log(mQ32/mU32))*pow4(msq)*pow8(mQ3) + 1920*msq2*pow4(mU3)*pow8(
        mQ3) - 960*msq2*dilog(1 - msq2/mU32)*pow4(mU3)*pow8(mQ3) + 2880*msq2*
        dilog(1 - mU32/msq2)*pow4(mU3)*pow8(mQ3) - 2880*msq2*log(mQ32/mU32)*
        pow4(mU3)*pow8(mQ3) + 240*msq2*dilog(1 - msq2/mU32)*log(mQ32/mU32)*
        pow4(mU3)*pow8(mQ3) - 1680*msq2*dilog(1 - mU32/msq2)*log(mQ32/mU32)*
        pow4(mU3)*pow8(mQ3) + 1440*msq2*log(mQ32/msq2)*log(mQ32/mU32)*pow4(mU3)
        *pow8(mQ3) - 1920*msq2*log(mU32/msq2)*pow4(mU3)*pow8(mQ3) + 480*msq2*
        log(mQ32/mU32)*log(mU32/msq2)*pow4(mU3)*pow8(mQ3) + 1440*msq2*pow2(log(
        mQ32/msq2))*pow4(mU3)*pow8(mQ3) - 840*msq2*log(mQ32/mU32)*pow2(log(
        mQ32/msq2))*pow4(mU3)*pow8(mQ3) - 240*msq2*log(mQ32/msq2)*pow2(log(
        mQ32/mU32))*pow4(mU3)*pow8(mQ3) + 120*msq2*pow3(log(mQ32/mU32))*pow4(
        mU3)*pow8(mQ3) - 32000*pow6(mU3)*pow8(mQ3) + 25728*lmQ3MR*pow6(mU3)*
        pow8(mQ3) + 480*dilog(1 - msq2/mU32)*pow6(mU3)*pow8(mQ3) - 2160*dilog(1
        - mU32/mQ32)*pow6(mU3)*pow8(mQ3) + 4320*dilog(1 - mU32/msq2)*pow6(mU3)*
        pow8(mQ3) + 640*log(mQ32/msq2)*pow6(mU3)*pow8(mQ3) + 8672*log(mQ32/
        mU32)*pow6(mU3)*pow8(mQ3) - 19840*lmQ3MR*log(mQ32/mU32)*pow6(mU3)*pow8(
        mQ3) - 120*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow6(mU3)*pow8(mQ3) -
        400*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow6(mU3)*pow8(mQ3) - 120*
        dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow6(mU3)*pow8(mQ3) - 3520*log(
        mQ32/msq2)*log(mQ32/mU32)*pow6(mU3)*pow8(mQ3) - 160*log(mU32/mQ32)*
        pow6(mU3)*pow8(mQ3) + 112*log(mQ32/mU32)*log(mU32/mQ32)*pow6(mU3)*pow8(
        mQ3) - 1920*log(mU32/msq2)*pow6(mU3)*pow8(mQ3) + 480*log(mQ32/mU32)*
        log(mU32/msq2)*pow6(mU3)*pow8(mQ3) + 2160*pow2(log(mQ32/msq2))*pow6(
        mU3)*pow8(mQ3) - 60*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow6(mU3)*pow8(
        mQ3) + 9256*pow2(log(mQ32/mU32))*pow6(mU3)*pow8(mQ3) + 896*lmQ3MR*pow2(
        log(mQ32/mU32))*pow6(mU3)*pow8(mQ3) - 40*log(mQ32/msq2)*pow2(log(mQ32/
        mU32))*pow6(mU3)*pow8(mQ3) - 212*pow3(log(mQ32/mU32))*pow6(mU3)*pow8(
        mQ3) + 5760*msq2*pow4(mQ3)*pow8(mU3) - 960*msq2*dilog(1 - msq2/mU32)*
        pow4(mQ3)*pow8(mU3) + 2880*msq2*dilog(1 - mU32/msq2)*pow4(mQ3)*pow8(
        mU3) - 3840*msq2*log(mQ32/msq2)*pow4(mQ3)*pow8(mU3) + 2880*msq2*log(
        mQ32/mU32)*pow4(mQ3)*pow8(mU3) - 240*msq2*dilog(1 - msq2/mU32)*log(
        mQ32/mU32)*pow4(mQ3)*pow8(mU3) + 1680*msq2*dilog(1 - mU32/msq2)*log(
        mQ32/mU32)*pow4(mQ3)*pow8(mU3) + 480*msq2*log(mQ32/msq2)*log(mQ32/mU32)
        *pow4(mQ3)*pow8(mU3) - 1920*msq2*log(mU32/msq2)*pow4(mQ3)*pow8(mU3) -
        480*msq2*log(mQ32/mU32)*log(mU32/msq2)*pow4(mQ3)*pow8(mU3) + 1440*msq2*
        pow2(log(mQ32/msq2))*pow4(mQ3)*pow8(mU3) + 840*msq2*log(mQ32/mU32)*
        pow2(log(mQ32/msq2))*pow4(mQ3)*pow8(mU3) - 960*msq2*pow2(log(mQ32/mU32)
        )*pow4(mQ3)*pow8(mU3) + 240*msq2*log(mQ32/msq2)*pow2(log(mQ32/mU32))*
        pow4(mQ3)*pow8(mU3) - 120*msq2*pow3(log(mQ32/mU32))*pow4(mQ3)*pow8(mU3)
        - 120*mQ32*dilog(1 - msq2/mU32)*pow4(msq)*pow8(mU3) - 2040*mQ32*dilog(1
        - mU32/msq2)*pow4(msq)*pow8(mU3) - 60*mQ32*dilog(1 - msq2/mU32)*log(
        mQ32/mU32)*pow4(msq)*pow8(mU3) - 1020*mQ32*dilog(1 - mU32/msq2)*log(
        mQ32/mU32)*pow4(msq)*pow8(mU3) + 120*mQ32*log(mQ32/msq2)*log(mQ32/mU32)
        *pow4(msq)*pow8(mU3) - 1020*mQ32*pow2(log(mQ32/msq2))*pow4(msq)*pow8(
        mU3) - 510*mQ32*log(mQ32/mU32)*pow2(log(mQ32/msq2))*pow4(msq)*pow8(mU3)
        - 60*mQ32*pow2(log(mQ32/mU32))*pow4(msq)*pow8(mU3) + 60*mQ32*log(mQ32/
        msq2)*pow2(log(mQ32/mU32))*pow4(msq)*pow8(mU3) - 30*mQ32*pow3(log(mQ32/
        mU32))*pow4(msq)*pow8(mU3) + 48256*pow6(mQ3)*pow8(mU3) - 37120*lmQ3MR*
        pow6(mQ3)*pow8(mU3) - 720*dilog(1 - msq2/mU32)*pow6(mQ3)*pow8(mU3) +
        3500*dilog(1 - mU32/mQ32)*pow6(mQ3)*pow8(mU3) - 2640*dilog(1 - mU32/
        msq2)*pow6(mQ3)*pow8(mU3) - 960*log(mQ32/msq2)*pow6(mQ3)*pow8(mU3) +
        14240*log(mQ32/mU32)*pow6(mQ3)*pow8(mU3) + 8512*lmQ3MR*log(mQ32/mU32)*
        pow6(mQ3)*pow8(mU3) + 1490*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow6(
        mQ3)*pow8(mU3) - 960*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow6(mQ3)*
        pow8(mU3) + 1680*log(mQ32/msq2)*log(mQ32/mU32)*pow6(mQ3)*pow8(mU3) +
        480*log(mU32/mQ32)*pow6(mQ3)*pow8(mU3) - 48*log(mQ32/mU32)*log(mU32/
        mQ32)*pow6(mQ3)*pow8(mU3) + 2880*log(mU32/msq2)*pow6(mQ3)*pow8(mU3) -
        1320*pow2(log(mQ32/msq2))*pow6(mQ3)*pow8(mU3) - 480*log(mQ32/mU32)*
        pow2(log(mQ32/msq2))*pow6(mQ3)*pow8(mU3) - 9730*pow2(log(mQ32/mU32))*
        pow6(mQ3)*pow8(mU3) + 5056*lmQ3MR*pow2(log(mQ32/mU32))*pow6(mQ3)*pow8(
        mU3) + 800*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow6(mQ3)*pow8(mU3) -
        2695*pow3(log(mQ32/mU32))*pow6(mQ3)*pow8(mU3) + 240*msq2*mU32*dilog(1 -
        msq2/mU32)*power10(mQ3) + 240*msq2*mU32*dilog(1 - mU32/msq2)*power10(
        mQ3) - 480*msq2*mU32*log(mQ32/msq2)*power10(mQ3) + 480*msq2*mU32*log(
        mQ32/mU32)*power10(mQ3) - 120*msq2*mU32*dilog(1 - msq2/mU32)*log(mQ32/
        mU32)*power10(mQ3) - 120*msq2*mU32*dilog(1 - mU32/msq2)*log(mQ32/mU32)*
        power10(mQ3) + 480*msq2*mU32*log(mU32/msq2)*power10(mQ3) - 240*msq2*
        mU32*log(mQ32/mU32)*log(mU32/msq2)*power10(mQ3) + 120*msq2*mU32*pow2(
        log(mQ32/msq2))*power10(mQ3) - 60*msq2*mU32*log(mQ32/mU32)*pow2(log(
        mQ32/msq2))*power10(mQ3) - 120*msq2*mU32*pow2(log(mQ32/mU32))*power10(
        mQ3) + 120*msq2*mU32*log(mQ32/msq2)*pow2(log(mQ32/mU32))*power10(mQ3) -
        60*msq2*mU32*pow3(log(mQ32/mU32))*power10(mQ3) - 120*dilog(1 - msq2/
        mU32)*pow4(msq)*power10(mQ3) - 120*dilog(1 - mU32/msq2)*pow4(msq)*
        power10(mQ3) + 60*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow4(msq)*
        power10(mQ3) + 60*dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow4(msq)*
        power10(mQ3) + 120*log(mQ32/msq2)*log(mQ32/mU32)*pow4(msq)*power10(mQ3)
        - 60*pow2(log(mQ32/msq2))*pow4(msq)*power10(mQ3) + 30*log(mQ32/mU32)*
        pow2(log(mQ32/msq2))*pow4(msq)*power10(mQ3) - 60*pow2(log(mQ32/mU32))*
        pow4(msq)*power10(mQ3) - 60*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(
        msq)*power10(mQ3) + 30*pow3(log(mQ32/mU32))*pow4(msq)*power10(mQ3) +
        7872*pow4(mU3)*power10(mQ3) - 6592*lmQ3MR*pow4(mU3)*power10(mQ3) - 120*
        dilog(1 - msq2/mU32)*pow4(mU3)*power10(mQ3) + 380*dilog(1 - mU32/mQ32)*
        pow4(mU3)*power10(mQ3) - 2040*dilog(1 - mU32/msq2)*pow4(mU3)*power10(
        mQ3) - 160*log(mQ32/msq2)*pow4(mU3)*power10(mQ3) - 6448*log(mQ32/mU32)*
        pow4(mU3)*power10(mQ3) + 8544*lmQ3MR*log(mQ32/mU32)*pow4(mU3)*power10(
        mQ3) + 60*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow4(mU3)*power10(mQ3) -
        10*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow4(mU3)*power10(mQ3) + 1020*
        dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow4(mU3)*power10(mQ3) + 1800*log(
        mQ32/msq2)*log(mQ32/mU32)*pow4(mU3)*power10(mQ3) - 80*log(mU32/mQ32)*
        pow4(mU3)*power10(mQ3) - 8*log(mQ32/mU32)*log(mU32/mQ32)*pow4(mU3)*
        power10(mQ3) + 480*log(mU32/msq2)*pow4(mU3)*power10(mQ3) - 240*log(
        mQ32/mU32)*log(mU32/msq2)*pow4(mU3)*power10(mQ3) - 1020*pow2(log(mQ32/
        msq2))*pow4(mU3)*power10(mQ3) + 510*log(mQ32/mU32)*pow2(log(mQ32/msq2))
        *pow4(mU3)*power10(mQ3) - 1574*pow2(log(mQ32/mU32))*pow4(mU3)*power10(
        mQ3) - 2496*lmQ3MR*pow2(log(mQ32/mU32))*pow4(mU3)*power10(mQ3) - 860*
        log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(mU3)*power10(mQ3) + 1161*pow3(
        log(mQ32/mU32))*pow4(mU3)*power10(mQ3) - 1920*mQ32*msq2*power10(mU3) +
        240*mQ32*msq2*dilog(1 - msq2/mU32)*power10(mU3) + 240*mQ32*msq2*dilog(1
        - mU32/msq2)*power10(mU3) + 1440*mQ32*msq2*log(mQ32/msq2)*power10(mU3)
        - 2400*mQ32*msq2*log(mQ32/mU32)*power10(mU3) + 120*mQ32*msq2*dilog(1 -
        msq2/mU32)*log(mQ32/mU32)*power10(mU3) + 120*mQ32*msq2*dilog(1 - mU32/
        msq2)*log(mQ32/mU32)*power10(mU3) + 480*mQ32*msq2*log(mQ32/msq2)*log(
        mQ32/mU32)*power10(mU3) + 480*mQ32*msq2*log(mU32/msq2)*power10(mU3) +
        240*mQ32*msq2*log(mQ32/mU32)*log(mU32/msq2)*power10(mU3) + 120*mQ32*
        msq2*pow2(log(mQ32/msq2))*power10(mU3) + 60*mQ32*msq2*log(mQ32/mU32)*
        pow2(log(mQ32/msq2))*power10(mU3) - 600*mQ32*msq2*pow2(log(mQ32/mU32))*
        power10(mU3) - 120*mQ32*msq2*log(mQ32/msq2)*pow2(log(mQ32/mU32))*
        power10(mU3) + 60*mQ32*msq2*pow3(log(mQ32/mU32))*power10(mU3) - 31488*
        pow4(mQ3)*power10(mU3) + 22912*lmQ3MR*pow4(mQ3)*power10(mU3) + 480*
        dilog(1 - msq2/mU32)*pow4(mQ3)*power10(mU3) - 2408*dilog(1 - mU32/mQ32)
        *pow4(mQ3)*power10(mU3) + 480*dilog(1 - mU32/msq2)*pow4(mQ3)*power10(
        mU3) + 640*log(mQ32/msq2)*pow4(mQ3)*power10(mU3) - 28432*log(mQ32/mU32)
        *pow4(mQ3)*power10(mU3) + 8320*lmQ3MR*log(mQ32/mU32)*pow4(mQ3)*power10(
        mU3) + 120*dilog(1 - msq2/mU32)*log(mQ32/mU32)*pow4(mQ3)*power10(mU3) -
        1616*dilog(1 - mU32/mQ32)*log(mQ32/mU32)*pow4(mQ3)*power10(mU3) + 120*
        dilog(1 - mU32/msq2)*log(mQ32/mU32)*pow4(mQ3)*power10(mU3) - 400*log(
        mU32/mQ32)*pow4(mQ3)*power10(mU3) - 88*log(mQ32/mU32)*log(mU32/mQ32)*
        pow4(mQ3)*power10(mU3) - 1920*log(mU32/msq2)*pow4(mQ3)*power10(mU3) -
        480*log(mQ32/mU32)*log(mU32/msq2)*pow4(mQ3)*power10(mU3) + 240*pow2(
        log(mQ32/msq2))*pow4(mQ3)*power10(mU3) + 60*log(mQ32/mU32)*pow2(log(
        mQ32/msq2))*pow4(mQ3)*power10(mU3) - 4124*pow2(log(mQ32/mU32))*pow4(
        mQ3)*power10(mU3) - 2944*lmQ3MR*pow2(log(mQ32/mU32))*pow4(mQ3)*power10(
        mU3) + 40*log(mQ32/msq2)*pow2(log(mQ32/mU32))*pow4(mQ3)*power10(mU3) +
        1508*pow3(log(mQ32/mU32))*pow4(mQ3)*power10(mU3)))/(3.*mQ32*pow3(mU3)*
        pow7(mQ32 - mU32))+
	(1280*mU32*Xt6*(-2*mQ32 + 2*mU32 + (mQ32 + mU32)*log(mQ32/mU32))*pow2(-
        mQ32 + mU32 + mQ32*log(mQ32/mU32)))/(3.*pow7(mQ32 - mU32));
      }
      case(Limits::MQ3_EQ_MU3_EQ_M3):{
	 return (Xt4*(10990*msq2*pow4(mQ3) - 296*lmQ3MR*msq2*pow4(mQ3) - 2724*msq2*log(
        mQ32/msq2)*pow4(mQ3) - 480*lmQ3MR*msq2*log(mQ32/msq2)*pow4(mQ3) - 1044*
        msq2*log(msq2/mQ32)*pow4(mQ3) - 312*msq2*pow2(lmQ3MR)*pow4(mQ3) + 612*
        msq2*pow2(log(mQ32/msq2))*pow4(mQ3) - 432*mQ32*log(mQ32/msq2)*pow4(msq)
        - 432*mQ32*log(msq2/mQ32)*pow4(msq) + 279*mQ32*pow2(log(mQ32/msq2))*
        pow4(msq) - 10990*pow6(mQ3) + 296*lmQ3MR*pow6(mQ3) + 1200*log(mQ32/
        msq2)*pow6(mQ3) + 480*lmQ3MR*log(mQ32/msq2)*pow6(mQ3) + 312*pow2(
        lmQ3MR)*pow6(mQ3) - 81*pow2(log(mQ32/msq2))*pow6(mQ3) + 18*dilog(1 -
        mQ32/msq2)*(28*msq2*pow4(mQ3) + 31*mQ32*pow4(msq) + 31*pow6(mQ3) - 90*
        pow6(msq)) + 2676*log(mQ32/msq2)*pow6(msq) + 2676*log(msq2/mQ32)*pow6(
        msq) - 810*pow2(log(mQ32/msq2))*pow6(msq) - 18*dilog(1 - msq2/mQ32)*(-
        68*msq2*pow4(mQ3) - 31*mQ32*pow4(msq) + 9*pow6(mQ3) + 90*pow6(msq))))/(
        27.*(mQ32 - msq2)*pow8(mQ3))
	-(Xt5*(2176*msq2*pow4(mQ3) - 11264*lmQ3MR*msq2*pow4(mQ3) - 3744*msq2*log(
        mQ32/msq2)*pow4(mQ3) + 2016*msq2*log(msq2/mQ32)*pow4(mQ3) + 2331*msq2*
        pow2(log(mQ32/msq2))*pow4(mQ3) - 47232*mQ32*log(mQ32/msq2)*pow4(msq) -
        47232*mQ32*log(msq2/mQ32)*pow4(msq) + 14085*mQ32*pow2(log(mQ32/msq2))*
        pow4(msq) - 2176*pow6(mQ3) + 11264*lmQ3MR*pow6(mQ3) - 1920*log(mQ32/
        msq2)*pow6(mQ3) - 1449*pow2(log(mQ32/msq2))*pow6(mQ3) + 52896*log(mQ32/
        msq2)*pow6(msq) + 52896*log(msq2/mQ32)*pow6(msq) - 14967*pow2(log(mQ32/
        msq2))*pow6(msq) - 18*dilog(1 - mQ32/msq2)*(-259*msq2*pow4(mQ3) - 1565*
        mQ32*pow4(msq) + 161*pow6(mQ3) + 1663*pow6(msq)) - 18*dilog(1 - msq2/
        mQ32)*(-259*msq2*pow4(mQ3) - 1565*mQ32*pow4(msq) + 161*pow6(mQ3) +
        1663*pow6(msq))))/(216.*(mQ32 - msq2)*pow9(mQ3))+
	(160*Xt6)/(9.*pow6(mQ3));
      }
      case(Limits::DEGENERATE):{
	 return (2*Xt4*(-5735 + 148*lmQ3MR + 156*pow2(lmQ3MR)))/(27.*pow4(mQ3))+
	 (-176*(-7 + 8*lmQ3MR)*Xt5)/(27.*pow5(mQ3))
	 +160/(9.*pow6(mQ3))*Xt6;
      }
   };
   
   throw std::runtime_error("Mass limit not included!");
}


/**
 * 	Sets the mass limit to check terms
 * 	@param limit an integer key for a mass limit
 */
void himalaya::ThresholdCalculator::setLimit(int limit){
   p.massLimit3LThreshold = limit;
}

/**
 * 	Get the mass limit determined by ThresholdCalculator
 * 	@return The determined mass limit
 */
int himalaya::ThresholdCalculator::getLimit(){
   return p.massLimit3LThreshold;
}

