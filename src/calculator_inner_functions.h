// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader6_2InnerFns
#define vpfHeader6_2InnerFns
#include "calculator_interface.h"

static ProjectInformationInstance projectInfoCalculatorInnerFunctionsHeader(__FILE__, "Header, calculator built-in functions. ");


class CalculatorFunctionsCrypto {
public:
  static bool innerAES_CBC_256_Encrypt(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAES_CBC_256_Decrypt(Calculator& theCommands, const Expression& input, Expression& output);
};

class CalculatorFunctions {
public:
  static bool LeftIntervalGreaterThanRight(const Expression& left, const Expression& right);
  static bool innerPrintRuleStack(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerStopServer(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCrash(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCrashByListOutOfBounds(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCrashByVectorOutOfBounds(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAutomatedTestProblemInterpretation(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrecomputeSemisimpleLieAlgebraStructure(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerTurnRulesOnOff(Calculator& theCommands, const Expression& input, Expression& output, bool turnOff);

  static bool innerTurnOffRules(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTurnOnRules(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerTurnOnApproximations(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTurnOffApproximations(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerEqualityToArithmeticExpressioN(Calculator& theCommands, const Expression& input, Expression& output);
  static bool functionEqualityToArithmeticExpression(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerRandomInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSelectAtRandom(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSetRandomSeed(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerConvertElementZmodPToInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerUrlStringToNormalString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerStringToURL(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerURLKeyValuePairsToNormalRecursive(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerStringToAtom(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerExpressionToString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerExpressionToUTF8String(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerQuoteToString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerBase64ToCharToBase64Test(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerConvertBase64ToString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerConvertStringToHex(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCharToBase64(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAppendDoubleSha256Check(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerConvertBase58ToHex(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerConvertHexToBase58(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerConvertIntegerUnsignedToBase58(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSliceString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerStringDifference(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerNISTEllipticCurveGenerator(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerNISTEllipticCurveOrder(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLoadKnownCertificates(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestLoadPEMCertificates(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestLoadPEMPrivateKey(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestTLSMessageSequence(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestTLSDecodeSSLRecord(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerJWTVerifyAgainstKnownKeys(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerJWTVerifyAgainstRSA256(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRSAEncrypt(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRSASign(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerShowKnownObjectIds(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSha224OfString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGenerateRandomPrime(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestRSASign(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSha256OfString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSha512(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRIPEMD160OfString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSha3_256OfString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerKeccak256OfString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSha256OfStringVerbose(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSha1OfString(Calculator& theCommands, const Expression& input, Expression& output);
  static bool functionHashString(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    const std::string& hashId,
    bool verbose
  );
  static bool innerTestJSON(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerBase64ToHex(Calculator& theCommands, const Expression& input, Expression& output );
  static bool innerIntegerToHex(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerHexToInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerHexToString(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerTestASN1Decode(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerX509CertificateServer(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerX509CertificateDecode(Calculator& theCommands, const Expression& input, Expression& output);


  static bool innerIntersection(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerUnion(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerUnionNoRepetition(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCrossProduct(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerDistributeExponent(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDistributeSqrt(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerChooseCartanSA(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerPolynomialDivisionSlidesGrLex(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPolynomialDivisionRemainder(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPolynomialDivisionQuotient(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPolynomialDivisionVerbose(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    List<MonomialP>::OrderLeftGreaterThanRight theMonOrder
  );
  static bool innerPolynomialDivisionVerboseGrLex(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPolynomialDivisionVerboseGrLexRev(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPolynomialDivisionVerboseLexRev(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPolynomialDivisionVerboseLex(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerGenerateVectorSpaceClosedWRTLieBracket(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFourierTransformEWA(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCasimirWRTlevi(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSqrt(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerFactorInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFactorOutNumberContent(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFactorial(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLogBaseNaturalToLn(Calculator& theCommands, const Expression& input, Expression& output);

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

  static bool innerIsProductLinearOrConstTermsIn(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsProductTermsUpToPower(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsLinearOrConstantIn(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCoefficientOf(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerConstTermRelative(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFloor(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRound(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLogarithmBaseNCeiling(Calculator& theCommands, const Expression& input, Expression& output);


  static bool innerArcsinAlgebraic(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerArccosAlgebraic(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerArcsin(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerArccos(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerArctan(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompositeArithmeticOperationEvaluatedOnArgument(
    Calculator& theCommands, const Expression& input, Expression& output
  );

  static bool innerMatchesPattern(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerThaw(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetFreeVariablesIncludeNamedConstants(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetFreeVariablesExcludeNamedConstants(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompareFunctionsNumerically(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompareExpressionsNumerically(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCompareExpressionsNumericallyAtPoints(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerPolynomialRelations(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerEnsureExpressionDependsOnlyOnMandatoryVariables(
    Calculator& theCommands, const Expression& input, Expression& output, bool excludeNamedConstants
  );
  static bool innerEnsureExpressionDependsOnlyOnMandatoryVariablesExcludeNamedConstants(
    Calculator& theCommands, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerEnsureExpressionDependsOnlyOnMandatoryVariables(theCommands, input, output, true);
  }
  static bool innerEnsureExpressionDependsOnlyOnMandatoryVariablesIncludeNamedConstants(
    Calculator& theCommands, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerEnsureExpressionDependsOnlyOnMandatoryVariables(theCommands, input, output, false);
  }
  static bool innerEnsureExpressionDependsOnlyOnStandard(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerMultiplySequence(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSumSequence(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSumTimesExpressionToSumOf(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSumAsOperatorToSumInternalNotation(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerHandleUnderscorePowerLimits(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerPolynomializE(Calculator& theCommands, const Expression& input, Expression& output);
  static bool functionPolynomialize(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerApplyToSubexpressionsRecurseThroughCalculusFunctions(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerSubList(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerApplyToList(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerNumerator(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDenominator(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerLength(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerDereferenceInterval(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDereferenceSequenceOrMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDereferenceSequenceStatements(Calculator& theCommands, const Expression& input, Expression& output);

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

  static bool innerOperatorBounds(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerAnyToZero(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerExponentToLog(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDiffdivDiffxToDifferentiation(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDdivDxToDiffDivDiffx(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestMathMouseHover(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerNewtonsMethod(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerBuildFreecalc(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerBuildFreecalcSingleSlides(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerBuildFreecalcSlidesOnTopic(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCrawlTexFile(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerCompareIntervalsNumerically(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntersectUnion(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntersectEmptySet(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerUnionEmptySet(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntersectIntervals(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerUnionUnionIntervals(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerUnionIntervals(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerNormalizeIntervals(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetFirstSummandContaining(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetSummand(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCollectMultiplicands(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCollectOpands(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCollectSummands(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerMergeConstantRadicals(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerCommuteConstants(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerDivideReplaceAdivBpowerItimesBpowerJ(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerAtimesBpowerJplusEtcDivBpowerI(Calculator& theCommands, const Expression& input, Expression& output);

  static bool outerCombineFractionsCommutative(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerIsPrimeMillerRabin(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsPossiblyPrime(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLCM(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGCD(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerIsNilpotent(Calculator& theCommands, const Expression& input, Expression& output);
  //static bool innerGramSchmidtVerbose(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerInvertMatrixRFsVerbose(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerInvertMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDFQsEulersMethod(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerConvertAlgebraicNumberToMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetAlgebraicNumberFromMinPoly(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerScaleToLeadingUnit(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRemoveDuplicates(Calculator& theCommands, const Expression& input, Expression& output);
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
  static bool innerEqualEqualEqual(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMax(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMin(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLessThanOrEqualTo(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGreaterThanOrEqualTo(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGreaterThan(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLessThan(Calculator& theCommands, const Expression& input, Expression& output);


  static bool outerCommuteAtimesBifUnivariate(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerCommuteAtimesBtimesCifUnivariate(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentialStandardHandler(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerIsDifferentialOneFormOneVariable(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerIntegrateRationalFunctionSplitToBuidingBlocks(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateRationalFunctionBuidingBlockIa(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateRationalFunctionBuidingBlockIb(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateRationalFunctionBuidingBlockIIaandIIIa(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerIntegrateRationalFunctionBuidingBlockIIb(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateRationalFunctionBuidingBlockIIIb(Calculator& theCommands, const Expression& input, Expression& output);

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

  static bool innerIntegrateDefiniteIntegral(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegratePowerByUncoveringParenthesisFirst(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateSum(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegratePullImaginaryUnit(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateXnDiffX(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegratePullConstant(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateSqrtOneMinusXsquared(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateSqrtXsquaredMinusOne(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateXpowerNePowerAx(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerIntegrateEpowerAxDiffX(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateSinPowerNCosPowerM(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerExploitCosEvenness(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerExploitSinOddness(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerTrigSumToTrigProduct(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSineOfAngleSumToTrig(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCosineOfAngleSumToTrig(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerConvertSinToExponent(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerConvertCosToExponent(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerImaginaryUnit(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerEulerFlaAsALaw(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegrateTanPowerNSecPowerM(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerInterpretAsDifferential(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntegralOperator(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDifferentiateSqrt(Calculator& theCommands, const Expression& input, Expression& output);
  static bool outerDifferentiateWRTxTimesAny(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerCanBeExtendedParabolicallyTo(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetSymmetricCartan(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTrace(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerReverseBytes(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMinPolyMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCharPolyMatrix(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerDrawRootSystem(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDrawWeightSupportWithMults(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDrawWeightSupport(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDrawExpressionGraphWithOptions(
    Calculator& theCommands, const Expression& input, Expression& output, bool useFullTree
  );
  static bool innerDrawExpressionGraph(Calculator& theCommands, const Expression& input, Expression& output) {
    return innerDrawExpressionGraphWithOptions(theCommands, input, output, false);
  }
  static bool innerDrawExpressionGraphFull(Calculator& theCommands, const Expression& input, Expression& output) {
    return innerDrawExpressionGraphWithOptions(theCommands, input, output, true);
  }

  static bool innerIfStandard(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIfFrozen(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerOr(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerOrIdentical(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAnd(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerSolveUnivariatePolynomialWithRadicalsWRT(
    Calculator& theCommands, const Expression& input, Expression& output
  );

  static bool innerMakeJavascriptExpressioN(Calculator& theCommands, const Expression& input, Expression& output);
  static bool functionMakeJavascriptExpression(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotCoordinateSystem(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotSetProjectionScreenBasis(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerPlotSurface(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotImplicitFunctionFull(Calculator& theCommands, const Expression& input, Expression& output, bool showGrid);
  static bool innerPlotImplicitFunction(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotImplicitShowGridFunction(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotSegment(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotMarkSegment(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotPath(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotParametricCurve(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotDirectionField(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotVectorField(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotDirectionOrVectorField(
    Calculator& theCommands, const Expression& input, Expression& output, bool vectorsAreNormalized
  );
  static bool innerPlotPolarRfunctionThetaExtended(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotPolarRfunctionTheta(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsPlot(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlot2D(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlot2DoverIntervals(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotFill(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotRectangle(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotRemoveCoordinateAxes(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotGrid(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotLabel(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotSetId(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotViewRectangle(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotViewWindow(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlotPoint(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPlot2DWithBars(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerContains(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerExpressionLeafs(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLastElement(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRemoveLastElement(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerLispify(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLispifyFull(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerChildExpression(Calculator& theCommands, const Expression& input, Expression& output);


  static bool innerDegreesToRadians(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerEvaluateToDoublE(Calculator& theCommands, const Expression& input, Expression& output);
  static bool functionEvaluateToDouble(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerEmbedSSalgInSSalg(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWeylDimFormula(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDecomposeCharGenVerma(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSplitFDpartB3overG2Init(
    Calculator& theCommands, const Expression& input, Expression& output, branchingData& theG2B3Data, Expression& outputContext
  );
  static bool innerDecomposeFDPartGeneralizedVermaModuleOverLeviPart(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerParabolicWeylGroups(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerParabolicWeylGroupsBruhatGraph(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerAllPartitions(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAllVectorPartitions(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDeterminant(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPrintGenVermaModule(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerWriteGenVermaModAsDiffOperatorUpToLevel(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerHWV(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSplitGenericGenVermaTensorFD(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerHWTAABF(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSplitFDpartB3overG2CharsOutput(
    Calculator& theCommands, const Expression& input, Expression& output, branchingData& theG2B3Data
  );
  static bool innerPrintB3G2branchingTableInit(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    branchingData& theG2B3data,
    int& desiredHeight,
    Expression& outputContext
  );
  static bool innerSplitFDpartB3overG2(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerCoefficientsPowersOf(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntervalOpenFromSequence(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntervalClosedFromSequence(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntervalLeftClosedFromSequence(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIntervalRightClosedFromSequence(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsNonEmptySequence(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerIsSquare(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsSquareFree(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsPower(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsEven(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerIsAlgebraicRadical(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsReal(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerIsConstant(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGaussianEliminationMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSplitToPartialFractionsOverAlgebraicRealsAlgorithm(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerSplitToPartialFractionsOverAlgebraicRealS(Calculator& theCommands, const Expression& input, Expression& output);
  static bool functionSplitToPartialFractionsOverAlgebraicReals(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestTopCommand(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestIndicator(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRootSAsAndSltwos(
    Calculator& theCommands, const Expression& input, Expression& output, bool showSLtwos, bool MustRecompute
  );
  static bool innerPrintRootSAs(Calculator& theCommands, const Expression& input, Expression& output) {
    return CalculatorFunctions::innerRootSAsAndSltwos(theCommands, input, output, false, false);
  }
  static bool innerPrintRootSAsForceRecompute(Calculator& theCommands, const Expression& input, Expression& output) {
    return CalculatorFunctions::innerRootSAsAndSltwos(theCommands, input, output, false, true);
  }
  static bool innerPrintSltwos(Calculator& theCommands, const Expression& input, Expression& output) {
    return CalculatorFunctions::innerRootSAsAndSltwos(theCommands, input, output, true, false);
  }
  static bool innerFindProductDistanceModN(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSolveProductSumEquationOverSetModN(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerSolveSerreLikeSystem(
    Calculator& theCommands, const Expression& input, Expression& output, bool useUpperLimit, bool startWithAlgebraicClosure
  );
  static bool innerSolveSerreLikeSystemNoUpperLimit(
    Calculator& theCommands, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerSolveSerreLikeSystem(theCommands, input, output, false, false);
  }
  static bool innerSolveSerreLikeSystemUpperLimit(
    Calculator& theCommands, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerSolveSerreLikeSystem(theCommands, input, output, true, false);
  }
  static bool innerSolveSerreLikeSystemAlgebraic(
    Calculator& theCommands, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerSolveSerreLikeSystem(theCommands, input, output, false, true);
  }
  static bool innerSolveSerreLikeSystemAlgebraicUpperLimit(
    Calculator& theCommands, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerSolveSerreLikeSystem(theCommands, input, output, true, true);
  }
  static bool innerFetchWebPagePOST(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFetchWebPageGET(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerFetchKnownPublicKeys(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSendEmailWithMailGun(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerElementEllipticCurveNormalForm(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerFormatCPPSourceCode(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerFormatCPPDirectory(
    Calculator& theCommands, const Expression& input, Expression& output
  );
};

#endif

