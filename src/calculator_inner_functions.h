// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader6_2InnerFns
#define vpfHeader6_2InnerFns
#include "calculator_interface.h"

class CalculatorFunctionsCrypto {
public:
  static bool innerAES_CBC_256_Encrypt(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAES_CBC_256_Decrypt(Calculator& calculator, const Expression& input, Expression& output);
  static bool loadKnownCertificates(Calculator& calculator, const Expression& input, Expression& output);
  static bool testLoadPEMCertificates(Calculator& calculator, const Expression& input, Expression& output);
  static bool testLoadPEMPrivateKey(Calculator& calculator, const Expression& input, Expression& output);
  static bool testTLSMessageSequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool testTLSDecodeSSLRecord(Calculator& calculator, const Expression& input, Expression& output);

  static bool jwtVerifyAgainstKnownKeys(Calculator& calculator, const Expression& input, Expression& output);
  static bool jwtVerifyAgainstRSA256(Calculator& calculator, const Expression& input, Expression& output);
  static bool RSAEncrypt(Calculator& calculator, const Expression& input, Expression& output);
  static bool RSASign(Calculator& calculator, const Expression& input, Expression& output);
  static bool showKnownObjectIds(Calculator& calculator, const Expression& input, Expression& output);
  static bool sha224OfString(Calculator& calculator, const Expression& input, Expression& output);

};

class CalculatorFunctions {
public:
  static bool leftIntervalGreaterThanRight(const Expression& left, const Expression& right);
  static bool printRuleStack(Calculator& calculator, const Expression& input, Expression& output);
  static bool crash(Calculator& calculator, const Expression& input, Expression& output);
  static bool crashByListOutOfBounds(Calculator& calculator, const Expression& input, Expression& output);
  static bool crashByVectorOutOfBounds(Calculator& calculator, const Expression& input, Expression& output);
  static bool testProblemInterpretation(Calculator& calculator, const Expression& input, Expression& output);
  static bool testTopicListProblems(Calculator& calculator, const Expression& input, Expression& output);
  static bool precomputeSemisimpleLieAlgebraStructure(Calculator& calculator, const Expression& input, Expression& output);

  static bool turnRulesOnOff(Calculator& calculator, const Expression& input, Expression& output, bool turnOff);

  static bool turnOffRules(Calculator& calculator, const Expression& input, Expression& output);
  static bool turnOnRules(Calculator& calculator, const Expression& input, Expression& output);

  static bool turnOnApproximations(Calculator& calculator, const Expression& input, Expression& output);
  static bool turnOffApproximations(Calculator& calculator, const Expression& input, Expression& output);

  static bool equalityToArithmeticExpression(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionEqualityToArithmeticExpression(Calculator& calculator, const Expression& input, Expression& output);

  static bool randomInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool selectAtRandom(Calculator& calculator, const Expression& input, Expression& output);
  static bool setRandomSeed(Calculator& calculator, const Expression& input, Expression& output);

  static bool convertElementZmodPToInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool urlStringToNormalString(Calculator& calculator, const Expression& input, Expression& output);
  static bool stringToURL(Calculator& calculator, const Expression& input, Expression& output);
  static bool urlKeyValuePairsToNormalRecursive(Calculator& calculator, const Expression& input, Expression& output);

  static bool stringToAtom(Calculator& calculator, const Expression& input, Expression& output);
  static bool expressionToString(Calculator& calculator, const Expression& input, Expression& output);
  static bool expressiontoUTF8String(Calculator& calculator, const Expression& input, Expression& output);
  static bool quoteToString(Calculator& calculator, const Expression& input, Expression& output);
  static bool base64ToCharToBase64Test(Calculator& calculator, const Expression& input, Expression& output);
  static bool convertBase64ToString(Calculator& calculator, const Expression& input, Expression& output);
  static bool convertStringToHex(Calculator& calculator, const Expression& input, Expression& output);
  static bool charToBase64(Calculator& calculator, const Expression& input, Expression& output);
  static bool appendDoubleSha256Check(Calculator& calculator, const Expression& input, Expression& output);

  static bool convertBase58ToHex(Calculator& calculator, const Expression& input, Expression& output);
  static bool convertHexToBase58(Calculator& calculator, const Expression& input, Expression& output);
  static bool convertIntegerUnsignedToBase58(Calculator& calculator, const Expression& input, Expression& output);
  static bool sliceString(Calculator& calculator, const Expression& input, Expression& output);
  static bool stringDifference(Calculator& calculator, const Expression& input, Expression& output);
  static bool nistEllipticCurveGenerator(Calculator& calculator, const Expression& input, Expression& output);
  static bool nistEllipticCurveOrder(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGenerateRandomPrime(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerTestRSASign(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSha256OfString(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSha512(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerRIPEMD160OfString(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSha3_256OfString(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerKeccak256OfString(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSha256OfStringVerbose(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSha1OfString(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionHashString(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    const std::string& hashId,
    bool verbose
  );
  static bool innerTestJSON(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerBase64ToHex(Calculator& calculator, const Expression& input, Expression& output );
  static bool innerIntegerToHex(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerHexToInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerHexToString(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerTestASN1Decode(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerX509CertificateServer(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerX509CertificateDecode(Calculator& calculator, const Expression& input, Expression& output);


  static bool innerIntersection(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerUnion(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerUnionNoRepetition(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCrossProduct(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerDistributeExponent(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDistributeSqrt(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerChoosecartanSubalgebra(Calculator& calculator, const Expression& input, Expression& output);


  static bool innerGenerateVectorSpaceClosedWRTLieBracket(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerFourierTransformEWA(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCasimirWRTlevi(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSqrt(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerFactorIntegeR(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionFactorInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerFactorOutNumberContent(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerFactorial(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLogBaseNaturalToLn(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerLogBaseSimpleCases(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLogBase(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLog(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAbs(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSin(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCos(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSec(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCsc(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerTan(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCot(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIsProductLinearOrConstantTermsIn(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsProductTermsUpToPower(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsLinearOrConstantIn(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCoefficientOf(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerConstTermRelative(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerFloor(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerRound(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLogarithmBaseNCeiling(Calculator& calculator, const Expression& input, Expression& output);


  static bool innerArcsinAlgebraic(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerArccosAlgebraic(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerArcsin(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerArccos(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerArctan(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCompositeArithmeticOperationEvaluatedOnArgument(
    Calculator& calculator, const Expression& input, Expression& output
  );

  static bool innerMatchesPattern(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerThaw(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetFreeVariablesIncludeNamedConstants(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetFreeVariablesExcludeNamedConstants(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCompareFunctionsNumerically(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCompareExpressionsNumerically(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCompareExpressionsNumericallyAtPoints(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerEnsureExpressionDependsOnlyOnMandatoryVariables(
    Calculator& calculator, const Expression& input, Expression& output, bool excludeNamedConstants
  );
  static bool innerEnsureExpressionDependsOnlyOnMandatoryVariablesExcludeNamedConstants(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerEnsureExpressionDependsOnlyOnMandatoryVariables(calculator, input, output, true);
  }
  static bool innerEnsureExpressionDependsOnlyOnMandatoryVariablesIncludeNamedConstants(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerEnsureExpressionDependsOnlyOnMandatoryVariables(calculator, input, output, false);
  }
  static bool innerEnsureExpressionDependsOnlyOnStandard(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerMultiplySequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSumSequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSumTimesExpressionToSumOf(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSumAsOperatorToSumInternalNotation(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerHandleUnderscorePowerLimits(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerPolynomializE(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionPolynomialize(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerApplyToSubexpressionsRecurseThroughCalculusFunctions(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerSubList(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerApplyToList(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerNumerator(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDenominator(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerLength(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerDereferenceInterval(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDereferenceSequenceOrMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDereferenceSequenceStatements(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerCompositeEWAactOnPoly(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCompositeConstTimesAnyActOn(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCompositeApowerBevaluatedAtC(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerConstantFunction(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerRationalFunctionSubstitution(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerGetPointsImplicitly(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerDifferentiateTrigAndInverseTrig(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDifferentiateChainRule(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDifferentiateAplusB(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDifferentiateAtimesB(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDifferentiateAdivideBCommutative(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDifferentiateAdivideBNONCommutative(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDifferentiateConstant(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDifferentiateX(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDifferentiateConstPower(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDifferentiateAPowerB(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCompositeDifferentiateLog(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerOperatorBounds(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerAnyToZero(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerExponentToLog(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDiffdivDiffxToDifferentiation(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDdivDxToDiffDivDiffx(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerTestMathMouseHover(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerNewtonsMethod(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerBuildFreecalc(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerBuildFreecalcSingleSlides(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerBuildFreecalcSlidesOnTopic(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCrawlTexFile(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerCompareIntervalsNumerically(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntersectUnion(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntersectEmptySet(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerUnionEmptySet(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntersectIntervals(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerUnionUnionIntervals(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerUnionIntervals(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerNormalizeIntervals(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetFirstSummandContaining(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetSummand(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCollectMultiplicands(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCollectOpands(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCollectSummands(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerMergeConstantRadicals(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerCommuteConstants(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerDivideReplaceAdivBpowerItimesBpowerJ(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerAtimesBpowerJplusEtcDivBpowerI(Calculator& calculator, const Expression& input, Expression& output);

  static bool combineFractionsCommutative(Calculator& calculator, const Expression& input, Expression& output);
  static bool combineFractionsCommutativeWithInternalLibrary(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIsPrimeMillerRabin(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsPossiblyPrime(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLCM(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGCD(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIsNilpotent(Calculator& calculator, const Expression& input, Expression& output);
  //static bool innerGramSchmidtVerbose(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerInvertMatrixRFsVerbose(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerInvertMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDFQsEulersMethod(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerConvertAlgebraicNumberToMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetAlgebraicNumberFromMinPoly(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPrintAlgebraicClosureStatus(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerScaleToLeadingUnit(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerRemoveDuplicates(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSort(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSortDescending(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetUserDefinedSubExpressions(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGrowDynkinType(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetPrincipalSl2Index(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetDynkinIndicesSlTwoSubalgebras(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerConstructCartanSubalgebra(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerGetCentralizerChainsSemisimpleSubalgebras(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerComputeSemisimpleSubalgebras(Calculator& calculator, const Expression& input, Expression& output);
  static bool innercomputePairingTablesAndFKFTsubalgebras(Calculator& calculator, const Expression& input, Expression& output);

  static bool outerDivideByNumber(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerDivCancellations(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerAssociateDivisionDivision(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerAssociateAdivBdivCpowerD(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerEqualEqual(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerEqualEqualEqual(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMax(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMin(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLessThanOrEqualTo(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGreaterThanOrEqualTo(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGreaterThan(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLessThan(Calculator& calculator, const Expression& input, Expression& output);


  static bool outerCommuteAtimesBifUnivariate(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerCommuteAtimesBtimesCifUnivariate(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDifferentialStandardHandler(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDifferentialOfPolynomial(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIsDifferentialOneFormOneVariable(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIntegrateRationalFunctionSplitToBuidingBlocks(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegrateRationalFunctionBuidingBlockIa(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegrateRationalFunctionBuidingBlockIb(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegrateRationalFunctionBuidingBlockIIaandIIIa(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerIntegrateRationalFunctionBuidingBlockIIb(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegrateRationalFunctionBuidingBlockIIIb(Calculator& calculator, const Expression& input, Expression& output);

  static bool extractQuadraticCoeffsWRTvariable(
    const Expression& theQuadratic,
    const Expression& theVariable,
    Expression& outputCoeffVarSquared,
    Expression& outputCoeffLinTerm,
    Expression& outputConstTerm
  );
  static bool extractLinearCoeffsWRTvariable(
    const Expression& theLinearExpression,
    const Expression& theVariable,
    Expression& outputCoeffLinTerm,
    Expression& outputConstTerm
  );

  static bool innerIntegrateDefiniteIntegral(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegratePowerByUncoveringParenthesisFirst(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegrateSum(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegratePullImaginaryUnit(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegrateXnDiffX(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegratePullConstant(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegrateSqrtOneMinusXsquared(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegrateSqrtXsquaredMinusOne(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegrateXpowerNePowerAx(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIntegrateEpowerAxDiffX(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegrateSinPowerNCosPowerM(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerExploitCosEvenness(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerExploitSinOddness(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerTrigSumToTrigProduct(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSineOfAngleSumToTrig(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCosineOfAngleSumToTrig(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerConvertSinToExponent(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerConvertCosToExponent(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerImaginaryUnit(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerEulerFormulaAsLaw(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegrateTanPowerNSecPowerM(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerInterpretAsDifferential(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntegralOperator(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDifferentiateSqrt(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerDifferentiateWRTxTimesAny(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerCanBeExtendedParabolicallyTo(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetSymmetricCartan(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerTrace(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerReverseBytes(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMinPolyMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCharPolyMatrix(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerdrawRootSystem(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDrawWeightSupportWithMults(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDrawWeightSupport(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDrawExpressionGraphWithOptions(
    Calculator& calculator, const Expression& input, Expression& output, bool useFullTree
  );
  static bool innerDrawExpressionGraph(Calculator& calculator, const Expression& input, Expression& output) {
    return innerDrawExpressionGraphWithOptions(calculator, input, output, false);
  }
  static bool innerDrawExpressionGraphFull(Calculator& calculator, const Expression& input, Expression& output) {
    return innerDrawExpressionGraphWithOptions(calculator, input, output, true);
  }

  static bool innerIfStandard(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIfFrozen(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerOr(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerOrIdentical(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAnd(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerSolveUnivariatePolynomialWithRadicalsWRT(
    Calculator& calculator, const Expression& input, Expression& output
  );

  static bool innerMakeJavascriptExpressioN(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionMakeJavascriptExpression(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotCoordinateSystem(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotSetProjectionScreenBasis(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerPlotSurface(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotImplicitFunctionFull(Calculator& calculator, const Expression& input, Expression& output, bool showGrid);
  static bool innerPlotImplicitFunction(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotImplicitShowGridFunction(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotSegment(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotMarkSegment(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotPath(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotParametricCurve(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotDirectionField(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotVectorField(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotDirectionOrVectorField(
    Calculator& calculator, const Expression& input, Expression& output, bool vectorsAreNormalized
  );
  static bool innerPlotPolarRfunctionThetaExtended(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotPolarRfunctionTheta(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsPlot(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlot2D(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlot2DoverIntervals(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotFill(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotRectangle(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotRemoveCoordinateAxes(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotGrid(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotLabel(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotSetId(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotViewRectangle(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotViewWindow(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlotPoint(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPlot2DWithBars(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerContains(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerExpressionLeafs(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLastElement(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerRemoveLastElement(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerLispify(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLispifyFull(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerChildExpression(Calculator& calculator, const Expression& input, Expression& output);


  static bool innerDegreesToRadians(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerEvaluateToDoublE(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionEvaluateToDouble(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerEmbedSemisimpleAlgebraInSemisimpleAlgebra(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWeylDimFormula(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDecomposeCharGenVerma(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDecomposeFDPartGeneralizedVermaModuleOverLeviPart(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerParabolicWeylGroups(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerParabolicWeylGroupsBruhatGraph(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerAllPartitions(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAllVectorPartitions(Calculator& calculator, const Expression& input, Expression& output);

  template<class Coefficient>
  static bool functionDeterminant(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    Expression::FunctionAddress conversionFunction,
    int maxiumDimension
  );

  static bool innerDeterminant(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerHighestWeightTransposeAntiAutomorphismBilinearForm(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCoefficientsPowersOf(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntervalOpenFromSequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntervalClosedFromSequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntervalLeftClosedFromSequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntervalRightClosedFromSequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsNonEmptySequence(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIsSquare(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsSquareFree(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsSquareFreePolynomial(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIsPower(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsEven(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIsAlgebraicRadical(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsReal(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsConstant(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGaussianEliminationMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSplitToPartialFractionsOverAlgebraicRealsAlgorithm(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerSplitToPartialFractionsOverAlgebraicRealS(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionSplitToPartialFractionsOverAlgebraicReals(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerTestTopCommand(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerTestIndicator(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerRootSAsAndSltwos(
    Calculator& calculator, const Expression& input, Expression& output, bool showSLtwos, bool MustRecompute
  );
  static bool innerPrintRootSAs(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctions::innerRootSAsAndSltwos(calculator, input, output, false, false);
  }
  static bool innerPrintRootSAsForceRecompute(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctions::innerRootSAsAndSltwos(calculator, input, output, false, true);
  }
  static bool innerPrintSltwos(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctions::innerRootSAsAndSltwos(calculator, input, output, true, false);
  }
  static bool innerFindProductDistanceModN(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSolveProductSumEquationOverSetModN(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerSolveSerreLikeSystem(
    Calculator& calculator, const Expression& input, Expression& output, bool useUpperLimit, bool startWithAlgebraicClosure
  );
  static bool innerSolveSerreLikeSystemNoUpperLimit(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerSolveSerreLikeSystem(calculator, input, output, false, false);
  }
  static bool innerSolveSerreLikeSystemUpperLimit(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerSolveSerreLikeSystem(calculator, input, output, true, false);
  }
  static bool innerSolveSerreLikeSystemAlgebraic(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerSolveSerreLikeSystem(calculator, input, output, false, true);
  }
  static bool innerSolveSerreLikeSystemAlgebraicUpperLimit(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerSolveSerreLikeSystem(calculator, input, output, true, true);
  }
  static bool innerFetchWebPagePOST(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerFetchWebPageGET(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerFetchKnownPublicKeys(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSendEmailWithMailGun(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerElementEllipticCurveNormalForm(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerValueOfModularExpression(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerFormatCPPSourceCode(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerformatCPPDirectory(
    Calculator& calculator, const Expression& input, Expression& output
  );

  static bool innerPrintZnEnumeration(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerNot(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerZmodP(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAttemptExtendingEtoHEFwithHinCartan(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerAdCommonEigenSpaces(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerEmbedG2inB3(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCasimir(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetLinksToSimpleLieAlgerbas(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCharacterSSLieAlgFD(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPrintSSSubalgebras(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool doForceRecompute,
    bool doAttemptToSolveSystems,
    bool docomputePairingTable,
    bool docomputeModuleDecompositionsition,
    bool doComputeNilradicals,
    bool doAdjustCentralizers
  );
  static bool innerPrintSSsubalgebrasNilradicals(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPrintSSsubalgebrasRecompute(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPrintSSsubalgebrasNoCentralizers(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPrintSSsubalgebrasNoSolutions(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPrintSSsubalgebrasRegular(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerGetChevGen(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetCartanGen(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPrintSSLieAlgebraShort(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctions::innerPrintSSLieAlgebra(calculator, input, output, false);
  }
  static bool innerPrintSSLieAlgebraVerbose(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctions::innerPrintSSLieAlgebra(calculator, input, output, true);
  }
  static bool innerWriteSSLieAlgebraToHD(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPrintSSLieAlgebra(Calculator& calculator, const Expression& input, Expression& output, bool Verbose);
  static bool innerWriteToHDOrPrintSSLieAlgebra(
    Calculator& calculator, const Expression& input, Expression& output, bool Verbose, bool writeToHD
  );
  static bool functionWriteToHDOrPrintSSLieAlgebra(
    Calculator& calculator, const Expression& input, Expression& output, bool Verbose, bool writeToHD
  );
  static bool innerRootSubsystem(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerConesIntersect(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPerturbSplittingNormal(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerPrintAllVectorPartitions(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPolynomialWithEWA(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctions::innerEWAorPoly(calculator, input, output, true);
  }
  static bool innerElementWeylAlgebra(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctions::innerEWAorPoly(calculator, input, output, false);
  }
  static bool innerEWAorPoly(Calculator& calculator, const Expression& input, Expression& output, bool assignPoly);
  static bool innerOperationBinary(Calculator& calculator, const Expression& input, Expression& output, int theOp);
  static bool innerInterpolatePoly(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerTimes(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctions::innerOperationBinary(calculator, input, output, calculator.opTimes());
  }
  static bool innerReverseOrder(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerReverseOrderRecursively(Calculator& calculator, const Expression& input, Expression& output);
  static bool transpose(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerFunctionToMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGenerateMultiplicativelyClosedSet(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerDeterminantPolynomial(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerInvertMatrixVerbose(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSuffixNotationForPostScript(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerFreudenthalFormula(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerFreudenthalFull(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerJacobiSymbol(Calculator& calculator, const Expression& input, Expression& output);
};
#endif

