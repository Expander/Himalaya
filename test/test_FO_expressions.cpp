#define BOOST_TEST_MODULE test_FO_expressions

#include <boost/test/unit_test.hpp>
#define private public
#include "HierarchyCalculator.hpp"
#undef private

himalaya::Parameters make_point()
{
   himalaya::Parameters pars;

   pars.scale = 1973.75;
   pars.mu = 1999.82;
   pars.g3 =  1.02907;
   pars.vd = 49.5751;
   pars.vu = 236.115;
   pars.mq2 <<  4.00428e+06 , 0, 0,
               0, 4.00428e+06, 0,
               0, 0, 3.99786e+06;
   pars.md2 << 4.00361e+06, 0, 0,
               0, 4.00361e+06, 0,
               0, 0, 4.00346e+06;
   pars.mu2 << 4.00363e+06 , 0, 0,
               0, 4.00363e+06, 0,
               0, 0, 3.99067e+06;
   pars.Ab = 9996.81;
   pars.At = 6992.34;

   pars.MA = 1992.14;
   pars.MG = 2000.96;
   pars.MW = 76.7777;
   pars.MZ = 88.4219;
   pars.Mt = 147.295;
   pars.Mb = 2.23149;
   pars.MSt << 1745.3 , 2232.1;
   pars.MSb <<  2000.14, 2001.09;
   pars.s2t = -0.999995;
   pars.s2b = -0.550527;

   return pars;
}


/**
 * Performs a sanity check of the implemented expansion terms by
 * comparing them to their numerical value at a given parameter point.
 */
BOOST_AUTO_TEST_CASE(test_FO_expansions)
{
   using namespace himalaya;

   const double eps = 1e-13;

   for (int i = 0; i < NUMBER_OF_HIERARCHIES; i++) {
      himalaya::HierarchyCalculator hc(make_point());
      hc.init();

      himalaya::HierarchyObject ho(false);
      ho.setMDRFlag(1);
      ho.setSuitableHierarchy(i);

      const auto oloMat  = hc.calculateHierarchy(ho, 1, 0, 0);
      const auto twloMat = hc.calculateHierarchy(ho, 0, 1, 0);
      const auto thloMat = hc.calculateHierarchy(ho, 0, 0, 1);

      BOOST_TEST_MESSAGE("Checking hierarchy " << i << " (" << ho.getH3mHierarchyNotation(i) << ")");

      switch(i){
      case h3:
         BOOST_CHECK_CLOSE_FRACTION(oloMat(0,0), -1033.437882123761, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,0), -394.3521101999062, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,1),  17633.47392819223, eps);

         BOOST_CHECK_CLOSE_FRACTION(twloMat(0,0), -13.48340821650015, 1e-5);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,0),  11.12436787252288, 1e-5);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,1),  1476.660068002361, 1e-5);

         BOOST_CHECK_CLOSE_FRACTION(thloMat(0,0), 1.096612614742133, 2e-6);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,0), 9.986750150481939, 2e-6);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,1), 370.2505433664134, 2e-6);
	 break;
      case h32q2g:
         BOOST_CHECK_CLOSE_FRACTION(oloMat(0,0), -1033.437882123761, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,0), -394.3521101999062, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,1),  17633.47392819223, eps);

         BOOST_CHECK_CLOSE_FRACTION(twloMat(0,0), -13.66052379180129, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,0),  11.26755617866339, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,1),  1477.465656153518, eps);

         BOOST_CHECK_CLOSE_FRACTION(thloMat(0,0), 1.113051431370291, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,0), 9.903809573970422, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,1), 369.7408109643386, eps);
         break;
      case h3q22g:
         BOOST_CHECK_CLOSE_FRACTION(oloMat(0,0), -1033.437882123761, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,0), -394.3521101999062, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,1),  17633.47392819223, eps);

         BOOST_CHECK_CLOSE_FRACTION(twloMat(0,0), -13.66052379180129, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,0),  11.26755617866339, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,1),  1477.465656153518, eps);

         BOOST_CHECK_CLOSE_FRACTION(thloMat(0,0), 1.058450932536496, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,0), 10.0141272838662, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,1), 370.3301180635573, eps);
         break;
      case h4:
         BOOST_CHECK_CLOSE_FRACTION(oloMat(0,0),                  0, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,0),                  0, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,1), 6685.123085628641, eps);

         BOOST_CHECK_CLOSE_FRACTION(twloMat(0,0),                 0, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,0), 1183.325484493686, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,1), 1458.970501474495, eps);

         BOOST_CHECK_CLOSE_FRACTION(thloMat(0,0), 162.1379208650191, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,0), 326.0219627343553, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,1), 431.6926278454841, eps);
         break;
      case h5:
         BOOST_CHECK_CLOSE_FRACTION(oloMat(0,0),  15921.69462848581, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,0), -388569.2043081555, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,1),  7874.401574063407, eps);

         BOOST_CHECK_CLOSE_FRACTION(twloMat(0,0), -86.77887344841422, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,0), -20625.63783863484, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,1), -42446.62009872038, eps);

         BOOST_CHECK_CLOSE_FRACTION(thloMat(0,0),  2442.115080578889, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,0), -3859.942907446577, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,1),  60593.055768119  , eps);
         break;
      case h5g1:
         BOOST_CHECK_CLOSE_FRACTION(oloMat(0,0),  15921.69462848581, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,0), -388569.2043081556, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,1),  7874.401574063407, eps);

         BOOST_CHECK_CLOSE_FRACTION(twloMat(0,0), -114.6037388932203, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,0), -20341.84471909946, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,1), -42843.48046642416, eps);

         BOOST_CHECK_CLOSE_FRACTION(thloMat(0,0),  2415.507513838155, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,0), -3766.750163753644, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,1),  59380.34497121828, eps);
         break;
      case h6:
         BOOST_CHECK_CLOSE_FRACTION(oloMat(0,0),  9272.477351702315, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,0), -184.7601614832763, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,1),  7581.278122072418, eps);

         BOOST_CHECK_CLOSE_FRACTION(twloMat(0,0), -1078.578574572312, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,0),  7096.529601647042, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,1), -1927.791631086123, eps);

         BOOST_CHECK_CLOSE_FRACTION(thloMat(0,0), 245.4412216221288, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,0), 573.1296253278389, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,1), 8448.4582538127  , eps);
         break;
      case h6b:
         BOOST_CHECK_CLOSE_FRACTION(oloMat(0,0),  9272.477351702311, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,0), -184.7601614832763, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,1),  7581.278122072418, eps);

         BOOST_CHECK_CLOSE_FRACTION(twloMat(0,0), -1078.578574572312, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,0),  7096.52960164704 , eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,1), -1900.197036824461, eps);

         BOOST_CHECK_CLOSE_FRACTION(thloMat(0,0), 283.0253770519464, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,0), 566.2182257407396, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,1), 10093.33785879814, eps);
         break;
      case h6b2qg2:
         BOOST_CHECK_CLOSE_FRACTION(oloMat(0,0),  9272.477351702311, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,0), -184.7601614832759, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,1),  7581.278122072418, eps);

         BOOST_CHECK_CLOSE_FRACTION(twloMat(0,0), -1089.201418061661, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,0),  7145.267026465748, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,1), -2077.345120153528, eps);

         BOOST_CHECK_CLOSE_FRACTION(thloMat(0,0), 285.3154791763894, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,0), 544.3654284413091, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,1), 10336.22756889787, eps);
         break;
      case h6bq22g:
         BOOST_CHECK_CLOSE_FRACTION(oloMat(0,0),  9272.477351702315, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,0), -184.7601614832763, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,1),  7581.278122072418, eps);

         BOOST_CHECK_CLOSE_FRACTION(twloMat(0,0), -1078.578574572311, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,0),  7096.529601647042, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,1), -1900.197036824461, eps);

         BOOST_CHECK_CLOSE_FRACTION(thloMat(0,0), 283.0220052455883, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,0), 566.2190953470737, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,1), 10093.33986048966, eps);
         break;
      case h6bq2g2:
         BOOST_CHECK_CLOSE_FRACTION(oloMat(0,0),  9272.477351702315, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,0), -184.7601614832759, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,1),  7581.278122072418, eps);

         BOOST_CHECK_CLOSE_FRACTION(twloMat(0,0), -1089.201418061661, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,0),  7145.267026465748, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,1), -2077.345120153528, eps);

         BOOST_CHECK_CLOSE_FRACTION(thloMat(0,0), 285.3120881213721, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,0), 544.3662758149513, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,1), 10336.23012077387, eps);
         break;
      case h6g2:
         BOOST_CHECK_CLOSE_FRACTION(oloMat(0,0),  9272.477351702315, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,0), -184.7601614832761, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,1),  7581.278122072418, eps);

         BOOST_CHECK_CLOSE_FRACTION(twloMat(0,0), -1089.201418061661, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,0),  7145.267026465748, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,1), -2112.642999123034, eps);

         BOOST_CHECK_CLOSE_FRACTION(thloMat(0,0), 246.0217489966267, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,0), 557.451210096066 , eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,1), 8628.076480526881, eps);
         break;
      case h9:
         BOOST_CHECK_CLOSE_FRACTION(oloMat(0,0), -1033.437882123761, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,0), -394.352110199906 , eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,1),  17633.47392819223, eps);

         BOOST_CHECK_CLOSE_FRACTION(twloMat(0,0),  420.2050380976995, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,0), -554.6021924866435, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,1), -797.8089039452509, eps);

         BOOST_CHECK_CLOSE_FRACTION(thloMat(0,0),  132.8584579769461, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,0), -171.9326869339159, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,1), -800.8408283898472, eps);
         break;
      case h9q2:
         BOOST_CHECK_CLOSE_FRACTION(oloMat(0,0), -1033.437882123761, eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,0), -394.352110199906 , eps);
         BOOST_CHECK_CLOSE_FRACTION(oloMat(1,1),  17633.47392819223, eps);

         BOOST_CHECK_CLOSE_FRACTION(twloMat(0,0),  420.2050380976995, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,0), -554.6021924866435, eps);
         BOOST_CHECK_CLOSE_FRACTION(twloMat(1,1), -797.8089039452509, eps);

         BOOST_CHECK_CLOSE_FRACTION(thloMat(0,0),  132.6358855624267, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,0), -171.4711818838455, eps);
         BOOST_CHECK_CLOSE_FRACTION(thloMat(1,1), -800.9569014303727, eps);
         break;
      default:
         BOOST_FAIL("unknown hierarchy!");
         break;
      }
   }
}
