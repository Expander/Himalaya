#define Pi M_PI

#include <HierarchyCalculator.hpp>
#include <iostream>
#include <stdexcept>
#include <type_traits>

// some templates to perform operations between int's and complex<double>
template< typename T, typename SCALAR > inline
typename std::enable_if< !std::is_same<T,SCALAR>::value, std::complex<T> >::type
operator* ( const std::complex<T>& c, SCALAR n ) { return c * T(n) ; }

template< typename T, typename SCALAR > inline
typename std::enable_if< !std::is_same<T,SCALAR>::value, std::complex<T> >::type
operator* ( SCALAR n, const std::complex<T>& c ) { return T(n) * c ; }

template< typename T, typename SCALAR > inline
typename std::enable_if< !std::is_same<T,SCALAR>::value, std::complex<T> >::type
operator/ ( const std::complex<T>& c, SCALAR n ) { return c / T(n) ; }

template< typename T, typename SCALAR > inline
typename std::enable_if< !std::is_same<T,SCALAR>::value, std::complex<T> >::type
operator/ ( SCALAR n, const std::complex<T>& c ) { return T(n) / c ; }

template< typename T, typename SCALAR > inline
typename std::enable_if< !std::is_same<T,SCALAR>::value, std::complex<T> >::type
operator+ ( const std::complex<T>& c, SCALAR n ) { return c + T(n) ; }

template< typename T, typename SCALAR > inline
typename std::enable_if< !std::is_same<T,SCALAR>::value, std::complex<T> >::type
operator+ ( SCALAR n, const std::complex<T>& c ) { return T(n) + c ; }

template< typename T, typename SCALAR > inline
typename std::enable_if< !std::is_same<T,SCALAR>::value, std::complex<T> >::type
operator- ( const std::complex<T>& c, SCALAR n ) { return c - T(n) ; }

template< typename T, typename SCALAR > inline
typename std::enable_if< !std::is_same<T,SCALAR>::value, std::complex<T> >::type
operator- ( SCALAR n, const std::complex<T>& c ) { return T(n) - c ; }

template <typename T> T pow2(T x)  { return x*x; }
template <typename T> T pow3(T x)  { return x*x*x; }
template <typename T> T pow4(T x)  { return x*x*x*x; }
template <typename T> T pow5(T x)  { return x*x*x*x*x; }
template <typename T> T pow6(T x)  { return x*x*x*x*x*x; }
template <typename T> T pow7(T x)  { return x*x*x*x*x*x*x; }
template <typename T> T pow8(T x)  { return x*x*x*x*x*x*x*x; }
template <typename T> T pow9(T x)  { return x*x*x*x*x*x*x*x*x; }
template <typename T> T power10(T x) { return x*x*x*x*x*x*x*x*x*x; }
template <typename T> T pow11(T x) { return x*x*x*x*x*x*x*x*x*x*x;}
template <typename T> T pow12(T x) { return x*x*x*x*x*x*x*x*x*x*x*x;}
template <typename T> T upcut(T x, bool cut) { if(cut){return 0;} else{return x;}}

extern "C" void DSZHiggs_(double *t, double *mg, double *T1, double *T2, double *st, double *ct, double *q, double *mu, double *tanb,
      double *v2, double *gs, int *OS, double *S11, double *S22, double *S12);

/*
 * 	constructor
 */
h3m::HierarchyCalculator::HierarchyCalculator(const Parameters& p){
   this -> p = p;
   // init constants
   // imaginary unit
   const std::complex<double> I(0., 1.);
   
   // Riemann-Zeta
   z2 = pow2(Pi)/6.;
   z3 = 1.202056903159594;
   z4 = pow4(Pi)/90.;
   
   // polylogs
   double pl412 = 0.51747906167389934317668576113647; // PolyLog[4,1/2]
   std::complex<double> pl2expPi3 (0.27415567780803773941206919444, 1.014941606409653625021202554275); // PolyLog[2, Exp[I Pi / 3]]
   std::complex<double> pl3expPi6sqrt3 (0.51928806536375962552715984277228, - 0.33358157526196370641686908633664); // PolyLog[3, Exp[- I Pi / 6] / Sqrt[3]]
   
   // polylog functions, checked
   B4 = (-4 * z2 * pow2(log(2)) + 2 / 3.* pow4(log(2)) - 13 / 2. * z4 + 16. * pl412);
   D3 = 6 * z3 - 15 / 4. * z4 - 6. * pow2(std::imag(pl2expPi3));
   DN = 6 * z3 - 4 * z2 * pow2(log(2)) + 2 / 3. * pow4(log(2)) - 21 / 2. * z4 + 16. * pl412;
   OepS2 = - 763 / 32. - (9 * Pi * sqrt(3) * pow2(log(3))) / 16. - (35 * pow3(Pi) * sqrt(3)) / 48.
      + 195 / 16. * z2 - 15 / 4. * z3 + 57 / 16. * z4 + 45 * sqrt(3) / 2. * std::imag(pl2expPi3)
      - 27 * sqrt(3) * std::imag(pl3expPi6sqrt3);
   S2 = 4 * std::imag(pl2expPi3) / (9. * sqrt(3));
   T1ep = - 45 / 2. - (Pi * sqrt(3) * pow2(log(3))) / 8. - (35 * pow3(Pi) * sqrt(3)) / 216. - 9 / 2. * z2 + z3 
      + 6. * sqrt(3) * std::imag(pl2expPi3) - 6. * sqrt(3) * std::imag(pl3expPi6sqrt3);
   
   // beta
   const double beta = atan(p.vu / p.vd);
   
   //sw2
   const double sw2 = 1 - pow2(p.MW / p.MZ);
   
   // Al4p
   Al4p = pow2(p.g3 / (4 * Pi));
   
   // MGl
   Mgl = p.MG;
   
   // Msq, checked
   Msq = (2 * sqrt(p.mq2(0, 0)) + sqrt(p.mu2(0, 0)) + sqrt(p.md2(0, 0))	// sup and sdown
      + 2 * sqrt(p.mq2(1, 1)) + sqrt(p.mu2(1, 1)) + sqrt(p.md2(1, 1))	// scharm and sstrange
      // sbottom
      + sqrt(p.mq2(2, 2) + pow2(p.Mb) - (1 / 2. - 1 / 3. * sw2) * pow2(p.MZ) * cos(2 * beta))
      + sqrt(p.md2(2, 2) + pow2(p.Mb) - 1 / 3. * sw2 * pow2(p.MZ) * cos(2 * beta))) / 10.;

   // lmMsq, checked
   lmMsq = log(pow2(p.scale / Msq));
   
   // lmMgl, checked
   lmMgl = log(pow2(p.scale / Mgl));
   
   // prefactor, GF = 1/(sqrt(2) * (vu^2 + vd^2)) (here GF is calculated in the DRbar scheme, checked)
   prefac = (3. / (sqrt(2) * (pow2(p.vu) + pow2(p.vd)) * sqrt(2) * pow2(Pi) * pow2(sin(beta))));
   //prefac = 2.53386E-6;
}

/*
 * 	compares deviation of all hierarchies with the exact 2-loop result and returns the hierarchy which minimizes the error
 */
int h3m::HierarchyCalculator::compareHierarchies(const bool isBottom){
   isComparingHierarchies = true;
   double error = -1.;
   int suitableHierarchy = -1;
   // sine of 2 times beta
   const double s2b = sin(2*atan(p.vu/p.vd));
   const double tbeta = p.vu/p.vd;
   // tree level Higgs mass matrix
   Eigen::Matrix2d treelvl;
   treelvl (0,0) = s2b/2.*(pow2(p.MZ) / tbeta + pow2(p.MA) * tbeta);
   treelvl (1,0) = s2b/2.*(-pow2(p.MZ) - pow2(p.MA));
   treelvl (0,1) = treelvl (1,0);
   treelvl (1,1) = s2b/2.*(pow2(p.MZ) * tbeta + pow2(p.MA) / tbeta);
   
   // calculate the exact 1-loop result (only alpha_t/b)
   Eigen::Matrix2d Mt41L = getMt41L(isBottom);

   // compare the exact higgs mass at 2-loop level with the expanded expressions to find a suitable hierarchy
   for(unsigned int hierarchy = h3; hierarchy <= h9q2; hierarchy ++){
      // first, check if the hierarchy is suitable to the mass spectrum
      if(isHierarchySuitable(hierarchy, isBottom)){
	 // call the routine of Pietro Slavich to get the alpha_s alpha_t/b corrections with the MDRbar masses
	 Eigen::Matrix2d Mt42L = getMt42L(hierarchyMap.at(hierarchy), isBottom);
	 
	 // check for spurious poles. If this is the case slightly change Mst2
	 if(std::isnan(Mt42L(0,0)) || std::isnan(Mt42L(1,0)) || std::isnan(Mt42L(1,1))){
	    deltaDSZ = 1.0E-6;
	    Mt42L = getMt42L(hierarchyMap.at(hierarchy), isBottom);
	 }
	 
	 // calc 1-loop shift for DRbar -> MDRbar
	 Eigen::Matrix2d shift = getShift(hierarchyMap.at(hierarchy), isBottom);
	 
	 //calculate the exact higgs mass at 2-loop (only up to alpha_s alpha_t/b)
	 Eigen::EigenSolver<Eigen::Matrix2d> es2L (treelvl + Mt41L + shift + Mt42L);
	 double Mh2l = sortEigenvalues(es2L).at(0);

	 // calculate the expanded 2-loop expression with the specific hierarchy
	 Eigen::EigenSolver<Eigen::Matrix2d> esExpanded (treelvl + Mt41L + calculateHierarchy(hierarchy, isBottom, 0,1,0));
	 
	 // calculate the higgs mass in the given mass hierarchy and compare the result to estimate the error
	 double Mh2LExpanded = sortEigenvalues(esExpanded).at(0);

	 // estimate the error
	 double currError = fabs((Mh2l - Mh2LExpanded));
	 
	 // if the error is negative, it is the first iteration and there is no hierarchy which fits better
	 if(error < 0){
	    error = currError;
	    suitableHierarchy = hierarchy;
	 }
	 // compare the current error with the last error and choose the hierarchy which fits best (lowest error)
	 else if(currError < error){
	    error = currError;
	    suitableHierarchy = hierarchy;
	 }
      }
   }
   isComparingHierarchies = false;
   return suitableHierarchy;
}

/*
 * 	calculates the expanded self-energy to a given order and for a specific hierarchy (tag)
 */
Eigen::Matrix2d h3m::HierarchyCalculator::calculateHierarchy(const unsigned int tag, const bool isBottom,
      const unsigned int oneLoopFlagIn,
         const unsigned int twoLoopFlagIn,
            const unsigned int threeLoopFlagIn) {
   // the hierarchy files containing 1-, 2- and 3-loop terms (alpha_s^0 alpha_t/b, alpha_s alpha_t/b, alpha_s^2 alpha_t/b)
   double sigS1Full = 0., sigS2Full = 0., sigS12Full = 0.;
   
   // common variables
   double At, Mt, s2t, Mst1, Mst2;
   if (!isBottom) {
      At = p.At;
      Mt = p.Mt;
      s2t = p.s2t;
   }
   else {
      At = p.Ab;
      Mt = p.Mb;
      s2t = p.s2b;
   }
   const double Tbeta = p.vu / p.vd;
   const double Cbeta = cos(atan(Tbeta));
   const double Sbeta = sin(atan(Tbeta));
   const double scale = p.scale;
   const double lmMt = log(pow2(scale / Mt));
   const double MuSUSY = p.mu;

   // these shifts are needed to eliminate huge corrections (cf. arXiv:1005.5709 [hep-ph] eq. (46) - (49))
   int shiftst1 = 1, shiftst2 = 1, shiftst3 = 1;
   // specific variables for hierarchies
   double Dmglst1, Dmglst2, Dmsqst1, Dmsqst2, Dmst12, lmMst1, lmMst2, Msusy, lmMsusy;
   int xDR2DRMOD;
   Mst1 = p.MSt(0, 0);
   Mst2 = p.MSt(1, 0);
   
   // bool to truncate the expansion while comparing at 2-loop level or to estimate the expansion error
   bool cut = false;
   // bool to truncate the expansion while comparing at 2-loop level or to estimate the expansion error (when estimating the error
   //	this variable should always be false!!
   bool cutErr = false;
   // if isComparingHierarchies == true, than truncate the two-terms like in H3m
   if(isComparingHierarchies){
      cut = true;
      cutErr = true;
   }
   // this loop is needed to calculate the suitable mass shift order by order
   for(int currentLoopOrder = 1; currentLoopOrder < 4; currentLoopOrder ++){
      bool runThisOrder;
      double curSig1 = 0., curSig2 = 0., curSig12 = 0.;
      int oneLoopFlag = 0, twoLoopFlag = 0, threeLoopFlag = 0;
      switch (currentLoopOrder){
	 case 1:
	    oneLoopFlag = 1;
	    runThisOrder = oneLoopFlag == oneLoopFlagIn;
	 break;
	 case 2:
	    twoLoopFlag = 1;
	    runThisOrder = twoLoopFlag == twoLoopFlagIn;
	 break;
	 case 3:
	    threeLoopFlag = 1;
	    runThisOrder = threeLoopFlag == threeLoopFlagIn;
	 break;
      }
      if(runThisOrder){
	 // select the suitable hierarchy for the specific tag and set variables
	 switch(hierarchyMap.at(tag)){
	    case h3:
	       if(threeLoopFlag == 0){
		  Mst1 = shiftMst1ToMDR(h3, isBottom, twoLoopFlag, 0);
		  Mst2 = shiftMst2ToMDR(h3, isBottom, twoLoopFlag, 0);
	       }
	       else{
		  Mst1 = shiftMst1ToMDR(h3, isBottom, 1, 1);
		  Mst2 = shiftMst2ToMDR(h3, isBottom, 1, 1);
	       }
	       Dmglst1 = Mgl - Mst1;
	       Dmsqst1 = pow2(Msq) - pow2(Mst1);
	       Dmst12 = pow2(Mst1) - pow2(Mst2);
	       lmMst1 = log(pow2(scale / Mst1));
	       lmMsusy = log(pow2(scale / ((Mst1 + Mst2 + Mgl + 10*Msq) / 13.)));
	       switch(tag){
		  case h3:
		     curSig1 = 
		     #include "../hierarchies/h3/sigS1Full.inc"
		     ;
		     curSig2 = 
		     #include "../hierarchies/h3/sigS2Full.inc"
		     ;
		     curSig12 = 
		     #include "../hierarchies/h3/sigS12Full.inc"
		     ;
		     break;
		  case h32q2g:
		     curSig1 = 
		     #include "../hierarchies/h32q2g/sigS1Full.inc"
		     ;
		     curSig2 =
		     #include "../hierarchies/h32q2g/sigS2Full.inc"
		     ;
		     curSig12 = 
		     #include "../hierarchies/h32q2g/sigS12Full.inc"
		     ;
		     break;
		  case h3q22g:
		     curSig1 = 
		     #include "../hierarchies/h3q22g/sigS1Full.inc"
		     ;
		     curSig2 = 
		     #include "../hierarchies/h3q22g/sigS2Full.inc"
		     ;
		     curSig12 = 
		     #include "../hierarchies/h3q22g/sigS12Full.inc"
		     ;
		     break;
	       }
	       break;
	    case h4:
	       if(threeLoopFlag == 0){
		  Mst1 = shiftMst1ToMDR(h4, isBottom, twoLoopFlag, 0);
		  Mst2 = shiftMst2ToMDR(h4, isBottom, twoLoopFlag, 0);
	       }
	       else{
		  Mst1 = shiftMst1ToMDR(h4, isBottom, 1, 1);
		  Mst2 = shiftMst2ToMDR(h4, isBottom, 1, 1);
	       }
	       Msusy = (Mst1 + Mst2 + Mgl) / 3.;
	       lmMsusy = log(pow2(scale / Msusy));
	       curSig1 = 
	       #include "../hierarchies/h4/sigS1Full.inc"
	       ;
	       curSig2 = 
	       #include "../hierarchies/h4/sigS2Full.inc"
	       ;
	       curSig12 = 
	       #include "../hierarchies/h4/sigS12Full.inc"
	       ;
	       break;
	    case h5:
	       if(threeLoopFlag == 0){
		  Mst1 = shiftMst1ToMDR(h5, isBottom, twoLoopFlag, 0);
		  Mst2 = shiftMst2ToMDR(h5, isBottom, twoLoopFlag, 0);
	       }
	       else{
		  Mst1 = shiftMst1ToMDR(h5, isBottom, 1, 1);
		  Mst2 = shiftMst2ToMDR(h5, isBottom, 1, 1);
	       }
	       Dmglst1 = Mgl - Mst1;
	       lmMst1 = log(pow2(scale / Mst1));
	       lmMst2 = log(pow2(scale / Mst2));
	       switch(tag){
		  case h5:
		     curSig1 = 
		     #include "../hierarchies/h5/sigS1Full.inc"
		     ;
		     curSig2 = 
		     #include "../hierarchies/h5/sigS2Full.inc"
		     ;
		     curSig12 = 
		     #include "../hierarchies/h5/sigS12Full.inc"
		     ;
		     break;
		  case h5g1:
		     curSig1 = 
		     #include "../hierarchies/h5g1/sigS1Full.inc"
		     ;
		     curSig2 = 
		     #include "../hierarchies/h5g1/sigS2Full.inc"
		     ;
		     curSig12 = 
		     #include "../hierarchies/h5g1/sigS12Full.inc"
		     ;
		     break;
	       }
	       break;
	    case h6:
	       if(threeLoopFlag == 0){
		  Mst1 = shiftMst1ToMDR(h6, isBottom, twoLoopFlag, 0);
		  Mst2 = shiftMst2ToMDR(h6, isBottom, twoLoopFlag, 0);
	       }
	       else{
		  Mst1 = shiftMst1ToMDR(h6, isBottom, 1, 1);
		  Mst2 = shiftMst2ToMDR(h6, isBottom, 1, 1);
	       }
	       Dmglst2 = Mgl - Mst2;
	       lmMst1 = log(pow2(scale / Mst1));
	       lmMst2 = log(pow2(scale / Mst2));
	       xDR2DRMOD = 1;
	       switch(tag){
		  case h6:
		     curSig1 = 
		     #include "../hierarchies/h6/sigS1Full.inc"
		     ;
		     curSig2 = 
		     #include "../hierarchies/h6/sigS2Full.inc"
		     ;
		     curSig12 = 
		     #include "../hierarchies/h6/sigS12Full.inc"
		     ;
		     break;
		  case h6g2:
		     curSig1 = 
		     #include "../hierarchies/h6g2/sigS1Full.inc"
		     ;
		     curSig2 = 
		     #include "../hierarchies/h6g2/sigS2Full.inc"
		     ;
		     curSig12 = 
		     #include "../hierarchies/h6g2/sigS12Full.inc"
		     ;
		     break;
	       }
	       break;
	    case h6b:
	       if(threeLoopFlag == 0){
		  Mst1 = shiftMst1ToMDR(h6b, isBottom, twoLoopFlag, 0);
		  Mst2 = shiftMst2ToMDR(h6b, isBottom, twoLoopFlag, 0);
	       }
	       else{
		  Mst1 = shiftMst1ToMDR(h6b, isBottom, 1, 1);
		  Mst2 = shiftMst2ToMDR(h6b, isBottom, 1, 1);
	       }
	       Dmglst2 = Mgl - Mst2;
	       Dmsqst2 = Msq - Mst2;
	       lmMst1 = log(pow2(scale / Mst1));
	       lmMst2 = log(pow2(scale / Mst2));
	       xDR2DRMOD = 1;
	       switch(tag){
		  case h6b:
		     curSig1 = 
		     #include "../hierarchies/h6b/sigS1Full.inc"
		     ;
		     curSig2 = 
		     #include "../hierarchies/h6b/sigS2Full.inc"
		     ;
		     curSig12 = 
		     #include "../hierarchies/h6b/sigS12Full.inc"
		     ;
		     break;
		  case h6b2qg2:
		     curSig1 = 
		     #include "../hierarchies/h6b2qg2/sigS1Full.inc"
		     ;
		     curSig2 = 
		     #include "../hierarchies/h6b2qg2/sigS2Full.inc"
		     ;
		     curSig12 = 
		     #include "../hierarchies/h6b2qg2/sigS12Full.inc"
		     ;
		     break;
		  case h6bq22g:
		     curSig1 = 
		     #include "../hierarchies/h6bq22g/sigS1Full.inc"
		     ;
		     curSig2 = 
		     #include "../hierarchies/h6bq22g/sigS2Full.inc"
		     ;
		     curSig12 = 
		     #include "../hierarchies/h6bq22g/sigS12Full.inc"
		     ;
		     break;
		  case h6bq2g2:
		     curSig1 = 
		     #include "../hierarchies/h6bq2g2/sigS1Full.inc"
		     ;
		     curSig2 = 
		     #include "../hierarchies/h6bq2g2/sigS2Full.inc"
		     ;
		     curSig12 = 
		     #include "../hierarchies/h6bq2g2/sigS12Full.inc"
		     ;
		     break;
	       }
	       break;
	    case h9:
	       if(threeLoopFlag == 0){
		  Mst1 = shiftMst1ToMDR(h9, isBottom, twoLoopFlag, 0);
		  Mst2 = shiftMst2ToMDR(h9, isBottom, twoLoopFlag, 0);
	       }
	       else{
		  Mst1 = shiftMst1ToMDR(h9, isBottom, 1, 1);
		  Mst2 = shiftMst2ToMDR(h9, isBottom, 1, 1);
	       }
	       lmMst1 = log(pow2(scale / Mst1));
	       Dmst12 = pow2(Mst1) - pow2(Mst2);
	       Dmsqst1 = pow2(Msq) - pow2(Mst1);
	       switch(tag){
		  case h9:
		     curSig1 = 
		     #include "../hierarchies/h9/sigS1Full.inc"
		     ;
		     curSig2 = 
		     #include "../hierarchies/h9/sigS2Full.inc"
		     ;
		     curSig12 = 
		     #include "../hierarchies/h9/sigS12Full.inc"
		     ;
		     break;
		  case h9q2:
		     curSig1 = 
		     #include "../hierarchies/h9q2/sigS1Full.inc"
		     ;
		     curSig2 = 
		     #include "../hierarchies/h9q2/sigS2Full.inc"
		     ;
		     curSig12 = 
		     #include "../hierarchies/h9q2/sigS12Full.inc"
		     ;
		     break;
	       }
	       break;
	    default:
	       throw std::runtime_error("Hierarchy not included!");
	       break;
	 }
      }
      sigS1Full += curSig1;
      sigS2Full += curSig2;
      sigS12Full += curSig12;
   }
   Eigen::Matrix2d higgsMassMatrix;
   higgsMassMatrix(0, 0) = prefac * sigS1Full;
   higgsMassMatrix(0, 1) = prefac * sigS12Full;
   higgsMassMatrix(1, 0) = higgsMassMatrix(0, 1);
   higgsMassMatrix(1, 1) = prefac * sigS2Full;
   return higgsMassMatrix;
}

/*
 * 	checks if hierarchy is suitable to the given mass spectrum
 */
bool h3m::HierarchyCalculator::isHierarchySuitable(const unsigned int tag, const bool isBottom){
   double Mst1, Mst2;
   if(!isBottom){
      Mst1 = p.MSt(0, 0);
      Mst2 = p.MSt(1, 0);
   }
   else{
      Mst1 = p.MSb(0, 0);
      Mst2 = p.MSb(1, 0);
   }
   switch (tag){
      case h3:
	 return Mgl > Mst2;
      case h32q2g:
	 return (Mst2 >= Msq) && (Mst2 > Mgl);
      case h3q22g:
	 return (Msq > Mst2) && (Mst2 > Mgl);
      case h4:
	 return (Mst1 < Msq) && (Mst1 >= Mgl);
      case h5:
	 return (Mst2 - Mst1 > 0.1*Mst1) && ((Mgl - Mst1) < abs(Mgl - Mst2)) && (Mst2 < Msq) && (Mst1 >= Mgl);
      case h5g1:
	 return (Mst2 - Mst1 > 0.1*Mst1) && ((Mgl - Mst1) < abs(Mgl - Mst2)) && (Mst2 < Msq) && (Mgl > Mst1);
      case h6:
	 return (Mst2 - Mst1 > 0.1*Mst1) && ((Mst2 - Mgl) < abs(Mgl - Mst1)) && (Mst2 < Msq) && (Mst2 >= Mgl);
      case h6g2:
	 return (Mst2 - Mst1 > 0.1*Mst1) && ((Mst2 - Mgl) < abs(Mgl - Mst1)) && (Mst2 < Msq) && (Mgl > Mst2);
      case h6b:
	 return (Mst2 - Mst1 > 0.1*Mst1) && ((Mst2 - Mgl) < abs(Mgl - Mst1)) && (Mst2 >= Msq) && (Mst2 >= Mgl);
      case h6b2qg2:
	 return (Mst2 - Mst1 > 0.1*Mst1) && ((Mst2 - Mgl) < abs(Mgl - Mst1)) && (Mst2 >= Msq) && (Mgl > Mst2);
      case h6bq22g:
	 return (Mst2 - Mst1 > 0.1*Mst1) && ((Mst2 - Mgl) < abs(Mgl - Mst1)) && (Msq > Mst2) && (Mst2 >= Mgl);
      case h6bq2g2:
	 return (Mst2 - Mst1 > 0.1*Mst1) && ((Mst2 - Mgl) < abs(Mgl - Mst1)) && (Msq > Mst2) && (Mgl > Mst2);
      case h9:
	 return (Mst2 >= Msq) && ((Mst2 - Mst1) < (Mst1 - Mgl));
      case h9q2:
	 return (Msq > Mst2) && ((Mst1 - Mst1) < (Mst1 - Mgl));
   }
}

/*
 * 	shifts Mst1/Msb1 according to the hierarchy to the MDRbar scheme, checked
 */
double h3m::HierarchyCalculator::shiftMst1ToMDR(const unsigned int tag, const bool isBottom, const unsigned int twoLoopFlag, const unsigned int threeLoopFlag) {
   double Mst1mod = 0., Mst1, Mst2;
   if(!isBottom){
      Mst1 = p.MSt(0, 0);
      Mst2 = p.MSt(1, 0);
   }
   else{
      Mst1 = p.MSb(0, 0);
      Mst2 = p.MSb(1, 0);
   }
   double lmMst2 = log(pow2(p.scale) / pow2(Mst2));
   double Dmglst2 = Mgl - Mst2;
   double mdr2mst1ka = (-8. * threeLoopFlag * pow2(Al4p) * (10 * pow2(Msq) * (-1 + 2 * lmMsq + 2 * z2) + pow2(Mst2) * (-1 + 2 * lmMst2 + 2 * z2))) / (3. * pow2(Mst1));
   switch (tag) {
   case h3:
      Mst1mod = (1 + mdr2mst1ka);
      break;
   case h4:
      Mst1mod = (1 + mdr2mst1ka);
      break;
   case h5:
      Mst1mod = (1 + mdr2mst1ka);
      break;
   case h6:
      Mst1mod = (144 * twoLoopFlag * Al4p * (1 + lmMgl) * pow2(Mgl) * pow4(Msq) + 27 * (1 + mdr2mst1ka) * pow4(Msq) * pow2(Mst1) +
         threeLoopFlag * pow2(Al4p) * Mgl * (-5 * (67 + 84 * lmMgl - 84 * lmMsq) * pow5(Mgl) - 40 * (43 + 30 * lmMgl - 30 * lmMsq) * pow3(Mgl) * pow2(Msq) +
            288 * Dmglst2 * pow4(Msq) * (1 - 2 * z2) + 12 * Mgl * pow4(Msq) * (79 + 144 * pow2(lmMgl) - 150 * lmMsq +
               90 * pow2(lmMsq) - 90 * lmMgl * (-3 + 2 * lmMsq) + 208 * z2))) / (27. * pow4(Msq) * pow2(Mst1));
      break;
   case h6b:
      Mst1mod = (48 * twoLoopFlag * Al4p * (1 + lmMgl) * pow2(Mgl) + 9 * (1 + mdr2mst1ka) * pow2(Mst1) +
         8 * threeLoopFlag * pow2(Al4p) * (-135 * pow2(Msq) + 12 * Dmglst2 * Mgl * (1 - 22 * z2) +
            pow2(Mgl) * (77 + 135 * lmMgl + 72 * pow2(lmMgl) - 75 * lmMsq -
               90 * lmMgl * lmMsq + 45 * pow2(lmMsq) + 104 * z2))) / (9. * pow2(Mst1));
      break;
   case h9:
      Mst1mod = (1 + mdr2mst1ka);
      break;
   }
   return Mst1 * sqrt(Mst1mod);
}

/*
 * 	shifts Mst2/Msb2 according to the hierarchy to the MDRbar scheme, checked
 */
double h3m::HierarchyCalculator::shiftMst2ToMDR(const unsigned int tag, const bool isBottom, const unsigned int twoLoopFlag, const unsigned int threeLoopFlag) {
   double Mst2mod;
   double Mst1;
   double Mst2;
   if(!isBottom){
      Mst1 = p.MSt(0, 0);
      Mst2 = p.MSt(1, 0);
   }
   else{
      Mst1 = p.MSb(0, 0);
      Mst2 = p.MSb(1, 0);
   }
   double Dmglst2 = Mgl - Mst2;
   double mdr2mst2ka = (-80. * threeLoopFlag * pow2(Al4p) * pow2(Msq) * (-1 + 2 * lmMsq + 2 * z2)) / (3. * pow2(Mst2));
   switch (tag) {
   case h3:
      Mst2mod = (1 + mdr2mst2ka);
      break;
   case h4:
      Mst2mod = (1 + mdr2mst2ka);
      break;
   case h5:
      Mst2mod = (1 + mdr2mst2ka);
      break;
   case h6:
      Mst2mod = (144 * twoLoopFlag * Al4p * (1 + lmMgl) * pow2(Mgl) * pow4(Msq) + 27 * (1 + mdr2mst2ka) * pow4(Msq) * pow2(Mst2) +
         threeLoopFlag * pow2(Al4p) * Mgl * (-5 * (67 + 84 * lmMgl - 84 * lmMsq) * pow5(Mgl) - 40 * (43 + 30 * lmMgl - 30 * lmMsq) * pow3(Mgl) * pow2(Msq) +
            288 * Dmglst2 * pow4(Msq) * (1 - 2 * z2) + 12 * Mgl * pow4(Msq) * (79 + 144 * pow2(lmMgl) - 150 * lmMsq +
               90 * pow2(lmMsq) - 90 * lmMgl * (-3 + 2 * lmMsq) + 208 * z2))) / (27. * pow4(Msq) * pow2(Mst2));
      break;
   case h6b:
      Mst2mod = (48 * twoLoopFlag * Al4p * (1 + lmMgl) * pow2(Mgl) + 9 * (1 + mdr2mst2ka) * pow2(Mst2) +
         8 * threeLoopFlag * pow2(Al4p) * (-135 * pow2(Msq) + 12 * Dmglst2 * Mgl * (1 - 22 * z2) +
            pow2(Mgl) * (77 + 135 * lmMgl + 72 * pow2(lmMgl) - 75 * lmMsq -
               90 * lmMgl * lmMsq + 45 * pow2(lmMsq) + 104 * z2))) / (9. * pow2(Mst2));
      break;
   case h9:
      Mst2mod = (1 + mdr2mst2ka);
      break;
   }
   return Mst2 * sqrt(Mst2mod);
}


/*
 * 	sorts the eigenvalues of a 2x2 matrix and returns them in a vector
 */
std::vector<double> h3m::HierarchyCalculator::sortEigenvalues(const Eigen::EigenSolver<Eigen::Matrix2d> es){
  std::vector<double> sortedEigenvalues;
  sortedEigenvalues.push_back(sqrt(std::real(es.eigenvalues()(0))));
  sortedEigenvalues.push_back(sqrt(std::real(es.eigenvalues()(1))));
  std::sort(sortedEigenvalues.begin(), sortedEigenvalues.end());
  return sortedEigenvalues;
}

/*
 * 	calculates the 1-loop alpha_t/b Higgs mass matrix
 */
Eigen::Matrix2d h3m::HierarchyCalculator::getMt41L(const bool isBottom){
   Eigen::Matrix2d Mt41L;
   double GF = 1/(sqrt(2) * (pow2(p.vu) + pow2(p.vd)));
   double Mst1;
   double Mst2;
   double Mt;
   double s2t;
   const double beta = atan(p.vu/p.vd);
   if(!isBottom){
      Mst1 = p.MSt(0, 0);
      Mst2 = p.MSt(1, 0);
      s2t = p.s2t;
      Mt = p.Mt;
   }
   else{
      Mst1 = p.MSb(0, 0);
      Mst2 = p.MSb(1, 0);
      s2t = p.s2b;
      Mt = p.Mb;
   }
   Mt41L (0,0) = (-3*GF*pow2(Mt)*pow2(p.mu)*pow2(1/sin(beta))*
      (-pow2(Mst1) + pow2(Mst2) + pow2(Mst1)*log(Mst1) + 
        pow2(Mst2)*log(Mst1) - pow2(Mst1)*log(Mst2) - 
        pow2(Mst2)*log(Mst2))*pow2(s2t))/
    (4.*sqrt(2)*(pow2(Mst1) - pow2(Mst2))*pow2(Pi));
   Mt41L (0,1) = (3*GF*pow2(1/sin(beta))*
      (-(pow3(Mt)*p.mu*(log(Mst1) - log(Mst2))*s2t)/2. + 
        (pow2(Mt)*pow2(p.mu)*1/tan(beta)*
           (-pow2(Mst1) + pow2(Mst2) + pow2(Mst1)*log(Mst1) + 
             pow2(Mst2)*log(Mst1) - pow2(Mst1)*log(Mst2) - 
             pow2(Mst2)*log(Mst2))*pow2(s2t))/
         (4.*(pow2(Mst1) - pow2(Mst2))) + 
        (Mt*p.mu*(-pow2(Mst1) + pow2(Mst2) + pow2(Mst1)*log(Mst1) + 
             pow2(Mst2)*log(Mst1) - pow2(Mst1)*log(Mst2) - 
             pow2(Mst2)*log(Mst2))*pow3(s2t))/8.))/
    (sqrt(2)*pow2(Pi));
   Mt41L (1,0) = Mt41L(0,1);
   Mt41L (1,1) =  (3*GF*pow2(1/sin(beta))*
      (pow4(Mt)*(log(Mst1) + log(Mst2) - 2*log(Mt)) + 
        pow3(Mt)*p.mu*1/tan(beta)*(log(Mst1) - log(Mst2))*s2t + 
        (pow2(Mt)*pow2(1/sin(beta))*
           (pow2(Mst1)*pow2(p.mu)*pow2(cos(beta)) - 
             pow2(Mst2)*pow2(p.mu)*pow2(cos(beta)) - 
             pow2(Mst1)*pow2(p.mu)*pow2(cos(beta))*log(Mst1) - 
             pow2(Mst2)*pow2(p.mu)*pow2(cos(beta))*log(Mst1) + 
             pow2(Mst1)*pow2(p.mu)*pow2(cos(beta))*log(Mst2) + 
             pow2(Mst2)*pow2(p.mu)*pow2(cos(beta))*log(Mst2) + 
             2*pow4(Mst1)*log(Mst1)*pow2(sin(beta)) - 
             4*pow2(Mst1)*pow2(Mst2)*log(Mst1)*pow2(sin(beta)) + 
             2*pow4(Mst2)*log(Mst1)*pow2(sin(beta)) - 
             2*pow4(Mst1)*log(Mst2)*pow2(sin(beta)) + 
             4*pow2(Mst1)*pow2(Mst2)*log(Mst2)*pow2(sin(beta)) - 
             2*pow4(Mst2)*log(Mst2)*pow2(sin(beta)))*pow2(s2t))/
         (4.*(pow2(Mst1) - pow2(Mst2))) - 
        (Mt*p.mu*1/tan(beta)*(-pow2(Mst1) + pow2(Mst2) + 
             pow2(Mst1)*log(Mst1) + pow2(Mst2)*log(Mst1) - 
             pow2(Mst1)*log(Mst2) - pow2(Mst2)*log(Mst2))*
           pow3(s2t))/4. - 
        ((pow2(Mst1) - pow2(Mst2))*
           (-pow2(Mst1) + pow2(Mst2) + pow2(Mst1)*log(Mst1) + 
             pow2(Mst2)*log(Mst1) - pow2(Mst1)*log(Mst2) - 
             pow2(Mst2)*log(Mst2))*pow4(s2t))/16.))/
    (sqrt(2)*pow2(Pi));
    return Mt41L;
}

/*
 * 	calculates the DRbar -> MDRbar shift for the one loop contribution
 */
Eigen::Matrix2d h3m::HierarchyCalculator::getShift(const int tag, const bool isBottom){
   Eigen::Matrix2d shift;
   double GF = 1/(sqrt(2) * (pow2(p.vu) + pow2(p.vd)));
   double Mst1;
   double Mst2;
   double Mt;
   double s2t;
   const double beta = atan(p.vu/p.vd);
   double deltamst1, deltamst2;
   if(!isBottom){
      Mst1 = p.MSt(0, 0);
      Mst2 = p.MSt(1, 0);
      s2t = p.s2t;
      Mt = p.Mt;
      deltamst1 = -(Mst1 - shiftMst1ToMDR(tag, false, 1, 0));
      deltamst2 = -(Mst2 - shiftMst2ToMDR(tag, false, 1, 0));
   }
   else{
      Mst1 = p.MSb(0, 0);
      Mst2 = p.MSb(1, 0);
      s2t = p.s2b;
      Mt = p.Mb;
      deltamst1 = -(Mst1 - shiftMst1ToMDR(tag, true, 1, 0));
      deltamst2 = -(Mst2 - shiftMst2ToMDR(tag, true, 1, 0));
   }
   shift(0, 0) = (3 * GF * (deltamst2 * Mst1 - deltamst1 * Mst2) * pow2(Mt) * pow2(p.mu) * pow2(1 / Pi) *
      pow2(1/sin(beta)) * pow2(1 / (pow2(Mst1) - pow2(Mst2))) * pow2(s2t) *
      (4 * (log(Mst1) - log(Mst2)) * pow2(Mst1) * pow2(Mst2) - pow4(Mst1) +
	 pow4(Mst2))) / (4. * sqrt(2) * Mst1 * Mst2);
   shift(0, 1) = (-3 * GF * Mt * p.mu * pow2(1 / Pi) * pow2(1/sin(beta)) *
      pow2(1 / (pow2(Mst1) - pow2(Mst2))) * s2t *
      (-(pow2(pow2(Mst1) - pow2(Mst2)) *
	    (4 * (-(deltamst2 * Mst1) + deltamst1 * Mst2) * pow2(Mt) +
	       (-2 * Mst1 * Mst2 * (deltamst1 * Mst1 + deltamst2 * Mst2) *
		  (log(Mst1) - log(Mst2)) +
		  (deltamst2 * Mst1 + deltamst1 * Mst2) * pow2(Mst1) -
		  (deltamst2 * Mst1 + deltamst1 * Mst2) * pow2(Mst2)) * pow2(s2t))) + 2 * (deltamst2 * Mst1 - deltamst1 * Mst2) * Mt * p.mu * 1/tan(beta) *
	 (4 * (log(Mst1) - log(Mst2)) * pow2(Mst1) * pow2(Mst2) - pow4(Mst1) +
	    pow4(Mst2)) * s2t)) / (8. * sqrt(2) * Mst1 * Mst2);
   shift(1, 0) = shift(0, 1);
   shift(1, 1) = (3 * GF * pow2(1 / Pi) * pow2(1/sin(beta)) *
      ((Mt * p.mu * 1/tan(beta) * (-(deltamst1 * Mst1) + deltamst2 * Mst2 +
	       2 * deltamst1 * Mst1 * log(Mst1) + 2 * deltamst2 * Mst2 * log(Mst1) -
	       2 * deltamst1 * Mst1 * log(Mst2) - 2 * deltamst2 * Mst2 * log(Mst2) -
	       (deltamst2 * pow2(Mst1)) / Mst2 + (deltamst1 * pow2(Mst2)) / Mst1) *
	    pow3(s2t)) / 4. + (pow2(Mt) * pow2(1 / (pow2(Mst1) - pow2(Mst2))) *
	    pow2(s2t) * (2 * deltamst2 * pow7(Mst1) -
	       2 * deltamst1 * pow6(Mst1) * Mst2 -
	       2 * deltamst2 * Mst1 * pow6(Mst2) + 2 * deltamst1 * pow7(Mst2) -
	       4 * deltamst1 * pow6(Mst1) * Mst2 * log(Mst1) +
	       4 * deltamst2 * Mst1 * pow6(Mst2) * log(Mst1) +
	       4 * deltamst1 * pow6(Mst1) * Mst2 * log(Mst2) -
	       4 * deltamst2 * Mst1 * pow6(Mst2) * log(Mst2) -
	       deltamst2 * pow5(Mst1) * pow2(p.mu) * pow2(1/tan(beta)) -
	       deltamst1 * pow5(Mst2) * pow2(p.mu) * pow2(1/tan(beta)) +
	       2 * deltamst2 * pow2(Mst2) *
	       (pow5(Mst1) * (-3 + 2 * log(Mst1) - 2 * log(Mst2)) +
		  2 * (log(Mst1) - log(Mst2)) * pow2(p.mu) * pow2(1/tan(beta)) *
		  pow3(Mst1)) - 2 * deltamst1 * pow2(Mst1) *
	       (pow5(Mst2) * (3 + 2 * log(Mst1) - 2 * log(Mst2)) +
		  2 * (log(Mst1) - log(Mst2)) * pow2(p.mu) * pow2(1/tan(beta)) *
		  pow3(Mst2)) + deltamst1 * Mst2 * pow2(p.mu) * pow2(1/tan(beta)) *
	       pow4(Mst1) + 6 * deltamst1 * pow3(Mst2) * pow4(Mst1) +
	       8 * deltamst1 * log(Mst1) * pow3(Mst2) * pow4(Mst1) -
	       8 * deltamst1 * log(Mst2) * pow3(Mst2) * pow4(Mst1) +
	       deltamst2 * Mst1 * pow2(p.mu) * pow2(1/tan(beta)) * pow4(Mst2) +
	       6 * deltamst2 * pow3(Mst1) * pow4(Mst2) -
	       8 * deltamst2 * log(Mst1) * pow3(Mst1) * pow4(Mst2) +
	       8 * deltamst2 * log(Mst2) * pow3(Mst1) * pow4(Mst2))) / (4. * Mst1 * Mst2) -
	 ((deltamst2 * Mst1 + deltamst1 * Mst2) * pow4(Mt)) / (Mst1 * Mst2) -
	 ((deltamst2 * pow5(Mst1) + deltamst1 * pow5(Mst2) -
	    4 * deltamst2 * pow2(Mst2) * pow3(Mst1) -
	    4 * deltamst1 * pow2(Mst1) * pow3(Mst2) +
	    3 * deltamst1 * Mst2 * pow4(Mst1) -
	    4 * deltamst1 * Mst2 * log(Mst1) * pow4(Mst1) +
	    4 * deltamst1 * Mst2 * log(Mst2) * pow4(Mst1) +
	    3 * deltamst2 * Mst1 * pow4(Mst2) +
	    4 * deltamst2 * Mst1 * log(Mst1) * pow4(Mst2) -
	    4 * deltamst2 * Mst1 * log(Mst2) * pow4(Mst2)) * pow4(s2t)) /
	 (16. * Mst1 * Mst2) + (-(deltamst1 / Mst1) + deltamst2 / Mst2) * p.mu *
	 1/tan(beta) * pow3(Mt) * s2t)) / sqrt(2);
   return shift;
}


/*
 * 	calculates the 2-loop higgs mass matrix according to Pietro Slavich
 */
Eigen::Matrix2d h3m::HierarchyCalculator::getMt42L(const int tag, const bool isBottom){
   Eigen::Matrix2d Mt42L;
   double S11, S12, S22;
   double Mt2;
   double MG = p.MG;
   double Mst12;
   double Mst22;
   double st;
   double ct;
   if(!isBottom){
      const double theta = asin(p.s2t)/2.;
      Mt2 = pow2(p.Mt);
      Mst12 = pow2(shiftMst1ToMDR(tag, false, 1, 0));
      Mst22 = pow2(shiftMst2ToMDR(tag, false, 1, 0) + deltaDSZ);
      st = sin(theta);
      ct = cos(theta);
   }
   else{
      const double theta = asin(p.s2b)/2.;
      Mt2 = pow2(p.Mb);
      Mst12 = pow2(shiftMst1ToMDR(tag, true, 1, 0));
      Mst22 = pow2(shiftMst2ToMDR(tag, true, 1, 0) + deltaDSZ);
      st = sin(theta);
      ct = cos(theta);
   }
   double scale2 = pow2(p.scale);
   // note the sign difference in mu
   double mu = - p.mu;
   double tanb = p.vu/p.vd;
   double v2 = pow2(p.vu) + pow2(p.vd);
   double gs = p.g3;
   int os = 0;
   DSZHiggs_(&Mt2, &MG, &Mst12, &Mst22, &st, &ct, &scale2, &mu, &tanb, &v2, &gs, &os, &S11, &S22, &S12);
   Mt42L(0, 0) = S11;
   Mt42L(1, 0) = S12;
   Mt42L(0, 1) = S12;
   Mt42L(1, 1) = S22;
   return Mt42L;
}
