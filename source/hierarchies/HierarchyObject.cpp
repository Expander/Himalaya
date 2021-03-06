// ====================================================================
// This file is part of Himalaya.
//
// Himalaya is licenced under the GNU General Public License (GNU GPL)
// version 3.
// ====================================================================

#include "HierarchyObject.hpp"
#include "Hierarchies.hpp"
#include <cmath>
#include <iostream>

/**
 * 	A constructor.
 * 	@param isAlphab the boolean which determines wether the members are proportinal to alpha_b or alpha_t.
 */
himalaya::HierarchyObject::HierarchyObject(bool isAlphab)
   : isAlphab(isAlphab)
{
}

/**
 * 	@return The value of isAlphab.
 */
bool himalaya::HierarchyObject::getIsAlphab() const{
   return isAlphab;
}

/**
 * 	Sets the suitable hierarchy
 * 	@param hierarchy the integer key of the hierarchy.
 */
void himalaya::HierarchyObject::setSuitableHierarchy(int hierarchy){
   this -> hierarchy = hierarchy;
}

/**
 * 	@return The key to the suitable hierarchy
 */
int himalaya::HierarchyObject::getSuitableHierarchy() const{
   return hierarchy;
}

/**
 * 	Sets the absolute difference of the Higgs masses at two-loop level
 * 	@param absDiff2L the absolute difference of the Higgs masses as a double.
 */
void himalaya::HierarchyObject::setAbsDiff2L(double absDiff2L){
   this -> absDiff2L = absDiff2L;
}

/**
 * 	@return The absolute difference of the exact and expanded Higgs masses at two-loop level at the order O(alpha_x + alpha_x*alpha_s).
 */
double himalaya::HierarchyObject::getAbsDiff2L() const{
   return absDiff2L;
}

/**
 * 	Sets the relative difference ot the Higgs masses at two-loop level
 * 	@param relDiff2L the relative difference of the Higgs masses as a double.
 */
void himalaya::HierarchyObject::setRelDiff2L(double relDiff2L){
   this -> relDiff2L = relDiff2L;
}

/**
 * 	@return The relative difference of the exact and expanded Higgs masses at two-loop level at the order O(alpha_x + alpha_x*alpha_s).
 */
double himalaya::HierarchyObject::getRelDiff2L() const{
   return relDiff2L;
}


/**
 * 	Sets the uncertainty of the expansion at a given loop level.
 * 	@param loops the integer value of the corresponding loops. Can be 1, 2 or 3.
 * 	@param uncertainty the expansion untertainty at the given loop order as a double.
 */
void himalaya::HierarchyObject::setDMhExpUncertainty(int loops, double uncertainty){
   if(loops > 0 && loops <= 3){
      expUncertainties[loops] = uncertainty;
   }
   else {
      throw std::runtime_error("Expansion uncertainty for " + std::to_string(loops) + " loop(s) is not available.");
   }
}

/**
 * 	@param loops an integer which can be 1, 2 or 3.
 * 	@return A double which is the expansion uncertainty for the given loop order.
 */
double himalaya::HierarchyObject::getDMhExpUncertainty(int loops) const{
   if(loops > 0 && loops <= 3){
      return expUncertainties.at(loops);
   }
   
   throw std::runtime_error("Expansion uncertainty for " + std::to_string(loops) + " loop(s) is not available.");
   
}

/**
 * 	Sets the delta of the CP-even Higgs mass matrix
 * 	@param loops the integer value of the corresponding loops. Can be 0, 1, 2 or 3. 0 corresponds to the tree-level.
 * 	@param dMh the delta of the mass matrix.
 */
void himalaya::HierarchyObject::setDMh(int loops, const Eigen::Matrix2d& dMh){
   if(loops >= 0 && loops <= 3){
      dMhMap[loops] = dMh;
   }
   else {
      throw std::runtime_error("Higgs mass matrix for " + std::to_string(loops) + " loop(s) is not available.");
   }
}

/**
 * 	@param loops an integer which can be 0, 1, 2, 3. Here, 0 corresponds to the tree-level matrix.
 * 	@return The CP-even Higgs mass matrix at the given loop order.
 */
Eigen::Matrix2d himalaya::HierarchyObject::getDMh(int loops) const{
   if(loops >= 0 && loops <= 3){
      return dMhMap.at(loops);
   }
   
   throw std::runtime_error("Higgs mass matrix for " + std::to_string(loops) + " loop(s) is not available.");
}

/**
 * 	Sets the DR' -> MDR' shift
 * 	@param mdrShift the DR' -> MDR' shiftet matrix of the form M(MDR') - M(DR').
 */
void himalaya::HierarchyObject::setDMhDRbarPrimeToMDRbarPrimeShift(const Eigen::Matrix2d& mdrShift){
   this -> mdrShift = mdrShift;
}

/**
 * 	@return The matrix M(MDR') - M(DR') at the order O(alpha_x + alpha_x*alpha_s)
 */
Eigen::Matrix2d himalaya::HierarchyObject::getDMhDRbarPrimeToMDRbarPrimeShift() const{
   return mdrShift;
}

void himalaya::HierarchyObject::setDMhDRbarPrimeToH3mShift(const Eigen::Matrix2d shift){
   h3mShift = shift;
}

Eigen::Matrix2d himalaya::HierarchyObject::getDMhDRbarPrimeToH3mShift() const{
   return h3mShift;
}


/**
 * 	Sets the MDR masses
 * 	@param mdrMasses a vector containting the MDR masses with the lightest particle at position 0.
 */
void himalaya::HierarchyObject::setMDRMasses(Eigen::Vector2d& mdrMasses){
   this -> mdrMasses = sortVector(mdrMasses);
}

/**
 * 	@return A vector of the MDR stop/sbottom masses. The 0th entry corresponds to the lighter particle.
 */
Eigen::Vector2d himalaya::HierarchyObject::getMDRMasses() const{
   return mdrMasses;
}

/**
 * 	Sets the mdrFlag to calculate the corretions in without MDR (0) or with MDR (1) shifts.
 * 	@param mdrFlag an int. (0) for H3m (DR')- and (1) for MDR-scheme.
 * 	@throws runtime_exception if the flag is neither 0 or 1 an exception is thrown.
 */
void himalaya::HierarchyObject::setMDRFlag(int mdrFlag){
   if(mdrFlag != 0 && mdrFlag != 1) {
      throw std::runtime_error("The MDR-flag has to be 0 (DR-scheme) or 1 (MDR-scheme). Input: " + std::to_string(mdrFlag) + ".");
}
   this -> mdrFlag = mdrFlag;
}


/**
 * 	@return the MDRFlag integer.
 */
int himalaya::HierarchyObject::getMDRFlag() const{
   return mdrFlag;
}

/**
 * 	Sets the renormalization scheme accodring to the RenScheme enum
 * 	@param renScheme an int according to the RenScheme enum.
 * 	@throws runtime_exception if the flag is not in {0,1,2,3} an exception is thrown
 */
void himalaya::HierarchyObject::setRenormalizationScheme(int renScheme){
   if(renScheme < 0 || renScheme > 3) {
      throw std::runtime_error("The renormalization scheme has to be 0 (H3m), 1 (DR'), 2 (H3m with MDR), 3 (MDR'). Input: " + std::to_string(renScheme) + ".");
   }
   renormalizationScheme = renScheme;
}

/**
 * 	@return Renormalization scheme key
 */
int himalaya::HierarchyObject::getRenormalizationScheme() const{
   return renormalizationScheme;
}


/**
 * 	Sets the delta_lambda at 3-loop order with H3m logs.
 * 	@param deltaLambda delta_lambda at 3-loop order.
 */
void himalaya::HierarchyObject::setDLambdaH3m(double deltaLambda){
   this -> dLambdaH3m = deltaLambda;
}

/**
 * 	@return 3-loop delta_lambda with H3m logs
 */
double himalaya::HierarchyObject::getDLambdaH3m() const{
   return dLambdaH3m;
}

/**
 * 	Sets the delta_lambda at 3-loop order with EFT logs.
 * 	@param deltaLambda delta_lambda at 3-loop order.
 */
void himalaya::HierarchyObject::setDLambdaEFT(double deltaLambda){
   this -> dLambdaEFT = deltaLambda;
}

/**
 * 	@return 3-loop delta_lambda with EFT logs
 */
double himalaya::HierarchyObject::getDLambdaEFT() const{
   return dLambdaEFT;
}

/**
 * 	Sets the constant part of delta_lambda at 3-loop order.
 * 	@param deltaLambda constant part of delta_lambda at 3-loop order.
 */
void himalaya::HierarchyObject::setDLambdaNonLog(double deltaLambda){
   dLambdaNonLog = deltaLambda;
}

/**
 *        @return 3-loop delta_lambda for the degenerated mass case with EFT logs
 */
double himalaya::HierarchyObject::getDLambdaNonLog() const{
   return dLambdaNonLog;
}

/**
 * 	Sets the Xt parts of the uncertainty of delta_lambda_H3m
 *        @param uncertainty of 3-loop delta_lambda
 */
void himalaya::HierarchyObject::setDLambdaH3mXtUncertainty(double uncertainty){
   dLambdaH3mXtUncertainty = uncertainty;
}

/**
 *        @return uncertainty of 3-loop delta_lambda_H3m
 */
double himalaya::HierarchyObject::getDLambdaH3mUncertainty() const {
   return std::abs(getDLambdaEFT() - getDLambdaH3m()) 
      + std::abs(dLambdaH3mXtUncertainty)
      + std::abs(dLambdaExpansionUncertainty);
}

/**
 * 	Sets the Xt parts of the uncertainty of delta_lambda_EFT
 *      @param uncertainty of 3-loop delta_lambda
 */
void himalaya::HierarchyObject::setDLambdaEFTXtUncertainty(double uncertainty){
   dLambdaEFTXtUncertainty = uncertainty;
}

/**
 *	@return uncertainty of 3-loop delta_lambda_EFT
 *	TODO: the difference should only be taken up to O(Xt^3/Xt^4) depending on the chosen hierarchy
 */
double himalaya::HierarchyObject::getDLambdaEFTUncertainty() const {
   return std::abs(dLambdaEFTXtUncertainty)
      + std::abs(dLambdaExpansionUncertainty);
}

/**
 * 	Sets the DR' -> MS shift for delta_lambda_himalaya which should be added to the DR' result
 * 	@param shift the DR' -> MS shift which should be added to the 3-loop threshold correction
 */
void himalaya::HierarchyObject::setDLambdaH3mDRbarPrimeToMSbarShift(double shift){
   dLambdaH3mdrBarPrimeToMSbarShift = shift;
}

/**
 * 	@return the DR' -> MS shift for delta_lambda_himalaya which should be added to the DR' result
 */
double himalaya::HierarchyObject::getDLambdaH3mDRbarPrimeToMSbarShift() const{
   return dLambdaH3mdrBarPrimeToMSbarShift;
}

/**
 * 	Sets the DR' -> MS shift for delta_lambda_EFT which should be added to the DR' result
 * 	@param shift the DR' -> MS shift which should be added to the 3-loop threshold correction
 */
void himalaya::HierarchyObject::setDLambdaEFTDRbarPrimeToMSbarShift(double shift){
   dLambdaEFTdrBarPrimeToMSbarShift = shift;
}

/**
 * 	@return the DR' -> MS shift for delta_lambda_EFT which should be added to the DR' result
 */
double himalaya::HierarchyObject::getDLambdaEFTDRbarPrimeToMSbarShift() const{
   return dLambdaEFTdrBarPrimeToMSbarShift;
}

/**
 * 	Set the expasion uncertainty for delta_lambda
 * 	@param expUncertLambda the expansion uncertainty for delta_lambda
 */
void himalaya::HierarchyObject::setDLambdaExpUncertainty(double expUncertLambda){
   dLambdaExpansionUncertainty = expUncertLambda;
}


/**
 * 	Sorts a vector.
 * 	@param vector The vector which should be sorted.
 * 	@return Returns a vector the lightest entry at position 0.
 */
Eigen::Vector2d himalaya::HierarchyObject::sortVector(Eigen::Vector2d& vector){
   // checks if all variables are ordered in the right way
   if (vector(0) > vector(1)) {
      std::swap(vector(0), vector(1));
   }
   return vector;
}

/**
 * 	@return delta_lambda
 * 	@param loops an integer, could be 0 (tree), 1 (1L), ..., 3 (3L Delta_lambda_EFT)
 */
double himalaya::HierarchyObject::getDLambda(int loops) const{
   if(loops >= 0 && loops <= 3){
      return dLambdaMap.at(loops);
   }
   
   throw std::runtime_error("Δλ for " + std::to_string(loops) + " loop(s) is not available.");
}

/**
 * 	Sets the Delta_lambda at loops-loop
 * 	@param loops an integer, could be 0 (tree), ..., 3 (3L Delta_lambda_EFT)
 * 	@param deltaLambda delta_lambda at tree-level
 */
void himalaya::HierarchyObject::setDLambda(int loops, double deltaLambda){
   if(loops >= 0 && loops <= 3){
      dLambdaMap[loops] = deltaLambda;
   }
   else {
      throw std::runtime_error("Δλ for " + std::to_string(loops) + " loop(s) is not available.");
   }
}

/**
 * 	@return Delta_Mh2_EFT
 * 	@param loops an integer, could be 0 (tree), 1 (1L), ..., 3 (3L with Delta_lambda_EFT)
 */
double himalaya::HierarchyObject::getDMh2EFT(int loops) const{
   if(loops >= 0 && loops <= 3){
      return dMh2EFTMap.at(loops);
   }
   
   throw std::runtime_error("Higgs mass for " + std::to_string(loops) + " loop(s) is not available.");
}

/**
 * 	Sets Delta_Mh2_EFT at loops-loop
 * 	@param loops an integer, could be 0 (tree), ..., 3 (3L with Delta_lambda_EFT)
 * 	@param deltaMh2 delta_Mh^2
 */
void himalaya::HierarchyObject::setDMh2EFT(int loops, double deltaMh2){
   if(loops >= 0 && loops <= 3){
      dMh2EFTMap[loops] = deltaMh2;
   }
   else {
      throw std::runtime_error("Higgs mass for " + std::to_string(loops) + " loop(s) is not available.");
   }
}

/**
 * 	@return shift to convert delta_lambda from DR' to MS scheme. This shift has to be added to the 1L result
 * 	@param loops an integer, could be 0 (tree), 1 (1L), ..., 3 (3L Delta_lambda_EFT)
 */
double himalaya::HierarchyObject::getDLambdaDRbarPrimeToMSbarShift(int loops) const{
   if(loops >= 0 && loops <= 3){
      return dLambdaDRbarPrimeToMSbarShiftMap.at(loops);
   }
   
   throw std::runtime_error("Δ_DR' -> MS shift for " + std::to_string(loops) + " loop(s) is not available.");
}

/**
 * 	Sets the DR' to MS shift for delta_lambda at loops-loop
 * 	@param loops an integer, could be 0(tree), ..., 3 (3L Delta_lambda_EFT shift)
 * 	@param shift the shift
 */
void himalaya::HierarchyObject::setDLambdaDRbarPrimeToMSbarShift(int loops, double shift){
   if(loops >= 0 && loops <= 3){
      dLambdaDRbarPrimeToMSbarShiftMap[loops] = shift;
   }
   else {
      throw std::runtime_error("Δ_DR' -> MS shift for " + std::to_string(loops) + " loop(s) is not available.");
   }
}

/**
 *      Returns the H3m notation of a given hierarchy.
 *      @param hierarchy An integer of a Himalaya hierarchy.
 *      @return Returns the corresponding H3m notation of the given hierarchy as a string.
 */
std::string himalaya::HierarchyObject::getH3mHierarchyNotation(int hierarchy) const{
   switch (hierarchy){
      case Hierarchies::h3:
	 return "h3";
      case Hierarchies::h32q2g:
	 return "h32q2g";
      case Hierarchies::h3q22g:
	 return "h3q22g";
      case Hierarchies::h4:
	 return "h4";
      case Hierarchies::h5:
	 return "h5";
      case Hierarchies::h5g1:
	 return "h5g1";
      case Hierarchies::h6:
	 return "h6";
      case Hierarchies::h6b:
	 return "h6b";
      case Hierarchies::h6b2qg2:
	 return "h6b2qg2";
      case Hierarchies::h6bq22g:
	 return "h6bq22g";
      case Hierarchies::h6bq2g2:
	 return "h6bq2g2";
      case Hierarchies::h6g2:
	 return "h6g2";
      case Hierarchies::h9:
	 return "h9";
      case Hierarchies::h9q2:
	 return "h9q2";
      default:
	 return "Hierarchy " + std::to_string(hierarchy) + " not included";
   }
}

/**
 * 	Prints out all information of the HierarchyObject
 */
std::ostream& himalaya::operator<<(std::ostream& ostr, himalaya::HierarchyObject const &ho){
   const int suitableHierarchy = ho.getSuitableHierarchy();
   const std::string renSchemeString = (ho.getRenormalizationScheme() == RenSchemes::H3m 
      || ho.getRenormalizationScheme() == RenSchemes::H3mMDRBAR) ? "H3m scheme" : "DR'";
   const std::string massString = ho.getIsAlphab() ? "Msbottom" : "Mstop";
   const std::string spaces = ho.getIsAlphab() == true ? "                " : "                   ";
   ostr << "===================================\n"
	<< "Himalaya HierarchyObject parameters\n"
        << "===================================\n"
	<< "Ren. scheme               =  " << renSchemeString << "\n"
        << "Hierarchy                 =  " << suitableHierarchy << " (" << ho.getH3mHierarchyNotation(suitableHierarchy) << ")\n"
	<< massString << "_1" << spaces << "=  " << ho.getMDRMasses()(0) << " GeV (MDR')\n"
	<< massString << "_2" << spaces << "=  " << ho.getMDRMasses()(1) << " GeV (MDR')\n"
        << "Abs. diff 2L              =  " << ho.getAbsDiff2L() << " GeV\n"
        << "Rel. diff 2L              =  " << ho.getRelDiff2L()*100 << " %\n"
        << "Mh^2_0L                   =  {{" << ho.getDMh(0).row(0)(0) << ", " << ho.getDMh(0).row(0)(1)
		   << "}, {" << ho.getDMh(0).row(1)(0) << ", " << ho.getDMh(0).row(1)(1) << "}} GeV^2\n"
        << "ΔMh^2_1L                  =  {{" << ho.getDMh(1).row(0)(0) << ", " << ho.getDMh(1).row(0)(1)
		   << "}, {" << ho.getDMh(1).row(1)(0) << ", " << ho.getDMh(1).row(1)(1) << "}} GeV^2\n"
        << "ΔMh^2_2L                  =  {{" << ho.getDMh(2).row(0)(0) << ", " << ho.getDMh(2).row(0)(1)
		   << "}, {" << ho.getDMh(2).row(1)(0) << ", " << ho.getDMh(2).row(1)(1) << "}} GeV^2\n"
        << "ΔMh^2_3L                  =  {{" << ho.getDMh(3).row(0)(0) << ", " << ho.getDMh(3).row(0)(1)
		   << "}, {" << ho.getDMh(3).row(1)(0) << ", " << ho.getDMh(3).row(1)(1) << "}} GeV^2\n"
        << "Exp. uncert. 1L           =  " << ho.getDMhExpUncertainty(1) << " GeV\n"
        << "Exp. uncert. 2L           =  " << ho.getDMhExpUncertainty(2) << " GeV\n"
        << "Exp. uncert. 3L           =  " << ho.getDMhExpUncertainty(3) << " GeV\n"
	<< "DR' -> MDR' shift         =  {{" << ho.getDMhDRbarPrimeToMDRbarPrimeShift().row(0)(0) << ", " << ho.getDMhDRbarPrimeToMDRbarPrimeShift().row(0)(1)
		   << "}, {" << ho.getDMhDRbarPrimeToMDRbarPrimeShift().row(1)(0) << ", " << ho.getDMhDRbarPrimeToMDRbarPrimeShift().row(1)(1)  << "}} GeV^2\n"
	<< "DR' -> H3m shift          =  {{" << ho.getDMhDRbarPrimeToH3mShift().row(0)(0) << ", " << ho.getDMhDRbarPrimeToH3mShift().row(0)(1)
		   << "}, {" << ho.getDMhDRbarPrimeToH3mShift().row(1)(0) << ", " << ho.getDMhDRbarPrimeToH3mShift().row(1)(1) << "}} GeV^2\n"
	<< "Δλ_0L                     =  " << ho.getDLambda(0) << " O(g_1^2, g_2^2)\n"
	<< "Δλ_1L                     =  " << ho.getDLambda(1) << " O(α_t)\n"
	<< "Δλ_2L                     =  " << ho.getDLambda(2) << " O(α_t*α_s)\n"
        << "Δλ_EFT_3L                 =  " << ho.getDLambdaEFT() << " +/- " << ho.getDLambdaEFTUncertainty() << " O(α_t*α_s^2)\n"
	<< "Δλ_H3m_3L                 =  " << ho.getDLambdaH3m() << " +/- " << ho.getDLambdaH3mUncertainty() << " O(α_t*α_s^2)\n"
	<< "Δλ_0L DR' -> MS shift     =  " << ho.getDLambdaDRbarPrimeToMSbarShift(0) << "\n"
	<< "Δλ_1L DR' -> MS shift     =  " << ho.getDLambdaDRbarPrimeToMSbarShift(1) << "\n"
	<< "Δλ_2L DR' -> MS shift     =  " << ho.getDLambdaDRbarPrimeToMSbarShift(2) << "\n"
	<< "Δλ_EFT_3L DR' -> MS shift =  " << ho.getDLambdaEFTDRbarPrimeToMSbarShift() << "\n"
	<< "Δλ_H3m_3L DR' -> MS shift =  " << ho.getDLambdaH3mDRbarPrimeToMSbarShift() << "\n"
	<< "Mh^2_EFT_0L               =  " << ho.getDMh2EFT(0) << " GeV^2 O(g_1^2, g_2^2)\n"
	<< "ΔMh^2_EFT_1L              =  " << ho.getDMh2EFT(1) << " GeV^2 O(α_t)\n"
	<< "ΔMh^2_EFT_2L              =  " << ho.getDMh2EFT(2) << " GeV^2 O(α_t*α_s)\n"
	<< "ΔMh^2_EFT_3L              =  " << ho.getDMh2EFT(3) << " GeV^2 O(α_t*α_s^2)"
        << '\n';

   return ostr;
}

