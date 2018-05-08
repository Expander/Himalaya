#include "doctest.h"
#include "HierarchyCalculator.hpp"
#include "Mh2EFTCalculator.hpp"

#define CHECK_CLOSE(a,b,eps) CHECK((a) == doctest::Approx(b).epsilon(eps))

namespace {

const double Pi = 3.141592653589793;

double pow2(double x) { return x*x; }
double pow4(double x) { return x*x*x*x; }
double pow6(double x) { return x*x*x*x*x*x; }

himalaya::Parameters make_point()
{
   const double MS = 100000.;
   const double xt = 2.;
   const double tb = 5.;
   const double beta = std::atan(tb);
   const double v = 245.;

   const double msq2 = MS/2;
   const double msu2 = MS*2;
   const double mg   = MS*3;

   const double Xt = xt * std::sqrt(std::sqrt(msq2 * msu2));

   himalaya::Parameters pars;
   pars.scale = MS;
   pars.mu    = MS;
   pars.g3    = 1.05733;
   pars.vu    = v*std::sin(beta);
   pars.vd    = v*std::cos(beta);
   pars.mq2   << pow2(msq2), 0, 0, 0, pow2(msq2), 0, 0, 0, pow2(msq2);
   pars.md2   << pow2(MS), 0, 0, 0, pow2(MS), 0, 0, 0, pow2(MS);
   pars.mu2   << pow2(msu2), 0, 0, 0, pow2(msu2), 0, 0, 0, pow2(msu2);
   pars.At    = Xt + pars.mu/tb;
   pars.Ab    = 0;
   pars.MG    = mg;
   pars.MW    = 74.597;
   pars.MZ    = 85.7704;
   pars.Mt    = 144.337;
   pars.Mb    = 2.37054;
   pars.MA    = MS;

   return pars;
}

// calculates y_t in the SM
double calc_gt(const himalaya::Parameters& pars)
{
   const double v = std::sqrt(pow2(pars.vu) + pow2(pars.vd));
   return std::sqrt(2.)*pars.Mt/v;
}

/// calculates lightest mass eigenvalue of given mass matrix
double calc_Mh2(const Eigen::Matrix2d& Mh_mat)
{
   const Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> es(Mh_mat);
   return es.eigenvalues()(0);
}

/// calculates Mh^2 in the EFT at tree level
double calc_Mh2_EFT_0L(const himalaya::Parameters& pars)
{
   const double beta = std::atan(pars.vu/pars.vd);
   const double cos_2beta = std::cos(2*beta);

   return pow2(pars.MZ * cos_2beta);
}

/// calculates Mh^2 in the EFT at 1-loop level
double calc_Mh2_EFT_1L(const himalaya::Parameters& pars)
{
   const double MR2 = pow2(pars.scale);

   himalaya::mh2_eft::Mh2EFTCalculator mhc(pars, MR2);

   return mhc.getDeltaMh2EFT1Loop(1,1);
}

/// calculates Mh^2 in the EFT at 2-loop level
double calc_Mh2_EFT_2L(const himalaya::Parameters& pars)
{
   const double MR2 = pow2(pars.scale);

   himalaya::mh2_eft::Mh2EFTCalculator mhc(pars, MR2);

   return mhc.getDeltaMh2EFT2Loop(1,1);
}

/// calculates Mh^2 in the EFT at 3-loop level
double calc_Mh2_EFT_3L(const himalaya::Parameters& pars,
                       double zeta_lambda_3L)
{
   const double mt = pars.Mt;
   const double MR2 = pow2(pars.scale);
   const double g3 = pars.g3;
   const double gt  = calc_gt(pars);
   const double v2  = pow2(pars.vu) + pow2(pars.vd);
   const double v   = std::sqrt(v2);   // ~ 245 GeV
   const double vDO = v/std::sqrt(2.); // ~ 173 GeV

   himalaya::mh2_eft::Mh2EFTCalculator mhc(pars, MR2);

   // only logs
   const double DMh2_EFT_3L_logs =
      mhc.getDeltaMh2EFT3Loop(1,1) - mhc.getDeltaMh2EFT3Loop(0,0);

   const double as = pow2(g3)/(4*pow2(Pi));
   // prefactor from paper
   const double pref = pow4(mt)/pow2(4*Pi*vDO)*pow2(as);
   // prefactor from HSSUSY
   const double pref2 = 8*pow4(gt*g3)/pow6(4*Pi) * v2;
   const double DMh2_EFT_3L_const = pref * zeta_lambda_3L;

   CHECK_CLOSE(pref, pref2, 1e-10);

   return DMh2_EFT_3L_logs + 0* DMh2_EFT_3L_const;
}

} // anonymous namespace

TEST_CASE("test_lambda_normalization")
{
   using namespace himalaya;

   const auto pars = make_point();
   auto hc = HierarchyCalculator(pars);
   const auto ho = hc.calculateDMh3L(false);

   // non-logarithmic part of zeta_lambda 3L
   const double zeta_3L_const = ho.getZetaConst();

   const auto DMh_0L = ho.getDMh(0);
   const auto DMh_1L = ho.getDMh(1);
   const auto DMh_2L = ho.getDMh(2);
   const auto DMh_3L = ho.getDMh(3);

   const auto Mh2_0L = calc_Mh2(DMh_0L);
   const auto Mh2_1L = calc_Mh2(DMh_0L + DMh_1L);
   const auto Mh2_2L = calc_Mh2(DMh_0L + DMh_1L + DMh_2L);
   const auto Mh2_3L = calc_Mh2(DMh_0L + DMh_1L + DMh_2L + DMh_3L);

   // 3L expansion uncertainty
   const double Mh2_3L_uncert = ho.getExpUncertainty(3);
   const double Mh2_3L_uncert_rel = Mh2_3L_uncert / Mh2_3L;

   const double Mh_0L = std::sqrt(Mh2_0L);
   const double Mh_1L = std::sqrt(Mh2_1L);
   const double Mh_2L = std::sqrt(Mh2_2L);
   const double Mh_3L = std::sqrt(Mh2_3L);
   const double Mh_3L_uncert_percent = Mh2_3L_uncert_rel*100;

   INFO("Mh(0L) = " << Mh_0L << " GeV");
   INFO("Mh(1L) = " << Mh_1L << " GeV");
   INFO("Mh(2L) = " << Mh_2L << " GeV");
   INFO("Mh(3L) = (" << Mh_3L << " +- " << Mh2_3L_uncert << ") GeV"
        << " (" << Mh_3L_uncert_percent << "%)");

   CHECK_CLOSE(Mh2_0L, calc_Mh2_EFT_0L(pars), 1e-5);

   CHECK_CLOSE(Mh2_1L, calc_Mh2_EFT_0L(pars)
                     + calc_Mh2_EFT_1L(pars), 1e-6);

   CHECK_CLOSE(Mh2_2L, calc_Mh2_EFT_0L(pars)
                     + calc_Mh2_EFT_1L(pars)
                     + calc_Mh2_EFT_2L(pars), 1e-6);

   CHECK_CLOSE(Mh2_3L, calc_Mh2_EFT_0L(pars)
                     + calc_Mh2_EFT_1L(pars)
                     + calc_Mh2_EFT_2L(pars)
                     + calc_Mh2_EFT_3L(pars,zeta_3L_const),
               Mh2_3L_uncert_rel);
}
