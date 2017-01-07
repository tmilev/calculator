//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader6_2InnerFns
#define vpfHeader6_2InnerFns
#include "vpfHeader3Calculator0_Interface.h"

static ProjectInformationInstance ProjectInfoVpf6_2Header(__FILE__, "Header, calculator built-in functions. ");

class CalculatorFunctionsGeneral
{
public:
  static bool innerPrintRuleStack(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCrash(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCrashByListOutOfBounds
  (Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCrashByVectorOutOfBounds
  (Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerTurnRulesOnOff
  (Calculator& theCommands, const Expression& input, Expression& output, bool turnOff);

  static bool innerTurnOffRules
  (Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTurnOnRules
  (Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerEqualityToArithmeticExpression
  (Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerRandomInteger
  (Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSelectAtRandom
  (Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSetRandomSeed
  (Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerUrlStringToNormalString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerURLKeyValuePairsToNormalRecursive(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerStringToAtom(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerExpressionToString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerQuoteToString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerBase64ToCharToBase64Test(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCharToBase64(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSha1OfString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerX509certificateCrunch(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerIntersection(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerUnion(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerUnionNoRepetition(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCrossProduct(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerChooseCartanSA(Calculator& theCommands, const Expression& input, Expression& output);


  static bool innerGenerateVectorSpaceClosedWRTLieBracket(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFourierTransformEWA(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCasimirWRTlevi(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSqrt(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFactorial(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLogBaseSimpleCases(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLogBase(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLog(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAbs(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSin(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCos(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSec(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCsc(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTan(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCot(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCoefficientOf(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFloor(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerArcsin(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerArccos(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerArctan(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeArithmeticOperationEvaluatedOnArgument(Calculator& theCommands, const Expression& input, Expression& output);


  static bool innerThaw(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetFreeVariablesIncludeNamedConstants(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetFreeVariablesExcludeNamedConstants(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompareFunctionsNumerically(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompareExpressionsNumerically(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompareExpressionsNumericallyAtPoints(Calculator& theCommands, const Expression& input, Expression& output);

//  static bool innerPowerSqrt(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerPolynomialRelations(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerEnsureExpressionDependsOnlyOnMandatoryVariables(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerEnsureExpressionDependsOnlyOnStandard(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerMultiplySequence(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerPolynomialize(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerNumerator(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDenominator(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerLength(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDereferenceOperator(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeEWAactOnPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeConstTimesAnyActOn(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeApowerBevaluatedAtC(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerConstantFunction(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRationalFunctionSubstitution(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerGetPointsImplicitly(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerDifferentiateTrigAndInverseTrig(Calculator& theCommands, const Expression& input, Expression& output);
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

  static bool innerIntegralUpperBound(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerAnyToZero(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerExponentToLog(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDiffdivDiffxToDifferentiation(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDdivDxToDiffDivDiffx(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestMathMouseHover(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerBuildFreecalc(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCrawlTexFile(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSetOutputFile(Calculator& theCommands, const Expression& input, Expression& output);

  static bool outerMergeConstantRadicals(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerCommuteConstants(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerDivideReplaceAdivBpowerItimesBpowerJ(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerAtimesBpowerJplusEtcDivBpowerI(Calculator& theCommands, const Expression& input, Expression& output);

  static bool outerCombineFractionsCommutative(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerIsPrimeMillerRabin(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLCM(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGCD(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerIsNilpotent(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerInvertMatrixRFsVerbose(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerInvertMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDFQsEulersMethod(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerGetAlgebraicNumberFromMinPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMakeMakeFile(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSort(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSortDescending(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserDefinedSubExpressions(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGrowDynkinType(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetPrincipalSl2Index(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetDynkinIndicesSlTwoSubalgebras(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerConstructCartanSA(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerGetCentralizerChainsSemisimpleSubalgebras(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerComputeSemisimpleSubalgebras(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerComputePairingTablesAndFKFTsubalgebras(Calculator& theCommands, const Expression& input, Expression& output);

  static bool outerDivideByNumber(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerDivCancellations(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerAssociateDivisionDivision(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerAssociateAdivBdivCpowerD(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerEqualEqual(Calculator& theCommands, const Expression& input, Expression& output);


  static bool outerCommuteAtimesBifUnivariate(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerCommuteAtimesBtimesCifUnivariate(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerIsDifferentialOneFormOneVariable(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerIntegrateRationalFunctionSplitToBuidingBlocks(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateRationalFunctionBuidingBlockIa(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateRationalFunctionBuidingBlockIb(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateRationalFunctionBuidingBlockIIaandIIIa(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateRationalFunctionBuidingBlockIIb(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateRationalFunctionBuidingBlockIIIb(Calculator& theCommands, const Expression& input, Expression& output);

  static bool extractQuadraticCoeffsWRTvariable
  (const Expression& theQuadratic, const Expression& theVariable,
   Expression& outputCoeffVarSquared, Expression& outputCoeffLinTerm, Expression& outputConstTerm);
  static bool extractLinearCoeffsWRTvariable
  (const Expression& theLinearExpression, const Expression& theVariable,
   Expression& outputCoeffLinTerm, Expression& outputConstTerm);

  static bool innerIntegratePowerByUncoveringParenthesisFirst(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateSum(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateXnDiffX(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerInterpretAsDifferential(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegralOperator(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateSqrt(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerDifferentiateWRTxTimesAny(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerCanBeExtendedParabolicallyTo(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetSymmetricCartan(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTrace(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMinPolyMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCharPolyMatrix(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerDrawRootSystem(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDrawWeightSupportWithMults(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDrawWeightSupport(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDrawExpressionGraphWithOptions
  (Calculator& theCommands, const Expression& input, Expression& output, bool useFullTree);
  static bool innerDrawExpressionGraph
  (Calculator& theCommands, const Expression& input, Expression& output)
  { return innerDrawExpressionGraphWithOptions(theCommands, input, output, false);
  }
  static bool innerDrawExpressionGraphFull
  (Calculator& theCommands, const Expression& input, Expression& output)
  { return innerDrawExpressionGraphWithOptions(theCommands, input, output, true);
  }

  static bool innerIf(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerOr(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAnd(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerSolveUnivariatePolynomialWithRadicalsWRT
  (Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerMakeJavascriptExpression(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotSurface(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotImplicitFunctionFull(Calculator& theCommands, const Expression& input, Expression& output, bool showGrid);
  static bool innerPlotImplicitFunction(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotImplicitShowGridFunction(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotConeUsualProjection(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotSegment(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotParametricCurve(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotIntegralOf(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotPolarRfunctionThetaExtended(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotPolarRfunctionTheta(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlot2D(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotFill(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotRectangle(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotLabel(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotViewRectangle(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotViewWindow(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotPoint(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlot2DWithBars(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerLastElement(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRemoveLastElement(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerPlotWedge(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLispify(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLispifyFull(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerChildExpression(Calculator& theCommands, const Expression& input, Expression& output);


  static bool innerEvaluateToDouble(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerEmbedSSalgInSSalg(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylDimFormula(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDecomposeCharGenVerma(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSplitFDpartB3overG2Init(Calculator& theCommands, const Expression& input, Expression& output, branchingData& theG2B3Data, Expression& outputContext);
  static bool innerDecomposeFDPartGeneralizedVermaModuleOverLeviPart(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerParabolicWeylGroups(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerParabolicWeylGroupsBruhatGraph(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerAllPartitions(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAllVectorPartitions(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAllSelectionsFixedRank(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerKostkaNumber(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDeterminant(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintGenVermaModule(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWriteGenVermaModAsDiffOperatorUpToLevel(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerHWV(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSplitGenericGenVermaTensorFD(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerHWTAABF(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSplitFDpartB3overG2CharsOutput(Calculator& theCommands, const Expression& input, Expression& output, branchingData& theG2B3Data);
  static bool innerPrintB3G2branchingTableInit
  (Calculator& theCommands, const Expression& input, Expression& output, branchingData& theG2B3data, int& desiredHeight, Expression& outputContext);
  static bool innerSplitFDpartB3overG2(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerCoefficientsPowersOf(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsNonEmptySequence(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsEven(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsConstant(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGaussianEliminationMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSplitToPartialFractionsOverAlgebraicReals(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestTopCommand(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestIndicator(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestStandardOutput(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRootSAsAndSltwos(Calculator& theCommands, const Expression& input, Expression& output, bool showSLtwos, bool MustRecompute);
  static bool innerPrintRootSAs(Calculator& theCommands, const Expression& input, Expression& output)
  { return CalculatorFunctionsGeneral::innerRootSAsAndSltwos(theCommands, input, output, false, false);
  }
  static bool innerPrintRootSAsForceRecompute(Calculator& theCommands, const Expression& input, Expression& output)
  { return CalculatorFunctionsGeneral::innerRootSAsAndSltwos(theCommands, input, output, false, true);
  }
  static bool innerPrintSltwos(Calculator& theCommands, const Expression& input, Expression& output)
  { return CalculatorFunctionsGeneral::innerRootSAsAndSltwos(theCommands, input, output, true, false);
  }
  static bool innerFindProductDistanceModN
  (Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSolveProductSumEquationOverSetModN
  (Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerSolveSerreLikeSystem
  (Calculator& theCommands, const Expression& input, Expression& output, bool useUpperLimit, bool startWithAlgebraicClosure);
  static bool innerSolveSerreLikeSystemNoUpperLimit
  (Calculator& theCommands, const Expression& input, Expression& output)
  { return CalculatorFunctionsGeneral::innerSolveSerreLikeSystem(theCommands, input, output, false, false);
  }
  static bool innerSolveSerreLikeSystemUpperLimit
  (Calculator& theCommands, const Expression& input, Expression& output)
  { return CalculatorFunctionsGeneral::innerSolveSerreLikeSystem(theCommands, input, output, true, false);
  }
  static bool innerSolveSerreLikeSystemAlgebraic
  (Calculator& theCommands, const Expression& input, Expression& output)
  { return CalculatorFunctionsGeneral::innerSolveSerreLikeSystem(theCommands, input, output, false, true);
  }
  static bool innerSolveSerreLikeSystemAlgebraicUpperLimit
  (Calculator& theCommands, const Expression& input, Expression& output)
  { return CalculatorFunctionsGeneral::innerSolveSerreLikeSystem(theCommands, input, output, true, true);
  }
  static bool innerFetchWebPage
  (Calculator& theCommands, const Expression& input, Expression& output);


};

#endif
