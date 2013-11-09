//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader6_2InnerFns
#define vpfHeader6_2InnerFns
#include "vpfHeader3Calculator0_Interface.h"

static ProjectInformationInstance ProjectInfoVpf6_2Header(__FILE__, "Heder, calculator built-in functions. ");

class CalculatorFunctionsGeneral
{
public:
  static bool innerGenerateVectorSpaceClosedWRTLieBracket(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFourierTransformEWA(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCasimirWRTlevi(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFactorial(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLog(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSin(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCos(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSec(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCsc(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTan(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCot(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerArcsin(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerArccos(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerArctan(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeSequenceDereference(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeEWAactOnPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeConstTimesAnyActOn(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeApowerBevaluatedAtC(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerConstantFunction(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerDifferentiateSinCos(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateChainRule(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateAplusB(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateAtimesB(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateAdivideBCommutative(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateAdivideBNONCommutative(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateConstant(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateX(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateConstPower(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateAPowerB(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeDifferentiateLog(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerPowerAnyToZero(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDdivDxToDifferentiation(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestMathMouseHover(Calculator& theCommands, const Expression& input, Expression& output);

  static bool outerAdivBpowerItimesBpowerJ(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerAtimesBpowerJplusEtcDivBpowerI(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerGetAlgebraicNumberFromMinPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMakeMakeFile(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSort(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserDefinedSubExpressions(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGrowDynkinType(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerGetCentralizerChainsSemisimpleSubalgebras(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerComputeSemisimpleSubalgebras(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerComputePairingTablesAndFKFTsubalgebras(Calculator& theCommands, const Expression& input, Expression& output);

  static bool outerAssociateDivisionDivision(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerAssociateAdivBdivCpowerD(Calculator& theCommands, const Expression& input, Expression& output);

  static bool outerCommuteAtimesBifUnivariate(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerCommuteAtimesBtimesCifUnivariate(Calculator& theCommands, const Expression& input, Expression& output);

  static bool outerDifferentiateWRTxTimesAny(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerPlotConeUsualProjection(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotWedge(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLispify(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLispifyFull(Calculator& theCommands, const Expression& input, Expression& output);
};

#endif

