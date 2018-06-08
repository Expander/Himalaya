InitializeHimalaya::usage = "Loads the Himalaya LibraryLink.  After
 InitializeHimalaya[] has been called, the function
 HimalayaCalculateDMh3L[] is available.

Usage:

  InitializeHimalaya[libraryLink];

Arguments:

 - libraryLink - The path to the Himalaya LibraryLink, 'Himalaya_LibraryLink.so'

";

HimalayaCalculateDMh3L::usage = "Runs Himalaya, given a set of input
parameters, and returns a list with all calculated loop corrections.

Usage:

  output = HimalayaCalculateDMh3L[settings -> {...}, parameters -> {...}];

Arguments:

 - settings - List of replacement rules with calculational settings.
   Allowed settings are:

    - bottom -> (True | False): calculate alpha_bottom (True) or
      alpha_top (False) corrections.  Default: False

    - verbose -> (True | False): print verbose output.  Default: True

 - parameters - List of replacement rules for the running MSSM input
   parameters.  Same definition as in Himalaya_interface.hpp.
   Allowed parameters are:

    - scale: renormalization scale
    - mu: superpotential mu parameter
    - g3: strong gauge coupling
    - vd: neutral down-type Higgs VEV (Sqrt[vu^2 + vd^2] ~ 246 GeV)
    - vu: neutral up-type Higgs VEV (Sqrt[vu^2 + vd^2] ~ 246 GeV)
    - mq2: 3x3 matrix of soft-breaking left-handed squark squared mass parameters
    - md2: 3x3 matrix of soft-breaking right-handed down-type squark squared mass parameters
    - mu2: 3x3 matrix of soft-breaking right-handed up-type squark squared mass parameters
    - Ab: trilinear sbottom coupling
    - At: trilinear stop coupling
    - MA: CP-odd Higgs boson mass
    - MG: gluino mass
    - MW: W boson mass
    - MZ: Z boson mass
    - Mt: top quark mass
    - Mb: bottom quark mass
    - MSt: (optional) 2-vector with stop masses
    - MSb: (optional) 2-vector with sbottom masses
    - s2t: (optional) sine of 2 times stop mixing angle
    - s2b: (optional) sine of 2 times sbottom mixing angle

Example:

MS = 2000;
TB = 20;
Xt = Sqrt[6]*MS;

output = HimalayaCalculateDMh3L[
    settings -> {
        bottom -> False,
        verbose -> True
    },
    parameters -> {
        scale -> MS,
        mu -> MS,
        g3 -> 0.1184,
        vd -> 246*Cos[ArcTan[TB]],
        vu -> 246*Sin[ArcTan[TB]],
        mq2 -> MS^2 IdentityMatrix[3],
        md2 -> MS^2 IdentityMatrix[3],
        mu2 -> MS^2 IdentityMatrix[3],
        Ab -> 0,
        At -> Xt + MS/TB,
        MA -> MS,
        MG -> MS,
        MW -> 80.384,
        MZ -> 91.1876,
        Mt -> 160,
        Mb -> 2.4
    }
];
";

(* function arguments *)
{ parameters, settings };

(* settings *)
{ bottom, verbose };

(* input parameters *)
{ scale, mu, g3, vd, vu, mq2, md2, mu2, Ab, At, MA, MG, MW, MZ, Mt,
  Mb, MSt, MSb, s2t, s2b };

(* output parameters *)
{ renormalizationScheme, hierarchyID, hierarchyName, Mstop, Mh2Tree,
  Mh21Loop, Mh22Loop, Mh23Loop, expansionUncertainty,
  deltaLambda3LoopH3mDRbarPrime,
  deltaLambda3LoopH3mShiftDRbarPrimeToMSbar,
  deltaLambda3LoopH3mUncertainty, deltaLambda3LoopEFTDRbarPrime,
  deltaLambda3LoopEFTShiftDRbarPrimeToMSbar,
  deltaLambda3LoopEFTUncertainty };

Himalaya::nonum = "Error: `1` is not a numeric input value!";
Himalaya::error = "`1`";
Himalaya::info  = "`1`";
HimalayaErrorMessage[s_] := Message[Himalaya::error, s];
HimalayaInfoMessage[s_]  := Message[Himalaya::info, s];

Begin["`Private`"];

InitializeHimalaya[libName_String] := (
       HimalayaCalculateDMh3LLibInterface =
          LibraryFunctionLoad[libName, "HimalayaCalculateDMh3L", LinkObject, LinkObject];
    );

himalayaDefaultSettings = {
    bottom -> False,
    verbose -> True
};

himalayaDefaultParameters = {
        scale -> 0,
        mu -> 0,
        g3 -> 0,
        vd -> 0,
        vu -> 0,
        mq2 -> {{0,0,0},{0,0,0},{0,0,0}},
        md2 -> {{0,0,0},{0,0,0},{0,0,0}},
        mu2 -> {{0,0,0},{0,0,0},{0,0,0}},
        Ab -> 0,
        At -> 0,
        MA -> 0,
        MG -> 0,
        MW -> 0,
        MZ -> 0,
        Mt -> 0,
        Mb -> 0,
        MSt -> {-1, -1}, (* invalid input *)
        MSb -> {-1, -1}, (* invalid input *)
        s2t -> -2,       (* invalid input *)
        s2b -> -2        (* invalid input *)
};

Options[HimalayaCalculateDMh3L] = {
    Sequence @@ himalayaDefaultSettings,
    Sequence @@ himalayaDefaultParameters
};

HimalayaNumericQ[a_?NumericQ] := N[a];
HimalayaNumericQ[a_] := (Message[Himalaya::nonum, a]; Abort[]);

HimalayaCalculateDMh3L[a___, (settings | parameters) -> s_List, r___] :=
    HimalayaCalculateDMh3L[a, Sequence @@ s, r];

HimalayaCalculateDMh3L[OptionsPattern[]] :=
    HimalayaCalculateDMh3LLibInterface[
        HimalayaNumericQ /@ {
            (* settings *)
            Boole[OptionValue[bottom]],
            Boole[OptionValue[verbose]],
            (* parameters *)
            OptionValue[scale],
            OptionValue[mu],
            OptionValue[g3],
            OptionValue[vd],
            OptionValue[vu],
            OptionValue[mq2][[1,1]],
            OptionValue[mq2][[1,2]],
            OptionValue[mq2][[1,3]],
            OptionValue[mq2][[2,1]],
            OptionValue[mq2][[2,2]],
            OptionValue[mq2][[2,3]],
            OptionValue[mq2][[3,1]],
            OptionValue[mq2][[3,2]],
            OptionValue[mq2][[3,3]],
            OptionValue[md2][[1,1]],
            OptionValue[md2][[1,2]],
            OptionValue[md2][[1,3]],
            OptionValue[md2][[2,1]],
            OptionValue[md2][[2,2]],
            OptionValue[md2][[2,3]],
            OptionValue[md2][[3,1]],
            OptionValue[md2][[3,2]],
            OptionValue[md2][[3,3]],
            OptionValue[mu2][[1,1]],
            OptionValue[mu2][[1,2]],
            OptionValue[mu2][[1,3]],
            OptionValue[mu2][[2,1]],
            OptionValue[mu2][[2,2]],
            OptionValue[mu2][[2,3]],
            OptionValue[mu2][[3,1]],
            OptionValue[mu2][[3,2]],
            OptionValue[mu2][[3,3]],
            OptionValue[Ab],
            OptionValue[At],
            OptionValue[MA],
            OptionValue[MG],
            OptionValue[MW],
            OptionValue[MZ],
            OptionValue[Mt],
            OptionValue[Mb],
            OptionValue[MSt][[1]],
            OptionValue[MSt][[2]],
            OptionValue[MSb][[1]],
            OptionValue[MSb][[2]],
            OptionValue[s2t],
            OptionValue[s2b]
        }
    ];

End[];