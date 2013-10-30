//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader6_2InnerFns
#define vpfHeader6_2InnerFns
#include "vpfHeader3Calculator0_Interface.h"

static ProjectInformationInstance ProjectInfoVpf6_2Header(__FILE__, "Heder, calculator built-in functions. ");

class CommandListFunctions
{
public:
  static bool innerGenerateVectorSpaceClosedWRTLieBracket(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerFourierTransformEWA(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerCasimirWRTlevi(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerFactorial(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerLog(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerSin(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerCos(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerSec(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerCsc(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerTan(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerCot(CommandList& theCommands, const Expression& input, Expression& output);

  static bool innerArcsin(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerArccos(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerArctan(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeSequenceDereference(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeEWAactOnPoly(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeConstTimesAnyActOn(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeApowerBevaluatedAtC(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerConstantFunction(CommandList& theCommands, const Expression& input, Expression& output);

  static bool innerDifferentiateSinCos(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateChainRule(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateAplusB(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateAtimesB(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateAdivideBCommutative(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateAdivideBNONCommutative(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateConstant(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateX(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateConstPower(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateAPowerB(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeDifferentiateLog(CommandList& theCommands, const Expression& input, Expression& output);

  static bool innerPowerAnyToZero(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerDdivDxToDifferentiation(CommandList& theCommands, const Expression& input, Expression& output);

  static bool outerAdivBpowerItimesBpowerJ(CommandList& theCommands, const Expression& input, Expression& output);
  static bool outerAtimesBpowerJplusEtcDivBpowerI(CommandList& theCommands, const Expression& input, Expression& output);

  static bool innerGetAlgebraicNumberFromMinPoly(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMakeMakeFile(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerCrash(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerSort(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserDefinedSubExpressions(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerGrowDynkinType(CommandList& theCommands, const Expression& input, Expression& output);

  static bool innerGetCentralizerChainsSemisimpleSubalgebras(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerComputeSemisimpleSubalgebras(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerComputePairingTablesAndFKFTsubalgebras(CommandList& theCommands, const Expression& input, Expression& output);

  static bool outerAssociateDivisionDivision(CommandList& theCommands, const Expression& input, Expression& output);
  static bool outerAssociateAdivBdivCpowerD(CommandList& theCommands, const Expression& input, Expression& output);

  static bool outerCommuteAtimesBifUnivariate(CommandList& theCommands, const Expression& input, Expression& output);
  static bool outerCommuteAtimesBtimesCifUnivariate(CommandList& theCommands, const Expression& input, Expression& output);

  static bool outerDifferentiateWRTxTimesAny(CommandList& theCommands, const Expression& input, Expression& output);

  static bool innerPlotConeUsualProjection(CommandList& theCommands, const Expression& input, Expression& output);

};

#endif

